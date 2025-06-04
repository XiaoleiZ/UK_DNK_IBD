#include "head.h"
#include "tool.h"
#include "read.h"
#include <stdio.h>
#include <stdlib.h>
#include <zlib.h>
#include <time.h>



int main(int argc,char **argv){

	check_input(argc,argv);
	
	//six input files
	//file1: the total IBD sharing for all pairs in UKBB
	//file2: the birth place at individual-level 
	//file3: the country we want to analyse (optionl)
 	//file4: the region we want to analyse (optional)
	//file5: the county we want to analyse (optionl)
	//file6: the council we want to analyse (optionl)

	read_ind(idfile);
	read_closeid(kinfile);
	read_ibdsum(headfile);
	write_output();
	
	free_all();
	return 0;
	
	





}
