#! /usr/bin/env bash

DCSC2_SYSTEMS="-sMDT -sTile -sTGC -sRPC -sTDQ -sCSC -sMagnets -sGlobal -sPixels -sTRT -sSCT -sLAr -sLucid"
#DCSC2_SYSTEMS="-sMDT -sTile -sTGC -sRPC -sTDQ -sCSC -sMagnets -sGlobal -sPixels -sTRT -sLAr -sLucid"
#DEST_DB=COOLOFL_GLOBAL/COMP200
DEST_DB=COOLOFL_GLOBAL/CONDBR2

RUN=$1
shift

if [ -z $RUN ]; then 
    echo "Usage: ./ExecuteDCSC2.sh [run_number]"
    exit 1
fi

echo "Running for $RUN"

export AtlasSetup=/afs/cern.ch/atlas/software/dist/AtlasSetup
pushd /afs/cern.ch/user/a/atlasdqm/ws/DCSCalc/prodarea > /dev/null
#source $AtlasSetup/scripts/asetup.sh 17.2.8.6,slc5
source $AtlasSetup/scripts/asetup.sh 20.7.8.1

CORAL_AUTH_PATH=/afs/cern.ch/user/a/atlasdqm/private
CORAL_DBLOOKUP_PATH=/afs/cern.ch/user/a/atlasdqm/private

dcsc.py $@ $DCSC2_SYSTEMS -r$RUN -o$DEST_DB --email-on-failure
