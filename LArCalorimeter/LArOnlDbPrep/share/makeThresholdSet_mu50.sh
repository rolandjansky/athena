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
foreach i in `seq 0 6`
#foreach i in `seq 16 22`
    if [[ $Noise == "" ]]
    then
       athena.py -s -c "RunNumber=$Run;tagNum=$i;pileupsqlite=\"$Pileup\";noisetag=\"$fulltag\";RunSince=$Run" LArOnlDbPrep/LArDSPThresholdTopOptions_mu50.py
    else   
       athena.py -s -c "RunNumber=$Run;tagNum=$i;pileupsqlite=\"$Pileup\";noisesqlite=\"$Noise\";noisetag=\"$fulltag\";RunSince=$Run" LArOnlDbPrep/LArDSPThresholdTopOptions_mu50.py
    fi   
end

