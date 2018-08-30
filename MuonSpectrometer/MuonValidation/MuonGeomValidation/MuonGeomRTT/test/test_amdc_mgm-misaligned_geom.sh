#!/bin/sh
#
# art-description: Art test job for MuonGeomRTT package, migrated from RTT job: AmdcMGM-MisalignedGeom
# # Purpose: Compare MuonGeoModel and AMDB detector element positions
#
# art-type: grid
# art-include: master/Athena
# art-include: 21.0/Athena
# art-include: 21.3/Athena
#
# art-output: *.txt

set -x

athena.py -c 'DetDescrVersion="ATLAS-R2-2015-03-01-00"' MuonGeoModelTest/runAmdcMGM.py

echo "art-result: $?"