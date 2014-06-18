#!/bin/bash

if [ $# -lt 1 -o $# -gt 2 ]; then
    echo "Usage: $0 dataset [outdir]"
    exit -1
fi

STARTDIR=$PWD

# check and create the proxy
voms-proxy-info -exists || voms-proxy-init -voms atlas

outputDS=$1
if [ $# -eq 1 ]; then
    outputDir=$1
else
    outputDir=$2
fi

datasets=$(dq2-list-datasets-container $outputDS)

# prepare and move in the output directory
mkdir -p $outputDir
cd $outputDir

# download the dataset container
dq2-get $outputDS

echo datasets = $datasets

# loop over the directories with the single datasets
for d in $datasets; do
    # loop over the output files and decompress them in the output directory
    for f in ${d}/*.tbz2*; do
	echo untarring $f
	tar xfj $f
    done
done

#move back to original directory
cd $STARTDIR
