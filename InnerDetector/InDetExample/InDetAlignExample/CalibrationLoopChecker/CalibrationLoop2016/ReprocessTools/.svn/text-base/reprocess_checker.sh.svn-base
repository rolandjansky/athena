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

export do_upload_dynamic=0
export do_upload_static=0
export do_upload_reprocess=1

export L11_CRITERIA="TIGHT"
#export L11_CRITERIA="LOOSE"

#export DB_FOR_UPLOAD="Iter1"

# tags for reprocessing. used when do_upload_reprocess==1
export IndetAlignL1ID_UPD3_TAG="IndetAlignL1ID-Repro-Rel21-UPD3-00"
export IndetAlignL2PIX_UPD3_TAG="IndetAlignL2PIX-Repro-Rel21-UPD3-00"
export IndetAlignL2SCT_UPD3_TAG="IndetAlignL2SCT-Repro-Rel21-UPD3-00"
export IndetIBLDist_UPD3_TAG="IndetIBLDist-Repro-Rel21-UPD3-00"
export TRTAlignL1_UPD3_TAG="TRTAlignL1-Repro-Rel21-UPD3-00"

export DbComment="For reprocessing for Rel21 in November 2016"

export HandshakeStream="calibration_BeamSpot"

#export DEBUG=TRUE
if [ "$exec" == "--exec" ]; then
    export SEND_EMAIL="TRUE"
fi
#export IOVSinceRunNumber=`cat runlist_2016_preTS1.txt runlist_2016_postTS1.txt | grep -B1 ${RunNumber} | head -n1`
export IOVUntilRunNumber=`cat runlist_2015.txt runlist_2016_preTS1.txt runlist_2016_postTS1.txt | grep -A1 ${RunNumber} | tail -n1`

workdir=/afs/cern.ch/user/a/atlidali/CalibrationLoop2016/DynamicIDAlignDBTools
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
