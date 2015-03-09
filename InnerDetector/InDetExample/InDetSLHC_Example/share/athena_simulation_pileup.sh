#/bin/bash
# Script to run pileup simulation job

if [ $# -lt 1 ]; then
  echo "Syntax:  . athena_simulation_pileup.sh jobindex [flag]"
  exit 1
fi

. config-job.sh

if [ $LAYOUT == "SLHC" ]; then
  export LAYOUT=
fi
export JOBINDEX=$1

export JOBNAME=$P_JOBNAME
export EVTMAX=$P_EVTMAX

export SCRIPTDIR="`dirname \"$0\"`"

export SEEDNUMBER=$JOBINDEX
export OUTFILENAME="${JOBNAME}_${LAYOUT}_${JOBINDEX}_Hits.pool.root"
export LOGFILENAME="${JOBNAME}_${LAYOUT}_${JOBINDEX}.log"

if [ $# -gt 1 ]; then
  if [ $2 == "-o" ]; then
    echo $OUTFILENAME
  elif [ $2 == "-l" ]; then
    echo $LOGFILENAME
  fi
  exit 0
fi

psec=`expr $JOBINDEX % 10`

echo "=================================================="
echo "SIMULATION PILEUP JOB " $JOBINDEX " FOR " $LAYOUT
echo "=================================================="

echo "JOBNAME     = " $JOBNAME
echo "LAYOUT      = " $LAYOUT
echo "OUTFILENAME = " $OUTFILENAME
echo "LOGFILENAME = " $LOGFILENAME
echo "SEEDNUMBER  = " $SEEDNUMBER
echo "EVTMAX      = " $EVTMAX
echo "SCRIPTDIR   = " $SCRIPTDIR

echo "=================================================="
echo "Running athena after $psec seconds"
echo "=================================================="

if [ $EXEC_MODE == "INTERACTIVE" ]; then
  time athena.py $SCRIPTDIR/jobOptions_simulation_pileup_SLHC.py
elif [ $EXEC_MODE == "TEST" ]; then
  athena.py -i $SCRIPTDIR/jobOptions_simulation_pileup_SLHC.py
else
  sleep $psec
  time athena.py $SCRIPTDIR/jobOptions_simulation_pileup_SLHC.py >& $LOGFILENAME
fi

echo "=================================================="
echo "Done"
echo "=================================================="

exit
