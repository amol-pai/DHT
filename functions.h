#ifndef __FUNCTIONS_H_
#define __FUNCTIONS_H_

#include "data_structures.h"

//Function to create a new DHT.
extern Node** create_DHT();

//Function to create a new node in the DHT.
extern void create_node(int key, Node** chord_table);

//Function to delete a node from the DHT.
extern void delete_node(int node_no, Node** chord_table);

//Function to insert a new key-value pair in the DHT.
extern void insert(int key, char* data, Node** chord_table);

//Function to retrieve a key-value pair from the DHT.
extern char* get(int key, Node** chord_table);

//Function to delete a key from the DHT.
extern void delete(int key, Node** chord_table);

//Function to delete the DHT.
extern void delete_DHT(Node** chord_table);

#endif