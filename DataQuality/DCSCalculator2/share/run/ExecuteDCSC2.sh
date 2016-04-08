#! /usr/bin/env bash

DCSC2_SYSTEMS="-sMDT -sTile -sTGC -sRPC -sTDQ -sCSC"
DEST_DB=COOLOFL_GLOBAL/COMP200
#DEST_DB=deleteme.db

LOG_PATH=/afs/cern.ch/user/a/atlasdqm/DQCalculators/DCSCalc/RunDCSCalc/logfiles
RUN=$1
shift

if [ -z $RUN ]; then 
    echo "Usage: ./RunDCSC2.sh [run_number]"
    exit
fi

echo "Running for $RUN"

AtlasSetup=/afs/cern.ch/atlas/software/dist/AtlasSetup
asetup=$AtlasSetup/scripts/asetup.sh
pushd /afs/cern.ch/user/a/atlasdqm/DQCalculators/DCSCalc/prodarea > /dev/null
source $asetup 16.0.0
#15.6.12


CORAL_AUTH_PATH=/afs/cern.ch/user/a/atlasdqm/private
CORAL_DBLOOKUP_PATH=/afs/cern.ch/user/a/atlasdqm/private

dcsc.py -v $@ $DCSC2_SYSTEMS -r$RUN -o$DEST_DB
echo "Run $RUN" | mail -s "New DCSC ran on run $RUN." pwaller
