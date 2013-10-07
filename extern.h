/*
 *   CS631 Advanced Programming in the UNIX Environment
 *               Project 1, ls implementation
 *           ls [ −AacdFfhiklnqRrSstuw1] [file ...]
 *                  Author: Kun Yao
 *                  Date:Sep,20,2013
 */

#include <sys/stat.h>
#include <sys/types.h>

void modeprint(mode_t mode, char *buf);
