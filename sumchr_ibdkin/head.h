#define _XOPEN_SOURCE 500
#include<stdio.h>
#include<string.h>
#include<stdlib.h>
#include<unistd.h>
#include<math.h>
#include<limits.h>
#include<time.h>
#include<pthread.h>
#include<zlib.h>
#include<assert.h>
#include <sys/resource.h>
#include <sys/time.h>
#include <errno.h> 
#include <stdarg.h>


#define BUFF 300
#define BUFF1 100
#define BUFF2 4194304
#define BUFF3 1048576
#define CHUNK 4096
#define max_g 10000
#define max_lenstr 200

long int Tfill_buff_ibd1, Tfill_buff_ibd2, Tstore_buff_ibd, Tcal_kinship, Tgzputs, Tgzread, Tshrinke_id_pair, Tcal_pair_num, Tcpbuff; //timing
double t_fill_buff_ibd1, t_fill_buff_ibd2, t_store_buff_ibd, t_cal_kinship, t_gzputs, t_gzread, t_shrinke_id_pair, t_cal_pair_num, t_cpbuff; //timing

int checkTime, checkMem;
int tagKinship, tagMask, tagCoverage;
int self, across;
char *pop1, *pop2;

int Parts, part;//data would be devide into ${Parts} and output the ${part} in this run
char *headfile, **ibdsum_file;
char *idfile, *mapfile, *outfile;
char *maskfile, *coveragefile, *rangefile;
char **chrs;

int Nfile;
int buffi, buffi2;
int KEYnumPAIR;
int KEYnumID;
int Nsample;
int idNum;
int chrN, CHR; //working chromosome number
int total_overlap;

long int *threadCount;
long int memSize; // in bytes
long int *memSizeByThread;
long int pairNum, segNum;
long int *pairNumByThread, *segNumByThread;

float minPos[100], maxPos[100]; //record minimum and maximum genetic positions for each chromosome. 22 for 22 automosomes
float **minpos, **maxpos; 
float BINSIZE; //kb
double **coverage, median;

int Nthreads;
int *outbuffi;
int Round;

long long IBD_seg; //XL: check the number of lines passing the threshold
int skip_firstline;


char haptag[2];
char *lock;
char **strbuff;
char **strbuff2;
char **outbuff;
char timestr[BUFF];

//sqlite3 database
//sqlite3 *db;



gzFile ifp;
FILE *ofp, *bf;

typedef struct Pair
{
    int index;
    int seg_num;
    float s1, s2,s_total;
} Pair_t;

/*save ID*/
typedef struct ID
{
    char *id;
    char *country;
    int pair_num;//num of pairs stored
    int seg_num;
    int *stack;//stack[0] is the capacity, this array will be freed in the second pass
    struct Pair **head;
} ID_t;

ID_t **idhead;

typedef struct IBD //BUFFER
{
    int id1, id2;
    //char chr;
    char pass;
    int segnum;
    //char h1, h2; //used to distinguish IBD1 and IBD2
    float s1, s2,s_total;//genetic positions
} IBD_t;

IBD_t **IBDdat;

