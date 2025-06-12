/*
* Author: Lorenzo Wyman
* Course: CS2
* Professor Ali
* Assignment: Student Grade Analyzer Using Linked List
* Date: April 20, 2025
*/

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

typedef struct Student{

    char first[30];
    char last[30];
    int score;
    struct Student* next;

} Student;

int isEmpty(Student *head){

    if (head == NULL){
        return 1;
    }

    return 0;
}

Student *createStudentNode(const char *first, const char *last, int score){

    Student *node = malloc(sizeof(Student)); //making the dma with size of node bcuz we only need one
    if (node == NULL){
        printf("memory allocation failed for 'node'\n");
        return NULL; //or exit(1)
    }

    strncpy(node -> first, first, sizeof(node -> first) - 1); // calculates the specific amount of n characters to be copied
    node -> first[sizeof(node -> first) - 1]; //just terminated the null character
    strncpy(node -> last, last, sizeof(node -> last) - 1); 
    node -> last[sizeof(node -> last) - 1]; 

    node -> score = score; //the score is now equal to score
    node -> next = NULL;

    return node;
}

Student *insertHead(Student *head, const char *first, const char *last, int score){

    Student *newNode = createStudentNode(first, last, score); //allocates memory for creation of a new node

    if (isEmpty(head)){ //check if its empty, then return new node if is
        return newNode;
    }

    newNode -> next = head; //makes the new node point to the head pointer
    head = newNode; //the head is now equal to the newNode

    return head;
}

Student *getStudentInfo(){

    Student *head = NULL;
    char fileName[100];

    printf("Enter input file name: ");
    if (scanf("%99s", fileName) != 1) {
        printf("Error reading filename.\n");
        return NULL;
    }

    while (getchar() != '\n'); //removes whitespace after user puts in filename

    //printf("Attempting to open file: %s\n", fileName);
    printf("\n");
    
    FILE *reader = fopen(fileName, "r");
    if (reader == NULL){
        printf("Error opening file: '%s'\n", fileName);
        return NULL;
    }

    char line[50];
    int score;
    char first[30];
    char last[30];

    while (fgets(line, sizeof(line), reader)){

        line[strcspn(line, "\n")] = 0;
        if (sscanf(line, "%s %s %d", first, last, &score) == 3){
           head = insertHead(head, first, last, score);
        } else {
            printf("error pasring/reading line: '%s'\n", line);
        }

    }
    fclose(reader);

    return head;
}

void displayStudentInfo(Student *head){

    Student *currentStudent = head; //pointer to the current student

    if (isEmpty(head)){
        printf("The list is empty.\n");
        return;
    }

    while (currentStudent != NULL){
        printf("%s %s - %d\n", currentStudent -> first, currentStudent -> last, currentStudent -> score);
        currentStudent = currentStudent -> next;
    }

}

/*Student *scoreSort(){

    Student *head = NULL;
    
    printf("Sorted List by score: \n");

    if (low < high){ //find midpoint so that we can start splitting in half

        int mid = (low + high) / 2; //the midpoint

        //now we recurrsivley call mergesort of each half
        mergeSort(array, low, mid); //first half of the array
        mergeSort(array, mid + 1, high); //second half of the array
        merge(array, low, mid, high); //wont execute if the two lines above havent reached base casw

    }

    return head;
}*/

void splitUpList(Student *origin, Student **start, Student **end){
   
    Student *singlemove = origin;
    Student *doublemove = origin ? origin -> next: NULL; //ptr moves twice as fast as singlemove

    while (doublemove != NULL){

        doublemove = doublemove -> next; //its gonna continue traversing the linkedlist

        if (doublemove != NULL){

            singlemove = singlemove -> next;
            doublemove = doublemove -> next;
        }

    }

    *start = origin; //by this point the single move ptr is at the halfway 
    *end = singlemove ? singlemove -> next : NULL;

    if (singlemove != NULL){
        singlemove -> next = NULL;
    }

}

Student *mergeSplitList(Student *list1, Student *list2){
    
    Student *mergedList = NULL; //this is going to become new head
    Student **tail = &mergedList;

    while (list1 != NULL && list2 != NULL) {

        if (list1 -> score >= list2 -> score) {
            *tail = list1;
            list1 = list1 -> next;
        } else {
            *tail = list2;
            list2 = list2 -> next;
        }
        tail = &((*tail)->next); // moves tail ptr to next position
    }

    if (list1 != NULL) {

        *tail = list1;

    } else if (list2 != NULL) {

        *tail = list2;
    }

    return mergedList;
}

Student *mergeToBigList(Student *head){
    
    if (head == NULL){ //recurrsive basecases
       
        return head;
    } 
    if (head -> next == NULL){
        
        return head;
    }

    Student *firstHalf; //of linkedlist
    Student *secondHalf;

    splitUpList(head, &firstHalf, &secondHalf);// this is actually splitting the list into 2
    
    firstHalf = mergeToBigList(firstHalf); //recursion to sort them
    secondHalf = mergeToBigList(secondHalf);
    Student *merged = mergeSplitList(firstHalf, secondHalf); //returns the combined list of the firsthald and secondhalf
   
    return merged;
}

int calcHighestScore(Student *head){

    Student *current = head;

    int highestScore = -1; //so that the first one we encounter is guaranteed to be higher in event a score is 0

    while (current != NULL){

        if (current -> score > highestScore){
            highestScore = current -> score; //the highest score is now found and stored as score
        }
        current = current -> next;
    }
    printf("Highest Score: %d\n", highestScore);

    return highestScore;
}

int calcLowestScore(Student *head){

    if (head == NULL){
        printf("list empty, no lowest scor\n");
        return -1;
    }

    Student *current = head;

    int lowestScore = current -> score; //so that the first one we encounter is guaranteed to be higher in event a score is 0

    while (current != NULL){

        if (current -> score < lowestScore){
            lowestScore = current -> score; //the low score is now found and stored as score
        }
        current = current -> next;
    }
    printf("Lowest Score: %d\n", lowestScore);

    return lowestScore;
}

int calcMedianScore(Student *head){

    if (head == NULL){
        printf("list empty, cant calc median\n");
        return -1;
    }
    
    int count = 0;
    Student *currentptr = head;

    while (currentptr != NULL){

        count++;
        currentptr = currentptr -> next;

    }

    int midpoint = (count / 2);
    currentptr = head;

    for (int i = 0; i < midpoint - 1 && count % 2 == 0; i++){

        currentptr = currentptr -> next;
    }
    for (int i = 0; i < midpoint && count % 2 != 0; i++){
        currentptr = currentptr -> next;
    }

    double medianScore;

    if (count % 2 == 0){

        double mid1 = currentptr -> score;
        double mid2 = currentptr -> next -> score;
        //printf("mid1 = %.2f, mid2 = %.2f\n", mid1, mid2);
        medianScore = (mid1 + mid2) / 2.0;

    } else {
        medianScore = currentptr -> score;
    }

    printf("Median Score: %.2lf\n", medianScore);

    return (int)medianScore;
}
    
double calcAverageScore(Student *head) {

    int sum = 0;
    int count = 0;

    Student *current = head;

    while (current != NULL) {
        sum += current->score;
        count++;
        current = current->next;
    }

    double avg = (count > 0) ? (double)sum / count : 0;
    printf("Class Average: %.2lf\n", avg);

    return avg;
}

int displayTopStudents(Student *head){

    Student *current = head;
    printf("\nTop 5 Students: \n");

    int topFive = 0;

    while (current != NULL && topFive < 5){
        printf("%s %s - %d\n", current -> first, current -> last, current -> score);
        current = current -> next;
        topFive++;
    }

    return 0;
}

void freeStudentList(Student* head){

    Student* current = head;
    Student* nextNode;

    while (current != NULL){
        nextNode = current -> next;
        free(current);
        current = nextNode;
    }
}

int main(int argc, char **argv){

    Student *listHead = getStudentInfo();
    //const char *fileName = argv[1];
    
    Student *sortedList = NULL;

    if (listHead != NULL){

        //displayStudentInfo(listHead);
        sortedList = mergeToBigList(listHead);
        
        printf("Sorted List by Score: \n");
        displayStudentInfo(sortedList);
        printf("\n");

        calcHighestScore(sortedList);
        calcLowestScore(sortedList);
        calcMedianScore(sortedList);
        calcAverageScore(sortedList);
        
        displayTopStudents(sortedList);
        freeStudentList(sortedList);
    } else {
        printf("unable to load student info :(\n");
        return 1;
    }

    return 0;

}