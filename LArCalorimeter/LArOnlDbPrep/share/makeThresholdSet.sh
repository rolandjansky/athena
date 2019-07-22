#!/bin/zsh

if [[ $# > 0  ]]
then
 Run=$1
else 
 Run=266280
fi 
if [[ $# > 1 ]]
then
 Noise=$2
else 
 Noise=""
fi 
if [[ $# > 2 ]]
then
 Pileup=$3
else 
 Pileup=""
fi 
fulltag=`getCurrentFolderTag.py "COOLOFL_LAR/CONDBR2"  /LAR/NoiseOfl/CellNoise | tail -1`
#foreach i in `seq 0 11`
#foreach i in `seq 0 22`
foreach i in `seq 5 22`
#foreach i in `seq 16 22`
    cmdline="RunNumber=$Run;RunSince=$Run;tagNum=$i;noisetag=\"$fulltag\";"
    if [[ $Noise != "" ]]
    then
       cmdline+="noisesqlite=\"$Noise\";" 
    fi
    if [[ $Pileup != "" ]]
    then
       cmdline+="pileupsqlite=\"$Pileup\";"
    fi   
    athena.py -c "$cmdline" LArOnlDbPrep/LArDSPThresholdTopOptions.py
end

#foreach i in `seq 0 16`
#    tagNum=$i; python LArDSPThresholdTopOptions.py
#end

##mv *.db *.txt *.xml *.root /afs/cern.ch/user/s/smajewsk/public/LAr/.

##rm *.xml.BAK
