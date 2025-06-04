#include<stdio.h>
#include<stdlib.h>
#include "head.h"
#include "tool.h"
#include "read.h"
#include<math.h>

int check_input(int arg, char**argv)
{
    fprintf(stderr, "############################\n");
    fprintf(stderr, "#Welcome to IBDstat V1#\n");
    fprintf(stderr, "############################\n");
    fprintf(stderr, "Copyright (C) 2021 Xiaolei Zhang xiaolei@ebi.ac.uk\n\n");
    //fprintf(stderr, "zlib verison: %s\n\n", ZLIB_VERSION);
    int i, fsize;
    
    check_genomesize=0;
    count_country=0;
    count_region=0;
    count_county=0;
    count_council=0;
    count_segnum=0;
    remove_close=0;
    

    country_file = (char *) calloc (16, sizeof(char));
    region_file = (char *) calloc (16, sizeof(char));
    county_file = (char *) calloc (16, sizeof(char));
    council_file = (char *) calloc (16, sizeof(char));
    
    memSize += 3* 16 * sizeof(char);
    //strcpy(country_file, "pair_count");  //copying kinship.gz to str outfile as output file name

    if(arg <=1)print_help(); 
    // the first argument is the name of the program
    for(i = 1; i< arg; i++){
    if(strcmp(argv[i], "--ibdsum")==0){  //if the two strings are equal
        fsize = strlen(argv[i+1])+10; 
        headfile = (char *) calloc (fsize, sizeof(char));    //allocate memory to the string
        memSize += (fsize) * sizeof(char); 
        strcpy(headfile, argv[i+1]); i = i + 1;
        fprintf(stderr,"\treading %s\n", headfile);
    }
    else if(strcmp(argv[i], "--ind")==0){
        fsize = strlen(argv[i+1])+10;
        idfile = (char *) calloc (fsize, sizeof(char));
        memSize += (fsize) * sizeof(char);
        strcpy(idfile, argv[i+1]); 
        i = i + 1;
    }else if(strcmp(argv[i], "--close_pair")==0){
        remove_close=1;
        fsize = strlen(argv[i+1])+10;
        kinfile = (char *) calloc (fsize, sizeof(char));
        memSize += (fsize) * sizeof(char);
        strcpy(kinfile, argv[i+1]); 
        i = i + 1;
    }else if(strcmp(argv[i], "--count_segnum")==0){
        count_segnum=1;
        }
    else if(strcmp(argv[i], "--count_region")==0){
        count_region=1;
        }
    else if(strcmp(argv[i], "--count_country")==0){
        count_country=1;
        }
    else if(strcmp(argv[i], "--count_council")==0){
        count_council=1;
        }
    else if(strcmp(argv[i], "--count_county")==0){
        count_county=1;
        }
    else if(strcmp(argv[i], "--out")==0){
        fsize = strlen(argv[i+1])+20;
            country_file = realloc (country_file, fsize*sizeof(char));
            region_file = realloc (region_file, fsize*sizeof(char));
            county_file = realloc (county_file, fsize*sizeof(char));
            council_file = realloc (council_file, fsize*sizeof(char));
            //maskfile = realloc (maskfile, fsize*sizeof(char));
            //coveragefile = realloc (coveragefile, fsize*sizeof(char));
            memSize += 3* (fsize - 16) * sizeof(char);
            sprintf(country_file, "%s.pair_country.gz", argv[i+1]); 
            sprintf(region_file, "%s.pair_region.gz", argv[i+1]); 
            sprintf(county_file, "%s.pair_county.gz", argv[i+1]); 
            sprintf(council_file, "%s.pair_council.gz", argv[i+1]); 
            //sprintf(maskfile, "%s.mask.gz", argv[i+1]); 
            //sprintf(coveragefile, "%s.coverage.gz", argv[i+1]); 
        i = i + 1;
        }
    else {
        print_help();
    }
    }
    return 0;
}



void count_country_ind(char *country){
    
    int isUnique=1; 
    int j;
    isUnique=1;
    for(j=0;j<index_country&&isUnique;j++){
        if(strcmp(unique_country[j],country)==0){
            //they are same
            n_country[j]+=1;
            isUnique=0;
        }
    }

    if(isUnique){

        //set up a new structure variable
        strcpy(unique_country[index_country],country);
        n_country[index_country]+=1;
        index_country++;

    }

    return;

}

void count_region_ind(char *region){
    
    int isUnique=1; 
    int j;
    isUnique=1;
    for(j=0;j<=index_region&&isUnique;j++){
        if(strcmp(unique_region[j],region)==0){
            //they are same
            n_region[j]+=1;
            isUnique=0;
        }
    }

    if(isUnique){
        strcpy(unique_region[index_region],region);
        n_region[index_region]+=1;
        index_region++;
    }

    return;

}

void count_county_ind(char *county){
    
    int isUnique=1; 
    int j;
    isUnique=1;
    for(j=0;j<=index_county&&isUnique;j++){
        if(strcmp(unique_county[j],county)==0){
            //they are same
            n_county[j]+=1;
            isUnique=0;
        }
    }

    if(isUnique){
        strcpy(unique_county[index_county],county);
        n_county[index_county]+=1;
        index_county++;
    }

    return;

}

void count_council_ind(char *council){
    
    int isUnique=1; 
    int j;
    isUnique=1;
    for(j=0;j<=index_council&&isUnique;j++){
        if(strcmp(unique_council[j],council)==0){
            //they are same
            n_council[j]+=1;
            isUnique=0;
        }
    }

    if(isUnique){
        strcpy(unique_council[index_council],council);
        n_council[index_council]+=1;
        index_council++;
    }

    return;

}




void read_ind(char *idfile)
{
    double a = my_wallclock();
    gzFile fp;
    int i;

    char str[BUFF];
    
    //init definition to count the number of samples
    index_country=0;
    index_region=0;
    index_county=0;
    index_council=0;
    

    fp = gzopen(idfile, "r"); idNum = 0;
    //first open the file to get the number
    //skip the header 
    gzgets(fp, str, BUFF-1);
    while(gzgets(fp, str, BUFF-1)!=NULL)idNum ++;
    gzclose(fp);
    
    
    idhead = (ID_t **)calloc(idNum, sizeof(ID_t*));

    //lock = (char *)calloc(idNum, sizeof(char));
    

    memSize += idNum * (sizeof(ID_t*) + sizeof(char)) + sizeof(ID_t**);
    //second-time open the file to construct individual information
    fp = gzopen(idfile, "r"); i = 0;
    //skip the first line;
    gzgets(fp, str, BUFF-1);
    while(gzgets(fp, str, BUFF-1)!=NULL){

    idhead[i] = (ID_t *)calloc(1, sizeof(ID_t));
	idhead[i]->id = (char *)calloc(strlen(str)+1,sizeof(char));

    idhead[i]->council = (char *)calloc(strlen(str)+1,sizeof(char));
    idhead[i]->county= (char *)calloc(strlen(str)+1,sizeof(char));
    idhead[i]->region = (char *)calloc(strlen(str)+1,sizeof(char));
    idhead[i]->country = (char *)calloc(strlen(str)+1,sizeof(char));
	memSize += sizeof(ID_t) + 5*strlen(str)*sizeof(char) + 2 * sizeof(float);
    // Note: I can also change here if I only want to calculate certain areas
    sscanf(str, "%[^\t]\t%[^\t]\t%[^\t]\t%[^\t]\t%[^\n]\n", idhead[i]->id,idhead[i]->country,idhead[i]->region,idhead[i]->county,idhead[i]->council);

    
    //add to the counts of different areas
    count_country_ind(idhead[i]->country);
    count_region_ind(idhead[i]->region);
    count_county_ind(idhead[i]->county);
    count_council_ind(idhead[i]->council);
    i++;


    }
    gzclose(fp);
    
    
    qsort(idhead, idNum, sizeof(ID_t *), ID_t_cmp); //qsort

    for(i = 0; i < (idNum-1); i++){

	if(strcmp(idhead[i]->id, idhead[i+1]->id)==0){
	    fprintf(stderr, "duplicated individuals: %s\n", idhead[i]->id);
	    exit(-1);
	}
    }

    fprintf(stderr, "%d individuals\n", idNum);
    p_std_time(my_wallclock() - a);
    fprintf(stderr,"read_ind() Time = %s\n", timestr);
    

    

    //put the country counts into structure
    geo_country = (Geo_t **)calloc(index_country, sizeof(Geo_t*));
    for(i=0;i<index_country;i++){
        geo_country[i] = (Geo_t *)calloc(1, sizeof(Geo_t));
        geo_country[i]->name=(char *)calloc(MAX_PLACE_NAME,sizeof(char));
        strcpy(geo_country[i]->name,unique_country[i]);
        geo_country[i]->count=n_country[i];
    }

    geo_region = (Geo_t **)calloc(index_region, sizeof(Geo_t*));
    for(i=0;i<index_region;i++){
        geo_region[i] = (Geo_t *)calloc(1, sizeof(Geo_t));
        geo_region[i]->name=(char *)calloc(MAX_PLACE_NAME,sizeof(char));
        strcpy(geo_region[i]->name,unique_region[i]);
        geo_region[i]->count=n_region[i];
    }

    geo_county = (Geo_t **)calloc(index_county, sizeof(Geo_t*));
    for(i=0;i<index_county;i++){
        geo_county[i] = (Geo_t *)calloc(1, sizeof(Geo_t));
        geo_county[i]->name=(char *)calloc(MAX_PLACE_NAME,sizeof(char));
        strcpy(geo_county[i]->name,unique_county[i]);
        geo_county[i]->count=n_county[i];
    }

    geo_council = (Geo_t **)calloc(index_council, sizeof(Geo_t*));
    for(i=0;i<index_council;i++){
        geo_council[i] = (Geo_t *)calloc(1, sizeof(Geo_t));
        geo_council[i]->name=(char *)calloc(MAX_PLACE_NAME,sizeof(char));
        strcpy(geo_council[i]->name,unique_council[i]);
        geo_council[i]->count=n_council[i];
    }

    qsort(geo_country,index_country,sizeof(Geo_t *),Geo_t_cmp);
    qsort(geo_region,index_region,sizeof(Geo_t *),Geo_t_cmp);
    qsort(geo_county,index_county,sizeof(Geo_t *),Geo_t_cmp);
    qsort(geo_council,index_council,sizeof(Geo_t *),Geo_t_cmp);
    fprintf(stderr,"------------------number of countries:%d --------------\n",index_country);
    for (i=0;i<index_country;i++){
    fprintf(stderr,"%s:%lld\n",geo_country[i]->name,geo_country[i]->count);
    }

    fprintf(stderr,"------------------number of regions:%d --------------\n",index_region);
    for (i=0;i<index_region;i++){
    fprintf(stderr,"%s:%lld\n",geo_region[i]->name,geo_region[i]->count);
    }

    fprintf(stderr,"------------------number of counties:%d --------------\n",index_county);

    for (i=0;i<index_county;i++){
    fprintf(stderr,"%s:%lld\n",geo_county[i]->name,geo_county[i]->count);
    }

    fprintf(stderr,"------------------number of councils:%d --------------\n",index_council);
    for (i=0;i<index_council;i++){
    fprintf(stderr,"%s:%lld\n",geo_council[i]->name,geo_council[i]->count);
    }
    


    return ;
}



void count_pair_3D(int id1,int id2, int totg){
    


    
    int i,j,k;
    int geo1, geo2;
    //Country pairs: find the index 
    //find the index for country 1 and country 2

    if(count_country){

        //find the index 
        i=totg;
        j=binary_search_geo_index(idhead[id1]->country,geo_country,index_country);
        k=binary_search_geo_index(idhead[id2]->country,geo_country,index_country);
        
        //we only count one diagnoal 
        if(j<=k){
            geo1=j;
            geo2=k;
        }else{
            geo2=j;
            geo1=k;
        }
        country_pair[i][geo1][geo2]+=1;
    }


     if(count_region){

        //find the index 
        i=totg;
        j=binary_search_geo_index(idhead[id1]->region,geo_region,index_region);
        k=binary_search_geo_index(idhead[id2]->region,geo_region,index_region);
        //we only count one diagnoal 
        if(j<=k){
            geo1=j;
            geo2=k;
        }else{
            geo2=j;
            geo1=k;
        }
        region_pair[i][geo1][geo2]+=1;
    }

    if(count_county){

        //find the index 
        i=totg;
        j=binary_search_geo_index(idhead[id1]->county,geo_county,index_county);
        k=binary_search_geo_index(idhead[id2]->county,geo_county,index_county);
        //we only count one diagnoal 
        if(j<=k){
            geo1=j;
            geo2=k;
        }else{
            geo2=j;
            geo1=k;
        }
        county_pair[i][geo1][geo2]+=1;
    }

    if(count_council){

        //find the index 
        i=totg;
        j=binary_search_geo_index(idhead[id1]->council,geo_council,index_council);
        k=binary_search_geo_index(idhead[id2]->council,geo_council,index_council);
        //we only count one diagnoal 
        if(j<=k){
            geo1=j;
            geo2=k;
        }else{
            geo2=j;
            geo1=k;
        }
        council_pair[i][geo1][geo2]+=1;
    }



    

    return;
}



void read_closeid(char *kinfile){
    double a = my_wallclock();

    gzFile fp;

    char str[BUFF];
    char str1[BUFF];
    char str2[BUFF];
    int id1,id2;
    
    int i;
    n_pair=0;

    fp = gzopen(kinfile, "r");

    gzgets(fp,str,BUFF-1);
    while(gzgets(fp, str, BUFF-1)!=NULL){

        sscanf(str,"%[^\t]\t%[^\t]\t",str1,str2);
        id1 = binary_search_ID_index(str1);
        id2 = binary_search_ID_index(str2);
        if(id1 == -1 || id2 == -1){
        continue;
        }

        n_pair ++;
        }
    gzclose(fp);

    close_pair = (close_pair_t **)calloc(n_pair,sizeof(close_pair_t*));

    fp = gzopen(kinfile, "r");
    //skip the first line;
    gzgets(fp, str, BUFF-1);
    //n_pair=0;
    i=0;
    while(gzgets(fp, str, BUFF-1)!=NULL){

        sscanf(str,"%[^\t]\t%[^\t]\t",str1,str2);
        
        id1 = binary_search_ID_index(str1);
        id2 = binary_search_ID_index(str2);
        if(id1 == -1 || id2 == -1){
        continue;
        }

        close_pair[i]=(close_pair_t *)calloc(1,sizeof(close_pair_t));
        close_pair[i]->id = (char *)calloc(strlen(str1)+strlen(str2)+5,sizeof(char));

        if(id1<id2){
            sprintf(close_pair[i]->id, "%s-%s", str1,str2);
        }else{
            sprintf(close_pair[i]->id, "%s-%s", str2,str1);
        }

        i++;

    }

    qsort(close_pair,n_pair,sizeof(close_pair_t *),close_pair_t_cmp);
    fprintf(stderr,"number of pairs:%d\n",n_pair);

    p_std_time(my_wallclock() - a);
    fprintf(stderr,"read kinfile Time = %s\n", timestr);
    gzclose(fp);

    return;




}


void read_ibdsum(char *headfile){
    //read line by line and then check the pairs and count++
    double a = my_wallclock();
    gzFile fp;
    long long n;  //number of lines read counter
    char str[BUFF];
    char str1[BUFF];
    char str2[BUFF];
    char tmp_str[BUFF];
    int id1,id2,tmp_id;
    int segnum;
    float g1,g2,total;
    int totg;
    int i,j,k;
    //skip the first line;
    char pair_id[BUFF*2+5];
    int label;


    int n_country_pair=(index_country)*(index_country);
    int n_region_pair=(index_region)*(index_region);
    int n_county_pair=(index_county)*(index_county);
    int n_council_pair=(index_council)*(index_council);

    
    country_pair = (long long ***)malloc(max_g*sizeof(long long **));
    
    for(i=0;i<max_g;i++){
        country_pair[i]=(long long **)malloc(n_country_pair*sizeof(long long **));
        for(j=0;j<index_country;j++){
            country_pair[i][j]=(long long *)malloc(index_country*sizeof(long long *));
            for(k=0;k<index_country;k++){
                //TODO: assign memory for each element
                country_pair[i][j][k]=0;
            }
        }
    }
    

    region_pair = (long long ***)malloc(max_g*sizeof(long long **));
    for(i=0;i<max_g;i++){
        region_pair[i]=(long long **)malloc(n_region_pair*sizeof(long long **));
        for(j=0;j<index_region;j++){
            region_pair[i][j]=(long long *)malloc(index_region*sizeof(long long *));
            for(k=0;k<index_region;k++){
                //TODO: assign memory for each element
                region_pair[i][j][k]=0;
            }
        }
    }

    county_pair = (long long ***)malloc(max_g*sizeof(long long **));
    for(i=0;i<max_g;i++){
        county_pair[i]=(long long **)malloc(n_county_pair*sizeof(long long **));
        for(j=0;j<index_county;j++){
            county_pair[i][j]=(long long *)malloc(index_county*sizeof(long long *));
            for(k=0;k<index_county;k++){
                //TODO: assign memory for each element
                county_pair[i][j][k]=0;
            }
        }
    }

    council_pair = (long long ***)malloc(max_g*sizeof(long long **));
    for(i=0;i<max_g;i++){
        council_pair[i]=(long long **)malloc(n_council_pair*sizeof(long long **));
        for(j=0;j<index_council;j++){
            council_pair[i][j]=(long long *)malloc(index_council*sizeof(long long *));
            for(k=0;k<index_council;k++){
                //TODO: assign memory for each element
                council_pair[i][j][k]=0;
            }
        }
    }



    fp = gzopen(headfile, "r"); n=0;
    fprintf(stderr,"Opening ibdsum file\n");
    gzgets(fp, str, BUFF-1);
    while(gzgets(fp, str, BUFF-1)!=NULL){
        n++;
        
        if(n%1000000==0){
            fprintf(stderr,"Finish %lld lines\n",n);
        }

        sscanf(str, "%[^\t]\t%[^\t]\t%d\t%f\t%f\t%f\n", str1,str2,&segnum,&g1,&g2,&total);
        
        if(check_genomesize==1){    
        if((g1>max_g)||(g1<0)){
            fprintf(stderr,"g1: out of possible max genomic size:line %lld -- %s\t%s\t%f\n",n,str1,str2,g1);
            exit(-1);
        }
        if((g2>max_g)||(g2<0)){
            fprintf(stderr,"g2: out of possible max genomic size:line %lld -- %s\t%s\t%f\n",n,str1,str2,g2);
            exit(-1);
        }
        }
        id1 = binary_search_ID_index(str1);
        id2 = binary_search_ID_index(str2);
        
        //skip the case of HBD
        if(id1==id2){
            continue;
        }

        if(strcmp(str1,str2)==0){
            continue;
        }

        if(id1>id2){
            tmp_id=id1;
            id1=id2;
            id2=tmp_id;
            strcpy(tmp_str,str1);
            strcpy(str1,str2);
            strcpy(str2,tmp_str);
        }
        //if the id is not in the sample info file
        if(id1 == -1 || id2 == -1){
        continue;
        }
        totg=(int)floor(total);
        if(totg>=max_g||totg==0){
            continue;
        }

        
        if(id1<id2){
            sprintf(pair_id,"%s-%s",str1,str2);
        }
        else{
            sprintf(pair_id,"%s-%s",str2,str1);
        }
        
        if(remove_close){
            
            label=binary_search_pairID_index(pair_id);
            if(label!=-1){
                fprintf(stderr,"skip relatives : %s\n",pair_id);
                fprintf(stderr,"totg: %d\n",totg);
                continue;
            }
            

        };

        if(count_segnum==0){
        
        count_pair_3D(id1,id2,totg);}
        else{
            count_pair_3D(id1,id2,segnum);
        }

        
    }


    p_std_time(my_wallclock() - a);
    fprintf(stderr,"total number of lines = %lld\n",n);
    fprintf(stderr,"read_ibdsum file Time = %s\n", timestr);
    gzclose(fp);

    return;
}


void write_output(void){
    
    gzFile country_fp=gzopen(country_file,"w");
    gzFile region_fp=gzopen(region_file,"w");
    gzFile county_fp=gzopen(county_file,"w");
    gzFile council_fp=gzopen(council_file,"w");
    char str[BUFF];
    int i,j,k;

    fprintf(stderr,"writing output - country\n");
    for(i=0;i<max_g;i++){
        for(j=0;j<index_country;j++){
            for(k=0;k<index_country;k++){
                if(country_pair[i][j][k]!=0){
                    sprintf(str,"%s\t%s\t%d\t%lld \n", geo_country[j]->name, geo_country[k]->name,i,country_pair[i][j][k]);
                    gzputs(country_fp,str);
                }
            }
        }
    }

    fprintf(stderr,"writing output - region\n");
    for(i=0;i<max_g;i++){
        for(j=0;j<index_region;j++){
            for(k=0;k<index_region;k++){
                if(region_pair[i][j][k]!=0){
                    sprintf(str,"%s\t%s\t%d\t%lld \n", geo_region[j]->name, geo_region[k]->name,i,region_pair[i][j][k]);
                    gzputs(region_fp,str);
                }
            }
        }
    }

    fprintf(stderr,"writing output - county\n");
    for(i=0;i<max_g;i++){
        for(j=0;j<index_county;j++){
            for(k=0;k<index_county;k++){
                if(county_pair[i][j][k]!=0){
                    sprintf(str,"%s\t%s\t%d\t%lld \n", geo_county[j]->name, geo_county[k]->name,i,county_pair[i][j][k]);
                    gzputs(county_fp,str);
                }
            }
        }
    }

    fprintf(stderr,"writing output - council\n");
    for(i=0;i<max_g;i++){
        for(j=0;j<index_council;j++){
            for(k=0;k<index_council;k++){
                if(council_pair[i][j][k]!=0){
                    sprintf(str,"%s\t%s\t%d\t%lld \n", geo_council[j]->name, geo_council[k]->name,i,council_pair[i][j][k]);
                    gzputs(council_fp,str);
                }
            }
        }
    }  
    
   gzclose(country_fp);
   gzclose(region_fp);
   gzclose(county_fp);
   gzclose(council_fp);
   


}

