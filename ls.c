/*
 *   CS631 Advanced Programming in the UNIX Environment
 *               Project 1, ls implementation
 *           ls [ −AacdFfhiklnqRrSstuw1] [file ...]
 *                  Author: Kun Yao
 *                  Date:Sep,20,2013
 */

#include <sys/stat.h>
#include <sys/types.h>

#include <dirent.h>
#include <errno.h>
#include <fcntl.h>
#include <fts.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "ls.h"
#include "extern.h"

int A_flag, a_flag, c_flag, d_flag, F_flag, f_flag, h_flag, i_flag;
int k_flag, l_flag, n_flag, q_flag, R_flag, r_flag, S_flag, s_flag;
int t_flag, u_flag, w_flag, one_flag, C_flag, x_flag;  

struct Node{
	char        *f_name;
    struct stat *f_stat;
    PtrToNode   next;
    
};

int simple_ls(int argc,char *argv[]){
	// char *cur_dir;
	// char *dir_ptr;
	DIR  		  *dirp;
	struct dirent *dp;
	

	// if((dir_ptr = getwd(cur_dir)) == NULL){
	// 	(void)fprintf(stderr,"Get working dirtory Error: %s\n",strerror(errno));
	// 	exit(EXIT_FAILURE);
	// }
	if((dirp=opendir("."))==NULL){
		(void)fprintf(stderr,"Open working dirtory Error: %s\n",strerror(errno));
		exit(EXIT_FAILURE);
	}

	while((dp = readdir(dirp))!=NULL)
		if((strcmp(dp->d_name,".")) && (strcmp(dp->d_name,"..")) && (dp->d_name[0]!='.')){
			fprintf(stdout,"%s\n",dp->d_name);
		}


	return EXIT_SUCCESS;
}

int do_ls(int argc, char *argv[]){
	int 	  argc_count        = 1;
	// int 	  argv_option_count = 0;
	// int 	  file_count 		= 0;
	// int 	  dirtory_count 	= 0;
	PtrToNode f_list 			= creat_list();
	PtrToNode d_list 			= creat_list(); 
	int       c;
	struct stat sb;

	if(argc>1 && argv[argc_count][0]=='-'){
		argc_count++;
	}
	f_flag = 1;
	
	while((c = getopt(argc, argv, "1ACFRSacdfhiklnqrstuwx")) != -1)
		switch(c){
			case 'A':
				fprintf(stdout,"A\n");
				break;
			case 'a':
				fprintf(stdout,"a\n");
				break;
			case 'c':
				fprintf(stdout,"c\n");
				break;
			case 'd':
				fprintf(stdout,"d\n");
				break;
			case 'F':
				fprintf(stdout,"F\n");
				break;
			case 'f':
				fprintf(stdout,"f\n");
				break;
			case 'h':
				fprintf(stdout,"h\n");
				break;
			case 'i':
				fprintf(stdout,"i\n");
				break;
			case 'k':
				fprintf(stdout,"k\n");
				break;
			case 'l':
				fprintf(stdout,"l\n");
				break;
			case 'n':
				fprintf(stdout,"n\n");
				break;
			case 'q':
				fprintf(stdout,"q\n");
				break;
			case 'R':
				fprintf(stdout,"R\n");
				break;
			case 'r':
				fprintf(stdout,"r\n");
				break;
			case 'S':
				fprintf(stdout,"S\n");
				break;
			case 's':
				fprintf(stdout,"s\n");
				break;
			case 't':
				fprintf(stdout,"t\n");
				break;
			case 'u':
				fprintf(stdout,"u\n");
				break;
			case 'w':
				fprintf(stdout,"w\n");
				break;
			case '1':
				fprintf(stdout,"l\n");
				break;
			case 'C':
				fprintf(stdout,"l\n");
				break;
			case 'x':
				fprintf(stdout,"l\n");
				break;
			default:
           	fprintf(stderr, "Usage: %s [ −AacdFfhiklnqRrSstuw1] [file ...]\n", argv[0]);
           	exit(EXIT_FAILURE);
		}
	
	

	//sort all (file & dirtory)'s stat in temp
	for(;argc_count<argc;argc_count++){
		if (lstat(argv[argc_count], &sb) == -1) {
			fprintf(stderr, "Can't stat %s: %s\n", argv[argc_count],
						strerror(errno));
		}
		if (S_ISDIR(sb.st_mode)){
			printf("directory\n");
			insert(d_list,&sb,argv[argc_count]);
			printlist( d_list);

		}
		else if(S_ISREG(sb.st_mode)||S_ISCHR(sb.st_mode)||S_ISBLK(sb.st_mode)
			||S_ISFIFO(sb.st_mode)||S_ISLNK(sb.st_mode)||S_ISSOCK(sb.st_mode)){
			printf("file\n");
			insert(f_list,&sb,argv[argc_count]);
			printlist( f_list);
		}else{
			fprintf(stderr, "%s unknown file type\n", argv[argc_count]);
			exit(EXIT_FAILURE);
		}
	}

	//excute each file and dirctory


	return EXIT_SUCCESS;
}


PtrToNode creat_list(){
	PtrToNode List;
	if((List = malloc(sizeof(struct Node)))==NULL){
		fprintf(stderr, "Out of memory\n");
		exit(EXIT_FAILURE);
	}else{
		List->f_name = NULL;
		List->f_stat = NULL;
		List->next   = NULL;
		return List;
	}
}

void insert(PtrToNode List, struct stat *sb, char *filename){
	PtrToNode temp = NULL;
	PtrToNode ptr  = List;
	if(ptr == NULL ){
		fprintf(stderr, "Empty List\n");
		exit(EXIT_FAILURE);
	}
	while((ptr->next != NULL) && (strcmp(filename,ptr->next->f_name)>0))
		ptr = ptr->next;
	if((temp=malloc(sizeof(struct Node)))==NULL){
		fprintf(stderr, "Out of memory\n");
		exit(EXIT_FAILURE);
	}else{
		temp->f_name = filename;
		temp->f_stat = sb;
		temp->next   = ptr->next;
		ptr->next    = temp;
	}
}

void printlist(PtrToNode List){
	PtrToNode temp = List;
	if(temp == NULL && temp->next!=NULL){
		fprintf(stderr, "Empty List\n");
		exit(EXIT_FAILURE);
	}
	do{
		temp = temp->next;
    	printf("%s,%o\n",temp->f_name,temp->f_stat->st_mode);
	}while(temp->next != NULL);
}