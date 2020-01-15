"""Define functions to configure Pixel conditions algorithms

Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
"""
from AthenaConfiguration.ComponentAccumulator import ComponentAccumulator
from AthenaConfiguration.ComponentFactory import CompFactory
from IOVDbSvc.IOVDbSvcConfig import addFolders,addFoldersSplitOnline

def PixelConfigCondAlgCfg(flags, name="PixelConfigCondAlg", **kwargs):
    """Return a ComponentAccumulator with configured PixelConfigCondAlg"""
    acc = ComponentAccumulator()
    acc.merge(addFolders(flags, "/PIXEL/PixMapOverlay", "PIXEL_OFL", className="CondAttrListCollection"))
    acc.merge(addFoldersSplitOnline(flags, "PIXEL", "/PIXEL/Onl/PixMapOverlay",
                                        "/PIXEL/PixMapOverlay", "CondAttrListCollection"))
    # FIXME commented properties are not currently accepted by PixelConfigCondAlg
    CondArgs = {}
    # Switch parameters
    CondArgs.update(
        UseCalibConditions=True,
        UseDeadmapConditions=True,
        UseDCSStateConditions=False,
        UseDCSStatusConditions=False,
        UseDCSHVConditions=True,
        UseDCSTemperatureConditions=True,
        UseTDAQConditions=False,
    )
    # Digitization parameters
    CondArgs.update(
        BunchSpace=25.0,
#        BarrelAnalogThreshold=[-1,-1,-1,-1],
        BarrelToTThreshold=[-1, 5, 5, 5],
        BarrelCrossTalk=[0.06,0.06,0.06,0.06],
        BarrelThermalNoise=[160.0,160.0,160.0,160.0],
        BarrelNoiseOccupancy=[5e-8,5e-8,5e-8,5e-8],
        BarrelDisableProbability=[9e-3,9e-3,9e-3,9e-3],
        IBLNoiseShape=[0.0,1.0],
        BLayerNoiseShape=[0.00000, 0.00596, 0.03491, 0.07058, 0.11991, 0.17971, 0.24105, 0.29884, 0.35167, 0.39912, 0.44188, 0.48016, 0.51471, 0.54587, 0.57405, 0.59958, 0.62288, 0.64411, 0.66360, 0.68159, 0.69823, 0.71362, 0.72781, 0.74096, 0.75304, 0.76415, 0.77438, 0.78383, 0.79256, 0.80066, 0.80821, 0.81547, 0.82246, 0.82918, 0.83501, 0.84054, 0.84576, 0.85078, 0.85558, 0.86018, 0.86455, 0.86875, 0.87273, 0.87653, 0.88020, 0.88369, 0.88705, 0.89027, 0.89336, 0.89633, 0.89921, 0.90195, 0.90460, 0.90714, 0.90961, 0.91198, 0.91426, 0.91644, 0.91853, 0.92055, 0.92250, 0.92435, 0.92611, 0.92782, 0.92947, 0.93105, 0.93257, 0.93404, 0.93547, 0.93688, 0.93822, 0.93953, 0.94079, 0.94201, 0.94318, 0.94432, 0.94542, 0.94649, 0.94751, 0.94851, 0.94949, 0.95045, 0.95137, 0.95227, 0.95314, 0.95399, 0.95483, 0.95563, 0.95646, 0.95729, 0.95812, 0.95896, 0.95980, 0.96063, 0.96144, 0.96224, 0.96301, 0.96377, 0.96451, 0.96521, 0.96590, 0.96657, 0.96722, 0.96787, 0.96849, 0.96911, 0.96971, 0.97031, 0.97090, 0.97148, 0.97204, 0.97260, 0.97314, 0.97367, 0.97421, 0.97474, 0.97525, 0.97576, 0.97627, 0.97676, 0.97722, 0.97769, 0.97815, 0.97861, 0.97906, 0.97950, 0.97992, 0.98033, 0.98073, 0.98111, 0.98147, 0.98182, 0.98216, 0.98249, 0.98281, 0.98312, 0.98343, 0.98374, 0.98402, 0.98430, 0.98456, 0.98482, 0.98507, 0.98532, 0.98555, 0.98579, 0.98602, 0.98624, 0.98646, 0.98668, 0.98690, 0.98711, 0.98732, 0.98753, 0.98773, 0.98793, 0.98813, 0.98832, 0.98851, 0.98870, 0.98888, 0.98907, 0.98925, 0.98943, 0.98961, 0.98979, 0.98996, 0.99014, 0.99031, 0.99048, 0.99064, 0.99081, 0.99098, 0.99114, 0.99131, 0.99147, 0.99163, 0.99179, 0.99194, 0.99210, 0.99225, 0.99240, 0.99256, 0.99271, 0.99286, 0.99300, 0.99315, 0.99329, 0.99344, 0.99358, 0.99372, 0.99386, 0.99400, 0.99414, 0.99427, 0.99440, 0.99453, 0.99466, 0.99479, 0.99491, 0.99503, 0.99515, 0.99527, 0.99538, 0.99549, 0.99560, 0.99571, 0.99582, 0.99592, 0.99602, 0.99613, 0.99623, 0.99633, 0.99643, 0.99653, 0.99662, 0.99672, 0.99682, 0.99691, 0.99701, 0.99710, 0.99719, 0.99728, 0.99737, 0.99746, 0.99755, 0.99764, 0.99772, 0.99781, 0.99790, 0.99798, 0.99806, 0.99814, 0.99823, 0.99831, 0.99839, 0.99847, 0.99855, 0.99863, 0.99871, 0.99879, 0.99887, 0.99895, 0.99902, 0.99910, 0.99918, 0.99925, 0.99933, 0.99940, 0.99948, 0.99955, 0.99963, 0.99971, 0.99978, 0.99985, 0.99993, 1.00000],
        PixelNoiseShape=[0.00000, 0.00596, 0.03491, 0.07058, 0.11991, 0.17971, 0.24105, 0.29884, 0.35167, 0.39912, 0.44188, 0.48016, 0.51471, 0.54587, 0.57405, 0.59958, 0.62288, 0.64411, 0.66360, 0.68159, 0.69823, 0.71362, 0.72781, 0.74096, 0.75304, 0.76415, 0.77438, 0.78383, 0.79256, 0.80066, 0.80821, 0.81547, 0.82246, 0.82918, 0.83501, 0.84054, 0.84576, 0.85078, 0.85558, 0.86018, 0.86455, 0.86875, 0.87273, 0.87653, 0.88020, 0.88369, 0.88705, 0.89027, 0.89336, 0.89633, 0.89921, 0.90195, 0.90460, 0.90714, 0.90961, 0.91198, 0.91426, 0.91644, 0.91853, 0.92055, 0.92250, 0.92435, 0.92611, 0.92782, 0.92947, 0.93105, 0.93257, 0.93404, 0.93547, 0.93688, 0.93822, 0.93953, 0.94079, 0.94201, 0.94318, 0.94432, 0.94542, 0.94649, 0.94751, 0.94851, 0.94949, 0.95045, 0.95137, 0.95227, 0.95314, 0.95399, 0.95483, 0.95563, 0.95646, 0.95729, 0.95812, 0.95896, 0.95980, 0.96063, 0.96144, 0.96224, 0.96301, 0.96377, 0.96451, 0.96521, 0.96590, 0.96657, 0.96722, 0.96787, 0.96849, 0.96911, 0.96971, 0.97031, 0.97090, 0.97148, 0.97204, 0.97260, 0.97314, 0.97367, 0.97421, 0.97474, 0.97525, 0.97576, 0.97627, 0.97676, 0.97722, 0.97769, 0.97815, 0.97861, 0.97906, 0.97950, 0.97992, 0.98033, 0.98073, 0.98111, 0.98147, 0.98182, 0.98216, 0.98249, 0.98281, 0.98312, 0.98343, 0.98374, 0.98402, 0.98430, 0.98456, 0.98482, 0.98507, 0.98532, 0.98555, 0.98579, 0.98602, 0.98624, 0.98646, 0.98668, 0.98690, 0.98711, 0.98732, 0.98753, 0.98773, 0.98793, 0.98813, 0.98832, 0.98851, 0.98870, 0.98888, 0.98907, 0.98925, 0.98943, 0.98961, 0.98979, 0.98996, 0.99014, 0.99031, 0.99048, 0.99064, 0.99081, 0.99098, 0.99114, 0.99131, 0.99147, 0.99163, 0.99179, 0.99194, 0.99210, 0.99225, 0.99240, 0.99256, 0.99271, 0.99286, 0.99300, 0.99315, 0.99329, 0.99344, 0.99358, 0.99372, 0.99386, 0.99400, 0.99414, 0.99427, 0.99440, 0.99453, 0.99466, 0.99479, 0.99491, 0.99503, 0.99515, 0.99527, 0.99538, 0.99549, 0.99560, 0.99571, 0.99582, 0.99592, 0.99602, 0.99613, 0.99623, 0.99633, 0.99643, 0.99653, 0.99662, 0.99672, 0.99682, 0.99691, 0.99701, 0.99710, 0.99719, 0.99728, 0.99737, 0.99746, 0.99755, 0.99764, 0.99772, 0.99781, 0.99790, 0.99798, 0.99806, 0.99814, 0.99823, 0.99831, 0.99839, 0.99847, 0.99855, 0.99863, 0.99871, 0.99879, 0.99887, 0.99895, 0.99902, 0.99910, 0.99918, 0.99925, 0.99933, 0.99940, 0.99948, 0.99955, 0.99963, 0.99971, 0.99978, 0.99985, 0.99993, 1.00000],
        FEI3BarrelLatency=[ -1, 151, 256, 256],
        FEI3BarrelHitDuplication=[False, False, False, False],
        FEI3BarrelSmallHitToT=[-1, -1, -1, -1],
        FEI3BarrelTimingSimTune=[2015,2015,2015,2015],
        FEI4BarrelHitDiscConfig=[2],
#        EndcapAnalogThreshold=[-1,-1,-1],
        EndcapToTThreshold=[ 5, 5, 5],
        EndcapCrossTalk=[0.06,0.06,0.06],
        EndcapThermalNoise=[160.0,160.0,160.0],
#        EndcapNoiseOccupancy=[5e-8,5e-8,5e-8],
#        EndcapDisableProbability=[9e-3,9e-3,9e-3],
#        EndcapNoiseShape=[[0.0,1.0],[0.0,1.0],[0.0,1.0]],    # just excluded so far.
        FEI3EndcapLatency=[256, 256, 256],
        FEI3EndcapHitDuplication=[False, False, False],
        FEI3EndcapSmallHitToT=[-1, -1, -1],
        FEI3EndcapTimingSimTune=[2015,2015,2015],
#        DBMAnalogThreshold=[-1,-1,-1],
        DBMToTThreshold=[-1,-1,-1],
        DBMCrossTalk=[0.06,0.06,0.06],
        DBMThermalNoise=[160.0,160.0,160.0],
#        DBMNoiseOccupancy=[5e-8,5e-8,5e-8],
#        DBMDisableProbability=[9e-3,9e-3,9e-3],
#        DBMNoiseShape=[[0.0,1.0],[0.0,1.0],[0.0,1.0]],
        FEI4EndcapHitDiscConfig=[2,2,2]
    )
    if flags.Beam.Type == "cosmics":
        CondArgs.update(
            UseComTime=True,
            BarrelTimeJitter=[25.0,25.0,25.0,25.0],
            EndcapTimeJitter=[25.0,25.0,25.0],
            DBMTimeJitter=[25.0,25.0,25.0],
            BarrelNumberOfBCID=[8,8,8,8],
            EndcapNumberOfBCID=[8,8,8],
            DBMNumberOfBCID=[8,8,8],
            BarrelTimeOffset=[100.0,100.0,100.0,100.0],
            EndcapTimeOffset=[100.0,100.0,100.0],
            DBMTimeOffset=[100.0,100.0,100.0]
        )
    else:
        CondArgs.update(
            UseComTime=False,
            BarrelTimeJitter=[0.0,0.0,0.0,0.0],
            EndcapTimeJitter=[0.0,0.0,0.0],
            DBMTimeJitter=[0.0,0.0,0.0],
            BarrelNumberOfBCID=[1,1,1,1],
            EndcapNumberOfBCID=[1,1,1],
            DBMNumberOfBCID=[1,1,1],
            BarrelTimeOffset=[5.0,5.0,5.0,5.0],
            EndcapTimeOffset=[5.0,5.0,5.0],
            DBMTimeOffset=[5.0,5.0,5.0]
        )
    # Charge calibration parameters
    CondArgs.update(
        DefaultCalibrationParameterA=70.2,
        DefaultCalibrationParameterE=-3561.25,
        DefaultCalibrationParameterC=26000.0
    )
    # DCS parameters
    CondArgs.update(
        DefaultBiasVoltage=150.0,
        DefaultTemperature=-7.0
    )
    # Cabling parameters
    useCablingConditions = False
    IdMappingDat="PixelCabling/Pixels_Atlas_IdMapping_2016.dat"
    if flags.Input.isMC:
        # ITk:
        if flags.GeoModel.Run == "RUN4":
            IdMappingDat = "ITk_Atlas_IdMapping.dat"
            if flags.GeoModel.Type == "BrlIncl4.0_ref":
                IdMappingDat = "ITk_Atlas_IdMapping_InclBrl4.dat"
            elif flags.GeoModel.Type == "IBrlExt4.0ref":
                IdMappingDat = "ITk_Atlas_IdMapping_IExtBrl4.dat"
            elif flags.GeoModel.Type == "BrlExt4.0_ref":
                IdMappingDat = "ITk_Atlas_IdMapping_ExtBrl4.dat"
            elif flags.GeoModel.Type == "BrlExt3.2_ref":
                IdMappingDat = "ITk_Atlas_IdMapping_ExtBrl32.dat"
        elif flags.GeoModel.Run == "RUN2" or flags.GeoModel.Run == "RUN3":
            # Planar IBL
            if flags.GeoModel.IBLLayout == "planar":
                if flags.GeoModel.Run == "RUN2":
                    IdMappingDat="PixelCabling/Pixels_Atlas_IdMapping_inclIBL_DBM.dat"
                else:
                    IdMappingDat="PixelCabling/Pixels_Atlas_IdMapping_inclIBL.dat"
            # Hybrid IBL plus DBM
            elif flags.GeoModel.IBLLayout == "3D":
                IdMappingDat="PixelCabling/Pixels_Atlas_IdMapping_Run2.dat"
        else:
            IdMappingDat="PixelCabling/Pixels_Atlas_IdMapping.dat"
        
    elif not flags.Input.isMC:
        runNum = flags.Input.RunNumber
        if runNum < 222222:
            useCablingConditions = False
            IdMappingDat="PixelCabling/Pixels_Atlas_IdMapping_May08.dat"
        else:
            useCablingConditions = True
            # Even though we are reading from COOL, set the correct fallback map.
            if (runNum >= 344494):
                IdMappingDat="PixelCabling/Pixels_Atlas_IdMapping_344494.dat"
            elif (runNum >= 314940 and runNum < 344494):
                IdMappingDat="PixelCabling/Pixels_Atlas_IdMapping_314940.dat"
            elif (runNum >= 289350 and runNum < 314940): # 2016
                IdMappingDat="PixelCabling/Pixels_Atlas_IdMapping_2016.dat"
            elif (runNum >= 222222 and runNum < 289350): # 2015
                IdMappingDat="PixelCabling/Pixels_Atlas_IdMapping_Run2.dat"
            else:
                IdMappingDat="PixelCabling/Pixels_Atlas_IdMapping_May08.dat"

    CondArgs.update(
        UseCablingConditions=useCablingConditions,
        CablingMapToFile=False,
        CablingMapFileName=IdMappingDat
    )
    # Distortion parameters
    CondArgs.update(
        DistortionInputSource=4,
        DistortionVersion=1,
        DistortionR1=0.1/1000,
        DistortionR2=0.1/1000,
        DistortionTwist=0.0005,
        DistortionMean_R=0.12/1000,
        DistortionRMS_R=0.08/1000,
        DistortionMean_twist=-0.0005,
        DistortionRMS_twist=0.0008,
        DistortionWriteToFile=False,
        DistortionFileName="PixelSurveyDistortions.txt"
    )
    # set default kwargs
    CondArgs.update(kwargs)
    acc.addCondAlgo(CompFactory.PixelConfigCondAlg(name, **CondArgs))
    return acc


def PixelAlignCondAlgCfg(flags, name="PixelAlignCondAlg", **kwargs):
    """Return a ComponentAccumulator with configured PixelAlignCondAlg"""
    acc = ComponentAccumulator()

    if flags.GeoModel.Align.Dynamic:
        acc.merge(addFoldersSplitOnline(flags,"INDET","/Indet/Onl/AlignL1/ID","/Indet/AlignL1/ID",className="CondAttrListCollection"))
        acc.merge(addFoldersSplitOnline(flags,"INDET","/Indet/Onl/AlignL2/PIX","/Indet/AlignL2/PIX",className="CondAttrListCollection"))
        acc.merge(addFoldersSplitOnline(flags,"INDET","/Indet/Onl/AlignL3","/Indet/AlignL3",className="AlignableTransformContainer"))
    else:
        if (not flags.Detector.SimulatePixel) or flags.Detector.OverlayPixel:
            acc.merge(addFoldersSplitOnline(flags,"INDET","/Indet/Onl/Align","/Indet/Align",className="AlignableTransformContainer"))
        else:
            acc.merge(addFoldersSplitOnline(flags,"INDET","/Indet/Onl/Align","/Indet/Align"))

    acc.merge(addFoldersSplitOnline(flags,"INDET","/Indet/Onl/IBLDist","/Indet/IBLDist",className="CondAttrListCollection"))

    kwargs.setdefault("UseDynamicAlignFolders", False)
    kwargs.setdefault("ReadKeyStatic", "/Indet/Align")
    kwargs.setdefault("ReadKeyDynamicL1", "/Indet/AlignL1/ID")
    kwargs.setdefault("ReadKeyDynamicL2", "/Indet/AlignL2/PIX")
    kwargs.setdefault("ReadKeyDynamicL3", "/Indet/AlignL3")
    kwargs.setdefault("ReadKeyIBLDist", "/Indet/IBLDist")
    kwargs.setdefault("WriteKey", "PixelAlignmentStore")

    acc.addCondAlgo(CompFactory.PixelAlignCondAlg(name, **kwargs))
    return acc

def PixelCablingCondAlgCfg(flags, name="PixelCablingCondAlg", **kwargs):
    """Return a ComponentAccumulator with configured PixelCablingCondAlg"""
    acc = ComponentAccumulator()
    if not flags.Input.isMC:
        acc.merge(addFoldersSplitOnline(flags, "PIXEL", "/PIXEL/Onl/CablingMap","/PIXEL/CablingMap", className="AthenaAttributeList"))

    # Cabling parameters
    IdMappingDat="PixelCabling/Pixels_Atlas_IdMapping_2016.dat"
    rodIDForSingleLink40=0
    if flags.Input.isMC:
        # ITk:
        if flags.GeoModel.Run == "RUN4":
            IdMappingDat = "ITk_Atlas_IdMapping.dat"
            if flags.GeoModel.Type == "BrlIncl4.0_ref":
                IdMappingDat = "ITk_Atlas_IdMapping_InclBrl4.dat"
            elif flags.GeoModel.Type == "IBrlExt4.0ref":
                IdMappingDat = "ITk_Atlas_IdMapping_IExtBrl4.dat"
            elif flags.GeoModel.Type == "BrlExt4.0_ref":
                IdMappingDat = "ITk_Atlas_IdMapping_ExtBrl4.dat"
            elif flags.GeoModel.Type == "BrlExt3.2_ref":
                IdMappingDat = "ITk_Atlas_IdMapping_ExtBrl32.dat"
        elif flags.GeoModel.Run == "RUN2" or flags.GeoModel.Run == "RUN3":
            # Planar IBL
            if flags.GeoModel.IBLLayout == "planar":
                if flags.GeoModel.Run == "RUN2":
                    IdMappingDat="PixelCabling/Pixels_Atlas_IdMapping_inclIBL_DBM.dat"
                else:
                    IdMappingDat="PixelCabling/Pixels_Atlas_IdMapping_inclIBL.dat"
            # Hybrid IBL plus DBM
            elif flags.GeoModel.IBLLayout == "3D":
                IdMappingDat="PixelCabling/Pixels_Atlas_IdMapping_Run2.dat"
        else:
            IdMappingDat="PixelCabling/Pixels_Atlas_IdMapping.dat"
        
    elif not flags.Input.isMC:
        runNum = flags.Input.RunNumber
        if runNum < 222222:
            IdMappingDat="PixelCabling/Pixels_Atlas_IdMapping_May08.dat"
            rodIDForSingleLink40=1300000
        else:
            rodIDForSingleLink40=1300000
            # Even though we are reading from COOL, set the correct fallback map.
            if (runNum >= 344494):
                IdMappingDat="PixelCabling/Pixels_Atlas_IdMapping_344494.dat"
            elif (runNum >= 314940 and runNum < 344494):
                IdMappingDat="PixelCabling/Pixels_Atlas_IdMapping_314940.dat"
            elif (runNum >= 289350 and runNum < 314940): # 2016
                IdMappingDat="PixelCabling/Pixels_Atlas_IdMapping_2016.dat"
            elif (runNum >= 222222 and runNum < 289350): # 2015
                IdMappingDat="PixelCabling/Pixels_Atlas_IdMapping_Run2.dat"
            else:
                IdMappingDat="PixelCabling/Pixels_Atlas_IdMapping_May08.dat"

    kwargs.setdefault("RodIDForSingleLink40", rodIDForSingleLink40)
    kwargs.setdefault("MappingFile", IdMappingDat)
    kwargs.setdefault("PixelModuleData", "PixelModuleData")
    kwargs.setdefault("PixelReadoutSpeedData", "PixelReadoutSpeedData")
    kwargs.setdefault("ReadKey", "/PIXEL/CablingMap")
    kwargs.setdefault("WriteKey", "PixelCablingCondData")
    acc.addCondAlgo(CompFactory.PixelCablingCondAlg(name, **kwargs))
    return acc

def PixelChargeCalibCondAlgCfg(flags, name="PixelChargeCalibCondAlg", **kwargs):
    """Return a ComponentAccumulator with configured PixelChargeCalibCondAlg"""
    acc = ComponentAccumulator()
    acc.merge(addFolders(flags, "/PIXEL/PixCalib", "PIXEL_OFL", className="CondAttrListCollection"))
    kwargs.setdefault("PixelDetEleCollKey", "PixelDetectorElementCollection")
    kwargs.setdefault("PixelModuleData", "PixelModuleData")
    kwargs.setdefault("ReadKey", "/PIXEL/PixCalib")
    kwargs.setdefault("WriteKey", "PixelChargeCalibCondData")
    acc.addCondAlgo(CompFactory.PixelChargeCalibCondAlg(name, **kwargs))
    return acc

def PixelDCSCondHVAlgCfg(flags, name="PixelDCSCondHVAlg", **kwargs):
    """Return a ComponentAccumulator with configured PixelDCSCondHVAlg"""
    acc = ComponentAccumulator()
    if flags.Common.isOnline:
        acc.merge(addFolders(flags, "/PIXEL/HLT/DCS/HV", "PIXEL_ONL", className="CondAttrListCollection"))
    else:
        acc.merge(addFolders(flags, "/PIXEL/DCS/HV", "DCS_OFL", className="CondAttrListCollection"))
    kwargs.setdefault("PixelModuleData", "PixelModuleData")
    kwargs.setdefault("ReadKey", "/PIXEL/DCS/HV")
    kwargs.setdefault("WriteKey", "PixelDCSHVCondData")
    acc.addCondAlgo(CompFactory.PixelDCSCondHVAlg(name, **kwargs))
    return acc

def PixelDCSCondStateAlgCfg(flags, name="PixelDCSCondStateAlg", **kwargs):
    """Return a ComponentAccumulator with configured PixelDCSCondStateAlg"""
    acc = ComponentAccumulator()
    if not flags.Input.isMC:
        acc.merge(addFolders(flags, "/PIXEL/DCS/FSMSTATE", "DCS_OFL", className="CondAttrListCollection"))
    kwargs.setdefault("PixelModuleData", "PixelModuleData")
    kwargs.setdefault("ReadKeyState", "/PIXEL/DCS/FSMSTATE")
    kwargs.setdefault("WriteKeyState", "PixelDCSStateCondData")
    acc.addCondAlgo(CompFactory.PixelDCSCondStateAlg(name, **kwargs))
    return acc

def PixelDCSCondStatusAlgCfg(flags, name="PixelDCSCondStatusAlg", **kwargs):
    """Return a ComponentAccumulator with configured PixelDCSCondStatusAlg"""
    acc = ComponentAccumulator()
    if not flags.Input.isMC:
        acc.merge(addFolders(flags, "/PIXEL/DCS/FSMSTATUS", "DCS_OFL", className="CondAttrListCollection"))
    kwargs.setdefault("PixelModuleData", "PixelModuleData")
    kwargs.setdefault("ReadKeyStatus", "/PIXEL/DCS/FSMSTATUS")
    kwargs.setdefault("WriteKeyStatus", "PixelDCSStatusCondData")
    acc.addCondAlgo(CompFactory.PixelDCSCondStatusAlg(name, **kwargs))
    return acc

def PixelDCSCondTempAlgCfg(flags, name="PixelDCSCondTempAlg", **kwargs):
    """Return a ComponentAccumulator with configured PixelDCSCondTempAlg"""
    acc = ComponentAccumulator()
    if flags.Common.isOnline:
        acc.merge(addFolders(flags, "/PIXEL/HLT/DCS/TEMPERATURE", "PIXEL_ONL", className="CondAttrListCollection"))
    else:
        acc.merge(addFolders(flags, "/PIXEL/DCS/TEMPERATURE", "DCS_OFL", className="CondAttrListCollection"))
    kwargs.setdefault("PixelModuleData", "PixelModuleData")
    kwargs.setdefault("ReadKey", "/PIXEL/DCS/TEMPERATURE")
    kwargs.setdefault("WriteKey", "PixelDCSTempCondData")
    acc.addCondAlgo(CompFactory.PixelDCSCondTempAlg(name, **kwargs))
    return acc

# NEW FOR RUN3 def PixelDeadMapCondAlgCfg(flags, name="PixelDeadMapCondAlg", **kwargs):
# NEW FOR RUN3     """Return a ComponentAccumulator with configured PixelDeadMapCondAlg"""
# NEW FOR RUN3     acc = ComponentAccumulator()
# NEW FOR RUN3     acc.merge(addFolders(flags, "/PIXEL/PixelModuleFeMask", "PIXEL_OFL", className="CondAttrListCollection"))
# NEW FOR RUN3     kwargs.setdefault("PixelModuleData", "PixelModuleData")
# NEW FOR RUN3     kwargs.setdefault("ReadKey", "/PIXEL/PixelModuleFeMask")
# NEW FOR RUN3     kwargs.setdefault("WriteKey", "PixelDeadMapCondData")
# NEW FOR RUN3     acc.addCondAlgo(PixelDeadMapCondAlg(name, **kwargs))
# NEW FOR RUN3     return acc

def PixelDetectorElementCondAlgCfg(flags, name="PixelDetectorElementCondAlg", **kwargs):
    """Return a ComponentAccumulator with configured PixelDetectorElementCondAlg"""
    acc = ComponentAccumulator()
    kwargs.setdefault("PixelAlignmentStore", "PixelAlignmentStore")
    kwargs.setdefault("WriteKey", "PixelDetectorElementCollection")
    acc.addCondAlgo(CompFactory.PixelDetectorElementCondAlg(name, **kwargs))
    return acc

def PixelDistortionAlgCfg(flags, name="PixelDistortionAlg", **kwargs):
    """Return a ComponentAccumulator with configured PixelDistortionAlg"""
    acc = ComponentAccumulator()
    acc.merge(addFoldersSplitOnline(flags,"INDET","/Indet/Onl/PixelDist","/Indet/PixelDist",className="DetCondCFloat"))
    kwargs.setdefault("PixelModuleData", "PixelModuleData")
    kwargs.setdefault("ReadKey", "/Indet/PixelDist")
    kwargs.setdefault("WriteKey", "PixelDistortionData")
    acc.addCondAlgo(CompFactory.PixelDistortionAlg(name, **kwargs))
    return acc

def PixelHitDiscCnfgAlgCfg(flags, name="PixelHitDiscCnfgAlg", **kwargs):
    """Return a ComponentAccumulator with configured PixelHitDiscCnfgAlg"""
    acc = ComponentAccumulator()
    if (flags.IOVDb.DatabaseInstance=="CONDBR2"):
        acc.merge(addFolders(flags, "/PIXEL/HitDiscCnfg", "PIXEL", className="AthenaAttributeList"))
    elif (flags.Input.isMC and flags.GeoModel.Run=="RUN2") or (flags.Input.isMC and flags.GeoModel.Run=="RUN3"):
        acc.merge(addFoldersSplitOnline(flags,"PIXEL","/PIXEL/HitDiscCnfg","/PIXEL/HitDiscCnfg", className="AthenaAttributeList"))
    kwargs.setdefault("PixelModuleData", "PixelModuleData")
    kwargs.setdefault("ReadKey", "/PIXEL/HitDiscCnfg")
    kwargs.setdefault("WriteKey", "PixelHitDiscCnfgData")
    acc.addCondAlgo(CompFactory.PixelHitDiscCnfgAlg(name, **kwargs))
    return acc

def PixelOfflineCalibCondAlgCfg(flags, name="PixelOfflineCalibCondAlg", **kwargs):
    """Return a ComponentAccumulator with configured PixelOfflineCalibCondAlg"""
    acc = ComponentAccumulator()
    acc.merge(addFolders(flags, "/PIXEL/PixReco", "PIXEL_OFL", className="DetCondCFloat"))
    kwargs.setdefault("InputSource",2)
    kwargs.setdefault("PixelClusterErrorDataFile", "PixelClusterErrorData.txt")
    kwargs.setdefault("PixelClusterOnTrackErrorDataFile", "PixelClusterOnTrackErrorData.txt")
    kwargs.setdefault("PixelChargeInterpolationDataFile", "PixelChargeInterpolationData.txt")
    kwargs.setdefault("DumpConstants", 0)
    kwargs.setdefault("ReadKey", "/PIXEL/PixReco")
    kwargs.setdefault("WriteKey", "PixelOfflineCalibData")
    acc.addCondAlgo(CompFactory.PixelOfflineCalibCondAlg(name, **kwargs))
    return acc

def PixelReadoutSpeedAlgCfg(flags, name="PixelReadoutSpeedAlg", **kwargs):
    """Return a ComponentAccumulator with configured PixelReadoutSpeedAlg"""
    acc = ComponentAccumulator()
    if not flags.Input.isMC:
        acc.merge(addFolders(flags, "/PIXEL/ReadoutSpeed", "PIXEL", className="AthenaAttributeList"))
    else:
        acc.merge(addFoldersSplitOnline(flags, "PIXEL","/PIXEL/ReadoutSpeed","/PIXEL/ReadoutSpeed", className="AthenaAttributeList"))
    kwargs.setdefault("ReadKey", "/PIXEL/ReadoutSpeed")
    kwargs.setdefault("WriteKey", "PixelReadoutSpeedData")
    acc.addCondAlgo(CompFactory.PixelReadoutSpeedAlg(name, **kwargs))
    return acc

def PixelTDAQCondAlgCfg(flags, name="PixelTDAQCondAlg", **kwargs):
    """Return a ComponentAccumulator with configured PixelTDAQCondAlg"""
    acc = ComponentAccumulator()
    if flags.Common.isOnline:
        acc.merge(addFolders(flags, "/TDAQ/Resources/ATLAS/PIXEL/Modules", "TDAQ_ONL", className="CondAttrListCollection"))
    kwargs.setdefault("PixelModuleData", "PixelModuleData")
    kwargs.setdefault("ReadKey", "/TDAQ/Resources/ATLAS/PIXEL/Modules")
    kwargs.setdefault("WriteKey", "PixelTDAQCondData")
    acc.addCondAlgo(CompFactory.PixelTDAQCondAlg(name, **kwargs))
    return acc


