include.block("TileSimAlgs/TileDigitization_jobOptions.py")
include.block("TileSimAlgs/TileFitDigitization_jobOptions.py")
###############################################################
#
# Job options file : TileFitDigitization
# read G4 hits and create TileRawChannels using Fit method
#
#==============================================================
from AthenaCommon.BeamFlags import jobproperties
from AthenaCommon import CfgGetter

# Create digits and run FitFilter for G4 data by default
if not 'doTileHitToDigit' in dir():
    doTileHitToDigit = True
if not 'doTileDigitToRawChannel' in dir():
    doTileDigitToRawChannel = True
if not 'doTileHitToRawChannelDirect' in dir():
    doTileHitToRawChannelDirect = False


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

#-------------------
# Adjust conditions
#-------------------

TileFrameLength = 9; # correct frame length for all testbeams
from TileConditions.TileInfoConfigurator import TileInfoConfigurator
tileInfoConfigurator = TileInfoConfigurator()
tileInfoConfigurator.NSamples = TileFrameLength
tileInfoConfigurator.OFPhysicsNSamples = TileFrameLength
tileInfoConfigurator.TrigSample = (TileFrameLength-1)/2
tileInfoConfigurator.NoiseScaleIndex = 3; # Noise for Fit method
from AthenaCommon.AppMgr import ToolSvc
from TileConditions.TileCondProxyConf import getTileCondProxy
ToolSvc.TileCondToolNoiseSample.ProxyNoiseSample = getTileCondProxy('FILE','Flt','Tile2003.ped','TileCondProxyFile_NoiseSample')
ToolSvc.TileBadChanTool.ProxyOflBch = getTileCondProxy('FILE','Bch','TileNoBad.oflBch','TileCondProxyFile_OflBch')

#----------------
# Add Algorithms
#----------------

## get a handle on the top sequence of algorithms
from AthenaCommon.AlgSequence import AlgSequence
topSequence = AlgSequence()


if doTileHitToDigit or doTileHitToRawChannelDirect:
    
    topSequence += CfgGetter.getAlgorithm("TileHitVecToCnt/TileHitVecToCnt", tryDefaultConfigurable=True)
    theTileHitVecToCnt = topSequence.TileHitVecToCnt.DigitizationTool


if doTileHitToRawChannelDirect:
    
    from TileSimAlgs.TileRawChannelFromHitsGetter import *
    theTileRawChannelFromHitsGetter=TileRawChannelFromHitsGetter()


if doTileHitToDigit:
    
    from TileSimAlgs.TileDigitsGetter import *
    theTileDigitsGetter=TileDigitsGetter()
    
    from TileSimAlgs.TileSimAlgsConf import *
    theTileDigitsMaker=TileDigitsMaker()
    theTileDigitsMaker.UseCoolPulseShapes=False
    theTileDigitsMaker.MaskBadChannels=False
    theTileDigitsMaker.RndmEvtOverlay=False
    
    # Random number engine
    from Digitization.DigitizationFlags import jobproperties
    theTileDigitsMaker.RndmSvc=jobproperties.Digitization.rndmSvc()
    jobproperties.Digitization.rndmSeedList.addSeed("Tile_DigitsMaker", 4789899, 989240512)


if doTileDigitToRawChannel:
    
    # Reconstruction method: Fit Method (default for testbeam reco)
    from TileRecUtils.TileRecFlags import jobproperties
    jobproperties.TileRecFlags.doTileFit = True
    jobproperties.TileRecFlags.doTileManyAmps = False
    jobproperties.TileRecFlags.doTileOpt = False
    jobproperties.TileRecFlags.doTileOptATLAS = False
    jobproperties.TileRecFlags.doTileOpt2 = False
    
    from TileRecUtils.TileRawChannelGetter import *
    theTileRawChannelGetter=TileRawChannelGetter()
    
    jobproperties.TileRecFlags.TileRawChannelContainer = "TileRawChannelCnt"
    ToolSvc.TileRawChannelBuilderFitFilter.TileRawChannelContainer = "TileRawChannelCnt"
    ToolSvc.TileRawChannelBuilderFitFilter.calibrateEnergy = True; # apply CIS calibration to get pCb
    ToolSvc.TileRawChannelBuilderFitFilter.RunType = 1; # physics 
    ToolSvc.TileRawChannelBuilderFitFilter.FrameLength = TileFrameLength; # 9 samples always
    #ToolSvc.TileRawChannelBuilderFitFilter.MaxTimeSampleOffset = 0; # do not go outside frame
    ToolSvc.TileRawChannelBuilderFitFilter.MaxIterate = 9; # iterate 9 times and choose best chi2
    ToolSvc.TileRawChannelBuilderFitFilter.RMSChannelNoise = 3; # use noise values from database
    ToolSvc.TileRawChannelBuilderFitFilter.NoiseThresholdRMS = 3; # 2-param fit forced for pedestal-like events
    ToolSvc.TileRawChannelBuilderFitFilter.SaturatedSampleError = 6; # error (in terms of RMS) assigned to sat.sample


from TileRecUtils.TileDQstatusAlgDefault import TileDQstatusAlgDefault
dqstatus = TileDQstatusAlgDefault()

#
include( "TileSimAlgs/TileSamplingFraction_jobOptions.py" )
