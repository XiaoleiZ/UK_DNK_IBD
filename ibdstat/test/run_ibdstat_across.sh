#!/bin/bash
#SBATCH -J "ibdstat"
#SBATCH --nodes=1
#SBATCH --ntasks-per-node=1
#SBATCH --mem=20G
#SBATCH -t 100:00:00

date
dir=/nfs/research/birney/users/xiaolei/IBD_github/scripts_to_share
testdir=$dir/ibdstat/test
ind=$testdir/sample_info
input=$testdir/chr1_chr2_totg.sum.gz
cd $testdir
ibdstat=$dir/ibdstat/ibdstat

$ibdstat --ibdsum ${input} --ind ${ind} --out $testdir/ibdstat --count_country --count_region --count_county --count_council
date
