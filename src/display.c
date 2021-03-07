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