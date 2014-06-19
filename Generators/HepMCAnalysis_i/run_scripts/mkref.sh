#!/bin/bash

PROCESSTABLE="run_scripts/process_table"

if [ -z $1 ]
    then
    echo "usage:"
    echo "./run_scripts/mkref <process> [<events per job>]"
    echo "available processes are:"
    awk '{print $1}' $PROCESSTABLE
    exit -1
fi
JobCoreName=$1

EventsPerJob=10000
if [ ! -z $2 ]
    then
    EventsPerJob=$2
fi

echo "Running $EventsPerJob events per job"

User=`whoami`

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

#---------------------------------------------------------------------- 
# Start execution
#---------------------------------------------------------------------- 
outdir=/tmp/$User/$JobCoreName
echo "Submitting reference files' production..."
echo "Output directory:" $outdir

rm -r $outdir >& /dev/null
mkdir $outdir

i=0

for i in {0..10};
do 
  echo "Job #" $i;
  
  mkdir $outdir/job_$i;
  cp share/$PostJoFile $outdir/job_$i;
  cp share/make_avg.C $outdir;
  cd $outdir/job_$i;
  
  tmpRnd=$RANDOM
   
  JOBOPTSEARCHPATH=/cvmfs/atlas.cern.ch/repo/sw/Generators/MC12JobOptions/latest/common:$JOBOPTSEARCHPATH; 
  Generate_trf.py ecmEnergy=8000. runNumber=$RunNr firstEvent=1 maxEvents=$EventsPerJob randomSeed=$tmpRnd \
    jobConfig=$InputJoFile outputEVNTFile=EVNT.$OutputName \
    postInclude=$PostJoFile >& $JobCoreName_$i.log &  

  cd - > /dev/null
done
