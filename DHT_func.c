#include "data_structures.h"
#include "functions.h"

Node **initChord();
Node *find_successor(int key, Node *n);
Node *find_DHT(int key, Node **chord_ring);
void print_chord_ring(Node **chord_ring);
void print_storage(Storage storage);

/*
    Function to create and initialize a new DHT. 
    Preconditions - NULL
    Postconditions - chord_ring!=NULL
*/
Node** create_DHT()
{
    Node **chord_ring = (Node **)malloc(sizeof(Node *) * MAX_VAL);

    for (int i = 0; i < MAX_VAL; i++)
    {
        chord_ring[i] = NULL;
    }

    assert(chord_ring!=NULL);
    return chord_ring;
}

/*
    Function to update the entries in the finger table of current node.
    Preconditions - curr_node != NULL and chord_ring != NULL. Chord ring represents the complete DHT.
    Postconditions - NULL
*/
void populate_finger_table(Node* curr_node, Node** chord_ring) {
    assert(curr_node!=NULL && chord_ring!=NULL);

    int node_no = curr_node->node_no;
    for (int i = 0; i < SIZE; i++)
    {
        int hash_val = (node_no + (1 << i)) % MAX_VAL;

        while (chord_ring[hash_val] == NULL)
        {
            hash_val = (hash_val + 1) % MAX_VAL;
        }
        curr_node->finger_table[i].identifier = hash_val;
        curr_node->finger_table[i].next = chord_ring[hash_val];
    }
}

/*
    Function to update the finger tables of all the nodes.
    Preconditions - chord_ring != NULL. Chord ring represents the complete DHT.
    Postconditions - NULL
*/
void update_finger_tables(Node** chord_ring) {
    assert(chord_ring!=NULL);

    for (int i = 0; i < MAX_VAL; i++)
    {
        if (chord_ring[i] != NULL)
        {
            populate_finger_table(chord_ring[i], chord_ring);
        }
    }
}

/*
    Function to create a new node and insert it in the DHT (Chord ring).
    Preconditions - node_no >= 0 and node_no < maximum value of hash_key and chord_ring != NULL.
                    The maximum value of hash key depends on the size of hash key. 
    Postconditions - NULL
*/
void create_node(int node_no, Node **chord_ring)
{   
    assert(node_no>=0 && node_no<MAX_VAL && chord_ring!=NULL);

    if (chord_ring[node_no] != NULL)
    {
        printf("ERROR! Node already exists. Returning\n");
        return;
    }
    printf("Node being inserted: %d\n", node_no);

    //Create a new node for the current key.
    chord_ring[node_no] = (Node *)malloc(sizeof(Node));
    Node *curr_node = chord_ring[node_no];
    curr_node->node_no = node_no;
    curr_node->storage.kv_pairs_head = NULL;
    curr_node->storage.kv_pairs_tail = NULL;
    curr_node->finger_table = malloc(sizeof(finger_table_struct) * SIZE);
    for (int i = 0; i < SIZE; i++)
    {
        curr_node->finger_table[i].identifier = -1;
        curr_node->finger_table[i].next = NULL;
    }

    //Update the entries in the finger table of current node
    populate_finger_table(curr_node, chord_ring);

    //Update the entries in the finger table of remaining nodes
    update_finger_tables(chord_ring);

    Node* successor = curr_node->finger_table[0].next;
    KV_Pair *temp = successor->storage.kv_pairs_head, *prev = NULL;

    //Redistribute the keys among the successor and the current node. 
    while(temp!=NULL) {

        //If the key stored in the successor is less than the value of the current node, it should have been mapped to the current node. Remove it from the successor and insert it in the current node.
        if(temp->key <= curr_node->node_no) {
            int key = temp->key;
            char* value = temp->value;

            KV_Pair *new_pair = (KV_Pair*)malloc(sizeof(KV_Pair));
            new_pair->key = key;
            new_pair->next = NULL;
            new_pair->value = (char*)malloc(sizeof(char)*strlen(value));
            strcpy(new_pair->value, value);

            if(curr_node->storage.kv_pairs_head == NULL) {
                curr_node->storage.kv_pairs_head = new_pair;
                curr_node->storage.kv_pairs_tail = new_pair;
            }
            else {
                curr_node->storage.kv_pairs_tail->next = new_pair;
                curr_node->storage.kv_pairs_tail = new_pair;
            }

            KV_Pair* free_node = temp;
            if(prev != NULL) {
                prev->next = temp->next;
            }
            else {
                successor->storage.kv_pairs_head = successor->storage.kv_pairs_head->next;
            }
            temp = temp->next;
            free_node->next = NULL;
            free(free_node);
        }
        else {
            prev = temp;
            temp = temp->next;
        }
    }
}

/*
    Function to delete a node from the chord ring .
    Preconditions - node_no>=0 and node_no < maximum value of hashkey and chord_ring != NULL
    Postconditions - NULL
*/
void delete_node(int node_no, Node** chord_ring) {
    assert(node_no>=0 && node_no<MAX_VAL && chord_ring!=NULL);

    if(chord_ring[node_no] == NULL) {
        printf("ERROR! Node not present\n");
        return;
    }

    Node* curr_node = chord_ring[node_no];
    Node* succ_node = chord_ring[node_no]->finger_table[0].next;

    KV_Pair* curr_head = curr_node->storage.kv_pairs_head;
    KV_Pair* succ_head = succ_node->storage.kv_pairs_head;
    KV_Pair* succ_tail = succ_node->storage.kv_pairs_tail;

    //Redistribute the keys stored in this node to its successor.
    while(curr_head != NULL) {
        KV_Pair* new_pair = (KV_Pair*)malloc(sizeof(KV_Pair));
        new_pair->key = curr_head->key;
        new_pair->value = (char*)malloc(sizeof(char)*strlen(curr_head->value));
        strcpy(new_pair->value, curr_head->value);
        new_pair->next = NULL;

        if(succ_head == NULL) {
            succ_head = new_pair;
            succ_tail = new_pair;
        }
        else {
            succ_tail->next = new_pair;
            succ_tail = new_pair;
        }

        KV_Pair* free_node = curr_head;
        curr_head = curr_head->next;
        free_node->next = NULL;
        free(free_node);
    }

    free(curr_node->finger_table);
    free(curr_node);

    chord_ring[node_no] = NULL;

    //After deleting the node, update the finger tables of remaining nodes.
    update_finger_tables(chord_ring);

    return;
}

/*
    Recursive function to find the successor of queried key. 
    The recursion will end when the key lies between the current node and its successor. 
*/
Node *find_successor(int key, Node *n)
{
    if(key == n->node_no)
        return n;
    else if (key > n->node_no && key <= n->finger_table[0].identifier)
        return n->finger_table[0].next;
    else
    {
        Node *temp = n;
        for (int i = SIZE - 1; i >= 0; i--)
        {
            if (n->finger_table[i].identifier > n->node_no && n->finger_table[i].identifier < key)
            {
                temp = n->finger_table[i].next;
                break;
            }
        }

        if(temp == n){
            return n;
        }
        return find_successor(key, temp);
    }
}

/*
    Function to find the node responsible for the queried key. 
    Preconditions - key>=0 and key<maximum value of hashkey and chord_ring != NULL
*/
Node *find_DHT(int key, Node **chord_ring)
{
    assert(key>=0 && key < MAX_VAL && chord_ring != NULL);

    Node *first_node = NULL;
    for (int i = 0; i < MAX_VAL; i++)
    {
        if (chord_ring[i])
        {
            first_node = chord_ring[i];
            break;
        }
    }

    if (first_node)
        return find_successor(key, first_node);
    return NULL;
}

/*
    Function to insert a new key-value pair in the chord ring.
    Preconditions - key >= 0 and key < maximum value of hashkey and chord_ring != NULL and value != NULL
    Postconditions - NULL
*/
void insert(int key, char *value, Node **chord_ring)
{
    assert(chord_ring!=NULL && value!=NULL && key>=0 && key<=MAX_VAL);

    //First find the node responsible for storing the key-value pair.
    Node* node = find_DHT(key, chord_ring);
    if(node != NULL) {

        //Check if the key-value pair already exists.
        char* check_value = get(key, chord_ring);
        if(check_value != NULL) {
            printf("ERROR! Key already exists\n");
            return;
        }

        KV_Pair* new_pair = (KV_Pair*)malloc(sizeof(KV_Pair));
        new_pair->key = key;
        new_pair->next = NULL;
        new_pair->value = (char*)malloc(sizeof(char)*strlen(value));
        strcpy(new_pair->value, value);
        
        //Insert the new key-value pair.
        if(node->storage.kv_pairs_head == NULL) {
            node->storage.kv_pairs_head = new_pair;
            node->storage.kv_pairs_tail = new_pair;
        }
        else {
            node->storage.kv_pairs_tail->next = new_pair;
            node->storage.kv_pairs_tail = new_pair;
        }
    }
    else {
        printf("ERROR! No node present currently\n");
    }

    return;
}

/*
    Function to get the value for the queried key.
    Preconditions - key >= 0 and key < maximum value of hashkey and chord_ring != NULL
    Postconditions - NULL
*/
char *get(int key, Node **chord_ring)
{
    assert(key>=0 && key<MAX_VAL && chord_ring!=NULL);

    //First find the node responsible for the key.
    Node *node = find_DHT(key, chord_ring);
    if (node == NULL)
    {
        return NULL;
    }

    //Check the storage of the node for finding the key.
    KV_Pair* temp = node->storage.kv_pairs_head;
    while(temp!=NULL) {
        if(temp->key == key) {
            return temp->value;
        }
        temp = temp->next;
    }
    return NULL;
}

/*
    Function to delete a key-value pair from the chord ring.
    Preconditions - key >= 0 and key < maximum value of hashkey and chord_ring != NULL
    Postconditions - NULL 
*/
void delete(int key, Node** chord_ring) {
    assert(key>0 && key<MAX_VAL && chord_ring!=NULL);

    //First, find the node responsible for the given key.
    Node* node = find_DHT(key, chord_ring);
    
    if(node == NULL) {
        printf("ERROR! Key not present.\n");
        return;
    }

    //Delete the key if present in the storage.
    KV_Pair *temp = node->storage.kv_pairs_head, *prev = NULL;
    int flag = 0;
    while(temp != NULL) {
        if(temp->key == key) {
            flag = 1;
            KV_Pair* free_node = temp;
            
            if(prev != NULL) {
                prev->next = temp->next;
            }
            else {
                node->storage.kv_pairs_head = node->storage.kv_pairs_head->next;
            }
            
            temp = temp->next;
            free_node->next = NULL;
            free(free_node);
        }
        else {
            prev = temp;
            temp = temp->next;
        }
    }

    if(flag == 0) {
        printf("ERROR! Key not present\n");
    }

    return;
}

/*
    Function to delete the Chord ring. 
    Preconditions - chord_ring != NULL
    Postconditions - NULL
*/
void delete_DHT(Node **chord_ring)
{   
    assert(chord_ring!=NULL);

    for (int i = 0; i < MAX_VAL; i++)
    {
        if (chord_ring[i] != NULL)
        {   
            free(chord_ring[i]->finger_table);
            KV_Pair *temp = chord_ring[i]->storage.kv_pairs_head;
            while(temp != NULL) {
                KV_Pair* free_node = temp;
                temp = temp->next;
                free_node->next = NULL;
                free(free_node);
            }
            free(chord_ring[i]);
            printf("%p\n", chord_ring[i]);
        }
    }
    free(chord_ring);
}