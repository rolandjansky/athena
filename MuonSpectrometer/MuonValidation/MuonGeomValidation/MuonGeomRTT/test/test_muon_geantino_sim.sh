#!/bin/sh
#
# art-description: Art test job for MuonGeomRTT package, migrated from RTT job: MuonGeantinoSim
# # Purpose: Check internal consistency of raw geometry (fed to G4 simulation)
# 
# art-type: grid
# art-include: master/Athena
# art-include: 21.0/Athena
# art-include: 21.3/Athena
#
# art-output: *.txt

set -x

echo $AtlasVersion
if [ ${AtlasVersion:0:4} == "21.0" ]; then
    DetDesVer='"ATLAS-R2-2016-01-00-01"'
elif [ ${AtlasVersion:0:4} == "21.3" ]; then
    DetDesVer='"ATLAS-R3-2021-00-00-00"'
else
    DetDesVer='"ATLAS-R2-2015-03-04-00"'
fi

athena.py -c 'DetDescrVersion = '$DetDesVer'; GlobalCondTag = "OFLCOND-RUN12-SDR-22"' MuonGeoModelTest/simulGeantinoHits_misal.py

echo "art-result: $?"
