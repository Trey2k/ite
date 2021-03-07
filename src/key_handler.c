#include "key_handler.h"

#include <ncurses.h>

void keyHandler(sDisplay *display, sContent *content, sCursor *cursor, int ch){
	int screenPadding = 1; //screenPadding is needed so there is room on the screen for thee cursor to go past the last charector.
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
            addChar(display, content, cursor, ch);
        }

		//If cursor is going past end of the screen on the right or end of word and it was a change in the X direction, scroll or wrap around
		if(cursor->x > cursor->maxX || (cursor->x + display->offset > display->displayLength[cursor->y] && xChange)){ 
				if(display->offset + display->width+1 > display->displayLength[cursor->y]){
					display->offset = 0;
					cursor->x = 0;
					cursor->y++;
				}else{
					cursor->x = cursor->maxX;
					display->offset++;
				}
		}//If cursor is going past the end of the screen on the left scroll or wrap around
		else if(cursor->x < 0){
				if(display->offset - 1 <= 0){
					cursor->x = cursor->maxX;
					cursor->y--;
					display->offset = display->displayLength[cursor->y] - display->width + screenPadding;
					if(display->offset <= 0){
						display->offset = 0;
						cursor->x = display->displayLength[cursor->y];
					}
				}else{
					cursor->x = 0;
					display->offset--;
				}
		}//If cursor was moved down a line further than edge of the content wrap to content
		else if(cursor->x + display->offset > display->displayLength[cursor->y] && yChange){
			if(display->displayLength[cursor->y]- display->offset < 0){
				display->offset = display->displayLength[cursor->y] - display->width + screenPadding;
				if(display->offset < 0){
					display->offset = 0;
				}
			}
			cursor->x = (display->displayLength[cursor->y]) - display->offset;
		}
		
		if(cursor->y > display->rowCount){
			cursor->y = display->rowCount;
		}

		if(cursor->y < 0){
				cursor->y = 0;
		}else if(cursor->y > cursor->maxY){
				cursor->y = cursor->maxY;
		}
	
}

void addChar(sDisplay *display, sContent *content, sCursor *cursor, int ch){
    int index = 0;
    char *temp = malloc(++content->size); //Allocationg space for the file in memory
    cursor->x++;

    for(int i = 0; i < cursor->y; i++){
        index += content->contentLength[cursor->y];
    }
	index += display->offset + cursor->x;

    for(int i = 0; i < index; i++){
        temp[i] = content->fcontent[i];
    }

    content->fcontent[index] = ch;
	
    for(int i = index+1; i < content->size; i++){
        temp[i] = content->fcontent[i-1];
    }

    free(content->fcontent);
    content->fcontent = temp;
}