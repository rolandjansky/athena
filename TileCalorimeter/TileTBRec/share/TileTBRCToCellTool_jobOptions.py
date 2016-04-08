# real data by default
if not 'doSim' in dir():
    doSim=False


from AthenaCommon.Logging import logging
mlog = logging.getLogger('TileTBRecLogger')

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

if doSim:
    # set correct name of input container in CTB simulation
    jobproperties.TileRecFlags.TileRawChannelContainer = "TileRawChannelFit"

ToolSvc.TileCellBuilder.maskBadChannels = True
ToolSvc.TileCellBuilder.TileRawChannelContainer = jobproperties.TileRecFlags.TileRawChannelContainer()

theCaloCellMaker.CaloCellMakerToolNames += [ ToolSvc.TileCellBuilder.getFullName() ]

print theCaloCellMaker
print ToolSvc.TileCellBuilder
