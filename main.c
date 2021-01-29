#include "data_structures.h"
#include "functions.h"

#define BUF 255

int main()
{
    //Create a chord ring first.
    Node **chord_ring = create_DHT();

    printf("Chord table successfully built\n");

    FILE* fp = fopen("operations.txt", "r");
    if(fp == NULL) {
        printf("ERROR! Cannot open file.\n");
        return 0;
    }

    int num_operations;
    fscanf(fp, "%d", &num_operations);

    /*
        The operations that are supported are:
        1. create_node: 
            Creates a new node and inserts it in the chord ring. 
            Arguments - node_number 

        2. delete_node:
            Deletes the node from the chord ring if it exists.
            Arguments - node_number
        
        3. insert:
            Inserts a key-value pair in the ring. 
            Arguments - key (an integer) and value (a string).
        
        4. delete: 
            Deletes a key-value pair from the ring if it exists.
            Arguments - key(an integer)
        
        5. get:
            Retrieves the value associated with the given key.
            Arguments - key(an integer)
    */

   char c;
   fscanf(fp, "%c", &c);
    for(int i=0; i<num_operations; i++){
        char str[BUF];
        
        fgets(str, BUF, fp);

        char operation[BUF] = "\0";
        int i;
        for(i=0; i<strlen(str) && str[i]!=' '; i++){
            operation[i] = str[i];
        }

        i++;
        int key = 0;
        for(; i<strlen(str) && str[i]!=' ' && str[i]!='\n'; i++) {
            key = key*10 + str[i] - '0';
        }

        printf("Operation: %s, key: %d\n", operation, key);

        //Operation - creating a new node in the chord ring.
        if(strcmp(operation, "create_node") == 0) {
            printf("Creating node %d\n", key);
            create_node(key, chord_ring);
        }

        //Operation - inserting a new key-value pair in the chord ring.
        else if(strcmp(operation, "insert") == 0) {
            char value[BUF] = "\0";
            int index = 0;
            for(; i<strlen(str); i++, index++) {
                value[index] = str[i];
            }
            printf("Inserting new key %d and value %s\n", key, value);
            insert(key, value, chord_ring);
        }

        //Operation - getting the value for a key from the chord ring.
        else if(strcmp(operation, "get") == 0) {
            printf("Getting value for key %d\n", key);
            char* value = get(key, chord_ring);
            if(value == NULL) {
                printf("Key not found\n");
            }
            else {
                printf("%s\n", value);
            }
        }

        //Operation - deleting a key-value pair from the chord ring.
        else if(strcmp(operation, "delete") == 0) {
            printf("Deleting value: %d\n", key);
            delete(key, chord_ring);
        }

        //Operation - deleting a node from the chord ring.
        else if(strcmp(operation, "delete_node") == 0) {
            printf("Deleting node: %d\n", key);
            delete_node(key, chord_ring);
        }
        printf("\n");
    }

    //Delete the chord ring once all operations are finished.
    printf("Deleting chord ring\n");
    delete_DHT(chord_ring);

    fclose(fp);
    return 0;

}
