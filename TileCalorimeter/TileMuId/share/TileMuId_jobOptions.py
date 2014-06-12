###############################################################
#  
# Job options file to control Tile standalone muon identification 
#                          (TileMuId)     
#==============================================================

include.block( "TileMuId/TileMuId_jobOptions.py" )

from TileMuId.TileMuGetter import *
theTileMuGetter=TileMuGetter()
