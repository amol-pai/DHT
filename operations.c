#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define OPERATIONS 5
#define SIZE 3
#define MAX_VAL (1<<SIZE)

int main() {
    FILE* fp = fopen("operations.txt", "w");

    srand(time(NULL));

    char *data = "This is sample.";
    int num_operations = 15;

    fprintf(fp, "%d\n", num_operations+3);
    fprintf(fp, "create_node %d\n", rand()%MAX_VAL);
    fprintf(fp, "create_node %d\n", rand()%MAX_VAL);
    fprintf(fp, "create_node %d\n", rand()%MAX_VAL);

    for(int i=0; i<num_operations; i++) {
        int option = rand()%OPERATIONS;
        switch (option)
        {
        case 0:
            fprintf(fp, "create_node %d\n", rand()%MAX_VAL);
            break;
        
        case 1:
            fprintf(fp, "insert %d %s\n", rand()%MAX_VAL, data);
            break;
        
        case 2: 
            fprintf(fp, "delete %d\n", rand()%MAX_VAL);
            break;

        case 3:
            fprintf(fp, "get %d\n", rand()%MAX_VAL);
            break;

        case 4:
            fprintf(fp, "delete_node %d\n", rand()%MAX_VAL);
            break;

        default:
            break;
        }
    }

    fclose(fp);

}