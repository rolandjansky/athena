#!/bin/bash

#label will be jordanv16
indir=$1  # BEAM
nreg=$2
dataset=$3
#export LD_PRELOAD=/opt/d-cache/dcap/lib/libpdcap.so
regs=`echo {0..$nreg}`

OUTDIR=$PWD
for i in {0..63}; do    

    OUTFILE=inputprod_raw_${dataset}${i}.list
    echo $OUTFILE
    ls ${indir}/*${dataset}_reg${i}_*dat > $OUTFILE
done