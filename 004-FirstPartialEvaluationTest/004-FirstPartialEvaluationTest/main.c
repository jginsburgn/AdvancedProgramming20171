/*
	Name: Jonathan Ginsburg Nagar
	ID number: A01021617
 
 TC2025 Advanced Programming
 First Partial Evaluation
 Evaluates:
 - Pointers
 - Dynamic memory
 - Process management
 - Inter Process Communication
 
 Program to test concentration counting exercise
 
 The problem:
 There is an exercise to develop concentration, where two numbers are chosen,
 the first will count up from 0, and the second counts down from 100.
 The player must keep counting in his head, adding and substracting each time
 from both sides. Example, in the range 0-100, adding 3 and substracting 4:
 3 - 96
 6 - 92
 9 - 88
 12 - 84
 15 - 80
 etc.
 
 You must write a program that can do this calculations, but the increment
 must be done in one process and the decrement in a different process.
 The parent will be the one who decides when to stop, so the two processes
 must keep constant communication. The child will send the updated value
 to the parent, and the parent will reply with a boolean value indicating
 whether the child should continue or it has finished already.
 The program will ask the user for the size of the range, and the amounts
 to add and substract
 
 You must show visually the progress of these counts, by using a dynamically
 sized string, initially filled with spaces. As the counts progress, the
 string will be filled with the '\' character from left to right, and with
 the '/' character from right to left. If the two counts intersect, you
 should write the 'X' character instead.
 Example, in the range 0-20, adding 4 and substracting 2
 
 4 -   18	\\\\              //
 8 -   16	\\\\\\\\        ////
 12 -   14	\\\\\\\\\\\\  //////
 16 -   12	\\\\\\\\\\\\XXXX////
 20 -   10	\\\\\\\\\\XXXXXXXXXX
 20 -    8	\\\\\\\\XXXXXXXXXXXX
 20 -    6	\\\\\\XXXXXXXXXXXXXX
 20 -    4	\\\\XXXXXXXXXXXXXXXX
 20 -    2	\\XXXXXXXXXXXXXXXXXX
 20 -    0	XXXXXXXXXXXXXXXXXXXX
 
	You must use pointer arithmetic to modify the string. Index notation is not
	allowed for this part of the test.
 
 QUESTIONS:
 Complete the following three functions to make the program work:
 - countUp
 - countDown
 - drawLine
 The description of what each does is at the end of this file.
 Write your functions there
 
 HINTS:
 - Write your name in this comment and in the "main" function
 - Read all of the existing code before answering anything. This will help you
 understand what needs to be done and how to do it
 - Keep calm. Think what you have to do before typing code
 - Write clean code and use correct indentation
 - Add comments to your code
 - You can add any other functions that you need to make your code more organized
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include <errno.h>
#include <signal.h>
#include <math.h>
#include "../../Libraries/JG.h"
#include <fcntl.h>

//#define BUFFER_SIZE 20

///// FUNCTION DECLARATIONS /////
void createProcess();
void openPipe(int pipe_channel[]);
void preparePipes(int pipe_out[], int pipe_in[]);
void closePipes(int pipe_out[], int pipe_in[]);
void countUp(int pipe_out[], int pipe_in[]);
void countDown(int pipe_out[], int pipe_in[]);
void drawLine(char * line, int range, int count_up, int count_down);

//Jonathan's Additional Functions
int numberOfDecimalCharactersInInt(const int i); //Returns the number of digits in -i-. E.g.: f(45)=2;
void appendChar(char ** str, const char ch); //Appends a character, -ch-, to a null-terminated string, -*str-, of exact dynamic memory size, i.e. performs realloc. If -*str- is null, -*str- is treated to be the null-terminated empty string, i.e.: "".
char * readFromPipe(const int fd); //Returns a dynamically allocated, null-terminated, exact-size string read from the pipe referred to by the file descriptor -fd-, or null if something failed.
char * intToString(const int i); //Returns a dynamically allocated, null-terminated, exact-size string with the decimal representation of integer -i-.
void modifyLine(char ** line, const int upperLimit, const int currentUpper, const int currentLower); //Sets the characters of -*line- to match the visual representation described in the instructions, with the new values -upperLimit-, -currentUpper-, and -currentLower-.

int main()
{
    printf("\n=== PROGRAM FOR CONCENTRATION EXERCISES ===\n");
    printf("By: Jonathan Ginsburg Nagar\n\n");
    
    createProcess();
    
    return 0;
}

///// FUNCTION DEFINITIONS /////

// Function to create the child process and declare the pipe variables
void createProcess()
{
    pid_t pid;
    // Variables for the pipes, indicating the direction of communication
    int pipe_parent_child[2];
    int pipe_child_parent[2];
    
    // Open the two channels
    openPipe(pipe_parent_child);
    openPipe(pipe_child_parent);
    
    // Create only one fork, the child will do the calculations
    pid = fork();
    
    // Parent process
    if (pid > 0)
    {
        // Open the corresponding pipes
        preparePipes(pipe_parent_child, pipe_child_parent);
        // Start the loop to count upwards from 0
        countUp(pipe_parent_child, pipe_child_parent);
        // Close the pipes
        closePipes(pipe_parent_child, pipe_child_parent);
        printf("Parent finishing\n");
    }
    // Child process
    else if (pid == 0)
    {
        // Open the corresponding pipes
        preparePipes(pipe_child_parent, pipe_parent_child);
        // Start the loop to count downwards from the range selected by the user
        countDown(pipe_child_parent, pipe_parent_child);
        // Close the pipes
        closePipes(pipe_child_parent, pipe_parent_child);
        printf("Child finishing\n");
    }
    // No process created
    else
    {
        perror("Unable to create a new process\n");
        exit(EXIT_FAILURE);
    }
}

// Verify the opening of a commuication pipe
void openPipe(int pipe_channel[])
{
    if (pipe(pipe_channel) == -1)
    {
        perror("Unable to create pipe\n");
        exit(EXIT_FAILURE);
    }
}

// Close the pipe directions that are not necessary
// Receive the file descriptors
void preparePipes(int pipe_out[], int pipe_in[])
{
    // Close the unwanted channels
    close(pipe_in[1]);
    close(pipe_out[0]);
}

// Close the remaining pipes
// Receive the file descriptors
void closePipes(int pipe_out[], int pipe_in[])
{
    // Close the file descriptors
    close(pipe_in[0]);
    close(pipe_out[1]);
}

// Main loop for the parent process, does the addition.
// It asks the user for the necessary inputs (range, increment, decrement)
// It passes initiates its variables and passes the variables range and
//  decrement to the child
// Then begins the main loop, where the two processes communicate the
//  updated values
// Receive: the arrays with the file descriptors for input and output
// Return: nothing
void countUp(int pipe_out[], int pipe_in[])
{
    //kill(getpid(), SIGSTOP); //Used for attaching debugger
    //Name fd's for easy reference
    int in = *pipe_in;
    int out = *(pipe_out + 1);
    
    //Range limits
    int upperLimit;
    int lowerLimit = 0;
    
    //Steps
    int upperStep;
    int lowerStep;
    
    //Read values from user
    printf("Enter the upper limit: ");
    scanf("%i", &upperLimit);
    printf("Enter the absolute value to subtract from upper limit: ");
    scanf("%i", &upperStep);
    printf("Enter the absolute value to add from lower limit: ");
    scanf("%i", &lowerStep);
    
    //Convert to character strings
    char * upperLimitStr = intToString(upperLimit);
    appendChar(&upperLimitStr, '\n');
    
    char * upperStepStr = intToString(upperStep);
    appendChar(&upperStepStr, '\n');
    
    //Send range and decrement to child process
    write(out, upperLimitStr, strlen(upperLimitStr));
    write(out, upperStepStr, strlen(upperStepStr));
    
    //Release dynamically allocated character strings of range and decrement
    free(upperLimitStr);
    free(upperStepStr);
    
    //Create working variables
    char * line = malloc(sizeof(char) * (upperLimit + 1));
    memset(line, ' ', upperLimit);
    printf("%s\n", line);
    int currentUpper = upperLimit;
    int currentLower = lowerLimit;
    
    char * trueStr = "true\n";
    char * falseStr = "false\n";
    
    //Start IPC
    char * input;
    //Main loop to communicate, operate and print if there are more steps to do
    while (currentUpper > lowerLimit || currentLower < upperLimit) {
        //Get, from child, new current upper if it is not past the lower limit
        if (currentUpper > lowerLimit) {
            write(out, trueStr, strlen(trueStr));
            char * newCurrentUpper = readFromPipe(in);
            currentUpper = atoi(newCurrentUpper);
            free(newCurrentUpper);
        }
        //Calculate new current lower if it is not past the upper limit
        if (currentLower < upperLimit) {
            currentLower += lowerStep;
        }
        //Modify line character string as needed
        modifyLine(&line, upperLimit, currentUpper, currentLower);
        
        //Show results to the user
        drawLine(line, upperLimit, currentLower, currentUpper);
    }
    write(out, falseStr, strlen(falseStr));
    
    //Free printable line
    free(line);
}

// Loop for the child process to update the decrement
// It gets from the parent the values for the range and the decrement
// It then loops, updating the count and listening for a reply from
//  the parent, indicating whether it has finished or not.
// Receive: the arrays with the file descriptors for input and output
// Return: nothing
void countDown(int pipe_out[], int pipe_in[])
{
    //kill(getpid(), SIGSTOP); //Used for attaching debugger
    close(STDIN_FILENO); //The bash only holds to the stdin of the initial process. This is useless.
    
    //Name fd's for easy reference
    int in = *pipe_in;
    int out = *(pipe_out + 1);
    
    //Read values from parent process
    char * upperLimitStr = readFromPipe(in);
    char * upperStepStr = readFromPipe(in);
    
    //Convert them to ints
    int upperLimit = atoi(upperLimitStr);
    int upperStep = atoi(upperStepStr);
    
    //Release dynamic memory
    free(upperLimitStr);
    free(upperStepStr);
    
    //Set current working variable
    int currentUpper = upperLimit;
    
    //Start IPC
    while (1) {
        //Get communication from parent
        char * pipeInput = readFromPipe(in);
        if (pipeInput == NULL) {
            continue;
        }
        else {
            int c = strcmp(pipeInput, "false");
            if (c == 0) { //Stop IPC and end child
                free(pipeInput);
                break;
            }
            else {
                currentUpper -= upperStep;
                char * currentUpperStr = intToString(currentUpper);
                appendChar(&currentUpperStr, '\n');
                write(out, currentUpperStr, strlen(currentUpperStr));
                free(currentUpperStr);
                free(pipeInput);
            }
        }
    }
}

// Draw the line showing the progress of the counts
// Called at every loop
// Will modify the string, adding '\' characters from the left
//  and '/' characters fromt he right. 'X' characters are used
//  when the counts intersect.
// Use only pointer arithmetic to modify the string
// Receives: the string, the size, and the counters for left and right
// Return: nothing
void drawLine(char * line, int range, int count_up, int count_down)
{
    printf("%i\t%i\t%s\n", count_up, count_down, line);
}


//Jonathan's functions
int numberOfDecimalCharactersInInt(const int i) {
    //Account for extreme case when -i- equals zero.
    if (i == 0) {
        return 1;
    }
    int power = 0;
    while ((int)pow(10, power) <= abs(i)) {
        ++power;
    }
    if (i < 0) {
        power++; //Consider minus sign
    }
    return power;
}

void appendChar(char ** str, const char ch) {
    //If there is no string in -*str- create one with the single char -ch- and the null byte.
    if (*str == NULL) {
        *str = malloc(sizeof(char) * 2);
        (*str)[0] = ch;
        (*str)[1] = '\0';
    }
    else {
        int oldLength = (int)strlen(*str);
        *str = realloc(*str, sizeof(char) * (oldLength + 2));
        (*str)[oldLength] = ch;
        (*str)[oldLength + 1] = '\0';
    }
}

char * readFromPipe(const int fd) {
    //Create a return variable
    char * retVal = NULL;
    char inputChar;
    //Digest one byte by one byte
    int readChars = (int)read(fd, &inputChar, sizeof(char));
    while (readChars >= 0 && inputChar != '\n') {
        if (readChars == sizeof(char)) {
            appendChar(&retVal, inputChar);
        }
        readChars = (int)read(fd, &inputChar, sizeof(char));
    }
    if (readChars == -1) {
        fprintf(stderr, "[C] Errno: %i, Error description: %s.\n", errno, strerror(errno));
        return NULL;
    }
    return retVal;
}

char * intToString(const int i) {
    int nodcii = numberOfDecimalCharactersInInt(i);
    char * nodciiStr = malloc(sizeof(char) * (nodcii + 1));
    sprintf(nodciiStr, "%i", i);
    nodciiStr[nodcii] = '\0';
    return nodciiStr;
}

void modifyLine(char ** line, const int upperLimit, const int currentUpper, const int currentLower) {
    //Get safe current upper and current lower variables in case it is below zero or above upper limit, respectively
    int safeCurrentUpper = currentUpper <= 0 ? 0 : currentUpper;
    int safeCurrentLower = currentLower >= upperLimit ? upperLimit : currentLower;
    
    //Modify from low to high
    for (int i = upperLimit - 1; i > safeCurrentUpper - 1; --i) {
        if ((*line)[i] != ' ' && (*line)[i] != '/') {
            (*line)[i] = 'X';
        }
        else {
            (*line)[i] = '/';
        }
    }
    //Modify from high to low
    for (int i = 0; i <= safeCurrentLower - 1; ++i) {
        if ((*line)[i] != ' ' && (*line)[i] != '\\') {
            (*line)[i] = 'X';
        }
        else {
            (*line)[i] = '\\';
        }
    }
}
