# Web-Browser-Tab-Management-System (DSA)

This project implements a web browser history management architecture inspired by modern browser systems. It handles tabs, web pages, navigation history, and various operations based on real browser functionalities.

Technologies: C Programming, Circular Doubly Linked Lists, Stack Data Structures, Dynamic Memory Management, Pointer Manipulation

## Main Features:

- Opening and closing tabs (new_tab, close, open_id)

- Navigating between tabs (next, prev)

- Loading pages into tabs (page_id)

- Forward/backward navigation between web pages

- Displaying open tabs (print)

- Showing navigation history per tab (print_history)

## Data Structures Used:

- page: Represents a web page (ID, URL, description)

- stack: Implementation for tab navigation stacks

- tab: Stores current page and navigation stacks

- tablist: Circular doubly linked list of tabs

- browser: Main structure managing tab list and current tab

## Key Functions:

- new_tab: Creates a new tab

- close: Closes current tab and activates left neighbor

- open_id: Activates tab with specified ID

- next/prev: Navigates between tabs

- page_id: Loads specified page into current tab

- backward/forward: Navigates through page history

- print: Displays open tabs and current page

- print_history: Shows navigation history for specified tab

## Implementation:
The program reads available pages and operations from input files, manages browser state using circular doubly linked lists and stacks, and outputs results while handling error cases. Dynamic memory is carefully managed to prevent leaks.

Technologies: C Programming, Circular Doubly Linked Lists, Stack Data Structures, Dynamic Memory Management, File I/O Operations, Pointer Manipulation
