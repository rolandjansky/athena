#==============================================================
#
# Job options file : TileCellBuilder_cosmics_jobOptions.py
# Choose proper container with TileRawChannels
# It should be always  TileRawChannelCnt for simulated data
# and normally TileRawChannelOpt2 (new Opt Filt with iterations) for real data
#
#==============================================================

# real data by default
if not 'doTileCosmicsSim' in dir():
    doTileCosmicsSim=False

from AthenaCommon.AppMgr import ToolSvc

if not hasattr( ToolSvc, "TileCellBuilder" ):
    from TileRecUtils.TileRecUtilsConf import TileCellBuilder
    theTileCellBuilder=TileCellBuilder()
    ToolSvc += theTileCellBuilder

from TileRecUtils.TileRecFlags import jobproperties

if doTileCosmicsSim:
    # set correct name of input container in simulation
    jobproperties.TileRecFlags.TileRawChannelContainer = "TileRawChannelCnt"

ToolSvc.TileCellBuilder.maskBadChannels = True
ToolSvc.TileCellBuilder.TileRawChannelContainer = jobproperties.TileRecFlags.TileRawChannelContainer()

# Configure TileROD_Decoder
#if not hasattr( ToolSvc, "TileROD_Decoder" ):
from TileByteStream.TileByteStreamConf import TileROD_Decoder
ToolSvc += TileROD_Decoder()

ToolSvc.TileROD_Decoder.calibrateEnergy = True
ToolSvc.TileROD_Decoder.suppressDummyFragments = True
ToolSvc.TileROD_Decoder.maskBadDigits = True

print ToolSvc.TileROD_Decoder
