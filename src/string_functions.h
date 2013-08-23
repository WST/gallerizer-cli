
/**
* Gallerizer — an image gallery indexing tool
* CLI version (pure libjpeg, no GD required)
* © 2012 Ilya I. Averkov (WST) <admin@jsmart.web.id>
*/

#ifndef STRING_FUNCTIONS_H
#define STRING_FUNCTIONS_H

#include <stdlib.h>
#include <string.h>
#include <stdio.h>

char *readfile(const char *filename);
char *substring(const char* str, size_t begin, size_t len);
void replace(char **where, const char *what, const char *with);
void append(char **source, const char *appendix);
char *concat(const char *left, const char *right);
char *integerToString(unsigned long int value);
char *formatDouble(double value);
void replaceWithSize(char **where, const char *what, unsigned long int bytes);
void replaceWithInteger(char **where, const char *what, unsigned long int with);
char *replaceStatic(char *where, const char *what, const char *with);
char *date(const char *format);
char *formatSize(unsigned long int bytes);

#endif
