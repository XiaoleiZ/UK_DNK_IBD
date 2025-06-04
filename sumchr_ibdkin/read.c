#include "head.h"
#include "tools.h"
#include "print.h"
#include "read.h"
#include "parallel.h"
#include<stdio.h>
#include<stdlib.h>
//#include<parallel.c>
//#include "/hps/software/users/birney/xiaolei/sqlite3/sqlite3.h"

void read_headfile(char *headfile)
{
    char str[BUFF] = "0";
    int i;
    Nfile = 0;
    FILE *ifp = fopen(headfile, "r");
    while(fscanf(ifp, "%s",str)==1){
	fprintf(stderr, "  %s", str);
	if(access(str, R_OK) == -1) {fprintf(stderr, " does not exist!\n"); exit(-1);}
	fprintf(stderr, "\n");
	Nfile++;}
    fclose(ifp);
    ibdsum_file = (char **)calloc(Nfile, sizeof(char*));
    memSize += Nfile * sizeof(char*) + sizeof(char**);

    ifp = fopen(headfile, "r"); i = 0;
    while(fscanf(ifp, "%s",str)==1){
	ibdsum_file[i] = (char * )calloc(strlen(str)+10, sizeof(char));
	memSize += (strlen(str)+10) * sizeof(char);
	strcpy(ibdsum_file[i], str);

	i ++;
    }
    fclose(ifp);
}


int check_input(int arg, char**argv)
{
    fprintf(stderr, "############################\n");
    fprintf(stderr, "#Welcome to sumchr_IBDkin Step 2 - combining multiple IBDkin output file#\n");
    fprintf(stderr, "############################\n");
    int i, fsize;
    checkTime = 0; checkMem = 0;
    self=0;
    across=0;
    BINSIZE = 1000; 

  
    tagKinship = 1;
    Nthreads = 2;
    outfile = (char *) calloc (16, sizeof(char));
    memSize += 3* 16 * sizeof(char);
    strcpy(outfile, "sum.gz");  //copying kinship.gz to str outfile as output file name

    if(arg <=1)print_help(); 
    // the first argument is the name of the program
    for(i = 1; i< arg; i++){
	if(strcmp(argv[i], "--ibdsum_file")==0){  //if the two strings are equal
	    fsize = strlen(argv[i+1])+10; 
	    headfile = (char *) calloc (fsize, sizeof(char));    //allocate memory to the string
	    memSize += (fsize) * sizeof(char); 
	    strcpy(headfile, argv[i+1]); i = i + 1;
	}else if(strcmp(argv[i], "--ind")==0){
	    fsize = strlen(argv[i+1])+10;
	    idfile = (char *) calloc (fsize, sizeof(char));
	    memSize += (fsize) * sizeof(char);
	    strcpy(idfile, argv[i+1]); 
	    i = i + 1;
	}else if(strcmp(argv[i],"--self")==0){
        self=1;
        pop1=(char *)calloc(30,sizeof(char));
        strcpy(pop1,argv[i+1]);
        i=i+1;
    }else if(strcmp(argv[i],"--across")==0){
        across=1;
        pop1=(char *)calloc(30,sizeof(char));
        pop2=(char *)calloc(30,sizeof(char));
        strcpy(pop1,argv[i+1]);
        strcpy(pop2,argv[i+2]);
        i=i+2;
    }else if(strcmp(argv[i], "--out")==0){
	   fsize = strlen(argv[i+1])+20;
        outfile = realloc (outfile, fsize*sizeof(char));
        memSize += 3* (fsize - 16) * sizeof(char);
        sprintf(outfile, "%s.sumchr.gz", argv[i+1]); 
	   i = i + 1;
    }
	else if(strcmp(argv[i], "--nthreads")==0){
	    Nthreads = atoi(argv[i+1]);i = i + 1;
	}
	else {
	    print_help();
	}
    }

    return 0;
}





void write_relate(void)
{
    double a = my_wallclock();
    long int b = clock();
    gzFile fp = gzopen(outfile, "a");
    gzbuffer(fp, BUFF3);
    Pair_t *prcur;
    int i, j, idi, ids, segnum;
    float  s1, s2,s_total;
    char str[BUFF];
    
    for(i = 0; i < idNum; i++){
	for(j = 0; j < idhead[i]->pair_num; j++){
	    prcur = (idhead[i]->head)[j];
	    idi = i;
	    ids = prcur->index;
	    segnum = prcur->seg_num;;
        s1=prcur->s1;
        s2=prcur->s2;
        s_total=prcur->s_total;
      
	    
    	if(s2 <= 0.00001){
            //the column of output: id1 id2 segnum s1 0
            sprintf(str, "%s\t%s\t%d\t%f\t0\t%f\n", idhead[idi]->id, idhead[ids]->id, segnum, s1,s_total);
        }
		else{
            sprintf(str, "%s\t%s\t%d\t%f\t%f\t%f\n", idhead[idi]->id, idhead[ids]->id, segnum, s1, s2,s_total);
          }
        gzputs(fp, str);
	    }
	
    }
    gzclose(fp);
    Tgzputs += clock() - b;
    t_gzputs += my_wallclock() - a;
    return ;
}

void read_ibd_1st_pass(void)
{
    double a;
    Round = 0;

    if(checkTime == 1 )a = my_wallclock();
    //int total_line=0;
    IBD_seg=0;

    int i;
    char *head;
    head = (char *)calloc(BUFF, sizeof(char));

    for (i = 0; i < Nfile; i++){    //Nfile: could be one file per chromosome or multiple files per chromosome
	ifp = gzopen(ibdsum_file[i], "rb");

    gzbuffer(ifp, BUFF3);
	fprintf(stderr, "\treading %s\n", ibdsum_file[i]);
    
	read_buff(); // put chunk of the file line by line into buff (array) and get the number of IBD segments: buff
    
    // the chromosome number is got from the input file 
	while(1){
	    Round += 1;
	    if(buffi2 == 0)break; //once there are no more lines to read, get out of the loop
	    buffi = buffi2;
        copy_buff();
        fill_buff_ibd_1st_pass(); //read information from input lines and put into buff
	    store_buff_ibd(); // put buff info into ID_t array (calculating how many pairs an individual has;)
	}

    
	gzclose(ifp);
    fprintf(stderr,"Total number of lines read: %lli\n", IBD_seg);
    }
    fprintf(stderr,"writing output..\n");
    
    if(checkTime ==1 ){
	p_std_time(my_wallclock() - a);
	fprintf(stderr,"\nread_ibd_1st_pass() Time = %s\n", timestr);}
    free(head);
    return ;
    
}



void read_ibd_2nd_pass(void)
{
    double a;
    if(checkTime)a = my_wallclock();
    int i;
    

    Round = 0;
    char *head;
    head = (char *)calloc(BUFF, sizeof(char));

    for (i = 0; i < Nfile; i++){
    ifp = gzopen(ibdsum_file[i], "r");
    
    fprintf(stderr, "\treading %s\n", ibdsum_file[i]);
    read_buff();  //put line by line into buffer; get the number of lines;

    while(1){
        Round += 1;
        if(buffi2 == 0)break;
        buffi = buffi2;
        copy_buff();
        fill_buff_ibd_2nd_pass();
        store_buff_ibd_2nd();//for distant relatives; for pairs only sharing 1 IBD segments   //put output info strbuff
        
    }
    p_mem();
        
    gzclose(ifp);
    }

    write_relate(); //use the information stored in pair structure

    if(checkTime){
    p_std_time(my_wallclock() - a);
    fprintf(stderr,"read_ibd_2nd_pass() Time = %s\n", timestr);
    
    }
    free(head);
}



void read_ind(char *idfile)
{
    double a = my_wallclock();
    gzFile fp;
    int i;
    char id[max_lenstr];
    char country[max_lenstr];
    char str[BUFF];


    fp = gzopen(idfile, "r"); idNum = 0;
    while(gzgets(fp, str, BUFF-1)!=NULL)idNum ++;
    gzclose(fp);
    //allocating memory to the list of ID 
    idhead = (ID_t **)calloc(idNum, sizeof(ID_t*));
    memSize += idNum * (sizeof(ID_t*) + sizeof(char)) + sizeof(ID_t**);
    fp = gzopen(idfile, "r"); i = 0;
    while(gzgets(fp, str, BUFF-1)!=NULL){
	idhead[i] = (ID_t *)calloc(1, sizeof(ID_t));
	idhead[i]->id = (char *)calloc(strlen(str)+1,sizeof(char));
    idhead[i]->country = (char *)calloc(strlen(str)+1,sizeof(char));
	idhead[i]->head = NULL;
	idhead[i]->pair_num = 0; //number of individuals shared IBD segments with
	idhead[i]->seg_num=0;
    idhead[i]->stack = (int *)calloc(4, sizeof(int));
	(idhead[i]->stack)[0] = 4;
	memSize += sizeof(ID_t) + strlen(str)*sizeof(char) + 4 * sizeof(int);
	
    sscanf(str, "%[^\t]\t%[^\n]\n", id,country);
    //only keep the pairs that is between DNK and UK
    strcpy(idhead[i]->id,id);
    strcpy(idhead[i]->country,country);
    i++;
    }
	
    

    gzclose(fp);
    qsort(idhead, idNum, sizeof(ID_t *), ID_t_cmp);

    for(i = 0; i < (idNum-1); i++){
	if(strcmp(idhead[i]->id, idhead[i+1]->id)==0){
	    fprintf(stderr, "duplicated individuals: %s\n", idhead[i]->id);
	    exit(-1);
	}
    }
    

    fprintf(stderr, "%d individuals\n", idNum);
    p_std_time(my_wallclock() - a);
    fprintf(stderr,"read_ind() Time = %s\n", timestr);
    return ;
}


void init(void)
{
    int i;
    Tfill_buff_ibd1 =0;
    Tstore_buff_ibd = 0;
    Tcal_kinship = 0;
    Tgzread = 0;
    Tgzputs = 0;

    t_fill_buff_ibd1 =0;
    t_store_buff_ibd = 0;
    t_cal_kinship = 0;
    t_gzread = 0;
    t_gzputs = 0;


    fprintf(stderr, "allocate IBD buffer\n");
    IBDdat = (IBD_t **)calloc(BUFF2, sizeof(IBD_t*));
    strbuff = (char **)calloc(BUFF2, sizeof(char*));
    strbuff2 = (char **)calloc(BUFF2, sizeof(char*));
    for(i = 0; i < BUFF2 ;i ++){
	IBDdat[i] = (IBD_t *)calloc(1, sizeof(IBD_t));
	strbuff[i] = (char *)calloc(BUFF, sizeof(char));
	strbuff2[i] = (char *)calloc(BUFF, sizeof(char));
	memSize += sizeof(IBD_t) + 2 * BUFF * sizeof(char);
    }
    memSize += BUFF2 * (sizeof(IBD_t*) + 2 * sizeof(char*));
    return ;
}


