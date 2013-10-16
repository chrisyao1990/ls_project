/*
 *   CS631 Advanced Programming in the UNIX Environment
 *               Project 1, ls implementation
 *           ls [ −AaCcdFfhiklnqRrSstuwx1] [file ...]
 *                  Author: Kun Yao
 *                  Date:Sep,20,2013
 */
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/ioctl.h>

#include <ctype.h>
#include <dirent.h>
#include <errno.h>
#include <fcntl.h>
#include <fts.h>
#include <grp.h>
#include <pwd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <unistd.h>

#include "ls.h"
#include "extern.h"

#define NOTPRINT 1

int A_flag, a_flag, c_flag, d_flag, F_flag, f_flag, h_flag, i_flag;
int k_flag, l_flag, n_flag, q_flag, R_flag, r_flag, S_flag, s_flag;
int t_flag, u_flag, w_flag, one_flag, C_flag, x_flag; 
int to_terminal_flag; 
int dirprint_total;
int regular_file_total;
int totalprint_flag;
int readoption;
int dirprint;
int termwidth = 80;
float blktimes;

static char *pro_name;
static void (*display)(void);
static int (*compare)(const FTSENT *a, const FTSENT *b);

int do_ls(int argc, char *argv[]){
	int    		   c;
	char 		   dot[] = ".", *dotav[] = { dot, (char *)NULL };
	struct winsize win;
	FTS 		   *ftsp;
    FTSENT         *p, *tmp;

	pro_name = *argv;
	r_flag     = 1;
	C_flag     = 1;
	display    = orderdisplay;
	compare    = cmpname;
	readoption = FTS_PHYSICAL;
	blktimes   = 1;

	/* determine output to terminal or file*/
	if (isatty(STDOUT_FILENO)) {
		if (ioctl(STDOUT_FILENO, TIOCGWINSZ, &win) == 0 && win.ws_col > 0)
			termwidth = win.ws_col;
		to_terminal_flag = q_flag = 1;
	} else{
		to_terminal_flag = 0;
		w_flag = one_flag = 1;
	}

	while((c = getopt(argc, argv, "1ACFRSacdfhiklnqrstuwx")) != -1)
		switch(c){
			case 'A':
				A_flag = 1;
				break;
			case 'a':
				a_flag = 1;
				readoption = FTS_PHYSICAL|FTS_SEEDOT;
				break;
			case 'c':
				c_flag = 1;
				u_flag = 0;
				compare = cmptimestch;
				break;
			case 'd':
				d_flag = 1;
				R_flag = 0;
				break;
			case 'F':
				F_flag = 1;
				break;
			case 'f':
				f_flag = 1;
				readoption = FTS_PHYSICAL|FTS_SEEDOT;
				break;
			case 'h':
				h_flag = 1;
				k_flag = 0;
				break;
			case 'i':
				i_flag = 1;
				break;
			case 'k':
				k_flag = 1;
				h_flag = 0;
				break;
			case 'l':
				l_flag = 1;
				one_flag = 0;
				n_flag = 0;
				C_flag = 0;
				x_flag = 0;
				display = longdisplay;
				break;
			case 'n':
				n_flag = 1;
				l_flag = 0;
				one_flag = 0;
				C_flag = 0;
				x_flag = 0;
				display = longdisplay;
				break;
			case 'q':
				q_flag = 1;
				w_flag = 0;
				break;
			case 'R':
				R_flag = 1;
				d_flag = 0;
				break;
			case 'r':
				r_flag = -1;
				break;
			case 'S':
				compare = cmpsize;
				break;
			case 's':
				s_flag = 1;
				break;
			case 't':
				compare = cmptimemdf;
				break;
			case 'u':
				u_flag = 1;
				c_flag = 0;
				compare = cmptimelacc;
				break;
			case 'w':
				w_flag = 1;
				q_flag = 0;
				break;
			case '1':
				n_flag = 1;
				l_flag = 0;
				one_flag = 0;
				C_flag = 0;
				x_flag = 0;
				display = onedisplay;
				break;
			case 'C':
				n_flag = 0;
				l_flag = 0;
				one_flag = 0;
				C_flag = 1;
				x_flag = 0;
				display = orderdisplay;
				break;
			case 'x':
				n_flag = 0;
				l_flag = 0;
				one_flag = 0;
				C_flag = 0;
				x_flag = 1;
				display = orderdisplay;
				break;
			default:
           	fprintf(stderr, "Usage: %s [−AaCcdFfhiklnqRrSstuwx1] [file ...]\n",
           	 pro_name);
           	exit(EXIT_FAILURE);
		}

	if(a_flag|f_flag)
		A_flag = 0;
	if(k_flag&&s_flag){
		if(getenv("BLOCKSIZE")!=NULL){
			blktimes = (float)atoi(getenv("BLOCKSIZE")) / (float)1024;
		}else
			blktimes = 0.5;
		setenv("BLOCKSIZE","1024",1);
	}
	if((argc==2 && argv[1][0]=='-')||argc==1){
		if((ftsp = fts_open(dotav, readoption,cmporder)) == (FTS *)NULL){
       		fprintf(stderr, "%s: fts_open error:%s\n",
       			pro_name,strerror(errno));
       		exit(EXIT_FAILURE);
   		}
   		argc -= 2;
	}else{
		if(argc>2 && argv[1][0]=='-'){
			argv+=2;
			argc -= 2;
		}else{
			argv++;
			argc--;
		}
		if((ftsp = fts_open(argv, readoption,cmporder)) == (FTS *)NULL){
       		fprintf(stderr, "%s: fts_open error:%s\n",
       			pro_name,strerror(errno));
       		exit(EXIT_FAILURE);
   		}
	} 

	do_print(NULL, fts_children(ftsp, 0));
    if(d_flag){
    	fts_close(ftsp);
    	return EXIT_SUCCESS;
    }

    while((p = fts_read(ftsp))!=NULL)
    	switch(p->fts_info){
    		case FTS_DC:
    			fprintf(stderr, "%s:A directory causes a cycle\n",p->fts_name);
    			exit(EXIT_FAILURE);
    			break;
    		case FTS_ERR:
    		case FTS_DNR:
    			errno = p->fts_errno; 
    			fprintf(stderr, "%s error: %s\n",p->fts_name,strerror(errno));
    			break;
    		case FTS_D:
    			if(dirprint){
    				printf("\n%s:\n",p->fts_path);
    			}else if(argc>1||R_flag){
    				printf("%s:\n", p->fts_path);
                	dirprint = 1;
    			}
    			tmp = fts_children(ftsp, 0);
    			totalprint_flag = 1;
				do_print(p, tmp);
				totalprint_flag = 0;
				if(R_flag == 0 && tmp)
					fts_set(ftsp, p, FTS_SKIP);
				break;
			default:
				break;
    	}

	fts_close(ftsp);
	return EXIT_SUCCESS;
}

void do_print(FTSENT *fp, FTSENT *list){
	register FTSENT *p;
	register struct stat *q;
	struct    passwd *pw;
	struct    group *gr;
	int       needstat = i_flag|s_flag|l_flag|n_flag|C_flag|x_flag;
	size_t    mx_fname  = 0;
	ino_t     mx_iod    = 0;
	blksize_t mx_blks   = 0;
	int       tt_blks   = 0;
	nlink_t   mx_links  = 0;
	int       mx_uidlen = 0;
	int       mx_gidlen = 0;
	off_t     mx_size   = 0;
	int       total     = 0;

	if(list == NULL)
		return;

    for(p = list; p; p = p->fts_link){
		if(p->fts_info == FTS_ERR || p->fts_info == FTS_NS 
			|| p->fts_info == FTS_NSOK){
            errno = p->fts_errno;
        	fprintf(stderr, "%s error: %s\n",p->fts_name,strerror(errno));
            p->fts_number = NOTPRINT;
            continue;
        }
        if(fp==NULL)
        	totalprint_flag = 0;
        if((fp == NULL) && (d_flag == 0)){
        	if((p->fts_info == FTS_D)){
        		dirprint_total++;
     			p->fts_number = NOTPRINT;
     			continue;
        	}else{
        		regular_file_total++;
        	}
        }
        if((fp != NULL)&& (!(f_flag || a_flag || A_flag)) 
        	&& p->fts_name[0]=='.' &&(!d_flag)){
        	p->fts_number = NOTPRINT;
        	continue;
        }

        if(q_flag)
        	qfroceprint(p->fts_name, p->fts_name);

        if(needstat){
        	q = p->fts_statp;
        	mx_iod  = (mx_iod<q->st_ino)?q->st_ino:mx_iod;
        	mx_blks = (mx_blks<q->st_blocks)?q->st_blocks:mx_blks;
        	tt_blks += q->st_blocks;
        	mx_links= (mx_links<q->st_nlink)?q->st_nlink:mx_links;
        	mx_size = (mx_size<q->st_size)?q->st_size:mx_size;
        	if(l_flag){
        		pw = getpwuid(q->st_uid);
        		gr = getgrgid(q->st_gid);
        		if(pw!=NULL && gr!=NULL){
        			mx_uidlen = (mx_uidlen<strlen(pw->pw_name))
        				?strlen(pw->pw_name):mx_uidlen;
        			mx_gidlen = (mx_gidlen<strlen(gr->gr_name))
        				?strlen(gr->gr_name):mx_gidlen;
        		}else{
        			mx_uidlen = (mx_uidlen<digitslength(q->st_uid))
        				?digitslength(q->st_uid):mx_uidlen;
        			mx_gidlen = (mx_gidlen<digitslength(q->st_gid))
        				?digitslength(q->st_uid):mx_gidlen;
        		}
        	}
        	if(n_flag){
        		mx_uidlen = (mx_uidlen<digitslength(q->st_uid))
        			?digitslength(q->st_uid):mx_uidlen;
        		mx_gidlen = (mx_gidlen<digitslength(q->st_gid))
        			?digitslength(q->st_uid):mx_gidlen;
        	}
        }
		mx_fname = (mx_fname<p->fts_namelen)?p->fts_namelen:mx_fname;
		total++;
    }
    b.bf_total    = total;
    b.bf_fnamelen = mx_fname;
    if(needstat){
    	b.bf_inodwidth = digitslength(mx_iod);
    	b.bf_blklen    = digitslength((int)((float)mx_blks*blktimes));
    	b.bf_totalblks = (int)((float)tt_blks*blktimes);
    	b.bf_linklen   = digitslength(mx_links);
    	b.bf_uidlen    = mx_uidlen;
    	b.bf_gidlen    = mx_gidlen;
    	b.bf_sizelen   = digitslength(mx_size);
    }

    b.bf_list = list;
    display();

}

void onedisplay(void){
	register FTSENT *p;
	if(b.bf_total == 0)
		return;
	if(s_flag && to_terminal_flag && totalprint_flag){
		printf("total %d\n", b.bf_totalblks);
	}
	for(p = b.bf_list; p; p = p->fts_link){
		if(p->fts_number == NOTPRINT){
			continue;
		}
		if(i_flag){
			printf("%*d ",b.bf_inodwidth,(int)p->fts_statp->st_ino);
		}
		if(s_flag){
			blkprint(p->fts_statp);
		}
		printf("%s", p->fts_name);
		if(F_flag)
			signprint(p->fts_statp->st_mode);
		printf("\n");	
	}

}

void orderdisplay(void){
	register FTSENT *p;
	int total_entry_len   = b.bf_fnamelen+2;
	int col_per_row       = 0;
	int total_row         = 0;
	int write_space       = 0;
	int count             = 0;
	int count1            = 0;
	if(b.bf_total == 0)
		return;

	total_entry_len += F_flag            ? 1:0;
	total_entry_len += h_flag&&s_flag    ? 5:0;
	total_entry_len += (!h_flag)&&s_flag ? b.bf_blklen+1:0;
	total_entry_len += i_flag            ? b.bf_inodwidth+1:0;

	col_per_row = (int)(termwidth/(total_entry_len+1));
	if(col_per_row == 0){
		col_per_row = 1;
	}else{
		count1 = col_per_row > b.bf_total ? b.bf_total:col_per_row;
		write_space = (int)((termwidth/count1) - total_entry_len);
		if(write_space>10) write_space = 10;
		
	}
	if(col_per_row == 1){
		onedisplay();
		return;
	}
	total_row = (b.bf_total/col_per_row)+1;

	if(s_flag && to_terminal_flag && totalprint_flag){
		printf("total %d\n", b.bf_totalblks);
	}
	if(x_flag || total_row==1){
		for(p = b.bf_list; p; p = p->fts_link){
			if(p->fts_number == NOTPRINT){
				continue;
			}
			count++;
			filedisplay(p);
			if(count==col_per_row){
				count = 0;
				printf("\n");
			}else{
				for(count1 = 0;count1<write_space;count1++)
					printf(" ");
			}
		}
		if(count)
			printf("\n");
	}else{
		int i=0,j = 0;
		int total = b.bf_total;
		int lineend = col_per_row;
		FTSENT **array;
		p = b.bf_list;
		if((array = (FTSENT **)malloc(sizeof(FTSENT *) * b.bf_total)) == NULL){
            fprintf(stderr, "%s error:%s\n",
       			pro_name,strerror(errno));
			exit(EXIT_FAILURE);
        }
        for(p = b.bf_list; p; p = p->fts_link) 
        	if(p->fts_number != NOTPRINT) 
            	array[i++] = p;
		for(i=0,count=0,p = b.bf_list;i<total;i++){
			count++;
			j = total_row*((i)%col_per_row) + (i)/col_per_row;
			if(j >= b.bf_total){
				total++;
				if(count >1)
					filedisplay(NULL);
				count = 0;
			}else{
				filedisplay(array[j]);
				if(count==lineend){
					count = 0;
					printf("\n");
				}else{
					for(count1 = 0;count1<write_space;count1++)
						printf(" ");
				}
			}
		}
		if(count)
			printf("\n");
	}	
}

void filedisplay(FTSENT *p){
	struct stat *q;
	int count1 = 0;
	if(p==NULL){
		printf("\n");
		return;
	}
	q = p->fts_statp;
	if(i_flag){
		printf("%*d ",b.bf_inodwidth,(int)q->st_ino);
	}
	if(s_flag){
		blkprint(q);
	}
	printf("%s", p->fts_name);
	if(F_flag)
		signprint(q->st_mode);
	for(count1 = 0;count1<b.bf_fnamelen-p->fts_namelen;count1++)
		printf(" ");
	printf("  ");
}

void longdisplay(void){
	register FTSENT *p;
	register struct stat *q;
	struct passwd *pw;
	struct group *gr;
	char   buf_mode[10];
	if(b.bf_total == 0)
		return;
	if(totalprint_flag){
		printf("total %d\n", b.bf_totalblks);
	}
	for(p = b.bf_list; p; p = p->fts_link){
		if(p->fts_number == NOTPRINT){
			continue;
		}
		q = p->fts_statp;
		if(i_flag){
			printf("%*d ",b.bf_inodwidth,(int)q->st_ino);
		}
		if(s_flag){
			blkprint(q);
		}
		modeprint(q->st_mode,buf_mode);
		printf("%s  ",buf_mode);
		printf("%*d ", b.bf_linklen,(int)q->st_nlink);

		if(l_flag){
			pw = getpwuid(q->st_uid);
        	gr = getgrgid(q->st_gid);
        	if(pw!=NULL && gr!=NULL){
        		printf("%*s  %*s  ",b.bf_uidlen,pw->pw_name
        		,b.bf_gidlen,gr->gr_name);
        	}else{
        		printf("%*d  %*d  ",b.bf_uidlen,(int)q->st_uid
        		,b.bf_gidlen,(int)q->st_gid);
        	}
		}else{
			printf("%*d  %*d  ",b.bf_uidlen,(int)q->st_uid
        		,b.bf_gidlen,(int)q->st_gid);
		}

		if(h_flag){
			readblesizeprint(q->st_size);
		}else{
			printf("%*ld ",b.bf_sizelen,(long int)q->st_size);
		}

		if(c_flag){
			timeprint(&q->st_ctime);
		}else if(u_flag){
			timeprint(&q->st_atime);
		}else{
			timeprint(&q->st_mtime);
		}

		printf("%s", p->fts_name);
		if(F_flag)
			signprint(q->st_mode);
		if(S_ISLNK(q->st_mode)){
			linkprint(p);
		}
		printf("\n");
	}
}

int cmporder(const FTSENT **a, const FTSENT **b){
    FTSENT *a_p, *b_p;
    a_p = *(FTSENT **)a;
    b_p = *(FTSENT **)b;

    if((a_p->fts_info == FTS_ERR)  || (b_p->fts_info == FTS_ERR))
        return 0; 
    if( (a_p->fts_info == FTS_NS)  || (a_p->fts_info == FTS_NSOK) 
    	||(b_p->fts_info == FTS_NS)|| (b_p->fts_info == FTS_NSOK))
        return cmpname(a_p, b_p);
    if(a_p->fts_info == b_p->fts_info)
        compare(a_p, b_p);
    if(a_p->fts_level == 0){       
        if(a_p->fts_info == FTS_D){
            if(b_p->fts_info != FTS_D)
                return 1;
        }
        else if(b_p->fts_info == FTS_D)
            return -1;
    }
    return compare(a_p, b_p);
}

int cmpname(const FTSENT *a, const FTSENT *b){
    return r_flag * strcmp(a->fts_name, b->fts_name);
}
int cmptimestch(const FTSENT *a, const FTSENT *b){
	return r_flag * (b->fts_statp->st_ctime - a->fts_statp->st_ctime);
}
int cmpsize(const FTSENT *a, const FTSENT *b){
	return r_flag * (b->fts_statp->st_size - a->fts_statp->st_size);
}
int cmptimemdf(const FTSENT *a, const FTSENT *b){
	return r_flag * (b->fts_statp->st_mtime - a->fts_statp->st_mtime);
}
int cmptimelacc(const FTSENT *a, const FTSENT *b){
	return r_flag * (b->fts_statp->st_atime - a->fts_statp->st_atime);
}

int digitslength(long int d){
	char buf[31];
	snprintf(buf, sizeof(buf), "%ld", d);
	return (strlen(buf));
}
