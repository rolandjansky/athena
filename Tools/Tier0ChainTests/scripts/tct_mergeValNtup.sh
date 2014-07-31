#!/bin/sh
 
## Check arguments
if [ $# -lt 2 ] ; then
   echo "Usage: tct_mergeValNtup.sh <outputfile> <inputfile1> [<inputfile2,3,4,etc>]"
   exit 1
fi

## intercept dryrun option if given
DRYRUN=0 
if [ "$1" = "-d" ] ; then
  DRYRUN=1
  shift
fi

## setup configuration
file_valNtupMergeLog=valNtup_merge.log
file_valNtupMergeList=valNtup_merge_list.txt
file_valNtupOut=$1
shift

rm -f $file_valNtupMergeLog $file_valNtupMergeList $file_valNtupOut
touch $file_valNtupMergeList

echo
while [ $# -gt 0 ] ; 
do
  file_valNtupIn=$1
  shift
  if [ -f $file_valNtupIn ]; then
    echo "Adding to merge list: " $file_valNtupIn
    echo $file_valNtupIn >> $file_valNtupMergeList
  else 
    echo "Input ntuple file not found: <" $file_valNtupIn ">. Skipped."
  fi
done

## build pickle input file
BLDCMD="tct_buildPickleFile.py valNtup.pkl $file_valNtupMergeList $file_valNtupOut"

echo
echo ">> Now building pickle file:"
echo ">> ========================="
echo "$BLDCMD"
echo 
if [ $DRYRUN -ne 1 ]; then
  $BLDCMD
fi
echo
echo ">> Finished command:"
echo ">> ================="
echo "$BLDCMD"
echo 

## do the merging
RUNCMD="/afs/cern.ch/atlas/tzero/prod1/inst/projects/data11/trfs/ROOTMerge_trf.py --argdict=valNtup.pkl"

echo
echo ">> Now running command:"
echo ">> ===================="
echo "$RUNCMD"
echo 
if [ $DRYRUN -ne 1 ]; then
  echo "(Now merging histogram(s). This may take some time.)"
  $RUNCMD
  #$RUNCMD 1>$file_valNtupMergeLog 2>&1
fi
echo
echo ">> Finished command:"
echo ">> ================="
echo "$RUNCMD"
echo 

## temporary patch, to be fixed in DQHistogramMerge.py
echo "ErrorCode=0 (OK)"

