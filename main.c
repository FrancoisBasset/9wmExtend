#include <X11/Xlib.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <unistd.h>
#include <X11/cursorfont.h>

#include "main.h"

Display *display;
Window window;
int screen_number;
Screen *screen;
Visual *visual;
Colormap colormap;

XColor panel_color;
XGCValues gcv_panel;
GC gc_panel;

XColor date_color;
XGCValues gcv_date;
GC gc_date;

XColor firefox_color;
XGCValues gcv_firefox;
GC gc_firefox;

int width;
int height;

Font font;

Cursor pointer;
Cursor hand;

int firefox = 0;

int main(void) {
	init();

	char *original_date = get_date();
	draw_panel();
	draw_firefox_button();
	refresh(original_date);

	XEvent event;

	while (1) {
		if (XPending(display)) {
			XNextEvent(display, &event);
		}

		switch (event.type) {
			case Expose:
				draw_panel();
				draw_firefox_button();
				refresh(original_date);
				break;
			case MotionNotify:
				if (event.xmotion.x > 10 && event.xmotion.x <= 140 && event.xmotion.y > height - 45 && event.xmotion.y <= height - 5) {
					XDefineCursor(display, window, hand);
					firefox = 1;
				} else {
					XDefineCursor(display, window, pointer);
					firefox = 0;
				}
				break;
		}

		char *date = get_date();

		if (strcmp(original_date, date) != 0) {
			refresh(date);
			strcpy(original_date, date);
		}

		free(date);

		//sleep(1);
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

	panel_color.red = 65535;
	panel_color.green = 25000;
	XAllocColor(display, colormap, &panel_color);
	gcv_panel.foreground = panel_color.pixel;
	gc_panel = XCreateGC(display, window, GCForeground, &gcv_panel);

	font = XLoadFont(display, "-bitstream-*-bold-r-normal--33-240-100-100-*-206-*-*");

	date_color.red = 65535;
	date_color.green = 65535;
	date_color.blue = 65535;
	XAllocColor(display, colormap, &date_color);
	gcv_date.foreground = date_color.pixel;
	gcv_date.font = font;
	gc_date = XCreateGC(display, window, GCForeground | GCFont, &gcv_date);

	firefox_color.red = 65535;
	firefox_color.green = 65535;
	firefox_color.blue = 0;
	XAllocColor(display, colormap, &firefox_color);
	gcv_firefox.foreground = firefox_color.pixel;
	gcv_firefox.font = font;
	gc_firefox = XCreateGC(display, window, GCForeground | GCFont, &gcv_firefox);
	
	width = XWidthOfScreen(screen);
	height = XHeightOfScreen(screen);

	hand = XCreateFontCursor(display, XC_hand1);
	pointer = XCreateFontCursor(display, XC_left_ptr);

	XMapWindow(display, window);
	XSelectInput(display, window, ExposureMask | ButtonPressMask | PointerMotionMask);
}

char* get_date(void) {
	time_t timestamp = time(NULL);
	struct tm *date = localtime(&timestamp);

	char *days[7] = { "dimanche", "lundi", "mardi", "mercredi", "jeudi", "vendredi", "samedi" };
	char *months[12] = { "janvier", "fevrier", "mars", "avril", "mai", "juin", "juillet", "aout", "septembre", "octobre", "novembre", "decembre"};

	char *date_str = malloc(sizeof(char) * 100);
	sprintf(date_str, "%s %d %s %02d:%02d:%02d", days[date->tm_wday], date->tm_mday, months[date->tm_mon], date->tm_hour, date->tm_min, date->tm_sec);

	return date_str;
}

void refresh(char *date) {
	int text_width = XTextWidth(XQueryFont(display, font), date, strlen(date));
	XDrawString(display, window, gc_date, width - text_width, height - 15, date, strlen(date));
}

void draw_panel(void) {
	XFillRectangle(display, window, gc_panel, 0, height - 50, width, 50);
}

void draw_firefox_button(void) {
	int black_pixel = XBlackPixel(display, screen_number);
	XGCValues gcv;
	gcv.foreground = black_pixel;
	gcv.font = font;
	GC gc = XCreateGC(display, window, GCForeground | GCFont, &gcv);
	XFillRectangle(display, window, gc_firefox, 10, height - 45, 130, 40);
	XDrawString(display, window, gc, 15, height - 15, "Firefox", 7);
}
