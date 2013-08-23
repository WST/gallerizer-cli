
/**
* Gallerizer — an image gallery indexing tool
* CLI version (pure libjpeg, no GD required)
* © 2012 Ilya I. Averkov (WST) <admin@jsmart.web.id>
*/

#ifndef CONSOLE_H
#define CONSOLE_H

#include "string_functions.h"

void consoleSimpleMessage(const char *text);
void consoleMessage(const char *text);
void consoleWarning(const char *text);
void consoleError(const char *text);

#endif
