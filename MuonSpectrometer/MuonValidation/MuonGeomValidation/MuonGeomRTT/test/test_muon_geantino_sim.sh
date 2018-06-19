#!/bin/sh
#
# art-description: Art test job for MuonGeomRTT package, migrated from RTT job: MuonGeantinoSim
# # Purpose: Check internal consistency of raw geometry (fed to G4 simulation)
# 
# art-type: grid

athena.py -c 'DetDescrVersion = "ATLAS-R2-2015-03-01-00"; GlobalCondTag = "OFLCOND-RUN12-SDR-22"' -b MuonGeoModelTest/simulGeantinoHits_misal.py

#athena.py -b MuonGeomRTT/MuonGeantinoSim_RTT_topOptions.py

