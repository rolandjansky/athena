#!/bin/sh
#
# art-description: Art test job for MuonGeomRTT package, migrated from RTT job: MuonGMCheck
# # Purpose: Compare MuonGeoModel raw and readout geometry relative to a reference
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

athena.py -c 'DetDescrVersion = '$DetDesVer MuonGeoModelTest/MuonGeoModelCheck_jobOption.py

echo "art-result: $?"
