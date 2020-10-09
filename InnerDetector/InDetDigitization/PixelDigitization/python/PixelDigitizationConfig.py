# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

from __future__ import print_function

from AthenaCommon import CfgMgr
from Digitization.DigitizationFlags import digitizationFlags

# The earliest bunch crossing time for which interactions will be sent
# to the Pixel Digitization code.
def Pixel_FirstXing():
    FirstXing = -50
    from AthenaCommon.BeamFlags import jobproperties
    if jobproperties.Beam.estimatedLuminosity()> 0.5e33:
        FirstXing = -25
    return FirstXing

# The latest bunch crossing time for which interactions will be sent
# to the Pixel Digitization code.
def Pixel_LastXing():
    LastXing = 100
    from AthenaCommon.BeamFlags import jobproperties
    if jobproperties.Beam.estimatedLuminosity()> 0.5e33:
        if jobproperties.Beam.bunchSpacing.get_Value() > 50 :
            LastXing = 75
        else :
            LastXing = 25
    return LastXing

###############################################################################

def RadDamageUtil(name="RadDamageUtil", **kwargs):
    kwargs.setdefault("defaultRamo", 1)
    kwargs.setdefault("betaElectrons", 4.5e-16)
    kwargs.setdefault("betaHoles", 6.0e-16)
    kwargs.setdefault("saveDebugMaps", False)
    return CfgMgr.RadDamageUtil(name, **kwargs)

def EfieldInterpolator(name="EfieldInterpolator", **kwargs):
    kwargs.setdefault("initialized", False)
    kwargs.setdefault("useSpline", True)
    kwargs.setdefault("sensorDepth", 200)
    return CfgMgr.EfieldInterpolator(name, **kwargs)

def EnergyDepositionTool(name="EnergyDepositionTool", **kwargs):
    kwargs.setdefault("DeltaRayCut", 117.)
    kwargs.setdefault("nCols", 5)
    kwargs.setdefault("LoopLimit", 100000)
    kwargs.setdefault("doBichsel", hasattr(digitizationFlags, "doBichselSimulation") and digitizationFlags.doBichselSimulation())
    kwargs.setdefault("doBichselBetaGammaCut", 0.7)   # dEdx not quite consistent below this
    kwargs.setdefault("doDeltaRay", False)            # needs validation
    kwargs.setdefault("doPU", True)
    return CfgMgr.EnergyDepositionTool(name, **kwargs)

def SensorSimPlanarTool(name="SensorSimPlanarTool", **kwargs):
    from AthenaCommon.AppMgr import ToolSvc
    kwargs.setdefault("SiPropertiesTool", ToolSvc.PixelSiPropertiesTool)
    kwargs.setdefault("LorentzAngleTool", ToolSvc.PixelLorentzAngleTool)
#    kwargs.setdefault("LorentzAngleTool", pixelLorentzAngleToolSetup.PixelLorentzAngleTool)
    return CfgMgr.SensorSimPlanarTool(name, **kwargs)

def SensorSim3DTool(name="SensorSim3DTool", **kwargs):
    from AthenaCommon.AppMgr import ToolSvc
    kwargs.setdefault("SiPropertiesTool", ToolSvc.PixelSiPropertiesTool)
    return CfgMgr.SensorSim3DTool(name, **kwargs)

def SensorSimTool(name="SensorSimTool", **kwargs):
    return CfgMgr.SensorSimTool(name, **kwargs)

def FrontEndSimTool(name="FrontEndSimTool", **kwargs):
    from PixelConditionsTools.PixelConditionsToolsConf import PixelConditionsSummaryTool
    pixelConditionsSummaryToolSetup = PixelConditionsSummaryTool("PixelConditionsSummaryTool", UseByteStream=False)
    kwargs.setdefault("PixelConditionsSummaryTool", pixelConditionsSummaryToolSetup)
    return CfgMgr.FrontEndSimTool(name, **kwargs)

def BarrelRD53SimTool(name="BarrelRD53SimTool", **kwargs):
    kwargs.setdefault("BarrelEC", 0)
    kwargs.setdefault("DoNoise", digitizationFlags.doInDetNoise.get_Value())
    return CfgMgr.RD53SimTool(name, **kwargs)

def EndcapRD53SimTool(name="EndcapRD53SimTool", **kwargs):
    kwargs.setdefault("BarrelEC", 2)
    kwargs.setdefault("DoNoise", digitizationFlags.doInDetNoise.get_Value())
    return CfgMgr.RD53SimTool(name, **kwargs)

def BarrelFEI4SimTool(name="BarrelFEI4SimTool", **kwargs):
    kwargs.setdefault("BarrelEC", 0)
    kwargs.setdefault("DoNoise", digitizationFlags.doInDetNoise.get_Value())
    return CfgMgr.FEI4SimTool(name, **kwargs)

def DBMFEI4SimTool(name="DBMFEI4SimTool", **kwargs):
    kwargs.setdefault("BarrelEC", 4)
    kwargs.setdefault("DoNoise", digitizationFlags.doInDetNoise.get_Value())
    return CfgMgr.FEI4SimTool(name, **kwargs)

def BarrelFEI3SimTool(name="BarrelFEI3SimTool", **kwargs):
    kwargs.setdefault("BarrelEC", 0)
    return CfgMgr.FEI3SimTool(name, **kwargs)

def EndcapFEI3SimTool(name="EndcapFEI3SimTool", **kwargs):
    kwargs.setdefault("BarrelEC", 2)
    return CfgMgr.FEI3SimTool(name, **kwargs)

def BasicPixelDigitizationTool(name="PixelDigitizationTool", **kwargs):
    from AthenaCommon.AppMgr import ServiceMgr
    from AthenaCommon.AppMgr import ToolSvc
    from IOVDbSvc.CondDB import conddb
    from AtlasGeoModel.CommonGMJobProperties import CommonGeometryFlags as commonGeoFlags
    from AtlasGeoModel.InDetGMJobProperties import InDetGeometryFlags as geoFlags
    from AthenaCommon.GlobalFlags import globalflags

############################################################################################
# Set up Pixel Module data (2018 condition)
############################################################################################
    from AthenaCommon.AlgSequence import AthSequencer
    condSeq = AthSequencer("AthCondSeq")

    #################
    # Module status #
    #################
    useNewDeadmapFormat = False
    useNewChargeFormat  = False

    if not useNewDeadmapFormat:
        if not (conddb.folderRequested("/PIXEL/PixMapOverlay") or conddb.folderRequested("/PIXEL/Onl/PixMapOverlay")):
            conddb.addFolderSplitOnline("PIXEL","/PIXEL/Onl/PixMapOverlay","/PIXEL/PixMapOverlay", className='CondAttrListCollection')

    if not hasattr(condSeq, 'PixelConfigCondAlg'):
        from PixelConditionsAlgorithms.PixelConditionsAlgorithmsConf import PixelConfigCondAlg
        PixelConfigCondAlg.UseCablingConditions=False

        from AthenaCommon.BeamFlags import jobproperties
        if jobproperties.Beam.beamType == "cosmics" :
            PixelConfigCondAlg.UseComTime=True
            PixelConfigCondAlg.BarrelTimeJitter=[25.0,25.0,25.0,25.0]
            PixelConfigCondAlg.EndcapTimeJitter=[25.0,25.0,25.0]
            PixelConfigCondAlg.DBMTimeJitter=[25.0,25.0,25.0]
            PixelConfigCondAlg.BarrelNumberOfBCID=[8,8,8,8]
            PixelConfigCondAlg.EndcapNumberOfBCID=[8,8,8]
            PixelConfigCondAlg.DBMNumberOfBCID=[8,8,8]
            PixelConfigCondAlg.BarrelTimeOffset=[100.0,100.0,100.0,100.0]
            PixelConfigCondAlg.EndcapTimeOffset=[100.0,100.0,100.0]
            PixelConfigCondAlg.DBMTimeOffset=[100.0,100.0,100.0]
        else:
            PixelConfigCondAlg.UseComTime=False
            PixelConfigCondAlg.BarrelTimeJitter=[0.0,0.0,0.0,0.0]
            PixelConfigCondAlg.EndcapTimeJitter=[0.0,0.0,0.0]
            PixelConfigCondAlg.DBMTimeJitter=[0.0,0.0,0.0]
            PixelConfigCondAlg.BarrelNumberOfBCID=[1,1,1,1]
            PixelConfigCondAlg.EndcapNumberOfBCID=[1,1,1]
            PixelConfigCondAlg.DBMNumberOfBCID=[1,1,1]
            PixelConfigCondAlg.BarrelTimeOffset=[5.0,5.0,5.0,5.0]
            PixelConfigCondAlg.EndcapTimeOffset=[5.0,5.0,5.0]
            PixelConfigCondAlg.DBMTimeOffset=[5.0,5.0,5.0]

        PixelConfigCondAlg.BunchSpace=25.0
        PixelConfigCondAlg.FEI4BarrelHitDiscConfig=[2]

        #====================================================================================
        # Run-dependent SIMULATION(digitization) parameters:
        #====================================================================================
        # RUN2 2015/2016
        PixelConfigCondAlg.BarrelToTThreshold2016       = [   -1,    5,    5,    5]
        PixelConfigCondAlg.FEI3BarrelLatency2016        = [    0,  151,  256,  256]
        PixelConfigCondAlg.FEI3BarrelHitDuplication2016 = [False,False,False,False]
        PixelConfigCondAlg.FEI3BarrelSmallHitToT2016    = [   -1,   -1,   -1,   -1]
        PixelConfigCondAlg.FEI3BarrelTimingSimTune2016  = [   -1, 2015, 2015, 2015]
        PixelConfigCondAlg.BarrelCrossTalk2016          = [ 0.30, 0.06, 0.06, 0.06]
        PixelConfigCondAlg.BarrelNoiseOccupancy2016     = [ 5e-8, 5e-8, 5e-8, 5e-8]
        PixelConfigCondAlg.BarrelDisableProbability2016 = [ 9e-3, 9e-3, 9e-3, 9e-3]

        PixelConfigCondAlg.EndcapToTThreshold2016       = [    5,    5,    5]
        PixelConfigCondAlg.FEI3EndcapLatency2016        = [  256,  256,  256]
        PixelConfigCondAlg.FEI3EndcapHitDuplication2016 = [False,False,False]
        PixelConfigCondAlg.FEI3EndcapSmallHitToT2016    = [   -1,   -1,   -1]
        PixelConfigCondAlg.FEI3EndcapTimingSimTune2016  = [ 2015, 2015, 2015]
        PixelConfigCondAlg.EndcapCrossTalk2016          = [ 0.06, 0.06, 0.06]
        PixelConfigCondAlg.EndcapNoiseOccupancy2016     = [ 5e-8, 5e-8, 5e-8]
        PixelConfigCondAlg.EndcapDisableProbability2016 = [ 9e-3, 9e-3, 9e-3]

        PixelConfigCondAlg.DBMToTThreshold2016       = [   -1,   -1,   -1]
        PixelConfigCondAlg.DBMCrossTalk2016          = [ 0.06, 0.06, 0.06]
        PixelConfigCondAlg.DBMNoiseOccupancy2016     = [ 5e-8, 5e-8, 5e-8]
        PixelConfigCondAlg.DBMDisableProbability2016 = [ 9e-3, 9e-3, 9e-3]

        PixelConfigCondAlg.IBLNoiseShape2016    = [0.0, 0.0330, 0.0, 0.3026, 0.5019, 0.6760, 0.8412, 0.9918, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 1.0]
        PixelConfigCondAlg.BLayerNoiseShape2016 = [0.0, 0.0, 0.0, 0.0, 0.2204, 0.5311, 0.7493, 0.8954, 0.9980, 1.0]
        PixelConfigCondAlg.PixelNoiseShape2016  = [0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.2418, 0.4397, 0.5858, 0.6949, 0.7737, 0.8414, 0.8959, 0.9414, 0.9828, 1.0]
        # Layer-2 noise shape                     [0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.2129, 0.4016, 0.5477, 0.6599, 0.7435, 0.8160, 0.8779, 0.9340, 0.9798, 1.0]

        # So far, Gaudi::Property does not support 2D vector.
        #PixelConfigCondAlg.EndcapNoiseShape=[[0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.1748, 0.3409, 0.4760, 0.5850, 0.6754, 0.7538, 0.8264, 0.8962, 0.9655, 1.0],
        #                                     [0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.1852, 0.3528, 0.4881, 0.5961, 0.6855, 0.7640, 0.8374, 0.9068, 0.9749, 1.0],
        #                                     [0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.1735, 0.3380, 0.4733, 0.5829, 0.6730, 0.7516, 0.8234, 0.8916, 0.9595, 1.0]]

        #====================================================================================
        # RUN2 2017
        PixelConfigCondAlg.BarrelToTThreshold2017       = [   -1,    5,    5,    5]
        PixelConfigCondAlg.FEI3BarrelLatency2017        = [    0,  151,  256,  256]
        PixelConfigCondAlg.FEI3BarrelHitDuplication2017 = [False,False,False,False]
        PixelConfigCondAlg.FEI3BarrelSmallHitToT2017    = [   -1,   -1,   -1,   -1]
        PixelConfigCondAlg.FEI3BarrelTimingSimTune2017  = [   -1, 2018, 2018, 2018]
        PixelConfigCondAlg.BarrelCrossTalk2017          = [ 0.30, 0.06, 0.06, 0.06]
        PixelConfigCondAlg.BarrelNoiseOccupancy2017     = [ 5e-8, 5e-8, 5e-8, 5e-8]
        PixelConfigCondAlg.BarrelDisableProbability2017 = [ 9e-3, 9e-3, 9e-3, 9e-3]

        PixelConfigCondAlg.EndcapToTThreshold2017       = [    5,    5,    5]
        PixelConfigCondAlg.FEI3EndcapLatency2017        = [  256,  256,  256]
        PixelConfigCondAlg.FEI3EndcapHitDuplication2017 = [False,False,False]
        PixelConfigCondAlg.FEI3EndcapSmallHitToT2017    = [   -1,   -1,   -1]
        PixelConfigCondAlg.FEI3EndcapTimingSimTune2017  = [ 2018, 2018, 2018]
        PixelConfigCondAlg.EndcapCrossTalk2017          = [ 0.06, 0.06, 0.06]
        PixelConfigCondAlg.EndcapNoiseOccupancy2017     = [ 5e-8, 5e-8, 5e-8]
        PixelConfigCondAlg.EndcapDisableProbability2017 = [ 9e-3, 9e-3, 9e-3]

        PixelConfigCondAlg.DBMToTThreshold2017       = [   -1,   -1,   -1]
        PixelConfigCondAlg.DBMCrossTalk2017          = [ 0.06, 0.06, 0.06]
        PixelConfigCondAlg.DBMNoiseOccupancy2017     = [ 5e-8, 5e-8, 5e-8]
        PixelConfigCondAlg.DBMDisableProbability2017 = [ 9e-3, 9e-3, 9e-3]

        PixelConfigCondAlg.IBLNoiseShape2017    = [0.0, 0.0330, 0.0, 0.3026, 0.5019, 0.6760, 0.8412, 0.9918, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 1.0]
        PixelConfigCondAlg.BLayerNoiseShape2017 = [0.0, 0.0, 0.0, 0.0, 0.2204, 0.5311, 0.7493, 0.8954, 0.9980, 1.0]
        PixelConfigCondAlg.PixelNoiseShape2017  = [0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.2418, 0.4397, 0.5858, 0.6949, 0.7737, 0.8414, 0.8959, 0.9414, 0.9828, 1.0]

        #====================================================================================
        # RUN2 2018
        PixelConfigCondAlg.BarrelToTThreshold2018       = [   -1,    3,    5,    5]
        PixelConfigCondAlg.FEI3BarrelLatency2018        = [    0,  151,  256,  256]
        PixelConfigCondAlg.FEI3BarrelHitDuplication2018 = [False,False,False,False]
        PixelConfigCondAlg.FEI3BarrelSmallHitToT2018    = [   -1,   -1,   -1,   -1]
        PixelConfigCondAlg.FEI3BarrelTimingSimTune2018  = [   -1, 2018, 2018, 2018]
        PixelConfigCondAlg.BarrelCrossTalk2018          = [ 0.30, 0.06, 0.06, 0.06]
        PixelConfigCondAlg.BarrelNoiseOccupancy2018     = [ 5e-8, 5e-8, 5e-8, 5e-8]
        PixelConfigCondAlg.BarrelDisableProbability2018 = [ 9e-3, 9e-3, 9e-3, 9e-3]

        PixelConfigCondAlg.EndcapToTThreshold2018       = [    5,    5,    5]
        PixelConfigCondAlg.FEI3EndcapLatency2018        = [  256,  256,  256]
        PixelConfigCondAlg.FEI3EndcapHitDuplication2018 = [False,False,False]
        PixelConfigCondAlg.FEI3EndcapSmallHitToT2018    = [   -1,   -1,   -1]
        PixelConfigCondAlg.FEI3EndcapTimingSimTune2018  = [ 2018, 2018, 2018]
        PixelConfigCondAlg.EndcapCrossTalk2018          = [ 0.06, 0.06, 0.06]
        PixelConfigCondAlg.EndcapNoiseOccupancy2018     = [ 5e-8, 5e-8, 5e-8]
        PixelConfigCondAlg.EndcapDisableProbability2018 = [ 9e-3, 9e-3, 9e-3]

        PixelConfigCondAlg.DBMToTThreshold2018       = [   -1,   -1,   -1]
        PixelConfigCondAlg.DBMCrossTalk2018          = [ 0.06, 0.06, 0.06]
        PixelConfigCondAlg.DBMNoiseOccupancy2018     = [ 5e-8, 5e-8, 5e-8]
        PixelConfigCondAlg.DBMDisableProbability2018 = [ 9e-3, 9e-3, 9e-3]

        PixelConfigCondAlg.IBLNoiseShape2018    = [0.0, 0.0330, 0.0, 0.3026, 0.5019, 0.6760, 0.8412, 0.9918, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 1.0]
        PixelConfigCondAlg.BLayerNoiseShape2018 = [0.0, 0.0, 0.0, 0.0, 0.2204, 0.5311, 0.7493, 0.8954, 0.9980, 1.0]
        PixelConfigCondAlg.PixelNoiseShape2018  = [0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.2418, 0.4397, 0.5858, 0.6949, 0.7737, 0.8414, 0.8959, 0.9414, 0.9828, 1.0]

        #====================================================================================
        # RUN1
        PixelConfigCondAlg.BarrelToTThresholdRUN1       = [    3,    3,    3]
        PixelConfigCondAlg.FEI3BarrelLatencyRUN1        = [  256,  256,  256]
        PixelConfigCondAlg.FEI3BarrelHitDuplicationRUN1 = [ True, True, True]
        PixelConfigCondAlg.FEI3BarrelSmallHitToTRUN1    = [    7,    7,    7]
        PixelConfigCondAlg.FEI3BarrelTimingSimTuneRUN1  = [ 2009, 2009, 2009]
        PixelConfigCondAlg.BarrelCrossTalkRUN1          = [ 0.06, 0.06, 0.06]
        PixelConfigCondAlg.BarrelNoiseOccupancyRUN1     = [ 5e-8, 5e-8, 5e-8]
        PixelConfigCondAlg.BarrelDisableProbabilityRUN1 = [ 9e-3, 9e-3, 9e-3]

        PixelConfigCondAlg.EndcapToTThresholdRUN1       = [    3,    3,    3]
        PixelConfigCondAlg.FEI3EndcapLatencyRUN1        = [  256,  256,  256]
        PixelConfigCondAlg.FEI3EndcapHitDuplicationRUN1 = [ True, True, True]
        PixelConfigCondAlg.FEI3EndcapSmallHitToTRUN1    = [    7,    7,    7]
        PixelConfigCondAlg.FEI3EndcapTimingSimTuneRUN1  = [ 2009, 2009, 2009]
        PixelConfigCondAlg.EndcapCrossTalkRUN1          = [ 0.06, 0.06, 0.06]
        PixelConfigCondAlg.EndcapNoiseOccupancyRUN1     = [ 5e-8, 5e-8, 5e-8]
        PixelConfigCondAlg.EndcapDisableProbabilityRUN1 = [ 9e-3, 9e-3, 9e-3]

        PixelConfigCondAlg.BLayerNoiseShapeRUN1 = [0.0, 0.0, 0.0, 0.0, 0.2204, 0.5311, 0.7493, 0.8954, 0.9980, 1.0]
        PixelConfigCondAlg.PixelNoiseShapeRUN1  = [0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.2418, 0.4397, 0.5858, 0.6949, 0.7737, 0.8414, 0.8959, 0.9414, 0.9828, 1.0]

        #====================================================================================
        # ITK
        PixelConfigCondAlg.BarrelToTThresholdITK       = [    3,    3,    3,    3,    3]
        PixelConfigCondAlg.BarrelCrossTalkITK          = [ 0.06, 0.06, 0.06, 0.06, 0.06]
        PixelConfigCondAlg.BarrelNoiseOccupancyITK     = [ 5e-8, 5e-8, 5e-8, 5e-8, 5e-8]
        PixelConfigCondAlg.BarrelDisableProbabilityITK = [ 9e-3, 9e-3, 9e-3, 9e-3, 9e-3]

        PixelConfigCondAlg.EndcapToTThresholdITK       = [    3,    3,    3,    3,    3,    3,    3,    3,    3,    3,    3,    3,    3,    3]
        PixelConfigCondAlg.EndcapCrossTalkITK          = [ 0.06, 0.06, 0.06, 0.06, 0.06, 0.06, 0.06, 0.06, 0.06, 0.06, 0.06, 0.06, 0.06, 0.06]
        PixelConfigCondAlg.EndcapNoiseOccupancyITK     = [ 5e-8, 5e-8, 5e-8, 5e-8, 5e-8, 5e-8, 5e-8, 5e-8, 5e-8, 5e-8, 5e-8, 5e-8, 5e-8, 5e-8]
        PixelConfigCondAlg.EndcapDisableProbabilityITK = [ 9e-3, 9e-3, 9e-3, 9e-3, 9e-3, 9e-3, 9e-3, 9e-3, 9e-3, 9e-3, 9e-3, 9e-3, 9e-3, 9e-3]

        PixelConfigCondAlg.InnermostNoiseShapeITK     = [0.0, 1.0]
        PixelConfigCondAlg.NextInnermostNoiseShapeITK = [0.0, 1.0]
        PixelConfigCondAlg.PixelNoiseShapeITK         = [0.0, 1.0]


        IdMappingDat="PixelCabling/Pixels_Atlas_IdMapping_2016.dat"

        # ITk:
        if geoFlags.isSLHC():
            IdMappingDat = "ITk_Atlas_IdMapping.dat"
            if "BrlIncl4.0_ref" == commonGeoFlags.GeoType():
                IdMappingDat = "ITk_Atlas_IdMapping_InclBrl4.dat"
            elif "IBrlExt4.0ref" == commonGeoFlags.GeoType():
                IdMappingDat = "ITk_Atlas_IdMapping_IExtBrl4.dat"
            elif "BrlExt4.0_ref" == commonGeoFlags.GeoType():
                IdMappingDat = "ITk_Atlas_IdMapping_ExtBrl4.dat"
            elif "BrlExt3.2_ref" == commonGeoFlags.GeoType():
                IdMappingDat = "ITk_Atlas_IdMapping_ExtBrl32.dat"
        elif not geoFlags.isIBL():
            IdMappingDat="PixelCabling/Pixels_Atlas_IdMapping.dat"
        else:
            # Planar IBL
            if (geoFlags.IBLLayout() == "planar"):
                if geoFlags.isDBM():
                    IdMappingDat="PixelCabling/Pixels_Atlas_IdMapping_inclIBL_DBM.dat"
                else:
                    IdMappingDat="PixelCabling/Pixels_Atlas_IdMapping_inclIBL.dat"
            # Hybrid IBL plus DBM
            elif (geoFlags.IBLLayout() == "3D"):
                IdMappingDat="PixelCabling/Pixels_Atlas_IdMapping_Run2.dat"

        PixelConfigCondAlg.CablingMapFileName=IdMappingDat

        condSeq += PixelConfigCondAlg(name="PixelConfigCondAlg")

    ############################################################################################
    # Set up Conditions DB
    ############################################################################################
    if useNewDeadmapFormat:
        if not conddb.folderRequested("/PIXEL/PixelModuleFeMask"):
            conddb.addFolder("PIXEL_OFL", "/PIXEL/PixelModuleFeMask", className="CondAttrListCollection")
        if not hasattr(condSeq, "PixelDeadMapCondAlg"):
            from PixelConditionsAlgorithms.PixelConditionsAlgorithmsConf import PixelDeadMapCondAlg
            condSeq += PixelDeadMapCondAlg(name="PixelDeadMapCondAlg")

    if not hasattr(condSeq, "PixelDCSCondStateAlg"):
        from PixelConditionsAlgorithms.PixelConditionsAlgorithmsConf import PixelDCSCondStateAlg
        condSeq += PixelDCSCondStateAlg(name="PixelDCSCondStateAlg",
                                        ReadKeyState = '')

    if not hasattr(condSeq, "PixelDCSCondStatusAlg"):
        from PixelConditionsAlgorithms.PixelConditionsAlgorithmsConf import PixelDCSCondStatusAlg
        condSeq += PixelDCSCondStatusAlg(name="PixelDCSCondStatusAlg",
                                         ReadKeyStatus = '')

    if not conddb.folderRequested("/PIXEL/DCS/HV"):
        conddb.addFolder("DCS_OFL", "/PIXEL/DCS/HV", className="CondAttrListCollection")

    if not hasattr(condSeq,"PixelDCSCondHVAlg"):
        from PixelConditionsAlgorithms.PixelConditionsAlgorithmsConf import PixelDCSCondHVAlg
        condSeq += PixelDCSCondHVAlg(name="PixelDCSCondHVAlg")

    if not conddb.folderRequested("/PIXEL/DCS/TEMPERATURE"):
        conddb.addFolder("DCS_OFL", "/PIXEL/DCS/TEMPERATURE", className="CondAttrListCollection")

    if not hasattr(condSeq,"PixelDCSCondTempAlg"):
        from PixelConditionsAlgorithms.PixelConditionsAlgorithmsConf import PixelDCSCondTempAlg
        condSeq += PixelDCSCondTempAlg(name="PixelDCSCondTempAlg")

    #####################
    # Calibration Setup #
    #####################
    if not useNewChargeFormat:
        if not conddb.folderRequested("/PIXEL/PixCalib"):
            conddb.addFolderSplitOnline("PIXEL", "/PIXEL/Onl/PixCalib", "/PIXEL/PixCalib", className="CondAttrListCollection")
        if not hasattr(condSeq, 'PixelChargeCalibCondAlg'):
            from PixelConditionsAlgorithms.PixelConditionsAlgorithmsConf import PixelChargeCalibCondAlg
            condSeq += PixelChargeCalibCondAlg(name="PixelChargeCalibCondAlg", ReadKey="/PIXEL/PixCalib")
    else:
        if not conddb.folderRequested("/PIXEL/ChargeCalibration"):
            conddb.addFolder("PIXEL_OFL", "/PIXEL/ChargeCalibration", className="CondAttrListCollection")
        if not hasattr(condSeq, 'PixelChargeLUTCalibCondAlg'):
            from PixelConditionsAlgorithms.PixelConditionsAlgorithmsConf import PixelChargeLUTCalibCondAlg
            condSeq += PixelChargeLUTCalibCondAlg(name="PixelChargeLUTCalibCondAlg", ReadKey="/PIXEL/ChargeCalibration")

    #####################
    # Cabling map Setup #
    #####################
    if geoFlags.isIBL() and not conddb.folderRequested("/PIXEL/HitDiscCnfg"):
        conddb.addFolderSplitMC("PIXEL","/PIXEL/HitDiscCnfg","/PIXEL/HitDiscCnfg", className="AthenaAttributeList")

    if geoFlags.isIBL() and not hasattr(condSeq, 'PixelHitDiscCnfgAlg'):
        from PixelConditionsAlgorithms.PixelConditionsAlgorithmsConf import PixelHitDiscCnfgAlg
        condSeq += PixelHitDiscCnfgAlg(name="PixelHitDiscCnfgAlg")

    if not conddb.folderRequested("/PIXEL/ReadoutSpeed"):
        conddb.addFolderSplitMC("PIXEL","/PIXEL/ReadoutSpeed","/PIXEL/ReadoutSpeed", className="AthenaAttributeList")

    if not hasattr(condSeq, 'PixelReadoutSpeedAlg'):
        from PixelConditionsAlgorithms.PixelConditionsAlgorithmsConf import PixelReadoutSpeedAlg
        condSeq += PixelReadoutSpeedAlg(name="PixelReadoutSpeedAlg")

    pixelReadKey = ''
    if (globalflags.DataSource=='data' and conddb.dbdata == 'CONDBR2'):  # for data overlay
        if not conddb.folderRequested("/PIXEL/CablingMap"):
            conddb.addFolderSplitOnline("PIXEL", "/PIXEL/Onl/CablingMap","/PIXEL/CablingMap", className="AthenaAttributeList")

    if not hasattr(condSeq, 'PixelCablingCondAlg'):
        from PixelConditionsAlgorithms.PixelConditionsAlgorithmsConf import PixelCablingCondAlg
        condSeq += PixelCablingCondAlg(name="PixelCablingCondAlg",
                                       ReadKey = pixelReadKey,
                                       MappingFile=IdMappingDat,
                                       RodIDForSingleLink40=0,
                                       RecordInInitialize=not globalflags.isOverlay())

    if not conddb.folderRequested("/PIXEL/PixReco"):
        conddb.addFolder("PIXEL_OFL", "/PIXEL/PixReco", className="DetCondCFloat")

    if not hasattr(condSeq, 'PixelOfflineCalibCondAlg'):
        from PixelConditionsAlgorithms.PixelConditionsAlgorithmsConf import PixelOfflineCalibCondAlg
        condSeq += PixelOfflineCalibCondAlg(name="PixelOfflineCalibCondAlg", ReadKey="/PIXEL/PixReco")
        PixelOfflineCalibCondAlg.InputSource = 2

    if not conddb.folderRequested("/Indet/PixelDist"):
        conddb.addFolder("INDET", "/Indet/PixelDist", className="DetCondCFloat")

    if not hasattr(condSeq, 'PixelDistortionAlg'):
        from PixelConditionsAlgorithms.PixelConditionsAlgorithmsConf import PixelDistortionAlg
        condSeq += PixelDistortionAlg(name="PixelDistortionAlg", ReadKey="/Indet/PixelDist")

    #######################
    # Setup Lorentz angle #
    #######################
    if not hasattr(condSeq, "PixelSiPropertiesCondAlg"):
        from SiPropertiesTool.SiPropertiesToolConf import PixelSiPropertiesCondAlg
        condSeq += PixelSiPropertiesCondAlg(name="PixelSiPropertiesCondAlg")

    if not hasattr(ToolSvc, "PixelSiPropertiesTool"):
        from SiPropertiesTool.SiPropertiesToolConf import SiPropertiesTool
        ToolSvc += SiPropertiesTool(name = "PixelSiPropertiesTool",
                                    DetectorName = "Pixel",
                                    ReadKey = "PixelSiliconPropertiesVector")

    if not hasattr(condSeq, "PixelSiLorentzAngleCondAlg"):
        from SiLorentzAngleTool.SiLorentzAngleToolConf import PixelSiLorentzAngleCondAlg
        condSeq += PixelSiLorentzAngleCondAlg(name = "PixelSiLorentzAngleCondAlg",
                                              SiPropertiesTool = ToolSvc.PixelSiPropertiesTool,
                                              UseMagFieldCache = True,
                                              UseMagFieldDcs = True)

    if not hasattr(ToolSvc, "PixelLorentzAngleTool"):
        from SiLorentzAngleTool.SiLorentzAngleToolConf import SiLorentzAngleTool
        ToolSvc += SiLorentzAngleTool(name="PixelLorentzAngleTool", 
                                      DetectorName="Pixel", 
                                      DetEleCollKey="PixelDetectorElementCollection",
                                      UseMagFieldCache=True,
                                      SiLorentzAngleCondData="PixelSiLorentzAngleCondData")

############################################################################################
# Set up Tool/Service
############################################################################################

    #####################
    # Setup Cabling Svc #
    #####################
    from PixelCabling.PixelCablingConf import PixelCablingSvc
    PixelCablingSvc = PixelCablingSvc()
    ServiceMgr += PixelCablingSvc
    print ( PixelCablingSvc)
    kwargs.setdefault("InputObjectName", "PixelHits")

    chargeTools = []
    feSimTools = []
    if geoFlags.isSLHC():
      chargeTools += ['SensorSimPlanarTool']
      feSimTools += ['BarrelRD53SimTool']
      feSimTools += ['EndcapRD53SimTool']
    else:
      chargeTools += ['SensorSimPlanarTool']
      chargeTools += ['SensorSim3DTool']
      feSimTools += ['BarrelFEI4SimTool']
      feSimTools += ['DBMFEI4SimTool']
      feSimTools += ['BarrelFEI3SimTool']
      feSimTools += ['EndcapFEI3SimTool']
    kwargs.setdefault("ChargeTools", chargeTools)
    kwargs.setdefault("FrontEndSimTools", feSimTools)
    kwargs.setdefault("EnergyDepositionTool", "EnergyDepositionTool")
    if digitizationFlags.doXingByXingPileUp(): # PileUpTool approach
        kwargs.setdefault("FirstXing", Pixel_FirstXing() )
        kwargs.setdefault("LastXing", Pixel_LastXing() )
    return CfgMgr.PixelDigitizationTool(name, **kwargs)

def PixelDigitizationTool(name="PixelDigitizationTool", **kwargs):
    kwargs.setdefault("HardScatterSplittingMode", 0)
    if digitizationFlags.PileUpPremixing and 'OverlayMT' in digitizationFlags.experimentalDigi():
        from OverlayCommonAlgs.OverlayFlags import overlayFlags
        kwargs.setdefault("RDOCollName", overlayFlags.bkgPrefix() + "PixelRDOs")
        kwargs.setdefault("SDOCollName", overlayFlags.bkgPrefix() + "PixelSDO_Map")
    else:
        kwargs.setdefault("RDOCollName", "PixelRDOs")
        kwargs.setdefault("SDOCollName", "PixelSDO_Map")
    return BasicPixelDigitizationTool(name, **kwargs)

def PixelGeantinoTruthDigitizationTool(name="PixelGeantinoTruthDigitizationTool", **kwargs):
    kwargs.setdefault("ParticleBarcodeVeto", 0)
    return PixelDigitizationTool(name, **kwargs)

def PixelDigitizationToolHS(name="PixelDigitizationToolHS", **kwargs):
    kwargs.setdefault("HardScatterSplittingMode", 1)
    return BasicPixelDigitizationTool(name, **kwargs)

def PixelDigitizationToolPU(name="PixelDigitizationToolPU", **kwargs):
    kwargs.setdefault("HardScatterSplittingMode", 2)
    kwargs.setdefault("RDOCollName", "Pixel_PU_RDOs")
    kwargs.setdefault("SDOCollName", "Pixel_PU_SDO_Map")
    return BasicPixelDigitizationTool(name, **kwargs)

def PixelDigitizationToolSplitNoMergePU(name="PixelDigitizationToolSplitNoMergePU", **kwargs):
    kwargs.setdefault("HardScatterSplittingMode", 0)
    kwargs.setdefault("InputObjectName", "PileupPixelHits")
    kwargs.setdefault("RDOCollName", "Pixel_PU_RDOs")
    kwargs.setdefault("SDOCollName", "Pixel_PU_SDO_Map")
    return BasicPixelDigitizationTool(name, **kwargs)

def PixelOverlayDigitizationTool(name="PixelOverlayDigitizationTool",**kwargs):
    from OverlayCommonAlgs.OverlayFlags import overlayFlags
    if overlayFlags.isOverlayMT():
        kwargs.setdefault("OnlyUseContainerName", False)
        kwargs.setdefault("RDOCollName", overlayFlags.sigPrefix() + "PixelRDOs")
        kwargs.setdefault("SDOCollName", overlayFlags.sigPrefix() + "PixelSDO_Map")
    else:
        kwargs.setdefault("RDOCollName", overlayFlags.evtStore() + "+PixelRDOs")
        kwargs.setdefault("SDOCollName", overlayFlags.evtStore() + "+PixelSDO_Map")
    kwargs.setdefault("HardScatterSplittingMode", 0)
    return BasicPixelDigitizationTool(name,**kwargs)

def getPixelRange(name="PixelRange" , **kwargs):
    kwargs.setdefault('FirstXing', Pixel_FirstXing() )
    kwargs.setdefault('LastXing',  Pixel_LastXing() )
    kwargs.setdefault('CacheRefreshFrequency', 1.0 ) #default 0 no dataproxy reset
    kwargs.setdefault('ItemList', ["SiHitCollection#PixelHits"] )
    return CfgMgr.PileUpXingFolder(name, **kwargs)

def PixelDigitizationHS(name="PixelDigitizationHS",**kwargs):
    kwargs.setdefault("DigitizationTool", "PixelDigitizationToolHS")
    return CfgMgr.PixelDigitization(name,**kwargs)

def PixelDigitizationPU(name="PixelDigitizationPU",**kwargs):
    kwargs.setdefault("DigitizationTool", "PixelDigitizationToolPU")
    return CfgMgr.PixelDigitization(name,**kwargs)

def PixelOverlayDigitization(name="PixelOverlayDigitization",**kwargs):
    kwargs.setdefault("DigitizationTool", "PixelOverlayDigitizationTool")
    # Multi-threading settinggs
    from AthenaCommon.ConcurrencyFlags import jobproperties as concurrencyProps
    is_hive = (concurrencyProps.ConcurrencyFlags.NumThreads() > 0)
    if is_hive:
        kwargs.setdefault('Cardinality', concurrencyProps.ConcurrencyFlags.NumThreads())
        # Set common overlay extra inputs
        kwargs.setdefault("ExtraInputs", [("McEventCollection", "TruthEvent")])

    return CfgMgr.PixelDigitization(name,**kwargs)
