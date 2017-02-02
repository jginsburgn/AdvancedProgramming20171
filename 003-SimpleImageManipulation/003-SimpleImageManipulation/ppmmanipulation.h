//
//  ppmmanipulation.h
//  003-SimpleImageManipulation
//
//  Created by Jonathan Ginsburg on 2/2/17.
//  Copyright © 2017 Jonathan Ginsburg. All rights reserved.
//

#ifndef ppmmanipulation_h
#define ppmmanipulation_h

#include "../../Libraries/JG.h"

//RGB Channel structure
struct Pixel {
    int r;
    int g;
    int b;
};
//PPM formatted (http://netpbm.sourceforge.net/doc/ppm.html) image data structure representation
struct PPMImage {
    int magicNumber; //Magic number as specified
    int width;
    int height;
    int maxColor;
    struct Pixel * * pixels;
};

//Reads a file at -path- containing a magic-numbered P6 PPM formatted image and returns its corresponding structure. -dataPosition- is filled with the byte offset from the beginning of the file where the image raster begins.
struct PPMImage * readPPMImage(const char * path, int * dataPosition);

//Returns a deep copy of -target-.
struct PPMImage * copyPPMImage(const struct PPMImage * target);

//Frees each dynamically allocated component of a PPMImage and the image itself.
void freePPMImage(struct PPMImage * target);

//Returns a new image obtained through the inversion (a.k.a. negation) of the image -target- raster.
struct PPMImage * invertPPMImage(const struct PPMImage * target);

//Returns a new byte array corresponding to taking the original -target- byte array and replacing its magic numbered P6, PPM formatted image raster by the one described in -image-, where -streamSize- is the number of bytes of -target- (in total, not only the raster) and -dataPosition- is the offset from the beginning of the byte array (-target-) in which the image raster begins.
unsigned char * PPMImageReplaceStreamsData(const struct PPMImage * image, const unsigned char * target, const int streamSize, const int dataPosition);
#endif /* ppmmanipulation_h */
