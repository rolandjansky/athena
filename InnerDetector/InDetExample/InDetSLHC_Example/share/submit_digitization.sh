#!/bin/bash
# Submit athena_digitization jobs to a PBS queue

if [ $# -lt 4 ]; then
  echo "Syntax:  . submit_digitization.sh npileup injobindex firstindex lastindex"
  exit 1
fi

. config-job.sh

INPILEUPLIST=$PILEUPLIST
NPILEUP=$1
injobindex=$2
index=$3
lastindex=$4
username=`whoami`
#cpu_time='20:00:00,mem=3g'
cpu_time='2:00:00'
cern_queue="8nh"

EVTMAX=$D_EVTMAX

outdir=`pwd`
scriptrdir="`dirname \"$0\"`"
scriptdir=`cd $scriptrdir; pwd`
scriptname="athena_digitization.sh"

while [ $index -le $lastindex ]; do

  # check to see if job output already exists in the working directory
  sim_output_file=`$scriptdir/$scriptname $NPILEUP $injobindex $index -o`
  if [ -e $sim_output_file ]; then
    echo "Job $index already has an output file $sim_output_file"
    index=`expr $index + 1`
    continue;
  fi

  job_name=${D_QUEUENAME}_${NPILEUP}_${injobindex}_$index
  if [ $EXEC_MODE != "INTERACTIVE" ]; then
    # check to see if job has been submitted
    if [ $EXEC_MODE == "PBS" ]; then
      job_submitted=`qstat | grep $username | grep $job_name`
    elif [ $EXEC_MODE == "LSF" ]; then
      job_submitted=`bjobs -w | grep $username | grep $job_name`
    fi
    if [ -n "${job_submitted}" ]; then
      echo "Job $index already submitted"
      index=`expr $index + 1`
      continue;
    fi
  fi

  case $EXEC_MODE in
    PBS) (echo cd $outdir; echo $scriptrdir/$scriptname $NPILEUP $injobindex $index) | \
         qsub -N $job_name -l cput=${cpu_time} -V - ;;
    LSF) bsub -J $job_name -q ${cern_queue} $scriptrdir/$scriptname $NPILEUP $injobindex $index ;;
    INTERACTIVE) $scriptdir/$scriptname $NPILEUP $injobindex $index ;;
    *) echo Unrecognized execution mode $EXEC_MODE ;;
  esac

  index=`expr $index + 1`
done 

exit

