#define _XOPEN_SOURCE 500
#include<stdio.h>
#include<string.h>
#include<stdlib.h>
#include<unistd.h>
#include<math.h>
#include<limits.h>
#include<time.h>
#include<zlib.h>
#include<assert.h>
#include <sys/resource.h>
#include <sys/time.h>
#include <errno.h> 
#include <stdarg.h>

#define BUFF 20000
#define BUFF1 32
#define BUFF2 4194304
#define BUFF3 1048576
#define CHUNK 4096
#define MAX_PLACE_NAME 1500

char *idfile, *outfile,*headfile,*kinfile;
int idNum;
char *country_file,*region_file,*county_file,*council_file;
char timestr[BUFF];
long int memSize; // in bytes

int check_genomesize;

char unique_country[2000][MAX_PLACE_NAME];
char unique_region[300][MAX_PLACE_NAME];
char unique_county[600][MAX_PLACE_NAME];
char unique_council[3000][MAX_PLACE_NAME];
int n_country[2000];
int n_region[300];
int n_county[600];
int n_council[3000];

int index_country;
//the current number of unique countries
int index_region; //... of unique regions
int index_county; //.. of unique counties
int index_council; //.. of unique councils

int count_country;
int count_council;
int count_region;
int count_county;
int count_segnum;
//int histogram; //whether the output stat is total summary or histogram
int remove_close;

#define max_g 10000

int unique_totg[max_g]; //array of unique totg 
int min_totg,max_totg;
int index_totg; //number of unique totg 
int index_pair_country;
int index_pair_region;
int index_pair_county;
int index_pair_council;
int n_pair; //number of close pairs 
//int n_pair_region;
//int n_pair_county;
//int n_pair_council;

//ID stores individual-level information
typedef struct ID   
{
    char *id; //the UKBB id 
   // float lat; ;//latitude
   // float longt; //longtitude
    char *council;
    char *county;
    char *region;
    char *country;
    //int *stack;//stack[0] is the capacity, this array will be freed in the second pass
    //struct Pair **head;
} ID_t;

ID_t **idhead;

typedef struct close_ind   
{
    char *id; //the UKBB id 
    //int *stack;//stack[0] is the capacity, this array will be freed in the second pass
    //struct Pair **head;
} close_pair_t;

close_pair_t **close_pair;


//typedef struct pair{

//    char *geo1;
//    char *geo2;
//    int total_ibd;
//    int count;
//} Pair_t; 

long long ***region_pair,***country_pair,***county_pair,***council_pair;


typedef struct geo{

    char *name;
    long long count;
} Geo_t;

Geo_t **geo_council,**geo_county,**geo_region,**geo_country;
