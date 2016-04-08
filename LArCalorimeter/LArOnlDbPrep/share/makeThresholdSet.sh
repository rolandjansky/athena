#!/bin/zsh

if [[ $# > 0  ]]
then
 Run=$1
else 
 Run=266280
fi 
if [[ $# > 1 ]]
then
 Pileup=$2
else 
 Pileup=""
fi 
if [[ $# > 2 ]]
then
 Noise=$3
else 
 Noise=""
fi 
fulltag=`getCurrentFolderTag.py "COOLOFL_LAR/CONDBR2"  /LAR/NoiseOfl/CellNoise | tail -1`
#foreach i in `seq 0 11`
#foreach i in `seq 0 22`
foreach i in `seq 5 22`
#foreach i in `seq 16 22`
    athena.py -s -c "RunNumber=$Run;tagNum=$i;pileupsqlite=\"$Pileup\";noisesqlite=\"$Noise\";noisetag=\"$fulltag\"" LArOnlDbPrep/LArDSPThresholdTopOptions.py
end

#foreach i in `seq 0 16`
#    tagNum=$i; python LArDSPThresholdTopOptions.py
#end

##mv *.db *.txt *.xml *.root /afs/cern.ch/user/s/smajewsk/public/LAr/.

##rm *.xml.BAK
