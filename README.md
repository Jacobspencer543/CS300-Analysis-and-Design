# CS300 DSA-Analysis-and-Design

## What was the problem you were solving in the projects for this course?

The projects were built around a course advising program for ABCU. Advisors needed a way to load course info from a CSV file, print all the Computer Science courses in alphanumeric order, and look up a single course to see its title and prerequisites. Project One was figuring out which data structure to use, and Project Two was actually building it in C++.

## How did you approach the problem? Consider why data structures are important to understand.

I started by writing pseudocode for a vector, a hash table, and a binary search tree, then did a runtime analysis on each one. The vector was easy to work with but slow to search. The hash table was fast for lookups, but it would need an extra sort every time I printed the list. The BST ended up being the best fit since it keeps courses in order as they're added, so printing the sorted list is just an in-order traversal, and searching stays quick. That comparison is really where I saw why data structures matter. The same program can run well or badly depending on how you store the data, and it's a lot easier to make that call before writing any code than after.

## How did you overcome any roadblocks you encountered while going through the activities or project?

The trickiest part was making sure the file data was actually valid. A prerequisite could point to a course that wasn't in the file, so I set up the load to read through the file twice. The first pass collects every course number, and the second pass checks each prerequisite against that list before anything goes into the tree. I also tested it with a broken CSV on purpose to make sure the error handling worked. Smaller stuff came up too, like course lookups being case-sensitive and reloading the file creating duplicates, so I fixed those as I went. Having the pseudocode and the BST from the earlier assignments to build off of helped a lot.

## How has your work on this project expanded your approach to designing software and developing programs?

I think more about how the data is going to be used before I start coding now. Stuff like how often it gets searched or whether it needs to stay sorted points pretty quickly to the right structure. Doing the pseudocode and runtime analysis first felt like extra work at the start, but it made Project Two go way smoother since the big decisions were already made.

## How has your work on this project evolved the way you write programs that are maintainable, readable, and adaptable?

I break things into smaller functions that each handle one job, like validating the file, loading courses, and printing a course. That makes the code easier to read and easier to fix when something breaks. I also kept the BST separate from the menu and file handling, so if the program ever needed a different data structure, it could be swapped in without rewriting everything. Clear names and simple comments go a long way too, especially when I come back to the code later.
