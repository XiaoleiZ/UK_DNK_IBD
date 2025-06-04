#include "head.h"
#include "print.h"
#include "read.h"
#include "tools.h"

void print_help(void)
{
    fprintf(stderr, "Usage: IBDkin [options] parameters\n\n \
    (Required inputs:)\n \
    \t--ibdfile [file]\n\t\t#<string> a list of input IBD pathnames\n \
    \t--ind [file]\n\t\t#<string> a list individuals to be analyzed\n \
    \t--self\n\t\t#<string> option - only output pairs that are within the same country\n \
    \t--across\n\t\t#<string> option - only output pairs that are between two different countries\n \
    \t--out ./\n\t\t#<string> output prefix\n \
    \t--nthreads 2\n\t\t#<int> number of threads\n \
    \t--cutcm 4.0 2.0\n\t\t#<float> <float> minimum long and short IBD segment lengths in cM \n \
    \n");
    exit(-1);
}


void p_parameters(void)
{
    int i;
    fprintf(stderr, "###Parameters###\n");
    fprintf(stderr, "Input:\n");
    fprintf(stderr, " ibd: *%s*\n", headfile);
    read_headfile(headfile);
    fprintf(stderr, " ind: *%s*\n", idfile);

    fprintf(stderr, "\nOutputs:\n");
    if(tagKinship == 1){
	fprintf(stderr, "  *%s*\n", outfile);
	gzFile fp = gzopen(outfile, "w");
	gzbuffer(fp, BUFF3);
	gzprintf(fp, "ID1\tID2\tsegnum\tg1\tg2\ttotg\n");
	gzclose(fp);
    }


    fprintf(stderr, "\nOther settings:\n");
    fprintf(stderr, " *nthreads = %d*\n", Nthreads);

    threadCount = (long int *)calloc(Nthreads, sizeof(long int));
    memSizeByThread = (long int *)calloc(Nthreads, sizeof(long int));
    pairNumByThread = (long int *)calloc(Nthreads, sizeof(long int));
    segNumByThread = (long int *)calloc(Nthreads, sizeof(long int));
    outbuffi = (int *)calloc(Nthreads, sizeof(int));
    outbuff = (char **)calloc(Nthreads, sizeof(char *));

    memSize += 4 * Nthreads * sizeof(long int) + Nthreads * (sizeof(int) + sizeof(char *));

    for(i = 0; i< Nthreads; i++) {
	threadCount[i] = 0;
	memSizeByThread[i] = 0;
	pairNumByThread[i] = 0;
	segNumByThread[i] = 0;
	outbuffi[i] = 0;
	memSize += BUFF3 * sizeof(char);
    }

    

    fprintf(stderr, "#################\n\n");
}


void p_mem(void)
{
    if(checkMem != 1)return;
    p_maxrss();
    double mem;
    long int tmp;
    int i;
    tmp = memSize; 
    for(i = 0; i < Nthreads; i++ )tmp += memSizeByThread[i];
    mem = (double)tmp / 1073741824;
    if(mem > 1){fprintf(stderr, " *Memory in use: %.5lf Gigabytes\n\n", mem);return;}
    mem = (double)tmp / 1048576;
    if(mem > 1){fprintf(stderr, " *Memory in use: %.5lf Megabytes\n\n", mem);return;}
    mem = (double)tmp / 1024;
    if(mem > 1){fprintf(stderr, " *Memory in use: %.5lf Kilobytes\n\n", mem);return;}
    fprintf(stderr, " *Memory in use: %ld Bytes\n\n", tmp);

    return ;

}

void p_maxrss(void)
{
    struct rusage buf;
    getrusage(RUSAGE_SELF, &buf);
    double mem, tmp = buf.ru_maxrss;
    mem = (double)tmp / 1048576;
    if(mem > 1){fprintf(stderr, " *maxrss: %.5lf Gigabytes\n", mem);return;}
    mem = (double)tmp / 1024;
    if(mem > 1){fprintf(stderr, " *maxrss: %.5lf Megabytes\n", mem);return;}
    fprintf(stderr, " *maxrss: %.5lf Kilobytes\n", tmp);
    return ;
}


void p_time(void)
{
    fprintf(stderr,"\n function time:\n");
    p_std_time(t_gzread);
    fprintf(stderr," *gz_read() cpu=%.3fs, wall=%s, r=%.3f\n", Tgzread*1.0/CLOCKS_PER_SEC, timestr, Tgzread*1.0/CLOCKS_PER_SEC/t_gzread);
    p_std_time(t_fill_buff_ibd1);
    fprintf(stderr," *fill_buff_ibd1() cpu=%.3fs, wall=%s, r=%.3f\n", Tfill_buff_ibd1*1.0/CLOCKS_PER_SEC, timestr, Tfill_buff_ibd1*1.0/CLOCKS_PER_SEC/t_fill_buff_ibd1);
    if(tagKinship == 1){
	p_std_time(t_fill_buff_ibd2);
	fprintf(stderr," *fill_buff_ibd2() cpu=%.3fs, wall=%s, r=%.3f\n", Tfill_buff_ibd2*1.0/CLOCKS_PER_SEC, timestr, Tfill_buff_ibd2*1.0/CLOCKS_PER_SEC/t_fill_buff_ibd2);
    }
    if(tagKinship == 1){
	p_std_time(t_shrinke_id_pair);
	fprintf(stderr," *shrinke_id_pair() cpu=%.3fs, wall=%s, r=%.3f\n", Tshrinke_id_pair*1.0/CLOCKS_PER_SEC, timestr, Tshrinke_id_pair*1.0/CLOCKS_PER_SEC/t_shrinke_id_pair);
    }
    p_std_time(t_cal_pair_num);
    fprintf(stderr," *cal_pair_num() cpu=%.3fs, wall=%s, r=%.3f\n", Tcal_pair_num*1.0/CLOCKS_PER_SEC, timestr, Tcal_pair_num*1.0/CLOCKS_PER_SEC/t_cal_pair_num);

    p_std_time(t_store_buff_ibd);
    fprintf(stderr," *store_buff_ibd() cpu=%.3fs, wall=%s, r=%.3f\n", Tstore_buff_ibd*1.0/CLOCKS_PER_SEC, timestr, Tstore_buff_ibd*1.0/CLOCKS_PER_SEC/t_store_buff_ibd);
    if(tagKinship == 1){
	p_std_time(t_cal_kinship);
	fprintf(stderr," *cal_kinship() cpu=%.3fs, wall=%s, r=%.3f\n", Tcal_kinship*1.0/CLOCKS_PER_SEC, timestr, Tcal_kinship*1.0/CLOCKS_PER_SEC/t_cal_kinship);
    }
    p_std_time(t_gzputs);
    fprintf(stderr," *gz_write() cpu=%.3fs, wall=%s, r=%.3f\n", Tgzputs*1.0/CLOCKS_PER_SEC, timestr, Tgzputs*1.0/CLOCKS_PER_SEC/t_gzputs);
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
