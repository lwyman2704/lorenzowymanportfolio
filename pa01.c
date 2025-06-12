/*============================================================================
| Assignment: pa01 - Encrypting a plaintext file using the Hill cipher
|
| Author: Lorenzo WYman
| Language: C
| To Compile: gcc -o pa01 pa01.c
| To Execute: ./pa01 kX.txt pX.txt
| where kX.txt is the keytext file
| and pX.txt is plaintext file
| Note:
| All input files are simple 8 bit ASCII input
| All execute commands above have been tested on Eustis
|
| Class: CIS3360 - Security in Computing - Spring 2025
| Instructor: McAlpin
| Due Date: February 23, 2025
+===========================================================================*/

#include <stdio.h>
#include <stdlib.h> 
#include <ctype.h>

#define MAX_ROW_CHARS 80

int **createMatrix(int size) {

    int **matrix = (int**)malloc(size * sizeof(int*)); // allocate memory for matrix
    if (matrix == NULL) {
        printf("Memory allocation failed for matrix\n");
        return NULL;
    }

    for (int i = 0; i < size; i++) {

        matrix[i] = (int*)malloc(size * sizeof(int)); // creates a new array for each row
        if (matrix[i] == NULL) { // freeing the previously allocated rows before returning NULL
            for (int j = 0; j < i; j++) {
                free(matrix[j]);
            }
            free(matrix);
            return NULL;
        }
    }
    // print the message only once after the entire matrix is created
    // printf("Matrix with size of %d rows & columns created.\n", size, size);
    
    return matrix;
}

int **getKeyMatrix(char *filename, int *size){

    // printf("\nAttempting to open key matrix file: '%s'\n", filename);

    FILE *reader = fopen(filename, "r");
    if (reader == NULL) {
        perror("Error opening file1.\n");
        return NULL; 
    }

    if (fscanf(reader, "%d", size) != 1) { // gets the dimensions from the key matrix in the file
        printf("Error reading matrix size from file: %s\n", filename);
        fclose(reader);
        return NULL;
    }

    // printf("Key matrix of size '%d' is found\n", *size);

    int **readInMatrix = createMatrix(*size); // read and allocate memory for the key matrix that is given in the file
    if (readInMatrix == NULL){
        fclose(reader);
        return NULL;
    }
    
    for (int i = 0; i < *size; i++) { // checks the dimensions for the readin matrix
        for (int j = 0; j < *size; j++) {
            if (fscanf(reader, "%d", &readInMatrix[i][j]) != 1) {
                printf("Error reading matrix values at [%d, %d]\n", i, j);
                fclose(reader);
                return NULL;
            }
        }
    }
    fclose(reader);

    printf("\nKey matrix:\n");
    for (int i = 0; i < *size; i++) { // for loop printing the matrix read in from the file (key matrix)
        for (int j = 0; j < *size; j++) {
            printf("%4d", readInMatrix[i][j]);
        }
        printf("\n");
    }
    
    return readInMatrix;
}

int getPlaintext(char *filename2, int size){

    char pc; // plaintext character
    char array[10000];

    // printf("\nAttempting to read plaintext file: '%s'\n", filename2);

    FILE *reader = fopen(filename2, "r");
    if (reader == NULL) {
        perror("Error opening file2.\n");
        return 1; 
    }
    
    printf("\nPlaintext: \n");
    fflush(stdout);
    int charCount = 0;

    while ((pc = fgetc(reader)) != EOF){ // while it is scanning the file and reading the characters individually (useful for detecting weird symbols) isn't at the end of the file

        if (isalpha(pc)){  // if it is not letters, ignore
            putchar(tolower(pc)); // print those plaintext characters (pc)
            array[charCount++] = tolower(pc);
            if(charCount % MAX_ROW_CHARS == 0){
                printf("\n");
            }
        }
    }
    fclose(reader);

    int xPadding = (charCount % size == 0) ? 0 : size - (charCount % size); // if the first condition (divisible by size) != true then the second half will run
    for (int i = 0; i < xPadding; i++){
        putchar('x');  // appends the 'x' to the end of the plaintext from file
        charCount++;
        if (charCount % MAX_ROW_CHARS == 0){
            printf("\n");
        }
    }
    printf("\n");

    return 0;
}

int *convertPlaintextToNums(char *filename2, int *count){

    char pc;
    int arraySize = 10000;

    int *numsArray = malloc(sizeof(int) * arraySize); // dma for array that is going to store the values of the plaintext characters when converted to numbers
    if (numsArray == NULL){
        printf("Mem allocation failed for plaintext numbers.\n");
        return NULL;
    }
    
    *count = 0;

    FILE *reader = fopen(filename2, "r");
    if (reader == NULL) {
        perror("Error opening file.\n");
        free(numsArray);
        return NULL; 
    }

    while ((pc = fgetc(reader)) != EOF){ //while it is scanning the file and reading the characters individually (useful for detecting weird symbols) isn't at the end of the file

        if (isalpha(pc)){  //if it is not letters, ignore
            numsArray[*count] = tolower(pc) - 'a';
            (*count)++;
        }
    }
    fclose(reader);

    if (*count % 2 != 0){ 
        numsArray[*count] = 23; // X
        (*count)++;  //if count is off then it pads with a 0
    }

    /*for (int i = 0; i < *count; i++){ // used to just print and align the numbers of the letters in a vextor format for matrix multiplication

        //for (int j = i; j < *count; j +=2){

            printf("%4d", numsArray[i]);
            if((i + 1) % 2 == 0){
                printf("\n");
            }
      // }
       // printf("\n");
    *///}

    return numsArray;
}

void getCiphertext(int **readInMatrix, int *plaintextNums, int size, int count){

    printf("\n");
    printf("Ciphertext: \n");

    if (readInMatrix == NULL){
        printf("Error reading matrix data (it's NULL)\n");
    }
    if (plaintextNums == NULL){
        printf("Error reading plaintext data (it's NULL)\n");
    }

    int charCount = 0;
    char cipherText[10000];

    for (int i = 0; i < count; i += size){
       
        int *vector = (int*)malloc(sizeof(int) * size);
        if (vector == NULL){
            printf("DMA failed for vector.\n");
            return;
        }

        for (int j = 0; j < size; j++){
            if (i + j < count){
                vector[j] = plaintextNums[i + j];
            } else {
                vector[j] = 23; //places 23 instead of x which will still translate back to x when converted from number to ciphertext
            }
        }
       // printf("\nCalculating vector: [%d, %d]\n", vector[0], vector[1]);
       // printf("Now calculating sums:\n");

        for (int rows = 0; rows < size; rows++){
            int sum = 0;

            for (int cols = 0; cols < size; cols++){
                sum += readInMatrix[rows][cols] * vector[cols];
            }

            sum = sum % 26;
            char cipherChar = (char)(sum + 'a'); // converting the numbers back into letters
            cipherText[charCount++] = cipherChar; // storing the ciphertext into the ciphertext array

            // printf("calculated ciphertext letter: %c\n", cipherChar);
            // printf("corresponding number of calculated ciphertext letter: %4d\n", sum);

        }   
        free(vector);
    }
   
    for (int i = 0; i < charCount; i++){ // for loop to print out all of the individual letters together as ciphertext
        putchar(cipherText[i]);
        if ((i + 1) % MAX_ROW_CHARS == 0){
            printf("\n");
        }
    }

}

int main(int argc, char **argv){

    char *filename = argv[1];
    char *filename2 = argv[2];

    int size;
    int **readInMatrix = getKeyMatrix(filename, &size);
    if (readInMatrix == NULL){
        printf("failed to read key matrix.\n");
        return 1;
    }

    getPlaintext(filename2, size);

    int count;
    int *plaintextNums = convertPlaintextToNums(filename2, &count);
    if (plaintextNums == NULL){
        printf("plaintextnums null;");
        return 1;
    }
    
    getCiphertext(readInMatrix, plaintextNums, size, count);

    for (int i = 0; i < size; i++) {
        free(readInMatrix[i]);
    }
    free(readInMatrix);
    free(plaintextNums);

    return 0;
}
/*=============================================================================
| I,Lorenzo Wyman, 5464079 affirm that this program is
| entirely my own work and that I have neither developed my code together with
| any another person, nor copied any code from any other person, nor permitted
| my code to be copied or otherwise used by any other person, nor have I
| copied, modified, or otherwise used programs created by others. I acknowledge
| that any violation of the above terms will be treated as academic dishonesty.
+=============================================================================*/