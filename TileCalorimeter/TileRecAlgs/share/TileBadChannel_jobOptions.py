from AthenaCommon.AlgSequence import AlgSequence
topSequence = AlgSequence()

from TileRecAlgs.TileRecAlgsConf import TileRawChannelToTTL1
theTileRawChannelToTTL1 = TileRawChannelToTTL1()

theTileRawChannelToTTL1.TileRawChannelContainer = "TileRawChannelCnt"
theTileRawChannelToTTL1.TileTTL1Container = "TileTTL1ContainerBad"
theTileRawChannelToTTL1.maskBadChannels = True

topSequence += theTileRawChannelToTTL1
