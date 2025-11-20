#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_BOOKS 50
#define MAX_USERS 50
#define MAX_RECOMM 10
#define MAX_POPULAR 10

//  Structures 
typedef struct {
    int bookID;
    char title[50];
    int timesBorrowed;
} Book;

typedef struct {
    int userID;
    int borrowedCount;
    int borrowedBooks[20];
} User;

typedef struct PopularQueue {
    int arr[MAX_POPULAR];
    int front, rear, size;
} PopularQueue;

//  Globals 
Book books[MAX_BOOKS];
User users[MAX_USERS];
int bookCount = 0, userCount = 0;
int userBookGraph[MAX_USERS][MAX_BOOKS];

//  Queue Functions 
void initQueue(PopularQueue *q) {
    q->front = q->rear = -1;
    q->size = 0;
}

void enqueue(PopularQueue *q, int id) {
    if (q->size == MAX_POPULAR) return;
    if (q->front == -1) q->front = 0;
    q->rear = (q->rear + 1) % MAX_POPULAR;
    q->arr[q->rear] = id;
    q->size++;
}

void displayQueue(PopularQueue *q) {
    if (q->size == 0) { printf("No popular books.\n"); return; }
    int i = q->front;
    printf("Popular Books: ");
    for (int c = 0; c < q->size; c++) {
        printf("%d ", q->arr[i]);
        i = (i + 1) % MAX_POPULAR;
    }
    printf("\n");
}

//  File Loading 
void loadBooks() {
    FILE *fp = fopen("books.txt", "r");
    if (!fp) return;
    while (fscanf(fp, "%d %s %d", &books[bookCount].bookID,
                  books[bookCount].title,
                  &books[bookCount].timesBorrowed) != EOF) {
        bookCount++;
    }
    fclose(fp);
}

void loadUsers() {
    FILE *fp = fopen("users.txt", "r");
    if (!fp) return;
    while (fscanf(fp, "%d %d", &users[userCount].userID,
                  &users[userCount].borrowedCount) != EOF) {
        for (int i = 0; i < users[userCount].borrowedCount; i++) {
            fscanf(fp, "%d", &users[userCount].borrowedBooks[i]);
        }
        userCount++;
    }
    fclose(fp);
}

//  File Updating 
void updateFiles() {
    FILE *fp1 = fopen("books.txt", "w");
    for (int i = 0; i < bookCount; i++) {
        fprintf(fp1, "%d %s %d\n", books[i].bookID, books[i].title, books[i].timesBorrowed);
    }
    fclose(fp1);

    FILE *fp2 = fopen("users.txt", "w");
    for (int i = 0; i < userCount; i++) {
        fprintf(fp2, "%d %d ", users[i].userID, users[i].borrowedCount);
        for (int j = 0; j < users[i].borrowedCount; j++) {
            fprintf(fp2, "%d ", users[i].borrowedBooks[j]);
        }
        fprintf(fp2, "\n");
    }
    fclose(fp2);
}

//  Hash Search for Book 
int findBook(int id) {
    for (int i = 0; i < bookCount; i++) {
        if (books[i].bookID == id) return i;
    }
    return -1;
}

//  Borrow Function 
void borrowBook(int userID, int bookID) {
    int u = -1;
    for (int i = 0; i < userCount; i++)
        if (users[i].userID == userID) u = i;

    int b = findBook(bookID);
    if (u == -1 || b == -1) {
        printf("Invalid User/Book.\n");
        return;
    }

    users[u].borrowedBooks[users[u].borrowedCount++] = bookID;
    books[b].timesBorrowed++;
    userBookGraph[u][b] = 1;

    updateFiles();
    printf("Book %d borrowed successfully.\n", bookID);
}

//  Recommendation 
void recommend(int userID) {
    int u = -1;
    for (int i = 0; i < userCount; i++)
        if (users[i].userID == userID) u = i;

    if (u == -1) { printf("Invalid user.\n"); return; }

    int score[MAX_BOOKS] = {0};

    for (int i = 0; i < userCount; i++) {
        if (i == u) continue;

        int similarity = 0;
        for (int j = 0; j < bookCount; j++)
            if (userBookGraph[u][j] && userBookGraph[i][j])
                similarity++;

        if (similarity > 0) {
            for (int j = 0; j < bookCount; j++)
                if (userBookGraph[i][j] && !userBookGraph[u][j])
                    score[j] += similarity;
        }
    }

    printf("\nRecommended Books: ");
    int found = 0;
    for (int i = 0; i < bookCount; i++) {
        if (score[i] > 0) {
            printf("%s ", books[i].title);
            found = 1;
        }
    }
    if (!found) printf("No recommendations.");
    printf("\n");
}

//  Main 
int main() {
    PopularQueue pq;
    initQueue(&pq);

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
            enqueue(&pq, bid);
        }
        else if (choice == 2) {
            printf("Enter User ID: ");
            scanf("%d", &uid);
            recommend(uid);
        }
        else if (choice == 3) {
            displayQueue(&pq);
        }
        else break;
    }
    return 0;
}
