#!/bin/sh
 
## Check arguments
if [ $# -lt 2 ] ; then
   echo "Usage: tct_mergeHistograms.sh <outputfile> <inputfile1> [<inputfile2,3,4,etc>]"
   exit 1 
fi

## intercept dryrun option if given
DRYRUN=0 
if [ "$1" = "-d" ] ; then
  DRYRUN=1
  shift
fi

## setup configuration
file_histMergeLog=hist_merge.log
file_histMergeList=hist_merge_list.txt
file_histOut=$1
shift

rm -f $file_histMergeLog $file_histMergeList $file_histOut
touch $file_histMergeList

echo
while [ $# -gt 0 ] ; 
do
  file_histIn=$1
  shift
  if [ -f $file_histIn ]; then
    echo "Adding to merge list: " $file_histIn
    echo $file_histIn >> $file_histMergeList
  else 
    echo "Input histogram file not found: <" $file_histIn ">. Skipped."
  fi
done

## do the merging
RUNCMD="DQHistogramMerge.py $file_histMergeList $file_histOut True"

echo
echo ">> Now running command:"
echo ">> ===================="
echo "$RUNCMD"
echo 
if [ $DRYRUN -ne 1 ]; then
  echo "(Now merging histogram(s). This may take some time.)"
  $RUNCMD
  #$RUNCMD 1>$file_histMergeLog 2>&1
fi
echo
echo ">> Finished command:"
echo ">> ================="
echo "$RUNCMD"
echo 

## temporary patch, to be fixed in DQHistogramMerge.py
echo "ErrorCode=0 (OK)"

