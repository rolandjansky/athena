#/bin/bash
# Script to run pileup digitization job
# Syntax:  athena_digitization.sh layout evtmax jobindex

if [ $# -lt 3 ]; then
  echo "Syntax:  . athena_digitization.sh npileup injobindex outjobindex [flag]"
  exit 1
fi

. config-job.sh

if [ $LAYOUT == "SLHC" ]; then
  export LAYOUT=
fi
export INPILEUPLIST=$PILEUPLIST
export NPILEUP=$1
export EVTMAX=$D_EVTMAX
export INJOBINDEX=$2
export OUTJOBINDEX=$3
zi=`expr $OUTJOBINDEX - 1`
export EVTSKIP=`expr $zi \* $EVTMAX`

export SCRIPTDIR="`dirname \"$0\"`"

export INJOBNAME=$S_JOBNAME
export JOBNAME=$D_JOBNAME
base1=`expr $INJOBINDEX \* 10`
base2=`expr $INJOBINDEX + $OUTJOBINDEX`
base3=`expr $base2 \* 1000`
export SEEDNUMBER1=`expr $base3 + 453`
export SEEDNUMBER2=`expr $base3 + 123`
export INFILENAME="${INJOBNAME}_${LAYOUT}_${INJOBINDEX}_Hits.pool.root"
export OUTFILENAME="${JOBNAME}_${LAYOUT}_${NPILEUP}_${INJOBINDEX}_${OUTJOBINDEX}_Digits.pool.root"
export LOGFILENAME="${JOBNAME}_${LAYOUT}_${NPILEUP}_${INJOBINDEX}_${OUTJOBINDEX}.log"

if [ $# -gt 3 ]; then
  if [ $4 == "-o" ]; then
    echo $OUTFILENAME
  elif [ $4 == "-l" ]; then
    echo $LOGFILENAME
  fi
  exit 0
fi

echo "=================================================="
echo "SIMULATION PILEUP JOB " $OUTJOBINDEX " FOR " $LAYOUT
echo "=================================================="

echo "JOBNAME     = " $JOBNAME
echo "INJOBNAME   = " $INJOBNAME
echo "LAYOUT      = " $LAYOUT
echo "INFILENAME  = " $INFILENAME
echo "OUTFILENAME = " $OUTFILENAME
echo "LOGFILENAME = " $LOGFILENAME
echo "SEEDNUMBER1 = " $SEEDNUMBER1
echo "SEEDNUMBER2 = " $SEEDNUMBER2
echo "INJOBINDEX  = " $INJOBINDEX
echo "OUTJOBINDEX = " $OUTJOBINDEX
echo "EVTMAX      = " $EVTMAX
echo "EVTSKIP     = " $EVTSKIP
echo "NPILEUP     = " $NPILEUP
echo "INPILEUPLIST= " $INPILEUPLIST
echo "SCRIPTDIR   = " $SCRIPTDIR

echo "=================================================="
echo "Running athena"
echo "=================================================="

if [ $EXEC_MODE == "INTERACTIVE" ]; then
  time athena.py $SCRIPTDIR/jobOptions_digitization_pileup_SLHC.py
else
  time athena.py $SCRIPTDIR/jobOptions_digitization_pileup_SLHC.py >& $LOGFILENAME
fi

echo "=================================================="
echo "Done"
echo "=================================================="

exit
