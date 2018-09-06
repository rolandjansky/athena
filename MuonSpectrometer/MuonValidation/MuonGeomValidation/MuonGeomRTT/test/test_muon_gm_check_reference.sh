#!/bin/sh
#
# art-description: Art test job for MuonGeomRTT package, migrated from RTT job: MuonGMCheckReference
# # Purpose: Compare MuonGeoModel raw and readout geometry relative to a reference
#
# art-type: grid
# art-include: master/Athena
# art-include: 21.0/Athena
# art-include: 21.3/Athena
#
# art-output: *.txt

set -x

athena.py -c 'DetDescrVersion="ATLAS-R2-2015-03-01-00"' MuonGeoModelTest/MuonGeoModelCheck_jobOption.py

echo "art-result: $?"
