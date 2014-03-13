#!/bin/bash

PROJECT_TARGET="gallerizer"
PROJECT_SHARED="dist/shared.tar.bz2"
PROJECT_CONFIG="dist/gallerizer.ini"
PROJECT_FILES="`cd src && ls *.c*`"
CFLAGS="-ffast-math -Os -pipe"
#CFLAGS="-g"
COMPILER="`which gcc`"
INSTALL="`which install`"
CP="`which cp`"
TAR="`which tar`"
INCLUDE=""
LIBS="-lm -ljpeg"
BINDIR="/usr/bin"
CONFIGDIR="/etc"
SHAREDIR="/usr/share"

DEFINE="-DUNIX -DCONFIG_FILENAME=\"${CONFIGDIR}/${PROJECT_TARGET}.ini\" -DGALLERIZER_SHARED=\"${SHAREDIR}/${PROJECT_TARGET}\""

if [[ ! -d build ]]; then
	mkdir build
fi

for i in ${PROJECT_FILES}; do
	NAME=$(echo $i | sed -e 's/\.[^.]*$//')
	COMMAND="${COMPILER} -c -I. ${INCLUDE} ${CFLAGS} ${DEFINE} -o ./build/${NAME}.o ./src/${i}"
	echo ${COMMAND}
	${COMMAND} || { echo "Failed to compile ${i}!"; exit 1; }
	PROJECT_OBJECT="${PROJECT_OBJECT} ./build/${NAME}.o"
done

COMMAND="${COMPILER} -o ./${PROJECT_TARGET} ${PROJECT_OBJECT} ${LIBS}"
echo ${COMMAND}
${COMMAND} || { echo "Failed to link executable!"; exit 1; }

COMMAND="${INSTALL} -m 644 ${PROJECT_CONFIG} ${CONFIGDIR}/${PROJECT_TARGET}.ini"
echo ${COMMAND}
${COMMAND} || { echo "Failed to install configuration file! Check permissions and run this script again."; exit 1; }

COMMAND="${INSTALL} -m 755 ./${PROJECT_TARGET} ${BINDIR}"
echo ${COMMAND}
${COMMAND} || { echo "Failed to install executable! Check permissions and run this script again."; exit 1; }

COMMAND="${TAR} -xvf ${PROJECT_SHARED} --directory ${SHAREDIR}";
echo ${COMMAND}
${COMMAND} || { echo "Failed to install shared files! Check permissions and run this script again."; exit 1; }

COMMAND="rm -rf build gallerizer"
echo ${COMMAND}
${COMMAND} || { echo "Failed to clean up"; exit 1; }
