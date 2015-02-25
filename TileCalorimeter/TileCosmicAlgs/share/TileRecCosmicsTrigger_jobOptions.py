###############################################################
#
# Job options file : TileRecCosmicsTrigger
#==============================================================

if not 'doTileCosmicsTrigger' in dir():
    doTileCosmicsTrigger = True

#-----------
# Load DLLs 
#-----------

if doTileCosmicsTrigger:
    theApp.Dlls += ["TileRecAlgs"]
    theApp.TopAlg += ["TileCosmicsTrigger"]
    TileCosmicsTrigger = Algorithm( "TileCosmicsTrigger" )
    TileCosmicsTrigger.TileTTL1Container = "TileTTL1CosmicsCnt"
    TileCosmicsTrigger.TileTriggerContainer = "TileTriggerCnt"
