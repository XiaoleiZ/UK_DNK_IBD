
# IBDbillion
 
C programs for efficient processing of billion-scale pairwise IBD sharing. 


## IBDkin – Calculate Pairwise Total IBD Sharing

We modified the published program [IBDkin](https://doi.org/10.1093/bioinformatics/btaa569) to accept IBD segment calls from **FastSMC** and added custom processing options.

---

### Usage

To run our version of the IBDkin, use the following command:

```bash
/pathto/IBDkin \
  --ibdfile ${ibdfile} \
  --map ${map} \
  --ind ${ind} \
  --range ${range} \
  --nthreads ${n_thread} \
  --out ${output_file} \
  --outmask \
  --outcoverage \
  --cutcm 2 100 \
  --cutprob 0.5 \
  --remove_overlap 1


Input Options

| Option             | Type          | Description                                                                    |
| ------------------ | ------------- | ------------------------------------------------------------------------------ |
| `--ibdfile`        | `string`      | File containing a list of paths to FastSMC IBD output files (one per line)     |
| `--map`            | `string`      | Genetic map file in PLINK format                                               |
| `--ind`            | `string`      | File with sample IDs to include (one ID per line)                              |
| `--range`          | `string`      | File with genomic regions included for each chromosome: <br>chromosome, start bp, end bp. We used the range covered by the genetic maps in our case.          |
| `--nthreads`       | `int`         | Number of threads to use                                                       |
| `--cutcm`          | `float float` | Minimum and maximum cM range for IBD segments                                  |
| `--cutprob`        | `float`       | Minimum predictive probability threshold for IBD segments (output of FastSMC)                      |
| `--remove_overlap` | `boolean`     | `1` to retain only the longest overlapping IBD segment per pair; <br> `0` otherwise |



The output has the following columns (a tab delimits each column): 

`ID1  ID2  segnum  IBD1  IBD2  totg`

Each column has the following information: 

ID1 - individual ID for individual 1

ID2 - individual ID for individual 2

segnum - total number of IBD segments

IBD1 - total sharing of IBD1 (cM) (only one pair of haplotypes shares IBD)  

IBD2 - total sharing of IBD2 (cM) (two pairs of haplotypes share IBD)

totg - total IBD sharing (cM), which is calculated as IBD1+2*IBD2

For cohorts with a biobank-scale sample size (N>500K), to speed up the process, one can run this program in parallel for each chromosome and then combine the computed results for all the chromosomes. Thus, we also developed **sumchr_IBDkin**. 

## sumchr_IBDkin  - combine multiple outputs from IBDkin into one



## ibdstat - summarise the total IBD sharing and total number of IBD segments among pairs from geographic areas

The output format is the following (each column is delimited by tab): 

'area1\tarea2\ttotg\tcount'

area1 - name of Area 2
area2 - name of Area 2
totg - Total IBD sharing (cM)
count - total number of pairs with the corresponding total IBD sharing 




