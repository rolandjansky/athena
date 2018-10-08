#!/bin/sh
#
# art-description: Art test job for MuonGeomRTT package, migrated from RTT job: MuonGMCheckReference
# # Purpose: Compare MuonGeoModel raw and readout geometry relative to a reference
#
# art-type: grid

athena.py -c 'DetDescrVersion="ATLAS-GEO-16-00-01"' -b MuonGeomRTT/MuonGMCheck_RTT_topOptions.py