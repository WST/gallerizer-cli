@SET CC="C:\MinGW\bin\gcc.exe"
@SET STRIP="C:\MinGW\bin\strip.exe"

%CC% -Isrc -Os -DWIN32 -pipe -ffast-math -c src/string_functions.c -o build/string_functions.o
%CC% -Isrc -Os -DWIN32 -pipe -ffast-math -c src/console.c -o build/console.o
%CC% -Isrc -I"C:\projects\jpeg-8" -Os -DWIN32 -pipe -ffast-math -c src/image.c -o build/image.o
%CC% -Isrc -Os -DWIN32 -DGALLERIZER_SHARED="\"C:\\gallerizer\\shared\"" -pipe -ffast-math -c src/gallerizer.c -o build/gallerizer.o

%CC% -o gallerizer.exe -Os -pipe -ffast-math build/gallerizer.o build/string_functions.o build/iniparser.o build/dictionary.o build/console.o build/image.o -L"C:\projects\jpeg-8" -ljpeg