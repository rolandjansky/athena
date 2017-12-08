#!/bin/bash

echo "  "
echo "  starting up _energyLossScript.sh ..."
echo "  ----------"
echo "  this script runs an EnergyLossValidation job (TG + Extrapolator)"
echo "  ----------"
echo "  usage is:"
echo "      source _energyLossScript.sh [athenarelease geomodel momentum pt maxevent runnumber extrapolator nocastor]"
echo "  examples:"
echo "      source _energyLossScript.sh"
echo "      source _energyLossScript.sh 17.0.0"
echo "      source _energyLossScript.sh dev,rel0 ATLAS-GEO-18-00-00"
echo "      source _energyLossScript.sh dev,rel0 ATLAS-GEO-18-01-03 5000"
echo "      source _energyLossScript.sh dev,rel0 ATLAS-GEO-18-01-03 10000 p"
echo "      source _energyLossScript.sh dev,rel0 ATLAS-GEO-18-01-03 10000 pt 100000"
echo "      source _energyLossScript.sh dev,rel0 ATLAS-GEO-18-01-03 10000 pt 100000 1"
echo "      source _energyLossScript.sh dev,rel0 ATLAS-GEO-18-01-03 10000 pt 100000 1 Atlas"
echo "      source _energyLossScript.sh dev,rel0 ATLAS-GEO-18-01-03 10000 pt 100000 1 Atlas foo"
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
  geo="ATLAS-GEO-18-01-03"
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
  echo "  pt choice not given, using default $pt"
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
  runnumber=0
  echo "  runnumber not given, using default value $runnumber"
else
  runnumber=$6
fi

if test -z "$7"
then
  extrapolator="Atlas"
  echo "  extrapolator not given, using default value $extrapolator"
elif [ $7=='Fatras' ]
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
###   create random seed

rndgen="/afs/cern.ch/user/w/wlukas/bin/gen_rnd.py"
if test -e $rndgen
then
  thisdate=$(date +%s)
  echo "  thisdate=$thisdate"
  rnd0=$(python $rndgen $runnumber $thisdate)
  rnd1=$(python $rndgen $momentum $maxevent $runnumber $athenarelease $geo)
  rnd2=$(python $rndgen $momentum $maxevent $runnumber $thisdate)
else
  rnd0=$thisdate
  rnd1="12345678"
  rnd2=$thisdate
fi

#rnd0="0"
#rnd1="12345678"
#rnd2="87654321"


###########################################
###   print parameters

echo "  -------- set parameters --------"
echo "  athenarelease = $athenarelease"
echo "      GEO model = $geo"
echo "       momentum = $momentum"
echo "      pt choice = $pt"
echo "       maxevent = $maxevent"
echo "      runnumber = $runnumber"
echo "   extrapolator = $extrapolator"
echo "       nocastor = $nocastor"
echo "  random offset = $rnd0"
echo "  random seed 1 = $rnd1"
echo "  random seed 2 = $rnd2"
echo "  -------- set parameters --------"


###########################################
###   make a tempdir on /pool/spool/

here=$PWD

if test -e /pool/spool/
then
  TMPDIR=/pool/spool/wlukas
fi
mkdir -p $TMPDIR
tempdir=$(mktemp -d -p $TMPDIR)
echo "  "
echo "  tempdir=$tempdir"
cd $tempdir
echo "  PWD=$PWD"
echo "  df $PWD :"
df $PWD
echo "  "


###########################################
###   set up athena in correct folder

cd ${HOME}
#cd /afs/cern.ch/user/w/wlukas/
source .bashrc
shopt -s expand_aliases
testdir="testarea/${athenarelease/,/_}"
#testdir="testarea/test"
cd $testdir
asetup AtlasProduction,$athenarelease,opt,here,runtime

cd $tempdir
echo "  ... now in directory $PWD"

jobpath="${HOME}/$testdir/Tracking/TrkExtrapolation/TrkExExample/share"
ln -s $jobpath/EnergyLossValidation_jobOptions.py .
#ln -s $here/EnergyLossValidation_jobOptions.py .

#cmt show path


###########################################
###   run the job

echo "  ... athena job started with momentum=$momentum MeV"

athena.py -c "myMomentum=$momentum; myMaxEvent=$maxevent; myRunNumber=$runnumber; myRandomSeed1=$rnd1; myRandomSeed2=$rnd2; myPt='$pt'; myExtrapolator='$extrapolator'; myGeo='$geo'" $jobpath/EnergyLossValidation_jobOptions.py 1>log.log 2>&1

echo "  ... athena job done with momentum=$momentum MeV"


###########################################
###   copy to castor

geo=${geo%_VALIDATION}

if [ -z "$nocastor" ]
then
  eosbase="root://eosatlas//eos/atlas/user/w/wlukas"
  eosdir="$eosbase/Cylinder"

  if test -z "$eosdir"
  then
    nsmkdir -p $eosdir
  fi

  if test -z "$eosdir/$geo/"
  then
    nsmkdir -p $eosdir/$geo/
  fi

  if test -z "$eosdir/$geo/logs/"
  then
    nsmkdir -p $eosdir/$geo/logs/
  fi

  eospath="$eosdir/$geo"

  option="r16r17."
  prefix=$extrapolator

  inputfile=$prefix.$geo.$pt.$momentum.MeV.$maxevent.$runnumber.root
  outputfile=$prefix.${athenarelease/,/_}.$geo.$option$pt.$momentum.MeV.$maxevent.$runnumber.root

  xrdcp $inputfile $eospath/$outputfile
  echo "  ... ROOT file copied to $eospath/$outputfile"

  logdest="log.$prefix.${athenarelease/,/_}.$geo.$option$pt.$momentum.MeV.$maxevent.$runnumber.log"
  xrdcp log.log $eospath/logs/$logdest
  echo "  ... logfile copied to $eospath/logs/$logdest"
fi

#cp log.log ${HOME}/$logdest


###########################################
###   clean up tempdir

#echo "  tempdir=$tempdir"
#echo "  deleting tempdir ..."
#rm -r $tempdir


###########################################
###   finish

echo "  "
echo "  finished _energyLossScript.sh ... all done!"
echo "  kthxbai."
echo "  "

