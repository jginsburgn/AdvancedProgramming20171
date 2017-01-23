//
//  JG.c
//  
//
//  Created by Jonathan Ginsburg on 1/22/17.
//
//

#include "JG.h"

//Reads a string from stdin until the untilChar is found (untilChar can be the end of file EOF signal). The returned string fits perfectly the read contents (uses exact memory space) and includes the terminating character; but, does not include the untilChar read.
char * readString(const char untilChar) {
    char * retVal = malloc(sizeof(char) * BUFFER_SIZE); //Allocates buffer for storing the read characters.
    int currentSize = BUFFER_SIZE; //Variable indicates the buffer's size.
    int nextPosition = 0; //Variable indicates the position in the buffer where the next character is to be put.
    int readCharacter = 0; //Variable contains the read character.
    readCharacter = getc(stdin); //Reads character from stdin.
    while (readCharacter != untilChar) { //Checks whether read file is EOF.
        retVal[nextPosition++] = readCharacter; //Puts read character in buffer.
        if (nextPosition == currentSize) { //Checks if buffer is about to overflow.
            currentSize += BUFFER_SIZE; //Calculates a new buffer size.
            retVal = realloc(retVal, sizeof(char) * currentSize); //Expands buffer.
        }
        readCharacter = getc(stdin); //Reads next character.
    } //Repeats.
    if (nextPosition == currentSize) { //Checks if buffer is about to overflow.
        currentSize += 1; //Calculates new buffer size.
        retVal = realloc(retVal, sizeof(char) * currentSize); //Expands buffer.
    }
    else if (nextPosition < currentSize) { //Checks if buffer has extra space.
        if (nextPosition + 1 != currentSize) { //Checks if buffer has more than one character left. Because if it is just one extra character then that is used for the terminating character and the buffer is returned.
            currentSize = nextPosition + 1; //Calculates new buffer size.
            retVal = realloc(retVal, sizeof(char) * currentSize); //Shrinks buffer.
        }
    }
    retVal[nextPosition] = '\0'; //Appends terminating character.
    return retVal; //Returns buffer.
}
