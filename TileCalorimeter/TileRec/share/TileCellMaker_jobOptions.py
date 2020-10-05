from AthenaCommon.Logging import logging
mlog = logging.getLogger('TileRecLogger')

try:        
    from CaloRec.CaloRecConf import CaloCellMaker                
except:
    mlog.error("could not import CaloRec.CaloCellMaker")
    mlog.error (traceback.format_exc())
   
theCaloCellMaker=CaloCellMaker()
theCaloCellMaker.CaloCellsOutputName = "AllCalo"

from AthenaCommon.AlgSequence import AlgSequence
topSequence = AlgSequence()
topSequence += theCaloCellMaker
 
from TileRecUtils.TileRecUtilsConf import TileCellBuilder
theTileCellBuilder=TileCellBuilder()

from TileRecUtils.TileRecFlags import jobproperties
theTileCellBuilder.TileRawChannelContainer = jobproperties.TileRecFlags.TileRawChannelContainer()
theTileCellBuilder.maskBadChannels = True

theCaloCellMaker.CaloCellMakerToolNames += [ theTileCellBuilder ]

nf = jobproperties.TileRecFlags.noiseFilter()
n3 = nf - nf%100
n2 = nf - n3 - nf%10

if n2 == 10:
    from TileRecUtils.TileRecUtilsConf import TileRawChannelNoiseFilter
    theTileRawChannelNoiseFilter = TileRawChannelNoiseFilter()
    theTileCellBuilder.NoiseFilterTools = [theTileRawChannelNoiseFilter]

if n3 == 100:
    from TileRecUtils.TileRecUtilsConf import TileCellNoiseFilter
    theTileCellNoiseFilter = TileCellNoiseFilter()
    theCaloCellMaker.CaloCellMakerToolNames += [theTileCellNoiseFilter]

from CaloRec.CaloRecConf import CaloCellContainerFinalizerTool
theCaloCellContainerFinalizerTool = CaloCellContainerFinalizerTool()
theCaloCellMaker.CaloCellMakerToolNames += [theCaloCellContainerFinalizerTool]

if ('doCaloNeighborsCorr' in dir()) and doCaloNeighborsCorr:
    from CaloCellCorrection.CaloCellCorrectionConf import CaloCellNeighborsAverageCorr
    theCaloCellNeighborsAverageCorr = CaloCellNeighborsAverageCorr("CaloCellNeighborsAverageCorr")
    theCaloCellNeighborsAverageCorr.testMode=False
    theCaloCellMaker.CaloCellMakerToolNames += [theCaloCellNeighborsAverageCorr]
else:
    doCaloNeighborsCorr=False

from CaloRec.CaloRecConf import CaloCellContainerCheckerTool
theCaloCellContainerCheckerTool = CaloCellContainerCheckerTool()
theCaloCellMaker.CaloCellMakerToolNames += [theCaloCellContainerCheckerTool]


printfunc (theCaloCellMaker)
printfunc (theTileCellBuilder)
