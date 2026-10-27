# C Library Manager

A command-line library management program written in C. The application allows users to manage a collection of books and save library information to a file.

## Features

- **Load library data** from a file
- **Display books** stored in the library
- **Add books** to the collection
- **Remove books** from the collection
- **Sort books** by title or author
- **Save library data** to a file
- **Validate input and file formats**
- **Manage memory** dynamically and free allocated resources

## Book Information

Each book stores the following information:
- Title
- Author
- Page count
- Publication year

## Technologies

- C
- Standard C library
- File handling
- Dynamic memory allocation

## Project Structure

\`\`\`text
c-library-manager/
├── a4.c
└── README.md
\`\`\`

## How to Run

Compile the program with a C compiler:

\`\`\`bash
gcc -Wall -Wextra -std=c11 a4.c -o library-manager
\`\`\`

Run the program with the required library data file:

\`\`\`bash
./library-manager <filename>
\`\`\`

Replace `<filename>` with the path to your library data file.

## Purpose

This project demonstrates C programming skills, including structures, file handling, input validation, dynamic memory management, and sorting algorithms.
