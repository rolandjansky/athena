#!/bin/bash

# art-description: Events processed by hardcoded chains with data requests, accepted randomly and sent to Full Event build, PEB and DS streams in athenaHLT
# art-type: build
# art-include: master/Athena

export NAME=CalibPeb
# use export ART_FILE_NAME=... to set the INPUT_FILE value 
export ART_CMD="athenaHLT.py -n 50 -f INPUT_FILE --ros2rob /cvmfs/atlas-nightlies.cern.ch/repo/data/data-art/TrigP1Test/ATLASros2rob2018-r22format.py TrigExPartialEB/MTCalibPeb.py"
export ART_TIMEOUT="20m"

exec_art_trigp1test.sh

