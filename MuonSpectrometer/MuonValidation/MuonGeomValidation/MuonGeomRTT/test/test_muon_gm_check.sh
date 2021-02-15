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

# Create empty PoolFileCatalog
art.py createpoolfile

set -x

athena.py -c "DetDescrVersion='ATLAS-R2-2016-01-00-01'" MuonGeoModelTest/MuonGeoModelCheck_jobOption.py
athena.py -c "DetDescrVersion='ATLAS-R3-2021-01-00-01'" MuonGeoModelTest/MuonGeoModelCheck_jobOption.py
athena.py -c "DetDescrVersion='ATLAS-R3S-2021-01-00-01'" MuonGeoModelTest/MuonGeoModelCheck_jobOption.py

echo "art-result: $?"
