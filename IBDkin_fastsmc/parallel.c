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


void write_buff(void)
{
    double a = my_wallclock();
    long int b = clock();

    gzFile fp = gzopen(outfile, "a");
    gzbuffer(fp, BUFF3); 
    int i;
    for(i = 0; i < buffi; i++){
	if(strbuff[i][0]!='\0'){
	
		gzputs(fp, strbuff[i]);
	}
    }
    gzclose(fp);
    Tgzputs += clock() - b;
    t_gzputs += my_wallclock() - a;
    return ;
}

int cal_seg_num(hapIBD_t *head)
{
int segnum = 0;
while(head!=NULL){
segnum += 1;
head = head -> next;
}

return segnum;
}



void cal_pair_num(void)
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

    rc = pthread_mutex_init(&mutex1, NULL);
    
    init_lock();
    if(rc){
	printf("\n ERROR code for pthread_mutex_init() is %d \n", rc);
	exit(1);
    }

    for ( i = 0; i < Nthreads; i++ ){
	thread_args[i] = i;
	rc = pthread_create(&thread_id[i], NULL, cal_pair_num_by_thread, &thread_args[i]);
	if(rc){
	    printf("\n ERROR code from thread %d is %d \n", i, rc);
	    exit(1);
	}

    }

    pairNum = 0; segNum = 0;
    for ( i = 0; i < Nthreads; i++ ){
	pthread_join(thread_id[i], NULL);
	pairNum += pairNumByThread[i]; pairNumByThread[i] = 0;
	segNum += segNumByThread[i]; segNumByThread[i] = 0;
    }

    pthread_mutex_destroy(&mutex1);

    fprintf(stderr, "\t %ld IBD segments + %ld ID pairs are kept.\n", segNum, pairNum);
    if(checkTime == 1){
	t_cal_pair_num += my_wallclock() - a;
	Tcal_pair_num += clock() - b;
    }
    return ;
}





void *cal_pair_num_by_thread(void *args)
{
    int th_index = *((int *) args);
    Pair_t *cur;

    int i, j;

    for(i = 0; i < idNum; i++){

	pthread_mutex_lock(&mutex1);
	if(lock[i] == '1'){pthread_mutex_unlock(&mutex1);continue;}
	lock[i] = '1';
	pthread_mutex_unlock(&mutex1);

	pairNumByThread[th_index] += idhead[i]->num;
	if(idhead[i]->head != NULL){
	    for(j = 0; j < idhead[i]->num; j++){	
	    cur = (idhead[i]->head)[j];
	    segNumByThread[th_index] += cal_seg_num(cur->ibdhead);
	    }
	}
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
    init_lock();
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
    //idNum: number of individuals 
    for(i = 0; i < idNum; i++){

	pthread_mutex_lock(&mutex2);
	if(lock[i] == '1'){pthread_mutex_unlock(&mutex2);continue;}
	lock[i] = '1';
	pthread_mutex_unlock(&mutex2);


	stack = idhead[i]->stack; //an array 
	memSizeByThread[th_index] -= (stack[0]) * sizeof(int);
	num = idhead[i]->num;
	if(num == 0){
	    free(idhead[i]->stack);
	    idhead[i]->stack = NULL;
	    continue;
	}

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
	k = 0;
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
	for(j = 0; j < k+1; j++){if(count[j]>1)num ++;}
	idhead[i]->num = num; // the number of individuals shared more than one IBD segments
	
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
	    memSizeByThread[th_index] += sizeof(Pair_t) + sizeof(Pair_t *);
	    tmp -> index = -1;
	    tmp -> segnum = 0;
	    tmp -> s1 = 0;
	    tmp -> s2 = 0;
	    tmp -> ibdhead = NULL;
	    (idhead[i]->head)[j] = tmp;
	}
	jj = 0;
	for(j = 0; j < k+1; j++){
	    if(count[j]>1){
		tmp = (idhead[i]->head)[jj];
		tmp->index = value[j];
		jj ++;
	    }
	}
	free(value);
	free(count);
    }
    return NULL;
}




void cal_kinship(void)
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

    for ( i = 0; i < Nthreads; i++ ){	rc = pthread_create(&thread_id[i], NULL, cal_kinship_by_thread, &thread_args[i]);

	thread_args[i] = i;
	if(rc){
	    printf("\n error code from thread %d is %d \n", i, rc);
	    exit(1);
	}

    }
    for ( i = 0; i < Nthreads; i++ )pthread_join(thread_id[i], NULL);
    if(checkTime == 1){ 
	Tcal_kinship += clock() - b;
	t_cal_kinship += my_wallclock() - a;
    }
    return ;
}

void *cal_kinship_by_thread(void *args)
{
    //int b = clock();
    int th_index = 0;
    th_index=*((int *) args);
    IBD_t *ibdcur;
    int i, ii;
    int id1, id2;
    char h1, h2; 
    float g1, g2;
    for(i = 0; i < buffi; i++){

	ibdcur = IBDdat[i];
	if(ibdcur->pass != '1'){continue;}

	id1 = ibdcur->id1;
	id2 = ibdcur->id2;
	if(id1 % Nthreads != th_index)continue;

	g1 = ibdcur->g1;
	g2 = ibdcur->g2;

	h1 = ibdcur->h1;
	h2 = ibdcur->h2;

	

	ii = binary_search_pair(idhead[id1], id2);
	
	if(ii == -1){



		//for pair who share one IBD segment
	    if((g2 - g1 >= IBDcM1)&&(g2 - g1<IBDcM2)){
		
		hapIBD_t *tmp = calloc(1, sizeof(hapIBD_t));
		tmp->h1 = h1;tmp->h2 = h2;
		tmp->g1 = g1;tmp->g2 = g2;
		tmp->next = NULL;
		output(tmp, id1, id2, i);
		pair_1ibd+=pair_1ibd;
		free(tmp);}
		//strbuff[i][0]='\0';
	    else {strbuff[i][0]='\0';}
	}
	else {
		
		
	    memSizeByThread[th_index] += push_ibd(&((idhead[id1] -> head)[ii]->ibdhead), h1, h2, g1, g2);
	    strbuff[i][0]='\0';
		

	}
    }
    return NULL;
}


void output(hapIBD_t *head, int idi, int ids, int buf)
//kinship coefficient based on one segment
{	

    int segnum, tmp;
    float s0, s1, s2,s_total,k1,kinship;
    char degree[8];
    segnum = 0; s0 = 0; k1=0;s1 = 0; s2 = 0;s_total=0;
    segnum = calIBD12_pair(head, &s0, &s1, &s2,&s_total);
    k1 = s1/totg;
   
    kinship = k1/4;

    if(kinship >= kincut){
  
	tmp = cal_degree(kinship);
	sprintf(degree,"%d",tmp);

	sprintf(strbuff[buf], "%s\t%s\t%d\t%f\t0\t%f\t%f\n", idhead[idi]->id, idhead[ids]->id, segnum,s1,s1,s_total);

    }
	else strbuff[buf][0]='\0';
    return ;
}


void cal_kinship2(void)//for close relatives
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
	rc = pthread_create(&thread_id[i], NULL, cal_kinship2_by_thread, &thread_args[i]);
	if(rc){
	    printf("\n error code from thread %d is %d \n", i, rc);
	    exit(1);
	}

    }
    for ( i = 0; i < Nthreads; i++ ){
	pthread_join(thread_id[i], NULL);
    }
    if(checkTime == 1){
	Tcal_kinship += clock() - b;
	t_cal_kinship += my_wallclock() - a;
    }
    
    return ;
}

void *cal_kinship2_by_thread(void *args)
{
    int th_index = *((int *) args);
    Pair_t *prcur;
    int i, j;

    for(i = 0; i < idNum; i++){


	if(i % Nthreads != th_index)continue; //pairs are split into threads based on id1; same pair is definitely in the same thread
	for(j = 0; j < idhead[i]->num; j++){ 

	    prcur = (idhead[i]->head)[j];

	    filter_IBD(prcur);

	    output2(prcur);

	    if(prcur->ibdhead!=NULL){
	    memSizeByThread[th_index] += freehapIBDList(prcur->ibdhead);}
	    prcur->ibdhead = NULL;
	}
    }
    return NULL;
}

void output2(Pair_t *head)
{

    int segnum;
    float s0, s1, s2,s_total;
    s0=0; s1 = 0; s2 = 0,s_total=0;

    segnum = calIBD12_pair(head->ibdhead, &s0, &s1, &s2,&s_total);
    s1 = s1;
    s2 = s2;

    head-> segnum += segnum;
    head-> s1 += s1;
    head-> s2 += s2;
    head-> s_total+=s_total;

    return ;
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
    int i, chr;
    int id1, id2;
    float g1, g2;
    int num, L;
    chr = CHR;
    for(i = 0; i < buffi; i++){

	ibdcur = IBDdat[i];
	if(ibdcur->pass != '1')continue;
	id1 = ibdcur->id1;
	id2 = ibdcur->id2;
	if(id1 % Nthreads != th_index)continue;

	//add length filters
	if((ibdcur->l < IBDcM1)||(ibdcur->l>=IBDcM2))continue;//filters //Filter out short segments

	//update genetic ranges
	g1 = ibdcur->g1;
	g2 = ibdcur->g2;
	chr = ibdcur->chr;


	//for the first pass, the range is found and saved using genomic unit
	if(g1 < minpos[chr][th_index]){minpos[chr][th_index] = g1;}
	if(g2 > maxpos[chr][th_index]){maxpos[chr][th_index] = g2;}


	//push pairs
	idhead[id1]->num += 1; //Attention: it only stores into id1 info instead of both id1 and id2 -> that makes sure that a pair is only visited once
	num = idhead[id1]->num;
	L = (idhead[id1]->stack)[0]; //shall it all be 4 here?  Yes L=4

	if(num > L - 1){ //num>3
	    memSizeByThread[th_index] -= L * sizeof(int); 
	    while (num > L - 1) L = (int)(L * 1.25) + 5;
	    idhead[id1]->stack = realloc(idhead[id1]->stack, L * sizeof(int));
	    memSizeByThread[th_index] += L * sizeof(int); 
	    (idhead[id1]->stack)[0] = L;
	}
	(idhead[id1]->stack)[num] = id2;
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
    int chr, id1, id2;
    float p1, p2, l, prob=0;
    int N;


    chr = CHR;

    if (Nthreads > 1) N = Nthreads-1;
    else N = Nthreads;

    for(i = 0; i < buffi; i++){   //i - the i-th line in a single file
	if(i % N != th_index)continue;

	sscanf(strbuff[i], "%[^\t] %*[^\t] %*[^\t] %[^\t] %*[^\t] %*[^\t] %*s %f %f %f %f %*s", str1, str2, &p1, &p2, &l, &prob); 

	cur = IBDdat[i];


	if((prob<IBDprob)||(l<IBDcM1)||(l>=IBDcM2)|| (prob>1)) {
		//only the one between IBDcM1 and IBDcM2 
		cur->pass = '0';
		strbuff[i][0] = '\0';
		continue;}
	IBD_seg++;     //the number would change if I split the dataset
	if((l>=IBDcM1) && (l <IBDcM2)) cal_coverage(th_index, chr, p1, p2);
	
	if(Parts > 1){   //Parts - number of chunks the dataset is splitted into
	    if((hash_str(str1) + hash_str(str2)) % Parts == part - 1) cur->pass = '1';
	    else cur->pass = '0';
	}
	else cur->pass = '1'; //"1" means writing the pair to the memory, '0' means skipping this pair
	
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
	cur->g1 = interpolate2cM(chr, p1);
	cur->g2 = interpolate2cM(chr, p2);
	//cur->g1=p1;
	//cur->g2=p2;
	

	if((cur->g2-cur->g1<IBDcM1)||(cur->g2-cur->g1>=IBDcM2)){
		cur->pass='0';
		strbuff[i][0]='\0';
		continue;
	}
	
	cur->chr = chr;
	cur->l = l;
	
    
	}

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

void *fill_buff_ibd_2nd_pass_by_thread(void *args)
{
    int th_index = *((int *) args);
    IBD_t *cur;
    int i;
    int N;

    int tmpid;
    int chr, id1, id2;
    float p1, p2, l, prob=0;
    char h1, h2, tmph;
    char str1[BUFF1], str2[BUFF1];
    chr = CHR;

    if (Nthreads > 1) N = Nthreads - 1;
    else N = Nthreads;

    for(i = 0; i < buffi; i++){
	if(i % N != th_index)continue;
	
	sscanf(strbuff[i], "%[^\t] %*[^\t] %c %[^\t] %*[^\t] %c %*s %f %f %f %f", str1, &h1, str2, &h2, &p1, &p2, &l,&prob); 
	
	cur = IBDdat[i];

	

	if((prob<IBDprob) || (l<IBDcM1) || (l>=IBDcM2) || (prob>1)) {
		strbuff[i][0] = '\0';	
		cur->pass='0';
		continue;
	}
	
	if(Parts > 1){
	    if((hash_str(str1) + hash_str(str2)) % Parts == part - 1) cur->pass = '1';
	    else cur->pass = '0';
	}
	else cur->pass = '1'; //"1" means pass to the memory, '0' means skip this pair

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

	if(id1 > id2){ //keep the order id1 < id2, for further ID pair assignment
	    tmpid = id1; tmph = h1;
	    id1 = id2; h1 = h2;
	    id2 = tmpid; h2 = tmph;
	}


	if(((id1 + id2) & 1) == 1) { 
	    //permutate the searching order
	    //after this step, id1 is the head index and id2 is the search index
	    tmpid = id1; tmph = h1;
	    id1 = id2; h1 = h2;
	    id2 = tmpid; h2 = tmph;
	}

	cur->id1 = id1;
	cur->id2 = id2;


	cur->h1 = h1;
	cur->h2 = h2;

	cur->chr = chr;
	cur->g1 = interpolate2cM(chr, p1);
	cur->g2 = interpolate2cM(chr, p2);
	

	if((cur->g2-cur->g1<IBDcM1)||(cur->g2-cur->g1>=IBDcM2)){
		cur->pass='0';
		strbuff[i][0]='\0';
		continue;
	}
	
    }
    return NULL;
}



hapIBD_t * bubbleSort(hapIBD_t* head,int segnum){
	bool swapped=false;
	hapIBD_t *sort_head,*p0,*tmp,*p1,*p2;
	int i;


	
	sort_head=head;

	for(i=0;i<segnum-1;i++){
		swapped=false;
		p0=sort_head;
		p1=p0;
		p2=p1->next;
		while(p2){
			
			if (p1->g1>p2->g1){
				swapped=true;
				if(p0!=p1){
			
				//if not the first element swapped
				p1->next=p2->next;
				p0->next=p2;
				p2->next=p1;
				tmp=p1;
				p1=p2;
				p2=tmp;
				

			}else{
			

				p0=p2;
				sort_head=p0;
				p1->next=p2->next;
				p0->next=p1;
				p2=p1;
				p1=p0;

			}

		}

		if(p0!=p1){
				p0=p0->next;				
		}
		p1=p1->next;
		p2=p2->next;
		}
	if(!swapped){
		break;
	}
		
	}
	return sort_head;
	}


void print_IBD(hapIBD_t * head){

	hapIBD_t * cur;
	cur=head;
	while(cur){
		fprintf(stderr,"cur: hap:%d %d; start:%.2f end:%.2f\n",cur->h1,cur->h2,cur->g1,cur->g2);
		cur=cur->next;
	}
	return;

}


void filter_IBD(Pair_t * pair_head){
	//removing overlapping IBDs

	hapIBD_t *cur=NULL,*head=NULL;
	hapIBD_t *hap1=NULL,*hap2=NULL,*hap3=NULL,*hap4=NULL;
 	hapIBD_t *hap1_cur,*hap2_cur,*hap3_cur,*hap4_cur;
 	hapIBD_t *lastlast, *last, *overlap;
	hapIBD_t *new_head=NULL,*tmp=NULL;

	int segnum;

	head=pair_head->ibdhead;
	cur=pair_head->ibdhead;segnum=0;

	segnum=cal_seg_num(cur);

	pair_head->ibdhead=bubbleSort(cur,segnum);
	head=pair_head->ibdhead;

	cur=head;

	

	if(segnum<1||remove_overlap!=1){
		return;
	}

	int overlap_count;
	overlap_count=0;
	
	//1. seperate each hap pair
		
		while(cur){

		if((cur->h1==haptag[0])&&(cur->h2==haptag[0])){
			if(hap1){
				hap1_cur->next=cur;
				hap1_cur=hap1_cur->next;
			}else{
				hap1=cur;
				hap1_cur=cur;

			}}else if((cur->h1==haptag[0])&&(cur->h2==haptag[1])){
				if(hap2){
					hap2_cur->next=cur;
					hap2_cur=hap2_cur->next;
				}else{
					hap2=cur;
					hap2_cur=cur;
				}

			}else if((cur->h1==haptag[1])&&(cur->h2==haptag[0])){
				if(hap3){
					hap3_cur->next=cur;
					hap3_cur=hap3_cur->next;
				}else{
					hap3=cur;
					hap3_cur=cur;
				}
			}else if((cur->h1==haptag[1])&&(cur->h2==haptag[1])){
				if(hap4){
					hap4_cur->next=cur;
					hap4_cur=hap4_cur->next;

				}else{
					hap4=cur;
					hap4_cur=cur;
				}
			}
			cur=cur->next;
		}

		if(hap1){
		hap1_cur->next=NULL;
		int seg1=0;
		hap1_cur=hap1;
		while(hap1_cur){
			seg1+=1;
			hap1_cur=hap1_cur->next;
		}
		}
		if(hap2){
		hap2_cur->next=NULL;
		int seg2=0;
		hap2_cur=hap2;
		while(hap2_cur){
			seg2+=1;
			hap2_cur=hap2_cur->next;
		}
		}

		if(hap3){
		hap3_cur->next=NULL;
		}
		if(hap4){
		hap4_cur->next=NULL;
		}


	hapIBD_t *hap[4]={hap1,hap2,hap3,hap4};



	hapIBD_t *tmp_head=NULL;
	for(int i=0;i<4;i++){


		//loop over the hap array 
		cur=NULL;
		tmp_head = hap[i];
		if(!tmp_head){
			continue;
		}else{
			lastlast=tmp_head;
			last=tmp_head;
			cur=last->next;
		
		}
		

		while(cur){
			
					if(((last->g1<=cur->g1) && (last->g2>=cur->g1))|| ((last->g1<=cur->g2) && (last->g2>=cur->g2))){   //check syntax
					//check if there is an overlap
					overlap_count+=1;
					
					if(last->g2-last->g1>=cur->g2-cur->g1){  
					// The default choice is to keep the larger one
					 //keep the last one, remove current one
					overlap = cur;
					last->next=cur->next;  
					cur=cur->next;
					}else{
					//keep the current one, remove the last one
					
					overlap=last;
					if(last!=hap[i]){
					lastlast->next=cur;
					last=cur;
					cur=cur->next;
					}else if(last==hap[i]){
					
					overlap=last;
					lastlast=cur;
					last=cur;
					cur=cur->next;

					
					hap[i]=last;
					tmp_head=hap[i];  //also need to update the start of the hap array : or may use the address form
					
					}
					

				}
					
					segnum=segnum-1;
					if(overlap->next){overlap->next=NULL;}
					
					free(overlap);
					

				}else{
					//if there is no overlap
					if(last!=hap[i]){
						lastlast=last;
						last=cur;
						cur=cur->next;
					}else{
						last=cur;
						cur=cur->next;	
					}
				}
			}
		}


		
		for(int i=0;i<4;i++){
			if(hap[i]!=NULL){
				if(new_head==NULL){
					new_head=hap[i];
			}else{
				tmp->next=hap[i];
			}
			tmp=hap[i];
			while(tmp->next!=NULL){
				tmp=tmp->next;
				}
			}
		}
		

		
	if(new_head!=NULL){pair_head->ibdhead=new_head;}
	total_overlap+=overlap_count;
	
	return;
}





int calIBD12_pair(hapIBD_t* head, float *S0, float *S1, float *S2,float *S_total)
{
    int L, segnum;
    int i, j, chr;
    int state;
    float from, to; 
    float a, b, mid, inc;
    int htag[2][2];
    char h1, h2;
    hapIBD_t *cur;
    float s0, s1,s2,s_total,chrL; //s_total = total IBD lengths shared regardless of the IBD1 or IBD2
    s0 = 0; s1 = 0; s2 = 0,s_total=0; 


    //variables for removing overlap
    
   

    cur = head;segnum = 0;
    chr = CHR;


    /////////////////////////////////////////////////Get the number of IBD segments//////////////////////////////
    while(cur){
	segnum += 1;
	cur = cur-> next;
    } // only to get the segment number
    chrL = maxPos[chr] - minPos[chr] - apply_mask(chr, minPos[chr], maxPos[chr]);
    

    

	//steps
	//1. order the linked list 
	//2. then figure out the situation where more than 2 IBD segments overlapped
	//sorting the linked list by the start genomic coordinate
	//after bubble sort return the new address of the linked list
	

	//////////////////////////////////////////////////////////Bubble Sort//////////////////////////////////////////	
	


	cur=head;
	
    // segnum = number of IBD segments
    if(segnum == 0){
	s0 =  chrL;
	s1 = 0;
	s2 = 0;
    }
    else if(segnum == 1){
    
	s1 = head->g2 -head->g1 - apply_mask(chr, head->g1, head->g2);
	s0 = chrL - s1;
	s2 = 0;
	s_total=s1;
	
	}
    else
    {
	cur = head;
	L = segnum * 2 + 2; //number of IBD segments
	float *segs; //store segments
	int *ibdstate; //store ibd0, 1, 2
	segs = (float *)calloc(L, sizeof(float));  //list of coordinates ordered from small to large
	ibdstate = (int *)calloc(L, sizeof(int));
	segs[0] = minPos[chr];
	segs[L-1] = maxPos[chr];
	i = 1;
	
	
	//using the updated segments
	while(1){
	    segs[i] = cur->g1; //g1-the start position of a segment in genetic distance
	    segs[i+1] = cur->g2;//g2 - the end position of a segment in genetic distance
	    i += 2; 
	    s_total=s_total+ cur->g2 - cur->g1 - apply_mask(chr,cur->g1,cur->g2);
	    if(cur->next == NULL)break;
	    cur = cur-> next;
	}


	qsort(segs, L, sizeof(float), float_cmp);
	

	//now segs is sorted -> what happended if segments are overlapped

	//calculate ibd0, ibd1, ibd2
	for (i = 0; i < L-1; i++){
	    a = segs[i];b = segs[i+1];mid = (a + b)/2;
	    if(a > b){
		//for(j = 0; j < L; j++)fprintf(stderr, " %f", segs[j]);
		//fprintf(stderr, "%f %f\n", a, b);
	    }
	    htag[0][0] = 0;htag[0][1] = 0;
	    htag[1][0] = 0;htag[1][1] = 0;
	    cur = head;
	    while(cur){
		if(cur->g1 <=mid && cur->g2 >mid){
		    h1 = cur->h1;h2 = cur->h2;
		    if(h1 == haptag[0] && h2 == haptag[0])htag[0][0] = 1;
		    if(h1 == haptag[0] && h2 == haptag[1])htag[0][1] = 1;
		    if(h1 == haptag[1] && h2 == haptag[0])htag[1][0] = 1;
		    if(h1 == haptag[1] && h2 == haptag[1])htag[1][1] = 1;
			//fprintf("%d\t%d\t%d\t%d\n",htag[0][0],htag[0][1],htag[1][0],htag[1])
		}
		cur = cur-> next;
	    }
	    //debug on the issues
	   
	    if((htag[0][0] == 1 && htag[1][1] == 1) \
		    || (htag[0][1] == 1 && htag[1][0] == 1)) ibdstate[i] = 2;
	    else if(htag[0][0] == 0 && htag[1][1] == 0 \
		    && htag[0][1] == 0 && htag[1][0] == 0 ) ibdstate[i] = 0;
	    else ibdstate[i] = 1;
		
	}


	//gapfilling
	for(i = 0; i < L-1; i++){
	    ////fill ibd0
	    if(ibdstate[i] == 0){
		j = i;while(j > 0 ){if(ibdstate[j] > 0)break;j--;}
		if( j == 0)from = segs[0] - gap1;
		else from = segs[j+1];

		j = i;while(j < L ){if(ibdstate[j] > 0)break;j++;}
		if(j == L)to=segs[j-1] + gap1;
		else to=segs[j];
		if((to - from) < gap1)ibdstate[i] = 1;
	    }

	    ////fill ibd2
	    
	    if(ibdstate[i] != 2){
		j = i;while(j > 0 ){if(ibdstate[j] == 2)break;j--;}
		if( j == 0)from = segs[0] - gap2;
		else from = segs[j+1];

		j = i;while(j < L ){if(ibdstate[j] == 2)break;j++;}
		if(j == L)to=segs[j-1] + gap2;
                else to=segs[j];
		if((to - from) < gap2)ibdstate[i] = 2;

	    }

	}


	//calculte s0, s1, s2
	for (i = 0; i < L-1; i++){
	    a = segs[i];b = segs[i+1];state=ibdstate[i];

	    inc = b -a - apply_mask(chr, a, b);
	    //fprintf(stderr,"%d: %f %f inc:%f\n",state, b, a, inc);
	    if(state == 0) s0 +=inc;
	    if(state == 1) s1 +=inc;
	    if(state == 2) s2 +=inc;
	    //fprintf(stderr,"s1:%f\n",s1);
	}

	free(segs);
	free(ibdstate);
	
	}

    *S0 += s0;
    *S1 += s1;
    *S2 += s2;
    *S_total += s_total;

    return segnum;
}
