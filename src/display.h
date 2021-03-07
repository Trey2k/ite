#ifndef DISPLAY_BUFFER_H
#define DISPLAY_BUFFER_H

#include "defs.h"
#include <ncurses.h>
#include <stdlib.h>

typedef struct{
	int offset;
	int rowCount;
	int width;
	int height;
	int *displayLength;
	
	char **displayBuffer;
} sDisplay;

extern void updateDisplayBuffer(sDisplay *display, const sContent *content);
extern void update(sDisplay *display, sContent *content, bool freeMem);
#endif