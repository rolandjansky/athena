#!/bin/bash
export release=19.2.0

echo `date`
echo Working directory is `pwd`
echo Setting up release $release

export MyTestArea=/var/www/lumicalc
export AtlasSetup=/afs/cern.ch/atlas/software/dist/AtlasSetup
#echo   $AtlasSetup/scripts/asetup.sh --help
#source $AtlasSetup/scripts/asetup.sh --help

echo   $AtlasSetup/scripts/asetup.sh --release=$release --testarea=$MyTestArea
source $AtlasSetup/scripts/asetup.sh --release=$release --testarea=$MyTestArea

echo which iLumiCalc.exe: `which iLumiCalc.exe`

echo Executing iLumiCalc.exe $*
iLumiCalc.exe $*

echo Done
echo `date`
