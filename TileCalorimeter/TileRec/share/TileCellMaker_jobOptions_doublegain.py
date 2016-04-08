from AthenaCommon.Logging import logging
mlog = logging.getLogger('TileRecLogger')

try:        
    from CaloRec.CaloRecConf import CaloCellMaker                
except:
    mlog.error("could not import CaloRec.CaloCellMaker")
    print traceback.format_exc()
   
theCaloCellMakerLG=CaloCellMaker("CaloCellMakerLG")
theCaloCellMakerLG.CaloCellsOutputName = "AllCaloLG"

theCaloCellMakerHG=CaloCellMaker("CaloCellMakerHG")
theCaloCellMakerHG.CaloCellsOutputName = "AllCaloHG"

from AthenaCommon.AlgSequence import AlgSequence
topSequence = AlgSequence()
topSequence += theCaloCellMakerLG
topSequence += theCaloCellMakerHG
 
from AthenaCommon.AppMgr import ToolSvc

if not hasattr( ToolSvc, "TileCellBuilderLG" ):
    from TileRecUtils.TileRecUtilsConf import TileCellBuilder
    theTileCellBuilderLG=TileCellBuilder("TileCellBuilderLG")
    theTileCellBuilderLG.MBTSContainer = "MBTSContainerLG"
    theTileCellBuilderLG.E4prContainer = "E4prContainerLG"
    ToolSvc += theTileCellBuilderLG

if not hasattr( ToolSvc, "TileCellBuilderHG" ):
    from TileRecUtils.TileRecUtilsConf import TileCellBuilder
    theTileCellBuilderHG=TileCellBuilder("TileCellBuilderHG")
    theTileCellBuilderHG.MBTSContainer = "MBTSContainerHG"
    theTileCellBuilderHG.E4prContainer = "E4prContainerHG"
    ToolSvc += theTileCellBuilderHG


from TileRecUtils.TileRecFlags import jobproperties
ToolSvc.TileCellBuilderLG.TileRawChannelContainer = jobproperties.TileRecFlags.TileRawChannelContainer()
ToolSvc.TileCellBuilderLG.maskBadChannels = True
ToolSvc.TileCellBuilderLG.SkipGain = 1

ToolSvc.TileCellBuilderHG.TileRawChannelContainer = jobproperties.TileRecFlags.TileRawChannelContainer()
ToolSvc.TileCellBuilderHG.maskBadChannels = True
ToolSvc.TileCellBuilderHG.SkipGain = 0

theCaloCellMakerLG.CaloCellMakerToolNames += [ ToolSvc.TileCellBuilderLG.getFullName() ]
theCaloCellMakerHG.CaloCellMakerToolNames += [ ToolSvc.TileCellBuilderHG.getFullName() ]


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
    theCaloCellMakerLG.CaloCellMakerToolNames += [ ToolSvc.TileCellNoiseFilterLG.getFullName() ]
    theCaloCellMakerHG.CaloCellMakerToolNames += [ ToolSvc.TileCellNoiseFilterHG.getFullName() ]

if not hasattr( ToolSvc, "CaloCellContainerFinalizerToolLG" ):
    from CaloRec.CaloRecConf import CaloCellContainerFinalizerTool
    theCaloCellContainerFinalizerToolLG=CaloCellContainerFinalizerTool("theCaloCellContainerFinalizerToolLG")
    ToolSvc += theCaloCellContainerFinalizerToolLG
    theCaloCellMakerLG.CaloCellMakerToolNames += [theCaloCellContainerFinalizerToolLG ]
else:
    theCaloCellMakerLG.CaloCellMakerToolNames += ["CaloCellContainerFinalizerToolLG"]


if not hasattr( ToolSvc, "CaloCellContainerFinalizerToolHG" ):
    from CaloRec.CaloRecConf import CaloCellContainerFinalizerTool
    theCaloCellContainerFinalizerToolHG=CaloCellContainerFinalizerTool("theCaloCellContainerFinalizerToolHG")
    ToolSvc += theCaloCellContainerFinalizerToolHG
    theCaloCellMakerHG.CaloCellMakerToolNames += [theCaloCellContainerFinalizerToolHG ]
else:
    theCaloCellMakerHG.CaloCellMakerToolNames += ["CaloCellContainerFinalizerToolHG"]




if ('doCaloNeighborsCorr' in dir()) and doCaloNeighborsCorr:
    if not hasattr( ToolSvc, "CaloCellNeighborsAverageCorr" ):
        from CaloCellCorrection.CaloCellCorrectionConf import CaloCellNeighborsAverageCorr
        theCaloCellNeighborsAverageCorr = CaloCellNeighborsAverageCorr("CaloCellNeighborsAverageCorr")
        theCaloCellNeighborsAverageCorr.testMode=False
        ToolSvc +=  theCaloCellNeighborsAverageCorr
        theCaloCellMakerLG.CaloCellMakerToolNames += [theCaloCellNeighborsAverageCorr]
        theCaloCellMakerHG.CaloCellMakerToolNames += [theCaloCellNeighborsAverageCorr]
    else:
        theCaloCellMakerLG.CaloCellMakerToolNames += ["CaloCellNeighborsAverageCorr"]
        theCaloCellMakerHG.CaloCellMakerToolNames += ["CaloCellNeighborsAverageCorr"]
else:
    doCaloNeighborsCorr=False

if not hasattr( ToolSvc, "CaloCellContainerCheckerToolLG" ):
    from CaloRec.CaloRecConf import CaloCellContainerCheckerTool
    theCaloCellContainerCheckerToolLG=CaloCellContainerCheckerTool("CaloCellContainerCheckerToolLG")
    ToolSvc += theCaloCellContainerCheckerToolLG
    theCaloCellMakerLG.CaloCellMakerToolNames += [theCaloCellContainerCheckerToolLG]
else:
    theCaloCellMakerLG.CaloCellMakerToolNames += ["CaloCellContainerCheckerToolLG"]



if not hasattr( ToolSvc, "CaloCellContainerCheckerToolHG" ):
    from CaloRec.CaloRecConf import CaloCellContainerCheckerTool
    theCaloCellContainerCheckerToolHG=CaloCellContainerCheckerTool("CaloCellContainerCheckerToolHG")
    ToolSvc += theCaloCellContainerCheckerToolHG
    theCaloCellMakerHG.CaloCellMakerToolNames += [theCaloCellContainerCheckerToolHG]
else:
    theCaloCellMakerHG.CaloCellMakerToolNames += ["CaloCellContainerCheckerToolHG"]

print theCaloCellMakerLG
print ToolSvc.TileCellBuilderLG

print "####################################################################################"

print theCaloCellMakerHG
print ToolSvc.TileCellBuilderHG
