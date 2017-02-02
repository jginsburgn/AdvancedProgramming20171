//
//  JG.h
//  
//
//  Created by Jonathan Ginsburg on 1/22/17.
//
//

#ifndef JG_h
#define JG_h

#define BUFFER_SIZE 1000

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

//Boolean type wrapper into int
typedef enum { false, true } bool;

//Reads a string from stdin until the -untilChar- is found (-untilChar- can be the end of file EOF signal). The returned string fits perfectly the read contents (uses exact memory space) and includes the terminating character; but, does not include the -untilChar- read.
char * readString(const char untilChar);

//Reads file at -path- and returns the contents in a newly allocated null-byte terminated string.
char * readFile(const char * path);

//Writes -string- to file located at -path-.
bool writeFile(const char * path, const char * string);

//Reads file at -path- and returns its contents as byte elements of the array. The total number of bytes read is stored in -readSize-.
unsigned char * readFileBinary(const char * path, int * readSize);

//Writes -size- bytes in -target- to file at -path-.
void writeFileBinary(const char * path, const unsigned char * target, const int size);

//Prints to <stdout> -size- bytes in -buffer- separated by spaces in -base- numerical base.
typedef enum {octal, decimal, hexadecimal} base;
void printBytes(const unsigned char * buffer, const int size, const base print_base);

//Returns the position of the first ocurrence of -byte- in -target- in the first -size- bytes. If not found -1 is returned.
int findByte(const unsigned char * target, const int size, const unsigned char byte);

//Finds the position of the first ocurrence of whitespace (i.e. BLANK, TAB, CR, LF) in -target-'s first -size- bytes. If not found, -1 is returned.
int findWhitespaceByte(const unsigned char * target, const int size);

//Returns the substring of -target- beginning at index -from- and ending at index -to-, inclusive both. E.g.:
/*
                                    indices 01234
                                            vvvvv
                               -target- := "hello"
      findWhitespaceByte("hello", 1, 3) :=  "ell"
 */
char * substring(const char * target, const int from, const int to);

#endif /* JG_h */
