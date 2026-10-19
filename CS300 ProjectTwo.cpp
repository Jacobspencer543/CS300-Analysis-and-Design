//============================================================================
// Name        : ProjectTwo.cpp
// Author      : Jacob Spencer
// Course      : CS 300 Analysis and Design
// Description : ABCU advising assistance program. Loads course data from a
//               CSV file into a binary search tree, prints an alphanumeric
//               course list, and prints a single course with its
//               prerequisites.
//============================================================================

#include <algorithm>
#include <cctype>
#include <fstream>
#include <iostream>
#include <string>
#include <vector>

using namespace std;

//============================================================================
// Course structure
//============================================================================

// One course from the input file
struct Course {
    string courseNumber;           // example: CSCI200
    string courseTitle;            // example: Data Structures
    vector<string> prerequisites;  // zero or more course numbers
};

//============================================================================
// String helpers
//============================================================================

// Remove spaces, tabs, and carriage returns from both ends of a string
string trim(const string& text) {
    const string whitespace = " \t\r\n";
    size_t start = text.find_first_not_of(whitespace);
    if (start == string::npos) {
        return "";
    }
    size_t end = text.find_last_not_of(whitespace);
    return text.substr(start, end - start + 1);
}

// Return an uppercase copy so lookups are not case sensitive
string toUpper(string text) {
    for (char& ch : text) {
        ch = static_cast<char>(toupper(static_cast<unsigned char>(ch)));
    }
    return text;
}

// Split one line of the file on commas and trim each token
vector<string> splitLine(const string& line) {
    vector<string> tokens;
    string current;
    for (char ch : line) {
        if (ch == ',') {
            tokens.push_back(trim(current));
            current.clear();
        }
        else {
            current += ch;
        }
    }
    tokens.push_back(trim(current));  // last token has no comma after it
    return tokens;
}

//============================================================================
// Binary search tree
//============================================================================

// Tree node holding one course and links to its children
struct Node {
    Course course;
    Node* left;
    Node* right;

    Node(const Course& aCourse) {
        course = aCourse;
        left = nullptr;
        right = nullptr;
    }
};

// Stores courses ordered by course number, lowest on the left
class BinarySearchTree {

private:
    Node* root;
    unsigned int nodeCount;

    void addNode(Node* node, const Course& course);
    void inOrder(Node* node) const;
    void destroyTree(Node* node);

public:
    BinarySearchTree();
    ~BinarySearchTree();
    void Insert(const Course& course);
    Course Search(const string& courseNumber) const;
    bool Contains(const string& courseNumber) const;
    void PrintCourseList() const;
    void Clear();
    unsigned int Size() const;
};

// Start with an empty tree
BinarySearchTree::BinarySearchTree() {
    root = nullptr;
    nodeCount = 0;
}

// Free every node when the tree goes out of scope
BinarySearchTree::~BinarySearchTree() {
    destroyTree(root);
}

// Delete children before the parent (post order)
void BinarySearchTree::destroyTree(Node* node) {
    if (node != nullptr) {
        destroyTree(node->left);
        destroyTree(node->right);
        delete node;
    }
}

// Empty the tree so the file can be loaded again without duplicates
void BinarySearchTree::Clear() {
    destroyTree(root);
    root = nullptr;
    nodeCount = 0;
}

// Add a course, starting at the root
void BinarySearchTree::Insert(const Course& course) {
    if (root == nullptr) {
        root = new Node(course);
        nodeCount++;
    }
    else {
        addNode(root, course);
    }
}

// Walk down the tree to find the right spot for a course
void BinarySearchTree::addNode(Node* node, const Course& course) {
    if (course.courseNumber == node->course.courseNumber) {
        node->course = course;  // replace a duplicate line
    }
    else if (course.courseNumber < node->course.courseNumber) {
        if (node->left == nullptr) {
            node->left = new Node(course);
            nodeCount++;
        }
        else {
            addNode(node->left, course);
        }
    }
    else {
        if (node->right == nullptr) {
            node->right = new Node(course);
            nodeCount++;
        }
        else {
            addNode(node->right, course);
        }
    }
}

// Find a course by number, returns an empty Course if not found
Course BinarySearchTree::Search(const string& courseNumber) const {
    string searchKey = toUpper(trim(courseNumber));
    Node* current = root;
    while (current != nullptr) {
        if (current->course.courseNumber == searchKey) {
            return current->course;
        }
        if (searchKey < current->course.courseNumber) {
            current = current->left;
        }
        else {
            current = current->right;
        }
    }
    return Course();
}

// True if the course number is in the tree
bool BinarySearchTree::Contains(const string& courseNumber) const {
    return !Search(courseNumber).courseNumber.empty();
}

// In-order traversal prints courses lowest to highest with no extra sort
void BinarySearchTree::PrintCourseList() const {
    cout << "Here is a sample schedule:" << endl << endl;
    inOrder(root);
    cout << endl;
}

void BinarySearchTree::inOrder(Node* node) const {
    if (node != nullptr) {
        inOrder(node->left);
        cout << node->course.courseNumber << ", " << node->course.courseTitle << endl;
        inOrder(node->right);
    }
}

unsigned int BinarySearchTree::Size() const {
    return nodeCount;
}

//============================================================================
// File loading
//============================================================================

// Check the file format before loading anything.
// Pass 1: every line needs a course number and a title.
// Pass 2: every prerequisite must also have its own line in the file.
bool validateFile(const string& filePath) {
    ifstream inputFile(filePath);
    if (!inputFile.is_open()) {
        cout << "Error: the file " << filePath << " could not be opened." << endl;
        return false;
    }

    BinarySearchTree numberStore;  // course numbers seen in pass 1
    string line;
    int lineNumber = 0;

    // Pass 1
    while (getline(inputFile, line)) {
        lineNumber++;
        if (trim(line).empty()) {
            continue;
        }
        vector<string> tokens = splitLine(line);
        if (tokens.size() < 2 || tokens[0].empty() || tokens[1].empty()) {
            cout << "Format error on line " << lineNumber
                 << ": fewer than two parameters -> " << trim(line) << endl;
            return false;
        }
        Course marker;
        marker.courseNumber = toUpper(tokens[0]);
        marker.courseTitle = tokens[1];
        numberStore.Insert(marker);
    }

    // Pass 2: go back to the start of the file
    inputFile.clear();
    inputFile.seekg(0);
    lineNumber = 0;
    while (getline(inputFile, line)) {
        lineNumber++;
        if (trim(line).empty()) {
            continue;
        }
        vector<string> tokens = splitLine(line);
        for (size_t i = 2; i < tokens.size(); ++i) {
            if (!tokens[i].empty() && !numberStore.Contains(tokens[i])) {
                cout << "Format error on line " << lineNumber << ": prerequisite "
                     << tokens[i] << " for course " << tokens[0]
                     << " has no line in the file." << endl;
                return false;
            }
        }
    }
    return true;
}

// Read the file into Course objects and store them in the tree
bool loadCourses(const string& filePath, BinarySearchTree& courses) {
    if (!validateFile(filePath)) {
        cout << "The file was not loaded because of the error above." << endl;
        return false;
    }

    ifstream inputFile(filePath);
    if (!inputFile.is_open()) {
        cout << "Error: the file " << filePath << " could not be opened." << endl;
        return false;
    }

    courses.Clear();  // start fresh if the file was loaded before
    string line;
    while (getline(inputFile, line)) {
        if (trim(line).empty()) {
            continue;
        }
        vector<string> tokens = splitLine(line);

        Course course;
        course.courseNumber = toUpper(tokens[0]);
        course.courseTitle = tokens[1];
        for (size_t i = 2; i < tokens.size(); ++i) {
            if (!tokens[i].empty()) {
                course.prerequisites.push_back(toUpper(tokens[i]));
            }
        }
        courses.Insert(course);
    }

    cout << courses.Size() << " courses were loaded." << endl;
    return true;
}

//============================================================================
// Course information
//============================================================================

// Print one course's title and each prerequisite's number and title
void printCourse(const BinarySearchTree& courses, const string& courseNumber) {
    Course course = courses.Search(courseNumber);
    if (course.courseNumber.empty()) {
        cout << "Course " << toUpper(courseNumber) << " was not found." << endl;
        return;
    }

    cout << course.courseNumber << ", " << course.courseTitle << endl;
    if (course.prerequisites.empty()) {
        cout << "Prerequisites: None" << endl;
        return;
    }

    cout << "Prerequisites:" << endl;
    for (const string& prereq : course.prerequisites) {
        Course prereqCourse = courses.Search(prereq);
        cout << "    " << prereq << ", " << prereqCourse.courseTitle << endl;
    }
}

//============================================================================
// Menu
//============================================================================

void printMenu() {
    cout << endl;
    cout << "  1. Load Data Structure." << endl;
    cout << "  2. Print Course List." << endl;
    cout << "  3. Print Course." << endl;
    cout << "  9. Exit" << endl;
    cout << endl;
    cout << "What would you like to do? ";
}

// Turn the menu input into a number, returns -1 if it is not a whole number
int parseChoice(const string& input) {
    string cleaned = trim(input);
    if (cleaned.empty() || cleaned.size() > 3) {
        return -1;
    }
    for (char ch : cleaned) {
        if (!isdigit(static_cast<unsigned char>(ch))) {
            return -1;
        }
    }
    return stoi(cleaned);
}

int main() {
    BinarySearchTree courses;
    bool loaded = false;
    int choice = 0;
    string input;

    cout << "Welcome to the course planner." << endl;

    while (choice != 9) {
        printMenu();

        // Read the whole line so letters or extra text cannot break cin
        if (!getline(cin, input)) {
            break;  // end of input
        }
        choice = parseChoice(input);

        switch (choice) {
        case 1: {
            cout << "Enter the name of the course data file: ";
            string filePath;
            getline(cin, filePath);
            filePath = trim(filePath);
            // Allow a path pasted with quotes around it
            if (filePath.size() >= 2 && filePath.front() == '"' && filePath.back() == '"') {
                filePath = filePath.substr(1, filePath.size() - 2);
            }
            if (filePath.empty()) {
                cout << "No file name was entered." << endl;
                break;
            }
            // Keep earlier data if a reload fails
            if (loadCourses(filePath, courses)) {
                loaded = true;
            }
            break;
        }
        case 2:
            if (!loaded) {
                cout << "Please load the data structure first (option 1)." << endl;
            }
            else {
                courses.PrintCourseList();
            }
            break;
        case 3:
            if (!loaded) {
                cout << "Please load the data structure first (option 1)." << endl;
            }
            else {
                cout << "What course do you want to know about? ";
                string courseNumber;
                getline(cin, courseNumber);
                courseNumber = trim(courseNumber);
                if (courseNumber.empty()) {
                    cout << "No course number was entered." << endl;
                }
                else {
                    printCourse(courses, courseNumber);
                }
            }
            break;
        case 9:
            cout << "Thank you for using the course planner!" << endl;
            break;
        default:
            if (trim(input).empty()) {
                cout << "Please enter 1, 2, 3, or 9." << endl;
            }
            else {
                cout << trim(input) << " is not a valid option." << endl;
            }
            break;
        }
    }

    return 0;
}
