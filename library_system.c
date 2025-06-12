/*
Name: Lorenzo Wyman
Date: 2/24/2025
Assignment 2: Library Book Borrowing System
UCFID: 5464079
Class: COP3502
Professor Mohammed Ali
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_TITLE 100
#define MAX_NAME 50

typedef struct node_s{

    char *title;
    char *lastName;
    char *firstName;
    
    struct node_s *nextptr;

} node_t;

// Prototypes
node_t *borrowBook(node_t *head, char *title, char *lastName, char *firstName, FILE *output);
node_t *returnBook(node_t *head, char *title, char *lastName, char *firstName, FILE *output);
void checkBook(node_t *head, char *title, char *lastName, char *firstName, FILE *output);
void displayBorrowedBooks(node_t *head, FILE *output);
void freeList(node_t *head, FILE *output);

int isEmpty(node_t *head){ //pass head of list 

    if (head == NULL){
        return 1; 
    }
    return 0;
}

node_t *operationHandler(node_t *head, char *fileName, char *title, char* lastName, char *firstName, FILE *output){

    //fprintf(output, "Attempting to open file: '%s'\n", fileName);
    
    FILE *reader = fopen(fileName, "r");
    if (reader == NULL){
        fprintf(output, "Error opening file: '%s'\n", fileName);
        return head;
    }

    int operation = 0;
    char line[300];

    while (fgets(line, sizeof(line), reader)){

        line[strcspn(line, "\n")] = 0;
        //printf("\nAssessing line: '%s'\n",line);
    
        if (sscanf(line, "%d \"%[^\"]\" %s %s", &operation, title, lastName, firstName) == 4){ 
            // gets the operation from in the file
            //printf("operation: '%d', title: '%s', lastname: '%s', firstname: '%s'\n", operation, title, lastName, firstName);

            switch (operation){
                case 1:
                    head = borrowBook(head, title, lastName, firstName, output); // "head =" updates from old head to new head
                    break;
                case 2:
                    head = returnBook(head, title, lastName, firstName, output);
                    break;
                case 3:
                    checkBook(head, title, lastName, firstName, output);
                    break;
                default:
                    fprintf(output, "Error handling book operation for line: '%s'\n", line);
                    break;  
            } 

        } else if (sscanf(line, "%d", &operation) == 1){
            
            if (operation == 4){
                displayBorrowedBooks(head, output);
            } else if (operation == 5){
                fprintf(output, "Thank you for using the Library System!\n");
                break;
            }
        }
    } 
    fclose(reader);
    return head;
}

node_t *createBookNode(node_t *head, char *title, char* lastName, char *firstName, FILE *output){

    node_t *node = malloc(sizeof(node_t)); //making the dma with size of node bcuz we only need one

    if (node == NULL){
        fprintf(output, "memory allocation failed for 'node'\n");
        return NULL;
    }
    //DMA for title, first and last name strings to be read from file unlike scanf in class
    node -> title = malloc(sizeof(char) * MAX_TITLE); 
    node -> lastName = malloc(sizeof(char) * MAX_NAME);
    node -> firstName = malloc(sizeof(char) * MAX_NAME);
    node -> nextptr = NULL;

    strcpy(node -> title, title); //copy those strings and put them into the data
    strcpy(node -> lastName, lastName);
    strcpy(node -> firstName, firstName);

    return node;
}

node_t *borrowBook(node_t *head, char *title, char *lastName, char *firstName, FILE *output){

    node_t *newNode = createBookNode(head, title, lastName, firstName, output);
    if (newNode == NULL){
        fprintf(output, "DMA failed for newNode.\n");
        return head;
    } 

    fprintf(output, "Borrowed \"%s\" by %s, %s\n", title, lastName, firstName);

    newNode -> title = strdup(title); //strdup copies strings while also doing dma for them
    newNode -> lastName = strdup(lastName);
    newNode -> firstName = strdup(firstName);
    newNode -> nextptr = NULL;

    if (head == NULL){ //making the new node the head if it is null
        head = newNode;
    } else{

        node_t *temp = head;
        while (temp -> nextptr != NULL){ //exactly what was done in class when inserting at tail instead of head
            temp = temp -> nextptr;
        }
        temp -> nextptr = newNode;  
    }
    
    return head;
}

node_t *returnBook(node_t *head, char *title, char *lastName, char *firstName, FILE *output){

    if (isEmpty(head)){
        fprintf(output, "the list is empty.\n");
        return NULL;
    }

    fprintf(output, "Returned \"%s\" by %s, %s\n", title, lastName, firstName);

    node_t *temp = head;
    node_t *prev = NULL;

    //find node to be removed (looping)
    while(temp != NULL){ //while it doesn't equal the data or reach the end
        if(strcmp(temp -> title, title) == 0 && strcmp(temp -> lastName, lastName) == 0 && strcmp(temp -> firstName, firstName) == 0){
            break;
        }
        prev = temp;
        temp = temp -> nextptr; //moving to the next node
    }

    if (temp == NULL){
        fprintf(output, "The node was not found.\n");
        return head; //returning prev head cuz it wasn't found
    }

    //if the node to be removed is head
    if (temp == head){
        head = head -> nextptr; //were moving the head to the next pointer to avoid errors
    } else { //need to point to the temp pointer before you delete it
        prev -> nextptr = temp -> nextptr;
    }
    
    free(temp); //freed memory for deleted node
    //printf("Book returned.\n");

    return head;
}

void checkBook(node_t *head, char *title, char *lastName, char *firstName, FILE *output){

    if (isEmpty(head)){
        fprintf(output, "the list is empty.\n");
        return;
    }

    node_t *temp = head;

    while(temp != NULL){ //while it doesnt equal the data or reach the end
        if(strcmp(temp -> title, title) == 0 && strcmp(temp -> lastName, lastName) == 0 && strcmp(temp -> firstName, firstName) == 0){
            fprintf(output, "\"%s\" is borrowed by %s, %s\n", title, lastName, firstName);
            return;
        } 
        temp = temp -> nextptr; // move to the next node
    }
    fprintf(output, "\"%s\" is not currently borrowed by %s, %s\n", title, lastName, firstName);
}

void displayBorrowedBooks(node_t *head, FILE *output){

    if (isEmpty(head)){
        fprintf(output, "The list is empty.\n");
        return;
    }

    fprintf(output, "Borrowed Books List:\n");
    int i = 1;
    node_t *temp = head;
    while (temp != NULL){
        fprintf(output, "%d. \"%s\" - %s, %s\n", i, temp -> title, temp ->lastName, temp -> firstName);
        temp = temp -> nextptr;
        i++;
    }
}

void freeList(node_t *head, FILE *output){

    if (isEmpty(head)){
        fprintf(output, "List is empty.\n");
        return;
    }

    node_t *temp = head;

    while (head != NULL){ // loop that loops through all of the nodes and then clears/frees them then head(temp) itself

        head = head -> nextptr;
    
        free(temp -> firstName);
        free(temp -> lastName);
        free(temp -> title);
        free(temp);
    }
}

int main(int argc, char **argv){

    FILE *output = fopen("output.txt", "w");
    if (output == NULL){
        fprintf(output, "error opening file output.\n");
        return 1;
    }

    char *fileName = argv[1];
    node_t *head = NULL;

    char title[MAX_TITLE];
    char lastName[MAX_NAME];
    char firstName[MAX_NAME];

    head = operationHandler(head, fileName, title, lastName, firstName,output); //head = updates from old head to new head
    createBookNode(head, title, lastName, firstName, output);

    fclose(output);
    return 0;

}