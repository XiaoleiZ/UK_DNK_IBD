
# IBDbillion
 
C programs for efficient processing of billion-scale pairwise IBD sharing. 


## IBDkin_fastsmc – Calculate Pairwise Total IBD Sharing

We modified the published program [IBDkin](https://doi.org/10.1093/bioinformatics/btaa569) to accept IBD segment calls from **FastSMC** and added custom processing options.

---

### Usage

To run our version of the IBDkin, use the following command:

```bash
/pathto/IBDkin_fastsmc \
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
```

Input Options

| Option             | Type          | Description                                                                    |
| ------------------ | ------------- | ------------------------------------------------------------------------------ |
| `--ibdfile`        | `string`      | File containing a list of paths to FastSMC IBD output files (one per line)     |
| `--map`            | `string`      | Genetic map file in PLINK format                                               |
| `--ind`            | `string`      | File with sample IDs to include (one ID per line)                              |
| `--range`          | `string`      | File with genomic regions included for each chromosome: chromosome, start bp, end bp. <br>We used the range covered by the genetic maps in our case.          |
| `--nthreads`       | `int`         | Number of threads to use                                                       |
| `--cutcm`          | `float float` | Minimum and maximum cM range for IBD segments                                  |
| `--cutprob`        | `float`       | Minimum predictive probability threshold for IBD segments (output of FastSMC)                      |
| `--remove_overlap` | `boolean`     | `1` to retain only the longest overlapping IBD segment per pair; <br> `0` otherwise |


Output Format
The output is a tab-delimited file with the following columns: 

| Column   | Description                                                 |
| -------- | ----------------------------------------------------------- |
| `ID1`    | Individual ID for person 1                                  |
| `ID2`    | Individual ID for person 2                                  |
| `segnum` | Total number of IBD segments shared                         |
| `IBD1`   | Total IBD1 sharing (cM): one pair of haplotypes shares IBD  |
| `IBD2`   | Total IBD2 sharing (cM): both pairs of haplotypes share IBD |
| `totg`   | Total IBD sharing (cM), calculated as `IBD1 + 2 × IBD2`     |

Parallelisation

For cohorts with biobank-scale sample sizes (e.g., N ~ 500,000), we recommend running IBDkin separately for each chromosome to speed up computation and then combining results across all the chromosomes. 
To assist with this, we also developed a program called **sumchr_IBDkin** to combine the results across chromosomes efficiently.

## sumchr_IBDkin  - combine multiple outputs from IBDkin into one



## ibdstat - summarise the total IBD sharing and total number of IBD segments among pairs from geographic areas

The output format is the following (each column is delimited by tab): 

'area1\tarea2\ttotg\tcount'

area1 - name of Area 2
area2 - name of Area 2
totg - Total IBD sharing (cM)
count - total number of pairs with the corresponding total IBD sharing 




