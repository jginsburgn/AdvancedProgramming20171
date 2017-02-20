//
//  main.c
//  Playground
//
//  Created by Jonathan Ginsburg on 1/11/17.
//  Copyright © 2017 Jonathan Ginsburg. All rights reserved.
//

#include <stdlib.h>
#include <signal.h>
#include <stdio.h>
#include <errno.h>
#include <string.h>
#include <unistd.h>

int main(int argc, const char * argv[]) {
    int * pipefd = malloc(sizeof(int) * 2);
    pid_t cpid;
    char buf;
    
    if (pipe(pipefd) == -1) {
        perror(strerror(errno));
        exit(EXIT_FAILURE);
    }
    if ((cpid = fork()) == -1) {
        perror(strerror(errno));
        exit(EXIT_FAILURE);
    }
    else if (cpid != 0) {
        //Parent
        printf("[P] PID: %i.\n", getpid());
        fflush(stdout);
        //kill(getpid(), SIGSTOP); //For debugging
        
        //---Code:
        printf("[P] *(%p): %i Out: *(%p): %i.\n", pipefd, pipefd[0], pipefd + 1, pipefd[1]);
        
        int out = pipefd[1];
        close(pipefd[0]);
        free(pipefd);
        
        char * input = malloc(sizeof(char) * 10);
        printf("[P] Read %zi bytes.\n", read(STDIN_FILENO, input, 10));
        write(out, input, 10);
        
        //---/Code
        printf("Closing [P] PID: %i.\n", getpid());
        fflush(stdout);
    }
    else {
        //Child
        printf("[C] PID: %i.\n", getpid());
        fflush(stdout);
        //kill(getpid(), SIGSTOP); //For debugging
        
        //---Code:
        printf("[C] *(%p): %i Out: *(%p): %i.\n", pipefd, pipefd[0], pipefd + 1, pipefd[1]);
        
        int in = pipefd[0];
        close(pipefd[1]);
        free(pipefd);
        
        char * input = malloc(sizeof(char) * 10);
        printf("[C] Read %zi bytes.\n", read(in, input, 10));
        for (int i = 0; i < 10; ++i) {
            char a = input[i];
            printf(".%i.", (int)a);
        }
        puts("");
        //puts(input);
        
        //---/Code
        printf("Closing [C] PID: %i.\n", getpid());
        fflush(stdout);
    }
    return 0;
}
