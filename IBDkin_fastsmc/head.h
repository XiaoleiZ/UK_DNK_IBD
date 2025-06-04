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
#define BUFF1 50  //character length of sample ID
#define BUFF2 4194304
#define BUFF3 1048576
#define CHUNK 4096


long int Tfill_buff_ibd1, Tfill_buff_ibd2, Tstore_buff_ibd, Tcal_kinship, Tgzputs, Tgzread, Tshrinke_id_pair, Tcal_pair_num, Tcpbuff; //timing
double t_fill_buff_ibd1, t_fill_buff_ibd2, t_store_buff_ibd, t_cal_kinship, t_gzputs, t_gzread, t_shrinke_id_pair, t_cal_pair_num, t_cpbuff; //timing

int checkTime, checkMem;
int tagKinship, tagMask, tagCoverage;

int Parts, part;//data would be devide into ${Parts} and output the ${part} in this run
char *headfile, **ibdfile;
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
int mapL[100];
int coverageL[100];
int *outbuffi;
int Round;

int IBD_seg; //XL: check the number of lines passing the threshold
int remove_overlap; //XL: remove overlapped IBD segments 
int overlap_count;

float IBDcM1, IBDcM2;
float IBDprob;
float **mapP;
float **mapG;
float totg;
float kincut;
float FOLD;
float gap1, gap2;

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
    int segnum;
    float s1, s2,s_total;
    struct hapIBD *ibdhead;
} Pair_t;

/*save ID*/
typedef struct ID
{
    char *id;
    int num;//num of pairs stored
    int *stack;//stack[0] is the capacity, this array will be freed in the second pass
    struct Pair **head;
} ID_t;

ID_t **idhead;

typedef struct IBD //BUFFER
{
    int id1, id2;
    char chr;
    char pass;
    char h1, h2; //used to distinguish IBD1 and IBD2
    float g1, g2, l; //genetic positions
} IBD_t;

IBD_t **IBDdat;

typedef struct hapIBD //store ibs information
{
    char h1, h2; //used to distinguish IBD1 and IBD2
    float g1, g2; //genetic positions
    struct hapIBD *next;
} hapIBD_t;

typedef struct cvrg
{
   double **coverage;
} cvrg_t;

cvrg_t **coverage_per_thread;


typedef struct msk
{
    float p1, p2;
    float g1, g2;
    struct msk *next;
} msk_t;

msk_t **mask;
