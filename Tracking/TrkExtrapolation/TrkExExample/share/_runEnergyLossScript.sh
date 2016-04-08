#!/bin/bash

echo "  "
echo "  starting up _runEnergyLossScript.sh ..."
echo "  ----------"
echo "  this script runs several EnergyLossValidation jobs (TG + Extrapolator) in the batch queue"
echo "  ----------"
echo "  usage is:"
echo "      source _runEnergyLossScript.sh [athenarelease geomodel momentum pt maxevent runnumber extrapolator nocastor]"
echo "  with:         pt = p/pt (momentum choice)"
echo "      extrapolator = Atlas/Fatras"
echo "          nocastor = (if set to ANYthing, output files will NOT be written to Castor!)"
echo "          momentum = (all = 500, 5000, 50000, 1000, 10000, 100000 MeV)"
echo "  examples:"
echo "      source _runEnergyLossScript.sh"
echo "      source _runEnergyLossScript.sh 17.0.0"
echo "      source _runEnergyLossScript.sh dev,rel0 ATLAS-GEO-18-00-00"
echo "      source _runEnergyLossScript.sh dev,rel0 ATLAS-GEO-18-01-03 all "
echo "      source _runEnergyLossScript.sh dev,rel0 ATLAS-GEO-18-01-03 10000 p"
echo "      source _runEnergyLossScript.sh dev,rel0 ATLAS-GEO-18-01-03 10000 pt 100000"
echo "      source _runEnergyLossScript.sh dev,rel0 ATLAS-GEO-18-01-03 10000 pt 100000 1"
echo "      source _runEnergyLossScript.sh dev,rel0 ATLAS-GEO-18-01-03 10000 pt 100000 1 Atlas"
echo "      source _runEnergyLossScript.sh dev,rel0 ATLAS-GEO-18-01-03 10000 pt 100000 1 Atlas foo"
echo "  ----------"
echo "  "


###########################################
###   set up parameters

if test -z "$1"
then
  athenarelease="dev,rel0"
  echo "  athenarelease not given, using default value $athenarelease"
else
  athenarelease=$1
fi

if test -z "$2"
then
  geo="ATLAS-GEO-18-01-00"
  echo "  GEO model not given, using default value $geo"
else
  geo=$2
fi

if test -z "$3"
then
  momentum=10000
  echo "  momentum not given, using default value $momentum"
else
  momentum=$3
fi

if test -z "$4"
then
  pt="pt"
  echo "  pt choice not given, using default value $pt"
else
  pt=$4
fi

if test -z "$5"
then
  maxevent=100000
  echo "  maxevent not given, using default value $maxevent"
else
  maxevent=$5
fi

if test -z "$6"
then
  runnumber=1
  echo "  runnumber not given, using default value $runnumber"
else
  runnumber=$6
fi

if test -z "$7"
then
  extrapolator="Atlas"
  echo "  extrapolator not given, using default value $extrapolator"
elif [ $7=="Fatras" ]
then
  extrapolator=$7
else
  extrapolator="Atlas"
fi

if test -z "$8"
then
  nocastor=""
  echo "  nocastor not set - copy output files to castor: YES"
else
  nocastor=$8
  echo "  nocastor set as '$8' - copy output files to castor: NO"
fi


###########################################
###   print parameters

echo "  --------  set parameters  --------"
echo "  athenarelease = $athenarelease"
echo "      GEO model = $geo"
echo "       momentum = $momentum"
echo "      pt choice = $pt"
echo "       maxevent = $maxevent"
echo "      runnumber = $runnumber"
echo "   extrapolator = $extrapolator"
echo "       nocastor = $nocastor"
echo "  --------  set parameters  --------"


###########################################
###   set file permissions for batch

test -x _energyLossScript.sh || chmod u+x _energyLossScript.sh


###########################################
###   run the scripts

qs=10000
if [ "$maxevent" -gt "$qs" ]
then
  queue="atlascatlong"    # or e.g. 8nh
else
  queue="atlascatshort"   # or e.g. 1nh
fi


if [ "$momentum" == "all" ]
then
  echo "  now executing:"
  for mom in 100000 10000 1000 50000 5000 500 ; do
    echo "      bsub -q $queue _energyLossScript.sh $athenarelease $geo $mom $pt $maxevent $runnumber $extrapolator $nocastor"
    bsub -q $queue _energyLossScript.sh $athenarelease $geo $mom $pt $maxevent $runnumber $extrapolator $nocastor
  done
else
  echo "  now executing:"
  echo "      bsub -q $queue _energyLossScript.sh $athenarelease $geo $momentum $pt $maxevent $runnumber $extrapolator $nocastor"
  bsub -q $queue _energyLossScript.sh $athenarelease $geo $momentum $pt $maxevent $runnumber $extrapolator $nocastor
fi


###########################################
###   finish

echo
echo "  finished _runEnergyLossScript.sh ... all done!"
echo "  kthxbai."
echo
