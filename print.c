/*
 *   CS631 Advanced Programming in the UNIX Environment
 *               Project 1, ls implementation
 *           ls [ −AaCcdFfhiklnqRrSstuwx1] [file ...]
 *                  Author: Kun Yao
 *                  Date:Sep,20,2013
 */
#include <sys/stat.h>
#include <sys/types.h>

#include <ctype.h>
#include <errno.h>
#include <fcntl.h>
#include <fts.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "extern.h"
#include "ls.h"

void modeprint(mode_t mode, char *buf){
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
    //#define S_IFWHT  0160000  /* whiteout */
    if((mode&S_IFMT)==0160000)
        buf[0] = 'w';
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

void signprint(mode_t mode){
    switch(mode & S_IFMT){
    case S_IFDIR:
        putchar('/');
        break;
    case S_IFLNK:
        putchar('@');
        break;
    //#define S_IFWHT  0160000  /* whiteout */
    case 0160000:
        putchar('%');
        break;
    case S_IFSOCK:
        putchar('=');
        break;
    case S_IFIFO:
        putchar('|');
        break;
    default:
        if(mode & (S_IXUSR | S_IXGRP | S_IXOTH)){
            putchar('*');
        }else{
            putchar(' ');
        }
        break;
    }
}

void qfroceprint(char *a, char *b){
    char *p, *q;

    for(p=a, q=b; *p; p++, q++){
        if(isprint(*p))
            *q = *p;
        else
            *q = '?';
    }
} 

void blkprint(struct stat *q){ 
    if(h_flag&&(!l_flag)){
        readblesizeprint(q->st_size);
    }else{
        printf("%*d ",b.bf_blklen,
           (int)((float)q->st_blocks*blktimes));
    }  
}

void readblesizeprint(off_t size){
    float size_tmp;
    if(size>=0 && size<=999){
        printf(" %3dB ",(int)size);
    }else if(size>999 && size<(off_t)(999*BYTE+512)){
        size_tmp = ((float)size / (float)BYTE);
        if(size_tmp < 10){
            printf(" %1.1fK ",(size_tmp));
        }else{
            printf(" %3dK ",(int)(size_tmp));
        }
    }else if(size>=(off_t)(999*BYTE+512) && size<(long int)(999*K+512)){
        size_tmp = ((float)size / (float)K);
        if(size_tmp < 10){
            printf(" %1.1fM ",(size_tmp));
        }else{
            printf(" %3dM ",(int)(size_tmp));
        }
    }else if(size>=(long int)(999*K+512) && size<=(long int)(999*M+512)){
        size_tmp = ((float)size / (float)M);
        if(size_tmp < 10){
            printf(" %1.1fG ",(size_tmp));
        }else{
            printf(" %3dG ",(int)(size_tmp));
        }
    }else{
        printf("     ");
    }
}

void timeprint(time_t *rawtime){
    struct tm * timeinfo = localtime(rawtime);
    static const char mon_name[][4] = {
    "Jan", "Feb", "Mar", "Apr", "May", "Jun",
    "Jul", "Aug", "Sep", "Oct", "Nov", "Dec"};
    if(timeinfo){
        printf("%.3s%3d %.2d:%.2d ",
        mon_name[timeinfo->tm_mon],
        timeinfo->tm_mday,
        timeinfo->tm_hour,
        timeinfo->tm_min);
    }else
        printf("             ");
}

void linkprint(FTSENT *p){
    int lnklen;
    char name[256], path[256];

    if(p->fts_level == 0)
        snprintf(name, sizeof(name), "%s", p->fts_name);
    else 
        snprintf(name, sizeof(name), "%s/%s", p->fts_parent->fts_accpath, p->fts_name);    
    if((lnklen = readlink(name, path, sizeof(path) - 1)) == -1){
        fprintf(stderr, "\n%s error: %s\n", name, strerror(errno));
        return;
    }
    path[lnklen] = '\0';
    printf(" -> %s", path);
}





