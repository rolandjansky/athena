#!/bin/bash

echo "  "
echo "  starting up _geantinoMappingScript.sh ..."
echo "  ----------"
echo "  this script runs a GeantinoMapping job (G4)"
echo "  ----------"
echo "  usage is:"
echo "      source _geantinoMappingScript.sh [momentum maxevent runnumber athenarelease pt geomodel]"
echo "  examples:"
echo "      source _geantinoMappingScript.sh"
echo "      source _geantinoMappingScript.sh 10000"
echo "      source _geantinoMappingScript.sh 10000 10000 1"
echo "      source _geantinoMappingScript.sh 10000 10000 1 16.7.0"
echo "      source _geantinoMappingScript.sh 10000 10000 1 16.7.0 p"
echo "      source _geantinoMappingScript.sh 10000 10000 1 16.7.0 pt ATLAS-GEO-18-00-00"
echo "  ----------"
echo "  "


###########################################
###   set up parameters

if test -z "$1"
then
  momentum=10000
  echo "  momentum not given, using default value $momentum"
else
  momentum=$1
fi

if test -z "$2"
then
  maxevent=10000
  echo "  maxevent not given, using default value $maxevent"
else
  maxevent=$2
fi

if test -z "$3"
then
  runnumber=0
  echo "  runnumber not given, using default value $runnumber"
else
  runnumber=$3
fi

if test -z "$4"
then
  athenarelease="16.7.0"
  echo "  athenarelease not given, using default value $athenarelease"
else
  athenarelease=$4
fi

if test -z "$5"
then
  pt="pt"
  echo "  pt choice not given, using default $pt"
else
  pt=$5
fi

if test -z "$6"
then
  geo="ATLAS-GEO-18-00-00"
  echo "  GEO model not given, using default value $geo"
else
  geo=$6
fi


###########################################
###   create random seed


thisdate=$(date +%s)
echo "  thisdate=$thisdate"
rndgen="/afs/cern.ch/user/w/wlukas/bin/gen_rnd.py"
if test -e $rndgen
then
  rndnumber=$(python $rndgen $momentum $maxevent $runnumber $athenarelease $geo)
  rndnumber2=$(python $rndgen $momentum $maxevent $runnumber $thisdate)
else
  rndnumber=`expr $runnumber \* 1000000 \\+ 678910`
  rndnumber2=`expr $thisdate \\+ $runnumber`
fi

#rndnumber="12345678"
#rndnumber2="87654321"


###########################################
###   print parameters

echo "  --------  set parameters  --------"
echo "       momentum = $momentum"
echo "       maxevent = $maxevent"
echo "      runnumber = $runnumber"
echo "  athenarelease = $athenarelease"
echo "  random seed 1 = $rndnumber"
echo "  random seed 2 = $rndnumber2"
echo "      pt choice = $pt"
echo "      GEO model = $geo"
echo "  --------  set parameters  --------"


###########################################
###   make a tempdir on /pool/spool/

if test -e /pool/spool/
then
  TMPDIR=/pool/spool/pvankov
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
# testdir="testarea/${athenarelease/,/_}"
testdir="/afs/cern.ch/user/p/pvankov/workspace/testarea_W/17.3.4.2_new"
cd $testdir
alias asetup='source $AtlasSetup/scripts/asetup.sh'
#asetup AtlasProduction,$athenarelease,rel_1,opt,here,runtime
asetup AtlasProduction,$athenarelease,opt,here,runtime


cd $tempdir
echo "  ... now in directory $PWD"

jobpath="$testdir/InnerDetector/InDetExample/InDetSLHC_Example/share"
ln -s $jobpath/GeantinoMapping_jobOptionsOverride.py .
#ln -s $jobpath/GeantinoMapping_jobOptions.py .
source setupLocalDBReplica_CERN.sh

###############################################################################
echo Setting up geomDB, triggerDB and sqlite200 links in current directory to use
echo CERN afs SQLite replicas of geometry, conditions and trigger databases
if [ ! -e geomDB ]
then
  # ln -s /afs/cern.ch/atlas/project/database/DBREL/packaging/DBRelease/current/geomDB $jobpath/geomDB
  ln -s /afs/cern.ch/atlas/project/database/DBREL/packaging/DBRelease/current/geomDB .
fi
if [ ! -e triggerDB ]
then
  # ln -s /afs/cern.ch/atlas/project/database/DBREL/packaging/DBRelease/current/triggerDB $jobpath/triggerDB
  ln -s /afs/cern.ch/atlas/project/database/DBREL/packaging/DBRelease/current/triggerDB .
fi
if [ ! -e sqlite200 ]
then
  if [ -n "$1" -a "$1" = "COMP200" ]
  then
    echo Special setup for COMP200 replica
    mkdir sqlite200
    # ln -s /afs/cern.ch/user/a/atlcond/coolrep/sqlite200/COMP200.db $jobpath/sqlite200/ALLP200.db
    ln -s /afs/cern.ch/user/a/atlcond/coolrep/sqlite200/COMP200.db sqlite200/ALLP200.db
  else
    # ln -s /afs/cern.ch/user/a/atlcond/coolrep/sqlite200 $jobpath/sqlite200
    ln -s /afs/cern.ch/user/a/atlcond/coolrep/sqlite200 .
  fi
fi
###############################################################################

ls -l
cmt show path


###########################################
###   run the job

echo "  ... athena job started with momentum=$momentum MeV"

logfile="log.$runnumber.log"
athena.py -c "myMomentum=$momentum; myMaxEvent=$maxevent; myRandomSeed=$rndnumber; myRandomSeed2=$rndnumber2; myPt='$pt'; myGeo='$geo'" GeantinoMapping_jobOptionsOverride.py 1>$logfile 2>&1
#athena.py -c "myMomentum=$momentum; myMaxEvent=$maxevent; myRandomSeed=$rndnumber; myRandomSeed2=$rndnumber2; myPt='$pt'; myGeo='$geo'" GeantinoMapping_jobOptions.py 1>$logfile 2>&1
#athena.py -c "myMomentum=$momentum; myMaxEvent=$maxevent; myRandomSeed=$rndnumber; myRandomSeed2=$rndnumber2; myPt='$pt'; myGeo='$geo'" GeantinoMapping_jobOptions.py 2>&1 | tee $logfile

echo "  ... athena job done with momentum=$momentum MeV"


###########################################
###   copy to castor

#castordir=$CASTOR_HOME/Geantinos
# castordir="/eos/atlas/atlasscratchdisk/user/pvankov/TrkGeometry"
castordir="/eos/atlas/user/p/pvankov/TrkGeometry"

if test -z "$castordir"
then
  eos mkdir $castordir
fi

if test -z "$castordir/$geo"
then
  # nsmkdir -p $castordir/$geo
  eos mkdir $castordir/$geo
fi

if test -z "$castordir/$geo/logs/"
then
  eos mkdir $castordir/$geo/logs/
fi

caspath="root://eosatlas/$castordir/$geo"


prefix="MaterStepFile"
suffix="root"

inputfile=$prefix.$suffix
outputfile=$prefix.$geo.$maxevent.$runnumber.$suffix

xrdcp $inputfile $caspath/$outputfile
echo "  ... $inputfile copied to $caspath/$outputfile"


prefix2="Hits"
suffix2="pool.root"

inputfile2=$prefix2.$suffix2
outputfile2=$prefix2.$geo.$maxevent.$runnumber.$suffix2

xrdcp $inputfile2 $caspath/$outputfile2
echo "  ... $inputfile2 copied to $caspath/$outputfile2"


logdest=log.$geo.$maxevent.$runnumber.log

xrdcp $logfile $caspath/logs/$logdest
echo "  ... $logfile copied to $caspath/logs/$logdest"


###########################################
###   finish

echo "  "
echo "  finished _geantinoMappingScript.sh ... all done!"
echo "  kthxbai."
echo "  "

