#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_BOOKS 50
#define MAX_USERS 50
#define MAX_BORROWED 20

typedef struct {
    int bookID;
    char title[50];
    int timesBorrowed;
} Book;

typedef struct {
    int userID;
    int borrowedCount;
    int borrowedBooks[MAX_BORROWED];
} User;

Book books[MAX_BOOKS];
User users[MAX_USERS];

int bookCount = 0;
int userCount = 0;
int userBookGraph[MAX_USERS][MAX_BOOKS] = {0};

int findBook(int id);
void loadBooks();
void loadUsers();
void updateFiles();
void borrowBook(int userID, int bookID);
void recommend(int userID);
void showTopPopularBooks();

int findBook(int id) {
    for (int i = 0; i < bookCount; i++) {
        if (books[i].bookID == id)
            return i;
    }
    return -1;
}

void loadBooks() {
    FILE *fp = fopen("books.txt", "r");
    if (!fp) {
        printf("Error: books.txt not found\n");
        return;
    }

    while (fscanf(fp, "%d %s %d",
                  &books[bookCount].bookID,
                  books[bookCount].title,
                  &books[bookCount].timesBorrowed) != EOF) {
        bookCount++;
    }
    fclose(fp);
}

void loadUsers() {
    FILE *fp = fopen("users.txt", "r");
    if (!fp) {
        printf("Error: users.txt not found\n");
        return;
    }

    while (fscanf(fp, "%d %d",
                  &users[userCount].userID,
                  &users[userCount].borrowedCount) != EOF) {

        for (int i = 0; i < users[userCount].borrowedCount; i++) {
            fscanf(fp, "%d", &users[userCount].borrowedBooks[i]);

            int bIndex = findBook(users[userCount].borrowedBooks[i]);
            if (bIndex != -1)
                userBookGraph[userCount][bIndex] = 1;
        }
        userCount++;
    }
    fclose(fp);
}

void updateFiles() {
    FILE *fp1 = fopen("books.txt", "w");
    FILE *fp2 = fopen("users.txt", "w");

    if (!fp1 || !fp2) {
        printf("Error updating files\n");
        return;
    }

    for (int i = 0; i < bookCount; i++) {
        fprintf(fp1, "%d %s %d\n",
                books[i].bookID,
                books[i].title,
                books[i].timesBorrowed);
    }

    for (int i = 0; i < userCount; i++) {
        fprintf(fp2, "%d %d ",
                users[i].userID,
                users[i].borrowedCount);

        for (int j = 0; j < users[i].borrowedCount; j++) {
            fprintf(fp2, "%d ", users[i].borrowedBooks[j]);
        }
        fprintf(fp2, "\n");
    }

    fclose(fp1);
    fclose(fp2);
}

void borrowBook(int userID, int bookID) {
    int u = -1;
    for (int i = 0; i < userCount; i++) {
        if (users[i].userID == userID)
            u = i;
    }

    int b = findBook(bookID);

    if (u == -1 || b == -1) {
        printf("Invalid user or book ID\n");
        return;
    }

    if (users[u].borrowedCount >= MAX_BORROWED) {
        printf("Borrow limit reached\n");
        return;
    }

    if (userBookGraph[u][b]) {
        printf("Book already borrowed by this user\n");
        return;
    }

    users[u].borrowedBooks[users[u].borrowedCount++] = bookID;
    books[b].timesBorrowed++;
    userBookGraph[u][b] = 1;

    updateFiles();
    printf("Book \"%s\" borrowed successfully\n", books[b].title);
}

void recommend(int userID) {
    int u = -1;
    for (int i = 0; i < userCount; i++) {
        if (users[i].userID == userID)
            u = i;
    }

    if (u == -1) {
        printf("Invalid user\n");
        return;
    }

    int score[MAX_BOOKS] = {0};

    for (int i = 0; i < userCount; i++) {
        if (i == u)
            continue;

        int similarity = 0;
        for (int j = 0; j < bookCount; j++) {
            if (userBookGraph[u][j] && userBookGraph[i][j])
                similarity++;
        }

        if (similarity > 0) {
            for (int j = 0; j < bookCount; j++) {
                if (userBookGraph[i][j] && !userBookGraph[u][j])
                    score[j] += similarity;
            }
        }
    }

    printf("Recommended Books:\n");
    int found = 0;
    for (int i = 0; i < bookCount; i++) {
        if (score[i] > 0) {
            printf("- %s\n", books[i].title);
            found = 1;
        }
    }

    if (!found)
        printf("No recommendations available\n");
}

void showTopPopularBooks() {
    Book temp[MAX_BOOKS];
    memcpy(temp, books, sizeof(Book) * bookCount);

    for (int i = 0; i < bookCount - 1; i++) {
        for (int j = i + 1; j < bookCount; j++) {
            if (temp[j].timesBorrowed > temp[i].timesBorrowed) {
                Book t = temp[i];
                temp[i] = temp[j];
                temp[j] = t;
            }
        }
    }

    printf("Top 3 Popular Books:\n");
    for (int i = 0; i < 3 && i < bookCount; i++) {
        printf("%d. %s (%d times)\n",
               i + 1,
               temp[i].title,
               temp[i].timesBorrowed);
    }
}

int main() {
    loadBooks();
    loadUsers();

    int choice, uid, bid;

    while (1) {
        printf("\n1. Borrow Book\n2. Recommend Books\n3. View Popular Books\n4. Exit\nEnter choice: ");
        scanf("%d", &choice);

        if (choice == 1) {
            printf("Enter User ID: ");
            scanf("%d", &uid);
            printf("Enter Book ID: ");
            scanf("%d", &bid);
            borrowBook(uid, bid);
        }
        else if (choice == 2) {
            printf("Enter User ID: ");
            scanf("%d", &uid);
            recommend(uid);
        }
        else if (choice == 3) {
            showTopPopularBooks();
        }
        else {
            break;
        }
    }
    return 0;
}
