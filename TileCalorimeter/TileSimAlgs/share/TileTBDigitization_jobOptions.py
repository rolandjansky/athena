###############################################################
#
# Job options file : TileTBDigitization
# create TileRawChannels from Hits stored in TileHitVector
# and TileBeamElem from hits in ancillary detectors
# Attention! noise is controlled by doCaloNoise variable
# (see TileConditions/TileConditions_jobOptions.py for details)      
#
#==============================================================
from AthenaCommon import CfgGetter

# Create digits and run FitFilter for G4 data by default
if not 'doTileHitToDigit' in dir():
    doTileHitToDigit = True
if not 'doTileDigitToRawChannel' in dir():
    doTileDigitToRawChannel = True
if not 'doTileHitToRawChannelDirect' in dir():
    doTileHitToRawChannelDirect = False
if not 'doTileTBHitToBeamElem' in dir():
    doTileTBHitToBeamElem = True
if not 'doTileHitToRawChannelCoherNoise' in dir():
    doTileHitToRawChannelCoherNoise=False


include( "TileConditions/TileTBConditions_jobOptions.py" )

# Add coherent noise
if doTileHitToRawChannelCoherNoise:
    tileInfoConfigurator.TileCoherNoise=True
else:
    tileInfoConfigurator.TileCoherNoise=False


#----------------
# Add Algorithms
#----------------

## get a handle on the top sequence of algorithms
from AthenaCommon.AlgSequence import AlgSequence
topSequence = AlgSequence()


if doTileHitToDigit or doTileHitToRawChannelDirect:
    
    topSequence += CfgGetter.getAlgorithm("TileHitVecToCnt/TileHitVecToCnt", tryDefaultConfigurable=True)
    theTileHitVecToCnt = topSequence.TileHitVecToCnt.DigitizationTool
    theTileHitVecToCnt.TileHitVectors=["TileHitVec"]
    if doTileHitToRawChannelDirect:
        theTileHitVecToCnt.HitTimeFlag=1; # reset hit time to zero, no pulse shape
    else:
        theTileHitVecToCnt.HitTimeFlag=2;       # shift time of all hits so that
        theTileHitVecToCnt.TriggerTime = -25.0; # time of the earliest hit in event is random in interval [-25,0] ns


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
    if doTileHitToRawChannelDirect:
        theTileDigitsMaker.IntegerDigits=False; # do not round
    else:
        theTileDigitsMaker.IntegerDigits=True; # save integer numbers in digits vecto
    

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
    
    jobproperties.TileRecFlags.TileRawChannelContainer = "TileRawChannelFit"
    ToolSvc.TileRawChannelBuilderFitFilter.TileRawChannelContainer = "TileRawChannelFit"
    ToolSvc.TileRawChannelBuilderFitFilter.calibrateEnergy = True; # apply CIS calibration to get pCb
    ToolSvc.TileRawChannelBuilderFitFilter.RunType = 1; # physics 
    ToolSvc.TileRawChannelBuilderFitFilter.FrameLength = 9; # 9 samples always
    #ToolSvc.TileRawChannelBuilderFitFilter.MaxTimeSampleOffset = 0; # do not go outside frame
    ToolSvc.TileRawChannelBuilderFitFilter.MaxIterate = 9; # iterate 9 times and choose best chi2
    ToolSvc.TileRawChannelBuilderFitFilter.RMSChannelNoise = 3; # use noise values from database
    ToolSvc.TileRawChannelBuilderFitFilter.NoiseThresholdRMS = 3; # 2-param fit forced for pedestal-like events
    ToolSvc.TileRawChannelBuilderFitFilter.SaturatedSampleError = 6; # error (in terms of RMS) assigned to sat.sample


if doTileHitToDigit and doTileDigitToRawChannel and doTileHitToRawChannelDirect:
    
    from TileRecUtils.TileRecUtilsConf import *
    theTTileRawChannelVerify = TileRawChannelVerify(TileRawChannelContainer1="TileRawChannelCnt",
                                                    TileRawChannelContainer2="TileRawChannelFit",
                                                    Precision=0.001,
                                                    DumpRawChannels=False,
                                                    SortFlag=False          
                                                    )
    
    topSequence += theTTileRawChannelVerify


if doTileTBHitToBeamElem:
    
    from TileSimAlgs.TileBeamElemGetter import *
    theTileBeamElemGetter = TileBeamElemGetter()


from TileRecUtils.TileDQstatusAlgDefault import TileDQstatusAlgDefault
dqstatus = TileDQstatusAlgDefault()

#
include( "TileSimAlgs/TileSamplingFraction_jobOptions.py" )
