#!/bin/sh

##########################

## script to get calibration file from DDM given a GUID

## Georges Aad <aad@cppm.in2p3.fr>

#########################

if [ $# = 0 ]; then
  echo "Usage: $0 GUID [option]"
  echo "options are:"
  echo "--dataset dataset where the file is stored - default oflcond.000001.conditions.recon.pool.v0000 - to know which one to use : http://atlas.web.cern.ch/Atlas/GROUPS/DATABASE/project/catrep/poolcond.html"
  echo "--help print this help message"
  exit
fi

if [ $1 = "--help" ]; then
  echo "Usage: $0 GUID [option]"
  echo "options are:"
  echo "--dataset dataset where the file is stored - default oflcond.000001.conditions.recon.pool.v0000 - to know which one to use : http://atlas.web.cern.ch/Atlas/GROUPS/DATABASE/project/catrep/poolcond.html"
  echo "--help print this help message"
  exit
fi

dataSetName=oflcond.000001.conditions.recon.pool.v0000
#dataSetName=cond08_mc.000001.gen.COND
guid=$1

iarg=1
args=($@)


for opt in $@
do

  if [ $opt = "--dataset" ]; then
    dataSetName=${args[$iarg]}
  fi

  (( iarg++ ))

done

logfile=log$$

dq2-ls -f $dataSetName >  $logfile

filename=`grep $guid $logfile | awk '{print $3}'`

echo file name: $filename
rm $logfile

dq2-get -f $filename $dataSetName 

