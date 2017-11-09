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


from RecExConfig.RecFlags import rec
if rec.doDigiTruth():
  CaloCellMaker=Algorithm("CaloCellMaker_DigiHSTruth")
  CaloCellMaker_DigiHSTruth.CaloCellMakerToolNames += [ "TileCellBuilder/TileCellBuilder_DigiHSTruth" ]
  ToolSvc.TileCellBuilder_DigiHSTruth.TileRawChannelContainer = "TileRawChannelCnt_DigiHSTruth"
  ToolSvc.TileCellBuilder_DigiHSTruth.TileDSPRawChannelContainer = "TileRawChannelCnt_DigiHSTruth"
  ToolSvc.TileCellBuilder_DigiHSTruth.E4prContainer = "E4prContainer_DigiHSTruth"
  ToolSvc.TileCellBuilder_DigiHSTruth.MBTSContainer = "MBTSContainer_DigiHSTruth"


