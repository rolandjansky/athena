include.block("TileSimAlgs/TileDigiCommon_jobOptions.py")
###############################################################
#
# Job options file : TileDigiCommon_jobOptions.py
# read G4 hits and create TileRawChannels
#
#==============================================================
from AthenaCommon.BeamFlags import jobproperties

# Create digits and do Opt.filtering by default
if not 'doTileHitToDigit' in dir():
    doTileHitToDigit = True
if not 'doTileDigitToRawChannel' in dir():
    doTileDigitToRawChannel = True
if not 'doTileHitToRawChannelDirect' in dir():
    doTileHitToRawChannelDirect = False
if not 'doTileDigitsFromPulse' in dir():
    doTileDigitsFromPulse = False

try:
    from __main__ import ALLOWIGNOREEXISTINGDATAOBJECT
    from __main__ import ALLOWIGNORECONFIGERROR
    from __main__ import ALLOWDISABLE
    from __main__ import ALLOWBACKNAV
except: # default values if not done
    ALLOWIGNOREEXISTINGDATAOBJECT=False
    ALLOWIGNORECONFIGERROR=False
    ALLOWDISABLE=False
    ALLOWBACKNAV=False
    from __main__ import ALLOWIGNOREEXISTINGDATAOBJECT
    from __main__ import ALLOWIGNORECONFIGERROR
    from __main__ import ALLOWDISABLE
    from __main__ import ALLOWBACKNAV

if doTileHitToRawChannelDirect:
    
    from TileSimAlgs.TileRawChannelFromHitsGetter import *
    theTileRawChannelFromHitsGetter=TileRawChannelFromHitsGetter()


if doTileHitToDigit:
    
    from TileSimAlgs.TileDigitsGetter import *
    theTileDigitsGetter=TileDigitsGetter()
    
    from TileSimAlgs.TileSimAlgsConf import *
    theTileDigitsMaker=TileDigitsMaker()
    theTileDigitsMaker.UseCoolPulseShapes=True
    theTileDigitsMaker.MaskBadChannels=False
    theTileDigitsMaker.RndmEvtOverlay=False
    
if doTileDigitsFromPulse:
    
    from AthenaCommon.Logging import logging
    import traceback
    mlog = logging.getLogger( 'TileDigitiization' )

    try:        
        from TileSimAlgs.TileSimAlgsConf import TileDigitsFromPulse
        theTileDigitsFromPulse=TileDigitsFromPulse()
        mlog.info("Adding TileDigitsFromPulse to topSequence")
        ## get a handle on the top sequence of algorithms
        from AthenaCommon.AlgSequence import AlgSequence
        topSequence = AlgSequence()
        topSequence += theTileDigitsFromPulse;

    except:
        mlog.error("could not import TileSimAlgs.TileDigitsFromPulse")
        print traceback.format_exc()


if doTileDigitToRawChannel:
    
    # Reconstruction method: OF2 (default in full ATLAS reco)
    from TileRecUtils.TileRecFlags import jobproperties
    jobproperties.TileRecFlags.doTileFit = False
    jobproperties.TileRecFlags.doTileManyAmps = False
    jobproperties.TileRecFlags.doTileOpt = False
    if jobproperties.Beam.beamType == 'collisions': 
        jobproperties.TileRecFlags.doTileOptATLAS = True
    else: 
        jobproperties.TileRecFlags.doTileOpt2 = True
    
    from TileRecUtils.TileRawChannelGetter import *
    theTileRawChannelGetter=TileRawChannelGetter()
    
    jobproperties.TileRecFlags.TileRawChannelContainer = "TileRawChannelCnt"
    if jobproperties.Beam.beamType == 'collisions': 
        ToolSvc.TileRawChannelBuilderOptATLAS.TileRawChannelContainer = jobproperties.TileRecFlags.TileRawChannelContainer()
    else:
        ToolSvc.TileRawChannelBuilderOpt2Filter.TileRawChannelContainer = jobproperties.TileRecFlags.TileRawChannelContainer()


if not hasattr( ToolSvc, "TileBeamInfoProvider" ):
    from TileRecUtils.TileRecUtilsConf import TileBeamInfoProvider
    theTileBeamInfoProvider = TileBeamInfoProvider()
    ToolSvc += theTileBeamInfoProvider

# change default parameters for TileBeamInfo
ToolSvc.TileBeamInfoProvider.TileBeamElemContainer=""; # disable reading of trigger type from BeamElem container
ToolSvc.TileBeamInfoProvider.TileDigitsContainer="";   # disable checking of Digits container size for bi-gain mode
ToolSvc.TileBeamInfoProvider.TileRawChannelContainer=""; # disable checking of DQstatus for simulated data

#
include( "TileSimAlgs/TileSamplingFraction_jobOptions.py" )
