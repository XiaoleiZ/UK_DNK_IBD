#include "head.h"
#include "read.h"
#include "tools.h"
#include "print.h"
#include "parallel.h"


int main(int argc, char **argv)
{
    memSize = 0;

    check_input(argc, argv);
    p_parameters(); 

    init();p_mem();

    p_mem();

    read_ind(idfile);
    p_mem();

    fprintf(stderr, "\n\nReading IBDkin_fastsmc results ...\n");

    read_ibd_1st_pass(); //first pass, index samples and save pairs

	fprintf(stderr, "\n\nCalculate ID pairs...\n");
	shrink_id_pair();

	fprintf(stderr, "\n\nSecond pass ...\n");
	read_ibd_2nd_pass(); 

    if(checkTime == 1)p_time();

    fprintf(stderr,"\n\nDone, Wallclock Time = %s\n", timestr);
    free_all();

    return 0;
}
