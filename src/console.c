
/**
* Gallerizer — an image gallery indexing tool
* CLI version (pure libjpeg, no GD required)
* © 2012 Ilya I. Averkov (WST) <admin@jsmart.web.id>
*/

#include "console.h"

void consoleSimpleMessage(const char *text) {
#ifndef WIN32
	printf("\033[22;32m");
#endif
	printf(text);
#ifndef WIN32
	printf("\033[0m\n");
#endif
}

void consoleMessage(const char *text) {
	char *time = date("%H:%M:%S");
#ifndef WIN32
	printf("\033[22;37m");
#endif
	printf("[%s] %s", time, text);
#ifndef WIN32
	printf("\033[0m\n");
#endif
	free(time);
}

void consoleWarning(const char *text) {
	char *time = date("%H:%M:%S");
#ifndef WIN32
	printf("\033[01;33m");
#endif
	printf("[%s] %s", time, text);
#ifndef WIN32
	printf("\033[0m\n");
#endif
	free(time);
}

void consoleError(const char *text) {
	char *time = date("%H:%M:%S");
#ifndef WIN32
	printf("\033[22;31m");
#endif
	printf("[%s] %s", time, text);
#ifndef WIN32
	printf("\033[0m\n");
#endif
	free(time);
}
