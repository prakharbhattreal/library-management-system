#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_USERS 10
#define MAX_BOOKS_QUEUE 50
#define HASH_TABLE_SIZE 31
#define BOOK_NAME_LEN 64

typedef struct BookNode {
    int bookId;
    char name[BOOK_NAME_LEN];
    struct BookNode* next;
} BookNode;

BookNode* hashTable[HASH_TABLE_SIZE];

int hashFunc(int id) {
    return id % HASH_TABLE_SIZE;
}

void insertBook(int id, const char* name) {
    int idx = hashFunc(id);
    BookNode* node = (BookNode*)malloc(sizeof(BookNode));
    node->bookId = id;
    strncpy(node->name, name, BOOK_NAME_LEN-1);
    node->name[BOOK_NAME_LEN-1] = '\0';
    node->next = hashTable[idx];
    hashTable[idx] = node;
}

const char* getBookName(int id) {
    int idx = hashFunc(id);
    BookNode* cur = hashTable[idx];
    while (cur) {
        if (cur->bookId == id) return cur->name;
        cur = cur->next;
    }
    return "Unknown";
}

typedef struct AdjNode {
    int bookId;
    struct AdjNode* next;
} AdjNode;

AdjNode* userGraph[MAX_USERS];

void addBorrow(int userId, int bookId) {
    if (userId < 0 || userId >= MAX_USERS) return;
    AdjNode* node = (AdjNode*)malloc(sizeof(AdjNode));
    node->bookId = bookId;
    node->next = userGraph[userId];
    userGraph[userId] = node;
}

int userHasBorrowed(int userId, int bookId) {
    AdjNode* cur = userGraph[userId];
    while (cur) {
        if (cur->bookId == bookId) return 1;
        cur = cur->next;
    }
    return 0;
}

typedef struct {
    int arr[MAX_BOOKS_QUEUE];
    int front, rear, count;
} Queue;

void initQueue(Queue* q) { q->front = 0; q->rear = -1; q->count = 0; }

void enqueue(Queue* q, int bookId) {
    if (q->count == MAX_BOOKS_QUEUE) return; // full
    q->rear = (q->rear + 1) % MAX_BOOKS_QUEUE;
    q->arr[q->rear] = bookId;
    q->count++;
}

int dequeue(Queue* q) {
    if (q->count == 0) return -1;
    int val = q->arr[q->front];
    q->front = (q->front + 1) % MAX_BOOKS_QUEUE;
    q->count--;
    return val;
}

void recommend(int userId) {
    if (userId < 0 || userId >= MAX_USERS) return;

    int recommendedIds[1000];   
    int recCount = 0;

    int seenBook[10000]; 
    memset(seenBook, 0, sizeof(seenBook));

    // mark user's own books
    AdjNode* cur = userGraph[userId];
    while (cur) {
        if (cur->bookId >=0 && cur->bookId < 10000) seenBook[cur->bookId] = 1;
        cur = cur->next;
    }

    // for every book user borrowed, find other users who borrowed same book
    cur = userGraph[userId];
    while (cur) {
        int bookId = cur->bookId;

        for (int u = 0; u < MAX_USERS; ++u) {
            if (u == userId) continue;
            if (userHasBorrowed(u, bookId)) {
                AdjNode* cur2 = userGraph[u];
                while (cur2) {
                    int candidate = cur2->bookId;
                    if (candidate >=0 && candidate < 10000 && !seenBook[candidate]) {
                        // check duplicate in recommendedIds
                        int dup = 0;
                        for (int k = 0; k < recCount; ++k) if (recommendedIds[k] == candidate) { dup = 1; break; }
                        if (!dup) {
                            recommendedIds[recCount++] = candidate;
                        }
                    }
                    cur2 = cur2->next;
                }
            }
        }
        cur = cur->next;
    }

    // Print recommendations
    printf("\nRecommendations for User %d:\n", userId);
    if (recCount == 0) {
        printf("  (No recommendations found.)\n");
    } else {
        for (int i = 0; i < recCount; ++i) {
            printf("  - [%d] %s\n", recommendedIds[i], getBookName(recommendedIds[i]));
        }
    }
}

void printUserBorrows(int userId) {
    printf("User %d borrowed: ", userId);
    AdjNode* cur = userGraph[userId];
    if (!cur) { printf("None\n"); return; }
    while (cur) {
        printf("[%d]%s", cur->bookId, (cur->next ? ", " : ""));
        cur = cur->next;
    }
    printf("\n");
}

int main() {
    // initialize
    for (int i = 0; i < HASH_TABLE_SIZE; ++i) hashTable[i] = NULL;
    for (int i = 0; i < MAX_USERS; ++i) userGraph[i] = NULL;

    Queue popularQ;
    initQueue(&popularQ);

    // Insert books into hash table
    insertBook(101, "C Programming");
    insertBook(102, "Data Structures");
    insertBook(103, "Algorithms");
    insertBook(104, "Operating Systems");
    insertBook(105, "Machine Learning");
    insertBook(106, "Database Systems");
    insertBook(107, "Computer Networks");

    // borrows (user, book)
    addBorrow(0, 101); addBorrow(0, 102);          
    addBorrow(1, 101); addBorrow(1, 103); addBorrow(1, 106); 
    addBorrow(2, 104); addBorrow(2, 101);          
    addBorrow(3, 105); addBorrow(3, 102);           
    addBorrow(4, 107); addBorrow(4, 106);           

    // Track popular borrows 
    enqueue(&popularQ, 101);
    enqueue(&popularQ, 102);
    enqueue(&popularQ, 101);
    enqueue(&popularQ, 103);
    enqueue(&popularQ, 106);

    // Print user borrows
    for (int u = 0; u <= 4; ++u) {
        printUserBorrows(u);
    }

    // Generate recommendations for users 0 1 2 3
    recommend(0); 
    recommend(1);
    recommend(3); 

    // Print popular queue content
    printf("\nPopular books (queue order):\n");
    while (popularQ.count > 0) {
        int b = dequeue(&popularQ);
        printf("  - [%d] %s\n", b, getBookName(b));
    }

    return 0;
}
