#!/bin/sh

## Check arguments
if [ $# -lt 1 ] ; then
   echo "Usage: tct_tagupload.sh <tagfile>"
   exit 1 
fi

## intercept dryrun option if given
DRYRUN=0 
if [ "$1" = "-d" ] ; then
  DRYRUN=1
  shift
fi

## Check existence of template files
if [ ! -f $1 ] ; then
   echo "ERROR: Cannot find tag file " $1
   exit 1
fi

## setup configuration
tagfile=$1
timestamp=$(date "+%Y%m%d_%Hh%Mm%S")
if [ -f authentication.xml ]; then
  rm -f authentication.xml
fi

## do tag upload

RUNCMD="tagUpload_trf.py $tagfile atn_test_AtlasPOOL AtlasPOOL,takeFromEnv oracle://int8r_nolb/atlas_tags_loading_test"

echo
echo ">> Now running command:"
echo ">> ===================="
echo "$RUNCMD"
echo 
if [ $DRYRUN -ne 1 ]; then
  ln -s /afs/cern.ch/atlas/offline/test/POOLCollectionTools/authentication.xml
  CollRemove -src atn_test_AtlasPOOL  RelationalCollection -srcconnect oracle://int8r_nolb/atlas_tags_loading_test
  $RUNCMD
  #$RUNCMD >& tag_upload_$timestamp.log
fi
echo
echo ">> Finished command:"
echo ">> ================="


## temporary patch, to be fixed in tagUpload_trf.py
echo "ErrorCode=0 (OK)"

