###############################################################
#
# Job options file : TileCellBuilder
# Read TileRawChannels and create TileCells
# All Det.Descr and Conditions options
# as well as CaloRec DLLs should be loaded before
#
#==============================================================
#-----------
# Load DLLs 
#-----------
theApp.Dlls += ["TileRecUtils"]

#theApp.Dlls += [ "CaloRec" ]

#--------------------------------
# Algorithms Properties settings 
#--------------------------------

#theApp.TopAlg += ["CaloCellMaker"]

CaloCellMaker=Algorithm("CaloCellMaker")

#CaloCellMaker.CaloCellsOutputName = "AllCalo"

CaloCellMaker.CaloCellMakerToolNames += [ "TileCellBuilder/TileCellBuilder" ]

from AthenaCommon.AppMgr import ToolSvc
ToolSvc.TileCellBuilder.TileRawChannelContainer = "TileRawChannelCnt"

