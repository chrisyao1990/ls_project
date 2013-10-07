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
#include <unistd.h>

extern int A_flag; 
extern int a_flag; 
extern int c_flag; 
extern int d_flag; 
extern int F_flag; 
extern int f_flag; 
extern int h_flag; 
extern int i_flag; 
extern int k_flag; 
extern int l_flag; 
extern int n_flag; 
extern int q_flag; 
extern int R_flag; 
extern int r_flag; 
extern int S_flag; 
extern int s_flag;
extern int t_flag; 
extern int u_flag; 
extern int w_flag; 
extern int one_flag;
extern int C_flag;
extern int x_flag;

struct Buf{
    int num;
    int total;
    int bcfile;
    int maxlen;
    int usrlen;
    int grplen;
    int linklen;
    int sizelen;
    int inodelen;
    int blocklen;
    FTSENT *list;
}b;

struct 		   Node;
typedef struct Node      *PtrToNode;

int simple_ls(int argc, char *argv[]);
int do_ls(int argc, char **argv);
PtrToNode creat_list();
void insert(PtrToNode List, struct stat *sb, char *filename);
void printlist(PtrToNode List);
void display_dir(PtrToNode Dir);
void display_file(PtrToNode File);