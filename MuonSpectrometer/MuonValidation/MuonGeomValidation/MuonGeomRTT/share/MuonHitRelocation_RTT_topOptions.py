############################################################
#
# MuonHitRelocation_RTT_topOptions.py
#
############################################################


DetDescrVersion = "ATLAS-GEO-10-00-01"
GlobalCondTag = "OFLCOND-DR-BS900-A1-01"
inputHits     = "myGeantinoHits_misal.pool.root"
#inputHits     = "MuonHits.root"
include( "MuonGeoModelTest/MuonHitRelocation.py" )

## hack needed to prevent RTT from substituting same input files as for
## first job -- needed since this jobs runs second
# I think this isn't needed anymore since the new chain tag exists in RTT -- SW
#include( "RecExRecoTest/rttchainhack.py" )
