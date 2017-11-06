#!/bin/bash

##############################################################################
#
#
# My FTK Tool
#
#
##############################################################################

EOSCOMMAND="eos"
DIR="$( cd "$( dirname "${BASH_SOURCE[0]}" )" && pwd )"

# Default values
OUTPUTPATHDEFAULT="/afs/cern.ch/user/p/phchang/scratch/ftksim/TrigFTKSim_output"
OUTPUTNAMEDEFAULT="output_test"
SAMPLEDEFAULT="IBLsingleMuons"
MAXPATTERNSDEFAULT="4194304"
#MAXPATTERNSDEFAULT="16777216"
MAXEVENTSDEFAULT="100000"
BATCHMODEDEFAULT="0"
SSFILEDEFAULT="../config/ss_file/raw_8LcIBL123_unusedmedium.ss"
NCONNDEFAULT="4"
BANKPOSITIONDEFAULT="/afs/cern.ch/user/p/phchang/scratch/ftksim/banks/TDAQTDR_banks"
QUEUEDEFAULT="8nh"
HWSDEVDEFAULT="../config/hwsdev_file/raw_12L.hw"
PMAPFILEDEFAULT="../config/map_file/raw_8LcIbl123.pmap"
NCOORDDEFAULT=16

# Help
usage() {
  echo "ERROR - Usage:"
  echo
  echo "      sh $0 OPTIONSTRINGS ..."
  echo
  echo "Options with arguments:"
  echo "  -h    Help"
  echo "  -O    Output path  (e.g. -O [DEFAULT=${OUTPUTPATHDEFAULT}])"
  echo "  -o    Output name  (e.g. -o [DEFAULT=${OUTPUTNAMEDEFAULT}])"
  echo "  -A    Action       (e.g. -A roadfindertsp,STOP,trackfitter711)"
  echo "  -I    Input sample (e.g. -I [DEFAULT=${SAMPLEDEFAULT}])"
  echo "  -p    Max patterns (e.g. -p [DEFAULT=${MAXPATTERNSDEFAULT}])"
  echo "  -n    Max events   (e.g. -n [DEFAULT=${MAXEVENTSDEFAULT}])"
  echo "  -s    ss file      (e.g. -s [DEFAULT=${SSFILEDEFAULT}])"
  echo "  -C    N connection (e.g. -n [DEFAULT=${NCONNDEFAULT}])"
  echo "  -R    Region       (e.g. -R 25)"
  echo "  -S    Subreg       (e.g. -S 0)"
  echo "  -Q    BSUB queue   (e.g. -Q [DEFAULT=${QUEUEDEFAULT}])"
  echo "  -B    Bankdir path (e.g. -n [DEFAULT=${BANKPOSITIONDEFAULT})"
  echo "  -b    batch mode   (e.g. -b [DEFAULT=${BATCHMODEDEFAULT}])"
  echo "  -H    hit warrior  (e.g. -b [DEFAULT=${HWSDEVDEFAULT}])"
  echo "  -M    PMAP         (e.g. -p [DEFAULT=${PMAPFILEDEFAULT}])"
  echo "  -N    NCOORD       (e.g. -p [DEFAULT=${NCOORDDEFAULT}])"
  echo
  exit
}

# Setup command for batch nodes
setup() {
  echo ""
  echo "=============================================="
  echo "Creating banks"
  source $DIR/myrun_create_bank_directory.sh ${REGION} ${SUBREG}
  echo "Setting up environment"
  cd $DIR/../../
  . ftksetup.sh
  cd TrigFTKSim/scripts
  echo "Environment setting done!"
  echo "=============================================="
  echo ""
}

# Command-line opts
while getopts ":O:o:A:I:p:n:s:b:C:S:R:Q:H:M:N:h" OPTION; do
  case $OPTION in
    O) OUTPUTPATH=${OPTARG};;
    o) OUTPUTNAME=${OPTARG};;
    A) ACTION=${OPTARG};;
    I) SAMPLE=${OPTARG};;
    p) MAXPATTERNS=${OPTARG};;
    n) MAXEVENTS=${OPTARG};;
    s) SSFILE=${OPTARG};;
    b) BATCHMODE=${OPTARG};;
    C) NCONN=${OPTARG};;
    S) SUBREG=${OPTARG};;
    B) BANKPOSITION=${OPTARG};;
    R) REGION=${OPTARG};;
    Q) QUEUE=${OPTARG};;
    H) HWSDEV=${OPTARG};;
    M) PMAPFILE=${OPTARG};;
    N) NCOORD=${OPTARG};;
    h) usage;
  esac
done

if [ -z ${OUTPUTPATH}   ] ; then OUTPUTPATH=${OUTPUTPATHDEFAULT}; fi
if [ -z ${OUTPUTNAME}   ] ; then OUTPUTNAME=${OUTPUTNAMEDEFAULT}; fi
if [ -z ${ACTION}       ] ; then usage; fi
if [ -z ${SAMPLE}       ] ; then SAMPLE=${SAMPLEDEFAULT}; fi
if [ -z ${MAXPATTERNS}  ] ; then MAXPATTERNS=${MAXPATTERNSDEFAULT}; fi
if [ -z ${MAXEVENTS}    ] ; then MAXEVENTS=${MAXEVENTSDEFAULT}; fi
if [ -z ${SSFILE}       ] ; then SSFILE=${SSFILEDEFAULT}; fi
if [ -z ${NCONN}        ] ; then NCONN=${NCONNDEFAULT}; fi
if [ -z ${SUBREG}       ] ; then usage; fi
if [ -z ${REGION}       ] ; then usage; fi
if [ -z ${BANKPOSITION} ] ; then BANKPOSITION=${BANKPOSITIONDEFAULT}; fi
if [ -z ${QUEUE}        ] ; then QUEUE=${QUEUEDEFAULT}; fi
if [ -z ${HWSDEV}       ] ; then HWSDEV=${HWSDEVDEFAULT}; fi
if [ -z ${PMAPFILE}     ] ; then PMAPFILE=${PMAPFILEDEFAULT}; fi
if [ -z ${NCOORD}       ] ; then NCOORD=${NCOORDDEFAULT}; fi

# Verbose
summary() {
  date
  echo "================================================"
  echo "$0 $*"
  echo "------------------------------------------------"
  echo "OUTPUTPATH  : ${OUTPUTPATH}"
  echo "OUTPUTNAME  : ${OUTPUTNAME}"
  echo "ACTION      : ${ACTION}"
  echo "SAMPLE      : ${SAMPLE}"
  echo "MAXPATTERNS : ${MAXPATTERNS}"
  echo "MAXEVENTS   : ${MAXEVENTS}"
  echo "SSFILE      : ${SSFILE}"
  echo "NCONN       : ${NCONN}"
  echo "REGION      : ${REGION}"
  echo "SUBREG      : ${SUBREG}"
  echo "BANKDIR     : ${BANKPOSITION}""    # If batch mode this will be overridden"
  echo "BATCHMODE   : ${BATCHMODE}"
  echo "QUEUE       : ${QUEUE}"
  echo "HWSDEV      : ${HWSDEV}"
  echo "PMAPFILE    : ${PMAPFILE}"
  echo "NCOORD      : ${NCOORD}"
  echo "================================================"
}

finished() {
  NJOBSDONE=$(ls ${OUTPUTDIR}/raw_*/tracks/${REGION}/${SUBREG} | wc -l)
  TOTALJOBS=$(cat ${DIR}/../config/input/inputprod_raw_${SAMPLE}.list | wc -l)
  if [ ${NJOBSDONE} -eq ${TOTALJOBS} ]; then
    echo 1
  else
    echo 0
  fi
}

#eof
