CC=gcc
CFLAGS=-Wall -pthread
LDFLAGS=-lgtk-3 -lgdk-3 -latk-1.0 -lgio-2.0 -lpangoft2-1.0 -lpangocairo-1.0 -lgdk_pixbuf-2.0 -lpng14 -lm -lcairo-gobject -lcairo -lpango-1.0 -lfreetype -lfontconfig -lgobject-2.0 -lgmodule-2.0 -lgthread-2.0 -lrt -lglib-2.0 -lnotify -liniparser
INCLUDES=-I/usr/include/glib-2.0 -I/usr/lib/glib-2.0/include -I/usr/include/gtk-3.0 -I/usr/include/atk-1.0 -I/usr/include/cairo -I/usr/include/gdk-pixbuf-2.0 -I/usr/include/pango-1.0 -I/usr/include/pixman-1 -I/usr/include/freetype2 -I/usr/include/libpng14
DEFINITIONS=-DGSEAL_ENABLE
SOURCES=notifmed.c
OBJECTS=notifmed.o
TARGET=notifmed
all: $(OBJECTS)
	$(CC) -o $(TARGET) $(OBJECTS) $(LDFLAGS)

notifmed.o:
	$(CC) -c notifmed.c $(CFLAGS) $(INCLUDES) $(DEFINITIONS)
