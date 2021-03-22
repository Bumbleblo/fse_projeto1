#include <stdio.h>

FILE *logfile;

void initLog(char *name);

void logStatus(float ti, float te, float tu, float control);

void closeLog();
