#!/bin/bash

if [[ -z $INPUT_PREFIX ]]
    then
    INPUT_PREFIX=`pwd`
fi

PROCESSTABLE=${INPUT_PREFIX}/run_scripts/process_table
if [ -z $1 ]
    then
    echo "usage:"
    echo "run <process> [<iteration>] [<events per job>]"
    echo "available processes are:"
    awk '{print $1}' $PROCESSTABLE
    exit -1
fi

JobCoreName=$1

ITERATION=1
if [ ! -z $2 ]
    then
    ITERATION=$2
fi

EventsPerJob=100000
if [ ! -z $3 ]
    then
    EventsPerJob=$3
fi

echo "Job segment number $ITERATION"
echo "Running $EventsPerJob events per job"

jobcorename=(`awk '{print $1}' $PROCESSTABLE`)
runnr=(`awk '{print $2}' $PROCESSTABLE`)
inputjofile=(`awk '{print $3}' $PROCESSTABLE`)
outputname=(`awk '{print $4}' $PROCESSTABLE`)
postjofile=(`awk '{print $5}' $PROCESSTABLE`)

i=0
RunNr="" 
for JCN in ${jobcorename[@]}
  do
  if [[ ${jobcorename[$i]} == $JobCoreName ]]
      then
      RunNr=${runnr[$i]}
      InputJoFile=${inputjofile[$i]}
      OutputName=${outputname[$i]}
      PostJoFile=${postjofile[$i]}
  fi
  i=$(($i+1))
done

if [[ $RunNr == "" ]]
    then
    echo "Process $JobCoreName not found"
    echo "available processes are:"
    awk '{print $1}' $PROCESSTABLE
    exit -1
fi

#run in current directory
mkdir run >& /dev/null
mkdir run/$JobCoreName >& /dev/null
mkdir run/$JobCoreName/job_${ITERATION} >& /dev/null
rundir=run/$JobCoreName/job_${ITERATION}
echo "running reference files production"
echo "Output directory:" ${INPUT_PREFIX}/out/$JobCoreName/

cp ${INPUT_PREFIX}/share/$PostJoFile $rundir;
cd $rundir;
  
tmpRnd=$RANDOM

echo "check log file:"
echo "$rundir/${JobCoreName}_${ITERATION}.log"
   
JOBOPTSEARCHPATH=/cvmfs/atlas.cern.ch/repo/sw/Generators/MC12JobOptions/latest/common:$JOBOPTSEARCHPATH; 
Generate_trf.py ecmEnergy=8000. runNumber=$RunNr firstEvent=1 maxEvents=$EventsPerJob randomSeed=$tmpRnd \
    jobConfig=$InputJoFile outputEVNTFile=EVNT.$OutputName \
    postInclude=$PostJoFile >& ${JobCoreName}_${ITERATION}.log

#save output
mkdir ${INPUT_PREFIX}/out >& /dev/null
mkdir ${INPUT_PREFIX}/out/$JobCoreName >& /dev/null
mkdir ${INPUT_PREFIX}/out/$JobCoreName/root >& /dev/null
mkdir ${INPUT_PREFIX}/out/$JobCoreName/log >& /dev/null

cp ${JobCoreName}_${ITERATION}.log ${INPUT_PREFIX}/out/$JobCoreName/log
rootfile=`ls RTT*.root` 
cp ${rootfile} ${INPUT_PREFIX}/out/$JobCoreName/root/${rootfile%.root}_${ITERATION}.root

#cleanup
cd ..
rm -r job_${ITERATION}
