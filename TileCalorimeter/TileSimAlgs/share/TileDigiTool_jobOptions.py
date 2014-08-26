include.block("TileSimAlgs/TileDigiTool_jobOptions.py")
###############################################################
#
# Job options file : TileDigiTool_jobOptions.py
# read G4 hits and create TileRawChannels
#
#==============================================================
# Create digits and do Opt.filtering by default
if not 'doTileHitToDigit' in dir():
    doTileHitToDigit = True
if not 'doTileHitToRawChannelDirect' in dir():
    doTileHitToRawChannelDirect = False

#----------------
# Add Algorithms
#----------------

## get a handle on the top sequence of algorithms
from AthenaCommon.AlgSequence import AlgSequence
topSequence = AlgSequence()


if doTileHitToDigit or doTileHitToRawChannelDirect:
    from AthenaCommon import CfgGetter
    topSequence.PileUpToolsAlg.PileUpTools += [ CfgGetter.getPrivateTool("TileHitVecToCntTool", checkType=True) ]

include( "TileSimAlgs/TileDigiCommon_jobOptions.py" )
