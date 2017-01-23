//
//  main.c
//  002-StringSearch
//
//  Created by Jonathan Ginsburg on 1/22/17.
//  Copyright © 2017 Jonathan Ginsburg. All rights reserved.
//
//Task directions:
//String search
//Write a program that counts how many times a word appears inside of a sentence.
//
//You can define two strings as arrays of chars, of sizes 100 and 10. The program will ask the user for these strings. The first string will be a full sentence, and the second one will be a word. You should use pointers to solve this problem.
//
//For example, with the following input:
//
//"This one is larger than the other one over there" and "one", the program should print that the string "one" appears 2 times in the sentence.
//
//You can do this easily with the functions "strchr", "strstr" and "strncmp". Try doing the program with or without these functions. You can always use "strlen".

#include "../../Libraries/JG.h"
#include <string.h>

int main(int argc, const char * argv[]) {
    char * credits = "This string searcher was done by Jonathan Ginsburg (A01021617) on the 22nd of January of 2017, for the advanced programming class.";
    puts(credits);
    char * instructions = "Usage:\n\t<executable>;\nBehaviour:\n\tThe program will read two strings from the standard input. Then it will display the times that the second string was found to be a substring of the first and exit with.\n";
    puts(instructions);
    puts("Enter the first string:");
    char * first = readString('\n');
    puts("Enter the second string:");
    char * second = readString('\n');
    int numberOfOcurrences = 0;
    char * foundSubstr = strstr(first, second);
    while (foundSubstr != NULL) {
        ++numberOfOcurrences;
        foundSubstr += strlen(second);
        foundSubstr = strstr(foundSubstr, second);
    }
    printf("The number of ocurrences of the second string in the first one is: %i.\n", numberOfOcurrences);
    return 0;
}
