from AthenaCommon.Logging import logging
mlog = logging.getLogger('TileRecLogger')
try:
    from CaloRec.CaloRecConf import CaloCellMaker
except:
    mlog.error("could not import CaloRec.CaloCellMaker")
    mlog.error (traceback.format_exc())

theCaloCellMakerLG=CaloCellMaker("CaloCellMakerLG")
theCaloCellMakerLG.CaloCellsOutputName = "AllCaloLG"

theCaloCellMakerHG=CaloCellMaker("CaloCellMakerHG")
theCaloCellMakerHG.CaloCellsOutputName = "AllCaloHG"

from AthenaCommon.AlgSequence import AlgSequence
topSequence = AlgSequence()
topSequence += theCaloCellMakerLG
topSequence += theCaloCellMakerHG

from AthenaCommon.AppMgr import ToolSvc

from TileRecUtils.TileRecUtilsConf import TileCellBuilder
theTileCellBuilderLG=TileCellBuilder("TileCellBuilderLG")
theTileCellBuilderLG.MBTSContainer = "MBTSContainerLG"
theTileCellBuilderLG.E4prContainer = "E4prContainerLG"

from TileRecUtils.TileRecUtilsConf import TileCellBuilder
theTileCellBuilderHG=TileCellBuilder("TileCellBuilderHG")
theTileCellBuilderHG.MBTSContainer = "MBTSContainerHG"
theTileCellBuilderHG.E4prContainer = "E4prContainerHG"


from TileRecUtils.TileRecFlags import jobproperties
theTileCellBuilderLG.TileRawChannelContainer = jobproperties.TileRecFlags.TileRawChannelContainer()
theTileCellBuilderLG.maskBadChannels = True
theTileCellBuilderLG.SkipGain = 1

theTileCellBuilderHG.TileRawChannelContainer = jobproperties.TileRecFlags.TileRawChannelContainer()
theTileCellBuilderHG.maskBadChannels = True
theTileCellBuilderHG.SkipGain = 0

theCaloCellMakerLG.CaloCellMakerToolNames += [theTileCellBuilderLG]
theCaloCellMakerHG.CaloCellMakerToolNames += [theTileCellBuilderHG]


nf = jobproperties.TileRecFlags.noiseFilter()
n3 = nf - nf%100
n2 = nf - n3 - nf%10

if n2 == 10:
    from TileRecUtils.TileRecUtilsConf import TileRawChannelNoiseFilter
    theTileRawChannelNoiseFilter = TileRawChannelNoiseFilter()
    theTileCellBuilderLG.NoiseFilterTools = [theTileRawChannelNoiseFilter]
    theTileCellBuilderHG.NoiseFilterTools = [theTileRawChannelNoiseFilter]

if n3 == 100:
    from TileRecUtils.TileRecUtilsConf import TileCellNoiseFilter
    theTileCellNoiseFilter = TileCellNoiseFilter()
    theCaloCellMakerLG.CaloCellMakerToolNames += [theTileCellNoiseFilter]
    theCaloCellMakerHG.CaloCellMakerToolNames += [theTileCellNoiseFilter]

from CaloRec.CaloRecConf import CaloCellContainerFinalizerTool
theCaloCellContainerFinalizerTool = CaloCellContainerFinalizerTool()
theCaloCellMakerLG.CaloCellMakerToolNames += [theCaloCellContainerFinalizerTool]
theCaloCellMakerHG.CaloCellMakerToolNames += [theCaloCellContainerFinalizerTool]

if ('doCaloNeighborsCorr' in dir()) and doCaloNeighborsCorr:
    from CaloCellCorrection.CaloCellCorrectionConf import CaloCellNeighborsAverageCorr
    theCaloCellNeighborsAverageCorr = CaloCellNeighborsAverageCorr("CaloCellNeighborsAverageCorr")
    theCaloCellNeighborsAverageCorr.testMode=False
    theCaloCellMakerLG.CaloCellMakerToolNames += [theCaloCellNeighborsAverageCorr]
    theCaloCellMakerHG.CaloCellMakerToolNames += [theCaloCellNeighborsAverageCorr]
else:
    doCaloNeighborsCorr=False

from CaloRec.CaloRecConf import CaloCellContainerCheckerTool
theCaloCellContainerCheckerTool = CaloCellContainerCheckerTool()
theCaloCellMakerLG.CaloCellMakerToolNames += [theCaloCellContainerCheckerTool]
theCaloCellMakerHG.CaloCellMakerToolNames += [theCaloCellContainerCheckerTool]

printfunc (theCaloCellMakerLG)
printfunc (theTileCellBuilderLG)

printfunc ("####################################################################################")

printfunc (theCaloCellMakerHG)
printfunc (theTileCellBuilderHG)

