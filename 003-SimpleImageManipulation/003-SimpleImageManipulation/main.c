//
//  main.c
//  003-SimpleImageManipulation
//
//  Created by Jonathan Ginsburg on 2/2/17.
//  Copyright © 2017 Jonathan Ginsburg. All rights reserved.
//
/*
 Task Directions:

 Simple_image_manipulation
 
Create a program that can read and write images in PPM format. The format is very simple, and can be used to store images of any size, using 24 bit color: 8 bits for each channel (red, green and blue)

The description of the format can be found here:

http://netpbm.sourceforge.net/doc/ppm.html

I recommend the use of structures to organize the image data you read. Your program should be capable of reading an image and creating a new one with the negative version of the original. A sample image you can use is provided here, along with the negative image your program should be able to get.

You can create other images yourselves to test, either by manually typing them, or converting them from other formats. The Imagemagick tooks are very useful for doing format conversion: https://www.imagemagick.org/script/index.php

You must create library files (.h and .c) for the handling of the image, so that you can reuse them for other projects. Beside these you will need a main program, and possibly a Makefile.

Upload only a single ZIP file containing your source files. Remember to write your name in all your files. Do not upload project files from Visual Studio or Xcode, only source files.
*/

#include "ppmmanipulation.h"

int main(int argc, const char * argv[]) {
    char * credits = "This simple image manipulator was done by Jonathan Ginsburg (A01021617) on the 1st of January of 2017, for the advanced programming class.\n";
    puts(credits);
    char * instructions = "Usage:\n\t<executable> <source> <dest>;\nBehaviour:\n\tThe program will read a magic-numbered P6, PPM formatted (http://netpbm.sourceforge.net/doc/ppm.html) image from <source>, produce its negative and write it to <dest>.\n";
    if (argc != 3) {
        puts(instructions);
        return 1;
    }
    int dataPosition = 0;
    int size = 0;
    unsigned char * rawData = readFileBinary(argv[1], &size);
    struct PPMImage * originalImage = readPPMImage(argv[1], &dataPosition);
    struct PPMImage * invertedImage = invertPPMImage(originalImage);
    writeFileBinary(argv[2], PPMImageReplaceStreamsData(invertedImage, rawData, size, dataPosition), size);
    free(rawData);
    freePPMImage(originalImage);
    freePPMImage(invertedImage);
    return 0;
}
