#!/bin/sh
#
# art-description: Art test job for MuonGeomRTT package, migrated from RTT job: MuonGMCheck
# # Purpose: Compare MuonGeoModel raw and readout geometry relative to a reference
#
# art-type: grid

athena.py -c 'DetDescrVersion="ATLAS-GEO-18-00-00"' -b MuonGeomRTT/MuonGMCheck_RTT_topOptions.py