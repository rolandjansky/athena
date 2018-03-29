# ****************************************************************************
# jobOptions file for setting up TilePulseForTileMuonReceiver algorithm
# ============================================================================

# The same pedestal and noise sigma values  for all channels 
# in simulation of TileMuonReceiver board will be used.
TileMuRcvNoiseSigma = 2.0
TileMuRcvPedestal = 11.73 # Should be the same in TileInfo and TileRawChannelBuilderMF

from AthenaCommon.AppMgr import ServiceMgr as svcMgr
svcMgr.TileInfoLoader.MuRcvNoiseSigma = TileMuRcvNoiseSigma
svcMgr.TileInfoLoader.MuRcvPed = TileMuRcvPedestal
# Uncomment for a local run BUT *COMMENT* this line prior to an upload to SVN
# svcMgr.TileCablingSvc.CablingType = 4 

from AthenaCommon import CfgMgr
toolSvc = CfgMgr.ToolSvc()

# Set up TileCondToolPulseShape to be used in
# TilePulseForTileMuonReceiver and TileCondToolOfc
from TileConditions.TileCondToolConf import getTileCondToolMuRcvPulseShape
toolSvc += getTileCondToolMuRcvPulseShape('FILE', 'TileCondToolMuRcvPulseShape')

# Set up TileCondToolOfc to be used in TileRawChannelBuilderMF
toolSvc += CfgMgr.TileCondToolOfc('TileCondToolMuRcvOfc'
                                  , OptFilterDeltaCorrelation = True
                                  , TileCondToolPulseShape = toolSvc.TileCondToolMuRcvPulseShape)

# Set up TileRawChannelBuilderMF to be used in TilePulseForTileMuonReceiver
toolSvc += CfgMgr.TileRawChannelBuilderMF('TileMuRcvRawChannelBuilderMF'
                                          , MF = 1
                                          , PedestalMode = 0
                                          , DefaultPedestal = TileMuRcvPedestal
                                          , calibrateEnergy = jobproperties.TileRecFlags.calibrateEnergy()
                                          , TileCondToolOfc = toolSvc.TileCondToolMuRcvOfc
                                          , TileCondToolOfcOnFly = toolSvc.TileCondToolMuRcvOfc)

#  Random number engine in TilePulseForTileMuonReceiver
from Digitization.DigitizationFlags import jobproperties

from AthenaCommon.AlgSequence import AlgSequence
topSequence = AlgSequence()

#  Set up TilePulseForTileMuonReceiver
topSequence += CfgMgr.TilePulseForTileMuonReceiver('TilePulseForTileMuonReceiver'
#                                                   , OutputLevel = VERBOSE 
                                                   , IntegerDigits = not jobproperties.Digitization.PileUpPremixing()
                                                   , UseCoolPedestal = False
                                                   , UseCoolPulseShapes = True
                                                   , TileCondToolPulseShape = toolSvc.TileCondToolMuRcvPulseShape
                                                   , TileRawChannelBuilderMF = toolSvc.TileMuRcvRawChannelBuilderMF
                                                   , RndmSvc = jobproperties.Digitization.rndmSvc())


jobproperties.Digitization.rndmSeedList.addSeed("Tile_PulseForTileMuonReceiver", 4789899, 989240512)

topSequence += CfgMgr.TileMuonReceiverDecision('TileMuonReceiverDecision'
#                                                , OutputLevel = VERBOSE 
						, MuonReceiverEneThreshCellD6Low = 500
						, MuonReceiverEneThreshCellD6andD5Low = 500
						, MuonReceiverEneThreshCellD6High = 600
						, MuonReceiverEneThreshCellD6andD5High = 600)

