#!/bin/bash
#SBATCH -J ibdkin_chr1
#SBATCH --nodes=1
#SBATCH --mem=50G
#SBATCH -t 200:00:00
date

chr=1
dir=/nfs/research/birney/users/xiaolei/IBD_github/scripts_to_share
testdir=$dir/IBDkin_fastsmc/test
cd $testdir

ibdkin=$dir/IBDkin_fastsmc/IBDkin_fastsmc
map=$dir/help_files/genetic_map/GRCh37/plink.chr1.GRCh37.map

ind=$testdir/sample_id
ibdfile=$testdir/sample_ibdfile

range=$dir/help_files/range/GRCh37/chr${chr}.range.txt
n_thread=10
ibd_prob=0.5
low_ibd=2
upper_ibd=100

$ibdkin --ibdfile ${ibdfile} --map ${map} --ind ${ind} --range ${range} --nthreads ${n_thread} --out $testdir/chr${chr} --cutcm ${low_ibd} ${upper_ibd} --cutprob ${ibd_prob} --remove_overlap 1
date
