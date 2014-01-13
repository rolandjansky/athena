#!/bin/bash

###--------------------------------------------------------
### Check input options
###
if [ "$#" -ne "5" ]
then
   echo "Incorrect number of parameters: $#"; exit 1
fi

###--------------------------------------------------------
### Setup ATLAS release and define local variables
###
SETDIR=` dirname $1`
SCRIPT=`basename $1`

pushd `pwd`
cd $SETDIR
source $SCRIPT
popd

###--------------------------------------------------------
### Execute rates script
###
ntpPath=$2
outPath=$3

exePath=$TestArea/Trigger/TrigCost/TrigCostRate/share/./runRates.py

$exePath $ntpPath -o $outPath