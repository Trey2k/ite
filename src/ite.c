#include <ncurses.h>
#include <stdlib.h>
#include <sys/ioctl.h>
#include <unistd.h>

#include "defs.h"
#include "display.h"
#include "key_handler.h"

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

void editor(const char *fname){

	set_escdelay(0);

	FILE *file;
	sContent content[1]; //creating a pointer of the content struct

	if(!(file = fopen(fname, "r+"))){
		return;
	}

	fseek(file, 0, SEEK_END); //Getting the size of the file
	content->size = ftell(file);
	fseek(file, 0, SEEK_SET);

	content->fcontent = malloc(content->size); //Allocationg space for the file in memory
	fread(content->fcontent, 1, content->size, file); //Reading content from file

	WINDOW *editor;
	int startX, startY;

	sDisplay display[1]; //Creating a pointer of the display stufct
	display->width = 0;
	display->height = 0;
	display->rowCount = 0;
	display->offset = 0;

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
	display->height = w.ws_row - startY;
	display->width = w.ws_col - startX;

	//Counting the number of rows
	for(int i = 0; i < content->size; i++){
		if(content->fcontent[i] == '\n'){
			display->rowCount++;
		}
	}

	//Allocating memory for the contentLength array
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


	//Allocating memory for the displayBuffer
	display->displayBuffer = malloc((display->height * sizeof(char *)));
	if(display->displayBuffer == NULL){
			fprintf(stderr, "out of memory\n");
			exit(-1);
	}

	//Allocating memory for the displayBuffer of i
	for(int i = 0; i < display->height; i++){
		display->displayBuffer[i] = malloc(display->width * sizeof(char));
		if(display->displayBuffer[i] == NULL){
			fprintf(stderr, "out of memory\n");
			exit(-1);
		}
	}

	//Updating the display buffer with files content
	updateDisplayBuffer(display, content);

	//Creating the editor window
	editor = newwin(display->height, display->width, startY, startX);
	

	sCursor cursor[1]; //Creating a pointer of the cursor struct
	cursor->y = 0;
	cursor->x = 0;
	cursor->maxY = display->height-1;
	cursor->maxX = display->width-1;

	//Setting color and printing top info
	attron(COLOR_PAIR(2));
	printw("Press Esc to exit Text Editor (X: %d, Y: %d)", cursor->x, cursor->y);
	refresh();

	//Printing out display buffer
	use_default_colors();
	for(int i = 0; i < display->height; i++){
		mvwprintw(editor, i, 0, display->displayBuffer[i]);
	}
	
	//Moving cursor back to original position after printing
	wmove(editor, cursor->y, cursor->x);

	//Refreshing window
	wrefresh(editor);

	int ch;
	while((ch = getch()) != ESCAPE){ //Get key events and quit if excape is pressed
		keyHandler(display, content, cursor, ch);
		updateDisplayBuffer(display, content);

		clear();

		attron(COLOR_PAIR(2));
		mvprintw(0, 0, "Press Esc to exit Text Editor (X: %d, Y: %d)", cursor->x, cursor->y);
		refresh();
		
		
		//Reprinting displayBuffer to window
		for(int i = 0; i < display->height; i++){
			mvwprintw(editor, i, 0, display->displayBuffer[i]);
		}
		
		wmove(editor, cursor->y, cursor->x);
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

