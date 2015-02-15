#/bin/bash
# Script to run tracking job
# Syntax:  athena_tracking.sh layout evtmax jobindex

if [ $# -lt 3 ]; then
  echo "Syntax:  . athena_tracking.sh npileup injobindex jobindex [flag]"
  exit 1
fi

. config-job.sh

if [ $LAYOUT == "SLHC" ]; then
  export LAYOUT=
fi
export EVTMAX=$T_EVTMAX
export NPILEUP=$1
export INJOBINDEX=$2
export JOBINDEX=$3

export SCRIPTDIR="`dirname \"$0\"`"

export INJOBNAME=$D_JOBNAME
export JOBNAME=$T_JOBNAME

export INFILENAME=`$SCRIPTDIR/athena_digitization.sh $NPILEUP $INJOBINDEX $JOBINDEX -o`

export VALIDATION_OUTFILENAME="${JOBNAME}_${LAYOUT}_${NPILEUP}_${INJOBINDEX}_${JOBINDEX}_TV.root"
export D3PD_OUTFILENAME="${JOBNAME}_${LAYOUT}_${NPILEUP}_${INJOBINDEX}_${JOBINDEX}_D3PD.root"
export LOGFILENAME="${JOBNAME}_${LAYOUT}_${NPILEUP}_${INJOBINDEX}_${JOBINDEX}.log"

if [ $# -gt 3 ]; then
  if [ $4 == "-o" ]; then
    echo $D3PD_OUTFILENAME
  elif [ $4 == "-l" ]; then
    echo $LOGFILENAME
  elif [ $4 == "-d" ]; then
    echo $D3PD_OUTFILENAME
  elif [ $4 == "-v" ]; then
    echo $VALIDATION_OUTFILENAME
  fi
  exit 0
fi

echo "=================================================="
echo "SIMULATION PILEUP JOB " $JOBINDEX " FOR " $LAYOUT
echo "=================================================="

echo "JOBNAME     = " $JOBNAME
echo "INJOBNAME   = " $INJOBNAME
echo "LAYOUT      = " $LAYOUT
echo "INFILENAME  = " $INFILENAME
echo "VALIDATION_OUTFILENAME = " $VALIDATION_OUTFILENAME
echo "D3PD_OUTFILENAME = " $D3PD_OUTFILENAME
echo "LOGFILENAME = " $LOGFILENAME
echo "EVTMAX      = " $EVTMAX
echo "SCRIPTDIR   = " $SCRIPTDIR

echo "=================================================="
echo "Running athena"
echo "=================================================="

if [ $EXEC_MODE == "INTERACTIVE" ]; then
  time athena.py $SCRIPTDIR/jobOptions_tracking_pileup_SLHC.py
else
  time athena.py $SCRIPTDIR/jobOptions_tracking_pileup_SLHC.py >& $LOGFILENAME
fi

echo "=================================================="
echo "Done"
echo "=================================================="

exit
