
/**
* Gallerizer — an image gallery indexing tool
* CLI version (pure libjpeg, no GD required)
* © 2012 Ilya I. Averkov (WST) <admin@jsmart.web.id>
*/

#include <stdio.h>
#include <stdlib.h>
#include <dirent.h>
#include <string.h>
#include <math.h>

#include "string_functions.h"
#include "image.h"

#ifdef WIN32
#include <windows.h>
#else
#include <sys/stat.h>
#include "iniparser.h"
#endif

typedef struct GalleryItem {
	struct GalleryItem *next;
	char *filename;
	unsigned long int filesize;
	unsigned long int preview_size;
	unsigned long int slide_size;
	unsigned int width;
	unsigned int height;
} GalleryItem;

GalleryItem *top = 0;
char *collection_title = 0;

#ifndef WIN32
dictionary *configuration = 0;
#endif

void addNewPhoto(const char *filename, unsigned long int filesize, unsigned int preview_width, unsigned int preview_height, unsigned long int preview_size, unsigned long int slide_size) {
	GalleryItem *newitem = (GalleryItem *) malloc(sizeof(GalleryItem));
	newitem->next = top;
	newitem->filename = strdup(filename);
	newitem->filesize = filesize;
	newitem->width = preview_width;
	newitem->height = preview_height;
	newitem->preview_size = preview_size;
	newitem->slide_size = slide_size;
	top = newitem;
}

unsigned long int filesize(const char *filename) {
#ifdef WIN32
	FILE *file = fopen(filename, "r");
	fseek(file, 0, SEEK_END);
	size_t size = ftell(file);
	fclose(file);
	return size;
#else
	struct stat st;
	stat(filename, & st);
	return st.st_size;
#endif
}

void renderPage() {
	// Шаблон страницы
	char *template = readfile(GALLERIZER_SHARED "/template/index.htt");
	if(template == 0) {
		consoleError("template file does not exist, exitting");
		exit(50);
	}
	replace(& template, "{TITLE}", collection_title);
	
	// Генерируем HTML-код
	char *html = calloc(1, 0);
	append(& html, "<tr>");
	
	unsigned long int iteration = 0;
	unsigned long int size = 0;
	unsigned long int previews_size = 0;
	unsigned long int slides_size = 0;
	
	while(top) {
		GalleryItem *current = top;
		top = current->next;
		
		if(++ iteration % 5 == 0) append(& html, "</tr><tr>");
		
		char *cell = calloc(1, 2048);
		
		sprintf(cell, "<td width=\"20%\" align=\"center\" valign=\"middle\"><a href=\"slides/%s\" rel=\"lightbox[group]\" title=\"&lt;a href=&quot;%s&quot;&gt;%s&lt;/a&gt;\"><img src=\"previews/%s\" width=\"%d\" height=\"%d\" /></a></td>", current->filename, current->filename, current->filename, current->filename, current->width, current->height);
		append(& html, cell);
		
		free(cell);
		
		size += current->filesize;
		previews_size += current->preview_size;
		slides_size += current->slide_size;
	}
	append(& html, "</tr>");
	
	replace(& template, "{PHOTOS}", html);
	replaceWithInteger(& template, "{COUNT}", iteration);
	replaceWithSize(& template, "{SIZE}", size);
	replaceWithSize(& template, "{PREVIEWS_SIZE}", previews_size);
	replaceWithSize(& template, "{SLIDES_SIZE}", slides_size);
	replaceWithSize(& template, "{TOTAL_SIZE}", slides_size + previews_size + size);
	replaceWithNow(& template, "{UPDATED}");
	
	// Результирующая страница
	FILE *page = fopen("index.html", "w");
	fputs(template, page);
	fclose(page);
}

void installStuff() {
#ifdef WIN32
	// TODO
#else
	char *command = "cp -r " GALLERIZER_SHARED "/js " GALLERIZER_SHARED "/css " GALLERIZER_SHARED "/images .";
	system(command);
#endif
}

void createDirectories() {
#ifdef WIN32
	int previews = mkdir("previews");
	int slides = mkdir("slides");
#else
	int previews = mkdir("previews", S_IRWXU | S_IRWXG | S_IRWXO);
	int slides = mkdir("slides", S_IRWXU | S_IRWXG | S_IRWXO);
#endif
}

Image *resizeImage(Image *image, unsigned int width, unsigned int height) {
	Image *dest;
	
	if(image->width > width || image->height > height) {
		double w = (double) image->width / width;
		double h = (double) image->height / height;
		dest = (w > h) ? resample(image, width, floor(image->height / w)) : resample(image, floor(image->width / h), height);
	} else {
		dest = resample(image, image->width, image->height);
	}
	
	return dest;
}

int main(int argc, char **argv) {
	
	// Про мну
	consoleSimpleMessage("\n Gallerizer 0.2 — an image gallery indexer\n Created by Ilya Averkov <ilya.averkov.net>\n");
	
	// Проверим синтаксис команды
	if(argc != 2) {
		char *message = "Usage: {CMD} <collection-title>";
		consoleError(replaceStatic(message, "{CMD}", argv[0]));
		return 1;
	}
	
#ifndef WIN32
	configuration = iniparser_load(CONFIG_FILENAME);
#endif
	
	// Создаём каталоги
	createDirectories();
	
#ifndef WIN32
	// Параметры коллекции
	collection_title = argv[1];
	unsigned int slide_width = iniparser_getint(configuration, "slide_width", 640);
	unsigned int preview_width = iniparser_getint(configuration, "preview_width", 160);
	unsigned int slide_height = slide_width * 3 / 4;
	unsigned int preview_height = preview_width * 3 / 4;
#else
	unsigned int slide_width = 640;
	unsigned int preview_width = 480;
	unsigned int slide_height = slide_width * 3 / 4;
	unsigned int preview_height = preview_width * 3 / 4;
#endif
	
	// Откроем каталог с изображениями (текущий)
	DIR *directory;
	struct dirent *entry;
	for(directory = opendir("."); (entry = readdir(directory)) != 0;) {
		if(strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0) continue;
		
		// Расширение файла должно быть jpg, мы пока не умеем работать с другими типами файлов
		if(strstr(entry->d_name, ".jpg") != entry->d_name + strlen(entry->d_name) - 4) continue;
		
		// Загрузим изображение в память
		Image *image = openJpeg(entry->d_name);
		
		// Если файл не является изображением jpeg
		if(image == 0) continue;
		
		char *message = "<{FILENAME}> processing file";
		char *outmessage = replaceStatic(message, "{FILENAME}", entry->d_name);
		consoleMessage(outmessage);
		free(outmessage);
		
		// Создадим слайд и эскиз. Для более быстрой работы эскиз делаем из слайда.
		Image *slide = resizeImage(image, slide_width, slide_height);
		Image *preview = resizeImage(slide, preview_width, preview_height);
		
		// Сохраним слайд в файл
		char *slide_filename = concat("slides/", entry->d_name);
#ifndef WIN32
		saveAsJpeg(slide, slide_filename, iniparser_getint(configuration, "slide_quality", 95));
#else
		saveAsJpeg(slide, slide_filename, 95);
#endif
		
		// Сохраним эскиз в файл
		char *preview_filename = concat("previews/", entry->d_name);
#ifndef WIN32
		saveAsJpeg(preview, preview_filename, iniparser_getint(configuration, "preview_quality", 60));
#else
		saveAsJpeg(preview, preview_filename, 60);
#endif
		
		// Добавляем изображение в список
		addNewPhoto(entry->d_name, filesize(entry->d_name), preview->width, preview->height, filesize(preview_filename), filesize(slide_filename));
		free(preview_filename);
		free(slide_filename);
		
		// Освободим память от изображения, слайда и эскиза
		disposeImage(image);
		disposeImage(slide);
		disposeImage(preview);
	}
	
	// Каталог разобран, можно закрыть
	closedir(directory);
	
	// Сгенерируем страницу
	consoleMessage("Rendering HTML page");
	renderPage();
	installStuff();
	
	// Готово
	consoleWarning("Job done");
	return 0;
}
