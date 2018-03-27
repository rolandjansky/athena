#!/bin/sh
#
# art-description: Art test job for MuonGeomRTT package, migrated from RTT job: AmdcMGM-testLayout
# # Purpose: Compare MuonGeoModel and AMDB detector element positions
#
# art-type: grid

athena.py -c 'useAlignData=True;DetDescrVersion="ATLAS-GEO-18-00-00"' -b MuonGeomRTT/AmdcMGM_RTT_topOptions.py