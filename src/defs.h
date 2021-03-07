#ifndef DEFS_H
#define DEFS_H

#define ESCAPE 27

typedef struct{
	int x;
	int y;
	int maxX;
	int maxY;
} sCursor;

typedef struct{
	int size;
	int *contentLength;

	char *fcontent; 
} sContent;

#endif

