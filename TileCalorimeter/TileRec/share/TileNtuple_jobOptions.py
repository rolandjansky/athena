#-------------------------------
# Create TileCal testbeam ntuple
#-------------------------------

from TileRec.TileRecConf import TileAANtuple
TileNtuple = TileAANtuple("TileNtuple")

from AthenaCommon.AlgSequence import AlgSequence
topSequence = AlgSequence()
topSequence += TileNtuple

TileNtuple.TileDigitsContainer = ""
TileNtuple.TileDigitsContainerFlt = ""
TileNtuple.TileRawChannelContainer = ""
TileNtuple.TileRawChannelContainerFit  = ""
TileNtuple.TileRawChannelContainerFitCool  = ""
TileNtuple.TileRawChannelContainerOpt  = ""
TileNtuple.TileRawChannelContainerQIE  = ""
TileNtuple.TileRawChannelContainerOF1  = ""
TileNtuple.TileRawChannelContainerMF = ""
TileNtuple.TileRawChannelContainerWiener = ""
TileNtuple.TileRawChannelContainerDsp  = ""
TileNtuple.TileLaserObject = ""
TileNtuple.TileMuRcvRawChannelContainer= ""
TileNtuple.TileMuRcvDigitsContainer= ""
TileNtuple.TileMuRcvContainer = ""

from TileRecUtils.TileRecFlags import jobproperties

if ('doSim' in dir()) and doSim:

    TileNtuple.BSInput = FALSE
    TileNtuple.CalibMode = FALSE
    TileNtuple.CalibrateEnergy = TRUE
    if ('ReadRch' in dir()) and not ReadRch: 
        TileNtuple.TileRawChannelContainer  = ""
    else:
        TileNtuple.TileRawChannelContainer  = "TileRawChannelCnt"
    TileNtuple.TileBeamElemContainer = ""
    if jobproperties.TileRecFlags.readDigits:
        TileNtuple.TileDigitsContainer = "TileDigitsCnt"
    else:
        TileNtuple.TileDigitsContainerFlt = "TileDigitsFlt"
else:

    if ('doRecoESD' in dir()) and doRecoESD:
        TileNtuple.BSInput = FALSE
        if ('ReadRch' in dir()) and ReadRch: 
            TileNtuple.TileRawChannelContainer  = "TileRawChannelFlt"
        if jobproperties.TileRecFlags.readDigits:
            TileNtuple.TileDigitsContainer = ""
            TileNtuple.TileDigitsContainerFlt = "TileDigitsFlt"
    else:

        TileNtuple.BSInput = TRUE
        if jobproperties.TileRecFlags.readDigits:
            TileNtuple.TileDigitsContainer = "TileDigitsCnt"
        elif ('ReadPool' in dir()) and ReadPool: 
            TileNtuple.TileDigitsContainerFlt = "TileDigitsFlt"
            TileNtuple.TileBeamElemContainer = "TileBeamElemCnt"


    if doTileFlat:
        TileNtuple.TileRawChannelContainer = "TileRawChannelFlat"

    if doTileOptATLAS:
        TileNtuple.TileRawChannelContainer  = "TileRawChannelFixed"
    
    if doTileFit:
        TileNtuple.TileRawChannelContainerFit  = "TileRawChannelFit"
    
    if doTileFitCool:
        TileNtuple.TileRawChannelContainerFitCool  = "TileRawChannelFitCool"
    
    if doTileOpt:
        TileNtuple.TileRawChannelContainerOpt  = "TileRawChannelOpt"

    if doTileOpt2:
        TileNtuple.TileRawChannelContainerOpt = "TileRawChannelOpt2"
        
    if doTileQIE:
        TileNtuple.TileRawChannelContainerQIE = "TileRawChannelQIE"
#        TileNtuple.TileRawChannelContainer = ""
        
    if doTileOF1:
        TileNtuple.TileRawChannelContainerOF1 = "TileRawChannelOF1"        

    if doTileMF:
        TileNtuple.TileRawChannelContainerMF = "TileRawChannelMF"

    if doTileWiener:
        TileNtuple.TileRawChannelContainerWiener = "TileRawChannelWiener"

    if useTMDB:
        TileNtuple.TileMuRcvRawChannelContainer = "MuRcvRawChCnt"
        TileNtuple.TileMuRcvDigitsContainer = "MuRcvDigitsCnt"
        TileNtuple.TileMuRcvContainer = "TileMuRcvCnt"

    if TileCisRun:
        # parameters for bigain CIS run
        if not 'TileCalibMode' in dir():
            TileNtuple.CalibMode = TRUE
        else:
            TileNtuple.CalibMode = TileCalibMode

        # do not apply Cesium calibration and pCb/GeV factor
        if not 'TileCalibrateEnergy' in dir():
            TileNtuple.CalibrateEnergy = FALSE
        else:
            TileNtuple.CalibrateEnergy = TileCalibrateEnergy

    elif TilePedRun:
        # parameters for bigain Ped run
        if not 'TileCalibMode' in dir():
            TileNtuple.CalibMode = TRUE
        else:
            TileNtuple.CalibMode = TileCalibMode

        # apply Cesium calibration and pCb/GeV factor to have energy in GeV
        if not 'TileCalibrateEnergy' in dir():
            TileNtuple.CalibrateEnergy = TRUE
        else:
            TileNtuple.CalibrateEnergy = TileCalibrateEnergy

        TileNtuple.TileLaserObject = ""

    elif TileLasRun:
        # parameters for Laser run
        if not 'TileCalibMode' in dir():
            TileNtuple.CalibMode = FALSE
        else:
            TileNtuple.CalibMode = TileCalibMode

        # apply Cesium calibration and pCb/GeV factor to have energy in GeV
        if not 'TileCalibrateEnergy' in dir():
            TileNtuple.CalibrateEnergy = TRUE
        else:
            TileNtuple.CalibrateEnergy = TileCalibrateEnergy

        TileNtuple.TileLaserObject = "TileLaserObj"
            
    else:
        # parameters for Phys run
        if not 'TileCalibMode' in dir():
            TileNtuple.CalibMode = FALSE
        else:
            TileNtuple.CalibMode = TileCalibMode

        # apply Cesium calibration and pCb/GeV factor to have energy in GeV
        if not 'TileCalibrateEnergy' in dir():
            TileNtuple.CalibrateEnergy = TRUE
        else:
            TileNtuple.CalibrateEnergy = TileCalibrateEnergy

        TileNtuple.TileLaserObject = ""
          

