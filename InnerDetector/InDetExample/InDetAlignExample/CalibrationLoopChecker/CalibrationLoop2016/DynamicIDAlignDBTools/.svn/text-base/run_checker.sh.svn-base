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

export do_upload_dynamic=1
export do_upload_static=1
export do_upload_reprocess=0

# tags for dynamic UPD1. used when do_upload_dynamic==1
export IndetAlignL1ID_UPD1_TAG="IndetAlignL1ID-RUN2-ES1-UPD1-02"
export IndetAlignL2PIX_UPD1_TAG="IndetAlignL2PIX-RUN2-ES1-UPD1-02"
export IndetAlignL2SCT_UPD1_TAG="IndetAlignL2SCT-RUN2-ES1-UPD1-02"
export IndetAlignL3_UPD1_TAG="IndetAlignL3-RUN2-ES1-UPD1-02"
export IndetIBLDist_UPD1_TAG="InDetIBLDist-RUN2-ES1-UPD1-01"
export TRTAlignL1_UPD1_TAG="TRTAlignL1-RUN2-ES1-UPD1-02"
export TRTAlignL2_UPD1_TAG="TRTAlignL2-RUN2-ES1-UPD1-02"

# tags for dynamic UPD4.  used when do_upload_dynamic==1
export IndetAlignL1ID_UPD4_TAG="IndetAlignL1ID-RUN2-BLK-UPD4-02"
export IndetAlignL2PIX_UPD4_TAG="IndetAlignL2PIX-RUN2-BLK-UPD4-02"
export IndetAlignL2SCT_UPD4_TAG="IndetAlignL2SCT-RUN2-BLK-UPD4-02"
export IndetAlignL3_UPD4_TAG="IndetAlignL3-RUN2-BLK-UPD4-02"
export IndetIBLDist_UPD4_TAG="InDetIBLDist-RUN2-BLK-UPD4-01"
export TRTAlignL1_UPD4_TAG="TRTAlignL1-RUN2-BLK-UPD4-02"
export TRTAlignL2_UPD4_TAG="TRTAlignL2-RUN2-BLK-UPD4-02"

# tags for new->old conversion. used when do_upload_static==1
export IndetAlign_UPD1_TAG="InDetAlign-RUN2-ES1-UPD1-06"
export IndetAlign_UPD4_TAG="InDetAlign-RUN2-BLK-UPD4-16"
export TRTAlign_UPD1_TAG="TRTAlign-RUN2-ES1-UPD1-05"
export TRTAlign_UPD4_TAG="TRTAlign-RUN2-BLK-UPD4-13"

# tags for reprocessing. used when do_upload_reprocess==1
export IndetAlignL1ID_UPD3_TAG="IndetAlignL1ID-RUN2-BLK-UPD3-TEST-00"
export IndetAlignL2PIX_UPD3_TAG="IndetAlignL2PIX-RUN2-BLK-UPD3-TEST-00"
export IndetAlignL2SCT_UPD3_TAG="IndetAlignL2SCT-RUN2-BLK-UPD3-TEST-00"
export IndetAlignL3_UPD3_TAG="IndetAlignL3-RUN2-BLK-UPD3-TEST-00"
export IndetIBLDist_UPD3_TAG="IndetIBLDist-RUN2-BLK-UPD3-TEST-00"
export TRTAlignL1_UPD3_TAG="TRTAlignL1-RUN2-BLK-UPD3-TEST-00"


export HandshakeStream="calibration_BeamSpot"

#export DEBUG="TRUE"

export L11_CRITERIA="LOOSE"
#export L11_CRITERIA="TIGHT"

if [ "$exec" == "--exec" ]; then
    export SEND_EMAIL="TRUE"
fi

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
