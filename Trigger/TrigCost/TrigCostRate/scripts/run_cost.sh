#!/bin/bash

###--------------------------------------------------------
### print castor debuging information
print_debug()
{
  echo "@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@"
  # switch on debugging and switch to different disk
  export RFIO_TRACE=3     # zsh / bash syntax
  export STAGER_TRACE=3   # zsh / bash syntax
  export STAGE_SVCCLASS=atlasgroupdisk

  # get details about the settings
  hostname
  date
  uname -a
  id
  env | grep RFIO
  env | grep STAGE
  env | grep PATH
  rpm -qa| grep castor
  which rfcp
  castor -v
  echo "@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@"
}

###--------------------------------------------------------
### Check input options
if [ "$#" -ne "5" ]
then
   echo "Incorrect number of parameters: $#"; exit 1
fi

###--------------------------------------------------------
### Setup ATLAS release and define local variables
SETDIR=` dirname $1`
SCRIPT=`basename $1`

pushd `pwd`
cd $SETDIR
source $SCRIPT
popd

###--------------------------------------------------------
### Run job

#print_debug
mkdir -p $3
cd $3

#make root file name
if [ $5 -eq "1" ]
then
    run_="$SETDIR/Trigger/TrigCost/TrigCostRate/share/runRates.py $2 -o $3 -n $4 --data --do-time -k COSTMON -r cost.root"
else
    run_="$SETDIR/Trigger/TrigCost/TrigCostRate/share/runRates.py $2 -o $3 -n $4 --data --do-time -k COSTMON"
fi

echo "Starting job at "`date`
echo "Working directory: "`pwd`
echo "Executing command: $run_"
echo "-------------------------------------------------------------------------------------------------------"

$run_

echo "-------------------------------------------------------------------------------------------------------"
echo "Finished job at "`date`
