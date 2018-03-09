#!/bin/bash

##############################################################################
#
#
# Process a single connection file for single region
#
#
##############################################################################

EOSCOMMAND="/afs/cern.ch/project/eos/installation/0.2.31/bin/eos.select"
DIR="$( cd "$( dirname "${BASH_SOURCE[0]}" )" && pwd )"

# Help
usage()
{
  echo "ERROR - Usage:"
  echo
  echo "put both 8L and 12L, .gcon.bz2 and .patt.bz2 files in the same directory. then..."
  echo "      sh $(basename $0) OPTIONSTRINGS ..."
  echo
  echo "Options with arguments:"
  echo "  -h    Help                   (Display this message)"
  echo "  -d    bankdir                (e.g. -d /path/to/bank/dir/)"
  echo "  -r    Region                 (e.g. -r 16)"
  echo ""
  echo "  e.g."
  echo "  sh generate_connection/make_conn/process_conn.sh -d ./sector_const -r 0"
  echo ""
  echo "  for more information consult : https://twiki.cern.ch/twiki/bin/viewauth/Atlas/FTKSimShortTutorial#Generating_connection_files"
  echo
  exit
}

# Command-line opts
while getopts ":d:r:h" OPTION; do
  case $OPTION in
    r) REGION=${OPTARG};;
    d) BANKDIR=${OPTARG};;
    h) usage;;
    :) usage;;
  esac
done

if [ -z ${REGION} ]; then usage; fi
if [ -z ${BANKDIR} ]; then usage; fi

# Verbose
date
echo "================================================"
echo "$(basename $0) $*"
echo "$(basename $0) $*" >> $DIR/.$(basename $0).history
echo "------------------------------------------------"
echo "REGION         : ${REGION}"
echo "BANKDIR        : ${BANKDIR}"
echo "================================================"

#
#
# function
#
#

# check if it's a number
function is_number() {
  re='^[0-9]+$'
  if [[ $1 =~ $re ]]; then echo 1; else echo 0; fi
}

#cd $BANKDIR/patt
cd $BANKDIR
mkdir -p make_connection_log/
LOG="make_connection_log/region_${REGION}.log"
python $DIR/find_extrapolation_relations_v2.py sectors_raw_8L_reg${REGION}.patt.bz2 1 &> $LOG
IS_SUCCESS=$(tail $LOG | grep "sucess")
echo $IS_SUCCESS
echo ""
echo ""
if [ -n "$IS_SUCCESS" ]; then
  echo "The run was sucessful. The logfile is outputted : " $DIR/$LOG
else
  echo "Something went wrong in the process check the log file: " $DIR/$LOG
fi


#eof
