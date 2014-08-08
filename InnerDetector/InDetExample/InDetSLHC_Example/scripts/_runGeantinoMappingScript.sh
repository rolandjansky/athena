#!/bin/bash

echo "  "
echo "  starting up _runGeantinoMappingScript.sh ..."
echo "  ----------"
echo "  this script runs several GeantinoMapping jobs in the batch queue (via _geantinoMappingScript.sh)"
echo "  ----------"
echo "  usage is:"
echo "      source _runGeantinoMappingScript.sh [momentum maxevent low high athenarelease pt geomodel]"
echo "  with low/high = lowest/highest runnumber"
echo "         and pt = p/pt momentum choice"
echo "  examples:"
echo "      source _runGeantinoMappingScript.sh"
echo "      source _runGeantinoMappingScript.sh 10000"
echo "      source _runGeantinoMappingScript.sh 10000 10000"
echo "      source _runGeantinoMappingScript.sh 10000 10000 1 10"
echo "      source _runGeantinoMappingScript.sh 10000 10000 1 10 16.7.0"
echo "      source _runGeantinoMappingScript.sh 10000 10000 1 10 16.7.0 p"
echo "      source _runGeantinoMappingScript.sh 10000 10000 1 10 16.7.0 pt ATLAS-GEO-18-00-00"
echo "  ----------"
echo "  "


###########################################
###   set up parameters

if test -z "$1"
then
  momentum=10000
  echo "  momentum not given ... using momentum=$momentum"
else
  momentum=$1
fi

if test -z "$2"
then
  maxevent=10000
  echo "  maxevent not given ... using maxevent=$maxevent"
else
  maxevent=$2
fi

if test -z "$3"
then
  low=0
  echo "  lowest runnumber not given ... using low=$low"
else
  low=$3
fi

if test -z "$4"
then
  high=0
  echo "  highest runnumber not given ... using high=$high"
else
  high=$4
fi

if test -z "$5"
then
  athenarelease="16.7.0"
  echo "  athenarelease not given ... using default athenarelease=$athenarelease"
else
  athenarelease=$5
fi

if test -z "$6"
then
  pt="pt"
  echo "  pt choice not given, using default $pt"
else
  pt=$6
fi

if test -z "$7"
then
  geo="ATLAS-GEO-18-00-00"
  echo "  GEO model not given, using default $geo"
else
  geo=$7
fi


###########################################
###   print parameters

echo "  --------  set parameters  --------"
echo "       momentum = $momentum"
echo "       maxevent = $maxevent"
echo "      runnumber = [$low-$high]"
echo "  athenarelease = $athenarelease"
echo "      pt choice = $pt"
echo "      GEO model = $geo"
echo "  --------  set parameters  --------"


############################
###   run the jobs

qs=1000
if [ "$maxevent" -gt "$qs" ]
then
  queue="8nh"    # or e.g. 8nh
else
  queue="1nh"   # or e.g. 1nh
fi

for number in `seq $low $high`
do
  bsub -q $queue _geantinoMappingScript.sh $momentum $maxevent $number $athenarelease $pt $geo
done


###########################################
###   finish

echo "  "
echo "  finished _runGeantinoMappingScript.sh ... all jobs are submitted!"
echo "  kthxbai."
echo "  "

