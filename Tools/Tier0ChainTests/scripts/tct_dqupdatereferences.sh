#!/bin/sh

## Check arguments
if [ $# -lt 4 ] ; then
   echo "Usage: tct_dqupdatereferences.sh <inputhcfg> <newreffile> <outputhcfg> <runnumber>"
   exit 1 
fi

## Intercept dryrun option if given
DRYRUN=0 
if [ "$1" = "-d" ] ; then
  DRYRUN=1
  shift
fi

## set configuration parameters
inputhcfg=$1
newrefpath=$2
outputhcfg=$3
runnumber=$(echo $4 | bc)
if [ ! -f $inputhcfg ]; then
  echo "ERROR: input han confuguration file <$inputhcfg> not found. Exit."
  exit 1
fi
newreffile=`echo $newrefpath | cut -d ":" -f 1`
if [ ! -f $newreffile ]; then
  echo "ERROR: input han results file <$newreffile> not found. Exit."
  exit 1
fi
toppath=`echo $newrefpath | cut -d ":" -f 2`
if [ ! $toppath = $newreffile ]; then
  if [ ! $toppath = run_${runnumber} ]; then
    echo "ERROR: toppath <$toppath> and given runnumber <$runnumber> do not match. Exit."
    exit 1
  fi
fi 


## first, update han references
RUNCMDA="tct_changehanreferences.py ${inputhcfg} ${newreffile}:run_${runnumber} intermediate.hcfg --forceref"
echo
echo ">> Now running command:"
echo ">> ===================="
echo "$RUNCMDA"
echo 
if [ $DRYRUN -ne 1 ]; then
  #source /afs/cern.ch/atlas/project/tdaq/cmt/bin/cmtsetup.sh dqm-common-00-03-02
  #cvs co -P -r DQConfMaker-01-00-01 common/DQConfMaker
  #export PYTHONPATH=$PYTHONPATH:$PWD/common/DQConfMaker/python
  ## fix this - put DQConfMaker in release
  export PYTHONPATH=$PYTHONPATH:/afs/cern.ch/atlas/offline/external/FullChainTest/tier0-vol3/rtt/fallbackfiles/DQConfMaker/python
  $RUNCMDA
  #rm -Rf common
fi
echo
echo ">> Finished command:"
echo ">> ================="
echo "$RUNCMDA"
echo 


## second, update the han algorithms
RUNCMDB="tct_changehanalgorithms.py intermediate.hcfg Tier0ChainTests.newalg_bincontentcomp ${outputhcfg}"
echo
echo ">> Now running command:"
echo ">> ===================="
echo "$RUNCMDB"
echo 
if [ $DRYRUN -ne 1 ]; then
  $RUNCMDB
fi
echo
echo ">> Finished command:"
echo ">> ================="
echo "$RUNCMDB"
echo 


## this should only be reached if all successful ;-)
echo "ErrorCode=0 (OK)"

