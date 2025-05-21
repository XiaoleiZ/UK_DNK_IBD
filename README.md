# UK_DNK_IBD

Programs used to analyse biobank-scale IBD sharing data

## IBDkin - get individual pairwise total IBD sharing and total number of IBD segments

We updated the published program IBDkin (https://doi.org/10.1093/bioinformatics/btaa569) to take IBD calling results from FastSMC as input and added customised processing options. 

To run IBDkin, run the command line with the following options:

`/pathto/IBDkin --ibdfile ${ibdfile} --map ${map} --ind ${ind} --range ${range} --nthreads ${n_thread} --out $output_file --outmask --outcoverage --cutcm 2 100 --cutprob 0.5 --remove_overlap 1`

The input options include: 
--ibdfile - a list of file paths of FastSMC IBD calling output (one file path per line)

--map - genetic map in PLINK format

--ind - a list of sample IDs included in the calculation (one ID per line)

--range - genomic regions considered for each chromosome (genomic coordinates). In our case, we used the range covered by the genetic maps. 

--nthreads number of threads processing at the same time

--cutcm [lowibd] [upibd] only include IBD segments within the range [lowibd, upibd]

--cutprob [prob] only include IBD segments with >0.5 predictive probability (from FastSMC)

--remove_overlap [Boolean] keey only the longest IBD segment from the same pair if there are multiple and overlapping with each other.    






The output format is the following (each column is delimited by tab): 

`ID1\tID2\tsegnum\tIBD1\tIBD2\ttotg`

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




