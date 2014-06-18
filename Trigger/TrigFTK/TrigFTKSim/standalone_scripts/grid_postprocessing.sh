#!/bin/bash

# this script downloads a grid dataset and runs trackmerger + analysis over it.
# only argument: name of dataset

scriptdir=/home/antonk/TrigFTKSim/scripts
rundir=/scratch/$USER
if [ "$#" -lt "1" ]; then
    echo -n "Usage:   "
    echo "$0 Name.Of.Grid.Dataset"
    exit 1;
fi;
sample=$1

runstring=`echo $sample | awk 'BEGIN{FS="."};{print $3}'`
filelabel=`echo $runstring | awk 'BEGIN{FS="_"};{for(i=1;i<=5;i++) $i=""; print $0}' | sed -e 's/^[ \t]*//' -e 's# #_#g'`
echo "Processing: ${sample}"
n=`dq2-ls -L UCT3 -n ${sample} | cut -f 1`
if [ "$n" -eq "16" ]; then
    cd ${rundir}
    dq2-get -L UCT3 ${sample}
    cd ${sample}
    for f in *.tbz2*; do tar xfj $f; done;
    cd ${scriptdir}
    ./ftk.py -o FILENAME_NOSTRIP -r ${rundir}/${sample}/output/ -t /share/ftkdata/jtuggle/bases/hugeBank/ -n25 -d ${runstring} -I /share/ftkdata/boveia/wrapper/cscwhbb1034/20090625/split/${filelabel}.dat -a trackmerger,anaone -q silent
else
    echo "Wrong number of files in the dataset: $n"
fi;
