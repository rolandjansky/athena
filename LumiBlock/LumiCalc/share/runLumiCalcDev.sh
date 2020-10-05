#!/bin/bash
export release=latest
export MyTestArea=/var/www/lumicalc_dev

echo `date`
echo Working directory is `pwd`
echo Setting up release $release

export ATLAS_LOCAL_ROOT_BASE=/cvmfs/atlas.cern.ch/repo/ATLASLocalRootBase
source ${ATLAS_LOCAL_ROOT_BASE}/user/atlasLocalSetup.sh ""

echo asetup --release=$release --testarea=$MyTestArea --branch=master Athena
asetup --release=$release --testarea=$MyTestArea --branch=master Athena

echo   source $MyTestArea/build/*/setup.sh
source $MyTestArea/build/*/setup.sh

echo which iLumiCalc: `which iLumiCalc`
echo Executing `which iLumiCalc` $*
`which iLumiCalc` $*

echo Done
echo `date`
