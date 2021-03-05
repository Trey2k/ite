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

void keyHandler(int *offset, sCursor *cursor, int ch){
	char toPrint = -1;
	int index;
	switch (ch)
        {
          case KEY_LEFT:
            cursor->x--;
            break;
          case KEY_RIGHT:
            cursor->x++;
            break;
          case KEY_UP:
		  	cursor->y--;
            break;
          case KEY_DOWN:
            cursor->y++;
            break;
		  default:
			index = cursor->x*cursor->y;
			//fcontent[index] = ch;
        }

		if(cursor->x > cursor->maxX){
				//cursor->x = 0;
				//cursor->y++;
				cursor->x = cursor->maxX;
				(*offset)++;
		}else if(cursor->x < 0){
				cursor->x = cursor->maxX;
				cursor->y--;
				
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

void updateDisplayBuffer(int offsetX, int width, int size, char *fcontent, char *displayBuffer){
	int index = 0;
	int cols = 0;
	for(int i = offsetX; i < size; i++){
		if(fcontent[i] == '\n'){
			if(cols <= width){
				displayBuffer[index++] = '\n';
			}
			cols = 0;
			i+=offsetX;
		}else{
			if(cols < width){
				displayBuffer[index++] = fcontent[i];
			}else if (cols == width){
				displayBuffer[index++] = '\n';
			}

			cols++;
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

	WINDOW *editor;
	int startX, startY, width, height;
	int ch;

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

	char *displayBuffer = malloc(width*height);

	int offset = 0;

	updateDisplayBuffer(offset, width, size, fcontent, displayBuffer);


	editor = createNewWin(height, width, startY, startX);

	wrefresh(editor);

	sCursor cursor;
	cursor.y = 0;
	cursor.x = 0;
	cursor.maxY = height-1;
	cursor.maxX = width-1;


	
	attron(COLOR_PAIR(1));
	printw("Press Esc to exit Text Editor (%d, %d)", cursor.x, cursor.y);
	refresh();

	wmove(editor, 0, 0);

	attron(COLOR_PAIR(2));
	wprintw(editor, displayBuffer);
	wmove(editor, cursor.y, cursor.x);

	wrefresh(editor);

	while((ch = getch()) != ESCAPE){ //Get key events and quit if excape is pressed
		keyHandler(&offset, &cursor, ch);
		updateDisplayBuffer(offset, width, size, fcontent, displayBuffer);

		attron(COLOR_PAIR(2));
		mvprintw(0, 0, "Press Esc to exit Text Editor (%d, %d)", cursor.x, cursor.y);
		refresh();
		
		wmove(editor, 0, 0);

		attron(COLOR_PAIR(1));

		wprintw(editor, displayBuffer);
		
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

