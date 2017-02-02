//
//  ppmmanipulation.c
//  003-SimpleImageManipulation
//
//  Created by Jonathan Ginsburg on 2/2/17.
//  Copyright © 2017 Jonathan Ginsburg. All rights reserved.
//

#include "ppmmanipulation.h"

struct PPMImage * readPPMImage(const char * path, int * dataPosition) {
    struct PPMImage * retVal = malloc(sizeof(struct PPMImage)); //Create image structure
    int fileSize = 0; //Variable that holds the raw file size in bytes
    unsigned char * fileContents = readFileBinary(path, &fileSize); //Get the contents and size of the raw file
    int from = findByte(fileContents, fileSize, 'P'); //Find position of first P
    int to = findWhitespaceByte(fileContents + from, fileSize - from); //Find position of first whitespace
    (* retVal).magicNumber = atoi(substring((char *)fileContents, from + 1, to - 1)); //Get magic number
    from = to + 1; //Set from variable to be right after the first whitespace found
    //Skip all contiguous whitespaces found
    int nextWhitespace = findWhitespaceByte(fileContents + from, fileSize - from); //Find next whitespace relative to -from-
    while (nextWhitespace == 0) { //If -from- is at a whitespace keep going
        ++from; //Move -from- forward
        nextWhitespace = findWhitespaceByte(fileContents + from, fileSize - from); //Recheck next whitespace position relative to -from-
    }
    if (fileContents[from] == '#') {
        from += findByte(fileContents + from, fileSize - from, '\n') + 1; //Find next line break relative to -from-
        while (fileContents[from] == '#') { //If -from- is at a whitespace keep going
            from += findByte(fileContents + from, fileSize - from, '\n') + 1;
        }
    }
    nextWhitespace = findWhitespaceByte(fileContents + from, fileSize - from); //Find next whitespace relative to -from-
    while (nextWhitespace == 0) { //If -from- is at a whitespace keep going
        ++from; //Move -from- forward
        nextWhitespace = findWhitespaceByte(fileContents + from, fileSize - from); //Recheck next whitespace position relative to -from-
    }
    (* retVal).width = atoi(substring((char *)fileContents, from, from + nextWhitespace)); //Get width
    from = from + nextWhitespace + 1; //Set -from- to point at next byte right after the first whitespace after the width
    //Skip all contiguous whitespaces found
    nextWhitespace = findWhitespaceByte(fileContents + from, fileSize - from); //Find next whitespace relative to -from-
    while (nextWhitespace == 0) { //If -from- is at a whitespace keep going
        ++from; //Move -from- forward
        nextWhitespace = findWhitespaceByte(fileContents + from, fileSize - from); //Recheck next whitespace position relative to -from-
    }
    (* retVal).height = atoi(substring((char *)fileContents, from, from + nextWhitespace)); //Get width
    from = from + nextWhitespace + 1; //Set -from- to point at next byte right after the first whitespace after the height
    //Skip all contiguous whitespaces found
    nextWhitespace = findWhitespaceByte(fileContents + from, fileSize - from); //Find next whitespace relative to -from-
    while (nextWhitespace == 0) { //If -from- is at a whitespace keep going
        ++from; //Move -from- forward
        nextWhitespace = findWhitespaceByte(fileContents + from, fileSize - from); //Recheck next whitespace position relative to -from-
    }
    (* retVal).maxColor = atoi(substring((char *)fileContents, from, from + nextWhitespace)); //Get maxColor
    from = from + nextWhitespace + 1; //Set -from- to point at next byte right after the first whitespace after the maxColor
    //Allocate enough space for the image data >>>
    (* retVal).pixels = malloc(sizeof(struct Pixel *) * (* retVal).height);
    for (int i = 0; i < (* retVal).height; ++i) {
        (* retVal).pixels[i] = malloc(sizeof(struct Pixel) * (* retVal).width);
    }
    // <<<
    * dataPosition = from;
    // Fill image data with >>>
    for (int i = 0; i < (* retVal).height; ++i) {
        for (int j = 0; j < (* retVal).width; ++j) {
            int currentPixelPosition = from + 3 * (i * (* retVal).width + j);
            (* retVal).pixels[i][j].r = fileContents[currentPixelPosition];
            (* retVal).pixels[i][j].g = fileContents[currentPixelPosition + 1];
            (* retVal).pixels[i][j].b = fileContents[currentPixelPosition + 2];
        }
    }
    // <<<
    //free(fileContents); //Memory release of aid variable
    return retVal;
}
struct PPMImage * copyPPMImage(const struct PPMImage * target) {
    struct PPMImage * retVal = malloc(sizeof(struct PPMImage)); //Create image structure
    // Copy nonpointer properties >>>
    (* retVal).height = (* target).height;
    (* retVal).width = (* target).width;
    (* retVal).magicNumber = (* target).magicNumber;
    (* retVal).maxColor = (* target).maxColor;
    // <<<
    //Allocate enough space for the image data >>>
    (* retVal).pixels = malloc(sizeof(struct Pixel *) * (* retVal).height);
    for (int i = 0; i < (* retVal).height; ++i) {
        (* retVal).pixels[i] = malloc(sizeof(struct Pixel) * (* retVal).width);
    }
    // <<<
    // Populate new image >>>
    for (int i = 0; i < (* retVal).height; ++i) {
        for (int j = 0; j < (* retVal).width; ++j) {
            (* retVal).pixels[i][j].r = (* target).pixels[i][j].r;
            (* retVal).pixels[i][j].g = (* target).pixels[i][j].g;
            (* retVal).pixels[i][j].b = (* target).pixels[i][j].b;
        }
    }
    // <<<
    return retVal;
}
void freePPMImage(struct PPMImage * target) {
    // Free each row >>>
    for (int i = 0; i < (* target).height; ++i) {
        free((* target).pixels[i]);
    }
    // <<<
    free((* target).pixels); //Free rows array
    free(target);
}
struct PPMImage * invertPPMImage(const struct PPMImage * target) {
    struct PPMImage * retVal = copyPPMImage(target);
    // Invert each pixel >>>
    for (int i = 0; i < (* retVal).height; ++i) {
        for (int j = 0; j < (* retVal).width; ++j) {
            (* retVal).pixels[i][j].r = (* target).maxColor - (* target).pixels[i][j].r;
            (* retVal).pixels[i][j].g = (* target).maxColor - (* target).pixels[i][j].g;
            (* retVal).pixels[i][j].b = (* target).maxColor - (* target).pixels[i][j].b;
        }
    }
    // <<<
    return retVal;
}
unsigned char * PPMImageReplaceStreamsData(const struct PPMImage * image, const unsigned char * target, const int streamSize, const int dataPosition) {
    unsigned char * retVal = malloc(sizeof(unsigned char) * streamSize);
    memcpy(retVal, target, streamSize);
    // Fill image data with >>>
    for (int i = 0; i < (* image).height; ++i) {
        for (int j = 0; j < (* image).width; ++j) {
            int currentPixelPosition = dataPosition + 3 * (i * (* image).width + j);
            retVal[currentPixelPosition] = (* image).pixels[i][j].r;
            retVal[currentPixelPosition + 1] = (* image).pixels[i][j].g;
            retVal[currentPixelPosition + 2] = (* image).pixels[i][j].b;
        }
    }
    // <<<
    return retVal;
}
