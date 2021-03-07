#ifndef DISPLAY_BUFFER_H
#define DISPLAY_BUFFER_H

#include "defs.h"

typedef struct{
	int offset;
	int rowCount;
	int width;
	int height;
	int *displayLength;
	
	char **displayBuffer;
} sDisplay;

extern void updateDisplayBuffer(sDisplay *display, const sContent *content);
#endif