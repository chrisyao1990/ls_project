/*
 *   CS631 Advanced Programming in the UNIX Environment
 *               Project 1, ls implementation
 *           ls [ −AacdFfhiklnqRrSstuw1] [file ...]
 *                  Author: Kun Yao
 *                  Date:Sep,20,2013
 */
#include <sys/stat.h>
#include <sys/types.h>


#include <errno.h>
#include <fcntl.h>
#include <fts.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "extern.h"
#include "ls.h"

void modeprint(mode_t mode, char *buf)
{
    strcpy(buf, "----------");
    if(S_ISDIR(mode))
        buf[0] = 'd';
    if(S_ISCHR(mode))
        buf[0] = 'c';
    if(S_ISBLK(mode))
        buf[0] = 'b';
    if(S_ISLNK(mode))
        buf[0] = 'l';
    if(S_ISSOCK(mode))
        buf[0] = 's';
    if(S_ISFIFO(mode))
        buf[0] = 'p';
    if(mode & S_IRUSR)
        buf[1] = 'r';
    if(mode & S_IWUSR)
        buf[2] = 'w';
    if(mode & S_IXUSR)
        buf[3] = 'x';
    if(mode & S_ISUID)
        buf[3] = 's';
    if(mode & S_IRGRP)
        buf[4] = 'r';
    if(mode & S_IWGRP)
        buf[5] = 'w';
    if(mode & S_IXGRP)
        buf[6] = 'x';
    if(mode & S_ISGID)
        buf[6] = 'x';
    if(mode & S_IROTH)
        buf[7] = 'r';
    if(mode & S_IWOTH)
        buf[8] = 'w';
    if(mode & S_IXOTH)
        buf[9] = 'x';
    if(mode & S_ISVTX)
        buf[9] = 't';
}