include.block ('TileRecAlgs/jobOptions_TileDigitsThresholdFilter.py')

from AthenaCommon.Logging import logging
tileDigitsFilterLogger = logging.getLogger( 'jobOptions_TileDigitsThresholdFilter.py' )

import AthenaCommon.CfgMgr as CfgMgr
from AthenaCommon.AlgSequence import AlgSequence
topSequence = AlgSequence() 

from TileRecUtils.TileRecFlags import jobproperties

from TileConditions.TileInfoConfigurator import TileInfoConfigurator
tileInfoConfigurator = TileInfoConfigurator()

if tileInfoConfigurator.setupCOOLDspThreshold():
    tileDigitsThresholdFilter = CfgMgr.TileDigitsThresholdFilter()
    jobproperties.TileRecFlags.TileDigitsContainer = 'TileDigitsFiltered'
    indices = [i for i, alg in enumerate(topSequence.getChildren()) if alg.getType() == 'TileRawChannelMaker']
    if len(indices):
        topSequence.insert(indices[0], tileDigitsThresholdFilter)
        if hasattr(topSequence, 'TileRChMaker'):
            topSequence.TileRChMaker.TileDigitsContainer = jobproperties.TileRecFlags.TileDigitsContainer()
    else:
        topSequence += tileDigitsThresholdFilter
    tileDigitsFilterLogger.info("Added TileDigitsThresholdFilter algorithm to filter Tile Digits over threshold")
else:
    tileDigitsFilterLogger.warn("It is impossible to get Tile DSP thresholds from COOL; No Tile Digits filtering!")

