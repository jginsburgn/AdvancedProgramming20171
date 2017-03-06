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

//Returns the contents of a file specified by path in a new string
char * readFile(const char * path) {
    FILE * file = fopen(path, "r"); //Open file
    fseek(file, 0, SEEK_END); //Position at end
    long length = ftell(file); //Get length of file
    fseek(file, 0, SEEK_SET); //Reposition at beginning
    char * contents = malloc(sizeof(char)*(length + 1)); //Create a string of needed size (the +1 is for the final null-character)
    char ch;
    int i = 0;
    while ((ch = fgetc(file)) != EOF) { //Read until end
        contents[i++] = ch; //Put each character in previously created string
    }
    fclose(file); //Close file
    contents[i] = '\0'; //Add null-character for correct termination of string
    return contents;
}

bool writeFile(const char * path, const char * string) {
    FILE * fd = fopen(path, "w"); //Open file for writing
    fputs(string, fd); //Write string
    fclose(fd); //Close file
    return true;
}

unsigned char * readFileBinary(const char * path, int * readSize) {
    FILE * fd = fopen(path, "r"); //Open file
    unsigned char * buffer = malloc(sizeof(unsigned char) * BUFFER_SIZE); //Create a buffer
    int currentSize = BUFFER_SIZE; //Create a variable that holds the buffer size
    int readObjectsCount = (int)fread(buffer, sizeof(unsigned char), BUFFER_SIZE, fd); //Fill the buffer with as much information from the file as possible
    while (readObjectsCount == BUFFER_SIZE) { //If the buffer was completely filled (i.e. no space was left) increase the size and read again more bytes
        currentSize += BUFFER_SIZE; //Keep the count of the new size; increase it by BUFFER_SIZE constant
        buffer = realloc(buffer, currentSize); //Increase the size of the buffer
        int bufferOffset = currentSize - BUFFER_SIZE; //Calculate where in the buffer to put the new data
        readObjectsCount = (int)fread(buffer + bufferOffset, sizeof(unsigned char), BUFFER_SIZE, fd); //Read more data from the file and store it in the buffer
    }
    int countDifference = BUFFER_SIZE - readObjectsCount; //Check what was the space left blank in the buffer
    currentSize -= countDifference; //Keep count of how big the buffer needs to be to not waste space
    buffer = realloc(buffer, currentSize); //Resize buffer to just fit without wasting space
    if (readSize) { //If readsize pointer was provided fill it with info
        * readSize = currentSize; //Set the read count of bytes
    }
    return buffer;
}

void writeFileBinary(const char * path, const unsigned char * target, const int size) {
    FILE * fd = fopen(path, "w"); //Open file
    fwrite(target, sizeof(unsigned char), size, fd); //Write -size- bytes
    fclose(fd); //Close file
}

void printBytes(const unsigned char * buffer, const int size, const base print_base) {
    char * formatSpecifier = ""; //Format specifier variable
    //Build variable of format according to base chosen >>>
    switch (print_base) {
            case decimal:
            formatSpecifier = "%03hhu ";
            break;
            case hexadecimal:
            formatSpecifier = "%02hhX ";
            break;
            case octal:
            formatSpecifier = "%03hho ";
            break;
    }
    // <<<
    // Print -size- bytes in buffer and flush >>>
    for (int i = 0; i < size; ++i) {
        printf(formatSpecifier, buffer[i]);
    }
    fflush(stdout);
    // <<<
}

int findByte(const unsigned char * target, const int size, const unsigned char byte) {
    int currentPosition = 0; //Variable to keep track of the position inspected
    while (currentPosition < size) { //Keep checking till the end
        unsigned char currentByte = target[currentPosition]; //Get the byte at the current position
        if (currentByte == byte) { //If byte is found return current position
            return currentPosition;
        }
        ++currentPosition; //Else, keep checking
    }
    return -1; //If not found, return -1
}

int findWhitespaceByte(const unsigned char * target, const int size) {
    int blank = findByte(target, size, ' ');
    int tab = findByte(target, size, '\t');
    int cr = findByte(target, size, 13);
    int lf = findByte(target, size, 10);
    blank = blank == -1 ? __INT_MAX__ : blank;
    tab = tab == -1 ? __INT_MAX__ : tab;
    cr = cr == -1 ? __INT_MAX__ : cr;
    lf = lf == -1 ? __INT_MAX__ : lf;
    int retVal = __INT_MAX__;
    if (blank < retVal) retVal = blank;
    if (tab < retVal) retVal = tab;
    if (cr < retVal) retVal = cr;
    if (lf < retVal) retVal = lf;
    if (retVal == __INT_MAX__) retVal = -1;
    return retVal;
}

char * substring(const char * target, const int from, const int to) {
    int substringSize = to - from + 1; //Calculate the needed size for the new string
    char * retVal = malloc(sizeof(char) * (substringSize + 1)); //Allocate new string
    memcpy(retVal, target + from, substringSize); //Copy bytes
    retVal[substringSize] = '\0';
    return retVal;
}

bool seeded = false;
float getRandomNumber() {
    if (!seeded) {
        srand(time(0));
        seeded = true;
    }
    return rand()/(float)RAND_MAX;
}

int getRandomNumberLimited(const int limit) {
    //The following instruction uses limit + 1 because when casting into int, the result is the same as flooring and then getting a random equal to limit would be very unlikely, much more unlikely than any other integer comprised between 0 and limit, excluding limit. Later, the possibility of getting a random equal to limit + 1 is eliminated and the possibility of this happening is negligible.
    float randomFloat = getRandomNumber() * (limit + 1);
    int randomInt = (int)randomFloat;
    if (randomInt == (limit + 1)) {
        randomInt = limit;
    }
    return randomInt;
}
