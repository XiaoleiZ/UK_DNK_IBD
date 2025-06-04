#include "head.h"
#include "tools.h"

int hash_str(char *str)
{
    int i;
    int l = strlen(str);
    int out;
    long int sum = 0;
    const int p = 7;
    for(i = 0; i < l; i++)sum += (int)str[i]*(int)pow(p,i);
    sum = sum % 991817;
    out = abs((int)sum);
    return out;
}

int ID_t_cmp(const void *a, const void *b)
{
    const ID_t *ia = *(const ID_t **)a;
    const ID_t *ib = *(const ID_t **)b;
    return(strcmp(ia->id, ib->id));
}

int int_cmp(const void *a, const void *b)
{
    const int *ia = (const int *)a; // casting pointer 
    const int *ib = (const int *)b;
    return (*ia  - *ib);
}


int float_cmp(const void *a, const void *b)
{
    const float *ia = (const float *)a; // casting pointer 
    const float *ib = (const float *)b; 
    if ((*ia  - *ib) >= 0) return 1;
    else return -1;
}



int double_cmp(const void *a, const void *b)
{
    const double *ia = (const double *)a; // casting pointer 
    const double *ib = (const double *)b; 
    if ((*ia  - *ib) >= 0) return 1;
    else return -1;
}


int string_cmp(const void *p1, const void *p2)
{
    return strcmp(* (char * const *) p1, * (char * const *) p2);
}


int binary_search_string(char **S, int L, char *s)
{
    if(strcmp(s, S[0]) <0 || strcmp(s, S[L-1]) >0) return -1;
    else {
	int tmp, i;
	int from, to, new;
	from=0; to= L-1;
	while(1){
	    new = (int)(from+to)/2;
	    tmp = strcmp(s, S[new]);
	    if(tmp == 0) return new;
	    else if(tmp > 0)from = new;
	    else to = new;
	    if(to - from < 3){
		for(i = from; i <= to; i++){
		    if(strcmp(s, S[i])==0)return i;
		}
		break;
	    }
	}
	return -1;
    }
}

int binary_search_ID_index(char *id)
{
    if(strcmp(id, idhead[0]->id) < 0 || strcmp(id, idhead[idNum-1]->id) > 0)return -1;
    else {
	int tmp, i;
	int from, to, new;
	char *str;
	from=0; to= idNum-1;
	while(1){
	    new = (int)(from+to)/2;
	    str = idhead[new]->id;
	    tmp = strcmp(id, str);
	    if(tmp == 0) return new;
	    else if(tmp > 0)from = new;
	    else to = new;
	    if(to - from < 3){
		for(i = from; i <= to; i++){
		    if(strcmp(id, idhead[i]->id)==0)return i;
		}
		break;
	    }
	}
	return -1;
    }
}



//*** need this****//
int binary_search_pair(ID_t *id, int index)
{
    int num = id->pair_num;
    if(num == 0)return -1;
    Pair_t **cur =id->head;

    if(id->head==NULL){
    	fprintf(stderr,"error no pair exists for sample %s\n",id->id);
    }
    if(index < (cur[0]->index) || index > (cur[num-1]->index))return -1;
    else {
	int tmp, i;
	int from, to, new;
	from=0; to= num-1;
	while(1){
	    new = (int)(from+to)/2;
	    tmp = index - cur[new]->index;
	    if(tmp == 0) return new;
	    else if(tmp > 0)from = new;
	    else to = new;
	    if(to - from < 3){
		for(i = from; i <= to; i++){
		    if((index - cur[i]->index)==0)return i;
		}
		break;
	    }
	}
	return -1;
    }

}


int binary_search(float pos, float *map, int mapl)
{
    if(pos < map[0]) return -1;
    else if (pos >= map[mapl - 1]) return mapl-1;
    else {
	int tag = 0;
	int i_old1, i_old2, i_new;
	i_old1=0; i_old2= mapl-1;
	while(tag == 0){
	    i_new = (int)(i_old1+i_old2)/2;
	    if(pos >= map[i_new] && pos < map[i_new+1])return(i_new);
	    else if(pos < map[i_new])i_old2 = i_new;
	    else if(pos >= map[i_new+1])i_old1 = i_new;
	    if(i_old2 == i_old1)break;
	}
    }
    fprintf(stderr, "Error in genetic position interpolation, please check map file\n");
    exit(-1);
    return -1;
}


double my_wallclock(void)
{
    double T;
    struct timeval timecheck;

    gettimeofday(&timecheck, NULL);
    T = timecheck.tv_sec  + (double)timecheck.tv_usec / 1000000;
    return T;
}

void init_lock(void)
{
    int i;
    for(i = 0; i < idNum; i++)lock[i]='0';
    return ;
}






void free_all(void)
{
    int i, j;
    free(headfile);
    for(i = 0; i < Nfile; i ++){free(ibdsum_file[i]);}
    free(ibdsum_file);

   
    free(idfile);

    free(threadCount);
    free(memSizeByThread);
    free(pairNumByThread);
    free(segNumByThread);

    free(outfile);

   
    for(i = 0; i < BUFF2 ;i ++){
	free(IBDdat[i]);
	free(strbuff[i]);
	free(strbuff2[i]);
    }
    free(IBDdat);
    free(strbuff);
    free(strbuff2);

    for (i = 0; i < Nthreads; i++){
	for(j = 0; j < chrN; j ++)
	
	free(outbuff[i]);
    }
    free(outbuff);
    free(outbuffi);

    for(i = 0; i < idNum; i ++ ){
	if(tagKinship == 1){
	    for(j = 0; j < idhead[i]->pair_num; j++){
		free((idhead[i]->head)[j]);
	    }
	    free(idhead[i]->head);
	}
	free(idhead[i]->id);
	free(idhead[i]);
    }
    free(idhead);

    
}


void my_error(const char * message, ...)
{
    va_list args;
    va_start (args, message);
    vfprintf (stderr, message, args);
    va_end (args);
    fprintf (stderr, "\n");
    exit (EXIT_FAILURE);
}
