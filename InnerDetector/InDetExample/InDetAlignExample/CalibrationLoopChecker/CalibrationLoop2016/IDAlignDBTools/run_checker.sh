#!/bin/bash

RunNumber=$1

export run_mode=MANUAL
if [ $# -ge 2 ]; then
    export run_mode=$2
fi

export exec=""
if [ $# -ge 3 ]; then
    export exec=$3
fi

export IndetAlign_UPD1_TAG="InDetAlign-RUN2-ES1-UPD1-06"
export IndetAlign_UPD4_TAG="InDetAlign-RUN2-BLK-UPD4-16"
export TRTAlign_UPD1_TAG="TRTAlign-RUN2-ES1-UPD1-05"
export TRTAlign_UPD4_TAG="TRTAlign-RUN2-BLK-UPD4-13"
export IBLDist_UPD1_TAG="InDetIBLDist-RUN2-ES1-UPD1-01"
export IBLDist_UPD4_TAG="InDetIBLDist-RUN2-BLK-UPD4-01"

export HandshakeStream="calibration_BeamSpot"

workdir=/afs/cern.ch/user/a/atlidali/CalibrationLoop2016/IDAlignDBTools
if ! which AtlCoolCopy.exe 2>/dev/null 
then
    echo "Setting up offline envirnoment..."
    export AtlasSetup=/afs/cern.ch/atlas/software/dist/AtlasSetup
    source $AtlasSetup/scripts/asetup.sh --input ${workdir}/asetupConfig
    if ! which AtlCoolCopy.exe 2>/dev/null 
	then
	echo "Failed to get AtlCoolCopy.exe"
	exit -1
    fi
fi


./CalibrationLoopChecker.py ${RunNumber}
