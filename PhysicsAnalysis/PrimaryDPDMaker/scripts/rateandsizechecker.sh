### Simple script for checking rates and sizes for DRAW and DESD(M) formats made at Tier-0
#
# Author: Christian Ohm (Sep 2017)
#
# Before running this, make sure to set up rucio and have a valid grid proxy:
# setupATLAS
# lsetup rucio
# voms-proxy-init


# find a good run, e.g. through a query like this:
# https://docs.google.com/spreadsheets/d/1Ctyt6CQeBejdvBN_w2Y4QttXgD5-p92R6vlmsM4S2ic/edit?usp=sharing

#runnumber=334960 # hard-coded

# take the run number from the CL argument
if [ "$#" -ne 1 ]; then
    echo "Please provide a run number as argument, i.e. 'rateandsizechecker.sh 334960'"
    exit 1
fi

runnumber=$1

echo "Will look up event counts and dataset sizes for run $runnumber"

# now get the datasets and save them to ds.txt
rucio ls --short --filter type=dataset "data17_13TeV.00${runnumber}.physics_Main.merge.AOD.*" | sort > ds.txt
rucio ls --short --filter type=dataset "data17_13TeV.00${runnumber}.physics_Main.*.D*.*" | grep -v DAOD | sort >> ds.txt

# now get the metadata for each of the samples and print the table
for format in `cat ds.txt | cut -d . -f5`; do ds=`cat ds.txt | grep $format`; events=`rucio get-metadata $ds | grep events | cut -d ':' -f2 | xargs`; size=`rucio get-metadata $ds | grep bytes | cut -d ':' -f2 | xargs`; echo "$format,$events,$size"; done 2>&1 | tee table.txt

# finally, paste the results into a spreadsheet like this:
# https://docs.google.com/spreadsheets/d/1Ctyt6CQeBejdvBN_w2Y4QttXgD5-p92R6vlmsM4S2ic/edit?usp=sharing

# Enjoy! /CO
