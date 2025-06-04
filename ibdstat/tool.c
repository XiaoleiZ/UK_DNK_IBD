#include "head.h"
#include "tool.h"


int ID_t_cmp(const void *a, const void *b)
{
    const ID_t *ia = *(const ID_t **)a;
    const ID_t *ib = *(const ID_t **)b;
    return(strcmp(ia->id, ib->id));
}


int string_cmp(const void *a, const void *b) 
{ 


    const char *ia = (const char *)a;
    const char *ib = (const char *)b;

    
    return strcmp(ia, ib);
	/* strcmp functions works exactly as expected from
	comparison function */ 
} 

int Geo_t_cmp(const void *a, const void *b){
	    const Geo_t *ia = *(const Geo_t **)a;
    	const Geo_t *ib = *(const Geo_t **)b;
    	return(strcmp(ia->name,ib->name));
}


int close_pair_t_cmp(const void *a, const void *b){
        const close_pair_t *ia = *(const close_pair_t **)a;
        const close_pair_t *ib = *(const close_pair_t **)b;
        return(strcmp(ia->id,ib->id));
}



int binary_search_pairID_index(char *pair_id)
{
    if(strcmp(pair_id, close_pair[0]->id) < 0 || strcmp(pair_id, close_pair[n_pair-1]->id) > 0)return -1;
    else {
    int tmp, i;
    int from, to, new;
    char *str;
    from=0; to=n_pair-1;
    while(1){
        new = (int)(from+to)/2;
        str = close_pair[new]->id;
        tmp = strcmp(pair_id, str);
        if(tmp == 0) return new;
        else if(tmp > 0)from = new;
        else to = new;
        if(to - from < 3){
        for(i = from; i <= to; i++){
            if(strcmp(pair_id, close_pair[i]->id)==0)return i;
        }
        break;
        }
    }
    return -1;
    }
}



int binary_search_geo_index(char *place, Geo_t **geo_place, int len_geo)
{
    if(strcmp(place, geo_place[0]->name) < 0 || strcmp(place, geo_place[len_geo-1]->name) > 0)return -1;
    else {
    int tmp, i;
    int from, to, new;
    char *str;
    from=0; to=len_geo-1;
    while(1){
        new = (int)(from+to)/2;
        str = geo_place[new]->name;
        tmp = strcmp(place, str);
        if(tmp == 0) return new;
        else if(tmp > 0)from = new;
        else to = new;
        if(to - from < 3){
        for(i = from; i <= to; i++){
            if(strcmp(place, geo_place[i]->name)==0)return i;
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

void p_std_time(double seconds)
{
    int hour, min;
    double sec;
    hour = (int) (seconds / 3600);
    min = (int) ((seconds - hour * 3600)/ 60);
    sec = seconds - hour * 3600 - min * 60;
    if(hour > 0)sprintf(timestr,"%dh%dm%.3fs", hour, min, sec);
    else if (min > 0)sprintf(timestr,"%dm%.3fs", min, sec);
    else sprintf(timestr,"%.3fs", sec);
    return ;
}

void print_help(void)
{
    fprintf(stderr, "Usage: IBDstat [options] parameters\n\n \
    (Required inputs:)\n \
    \t--ibdsum [file]\n\t\t#<string> a list of input IBD pathnames\n \
    \t--ind [file]\n\t\t#<string> a list individuals to be analyzed\n \
    (Optional parameters:)\n \
    \t--out ./\n\t\t#<string> output prefix\n \
    \t--nthreads 2\n\t\t#<int> number of threads\n \
    \t--country --region --county --council\n \
    (Other flags:)\n \
    \t--outmask\n\t\t#output genome mask\n \
    \t--outcoverage\n\t\t#output IBD coverage\n \
    \n");
    exit(-1);
}

double my_wallclock(void)
{
    double T;
    struct timeval timecheck;

    gettimeofday(&timecheck, NULL);
    T = timecheck.tv_sec  + (double)timecheck.tv_usec / 1000000;
    return T;
}

void free_all(void){
	int i,j;
	free(headfile);
	free(idfile);
	free(outfile);
	for(i = 0; i < idNum; i ++ ){
	free(idhead[i]->id);
	free(idhead[i]->council);
	free(idhead[i]->county);
	free(idhead[i]->region);
	free(idhead[i]->country);
	free(idhead[i]);
    
    }
    if(count_country){
    	for(i=0;i<index_country;i++){
    	free(geo_country[i]->name);
    	free(geo_country[i]);
    	}
        for(i=0;i<max_g;i++){
            for(j=0;j<index_country;j++){    
                free(country_pair[i][j]);
            }
            free(country_pair[i]);
        }
        free(country_pair);
    }

    if(count_region){
    	for(i=0;i<index_region;i++){
    	free(geo_region[i]->name);
    	free(geo_region[i]);
    	}

        for(i=0;i<max_g;i++){
            for(j=0;j<index_region;j++){    
                free(region_pair[i][j]);
            }
            free(region_pair[i]);
        }
        free(region_pair);

    }

       // for(i=0;i<index_pair_region;i++){
       //     free(region_pair[i]->geo1);
       //     free(region_pair[i]->geo2);
       //     free(region_pair[i]);
       // }
       // free(region_pair);


   
    if(count_county){
    	for(i=0;i<index_county;i++){
    	free(geo_county[i]->name);
    	free(geo_county[i]);
    	}

        for(i=0;i<max_g;i++){
            for(j=0;j<index_county;j++){    
                free(county_pair[i][j]);
            }
            free(county_pair[i]);
        }
        free(county_pair);



    }
    if(count_council){
    	for(i=0;i<index_council;i++){
    	free(geo_council[i]->name);
    	free(geo_council[i]);
    	}

        for(i=0;i<max_g;i++){
            for(j=0;j<index_council;j++){    
                free(council_pair[i][j]);
            }
            free(council_pair[i]);
        }
        free(council_pair);

    }

    for(i=0;i<n_pair;i++){
        free(close_pair[i]->id);
        free(close_pair[i]);
    }

    free(close_pair);
    free(idhead);
    free(geo_country);
    free(geo_region);
    free(geo_county);
    free(geo_council);

    free(country_file);
    free(region_file);
    free(county_file);
    free(council_file);
    



}