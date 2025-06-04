#!/bin/bash
#SBATCH -J sumchr
#SBATCH --nodes=1
#SBATCH --mem=50G
#SBATCH -t 200:00:00
date
dir=/nfs/research/birney/users/xiaolei/IBD_github/scripts_to_share/sumchr_ibdkin
testdir=$dir/test
cd $testdir

sumchr_ibdkin=$dir/sumchr_IBDkin
ind=$testdir/sample_id
ibdsum_file=$testdir/ibdsum_file
n_thread=10

$sumchr_ibdkin --ibdsum_file $ibdsum_file --ind $ind --self Pop1 --across Pop1 Pop2 --nthreads $n_thread --out $testdir/chr1_chr22
date
