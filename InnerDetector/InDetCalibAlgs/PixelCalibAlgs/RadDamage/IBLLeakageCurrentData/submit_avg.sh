#!/bin/bash

marray=`cat modules.txt`

#m=LI_S01_A_M2
#bsub -C 0 -q 1nh -o ${m}.log -e ${m}.err -R 'pool>4000' run_avg.sh $m

for m in $marray
do
    echo $m

    subfile=means_HVON/$m.sub
    rm $subfile

    echo "executable            = run_avg.sh" >> $subfile
    echo "arguments             = $m" >> $subfile
    echo "output                = means_HVON/run_avg_$m.out" >> $subfile
    echo "error                 = means_HVON/run_avg_$m.err" >> $subfile
    echo "log                   = means_HVON/run_avg_$m.log" >> $subfile
    echo "queue" >> $subfile

    condor_submit $subfile

#    bsub -C 0 -q 8nh -o ${m}.log -e ${m}.err -R 'pool>4000' run_avg.sh $m
#    ./run_avg.sh $m

done

