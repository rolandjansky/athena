#!/bin/sh

## Check arguments
if [ $# -lt 1 ] ; then
   echo "Usage: tct_recotrf.sh <jobid> [<trigstream>] [<nfiles>] [<nevents>] [<castorrundir>] [<amitag>] [<findlatestcastordir>] [<logfile>]"
   exit 1 
fi

## Intercept dryrun option if given
DRYRUN=0 
if [ "$1" = "-d" ] ; then
  DRYRUN=1
  shift
fi

## set configuration parameters
JOB=$1
TRIGSTR=IDCosmic
NFILES=3
NEVENTS=500
CASTOR=/castor/cern.ch/grid/atlas/DAQ/2008/91890
AMITAG=f128
CASTORSUBDIR=False
LOGFILE=
if [ $# -gt 1 ] ; then
  TRIGSTR=$2
fi
if [ $# -gt 2 ] ; then
  if [ $3 -lt 1 ] ; then
    echo "Need at least one input collection. Exit."
    echo "Usage: tct_recotrf.sh <jobid> [<trigstream>] [<nfiles>] [<nevents>] [<castorrundir>] [<amitag>] [<beamtype>] [<findlatestcastordir>] [<logfile>]"
    exit 1
  fi
  NFILES=$3
fi
if [ $# -gt 3 ] ; then
  NEVENTS=$4
fi
if [ $# -gt 4 ] ; then
  CASTOR=$5
fi
if [ $# -gt 5 ] ; then
  AMITAG=$6
fi
if [ $# -gt 6 ] ; then
  CASTORSUBDIR=$7
fi
if [ $# -gt 7 ] ; then
  BEAMTYPE=$8
fi
if [ $# -gt 8 ] ; then
  LOGFILE=$9
fi

## generate and run dynamic script for copying (3) files from castor (named 0.data,1.data,2.data)
date
tct_generate_copyscript.py $JOB physics_$TRIGSTR $NFILES $CASTOR $CASTORSUBDIR
if [ $DRYRUN -ne 1 ]; then
  ./copylinkscript.sh
fi
date

INFILE=0.data
count=1
while [ $count -lt ${NFILES} ] ;
do
  INFILE=${INFILE},$count.data
  count=`expr $count + 1`
done                                                            

## implement additional recotrf arguments, obtained from ami
#if [ ${PREEXEC} ]; then
#  PREEXEC="preExec='${PREEXEC};rec.doPerfMon=True;rec.doDetailedPerfMon=True'"
#else
#  PREEXEC="preExec='rec.doPerfMon=True;rec.doDetailedPerfMon=True'"
#fi

MAXEVENTS="maxEvents=${NEVENTS}"

## now run the transfrom
RUNCMD="Reco_trf.py AMI=${AMITAG} inputBSFile=${INFILE} ${MAXEVENTS} trigStream=${TRIGSTR} outputESDFile=myESD_${TRIGSTR}_${JOB}.pool.root outputAODFile=job_${TRIGSTR}_${JOB}.AOD.pool.root outputMuonCalibNtup=muonCalib_${TRIGSTR}_${JOB}.root outputPixelCalibNtup=trackVal_${TRIGSTR}_${JOB}.root  HIST=myMergedMonitoring_${TRIGSTR}_${JOB}.root outputTAGComm=myTAGCOMM_${TRIGSTR}_${JOB}.root outputDESD_PIXELCOMMFile=PIXELCOMM_${TRIGSTR}_${JOB}.pool.root outputDESD_IDCOMMFile=IDCOMM_${TRIGSTR}_${JOB}.pool.root outputDESD_CALOCOMMFile=CALOCOMM_${TRIGSTR}_${JOB}.pool.root outputDESD_MUONCOMMFile=MUONCOMM_${TRIGSTR}_${JOB}.pool.root outputDESD_TILECOMMFile=TILECOMM_${TRIGSTR}_${JOB}.pool.root outputNTUP_TRIG=TRIG_${TRIGSTR}_${JOB}.root"
if [ ${POSTINC} ]; then
  echo adding postInclude ${POSTINC}
  RUNCMD="Reco_trf.py AMI=${AMITAG} inputBSFile=${INFILE} ${MAXEVENTS} trigStream=${TRIGSTR} postInclude=${POSTINC} outputESDFile=myESD_${TRIGSTR}_${JOB}.pool.root outputAODFile=job_${TRIGSTR}_${JOB}.AOD.pool.root outputMuonCalibNtup=muonCalib_${TRIGSTR}_${JOB}.root outputPixelCalibNtup=trackVal_${TRIGSTR}_${JOB}.root  HIST=myMergedMonitoring_${TRIGSTR}_${JOB}.root outputTAGComm=myTAGCOMM_${TRIGSTR}_${JOB}.root outputDESD_PIXELCOMMFile=PIXELCOMM_${TRIGSTR}_${JOB}.pool.root outputDESD_IDCOMMFile=IDCOMM_${TRIGSTR}_${JOB}.pool.root outputDESD_CALOCOMMFile=CALOCOMM_${TRIGSTR}_${JOB}.pool.root outputDESD_MUONCOMMFile=MUONCOMM_${TRIGSTR}_${JOB}.pool.root outputDESD_TILECOMMFile=TILECOMM_${TRIGSTR}_${JOB}.pool.root outputNTUP_TRIG=TRIG_${TRIGSTR}_${JOB}.root"
fi

if [ ${PREEXEC} ]; then
  echo Adding preExec ${PREEXEC}
  RUNCMD="${RUNCMD} append_preExec='${PREEXEC}'"
fi

if [ ${BEAMTYPE} ]; then
  echo Changing beam type ${BEAMTYPE}
  RUNCMD="Reco_trf.py AMI=${AMITAG} inputBSFile=${INFILE} ${MAXEVENTS} trigStream=${TRIGSTR} outputESDFile=myESD_${TRIGSTR}_${JOB}.pool.root outputAODFile=job_${TRIGSTR}_${JOB}.AOD.pool.root outputMuonCalibNtup=muonCalib_${TRIGSTR}_${JOB}.root outputPixelCalibNtup=trackVal_${TRIGSTR}_${JOB}.root  HIST=myMergedMonitoring_${TRIGSTR}_${JOB}.root outputTAGComm=myTAGCOMM_${TRIGSTR}_${JOB}.root outputDESD_PHOJETFile=PHOJET.root outputDESD_SGLMUFile=SGLMU.root outputDESDM_TRACKFile=TRACK.root outputDESDM_MUONFile=MUON.root outputDESD_METFile=MET.root outputDESD_MBIASFile=MBIAS.root outputDESDM_EGAMMAFile=EGAMMA.root outputDESDM_CALJETFile=CALJET.root outputDESD_SGLELFile=SGLEL.root outputNTUP_TRIG=TRIG_${TRIGSTR}_${JOB}.root beamType=${BEAMTYPE}"
  #RUNCMD="${RUNCMD} beamType=${BEAMTYPE}"
fi

echo
echo ">> Now running command:"
echo ">> ===================="
echo "$RUNCMD"
echo 

if [ $DRYRUN -ne 1 ]; then
  if [ ! "$LOGFILE" = "" ] ; then
    $RUNCMD 2>&1 | tee $LOGFILE
  else
    $RUNCMD
  fi
fi

echo
echo ">> Finished command:"
echo ">> ================="
echo "$RUNCMD"
echo 

## perfmon
if [ ! -z "`ls *.pmon.gz`" ]; then
  for pmonfile in `ls *.pmon.gz`; do
    echo "Now post-processing perfmon file : ${pmonfile}"
    pmoncmd="perfmon.py ${pmonfile} -s True"
    $pmoncmd
  done
fi

## done
date

