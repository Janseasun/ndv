#include "log.h"
#include <stdio.h>
#include <stdarg.h>

void plog (char *template, ...){
        va_list ap;
        char string[2048];
	static FILE *fp = NULL;

	if (fp == NULL){
		fp = fopen(logfile, "a");
	}

        va_start(ap, template);
        vsprintf(string, template, ap);
        va_end(ap);
	fputs(string, fp);
	fputs("\n", fp);

	// fclose (fp);
}

