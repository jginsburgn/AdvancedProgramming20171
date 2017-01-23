//
//  JG.h
//  
//
//  Created by Jonathan Ginsburg on 1/22/17.
//
//

#ifndef JG_h
#define JG_h

#define BUFFER_SIZE 100

#include <stdlib.h>
#include <stdio.h>

//Reads a string from stdin until the untilChar is found (untilChar can be the end of file EOF signal). The returned string fits perfectly the read contents (uses exact memory space) and includes the terminating character; but, does not include the untilChar read.
char * readString(const char untilChar);

#endif /* JG_h */
