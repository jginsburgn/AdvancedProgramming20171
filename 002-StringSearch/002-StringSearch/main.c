//
//  main.c
//  002-StringSearch
//
//  Created by Jonathan Ginsburg on 1/22/17.
//  Copyright © 2017 Jonathan Ginsburg. All rights reserved.
//

#include "../../Libraries/JG.h"

int main(int argc, const char * argv[]) {
    char * credits = "This string searcher was done by Jonathan Ginsburg (A01021617) on the 22nd of January of 2017, for the advanced programming class.";
    puts(credits);
    char * instructions = "Usage:\n\t<executable>;\nBehaviour:\n\tThe program will read from the standard input until an end of file (EOF) is found, or Ctrl+D is pressed, if reading from console. After that, the program will read another string in the same fashion. Then it will display the times that the second string was found to be a substring of the first and exit with code 0.";
    puts(instructions);
    readString();
    return 0;
}
