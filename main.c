/*
 *   CS631 Advanced Programming in the UNIX Environment
 *               Project 1, ls implementation
 *           ls [ −AacdFfhiklnqRrSstuw1] [file ...]
 *                  Author: Kun Yao
 *                  Date:Sep,20,2013
 */

#include "extern.h"
#include "ls.h"

int 
main(int argc,char** argv){
	if(argc == 1){
		return (simple_ls(argc,argv));
	}else{
		return (do_ls(argc,argv));
	}
}