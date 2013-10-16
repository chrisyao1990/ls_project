/*
 *   CS631 Advanced Programming in the UNIX Environment
 *               Project 1, ls implementation
 *           ls [-AacdFfhiklnqRrSstuw1] [file ...]
 *                  Author: Kun Yao
 *                  Date:Sep,20,2013
 */

#include <sys/stat.h>
#include <sys/types.h>

#include <fts.h>

#define BYTE (1024L)
#define K    (1024L*BYTE)
#define M    (1024L*K)


void modeprint(mode_t mode, char *buf);
void signprint(mode_t mode);
void qfroceprint(char *a, char *b);
void blkprint(struct stat *q);
void readblesizeprint(off_t size);
void timeprint(time_t *rawtime);
void linkprint(FTSENT *p);

