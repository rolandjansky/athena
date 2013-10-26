###############################################################
#
# Job options file : TileRecCosmicsTrigger
#==============================================================

if not 'doTileCosmicsTriggerM3' in dir():
    doTileCosmicsTriggerM3 = True

#-----------
# Load DLLs 
#-----------

if doTileCosmicsTriggerM3:
    theApp.Dlls += ["TileRecAlgs"]
    theApp.TopAlg += ["TileCosmicsTrigger"]
    TileCosmicsTrigger = Algorithm( "TileCosmicsTrigger" )
    TileCosmicsTrigger.TileTTL1Container = "TileTTL1CosmicsCnt"
    TileCosmicsTrigger.TileTriggerContainer = "TileTriggerCntM3"
    TileCosmicsTrigger.ConnectedDrawers1 = ["","LBA44","LBA45","LBA46","","LBA48","LBA49","LBA50","","LBA52","LBA53","LBA54"]
    TileCosmicsTrigger.ConnectedDrawers2 = ["EBA43","EBA44","EBA45","EBA46","EBA47","EBA48","","","","","EBA53","EBA54"]
    TileCosmicsTrigger.ConnectedDrawers3 = ["LBA11","LBA12","LBA13","LBA14","LBA15","LBA16","LBA17","LBA18","LBA19","LBA20","LBA21","LBA22"]
    TileCosmicsTrigger.ConnectedDrawers4 = ["EBA11","EBA12","EBA13","EBA14","EBA15","EBA16","EBA17","EBA18","EBA19","EBA20","EBA21","EBA22"]
    TileCosmicsTrigger.ConnectedDrawers5 = ["LBC43","LBC44","","","LBC47","LBC48","LBC49","LBC50","LBC51","LBC52","",""]
    TileCosmicsTrigger.ConnectedDrawers6 = ["","","","","","","","","","","",""]
    TileCosmicsTrigger.ConnectedDrawers7 = ["","","LBC13","","LBC15","LBC16","LBC17","LBC18","LBC19","LBC20","LBC21","LBC22"]
    TileCosmicsTrigger.ConnectedDrawers8 = ["","","","","","","","","","","",""]
