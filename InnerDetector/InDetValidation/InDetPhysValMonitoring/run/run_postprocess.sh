#!/usr/bin/env bash

#  Copyright (C) 2020 CERN for the benefit of the ATLAS collaboration

# this is the 'old' postprocessing script, updated to use the compiled 
# executable for backward compatibility. 

function printhelp(){
       echo "
       Script summs InDetPhysValMonitorinig outputs from independent runs.

       run command:
       $0 fileout.root [file1.root file2.root file3.root ...]

       where:
  
       fileout.root
       ...... output file containing summed histograms,efficiencies and profiles
       ...... If the outuput file exists, we'll assume you obtained if from hadd 
       ......      (eg grid doing hadd for you) and only run postprocessing 

       [file1.root file2.root file3.root ...]
       ...... files to summ
       ...... obtained prior to running the script from 
       ......       independent (eg grid) runs
  
       "
}

# mandatory arguments
NMANDARGS=1

if [ -z $ROOTSYS ]; then
    echo "Please set up ROOT to run"
    exit 1
fi 

if [[ "$#" < "$NMANDARGS" ]]; then
    echo "ERROR : too few arguments. Provided / expected : " $# " / " $NMANDARGS 
    echo "print help and exit"
    printhelp    
    exit 1;
fi    

# no hadd options support (eg -f etc); keep it simple
if [[ "$#" > "$NMANDARGS" ]]; then
    echo "---------------------------------------------------------------------------------------"
    echo "$0 : 1st step: hadd-ing histograms"
    echo "---------------------------------------------------------------------------------------"
    hadd $@
    if [[ "0" != "$?" ]]; then
	echo "$0 : hadd failed. No postprocessing run."
	exit 1;
    fi
fi
echo "---------------------------------------------------------------------------------------"
echo "$0 : 2rd step: running postprocessing"
echo "---------------------------------------------------------------------------------------"
postProcessIDPVMHistos $1
if [[ "0" != "$?" ]]; then
    echo "$0 : postProcessIDPVMHistos failed for file $1"
    exit 1;
fi

echo "---------------------------------------------------------------------------------------"
echo "$0: all done"
echo "---------------------------------------------------------------------------------------"
exit 0;
