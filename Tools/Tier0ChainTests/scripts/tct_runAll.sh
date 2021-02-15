#!/bin/sh

## Check arguments
if [ $# -lt 1 ] ; then
   echo "Usage: tct_runAll.sh <numrecojobs> [<trigstream>] [<nfilesperjob>] [<neventsperjob>] [<castorbasedir>]"
   exit 1 
fi

## Intercept dryrun option if given
DRYRUN=0 
if [ "$1" = "-d" ] ; then
  DRYRUN=1
  shift
fi

## set configuration parameters
NUMJOBS=$1
TRIGSTR=IDCosmic
if [ $# -gt 1 ] ; then
  TRIGSTR=$2
fi
NFILES=1
if [ $# -gt 2 ] ; then
  if [ $3 -lt 1 ] ; then
    echo "Need at least one input collection per job. Exit."
    echo "Usage: tct_runAll.sh <numrecojobs> [<trigstream>] [<nfiles>] [<nevents>] [<castorrundir>]"
    exit 1
  fi
  NFILES=$3
fi
NEVENTS=-1
if [ $# -gt 3 ] ; then
  NEVENTS=$4
fi
CASTOR=/castor/cern.ch/grid/atlas/DAQ/2008/91890
if [ $# -gt 4 ] ; then
  CASTOR=$5
fi

## print configuration
echo ""
echo "Test chain configuration: "
echo "-------------------------"
echo "Number of reconstruction jobs : ${NUMJOBS}"
echo "Trigger stream                : ${TRIGSTR}"
echo "Collection / reco job         : ${NFILES}"
echo "Events / reco job             : ${NEVENTS}"
echo "Castor base directory         : ${CASTOR}"
echo ""

## run all tct commands in a sequential chain

## get ami tag
RUNCMD="tct_getAmiTag.py latest ami_recotrf.cmdargs"
echo
echo ">> Now running command:"
echo ">> ===================="
echo ${RUNCMD}
echo 
if [ $DRYRUN -ne 1 ]; then
  $RUNCMD
fi
echo 
echo ">> Finished command:"
echo ">> ===================="
echo ${RUNCMD}
echo 

## reco jobs
idx=0
while [ $idx -lt ${NUMJOBS} ] ;
do
  RUNCMD="tct_recotrf_Tier0.sh $idx ${TRIGSTR} ${NFILES} ${NEVENTS} ${CASTOR} ami_recotrf.cmdargs"
  echo
  echo ">> Now running command:"
  echo ">> ===================="
  echo ${RUNCMD}
  echo 
  if [ $DRYRUN -ne 1 ]; then
    $RUNCMD
  fi
  echo 
  echo ">> Finished command:"
  echo ">> ===================="
  echo ${RUNCMD}
  echo 
  idx=`expr $idx + 1`
done

TRFARGS="preInclude=RecExCond/MinimalCommissioningSetup.py --ignoreunknown"

## merge aods
INFILE=job_${TRIGSTR}_0.AOD.pool.root
count=1
while [ $count -lt ${NUMJOBS} ] ;
do
  INFILE=${INFILE},job_${TRIGSTR}_$count.AOD.pool.root
  count=`expr $count + 1`
done
RUNCMD="MergePool_trf.py maxEvents=-1 inputAODFile=${INFILE} trigStream=$TRIGSTR autoConfiguration=FieldAndGeo,ConditionsTag outputAODFile=MergedAOD.pool.root ${TRFARGS}"
echo
echo ">> Now running command:"
echo ">> ===================="
echo ${RUNCMD}
echo 
if [ $DRYRUN -ne 1 ]; then
  $RUNCMD
fi
echo 
echo ">> Finished command:"
echo ">> ===================="
echo ${RUNCMD}
echo 

## merge esds
INFILE=myESD_${TRIGSTR}_0.pool.root
count=1
while [ $count -lt ${NUMJOBS} ] ;
do
  INFILE=${INFILE},myESD_${TRIGSTR}_$count.pool.root
  count=`expr $count + 1`
done

RUNCMD="MergePool_trf.py maxEvents=-1 inputESDFile=${INFILE} trigStream=$TRIGSTR autoConfiguration=FieldAndGeo,ConditionsTag outputESDFile=MergedESD.pool.root ${TRFARGS}"
echo
echo ">> Now running command:"
echo ">> ===================="
echo ${RUNCMD}
echo 
if [ $DRYRUN -ne 1 ]; then
  $RUNCMD
fi
echo 
echo ">> Finished command:"
echo ">> ===================="
echo ${RUNCMD}
echo 

## merge monitoring files
INFILE=""
count=0
while [ $count -lt ${NUMJOBS} ] ;
do
  INFILE="${INFILE} myMergedMonitoring_${TRIGSTR}_$count.root"
  count=`expr $count + 1`
done

RUNCMD="tct_mergeMonHistograms.sh myMergedMonitoring.root $INFILE"
echo
echo ">> Now running command:"
echo ">> ===================="
echo ${RUNCMD}
echo 
if [ $DRYRUN -ne 1 ]; then
  $RUNCMD
fi
echo 
echo ">> Finished command:"
echo ">> ===================="
echo ${RUNCMD}
echo 

## make tag file
RUNCMD="AODtoTAG_trf.py maxEvents=-1 trigStream=${TRIGSTR} inputAODFile=MergedAOD.pool.root autoConfiguration=FieldAndGeo,ConditionsTag outputTAGFile=myTag.pool.root ${TRFARGS}"
echo
echo ">> Now running command:"
echo ">> ===================="
echo ${RUNCMD}
echo 
if [ $DRYRUN -ne 1 ]; then
  $RUNCMD
fi
echo 
echo ">> Finished command:"
echo ">> ===================="
echo ${RUNCMD}
echo 

## upload tag file
RUNCMD="tct_tagupload.sh myTag.pool.root"
echo
echo ">> Now running command:"
echo ">> ===================="
echo ${RUNCMD}
echo 
if [ $DRYRUN -ne 1 ]; then
  $RUNCMD
fi
echo 
echo ">> Finished command:"
echo ">> ===================="
echo ${RUNCMD}
echo 

## dq webdisplay
RUNCMD="tct_dqwebdisplay.sh myMergedMonitoring.root"
echo
echo ">> Now running command:"
echo ">> ===================="
echo ${RUNCMD}
echo 
if [ $DRYRUN -ne 1 ]; then
  $RUNCMD
fi
echo 
echo ">> Finished command:"
echo ">> ===================="
echo ${RUNCMD}
echo 

