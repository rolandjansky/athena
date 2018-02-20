#!/bin/sh
#
# art-description: Art test job for MuonGeomRTT package, migrated from RTT job: AmdcMGM-MisalignedGeom
# # Purpose: Compare MuonGeoModel and AMDB detector element positions
#
# art-type: grid

athena.py -c 'DetDescrVersion="ATLAS-GEO-16-00-01"' -b MuonGeomRTT/AmdcMGM_RTT_topOptions.py

