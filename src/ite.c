#include <ncurses.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/ioctl.h>
#include <unistd.h>

#include "key_defs.h"

typedef struct{
	int x;
	int y;
	int maxX;
	int maxY;
} sCursor;

int fileExists(const char *fname){
	FILE *file;
	if((file = fopen(fname, "r"))){
		fclose(file);
		return 1;
	}
	return 0;
}

int fileWritePerms(const char *fname){
	FILE *file;
	if((file = fopen(fname, "r+"))){
		fclose(file);
        	return 1;
	}
	return 0;
}

void keyHandler(int width, int rowCount, int *offset, int *contentLength, sCursor *cursor, int ch){
	char toPrint = -1;
	int index;
	bool xChange = false;
	bool yChange = false;
	switch (ch)
        {
          case KEY_LEFT:
            cursor->x--;
			xChange = true;
            break;
          case KEY_RIGHT:
            cursor->x++;
			xChange = true;
            break;
          case KEY_UP:
		  	cursor->y--;
			  yChange = true;
            break;
          case KEY_DOWN:
            cursor->y++;
			yChange = true;
            break;
		  default:
			index = cursor->x*cursor->y;
			//fcontent[index] = ch;
        }

		if(cursor->x > cursor->maxX || (cursor->x+(*offset) > contentLength[cursor->y] && xChange)){
				if((*offset)+width+1 > contentLength[cursor->y]){
					(*offset)=0;
					cursor->x = 0;
					cursor->y++;
				}else{
					cursor->x = cursor->maxX;
					(*offset)++;
				}
		}else if(cursor->x < 0){
				if((*offset)-1 <= 0){
					cursor->x = cursor->maxX;
					cursor->y--;
					(*offset) = contentLength[cursor->y]-width;
					if((*offset) <= 0){
						(*offset)=0;
						cursor->x = contentLength[cursor->y];
					}
				}else{
					cursor->x = 0;
					(*offset)--;
				}
		}else if(cursor->x+(*offset) > contentLength[cursor->y] && yChange){
			if(contentLength[cursor->y]-(*offset) < 0){
				(*offset) = contentLength[cursor->y]-width;
				if((*offset) < 0){
					(*offset) = 0;
				}
			}
			cursor->x = contentLength[cursor->y]-(*offset);
		}
		
		if(cursor->y > rowCount){
			cursor->y = rowCount;
		}

		if(cursor->y < 0){
				cursor->y = 0;
		}else if(cursor->y > cursor->maxY){
				cursor->y = cursor->maxY;
		}
	
}

WINDOW *createNewWin(int height, int width, int startY, int startX){
	WINDOW *win;

	win = newwin(height, width, startY, startX);
  	wrefresh(win);

	return win;
}

void updateDisplayBuffer(int offsetX, int width, int height, int size, char *fcontent, char **displayBuffer){
	int rows = 0;
	int cols = 0;
	int chars = 0;

	for(int i = 0; i < height; i++){
		for(int y = 0; y < width; y++){
			displayBuffer[i][y]= '\0';
		}
	}

	for(int i = 0; i < size; i++){
			if(fcontent[i] == '\n'){
				if(cols <= width){
					displayBuffer[rows][cols] = '\n';
				}
				cols = 0;
				chars = 0;
				rows++;
			}else{
				if(chars >= offsetX){
					if(cols < width){
						displayBuffer[rows][cols] = fcontent[i];
					}else if (cols == width){
						displayBuffer[rows][cols] = '\n';
					}

					cols++;
				}
				chars++;
			}
	}
}

void editor(const char *fname){

	FILE *file;
	if(!(file = fopen(fname, "r+"))){
		return;
	}

	fseek(file, 0, SEEK_END); //Getting the size of the file
	int size = ftell(file);
	fseek(file, 0, SEEK_SET);

	char *fcontent = malloc(size); //Allocationg space for the file in memory
	fread(fcontent, 1, size, file); //Reading content from file

	/*for(int i = 0; i < size; i++){
		printf("%c", fcontent[i]);
	}*/
	//exit(0);

	WINDOW *editor;
	int startX, startY, width, height;
	int ch;

	int rowCount = 0;

	initscr();

	start_color();
	init_pair(1, COLOR_BLACK, COLOR_RED);
	init_pair(2, COLOR_BLACK, COLOR_GREEN);

	cbreak();
	keypad(stdscr, true);
	noecho();

	struct winsize w;
    	ioctl(STDOUT_FILENO, TIOCGWINSZ, &w);

	startY = 1;
	startX = 5;
	height = w.ws_row - startY;
	width = w.ws_col - startX;

	for(int i = 0; i < size; i++){
		if(fcontent[i] == '\n'){
			rowCount++;
		}
	}

	int *contentLength;
	contentLength = malloc(rowCount * sizeof(int));
	if(contentLength == NULL){
			fprintf(stderr, "out of memory for content length\n");
			exit(-1);
	}

	for(int i = 0; i <= rowCount; i++){
		contentLength[i] = 0;
	}

	int curRowLength = 0;
	int rowIndex = 0;
	for(int i = 0; i < size; i++){
		if(fcontent[i] == '\t'){
			curRowLength+=8;
		}else{
			curRowLength++;
		}
		if(fcontent[i] == '\n' || fcontent[i] == EOF || fcontent[i] == '\000'){
			contentLength[rowIndex++] = curRowLength;
			curRowLength = 0;
		}
	}

	char **displayBuffer;
	displayBuffer = malloc((height * sizeof(char *)));
	if(displayBuffer == NULL){
			fprintf(stderr, "out of memory\n");
			exit(-1);
	}

	
	for(int i = 0; i < height; i++){
		displayBuffer[i] = malloc(width * sizeof(char));
		if(displayBuffer[i] == NULL){
			fprintf(stderr, "out of memory\n");
			exit(-1);
		}
	}


	int offset = 0;

	updateDisplayBuffer(offset, width, height, size, fcontent, displayBuffer);


	editor = createNewWin(height, width, startY, startX);

	wrefresh(editor);

	sCursor cursor;
	cursor.y = 0;
	cursor.x = 0;
	cursor.maxY = height-1;
	cursor.maxX = width-1;


	
	attron(COLOR_PAIR(2));
	printw("Press Esc to exit Text Editor (%d, %d) %d", cursor.x, cursor.y, rowCount);
	refresh();

	use_default_colors();
	for(int i = 0; i < height; i++){
		mvwprintw(editor, i, 0, displayBuffer[i]);
	}
	
	wmove(editor, cursor.y, cursor.x);

	wrefresh(editor);

	while((ch = getch()) != ESCAPE){ //Get key events and quit if excape is pressed
		keyHandler(width, rowCount, &offset, contentLength, &cursor, ch);
		updateDisplayBuffer(offset, width, height, size, fcontent, displayBuffer);

		attron(COLOR_PAIR(2));
		mvprintw(0, 0, "Press Esc to exit Text Editor (%d, %d)", cursor.x, cursor.y);
		refresh();
		
		

		for(int i = 0; i < height; i++){
			mvwprintw(editor, i, 0, displayBuffer[i]);
		}
		
		wmove(editor, cursor.y, cursor.x);
		wrefresh(editor);
	}
	endwin();
	fclose(file);
}

int main(int argc, char *argv[]){
	if(argc == 2){
		if(fileExists(argv[1])){
			printf("Found file %s\n", argv[1]);

			if(fileWritePerms(argv[1])){
				printf("Write perms\n");
				editor(argv[1]);
			}else{
				printf("No Wrie Perms\n");
			}
		}else{
			printf("File not found %s\n", argv[1]);
		}
	}else if(argc > 2){
		printf("Expected only 1 arg\n");
	}else{
		printf("Expected atleast 1 arg\n");
	}
	return 0;
}

