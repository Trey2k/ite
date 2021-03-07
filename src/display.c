#include "display.h"

void updateDisplayBuffer(sDisplay *display, const sContent *content){
	int rows = 0;
	int cols = 0;
	int chars = 0;

	for(int i = 0; i < display->height; i++){
		for(int y = 0; y < display->width; y++){
			display->displayBuffer[i][y]= '\0';
		}
	}

	for(int i = 0; i < content->size; i++){
			if(content->fcontent[i] == '\n'){
				if(cols <= display->width){
					display->displayBuffer[rows][cols] = '\n';
				}
				cols = 0;
				chars = 0;
				rows++;
			}else{
				if(chars >= display->offset){
					if(cols < display->width){
						display->displayBuffer[rows][cols] = content->fcontent[i];
					}else if (cols == display->width){
						display->displayBuffer[rows][cols] = '\n';
					}

					cols++;
				}
				chars++;
			}
	}
}

void update(sDisplay *display, sContent *content, bool freeMem){

	display->rowCount = 0;
	//Counting the number of rows
	for(int i = 0; i < content->size; i++){
		if(content->fcontent[i] == '\n'){
			display->rowCount++;
		}
	}

	//Allocating memory for the contentLength array
	if(freeMem){
		free(content->contentLength);
	}
	content->contentLength = malloc(display->rowCount * sizeof(int));
	if(content->contentLength == NULL){
			fprintf(stderr, "out of memory\n");
			exit(-1);
	}

	
	for(int i = 0; i <= display->rowCount; i++){
		content->contentLength[i] = 0;
	}

	//Filling the content length array
	int curRowLength = 0;
	int rowIndex = 0;
	for(int i = 0; i < content->size; i++){
		curRowLength++;
		if(content->fcontent[i] == '\n' || content->fcontent[i] == EOF || content->fcontent[i] == '\000'){
			content->contentLength[rowIndex++] = curRowLength;
			curRowLength = 0;
		}
	}

	//Allocating memory for the displayLength array (Display length is the current hack around tabs and newlines. In the future we need to check for them on the fly)
	if(freeMem){
		free(display->displayLength);
	}
	display->displayLength = malloc(display->rowCount * sizeof(int));
	if(display->displayLength == NULL){
			fprintf(stderr, "out of memory\n");
			exit(-1);
	}

	for(int i = 0; i <= display->rowCount; i++){
		display->displayLength[i] = 0;
	}

	//FIlling the displayLength array
	curRowLength = 0;
	rowIndex = 0;
	for(int i = 0; i < content->size; i++){
		if(content->fcontent[i] == '\t'){
			curRowLength+=8;
		}else if(content->fcontent[i] != '\n'){
			curRowLength++;
		}
		
		if(content->fcontent[i] == '\n' || content->fcontent[i] == EOF || content->fcontent[i] == '\000'){
			display->displayLength[rowIndex++] = curRowLength;
			curRowLength = 0;
		}
	}
}