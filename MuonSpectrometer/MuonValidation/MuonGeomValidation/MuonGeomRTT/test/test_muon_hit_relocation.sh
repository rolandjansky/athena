#!/bin/sh
#
# art-description: Art test job for MuonGeomRTT package, migrated from RTT job: MuonHitRelocation
# # Purpose: Check internal consistency of readout geometry (used by reconstruction)
# 
# art-type: grid

athena.py -c 'DetDescrVersion = "ATLAS-GEO-16-00-01"; GlobalCondTag = "OFLCOND-DR-BS900-A1-01"' -b MuonGeoModelTest/MuonHitRelocation.py

#athena.py -b MuonGeomRTT/MuonHitRelocation_RTT_topOptions.py
