/*============================================================================
| Assignment: pa02 - Calculate the checksum of an input file given:
| -> the name of the input file,
| -> the checksum size of either 8, 16, or 32 bits
| Author: Lorenzo Wyman
| Language: c
| To Compile: gcc -o pa02 pa02.c
| To Execute: c -> ./pa02 inputFilename.txt checksumSize
| where inputFilename.txt is the input file
| and checksumSize is either 8, 16, or 32
| Note:
| All input files are simple 8 bit ASCII input
| All execute commands above have been tested on Eustis
| Class: CIS3360 - Security in Computing - Spring 2025
| Instructor: McAlpin
| Due Date: March 23, 2025
+===========================================================================*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int readChecksumFile(char *fileName, FILE *outFile) {

    FILE *reader = fopen(fileName, "r"); //simple File i/o
    if (reader == NULL) {
        perror("Error opening file for reading.\n");
        return -1;
    }

    char buffer[300]; //set buffer so that it can look and read the files contents comfortably
    int charAmount = 0; //initialized the amount of characters to 0 so they can be added later on

    printf("\n"); //this not being here caused the test file to not run in the beginning

    while (fgets(buffer, sizeof(buffer), reader) != NULL) { //while the buffer is not null, we're going to print the characters one by one

        int lineLength = strlen(buffer);
        for (int i = 0; i < lineLength; i++) {
            printf("%c", buffer[i]);
            charAmount++;

            if (charAmount % 80 == 0) { //then we check if it is more than 80 characters, if so then newline
                printf("\n");
            }
        }
    }

    fclose(reader);
    return 0;
}

void calculateCheckSum(int checkSumSize, char *fileName, FILE *outFile) { //function to calculate checksum and do arithmetic

    FILE *reader = fopen(fileName, "r");
    if (reader == NULL) {
        perror("Error opening file for checksum calculation\n");
        exit(1);
    }

    unsigned long checksum = 0; //need to be used because of the %8lx below and because of the size of numbers we're dealing with
    unsigned long block = 0;

    int byteCnt = 0; 
    int characterCnt = 0;
    int ch;

    while ((ch = fgetc(reader)) != EOF) { //while we havent reached the end of file, get every single character including spaces and \n

        unsigned char byte = (unsigned char)ch; 
        characterCnt++; 
        //printf("'%c' -> '%d'\n", byte, byte); debugging statement for when printing the ascii values

        if (checkSumSize == 16) { //do arithmetic shift if the block is 16 or 32 and not needed for 8
            block = (block << 8) | byte; //this works better than before
            byteCnt++;

            if (byteCnt % 2 == 0) { //if it is divisible by 2

                checksum += block;
                checksum &= 0xFFFF; 
                block = 0;
            }

        } else if (checkSumSize == 32) {

            block = (block << 8) | byte;
            byteCnt++;

            if (byteCnt % 4 == 0) {

                checksum += block;
                checksum &= 0xFFFFFFFF; // Keep within 32-bit range
                block = 0;
            }

        } else if (checkSumSize == 8) {

            checksum += byte;
            checksum &= 0xFF; // Keep within 8-bit range
        }
    }

    if (checkSumSize == 16 && byteCnt % 2 != 0) {

        unsigned char xPadding = 'X'; //add the necessary padding (58) or X and include it in the counting
        printf("%c", xPadding); //then print it 
        //printf("'%c' -> '%d'\n", xPadding, xPadding); same as before debugging statement for when printing the ascii values
        block = (block << 8) | xPadding;
        checksum += block;
        checksum &= 0xFFFF;

        characterCnt++; 

    } 
    else if (checkSumSize == 32 && byteCnt % 4 != 0) {

        while (byteCnt % 4 != 0) {

            unsigned char xPadding = 'X';
            printf("%c", xPadding); //this prints the padding necessary (most often for 16 or 32 bits)
            //printf("'%c' -> '%d'\n", xPadding, xPadding);
            block = (block << 8) | xPadding; //shift left the blocks

            byteCnt++;
            characterCnt++;
        }
        checksum += block;
        checksum &= 0xFFFFFFFF;

    }
    fclose(reader);

    printf("\n%2d bit checksum is %8lx for all %4d chars\n", checkSumSize, checksum, characterCnt);
}

int main(int argc, char **argv) {

    FILE *outFile = fopen("output.txt", "w"); //to write to an output file to test whether or not it was similar to the ouput text files
    if (outFile == NULL) {
        perror("Error opening output file.\n");
        return -1;
    }

    int checkSumSize = atoi(argv[2]);
    if (checkSumSize != 8 && checkSumSize != 16 && checkSumSize != 32) {
        fprintf(stderr, "Valid checksum sizes are 8, 16, or 32\n"); //per the instructions
        return 1;
    }

    readChecksumFile(argv[1], outFile);
    calculateCheckSum(checkSumSize, argv[1], outFile);

    fclose(outFile);
    return 0;
}
/*=============================================================================
| I [Lorenzo Wyman] ([5464079]) affirm that this program is
| entirely my own work and that I have neither developed my code together with
| any another person, nor copied any code from any other person, nor permitted
| my code to be copied or otherwise used by any other person, nor have I
| copied, modified, or otherwise used programs created by others. I acknowledge
| that any violation of the above terms will be treated as academic dishonesty.
+=============================================================================*/