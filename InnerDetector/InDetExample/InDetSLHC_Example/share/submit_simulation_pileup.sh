#!/bin/bash
# Submit athena_simulation_pileup jobs to a PBS queue

if [ $# -lt 2 ]; then
  echo "Syntax:  . submit_simulation_pileup.sh firstindex lastindex"
  exit 1
fi

index=$1
lastindex=$2
username=`whoami`
#cpu_time='20:00:00,mem=3g'
cpu_time='20:00:00'
cern_queue="1nd"

. config-job.sh

outdir=`pwd`
scriptrdir="`dirname \"$0\"`"
scriptdir=`cd $scriptrdir; pwd`
scriptname="athena_simulation_pileup.sh"

while [ $index -le $lastindex ]; do

  # check to see if job output already exists in the working directory
  sim_output_file=`$scriptdir/$scriptname $index -o`
  if [ -e $sim_output_file ]; then
    echo "Job $index already has an output file $sim_output_file"
    index=`expr $index + 1`
    continue;
  fi

  if [ $EXEC_MODE != "INTERACTIVE" ]; then
    # check to see if job has been submitted
    if [ $EXEC_MODE == "PBS" ]; then
      job_submitted=`qstat | grep $username | grep ${P_QUEUENAME}_$index`
    elif [ $EXEC_MODE == "LSF" ]; then
      job_submitted=`bjobs -w | grep $username | grep ${P_QUEUENAME}_$index`
    fi
    if [ -n "${job_submitted}" ]; then
      echo "Job $index already submitted"
      index=`expr $index + 1`
      continue;
    fi
  fi

  case $EXEC_MODE in
    PBS) (echo cd $outdir; echo $scriptrdir/$scriptname $index) | \
         qsub -N ${P_QUEUENAME}_$index -l cput=${cpu_time} -V - ;;
    LSF) bsub -J ${P_QUEUENAME}_$index -q ${cern_queue} $scriptrdir/$scriptname $index ;;
    INTERACTIVE) $scriptdir/$scriptname $index ;;
    *) echo Unrecognized execution mode $EXEC_MODE ;;
  esac

  index=`expr $index + 1`
done

exit

