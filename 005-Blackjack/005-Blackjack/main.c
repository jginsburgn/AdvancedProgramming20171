//
//  main.c
//  005-Blackjack
//
//  Created by Jonathan Ginsburg on 3/4/17.
//  Copyright © 2017 Jonathan Ginsburg. All rights reserved.
//

#include "../../Libraries/JG.h"

#include "Card.h"
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>

#define MSSG_EXCH_BUF_SIZE 400

void client(const char * address, const char * port) {
    //Create socket
    int socket_fd = socket(PF_INET, SOCK_STREAM, 0);
    if (socket_fd == -1) {
        perror("Error in socket function call");
        return;
    }
    
    //Specify connection details
    struct sockaddr_in remote_address;
    remote_address.sin_family = AF_INET;
    remote_address.sin_addr.s_addr = inet_addr(address);
    remote_address.sin_port = htons(atoi(port));
    
    //Connect
    if (connect(socket_fd, (struct sockaddr *)&remote_address, sizeof(remote_address)) == -1) {
        perror("Error in connect function call");
        return;
    }
    
    //Start conversation
    FILE * socketFile = fdopen(socket_fd, "r+");
    char buffer[MSSG_EXCH_BUF_SIZE];
    while (true) {
        read(socket_fd, buffer, MSSG_EXCH_BUF_SIZE);
        char * action = substring(buffer, 0, 1);
        char * arguments = substring(buffer, 2, findByte((unsigned char *)buffer, MSSG_EXCH_BUF_SIZE, 0));
        
        //Main execution after parsing incoming message
        if (strcmp("A:", action) == 0) { //A: is for action
            if (strcmp("end", arguments) == 0) {
                puts("Finishing session.");
                free(action);
                free(arguments);
                fprintf(socketFile, "C:continue");
                fflush(socketFile);
                fclose(socketFile);
                return;
            }
        }
        else if (strcmp("D:", action) == 0) { //D: is for display
            puts(arguments);
            fprintf(socketFile, "C:continue");
            fflush(socketFile);
        }
        else if (strcmp("Q:", action) == 0) { //Q: is for querying user
            if (strcmp("int", arguments) == 0) {
                int userInput;
                printf(">> ");
                scanf(" %i", &userInput);
                fprintf(socketFile, "R:%i", userInput);
                fflush(socketFile);
            }
            else if (strcmp("char", arguments) == 0) {
                char userInput;
                printf(">> ");
                scanf(" %c", &userInput);
                fprintf(socketFile, "R:%c", userInput);
                fflush(socketFile);
            }
            else if (strcmp("float", arguments) == 0) {
                float userInput;
                printf(">> ");
                scanf(" %f", &userInput);
                fprintf(socketFile, "R:%f", userInput);
                fflush(socketFile);
            }
        }
        free(action);
        free(arguments);
        bzero(buffer, MSSG_EXCH_BUF_SIZE);
    }
}

void serverChild(int socket_fd) {
    //Display information about this child
    printf("Child's process ID: %i.\n", getpid());
    
    //Make useful variables and buffer
    FILE * socketFile = fdopen(socket_fd, "r+");
    char buffer[MSSG_EXCH_BUF_SIZE];
    
    //Begin blackjack session
    
    //Get available money of player
    float playersMoney;
    
    fprintf(socketFile, "D:Welcome to the table %i, how much money are you bringing? ", getpid());
    fflush(socketFile);
    read(socket_fd, buffer, MSSG_EXCH_BUF_SIZE);
    fprintf(socketFile, "Q:float");
    fflush(socketFile);
    bzero(buffer, MSSG_EXCH_BUF_SIZE);
    read(socket_fd, buffer, MSSG_EXCH_BUF_SIZE);
    sscanf(buffer, "R:%f", &playersMoney);
    fprintf(socketFile, "D:Ok, $%.2f.", playersMoney);
    fflush(socketFile);
    read(socket_fd, buffer, MSSG_EXCH_BUF_SIZE);
    
    //Begin main cycle
    struct Card * deck = getNewDeck();
    while (true) {
        //Ask what to do
        fprintf(socketFile, "D:Wanna bet or leave? [B,L]");
        fflush(socketFile);
        read(socket_fd, buffer, MSSG_EXCH_BUF_SIZE);
        fprintf(socketFile, "Q:char");
        fflush(socketFile);
        bzero(buffer, MSSG_EXCH_BUF_SIZE);
        read(socket_fd, buffer, MSSG_EXCH_BUF_SIZE);
        char inChar;
        sscanf(buffer, "R:%c", &inChar);
        if (inChar == 'B') {
            
            //Inform finances to user
            fprintf(socketFile, "D:Your current money is $%.2f.", playersMoney);
            fflush(socketFile);
            read(socket_fd, buffer, MSSG_EXCH_BUF_SIZE);
            
            //Get bet amount
            fprintf(socketFile, "D:How much do you want to bet?");
            fflush(socketFile);
            read(socket_fd, buffer, MSSG_EXCH_BUF_SIZE);
            fprintf(socketFile, "Q:float");
            fflush(socketFile);
            bzero(buffer, MSSG_EXCH_BUF_SIZE);
            read(socket_fd, buffer, MSSG_EXCH_BUF_SIZE);
            float betAmount;
            sscanf(buffer, "R:%f", &betAmount);
            playersMoney -= betAmount;
            fprintf(socketFile, "D:Ok, betting $%.2f.", betAmount);
            fflush(socketFile);
            read(socket_fd, buffer, MSSG_EXCH_BUF_SIZE);
            
            //Shuffle deck and create player and dealers hands
            shuffleDeck(deck);
            struct Card playersHand[DECK_SIZE];
            int playersHandSize = 0;
            struct Card dealersHand[DECK_SIZE];
            int dealersHandSize = 0;
            
            //Create an index of how many cards of the deck have been dealt
            int dealtCards = 0;
            
            //Deal first round of cards
            playersHand[playersHandSize++] = deck[dealtCards++];
            dealersHand[dealersHandSize++] = deck[dealtCards++];
            playersHand[playersHandSize++] = deck[dealtCards++];
            dealersHand[dealersHandSize++] = deck[dealtCards++];
            
            //Check for naturals or blackjacks
            if (getHandValue(playersHand, playersHandSize) == BLACK_JACK) {
                //User has got Blackjack
                if (getHandValue(dealersHand, dealersHandSize) == BLACK_JACK) {
                    //Naturals tie
                    
                    //Inform user
                    char * dealersHandDescription = getHandDescription(dealersHand, dealersHandSize, false);
                    char * playersHandDescription = getHandDescription(playersHand, playersHandSize, false);
                    fprintf(socketFile, "D:Hands:\nDEALER:\n%s\nYOU:\n%s\nIt is a tie.", dealersHandDescription, playersHandDescription);
                    fflush(socketFile);
                    read(socket_fd, buffer, MSSG_EXCH_BUF_SIZE);
                    free(dealersHandDescription);
                    free(playersHandDescription);
                    
                    //Return money to user
                    playersMoney += betAmount;
                }
                else {
                    //User Blackjack
                    
                    //Inform user
                    char * dealersHandDescription = getHandDescription(dealersHand, dealersHandSize, false);
                    char * playersHandDescription = getHandDescription(playersHand, playersHandSize, false);
                    fprintf(socketFile, "D:Hands:\nDEALER:\n%s\nYOU:\n%s\nCongratulations, it is your blackjack. Amount paid: $%.2f.", dealersHandDescription, playersHandDescription, betAmount * 1.5);
                    fflush(socketFile);
                    read(socket_fd, buffer, MSSG_EXCH_BUF_SIZE);
                    free(dealersHandDescription);
                    free(playersHandDescription);
                    
                    //Pay user
                    playersMoney += betAmount * 2.5;
                }
            }
            else {
                //User did not get Blackjack
                if (getHandValue(dealersHand, dealersHandSize) == BLACK_JACK) {
                    //Dealer's blackjack
                    
                    //Inform user
                    char * dealersHandDescription = getHandDescription(dealersHand, dealersHandSize, false);
                    char * playersHandDescription = getHandDescription(playersHand, playersHandSize, false);
                    fprintf(socketFile, "D:Hands:\nDEALER:\n%s\nYOU:\n%s\nDealer's blackjack. Amount retained: $%.2f.", dealersHandDescription, playersHandDescription, betAmount);
                    fflush(socketFile);
                    read(socket_fd, buffer, MSSG_EXCH_BUF_SIZE);
                    free(dealersHandDescription);
                    free(playersHandDescription);
                }
                else {
                    //No naturals
                    
                    while (getHandValue(playersHand, playersHandSize) < BLACK_JACK) {
                        //Aid variable for exiting loop
                        bool exitLoop = false;
                        
                        //Inform user of situation
                        char * dealersHandDescription = getHandDescription(dealersHand, dealersHandSize, true);
                        char * playersHandDescription = getHandDescription(playersHand, playersHandSize, false);
                        fprintf(socketFile, "D:Hands:\nDEALER:\n%s\nYOU:\n%s", dealersHandDescription, playersHandDescription);
                        fflush(socketFile);
                        read(socket_fd, buffer, MSSG_EXCH_BUF_SIZE);
                        free(dealersHandDescription);
                        free(playersHandDescription);
                        
                        //Ask user if he wants to hit or stand
                        fprintf(socketFile, "D:Do you want to hit or stand? [H,S]");
                        fflush(socketFile);
                        read(socket_fd, buffer, MSSG_EXCH_BUF_SIZE);
                        fprintf(socketFile, "Q:char");
                        fflush(socketFile);
                        bzero(buffer, MSSG_EXCH_BUF_SIZE);
                        read(socket_fd, buffer, MSSG_EXCH_BUF_SIZE);
                        sscanf(buffer, "R:%c", &inChar);
                        
                        //Execute petition
                        switch (inChar) {
                            case 'H':
                                playersHand[playersHandSize++] = deck[dealtCards++];
                                break;
                            case 'S':
                                exitLoop = true;
                                break;
                            default:
                                fprintf(socketFile, "D:Didn't quite get that.");
                                fflush(socketFile);
                                read(socket_fd, buffer, MSSG_EXCH_BUF_SIZE);
                                break;
                        }
                        
                        //Exit if user stood
                        if (exitLoop) {
                            break;
                        }
                    }
                    
                    if (getHandValue(playersHand, playersHandSize) > BLACK_JACK) {
                        
                        //Inform user of situation
                        char * dealersHandDescription = getHandDescription(dealersHand, dealersHandSize, true);
                        char * playersHandDescription = getHandDescription(playersHand, playersHandSize, false);
                        fprintf(socketFile, "D:Hands:\nDEALER:\n%s\nYOU:\n%s. You busted. Amount retained $%.2f.", dealersHandDescription, playersHandDescription, betAmount);
                        fflush(socketFile);
                        read(socket_fd, buffer, MSSG_EXCH_BUF_SIZE);
                        free(dealersHandDescription);
                        free(playersHandDescription);
                        
                    }
                    else {
                        //Dealer's turn
                        while (getHandValue(dealersHand, dealersHandSize) < MIN_DEALERS_HAND) {
                            //Dealer hits
                            dealersHand[dealersHandSize++] = deck[dealtCards++];
                        }
                        
                        if (getHandValue(dealersHand, dealersHandSize) > BLACK_JACK) {
                            //User wins
                            
                            //Inform user of situation
                            char * dealersHandDescription = getHandDescription(dealersHand, dealersHandSize, false);
                            char * playersHandDescription = getHandDescription(playersHand, playersHandSize, false);
                            fprintf(socketFile, "D:Hands:\nDEALER:\n%s\nYOU:\n%s. You won. Amount payed $%.2f.", dealersHandDescription, playersHandDescription, betAmount);
                            fflush(socketFile);
                            read(socket_fd, buffer, MSSG_EXCH_BUF_SIZE);
                            free(dealersHandDescription);
                            free(playersHandDescription);
                            
                            //Pay user
                            playersMoney += betAmount * 2;
                        }
                        else {
                            if (getHandValue(playersHand, playersHandSize) == getHandValue(dealersHand, dealersHandSize)) {
                                //It is a tie
                                
                                //Inform user
                                char * dealersHandDescription = getHandDescription(dealersHand, dealersHandSize, false);
                                char * playersHandDescription = getHandDescription(playersHand, playersHandSize, false);
                                fprintf(socketFile, "D:Hands:\nDEALER:\n%s\nYOU:\n%s\nIt is a tie.", dealersHandDescription, playersHandDescription);
                                fflush(socketFile);
                                read(socket_fd, buffer, MSSG_EXCH_BUF_SIZE);
                                free(dealersHandDescription);
                                free(playersHandDescription);
                                
                                //Return money to user
                                playersMoney += betAmount;
                            }
                            else if (getHandValue(playersHand, playersHandSize) < getHandValue(dealersHand, dealersHandSize)) {
                                //Dealer wins
                                
                                //Inform user of situation
                                char * dealersHandDescription = getHandDescription(dealersHand, dealersHandSize, false);
                                char * playersHandDescription = getHandDescription(playersHand, playersHandSize, false);
                                fprintf(socketFile, "D:Hands:\nDEALER:\n%s\nYOU:\n%s. You lose. Amount retained $%.2f.", dealersHandDescription, playersHandDescription, betAmount);
                                fflush(socketFile);
                                read(socket_fd, buffer, MSSG_EXCH_BUF_SIZE);
                                free(dealersHandDescription);
                                free(playersHandDescription);
                            }
                            else {
                                //User wins
                                
                                //Inform user of situation
                                char * dealersHandDescription = getHandDescription(dealersHand, dealersHandSize, false);
                                char * playersHandDescription = getHandDescription(playersHand, playersHandSize, false);
                                fprintf(socketFile, "D:Hands:\nDEALER:\n%s\nYOU:\n%s. You won. Amount payed $%.2f.", dealersHandDescription, playersHandDescription, betAmount);
                                fflush(socketFile);
                                read(socket_fd, buffer, MSSG_EXCH_BUF_SIZE);
                                free(dealersHandDescription);
                                free(playersHandDescription);
                                
                                //Pay user
                                playersMoney += betAmount * 2;
                            }
                        }
                    }
                }
            }
            continue;
        }
        else if (inChar == 'L') {
            fprintf(socketFile, "D:Alright, take care. You are leaving with $%.2f.", playersMoney);
            fflush(socketFile);
            read(socket_fd, buffer, MSSG_EXCH_BUF_SIZE);
            fprintf(socketFile, "A:end");
            fflush(socketFile);
            read(socket_fd, buffer, MSSG_EXCH_BUF_SIZE);
            break;
        }
        else {
            fprintf(socketFile, "D:Didn't quite get that.");
            fflush(socketFile);
            read(socket_fd, buffer, MSSG_EXCH_BUF_SIZE);
            continue;
        }
    }
    free(deck);
    fclose(socketFile);
}

void server(const char * address, const char * port) {
    printf("Process ID: %i\n", getpid());
    fflush(stdout);
    
    int listener_socket = socket(PF_INET, SOCK_STREAM, 0);
    if (listener_socket == -1) {
        perror("Error in socket function call");
        exit(1);
    }
    puts("Socket created.");
    
    struct sockaddr_in listening_address;
    listening_address.sin_family = AF_INET;
    listening_address.sin_addr.s_addr = inet_addr(address);
    listening_address.sin_port = htons(atoi(port));
    
    if (bind(listener_socket, (struct sockaddr *)&listening_address, sizeof(struct sockaddr_in)) == -1) {
        perror("Error in bind function call");
        exit(1);
    }
    puts("Socket bound.");
    
    if (listen(listener_socket, 2) == -1) {
        perror("Error in listen function call");
        exit(1);
    }
    puts("Socket listening.");
    
    struct sockaddr_in incoming_address;
    unsigned int incoming_address_length = sizeof(incoming_address);
    
    while (true) {
        int connected_socket = accept(listener_socket, (struct sockaddr *)&incoming_address, &incoming_address_length);
        if (connected_socket == -1) {
            perror("Error in accept function call");
            close(listener_socket);
            exit(1);
        }
        puts("Connection accepted.");
        
        pid_t childPID = fork();
        if (childPID == 0) {
            serverChild(connected_socket);
            break;
        }
        else {
            if (childPID == -1) {
                perror("Error in fork function call");
                exit(1);
            }
            close(connected_socket);
        }
    }
    //close(listener_socket);
}

int main(int argc, const char * argv[]) {
    puts("Welcome to Blackjack over TCP by Jonathan Ginsburg (A01021617). Finished on March 6, 2017.");
    if (argc < 2) {
        fprintf(stderr, "Incorrect usage. See README.txt file.\n");
        return 1;
    }
    if (strcmp("client", argv[1]) == 0) {
        client(argv[2], argv[3]);
    }
    else if (strcmp("server", argv[1]) == 0){
        server(argv[2], argv[3]);
    }
    else {
        fprintf(stderr, "Unknown subcommand %s. See README.txt file.\n", argv[1]);
        return 1;
    }
    
    return 0;
}
