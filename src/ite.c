#include <ncurses.h>
#include <stdio.h>
#include <stdlib.h>

#include "key_defs.h"

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

void keyHandler(int ch, bool *running){
	if(ch == EXCAPE){
		*running = false;
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

	initscr(); //Ncruses init
	bool running = true;
	addstr(fcontent);
	refresh();

	while(running){
		int ch;
		ch = getch(); //Get key press events
		keyHandler(ch, &running);
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

