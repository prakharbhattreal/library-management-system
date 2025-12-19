# 📚 Library Management & Recommendation System (C Project)

This project is a **file-based Library Management System written in C**.  

The system allows users to borrow books, get book recommendations based on similar users, and view the most popular books.

---

## 📂 Files Used in the Project

### 1️⃣ books.txt
Stores all book-related data permanently.

**File Format**
```
bookID title timesBorrowed
```

**Example**
```
101 CProgramming 5
102 PythonBasics 3
103 DataStructures 8
```

- `bookID` → Unique identifier for each book  
- `title` → Name of the book (single word)  
- `timesBorrowed` → Total number of times the book has been borrowed  

---

### 2️⃣ users.txt
Stores user data and borrowing history.

**File Format**
```
userID borrowedCount bookID1 bookID2 ...
```

**Example**
```
123 2 9 10
124 3 11 12 14 
```

- `userID` → Unique user identifier  
- `borrowedCount` → Number of books borrowed by the user  
- `bookID` → IDs of borrowed books  

---

## 🧱 Core Data Structures

### Book Structure
```c
typedef struct {
    int bookID;
    char title[50];
    int timesBorrowed;
} Book;
```

Stores book information loaded from `books.txt`.

---

### User Structure
```c
typedef struct {
    int userID;
    int borrowedCount;
    int borrowedBooks[MAX_BORROWED];
} User;
```

Stores user details and borrowing history loaded from `users.txt`.

---

### User–Book Graph
```c
int userBookGraph[MAX_USERS][MAX_BOOKS];
```

- Represents which user has borrowed which book  
- `1` → Book borrowed  
- `0` → Not borrowed  
- Used for recommendation logic  

---

## 📁 File Handling Functions

### 🔹 loadBooks()
- Opens `books.txt` in read mode
- Reads book data using `fscanf`
- Stores books in the `books[]` array
- Updates `bookCount`

Purpose: **Loads all book data into memory**

---

### 🔹 loadUsers()
- Opens `users.txt` in read mode
- Reads user data and borrowed books
- Updates `users[]` array
- Builds the `userBookGraph`

Purpose: **Loads users and establishes user–book relationships**

---

### 🔹 updateFiles()
- Opens `books.txt` and `users.txt` in write mode
- Writes updated book and user data using `fprintf`
- Ensures data persistence after borrowing

Purpose: **Saves updated data back to files**

---

## 📘 Functional Modules

### 🔹 borrowBook(userID, bookID)
- Validates user and book
- Prevents duplicate borrowing
- Updates:
  - User borrowing list
  - Book popularity count
  - User–Book graph
- Calls `updateFiles()` to save changes

Uses file handling indirectly through **persistent updates**

---

### 🔹 recommend(userID)
- Compares borrowing history with other users
- Calculates similarity based on common books
- Suggests books borrowed by similar users
- Does not modify files (read-only logic)

Uses the **graph-based recommendation approach**

---

### 🔹 showTopPopularBooks()
- Sorts books based on `timesBorrowed`
- Displays top 3 most popular books

Uses in-memory data loaded from files

---

## ▶️ How to Run

1. Ensure `books.txt` and `users.txt` exist in the same directory
2. Compile the program:
   ```bash
   gcc main.c -o main
   ```
3. Run the program:
   ```bash
   ./main
   ```

---

## 📌 Sample Menu
```
1. Borrow Book
2. Recommend Books
3. View Popular Books
4. Exit
```
