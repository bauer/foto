#
# Makefile
# bauer.sweden@gmail.com
#
CC = gcc
#CFLAGS += -ggdb -Wall -DDEBUG -I.
#CFLAGS += -ggdb -Wall -DTEST -I.
CFLAGS += -ggdb -Wall -I.
SOURCES = main.c parse.c\
	./libexif/exif-byte-order.c	\
	./libexif/exif-content.c		\
	./libexif/exif-data.c		\
	./libexif/exif-entry.c		\
	./libexif/exif-format.c		\
	./libexif/exif-ifd.c		\
	./libexif/exif-loader.c		\
	./libexif/exif-log.c		\
	./libexif/exif-mem.c		\
	./libexif/exif-mnote-data.c	\
	./libexif/exif-mnote-data-priv.h	\
	./libexif/exif-tag.c		\
	./libexif/exif-utils.c		\
	./libexif/i18n.h

OBJECTS = $(SOURCES:.c=.o)
EXECUTABLE=foto

.PHONY: all run clean buildstr1 buildstr2

.c.o:
	$(CC) $(CFLAGS) -c $<


all : buildstr1 $(OBJECTS) buildstr2
	$(CC) *.o -o $(EXECUTABLE) -lm

run:
	$(EXECUTABLE)

clean:
	rm -rf *.o ./libexif/*.o $(EXECUTABLE) *~

buildstr1:
	@echo
	@echo Building $(EXECUTABLE)...
	@echo

buildstr2:
	@echo
	@echo Linking $(EXECUTABLE)...
	@echo

