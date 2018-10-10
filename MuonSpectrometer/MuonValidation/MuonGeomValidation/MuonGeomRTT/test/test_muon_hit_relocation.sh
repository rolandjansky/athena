#!/bin/sh
#
# art-description: Art test job for MuonGeomRTT package, migrated from RTT job: MuonHitRelocation
# # Purpose: Check internal consistency of readout geometry (used by reconstruction)
# 
# art-type: grid
# art-include: master/Athena
# art-include: 21.0/Athena
# art-include: 21.3/Athena
#
# art-output: *.txt
# art-output: *.root

set -x

echo $AtlasVersion
if [ ${AtlasVersion:0:4} == "21.0" ]; then
    DetDesVer='"ATLAS-R2-2016-01-00-01"'
elif [ ${AtlasVersion:0:4} == "21.3" ]; then
    DetDesVer='"ATLAS-R3-2021-00-00-00"'
else
    DetDesVer='"ATLAS-R2-2015-03-04-00"'
fi

athena.py -c 'DetDescrVersion = '$DetDesVer'; GlobalCondTag = "OFLCOND-DR-BS900-A1-01"; inputHits = "/cvmfs/atlas-nightlies.cern.ch/repo/data/data-art/MuonGeomRTT/geantinoHits.pool.root"' MuonGeoModelTest/MuonHitRelocation.py

echo "art-result: $?"