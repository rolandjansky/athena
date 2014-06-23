#!/bin/bash

if [ $# -lt 3 ]
then
  echo "Usage example: `basename $0` test (test12p4) stream (\"19 24 45\") outset (3) "
  exit 65
fi

testn=$1
stream=$2
outset=$3

rm -f progress77.txt ; touch progress77.txt

for s in {0..9}; do echo "EXT${s}"
 for d in `dq2-list-datasets-container group.dataprep.data12_hip.HImerge.physics_MinBiasOverlay_EXT${s}.test${outset}/`; do

#for s in ${stream}; do echo "EXT${s}"; #pass in the list of bad EXT numbers from above step, i.e. "10 42 44"
# for d in `dq2-list-datasets-container group.dataprep.BSmultipleselector.GRL.${testn}_EXT${s}.merge.test${outset}/`; do

  dq2-ls -f -H $d |grep Zerobias > temp192.txt;
  cat temp192.txt >> progress77.txt
  grep "bytes" temp192.txt > temp193.txt
  if [ $? -eq 0 ]; then echo "Dataset $d has "; cat temp193.txt; 
   echo
   cat temp193.txt | cut -f 3
   echo
   for gg in `cat temp193.txt | cut -f 3`; do 
    echo "Deleting file with guid $gg from dataset $d"
    dq2-delete-files $d $gg
   done
  fi

 done
 rm temp*.txt
done
rm progress77.txt

