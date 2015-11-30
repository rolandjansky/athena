# enable all possible corrections
# in general, it's safe to set CorrectEnergy and MaskBadChannels to true
# while CorrectTime can be applied only if timing folder in database
# contains corrections relative to previous correction
# (this is the case only for ESDtoESD production of 2008 data)

TccName = "TileCellCorrection"

from AthenaCommon.AppMgr import ToolSvc
from CaloTools.CaloNoiseToolDefault import CaloNoiseToolDefault
theNoiseTool = CaloNoiseToolDefault()
ToolSvc += theNoiseTool

from TileRecUtils.TileRecUtilsConf import TileCellNoiseFilter
theTileCellNoiseFilter = TileCellNoiseFilter()
theTileCellNoiseFilter.CaloNoiseTool = theNoiseTool
theTileCellNoiseFilter.UseTwoGaussNoise = False
ToolSvc += theTileCellNoiseFilter
 
from TileRecUtils.TileRecUtilsConf import TileCellMaskingTool
theTileCellMaskingTool = TileCellMaskingTool()
theTileCellMaskingTool.RejectedTileDrawer=[] # put "ROS drawer" pairs here which should be masked
ToolSvc += theTileCellMaskingTool

from CaloCellCorrection.CaloCellCorrectionConf import CaloCellNeighborsAverageCorr
theCaloCellNeighborsAverageCorr = CaloCellNeighborsAverageCorr("CaloCellNeighborsAverageCorr")
theCaloCellNeighborsAverageCorr.testMode=False
# skip LAr cells
theCaloCellNeighborsAverageCorr.skipDeadFeb=True
theCaloCellNeighborsAverageCorr.skipDeadLAr=True
# do not skip Tile cells (i.e. apply correction)
theCaloCellNeighborsAverageCorr.skipDeadDrawer=False
theCaloCellNeighborsAverageCorr.skipDeadTile=False
ToolSvc +=  theCaloCellNeighborsAverageCorr

from TileRecAlgs.TileRecAlgsConf import TileCellCorrection
theTileCellCorrection = TileCellCorrection(TccName)
theTileCellCorrection.CaloCellContainer = "AllCalo"
theTileCellCorrection.CorrectTime = False
theTileCellCorrection.CorrectEnergy = False
theTileCellCorrection.MaskBadChannels = True
theTileCellCorrection.TileCellMasking = False
theTileCellCorrection.TileCellNeighbors = True
theTileCellCorrection.TileCellNoiseFilter = True
theTileCellCorrection.TileCellMaskingTool = theTileCellMaskingTool
theTileCellCorrection.TileCellNeighborsTool = theCaloCellNeighborsAverageCorr
theTileCellCorrection.TileCellNoiseFilterTool = theTileCellNoiseFilter


#=== add to ServiceMgr
from AthenaCommon.AppMgr import ServiceMgr as svcMgr
if not hasattr( svcMgr , TccName ):
    svcMgr += theTileCellCorrection

#=== ensure the creation of service
from AthenaCommon.AppMgr import theApp
if not TccName in theApp.CreateSvc:
    theApp.CreateSvc += [ TccName ]

