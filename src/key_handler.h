#ifndef KEY_HANDLER_H
#define KEY_HANDLER_H
#include "defs.h"
#include "display.h"

extern void keyHandler(sDisplay *display, sContent *content, sCursor *cursor, int ch);

void addChar(sDisplay *display, sContent *content, sCursor *cursor, int ch);
#endif