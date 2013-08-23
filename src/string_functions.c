
/**
* © 2012 Ilya I. Averkov <admin@jsmart.web.id>
*/

#include "string_functions.h"

#include <time.h>

#ifdef WIN32
char *strndup (const char *s, size_t n) {
	char *result;
	size_t len = strlen(s);
	
	if(n < len) len = n;
	
	result = (char *) malloc(len + 1);
	if(!result) return 0;
	
	result[len] = '\0';
	return (char *) memcpy(result, s, len);
}
#endif

char *readfile(const char *filename) {
	FILE *file = fopen(filename, "r");
	fseek(file, 0, SEEK_END);
	size_t filesize = ftell(file);
	fseek(file, 0, SEEK_SET);
	char *data = (char *) malloc(filesize + 1);
	fread(data, 1, filesize, file);
	fclose(file);
	data[filesize] = 0;
	return data;
}

char *substring(const char* str, size_t begin, size_t len) {
	if(str == 0 || strlen(str) == 0 || strlen(str) < begin || strlen(str) < (begin+len)) return 0;
	return strndup(str + begin, len);
}

long int strpos(const char *haystack, const char *needle) {
	char *position = strstr(haystack, needle);
	return position ? position - haystack : -1;
}

void append(char **source, const char *appendix) {
	size_t string_length = strlen(*source);
	size_t appendix_length = strlen(appendix);
	size_t length = string_length + appendix_length;
	*source = (char *) realloc(*source, length + 1);
	memcpy(*source + string_length, appendix, appendix_length);
	(* source)[length] = 0;
}

char *replaceStatic(char *where, const char *what, const char *with) {
	char *offset = where;
	char *position = 0;
	char *result = calloc(1, 0);
	size_t newlen = strlen(with);
	size_t oldlen = strlen(what);
	
	while(position = strstr(offset, what)) {
		char *prefix = substring(offset, 0, (position - offset));
		append(& result, prefix);
		free(prefix);
		append(& result, with);
		offset = position + oldlen;
	}
	
	append(& result, offset);
	
	return result;
}

void replace(char **where, const char *what, const char *with) {
	char *offset = *where;
	char *position = 0;
	char *result = calloc(1, 0);
	size_t newlen = strlen(with);
	size_t oldlen = strlen(what);
	
	while(position = strstr(offset, what)) {
		char *prefix = substring(offset, 0, (position - offset));
		append(& result, prefix);
		free(prefix);
		append(& result, with);
		offset = position + oldlen;
	}
	
	append(& result, offset);
	
	free(*where);
	*where = result;
}

void replaceWithInteger(char **where, const char *what, unsigned long int with) {
	char *replacement = integerToString(with);
	replace(where, what, replacement);
	free(replacement);
}

void replaceWithSize(char **where, const char *what, unsigned long int bytes) {
	char *replacement = formatSize(bytes);
	replace(where, what, replacement);
	free(replacement);
}

void replaceWithNow(char **where, const char *what) {
	char *replacement = date("%d.%m.%Y %H:%M");
	replace(where, what, replacement);
	free(replacement);
}

char *integerToString(unsigned long int value) {
	char *buf = (char *) calloc(1, 40);
	sprintf(buf, "%lu", value);
	return buf;
}

char *formatDouble(double value) {
	char *buf = (char *) calloc(1, 40);
	sprintf(buf, "%.2F", value);
	return buf;
}

char *formatSize(unsigned long int bytes) {
	double dbytes = (double) bytes;
	char *postfix[] = {" байт", " кб", " Мб", " Гб", " Тб"};
	unsigned short int step = 0;
	while(dbytes > 1024) {
		dbytes /= 1024;
		++ step;
	}
	char *left = formatDouble(dbytes);
	char *result = concat(left, postfix[step]);
	free(left);
	
	return result;
}

char *concat(const char *left, const char *right) {
	size_t left_length = strlen(left);
	size_t right_length = strlen(right);
	size_t length = left_length + right_length;
	char *result = (char *) malloc(length + 1);
	memcpy(result, left, left_length);
	memcpy(result + left_length, right, right_length);
	result[length] = 0;
	return result;
}

char *date(const char *format) {
	time_t rawtime;
	struct tm *timeinfo;
	time(& rawtime);
	timeinfo = localtime(& rawtime);
	char *buf = (char *) calloc(1, 40);
	strftime(buf, 40, format, timeinfo);
	return buf;
}
