from AthenaCommon.Logging import logging
mlog = logging.getLogger('TileRecLogger')

try:        
    from CaloRec.CaloRecConf import CaloCellMaker                
except:
    mlog.error("could not import CaloRec.CaloCellMaker")
    print traceback.format_exc()
   
theCaloCellMaker=CaloCellMaker()
theCaloCellMaker.CaloCellsOutputName = "AllCalo"

from AthenaCommon.AlgSequence import AlgSequence
topSequence = AlgSequence()
topSequence += theCaloCellMaker
 
from AthenaCommon.AppMgr import ToolSvc

if not hasattr( ToolSvc, "TileCellBuilder" ):
    from TileRecUtils.TileRecUtilsConf import TileCellBuilder
    theTileCellBuilder=TileCellBuilder()
    ToolSvc += theTileCellBuilder

from TileRecUtils.TileRecFlags import jobproperties
ToolSvc.TileCellBuilder.TileRawChannelContainer = jobproperties.TileRecFlags.TileRawChannelContainer()
ToolSvc.TileCellBuilder.maskBadChannels = True

theCaloCellMaker.CaloCellMakerToolNames += [ ToolSvc.TileCellBuilder.getFullName() ]

nf = jobproperties.TileRecFlags.noiseFilter()
n3 = nf - nf%100
n2 = nf - n3 - nf%10

if n2 == 10:
    from TileRecUtils.TileRecUtilsConf import TileRawChannelNoiseFilter
    theTileRawChannelNoiseFilter = TileRawChannelNoiseFilter()
    ToolSvc += theTileRawChannelNoiseFilter
    ToolSvc.TileCellBuilder.NoiseFilterTools = [theTileRawChannelNoiseFilter]

if n3 == 100:
    from TileRecUtils.TileRecUtilsConf import TileCellNoiseFilter
    theTileCellNoiseFilter = TileCellNoiseFilter()
    ToolSvc += theTileCellNoiseFilter
    theCaloCellMaker.CaloCellMakerToolNames += [ ToolSvc.TileCellNoiseFilter.getFullName() ]

if not hasattr( ToolSvc, "CaloCellContainerFinalizerTool" ):
    from CaloRec.CaloRecConf import CaloCellContainerFinalizerTool
    theCaloCellContainerFinalizerTool=CaloCellContainerFinalizerTool()
    ToolSvc += theCaloCellContainerFinalizerTool
    theCaloCellMaker.CaloCellMakerToolNames += [theCaloCellContainerFinalizerTool ]
else:
    theCaloCellMaker.CaloCellMakerToolNames += ["CaloCellContainerFinalizerTool"]

if ('doCaloNeighborsCorr' in dir()) and doCaloNeighborsCorr:
    if not hasattr( ToolSvc, "CaloCellNeighborsAverageCorr" ):
        from CaloCellCorrection.CaloCellCorrectionConf import CaloCellNeighborsAverageCorr
        theCaloCellNeighborsAverageCorr = CaloCellNeighborsAverageCorr("CaloCellNeighborsAverageCorr")
        theCaloCellNeighborsAverageCorr.testMode=False
        ToolSvc +=  theCaloCellNeighborsAverageCorr
        theCaloCellMaker.CaloCellMakerToolNames += [theCaloCellNeighborsAverageCorr]
    else:
        theCaloCellMaker.CaloCellMakerToolNames += ["CaloCellNeighborsAverageCorr"]
else:
    doCaloNeighborsCorr=False

if not hasattr( ToolSvc, "CaloCellContainerCheckerTool" ):
    from CaloRec.CaloRecConf import CaloCellContainerCheckerTool
    theCaloCellContainerCheckerTool=CaloCellContainerCheckerTool()
    ToolSvc += theCaloCellContainerCheckerTool
    theCaloCellMaker.CaloCellMakerToolNames += [theCaloCellContainerCheckerTool]
else:
    theCaloCellMaker.CaloCellMakerToolNames += ["CaloCellContainerCheckerTool"]


print theCaloCellMaker
print ToolSvc.TileCellBuilder
