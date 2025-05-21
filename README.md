# UK_DNK_IBD

Programs used to analyse biobank-scale IBD sharing data

## IBDkin - get individual pairwise total IBD sharing and total number of IBD segments

We updated the published program IBDkin (https://doi.org/10.1093/bioinformatics/btaa569) to take IBD calling results from FastSMC as input and added customised processing options. 

To run IBDkin, run the command line with the following options:

`/pathto/IBDkin --ibdfile ${ibdfile} --map ${map} --ind ${ind} --range ${range} --nthreads ${n_thread} --out $output_file --outmask --outcoverage --cutcm 2 100 --cutprob 0.5 --remove_overlap 1`

The input options include: 
--ibdfile [string] - filename of a list of file paths of FastSMC IBD calling output (one file path per line)

--map [string] - filename of genetic map in PLINK format

--ind [string] - filename of a list of sample IDs included in the calculation (one ID per line)

--range [string]-  filename of a list of genomic regions considered for each chromosome, including three columns: the chromosome identifier, starting bp position, and ending bp position. In our case, we used the range covered by the genetic maps. 

--nthreads [int] - number of threads processing at the same time

--cutcm [float] [float] only include IBD segments within the input range; first value is lower bound and the second one for the upper bound 

--cutprob [float] only include IBD segments with the predictive probability (from FastSMC) above the threshold 

--remove_overlap [boolean] 1 if only to keep the longest IBD segment from the same pair if there are multiple and overlapping with each other otherwise 0.     


The output has the following columns (each column is delimited by a tab): 

`ID1  ID2  segnum  IBD1  IBD2  totg`

Each column has the following information: 

ID1 - individual ID for individual 1

ID2 - individual ID for individual 2

segnum - total number of IBD segments

IBD1 - total sharing of IBD1 (cM) (only one pair of haplotypes share IBD)  

IBD2 - total sharing of IBD2 (cM) (two pairs of haplotypes share IBD)

totg - total IBD sharing (cM), which is calculated as IBD1+2*IBD2

For cohorts with a biobank-scale sample size (N>500K), to speed up the process, one can run this program in parallel for each chromosome and then combine the computed results for all the chromosomes. Thus we also developed sumchr_IBDkin. 

## sumchr_IBDkin  - combine multiple outputs from IBDkin into one



## ibdstat - summarise the total IBD sharing and total number of IBD segments among pairs from geographic areas

The output format is the following (each column is delimited by tab): 

'area1\tarea2\ttotg\tcount'

area1 - name of Area 2
area2 - name of Area 2
totg - Total IBD sharing (cM)
count - total number of pairs with the corresponding total IBD sharing 




