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

from AthenaCommon.Logging import logging
mlog = logging.getLogger( 'TileDigitiization' )

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
    from Digitization.DigitizationFlags import digitizationFlags
    theTileDigitsMaker.DoHSTruthReconstruction = digitizationFlags.doDigiTruth()
    
if doTileDigitsFromPulse:
    
    import traceback

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
    
    #from AtlasGeoModel.CommonGMJobProperties import CommonGeometryFlags as commonGeoFlags
    #from AtlasGeoModel.InDetGMJobProperties import InDetGeometryFlags as geoFlags
    #RUN1=(commonGeoFlags.Run() == "RUN1" or (commonGeoFlags.Run() == "UNDEFINED" and geoFlags.isIBL() == False))
    OF2asDefault=True

    # Reconstruction method: COF or OF2-NI for collisions, OF2-I for cosmics  
    from TileRecUtils.TileRecFlags import jobproperties
    if not (jobproperties.TileRecFlags.doTileFlat                \
            or jobproperties.TileRecFlags.doTileFit              \
            or jobproperties.TileRecFlags.doTileFitCool          \
            or jobproperties.TileRecFlags.doTileOpt              \
            or jobproperties.TileRecFlags.doTileOF1              \
            or jobproperties.TileRecFlags.doTileOpt2             \
            or (hasattr(jobproperties.TileRecFlags, 'doTileQIE') \
                and jobproperties.TileRecFlags.doTileQIE)        \
            or jobproperties.TileRecFlags.doTileOptATLAS         \
            or jobproperties.TileRecFlags.doTileMF):
        if jobproperties.Beam.beamType == 'collisions':
            if OF2asDefault: 
                jobproperties.TileRecFlags.doTileOptATLAS = True
            else:
                jobproperties.TileRecFlags.doTileMF = True
        else:
            jobproperties.TileRecFlags.doTileOpt2 = True
    
    if jobproperties.Beam.beamType == 'collisions':
        if jobproperties.Beam.bunchSpacing.get_Value()<75:
            from Digitization.DigitizationFlags import digitizationFlags
            halfBS=float(digitizationFlags.bunchSpacing.get_Value())
            if halfBS>0.0:
                if digitizationFlags.BeamIntensityPattern.statusOn:
                    pat=digitizationFlags.BeamIntensityPattern.get_Value()
                    if len(pat)>1:
                        filled=0
                        for p in xrange(len(pat)):
                            if float(pat[p]) > 0.0:
                                if filled > 0:
                                    halfBS/=2.
                                    break
                                else:
                                    filled=1
                            else:
                                filled=0
                        if halfBS>25.0: halfBS=25.0
                    else:
                        halfBS/=2.
                else:
                    halfBS/=2.
                mlog.info("Setting max/min time for parabolic correction to +/- %4.1f ns" % halfBS)
                jobproperties.TileRecFlags.TimeMinForAmpCorrection = -halfBS
                jobproperties.TileRecFlags.TimeMaxForAmpCorrection = halfBS
    
    from TileRecUtils.TileRawChannelGetter import *
    theTileRawChannelGetter=TileRawChannelGetter()
    
    jobproperties.TileRecFlags.TileRawChannelContainer = "TileRawChannelCnt"
    from Digitization.DigitizationFlags import digitizationFlags
    if digitizationFlags.doDigiTruth():
      from TileRecUtils.TileRawChannelGetter_DigiHSTruth import *
      theTileRawChannelGetter_DigiHSTruth=TileRawChannelGetter_DigiHSTruth()
      theTileRawChannelGetter_DigiHSTruth.TileDigitsContainer = "TileDigitsCnt_DigiHSTruth"
      theTileRawChannelGetter_DigiHSTruth.TileRawChannelContainer = "TileRawChannelCnt_DigiHSTruth"

    TileRChMaker = theTileRawChannelGetter.TileRChMaker()

    TileRawChannelBuilderOptATLAS = None
    if jobproperties.TileRecFlags.doTileOptATLAS():
        TileRawChannelBuilderOptATLAS = TileRChMaker.TileRawChannelBuilder['TileRawChannelBuilderOptATLAS']

    TileRawChannelBuilderOpt2Filter = None
    if jobproperties.TileRecFlags.doTileOpt2():
        TileRawChannelBuilderOpt2Filter = TileRChMaker.TileRawChannelBuilder['TileRawChannelBuilderOpt2Filter']

    TileRawChannelBuilderMF = None
    if jobproperties.TileRecFlags.doTileMF():
        TileRawChannelBuilderMF = TileRChMaker.TileRawChannelBuilder['TileRawChannelBuilderMF']

    # make sure that only one output container has default name "TileRawChannelCnt"
    if jobproperties.Beam.beamType == 'collisions': 
        if OF2asDefault:
            if jobproperties.TileRecFlags.doTileOptATLAS():
                TileRawChannelBuilderOptATLAS.TileRawChannelContainer = jobproperties.TileRecFlags.TileRawChannelContainer()
                if jobproperties.TileRecFlags.doTileMF():
                    TileRawChannelBuilderMF.TileRawChannelContainer = "TileRawChannelMF"
                if jobproperties.TileRecFlags.doTileOpt2():
                    TileRawChannelBuilderOpt2Filter.TileRawChannelContainer = "TileRawChannelOpt2"
            elif jobproperties.TileRecFlags.doTileMF():
                TileRawChannelBuilderMF.TileRawChannelContainer = jobproperties.TileRecFlags.TileRawChannelContainer()
            elif jobproperties.TileRecFlags.doTileOpt2():
                TileRawChannelBuilderOpt2Filter.TileRawChannelContainer = jobproperties.TileRecFlags.TileRawChannelContainer()
        else:
            if jobproperties.TileRecFlags.doTileMF():
                TileRawChannelBuilderMF.TileRawChannelContainer = jobproperties.TileRecFlags.TileRawChannelContainer()
                if jobproperties.TileRecFlags.doTileOptATLAS():
                    TileRawChannelBuilderOptATLAS.TileRawChannelContainer = "TileRawChannelFixed"
                if jobproperties.TileRecFlags.doTileOpt2():
                    TileRawChannelBuilderOpt2Filter.TileRawChannelContainer = "TileRawChannelOpt2"
            elif jobproperties.TileRecFlags.doTileOptATLAS():
                TileRawChannelBuilderOptATLAS.TileRawChannelContainer = jobproperties.TileRecFlags.TileRawChannelContainer()
            elif jobproperties.TileRecFlags.doTileOpt2():
                TileRawChannelBuilderOpt2Filter.TileRawChannelContainer = jobproperties.TileRecFlags.TileRawChannelContainer()
    else:
        if jobproperties.TileRecFlags.doTileOpt2():
            TileRawChannelBuilderOpt2Filter.TileRawChannelContainer = jobproperties.TileRecFlags.TileRawChannelContainer()
            if jobproperties.TileRecFlags.doTileMF():
                TileRawChannelBuilderMF.TileRawChannelContainer = "TileRawChannelMF"
            if jobproperties.TileRecFlags.doTileOptATLAS():
                TileRawChannelBuilderOptATLAS.TileRawChannelContainer = "TileRawChannelFixed"
        elif jobproperties.TileRecFlags.doTileOptATLAS():
            TileRawChannelBuilderOptATLAS.TileRawChannelContainer = jobproperties.TileRecFlags.TileRawChannelContainer()
        elif jobproperties.TileRecFlags.doTileMF():
            TileRawChannelBuilderMF.TileRawChannelContainer = jobproperties.TileRecFlags.TileRawChannelContainer()

    if TileRawChannelBuilderOpt2Filter:
        TileRawChannelBuilderOpt2Filter.DSPContainer = ''
    if TileRawChannelBuilderOptATLAS:
        TileRawChannelBuilderOptATLAS.DSPContainer = ''
    if TileRawChannelBuilderMF:
        TileRawChannelBuilderMF.DSPContainer = ''

# Change default parameters for TileDQstatusAlg.
from TileRecUtils.TileDQstatusAlgDefault import TileDQstatusAlgDefault
dqstatus = TileDQstatusAlgDefault()
dqstatus.TileBeamElemContainer=""; # disable reading of trigger type from BeamElem container
dqstatus.TileDigitsContainer="";   # disable checking of Digits container size for bi-gain mode
dqstatus.TileRawChannelContainer=""; # disable checking of DQstatus for simulated data


#
include( "TileSimAlgs/TileSamplingFraction_jobOptions.py" )

if jobproperties.TileRecFlags.doTileMF():
    print  TileRawChannelBuilderMF
if jobproperties.TileRecFlags.doTileOptATLAS():
    print  TileRawChannelBuilderOptATLAS
if jobproperties.TileRecFlags.doTileOpt2():
    print  TileRawChannelBuilderOpt2Filter
