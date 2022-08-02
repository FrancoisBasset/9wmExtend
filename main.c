#include <X11/Xlib.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <unistd.h>

#include "main.h"

Display *display;
Window window;
int screen_number;
Screen *screen;
Visual *visual;
Colormap colormap;

XColor green;
XGCValues gcv_panel;
GC gc_panel;

XColor red;
XGCValues gcv_date;
GC gc_date;

int width;
int height;

Font font;

int main(void) {
	init();

	char *original_date = get_date();
	draw_panel();
	refresh(original_date);

	XEvent event;

	while (1) {
		if (XPending(display)) {
			XNextEvent(display, &event);
		}

		switch (event.type) {
			case Expose:
				draw_panel();
				refresh(original_date);
				break;
		}

		char *date = get_date();

		if (strcmp(original_date, date) != 0) {
			refresh(date);
			strcpy(original_date, date);
		}

		free(date);

		sleep(1);
	}

	free(original_date);
	XDestroyWindow(display, window);
	XCloseDisplay(display);
}

void init(void) {
	display = XOpenDisplay("");
	window = XDefaultRootWindow(display);
	screen_number = XDefaultScreen(display);
	screen = XDefaultScreenOfDisplay(display);
	visual = XDefaultVisual(display, screen_number);
	colormap = XCreateColormap(display, window, visual, AllocNone);

	green.green = 65535;
	XAllocColor(display, colormap, &green);
	gcv_panel.foreground = green.pixel;
	gc_panel = XCreateGC(display, window, GCForeground, &gcv_panel);

	font = XLoadFont(display, "-bitstream-*-bold-r-normal--33-240-100-100-*-206-*-*");

	red.red = 65535;
	XAllocColor(display, colormap, &red);
	gcv_date.foreground = red.pixel;
	gcv_date.font = font;
	gc_date = XCreateGC(display, window, GCForeground | GCFont, &gcv_date);

	width = XWidthOfScreen(screen);
	height = XHeightOfScreen(screen);

	XMapWindow(display, window);
	XSelectInput(display, window, ExposureMask);
}

char* get_date(void) {
	time_t timestamp = time(NULL);
	struct tm *date = localtime(&timestamp);

	char *days[7] = { "dimanche", "lundi", "mardi", "mercredi", "jeudi", "vendredi", "samedi" };
	char *months[12] = { "janvier", "fevrier", "mars", "avril", "mai", "juin", "juillet", "aout", "septembre", "octobre", "novembre", "decembre"};

	char *date_str = malloc(sizeof(char) * 100);
	sprintf(date_str, "%s %d %s %d:%d:%d", days[date->tm_wday], date->tm_mday, months[date->tm_mon], date->tm_hour, date->tm_min, date->tm_sec);

	return date_str;
}

void refresh(char *date) {
	int text_width = XTextWidth(XQueryFont(display, font), date, strlen(date));
	XDrawString(display, window, gc_date, width - text_width, height - 15, date, strlen(date));
}

void draw_panel(void) {
	XFillRectangle(display, window, gc_panel, 0, height - 50, width, 50);
}
