#!/bin/tcsh
echo Will first source setup.csh in CMTWORK area
#source ${CMTWORK}/TestRelease/*/cmt/setup.csh
source ${NIGHTLYAREA}/TestRelease/*/cmt/setup.csh
echo ----------------------------------------------
echo This assumes that checkImplicit.sh has already 
echo   been run to create the inputs !!!
echo ----------------------------------------------
echo About to run EventSplit_jo with output in logsplit2
athena.py EventSplitCollections_jo.py >! logexp1
echo About to run CheckImplicit_jo with output in logsplit3
athena.py CheckExplicit_jo.py >! logexp2
athena.py CheckExplicitMySQL_jo.py >> logexp2

echo Listing files
ls -lrt Collection*.root >! logfiles
