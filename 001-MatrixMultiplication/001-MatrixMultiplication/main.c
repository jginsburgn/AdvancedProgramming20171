//
//  main.c
//  001-MatrixMultiplication
//
//  Created by Jonathan Ginsburg on 1/11/17.
//  Copyright © 2017 Jonathan Ginsburg. All rights reserved.
//

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

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

//Returns the string resulting by trimming the trailing line feed characters (removes as many as needed) out of str
char * cleanTrailingEOLs(const char * str) {
    char localStr[strlen(str) + 1]; //Create local, modifiable string copy
    strcpy(localStr, str);
    while (localStr[strlen(localStr)-1] == '\n') { //Remove as many new line characters as possible
        localStr[strlen(localStr)-1] = '\0';
    }
    char * retVal = malloc(sizeof(char)*strlen(localStr) + 1); //Create a new string to return (this one has no extra space, just what is needed)
    strcpy(retVal, localStr);
    return retVal;
}

//Returns an array of strings of which each entry is a substring delimited by the splitter character on string and takes a pointer to an int (size) where it stores the size of the resulting array.
char ** splitString(const char * string, const char splitter, int * size) {
    //Remove extra end of line characters
    char * str = cleanTrailingEOLs(string);
    //Determine the number of entries in splitted array
    int entries = 0;
    for (int i = 0; i < strlen(str); i++) {
        char ch = str[i];
        if (ch == splitter) {
            entries++;
        }
    }
    if (str[strlen(str)-1] != splitter) {
        entries++;
    }
    if (size) {
        *size = entries;
    }
    //Create array of strings with the needed number of elements
    char ** retVal = malloc(sizeof(char*)*entries);
    //Determine the length of each row
    int * lengths = malloc(sizeof(int)*entries);
    int j = 0;
    int maxRowsLength = 0; //Max row's length is not used (just kept for future use)
    int currentRowsLength = 0;
    for (int i = 0; i < strlen(str); i++) {
        char ch = str[i];
        if (ch == splitter) {
            if (maxRowsLength < currentRowsLength) {
                maxRowsLength = currentRowsLength;
            }
            lengths[j++] = currentRowsLength;
            currentRowsLength = 0;
        }
        else {
            currentRowsLength++;
        }
    }
    //Build each new string
    for (int i = 0; i < entries; i++) {
        retVal[i] =  malloc(sizeof(char)*lengths[i] + 1);
    }
    //Set each row as a string of retVal
    j = 0;
    int offset = 0;
    for (int i = 0; i < strlen(str); i++) {
        char ch = str[i];
        if (ch == splitter) {
            offset = i + 1;
            ++j;
        }
        else {
            retVal[j][i-offset] = ch;
        }
    }
    free(str);
    free(lengths);
    return retVal;
}

//Frees memory of dynamic bidimensional array matrix of n rows
void destroyMatrix(void ** matrix, const int n) {
    for (int i = 0; i < n; i++) {
        free(matrix[i]);
    }
    free(matrix);
}

//Returns an empty double matrix of r x c size
double ** buildMatrix(const int r, const int c) {
    double ** retVal = malloc(sizeof(double*)*r);
    for (int i = 0; i < r; ++i) {
        retVal[i] = malloc(sizeof(double)*c);
    }
    return retVal;
}

//Returns matrix represented in file given by path with rows divided by newlines and entries in each row divided by blank spaces; it stores the number of rows and columns in r and c, respectively
double ** fileToMatrix(const char * path, int * r, int * c) {
    char * file = readFile(path);
    char ** rows = splitString(file, '\n', r);
    splitString(rows[0], ' ', c);
    double ** retVal = buildMatrix(*r, *c);
    for (int i = 0; i < *r; i++) {
        char ** row = splitString(rows[i], ' ', NULL);
        for (int j = 0; j < *c; j++) {
            retVal[i][j] = atof(row[j]);
        }
        free(row);
    }
    destroyMatrix((void*)rows, *r);
    free(file);
    return retVal;
}

//Prints a matrix of rows x columns size to stdout with two decimal precision
void printMatrix(const double ** matrix, const int rows, const int columns){
    for (int i = 0; i < rows; i++) {
        for (int j = 0; j < columns; j++) {
            if (j == columns - 1) {
                printf("%.2f", matrix[i][j]);
            }
            else {
                printf("%.2f ", matrix[i][j]);
            }
        }
        puts("");
    }
}

//Returns the multiplication of matrix a of size rA x cA and matrix b of size rB x cB
double ** multiplyMatrices(const double ** a, const int rA, const int cA, const double ** b, const int rB, const int cB) {
    double ** retVal = buildMatrix(rA, cB);
    for (int i = 0; i < rA; i++) {
        for (int j = 0; j < cB; j++) {
            retVal[i][j] = 0;
            for (int ii = 0; ii < cA; ii++) {
                retVal[i][j] += a[i][ii]*b[ii][j];
            }
        }
    }
    return retVal;
}

//Writes matrix matrix of size rows x columns to file given by path, with the format so that it can be read with fileMatrix function defined above
void writeMatrixToFile(const char * path, const double ** matrix, const int rows, const int columns) {
    FILE * file = fopen(path, "w");
    for (int i = 0; i < rows; i++) {
        for (int j = 0; j < columns; j++) {
            if (j == columns - 1) {
                fprintf(file, "%.2f\n", matrix[i][j]);
            }
            else {
                fprintf(file, "%.2f ", matrix[i][j]);
            }
        }
    }
    fclose(file);
}

int main(int argc, const char * argv[]) {
    char * credits = "This matrix multiplier was done by Jonathan Ginsburg (A01021617) on the 12th of January of 2017, for the advanced programming class.";
    puts(credits);
    char * instructions = "Usage: <executable> arg1 arg2 arg3, where each arg is a path.\nThe first and second arguments, each, refer to a file containing a matrix formatted with the following rules: each row of the matrix is divided by a new line character and each entry in a row is divided by a single blank space. The third argument refers to a file that will be created (or replaced) where the result of the matrix multiplication of the matrices represented by arg1 and arg2 will be stored.";
    if (argc != 4) {
        puts(instructions);
    }
    int rA;
    int cA;
    double ** matrixA = fileToMatrix(argv[1], &rA, &cA);
    int rB;
    int cB;
    double ** matrixB = fileToMatrix(argv[2], &rB, &cB);
    double ** result = multiplyMatrices((const double **)matrixA, rA, cA, (const double **)matrixB, rB, cB);
    printMatrix((const double **)result, rA, cB);
    writeMatrixToFile(argv[3], (const double **)result, rA, cB);
    destroyMatrix((void*)matrixA, rA);
    destroyMatrix((void*)matrixB, rB);
    destroyMatrix((void*)result, rA);
    return 0;
}
