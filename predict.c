/*
* Author: Lorenzo Wyman
* Course: CS2
* Professor Ali
* Assignment: Smart Typing System 
* Date: April 6, 2025
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_WORD_CHARS 100

typedef struct TrieNode_s {
    struct TrieNode_s *children[26];
    int wordEnd;
    int frequency;
    int maxFrequency;
    char mostFrequent[MAX_WORD_CHARS];
} TrieNode;

TrieNode* createNode() {

    TrieNode *node = malloc(sizeof(TrieNode)); //dma for trie nodes

    if (node != NULL) {

        node -> frequency = 0;
        node -> maxFrequency = 0;
        node -> mostFrequent[0] = '\0'; //bcuz its a char this is how it is intitialized
        node->wordEnd = 0;

        for (int x = 0; x < 26; ++x) { //if it werent array, then wed malloc
            node -> children[x] = NULL; //same as before
        }
    }

    return node;
}

void insertWord(TrieNode* root, const char* word, int frequency){

    if (root == NULL){
        //printf("this is null :/\n");
        return;
    }

    int length = strlen(word); //need to get the length of the string
    TrieNode *temp = root; //bcuz we cant move root

    for (int level = 0; level < length; ++level){ //traversing nodes
        int index = word[level] - 'a'; //we are going to subtract from the ascii value of a to then search for wtv letter is there. 

        if (index < 0 || index >= 26){
            return;
        }
        if (temp -> children[index] == NULL){ //if it is there then dont make dma for it, if not then do. making node for missing letter
            temp -> children[index] = createNode();
        }
        temp = temp -> children[index]; //when not null
    }

    temp -> wordEnd = 1; //the word is at the end when it turns to 1 from 0
    temp -> frequency = frequency;
    temp = root;

    for (int level = 0; level < length; ++level){ //uodating mostFrequent

        int index = word[level] - 'a';
        temp = temp -> children[index];
        
        if (frequency > temp -> maxFrequency){ //change or update the word if it has a higher frequency 
            temp -> maxFrequency = frequency;
            strcpy(temp -> mostFrequent, word);
        }
    }
}

char* search(TrieNode *root, char *string){

    if (root == NULL){
        return NULL;
    }

    int length = strlen(string);

    TrieNode *temp = root;

    for (int level = 0; level < length; ++level) {

        int index = string[level] - 'a';
        //If a child node does not exist at the expected index, return false (0)
        if (temp -> children[index] == NULL){ //from the notes in class
            return NULL; //the word aint found
        }
        temp = temp -> children[index]; //move to the next node
    }

    if (temp -> wordEnd){ //if the word exists

        char *result = malloc(strlen(string) + 1);

        if (result){
            strcpy(result, string);
        }
        return result;

    }

    return NULL; //if the word does not exist or copy of word exists alr
}

char* queryPrefix(TrieNode *root, const char *prefix) {

    if (root == NULL) {
        return strdup("unknown word"); //return dma copy of unkown word, this fixed blank output and reduced function size
    }

    int length = strlen(prefix);
    TrieNode *temp = root;

    for (int level = 0; level < length; ++level) {
        int index = prefix[level] - 'a';

        if (temp -> children[index] == NULL) {
            return strdup("unknown word");
        }
        temp = temp -> children[index];
    }

    if (temp -> mostFrequent[0] != '\0') { //if the char (word) is not equal to nothing
        return strdup(temp -> mostFrequent);
    } else if (temp -> wordEnd) {
        return strdup(prefix);
    }

    return strdup("unknown word"); //strup returns dma copy of unknown word
}

int main() {

    TrieNode *root = createNode();
    if (root == NULL){
        return 1;
    }

    int nCommands;

    scanf("%d", &nCommands);
    //printf("Number of commands read: %d\n", nCommands);

    while (getchar() != '\n'); //clears buffer that caused words to not output

    char **wordResults = malloc(sizeof(char*) * nCommands); //holds the results of the words until we need them in an arr
    if (wordResults == NULL){
        printf("mem allocation failed for 'wordResults arr\n");
        return 1;
    }

    int wordResultIndex = 0;

    for (int i = 0; i < nCommands; ++i){

        int action;
        char word[MAX_WORD_CHARS];
        int frequency = 0; 

        scanf("%d %s", &action, word);
        //printf("action read: %d, word/prefix read: %s,", action, word);

        if (action == 1){

            scanf("%d", &frequency);
            //printf(" frequency read: %d\n", frequency);
            insertWord(root, word, frequency);

        } else if (action == 2){

            char *result = queryPrefix(root, word);
            
            wordResults[wordResultIndex] = result; // store the result to print outside of the loop after the things have been inputted
            wordResultIndex++;

            //printf("\nquery result stored: \n");
        }

        while (getchar() != '\n');
    }

    //printf("\nquery results:\n");

    printf("\n");
    for (int i = 0; i < wordResultIndex; ++i){
        printf("%s\n", wordResults[i]);
        free(wordResults[i]);
    }
    free(wordResults);

    return 0;
}
