#-------------------------------
# Create TileCal testbeam ntuple
#-------------------------------
from TileTBRec.TileTBRecConf import TileTBAANtuple
TileTBNtuple = TileTBAANtuple("TileTBNtuple")

from AthenaCommon.AlgSequence import AlgSequence
topSequence = AlgSequence()
topSequence += TileTBNtuple

# in which order fragments appear in the ntuple
# this is not needed for 2004 data, drawer list will be extracted from the data
# TileTBNtuple.drawerList = [ "0x200", "0x201", "0x202", "0x100", "0x101", "0x102" ]

# fragment type (==PARTITION in ATLAS) 1 - LBA, 2-LBC, 3-EBA, 4-EBC
# this is not needed for 2004 data, because drawerType can be calculated from drawerList
# and it is only needed for june-july 2003 (runs with ext. barrels)
# line below is for 2003 june-july data
# TileTBNtuple.drawerType = [ 2, 2, 4, 1, 1, 3 ]
# if drawerType is 0, then channel number is not convereted to pmt number
# i.e. array of 48 is ordered according to channel number (useful for CIS)
# TileTBNtuple.drawerType = [ 0, 0, 0, 0, 0, 0 ]

#it is possible to have ntuple with only one drawer, e.g
# TileTBNtuple.drawerList = [ "0x200" ]
#but then, it's probably preferable to have channel-type ordering instead of PMT-ordering
# TileTBNtuple.drawerType = [ 0 ]

# if we put 0 as number of samples, it'll be taken from the data
# (7 samples for CIS, 9 samples for physics)
# default value in 9 samples, i.e. last two samples in CIS will contain zeros
# TileTBNtuple.NSamples = 0

# prefix for array name in the ntuple (for different drawer types listed above)
# for 2003               beam  B+   B-   EB+  EB-
#TileTBNtuple.rosName = [ "B", "P", "N", "P", "N" ]
# for 2004 to avoid mismatch with other systems, do not use Neg and Pos names 
TileTBNtuple.rosName = [ "B", "A", "C", "D", "E" ]

TileTBNtuple.TileRawChannelContainerFlat = ""
TileTBNtuple.TileRawChannelContainerFit  = ""
TileTBNtuple.TileRawChannelContainerFitCool  = ""
TileTBNtuple.TileRawChannelContainerOpt  = ""

if doSim:
    theApp.Dlls += [ "TileRecUtils", "TileByteStream" ]
    TileTBNtuple.BSInput = FALSE
    TileTBNtuple.NSamples = -1
    TileTBNtuple.CompleteNtuple = FALSE
    TileTBNtuple.CalibMode = FALSE
    TileTBNtuple.CalibrateEnergy = TRUE
    TileTBNtuple.beamFragList = [ "0x001", "0x002", "0x003", "0x007" ]
    TileTBNtuple.UnpackAdder = FALSE
    TileTBNtuple.TileRawChannelContainerFit  = "TileRawChannelFit"
    # TileTBNtuple.TileRawChannelContainerFitCool  = "TileRawChannelFitCool"
    # TileTBNtuple.TileRawChannelContainerFlat = "TileRawChannelCnt"
    TileTBNtuple.TileDigitsContainer = ""
    TileTBNtuple.TileBeamElemContainer = ""
    TileTBNtuple.TileLaserObject = ""
    if TileBarrelOnly:
        TileTBNtuple.drawerList = [ "0x200", "0x201", "0x202", "0x100", "0x101", "0x102" ]
        TileTBNtuple.drawerType = [ 2, 2, 2, 1, 1, 1 ]
    else:
        if Tile2004:
            TileTBNtuple.drawerList = [ "0x200", "0x201", "0x202", "0x400", "0x401", "0x402", "0x100", "0x101", "0x102" ]
            TileTBNtuple.drawerType = [ 2, 2, 2, 4, 4, 4, 1, 1, 1 ]
        else:
            TileTBNtuple.drawerList = [ "0x200", "0x201", "0x400", "0x100", "0x101", "0x300" ]
            TileTBNtuple.drawerType = [ 2, 2, 4, 1, 1, 3 ]
else:
    if doTileFlat:
        TileTBNtuple.TileRawChannelContainerFlat = "TileRawChannelFlat"

    if doTileOptATLAS:
        TileTBNtuple.TileRawChannelContainerFlat  = "TileRawChannelOptFixed"
    
    if doTileFit:
        TileTBNtuple.TileRawChannelContainerFit  = "TileRawChannelFit"
    
    if doTileFitCool:
        TileTBNtuple.TileRawChannelContainerFitCool  = "TileRawChannelFitCool"
    
    if doTileOpt:
        TileTBNtuple.TileRawChannelContainerOpt  = "TileRawChannelOpt"

    if doTileOpt2:
        TileTBNtuple.TileRawChannelContainerOpt = "TileRawChannelOpt2"

    if Tile2004:
        TileTBNtuple.UnpackAdder = FALSE
    else:
        TileTBNtuple.UnpackAdder = TRUE
        TileTBNtuple.TileAdderFlatFilter.TileRawChannelContainer = "TileAdderFlat"
        if TileBarrelOnly:
            TileTBNtuple.drawerList = [ "0x200", "0x201", "0x202", "0x100", "0x101", "0x102" ]
            TileTBNtuple.drawerType = [ 2, 2, 2, 1, 1, 1 ]
        else:
            TileTBNtuple.drawerList = [ "0x200", "0x201", "0x202", "0x100", "0x101", "0x102" ]
            TileTBNtuple.drawerType = [ 2, 2, 4, 1, 1, 3 ]

    if TileCisRun:
        # parameters for CIS run with 7 samples
        if not 'TileCalibMode' in dir():
            TileTBNtuple.CalibMode = TRUE
        else:
            TileTBNtuple.CalibMode = TileCalibMode
        if not 'TileFrameLength' in dir():
            TileTBNtuple.NSamples = 7
        else:
            TileTBNtuple.NSamples = TileFrameLength
        # do not apply Cesium calibration and pCb/GeV factor
        if not 'TileCalibrateEnergy' in dir():
            TileTBNtuple.CalibrateEnergy = FALSE
        else:
            TileTBNtuple.CalibrateEnergy = TileCalibrateEnergy
        TileTBNtuple.TileLaserObject = ""

    elif TilePedRun:
        # parameters for Ped run with 7 samples
        if not 'TileCalibMode' in dir():
            TileTBNtuple.CalibMode = TRUE
        else:
            TileTBNtuple.CalibMode = TileCalibMode
        if not 'TileFrameLength' in dir():
            TileTBNtuple.NSamples = 7
        else:
            TileTBNtuple.NSamples = TileFrameLength
        # apply Cesium calibration and pCb/GeV factor to have energy in GeV
        if not 'TileCalibrateEnergy' in dir():
            TileTBNtuple.CalibrateEnergy = TRUE
        else:
            TileTBNtuple.CalibrateEnergy = TileCalibrateEnergy
        TileTBNtuple.TileLaserObject = ""

    elif TileLasRun:
        # parameters for Laser run with 9 samples
        if not 'TileCalibMode' in dir():
            TileTBNtuple.CalibMode = FALSE
        else:
            TileTBNtuple.CalibMode = TileCalibMode
        if not 'TileFrameLength' in dir():
            TileTBNtuple.NSamples = 9
        else:
            TileTBNtuple.NSamples = TileFrameLength
        # apply Cesium calibration and pCb/GeV factor to have energy in GeV
        if not 'TileCalibrateEnergy' in dir():
            TileTBNtuple.CalibrateEnergy = TRUE
        else:
            TileTBNtuple.CalibrateEnergy = TileCalibrateEnergy
        TileTBNtuple.TileLaserObject = "TileLaserObj"
            
    else:
        # parameters for Phys run with 9 samples
        if not 'TileCalibMode' in dir():
            TileTBNtuple.CalibMode = FALSE
        else:
            TileTBNtuple.CalibMode = TileCalibMode
        if not 'TileFrameLength' in dir():
            TileTBNtuple.NSamples = 9
        else:
            TileTBNtuple.NSamples = TileFrameLength
        # apply Cesium calibration and pCb/GeV factor to have energy in GeV
        if not 'TileCalibrateEnergy' in dir():
            TileTBNtuple.CalibrateEnergy = TRUE
        else:
            TileTBNtuple.CalibrateEnergy = TileCalibrateEnergy
        TileTBNtuple.TileLaserObject = ""
            
if Tile2004:
    # calib coeff for beam chambers in JULY 2004
    #
    TileTBNtuple.BN2X1 = -0.522
    TileTBNtuple.BN2X2 = -0.175 
    TileTBNtuple.BN2Y1 = -1.44  
    TileTBNtuple.BN2Y2 = -0.173
    TileTBNtuple.BN2Z  =  1.0  

    TileTBNtuple.BN1X1 = 0.309
    TileTBNtuple.BN1X2 = -0.173 
    TileTBNtuple.BN1Y1 = 0.561
    TileTBNtuple.BN1Y2 = -0.175 
    TileTBNtuple.BN1Z  =  1.0  

    TileTBNtuple.BC0X1 = -0.111
    TileTBNtuple.BC0X2 =  0.185
    TileTBNtuple.BC0Y1 =  0.028
    TileTBNtuple.BC0Y2 =  0.181
    TileTBNtuple.BC0Z  =  17138.0

    TileTBNtuple.BC1X1 = -5.56 
    TileTBNtuple.BC1X2 =  0.183
    TileTBNtuple.BC1Y1 = -3.04 
    TileTBNtuple.BC1Y2 =  0.180 
    TileTBNtuple.BC1Z  =  13788.0

    TileTBNtuple.BC2X1 = 2.22 
    TileTBNtuple.BC2X2 = -0.195  
    TileTBNtuple.BC2Y1 = -1.91 
    TileTBNtuple.BC2Y2 = -0.188  
    TileTBNtuple.BC2Zperiod1  =  9411.0
    TileTBNtuple.BC2Zperiod2  =  12188.0
    
    # inner radius of sensitive part of TileCal
    TileTBNtuple.Radius = 2300.0

else:
    # calib coeff for beam chambers from run 320082
    #
    TileTBNtuple.BC1X1 = -0.938  
    TileTBNtuple.BC1X2 =  0.1747 
    TileTBNtuple.BC1Y1 =  0.125  
    TileTBNtuple.BC1Y2 =  0.1765 
    TileTBNtuple.BC1Z  =  10385.05  

    TileTBNtuple.BC2X1 = -0.9369
    TileTBNtuple.BC2X2 =  0.191  
    TileTBNtuple.BC2Y1 = -1.29  
    TileTBNtuple.BC2Y2 =  0.187  
    TileTBNtuple.BC2Z  =  2085.0

