#!/bin/bash

##############################################################################
#
#
# Process a single connection file for single region
#
#
##############################################################################

EOSCOMMAND="eos"
DIR="$( cd "$( dirname "${BASH_SOURCE[0]}" )" && pwd )"

# Help
usage()
{
  echo "ERROR - Usage:"
  echo
  echo "      sh $(basename $0) OPTIONSTRINGS ..."
  echo
  echo "Options with arguments:"
  echo "  -h    Help                   (Display this message)"
  echo "  -d    bankdir                (e.g. -d /path/to/bank/dir/ the bankdir is the directory that contains patt/ and gcon/ directory)"
  echo "  -r    Region                 (e.g. -r 16)"
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

cd $BANKDIR/patt
mkdir -p make_connection_log/
LOG="make_connection_log/region_${REGION}.log"
python $DIR/find_extrapolation_relations_v2.py sectors_raw_8L_reg${REGION}.patt.bz2 1 &> $LOG
IS_SUCCESS=$(tail $LOG | grep "sucess")
echo $IS_SUCCESS
echo ""
echo ""
echo ""
if [ -n "$IS_SUCCESS" ]; then
  echo "The run was sucessful. The logfile is outputted : " $DIR/../patt/$LOG
else
  echo "Something went wrong in the process check the log file: " $DIR/../patt/$LOG
fi


#eof
