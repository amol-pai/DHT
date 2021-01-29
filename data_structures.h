#ifndef __DATA_STRUCTURES_H_
#define __DATA_STRUCTURES_H_

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <assert.h>

#define SIZE 3
#define MAX_VAL (1<<SIZE)

/*
    Data structure for an entry in the finger table. Each entry contains the identifier and the pointer to the node.
*/
struct finger_table_struct {
    int identifier;
    struct Node* next;
};

typedef struct finger_table_struct finger_table_struct;

/*
    Data structure for key-value pair.
*/
struct KV_Pair {
    int key;
    char* value;
    struct KV_Pair *next;
};

typedef struct KV_Pair KV_Pair;

/*
    Data structure for the storage of all the key-value pairs. It is implemented as a linked list. The head and tail refers to the start and end of the hash table.
*/
struct Storage {
    KV_Pair* kv_pairs_head;
    KV_Pair* kv_pairs_tail;
};

typedef struct Storage Storage;

/*
    Data structure for a node in the hash table. It contains the node number, the storage for key-value pairs and the finger table.
*/
struct Node {
    int node_no;
    Storage storage;
    finger_table_struct *finger_table;
};


typedef struct Node Node;

#endif