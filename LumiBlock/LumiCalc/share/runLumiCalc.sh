#!/bin/bash
export release=20.7.8.6

echo `date`
echo Working directory is `pwd`
echo Setting up release $release

export MyTestArea=/var/www/lumicalc
export AtlasSetup=/afs/cern.ch/atlas/software/dist/AtlasSetup
#echo   $AtlasSetup/scripts/asetup.sh --help
#source $AtlasSetup/scripts/asetup.sh --help

echo   $AtlasSetup/scripts/asetup.sh --release=$release --testarea=$MyTestArea
source $AtlasSetup/scripts/asetup.sh --release=$release --testarea=$MyTestArea

chmod +x /var/www/lumicalc/LumiBlock/LumiCalc/x86_64-slc6-gcc49-opt/iLumiCalc
echo which iLumiCalc: `which iLumiCalc`
echo Executing /var/www/lumicalc/LumiBlock/LumiCalc/x86_64-slc6-gcc49-opt/iLumiCalc $*
/var/www/lumicalc/LumiBlock/LumiCalc/x86_64-slc6-gcc49-opt/iLumiCalc $*

echo Done
echo `date`
