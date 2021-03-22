#include <time.h>
#include <stdio.h>
#include "log.h"

void initLog(char *name)
{
    logfile = fopen(name, "a+");
}

void logStatus(float ti, float te, float tu, float control)
{   
    time_t rawtime;
    time(&rawtime);

    struct tm *ptm = localtime(&rawtime);
    char message[100];

    strftime(message, 100, "%DT%T", ptm);

    fprintf(logfile,"%s,%.4f,%.4f,%.4f,%.4f\n", message, ti, te, tu, control);

}

void closeLog()
{
    fclose(logfile);
}
