
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
  --cutcm [min] [max] \
  --cutprob [max] \
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
| `--cutcm` [min] [max]         | `float float` | Minimum and maximum cM range for IBD segments                                  |
| `--cutprob` [prob]       | `float`       | Minimum predictive probability threshold for IBD segments (output of FastSMC)                      |
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

sumchr_IBDkin takes multiple text files output from IBDkin_fastsmc, and aggregate the IBD sharing infomation including total IBD sharing and total IBD segments for all pairs observed. 

---

### Usage

```bash
/pathto/sumchr_IBDkin \
  --ibdsum_file ${ibdsum} \
  --ind ${ind} \
  --nthreads ${n_thread} \
  --out ${output_file} \
  --self [Pop_name]
  --across [Pop1_name] [Pop2_name]
   \
  ```

Input Options

| Option             | Type          | Description                                                                    |
| ------------------ | ------------- | ------------------------------------------------------------------------------ |
| `--ibdsum_file`        | `string`      | File containing a list of paths to IBDkin_fastsmc output files (one per line)     |
| `--ind`            | `string`      | File with sample IDs and the sample population name to include. Format: ID\tPop\n. No header. One sample per line                              |
| `--nthreads`       | `int`         | Number of threads to use                                                       |
| `--self`   [Pop_name]     | `string`       | Include the intra-population sharing for Population [Pop_name]                       |
| `--across` [Pop1_name] [Pop2_name] | `string` `string`     | Include the inter-popualtion sharing between Population [Pop1_name] and [Pop2_name] |


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



## ibdstat - summarise the total IBD sharing and total number of IBD segments among pairs from geographic areas

---

### Usage

```bash
/pathto/ibdstat \
  --ibdsum ${ibdsum} \
  --ind ${ind} \
  --nthreads ${n_thread} \
  --out ${output_file} \
  --count_country
  --count_region 
  --count_county 
  --count_council
  --segnum
 \
  ```

Input Options

| Option             | Type          | Description                                                                    |
| ------------------ | ------------- | ------------------------------------------------------------------------------ |
| `--ibdsum`        | `string`      | Filename for pairwise IBD sharing summary (one pair per line), e.g. output by sumchr_IBDkin. Format follows the output of sumchr_IBDkin |
| `--ind`            | `string`      | File with sample IDs and the sample birthplace information on different levels of geographic divisions. Format: ID\tCountry\tRegion\tCounty\tCouncil. With Header. One sample per line                              |
| `--nthreads`       | `int`         | Number of threads to use                                                     |
| `--count_country`  | NA | Output pairwise statistics at country level |
| `--count_region`  | NA | Output pairwise statistics at region level |
| `--count_county`  | NA | Output pairwise statistics at county level |
| `--count_council`  | NA | Output pairwise statistics at council level |
| `--segnum`  | NA | Use this option to output the summary statistics of the total number of IBD segments. Without this option, will output summary statistics based on the total sum of IBD sharing lengths | 

Output format

The output format is the following (each column is delimited by tab): 


| Column   | Description                                                 |
| -------- | ----------------------------------------------------------- |
| `area1`    | name of Area 1                            |
| `area2`    | name of Area 2                              |
| `totg` | Total sum of IBD lengths                         |
| `count`   | number of pairs observed with the corresponding total IBD sharing   |
