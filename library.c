/*
  Name: Lorenzo Wyman
  Date: 1/27/2024
  UCFID: 5464079
  Class: COP3502
  Professor Mohammed Ali
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_CHARS 100 // Defined a variable for the maximum amount of characters (for author name and book titles).

typedef struct book {
    
    char *bookTitle;
    char *authorName;
    int bookYear;

} Book;

typedef struct library { // Second struct that allowed me to reference the elements of the first struct.

    Book *books;
    int numOfBooks;

} Library;

void *getUserInput(Library *library){ 

int inputFlag = 0; // Initialized value for flag that tracks whether a positive or negative integer was entered.

    while (!inputFlag){
        
        printf("Enter the number of books: ");
        if (scanf("%d", &library -> numOfBooks) != 1 || library -> numOfBooks <= 0) {
            printf("Error! Please enter a positive NUMBER or INTEGER greater than 0.\n");
            while (getchar() != '\n'); // Removes the new line character (extra space).
        } else {
            inputFlag = 1; // Changes to 1 so the program can continue.
        } 
    }

    library -> books = (Book*)malloc(sizeof(Book) * library -> numOfBooks);
    if (library -> books == NULL){
        printf("Memory Allocation Failed.");
        return NULL;
    }

    for (int i = 0; i < library -> numOfBooks; i++){

        // These allocations reference the struct and then the variable where we will hold the string entered from the user.
        library -> books[i].bookTitle = (char*)malloc(sizeof(char) * MAX_CHARS);
        library -> books[i].authorName = (char*)malloc(sizeof(char) * MAX_CHARS); 
        
        if (library -> books[i].bookTitle == NULL){ 
            printf("Memory Allocation Failed for bookTitle\n");
            return NULL;
        }
        // Split these up because when they were together it caused a failure.
        if ( library -> books[i].authorName == NULL){
            printf("Memory Allocation Failed for authorName");
            return NULL;
        }

        printf("-------------------------\n");
        printf("Enter details for book %d: \n", i + 1);

        printf("Title: ");
        getchar();
        fgets(library -> books[i].bookTitle, MAX_CHARS, stdin);
        library -> books[i].bookTitle[strcspn(library -> books[i].bookTitle, "\n")] = '\0';

        if (strlen(library -> books[i].bookTitle) == 0){
            strcpy(library -> books[i].bookTitle,"--No Title Entered--");
        }
        
        printf("Author: ");
        fgets(library -> books[i].authorName, MAX_CHARS, stdin);
        library -> books[i].authorName[strcspn(library -> books[i].authorName, "\n")] = '\0'; 

        if (strlen(library -> books[i].authorName) == 0){
            strcpy(library -> books[i].authorName,"--No Author Name Entered--");
        }

        printf("Year: ");
        scanf("%d", &library -> books[i].bookYear);
        if(library -> books[i].bookYear < 0){
            printf("Error! Year can only be positive integers!\n");
        } 
    }
}

void librarySort(Library *library){

    int temporaryBook; // Holds a temporary spot for the books that are going to be sorted along with the year.

    // Sorting the books by year.
    for (int i = 0; i < library -> numOfBooks - 1; i++){
        for (int j = 0; j < library -> numOfBooks - i - 1; j++){
            if(library -> books[j].bookYear > library -> books[j + 1].bookYear){

                // This sorts the entire entry with the year (author name and title).
                Book temporaryBook = library -> books[j]; 
                library -> books[j] = library -> books[j + 1];
                library -> books[j + 1] = temporaryBook;
            }
        }
    }
}

void printLibrary(Library *library){

    // Call the sorting function here; When I previously did not, the program was not sorting at all.
    librarySort(library); 

    printf("\nLibrary Inventory (Sorted by Year of Publication): \n");

    for(int i = 0; i < library -> numOfBooks; i++){
        printf("-----------------------\n");
        printf("%d. Title: %s\n", i + 1, library -> books[i].bookTitle);
        printf("   Author: %s\n", library -> books[i].authorName);
        printf("   Year: %d\n",library -> books[i].bookYear);
    }
}

int main(){

    Library library;
    library.numOfBooks = 0; // Just initialized.

    printf("--------------------------\n");
    printf("Welcome to the library!\n");

    getUserInput(&library);
    printLibrary(&library);
    librarySort(&library);

    // Freeing the memory that was used in the beginning of the program.
    for (int i = 0; i < library.numOfBooks; i++){ 

        free(library.books[i].bookTitle);
        free(library.books[i].authorName);
    }
    free(library.books);

    return 0;
}