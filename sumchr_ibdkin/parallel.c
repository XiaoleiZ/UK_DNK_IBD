#include "head.h"
#include "tools.h"
#include "print.h"
#include "parallel.h"
#include <stdbool.h>

pthread_mutex_t mutex1;
pthread_mutex_t mutex2;
int pair_1ibd=0;
int pair_lotibd=0;



void read_buff(void)
{	

    double a = my_wallclock();
    long int b = clock();
	
    int i = 0;
    while(i < BUFF2 && (gzgets(ifp, strbuff2[i], BUFF-1) != Z_NULL)){
        i++;
    }
    buffi2 = i;   // the number of lines read in this buff

    Tgzread += clock() - b;
    t_gzread += my_wallclock() - a;
    return ;
}



void copy_buff(void)
{
    double a = my_wallclock();
    long int b = clock();

    pthread_t thread_id[Nthreads];
    int thread_args[Nthreads];
    int rc, i;

    for ( i = 0; i < Nthreads; i++ ){
	thread_args[i] = i;
	rc = pthread_create(&thread_id[i], NULL, copy_buff_by_thread, &thread_args[i]);
	if(rc){
	    printf("\n ERROR code from thread %d is %d \n", i, rc);
	    exit(1);
	}

    }
    for ( i = 0; i < Nthreads; i++ )pthread_join(thread_id[i], NULL);
    Tcpbuff += clock() - b;
    t_cpbuff += my_wallclock() - a;

    return ;
}

void *copy_buff_by_thread(void *args)
{
    int th_index = *((int *) args);
    int i;

    for(i = 0; i < buffi; i++){
        if(i % Nthreads != th_index)continue;
	strcpy(strbuff[i], strbuff2[i]);
    }
    return NULL;
}



void store_buff_ibd(void)
{
    double a;
    long int b;


    if(checkTime == 1){
	a = my_wallclock();
	b = clock();
    }
    pthread_t thread_id[Nthreads];
    int thread_args[Nthreads];
    int rc, i;

    for ( i = 0; i < Nthreads; i++ ){
	thread_args[i] = i;
	rc = pthread_create(&thread_id[i], NULL, store_buff_ibd_by_thread, &thread_args[i]);
	if(rc){
	    printf("\n error code from thread %d is %d \n", i, rc);
	    exit(1);
	}

    }
    for ( i = 0; i < Nthreads; i++ )pthread_join(thread_id[i], NULL);
    if(checkTime == 1){
	Tstore_buff_ibd += clock() - b;
	t_store_buff_ibd += my_wallclock() - a;
    }

   


    return ;
}

void *store_buff_ibd_by_thread(void *args)
{
    int b = clock()/10000;

    int th_index = *((int *) args);
    IBD_t *ibdcur;
    int i;
    int id1, id2;
    int pair_num=0, L;
    for(i = 0; i < buffi; i++){

	ibdcur = IBDdat[i];
	if(ibdcur->pass != '1')continue;
	id1 = ibdcur->id1;
	id2 = ibdcur->id2;
	if(id1 % Nthreads != th_index)continue;

	//push pairs
	idhead[id1]->pair_num += 1; //Attention: it only stores into id1 info instead of both id1 and id2 -> that makes sure that a pair is only visited once
	pair_num = idhead[id1]->pair_num;
	L = (idhead[id1]->stack)[0]; //shall it all be 4 here?  Yes L=4
	
	if(pair_num > L - 1){ //num>3
	   
	    while (pair_num > L - 1) L = (int)(L * 1.25) + 5;
	    idhead[id1]->stack = realloc(idhead[id1]->stack, L * sizeof(int));
	   
	    (idhead[id1]->stack)[0] = L;   //size of pairs that can be stored
	}
	(idhead[id1]->stack)[pair_num] = id2;
    }
    threadCount[th_index] += clock()/10000 - b;

    return NULL;
}




void fill_buff_ibd_1st_pass(void)
{
    double a;
    long int b;

    if(checkTime == 1){
	a = my_wallclock();
	b = clock();
    }


    pthread_t thread_id[Nthreads];
    int thread_args[Nthreads];
    int rc, i;
    int N;
    if (Nthreads > 1) N = Nthreads - 1;
    else N = Nthreads;


    for ( i = 0; i < N; i++ ){
	thread_args[i] = i;
	rc = pthread_create(&thread_id[i], NULL, fill_buff_ibd_1st_pass_by_thread, &thread_args[i]);
	if(rc){
	    printf("\n ERROR code from thread %d is %d \n", i, rc);
	    exit(1);
	}

    }
    read_buff();  //after fill buff to ibd structure; then read next chunk 
    for ( i = 0; i < N; i++ ){
	rc = pthread_join(thread_id[i], NULL);
	if(rc > 0) printf("\n ERROR code from thread %d is %d \n", i, rc);

    }
    if(checkTime == 1){
	Tfill_buff_ibd1 += clock() - b;
	t_fill_buff_ibd1 += my_wallclock() - a;
    }
    return ;
}

void *fill_buff_ibd_1st_pass_by_thread(void *args)
{
    int th_index = *((int *) args);
    IBD_t *cur;
    int i;

    char str1[BUFF1], str2[BUFF1];
    int tmpid;
    int id1, id2;
   
    int N;

    if (Nthreads > 1) N = Nthreads-1;
    else N = Nthreads;

    for(i = 0; i < buffi; i++){   //i - the i-th line in a single file
	if(i % N != th_index)continue;
	sscanf(strbuff[i], "%s\t%s\t%*s", str1, str2); 
	//if it's the header line, we are going to skip it
	if(strcmp(str1,"ID1")==0 && strcmp(str2,"ID2")==0){

		continue;
	}
	
	cur = IBDdat[i];

	cur->pass='1';
	id1 = binary_search_ID_index(str1);
	id2 = binary_search_ID_index(str2);
	threadCount[th_index] ++;
	

	if(cur->pass == '0')strbuff[i][0] = '\0';

	if(id1 == -1 || id2 == -1){
	    cur->pass = '0';
	    strbuff[i][0] = '\0';
	    cur->id1 = id1;
	    cur->id2 = id2;
	    continue;
	}
	//If we are only interested in the cross-country pairs, let's only focus on these pairs.
	if(self==1&&across==0){
	if(strcmp(idhead[id1]->country,idhead[id2]->country)!=0){
		cur->pass = '0';
	    strbuff[i][0] = '\0';
	    continue;

	}

	if(strcmp(idhead[id1]->country,pop1)!=0){
		cur->pass = '0';
		strbuff[i][0] = '\0';
	    continue;
	}

	}


	if(self==0&&across==1){
		// skip the pairs that are from the same country
	if(strcmp(idhead[id1]->country,idhead[id2]->country)==0){
		cur->pass = '0';
	    strbuff[i][0] = '\0';
	    continue;
	}
	// get the right cross-country pair
	if((strcmp(idhead[id1]->country,pop1)!=0 && strcmp(idhead[id1]->country,pop2)!=0)){
		cur->pass = '0';
	    strbuff[i][0] = '\0';
	    continue;
	}

	if((strcmp(idhead[id2]->country,pop1)!=0 && strcmp(idhead[id2]->country,pop2)!=0)){
		cur->pass = '0';
	    strbuff[i][0] = '\0';
	    continue;
	}

	}
	//the default vallues for self=0 and across=0

	if(id1 > id2){ //keep the order id1 < id2, for further ID pair assignment
	    tmpid = id1;
	    id1 = id2;
	    id2 = tmpid;
	}


	if(((id1 + id2) & 1) == 1) { 
	    /*permutate the searching order*/
	    /*after this step, id1 is the head index and id2 is the search index*/
	    tmpid = id1;
	    id1 = id2;
	    id2 = tmpid;
	}

	cur->id1 = id1;
	cur->id2 = id2;

	IBD_seg++;   

    }

    return NULL;
}



void shrink_id_pair(void)
{
    double a;
    long int b;
    if(checkTime == 1){
	a = my_wallclock();
	b = clock();
    }

    pthread_t thread_id[Nthreads];
    int thread_args[Nthreads];
    int rc, i;

    rc = pthread_mutex_init(&mutex2, NULL);
    if(rc){
	printf("\n ERROR code for pthread_mutex_init() is %d \n", rc);
	exit(1);
    }


    for ( i = 0; i < Nthreads; i++ ){
	thread_args[i] = i;
	rc = pthread_create(&thread_id[i], NULL, shrink_id_pair_by_thread, &thread_args[i]);
	if(rc){
	    printf("\n ERROR code from thread %d is %d \n", i, rc);
	    exit(1);
	}

    }
    for ( i = 0; i < Nthreads; i++ )pthread_join(thread_id[i], NULL);
    if(checkTime == 1){
	Tshrinke_id_pair += clock() - b;
	t_shrinke_id_pair += my_wallclock() - a;
	p_std_time(t_shrinke_id_pair);
	fprintf(stderr,"shrink_id_pair() Time = %s\n", timestr);

    }
    pthread_mutex_destroy(&mutex2);

    return ;
}

void *shrink_id_pair_by_thread(void *args)
{
    int th_index = *((int *) args);
    int *stack;
    int *value, *count;
    Pair_t *tmp;
    Pair_t **tmpp;
    int i, j, jj, k,num;
    long int b;
    b = clock();
    //idNum: number of individuals 
    for(i = 0; i < idNum; i++){
    if(i % Nthreads != th_index)continue;



	stack = idhead[i]->stack; //an array 

	num = idhead[i]->pair_num;
	
	if(num == 0){ //this shall never happen
	    free(idhead[i]->stack);
	    idhead[i]->stack = NULL;
	    continue;
	}
	
	//sort out the id lists so that it's ordered from small to large and be enquired
	qsort(&(stack[1]), num, sizeof(int), int_cmp);


	value = (int *)calloc(num, sizeof(int));
	if(value == NULL){
	    my_error("fail to alloc mem for 'value'");
        }

	count = (int *)calloc(num, sizeof(int));
        if(count == NULL){
            my_error("fail to alloc mem for 'count'");
        }

    value[0] = stack[1]; // that shall be the idx of the first individual sharing IBD with
	count[0] = 1;
	k = 0; //K - is the number of unique index
	for(j = 2; j < num+1; j++){
	    if(value[k] == stack[j])count[k] ++;
	    else {
		k++;
		value[k] = stack[j];
		count[k] = 1;
	    }
	}

	free(idhead[i]->stack);
	idhead[i]->stack = NULL;

	num = 0;
	for(j = 0; j < k+1; j++){if(count[j]>=1)num ++;}
	idhead[i]->pair_num = num; // the number of individuals shared more than one IBD segments
	
	tmpp = (Pair_t **)calloc(num, sizeof(Pair_t *));
	if(tmpp == NULL){
	    my_error("fail to alloc mem for idhead[%d]->head, size = %u\n",i, num*sizeof(Pair_t *));
	}
	idhead[i]->head = tmpp;

	//only set up pair structure for the individuals with IBD sharing more than 1
	for(j = 0; j < num; j++){
	    tmp = calloc(1, sizeof(Pair_t));
	    if(tmp == NULL){
		my_error("fail to alloc mem for (idhead[%d]->head)[%d], size = %u\n",i,j, sizeof(Pair_t));
	    }
	    tmp -> index = -1;
	    tmp -> seg_num = 0;
	    tmp -> s1 = 0;
	    tmp -> s2 = 0;
	    (idhead[i]->head)[j] = tmp;
	}
	jj = 0;
	for(j = 0; j < k+1; j++){
	    if(count[j]>=1){
		tmp = (idhead[i]->head)[jj];
		tmp->index = value[j];
		jj ++;
	    }
	}
	free(value);
	free(count);
    }
    threadCount[th_index] += clock()/10000 - b;
    return NULL;
}

void fill_buff_ibd_2nd_pass(void)
{
    double a;
    long int b;

    if(checkTime == 1){
	a = my_wallclock();
	b = clock();
    }
    pthread_t thread_id[Nthreads+1];
    int thread_args[Nthreads];
    int rc, i;
    int N;
	if (Nthreads > 1) N = Nthreads - 1;
else N = Nthreads;

    for ( i = 0; i < N; i++ ){
	thread_args[i] = i;
	rc = pthread_create(&thread_id[i], NULL, fill_buff_ibd_2nd_pass_by_thread, &thread_args[i]);
	if(rc){
	    printf("\n ERROR code from thread %d is %d \n", i, rc);
	    exit(1);
	}

    }
    read_buff();
    for ( i = 0; i < N; i++ ){
rc = pthread_join(thread_id[i], NULL);
if(rc > 0) printf("\n ERROR code from thread %d is %d \n", i, rc);
}

    if(checkTime == 1){
	Tfill_buff_ibd2 += clock() - b;
	t_fill_buff_ibd2 += my_wallclock() - a;
    }
    return ;
}

void *fill_buff_ibd_2nd_pass_by_thread(void *args) // 
{
    int th_index = *((int *) args);
    IBD_t *cur;
    int i;
    int N;

    int tmpid;
    int id1, id2,segnum;
    float s1, s2,s_total;
    char str1[BUFF1], str2[BUFF1];


    if (Nthreads > 1) N = Nthreads - 1;
    else N = Nthreads;

    for(i = 0; i < buffi; i++){
	if(i % N != th_index)continue;

	sscanf(strbuff[i], "%s\t%s\t%d\t%f\t%f\t%f", str1, str2, &segnum, &s1, &s2,&s_total); 
	if(strcmp(str1,"ID1")==0 && strcmp(str2,"ID2")==0){ 
		continue;

	}
	cur = IBDdat[i];
		
	cur->pass = '1'; //"1" means pass to the memory, '0' means skip this pair

	threadCount[th_index] ++;
	id1 = binary_search_ID_index(str1);
	id2 = binary_search_ID_index(str2);
	if(cur->pass == '0')strbuff[i][0] = '\0';

	if(id1 == -1 || id2 == -1){
		
	    cur->pass = '0';
	    strbuff[i][0] = '\0';
	    cur->id1 = id1;
	    cur->id2 = id2;
	    continue;
	}

	
	//if only look at individuals within the populations, ignore the pairs across populations
	if(self==1&&across==0){
	if(strcmp(idhead[id1]->country,idhead[id2]->country)!=0){
		cur->pass = '0';
	    strbuff[i][0] = '\0';
	    continue;
	}
	if(strcmp(idhead[id1]->country,pop1)!=0){
		cur->pass = '0';
		strbuff[i][0] = '\0';
		continue;
	}
	}

	if(self==0&&across==1){
	if(strcmp(idhead[id1]->country,idhead[id2]->country)==0){
		cur->pass = '0';
	    strbuff[i][0] = '\0';
	    continue;
	}

	if((strcmp(idhead[id1]->country,pop1)!=0 && strcmp(idhead[id1]->country,pop2)!=0)){
		cur->pass = '0';
	    strbuff[i][0] = '\0';
	    continue;
	}

	if((strcmp(idhead[id2]->country,pop1)!=0 && strcmp(idhead[id2]->country,pop2)!=0)){
		cur->pass = '0';
	    strbuff[i][0] = '\0';
	    continue;
	}

	}
	

	if(id1 > id2){ //keep the order id1 < id2, for further ID pair assignment
	    tmpid = id1;
	    id1 = id2;
	    id2 = tmpid;
	}


	if(((id1 + id2) & 1) == 1) { 
	    /*permutate the searching order*/
	    /*after this step, id1 is the head index and id2 is the search index*/
	    tmpid = id1;
	    id1 = id2;
	    id2 = tmpid;
	}
	//checking output

	cur->id1 = id1;cur->id2 = id2;


	if(s1 >= max_g || s2 >= max_g || s1<0||s2<0){
	    cur->pass = '0';
	    strbuff[i][0] = '\0';
	    continue;
	}

	cur->s1 = s1;
	cur->s2 = s2;
	cur->s_total = s_total;
	cur->segnum=segnum;
	
	IBD_seg++;   	

    }
    return NULL;
}

void store_buff_ibd_2nd(void)
{
    double a;
    long int b;


    if(checkTime == 1){
	a = my_wallclock();
	b = clock();
    }
    pthread_t thread_id[Nthreads];
    int thread_args[Nthreads];
    int rc, i;

    for ( i = 0; i < Nthreads; i++ ){
	thread_args[i] = i;
	rc = pthread_create(&thread_id[i], NULL, store_buff_ibd_2nd_by_thread, &thread_args[i]);
	if(rc){
	    printf("\n error code from thread %d is %d \n", i, rc);
	    exit(1);
	}

    }
    for ( i = 0; i < Nthreads; i++ )pthread_join(thread_id[i], NULL);

    if(checkTime == 1){
	Tstore_buff_ibd += clock() - b;
	t_store_buff_ibd += my_wallclock() - a;
    }



    return ;
}

void *store_buff_ibd_2nd_by_thread(void *args)
{
    int b = clock()/10000;

    int th_index = *((int *) args);
    IBD_t *ibdcur;
    int i,ii;
    int id1, id2;
    float s1, s2,s_total;
    int  segnum=0; 


    for(i = 0; i < buffi; i++){

	ibdcur = IBDdat[i];
	

	if(ibdcur->pass != '1')continue;
	id1 = ibdcur->id1;
	id2 = ibdcur->id2;
	if(id1 % Nthreads != th_index)continue;
	s1 = ibdcur->s1;
	s2 = ibdcur->s2;
	s_total = ibdcur->s_total;
	segnum = ibdcur->segnum;
	
	//check whether id2 is already exists
	ii = binary_search_pair(idhead[id1], id2);
	
	//if id2 is not yet in the Pair structure, (1)set up a new Pair (2)update pair_num 
	if(ii == -1){
		fprintf(stderr,"id2:%s is not stored in the first round for id1:%s\n",idhead[id1]->id,idhead[id2]->id);

 	}else{
 		idhead[id1]->seg_num+=segnum;
 		(idhead[id1]->head)[ii]->s1+=s1;
 		(idhead[id1]->head)[ii]->s2+=s2;
 		(idhead[id1]->head)[ii]->s_total+=s_total;
 		(idhead[id1]->head)[ii]->seg_num+=segnum;
 	
 	}

    }
    threadCount[th_index] += clock()/10000 - b;

    return NULL;
}
