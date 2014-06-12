###############################################################
#  
# Job options file to control TileMuId
#     
#==============================================================

include.block( "TileMuId/TileMuId_cosmics_jobOptions.py" )

from TileMuId.TileMuGetter import *
theTileMuGetter=TileMuGetter()

from TileMuId.TileMuIdConf import *
theTileLookForMuAlg=TileLookForMuAlg()

# Lower energy thresholds in MeV
#theTileLookForMuAlg.LowerTresh0MeV=150.0
#theTileLookForMuAlg.LowerTresh1MeV=150.0 
#theTileLookForMuAlg.LowerTresh2MeV=150.0

##Trhesholds set to 80.0 MeV (TileMuGetter)
