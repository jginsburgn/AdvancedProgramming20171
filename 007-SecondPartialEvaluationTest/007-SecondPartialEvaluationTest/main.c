//
//  main.c
//  007-SecondPartialEvaluationTest
//
//  Created by Jonathan Ginsburg on 3/29/17.
//  Copyright © 2017 Jonathan Ginsburg. All rights reserved.
//

#include "../../Libraries/JG.h"
#include <pthread.h>
#include <fcntl.h>
#include <unistd.h>

//Global configuration constants
#define REEL_COUNT 5
#define CHARACTER_COUNT 5
#define INITIAL_REEL_CHARACTER 'A'
#define WINDOW_INDEX 1 //This index indicates which character of the reels are to be played by and shown to the user

//Global variables

char * aid_reel; //Aid array reel
char * * reels; //Array of reels

bool should_stop_game;

//Mutexes
pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER; //Temporary aid array reel mutex


void shiftReel(char * reel, int shift_count) { //Shifts reel's characters by shift_count
    
    //Arrange characters of reel into aid_reel after shifting
    for (int i = 0; i < CHARACTER_COUNT; ++i) {
        aid_reel[(i + shift_count) % CHARACTER_COUNT] = reel[i];
    }
    
    //Put them back in the original array
    for (int i = 0; i < CHARACTER_COUNT; ++i) {
        reel[i] = aid_reel[i];
    }
}

void fillReel(char * reel) { //Fills reel with characters from (IRC) INITIAL_REEL_CHARACTER to IRC + (CC) CHARACTER_COUNT - 1, in their corresponding position index (i.e. reel[0] <- IRC, reel[1] <- IRC + 1, ...)
    for (int i = 0; i < CHARACTER_COUNT; ++i) {
        reel[i] = INITIAL_REEL_CHARACTER + i;
    }
}

void createAndFillReels(char * * * reels) { //Allocates dynamic memory for the array of reels and for each reel and fills each reel
    *reels = malloc(sizeof(char *) * REEL_COUNT);
    for (int i = 0; i < REEL_COUNT; ++i) {
        (*reels)[i] = malloc(sizeof(char) * CHARACTER_COUNT);
        fillReel((*reels)[i]);
    }
}

void destroyReels(char * * * reels) { //Frees dynamic memory associated with each reel and that of the array of reels and sets the reels pointer to NULL
    for (int i = 0; i < REEL_COUNT; ++i) {
        free((*reels)[i]);
    }
    free(*reels);
    (*reels) = NULL;
}

void * reel(void * arg) { //Each reel thread's execution function. It shifts characters in its reel when it holds the mutex for the aid array, then releases it, and tries to get it again to repeat
    
    //Get the index of the reel in the reels array that corresponds to this thread, passed as a dynamically allocated int that needs to be freed by this thread
    int * reel_index = (int *)(arg);
    
    //Prepare a shift_count based on the reel_index of this specific thread; add one to avoid zero shift_count which would render the shift operation idempotent
    int shift_count = *reel_index + 1;
    
    //Make sure that the shift_count is not a multiple of CHARACTER_COUNT, because when it is the shift operation is idempotent
    if (shift_count % CHARACTER_COUNT == 0) {
        if (abs(CHARACTER_COUNT) < 2) {
            puts("CHARACTER_COUNT found to be less than 2, which does not allow for slot machine playing. Please modify and rebuild to play.");
            exit(EXIT_FAILURE);
        }
        else {
            ++shift_count;
        }
    }
    
    while (!should_stop_game) {
        //Lock mutex
        if (pthread_mutex_lock(&mutex) != 0) {
            puts("Error locking mutex");
            exit(EXIT_FAILURE);
        }
        
        shiftReel(reels[*reel_index], shift_count);
        
        //Unlock mutex
        if (pthread_mutex_unlock(&mutex) != 0) {
            puts("Error unlocking mutex");
            exit(EXIT_FAILURE);
        }
    }
    
    //Free the dynamically allocated index of this thread's reel in the reels array
    free(reel_index);
    
    return 0;
}

void printWindow() { //Prints the characters in the reels at position indicated by WINDOW_INDEX
    for (int i = 0; i < REEL_COUNT; ++i) {
        putchar(reels[i][WINDOW_INDEX]);
        if (i < REEL_COUNT - 1) {
            printf(" | ");
        }
    }
    fflush(stdout);
}

void clearWindow() { //Clears the row of characters printed by a call to printWindow() by sending backspace characters to stdout
    
    printf("\33[2K\r");
}

bool didWin() { //Indicates if the combination of reel characters at the window is winner (i.e. all characters are the same).
    
    //The first character in the window, to be compared with the rest of them in the window
    char first_character = reels[0][WINDOW_INDEX];
    
    //Check for mismatches and report if one is found
    for (int i = 1; i < REEL_COUNT; ++i) {
        if (first_character != reels[i][WINDOW_INDEX]) {
            return false;
        }
    }
    
    return true;
}

void * ui(void * arg) { //UI thread's execution function. It prints the reel window and checks for???
    //Make reading from stdin non-blocking
    fcntl(STDIN_FILENO, F_SETFL, O_NONBLOCK);
    
    while (!should_stop_game) {
        //Lock mutex
        if (pthread_mutex_lock(&mutex) != 0) {
            puts("Error locking mutex");
            exit(EXIT_FAILURE);
        }
        
        //Check for new character
        char input = getchar();
        
        //Update user on reel status
        clearWindow();
        printWindow();
        
        if (input != EOF && input != -1 /*This last comparison is for the case when EOF is different from -1 and for handling unexistant input in the nonblocking call of getchar (see fcntl(2) O_NONBLOCK)*/) {
            if (input == 'e') { //Check if the game should stop
                should_stop_game = true;
            }
            else if (input == '\n') { //The user pulls the lever
                if (didWin()) { //Check if the user won
                    puts(": You won.");
                }
                else {
                    puts(": You lose.");
                }
            }
            else { //Tell user that the character he wrote is unrecognized
                printf(" Unrecognized character: %c\n.", input);
            }
        }
        
        //Unlock mutex
        if (pthread_mutex_unlock(&mutex) != 0) {
            puts("Error unlocking mutex");
            exit(EXIT_FAILURE);
        }
    }
    
    return 0;
}

int main(int argc, const char * argv[]) {
    //Print welcome message and instructions
    puts("\nWelcome to the slot machine by Jonathan Ginsburg (A01021617), finished on March the 30th of 2017 at 00:18 approximately. It is a project for evaluating the second partial of the Advanced Programming class. This program brings together the concepts of threads and mutexes:\n\nEach reel is shifted by an independent thread. Yet another thread, the UI one, is in charge of printing the reels' window and reading input from the user. Furthermore, each reel is represented by a character array, and another character array is used as aid during the shift operation; therefore, a mutex is used because all these arrays are shared among threads.\n\nTo pull the lever of the machine, press enter. To exit, press E followed by enter.\n");
    
    //Set stop game boolean controller to false, for game to proceed
    should_stop_game = false;
    
    //Prepare reels and aid reel
    aid_reel = malloc(sizeof(char) * CHARACTER_COUNT);
    createAndFillReels(&reels);
    
    //Create reel threads
    pthread_t thread_ids[REEL_COUNT];
    for (int i = 0; i < REEL_COUNT; ++i) {
        int * threads_reel_index = malloc(sizeof(int));
        *threads_reel_index = i;
        if (pthread_create(thread_ids + i, 0, &reel, threads_reel_index) != 0) {
            puts("Error during thread creation");
            exit(EXIT_FAILURE);
        }
    }
    
    //Create UI thread
    pthread_t ui_thread_id;
    if (pthread_create(&ui_thread_id, 0, &ui, 0) != 0) {
        puts("Error during thread creation");
        exit(EXIT_FAILURE);
    }
    
    //Join UI thread
    if (pthread_join(ui_thread_id, 0) != 0) {
        puts("Error during thread joining");
        exit(EXIT_FAILURE);
    }
    
    //Join reel threads
    for (int i = 0; i < REEL_COUNT; ++i) {
        if (pthread_join(thread_ids[i], 0) != 0) {
            puts("Error during thread joining");
            exit(EXIT_FAILURE);
        }
    }
    
    //Free reels and aid reel
    free(aid_reel);
    destroyReels(&reels);
    
    printf("\nThanks for using this slot machine.\n");
    
    return 0;
}
