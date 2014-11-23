#!/bin/bash

echo `date`
echo Working directory is `pwd`
echo Setting up release 16.6.6

# Must fake setting up a release here
export MyTestArea=/var/vhost/atlas-lumicalc/secure/deve

export AtlasSetup=/afs/cern.ch/atlas/software/dist/AtlasSetup
echo   $AtlasSetup/scripts/asetup.sh 16.6.6 --testarea $MyTestArea
source $AtlasSetup/scripts/asetup.sh 16.6.6 --testarea $MyTestArea

export TNS_ADMIN=/afs/cern.ch/sw/lcg/external/oracle/10.2.0.4p1/slc4_ia32_gcc34/../admin
export HOSTNAME=$HOSTNAME

export NLS_LANG=AMERICAN_AMERICA.WE8ISO8859P1

export COOL_DISABLE_CORALCONNECTIONPOOLCLEANUP=YES

echo Executing iLumiCalc.exe $*

# Change following line to match actual location
$MyTestArea/LumiBlock/LumiBlockComps/i686-slc5-gcc43-opt/iLumiCalc.exe $*
