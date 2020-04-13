"""Define functions to configure Pixel conditions algorithms

Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
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
    runNum = flags.Input.RunNumber[0]

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
    )

    # RUN-1 and beginning of RUN-2(2015)
    if (runNum<284500):
        CondArgs.update(
#            BarrelAnalogThreshold=[-1,-1,-1,-1],
#            EndcapAnalogThreshold=[-1,-1,-1],
#            DBMAnalogThreshold=[-1,-1,-1],
            BarrelToTThreshold=[-1,3,3,3],
            EndcapToTThreshold=[3,3,3],
            DBMToTThreshold=[-1,-1,-1],
            FEI3BarrelLatency=[-1,256,256,256],
            FEI3EndcapLatency=[256,256,256],
            FEI3BarrelHitDuplication=[True,True,True,True],
            FEI3EndcapHitDuplication=[True,True,True],
            FEI3BarrelSmallHitToT=[-1,7,7,7],
            FEI3EndcapSmallHitToT=[7,7,7],
            FEI3BarrelTimingSimTune=[2015,2015,2015,2015],
            FEI3EndcapTimingSimTune=[2015,2015,2015],
            FEI4BarrelHitDiscConfig=[0]
        )

    # mc16a and mc16c (2016)
    elif (runNum<300000):
        CondArgs.update(
#            BarrelAnalogThreshold=[-1,-1,-1,-1],
#            EndcapAnalogThreshold=[-1,-1,-1],
#            DBMAnalogThreshold=[-1,-1,-1],
            BarrelToTThreshold=[-1,5,5,5],
            EndcapToTThreshold=[5,5,5],
            DBMToTThreshold=[-1,-1,-1],
            FEI3BarrelLatency=[-1,151,256,256],
            FEI3EndcapLatency=[256,256,256],
            FEI3BarrelHitDuplication=[False,False,False,False],
            FEI3EndcapHitDuplication=[False,False,False],
            FEI3BarrelSmallHitToT=[-1, -1, -1, -1],
            FEI3EndcapSmallHitToT=[-1, -1, -1],
            FEI3BarrelTimingSimTune=[2015,2015,2015,2015],
            FEI3EndcapTimingSimTune=[2015,2015,2015],
            FEI4BarrelHitDiscConfig=[2]
        )

    # mc16d (2017)
    elif (runNum<310000):
        CondArgs.update(
#            BarrelAnalogThreshold=[-1,-1,-1,-1],
#            EndcapAnalogThreshold=[-1,-1,-1],
#            DBMAnalogThreshold=[-1,-1,-1],
            BarrelToTThreshold=[-1,5,5,5],
            EndcapToTThreshold=[5,5,5],
            DBMToTThreshold=[-1,-1,-1],
            FEI3BarrelLatency=[-1,151,256,256],
            FEI3EndcapLatency=[256,256,256],
            FEI3BarrelHitDuplication=[False,False,False,False],
            FEI3EndcapHitDuplication=[False,False,False],
            FEI3BarrelSmallHitToT=[-1, -1, -1, -1],
            FEI3EndcapSmallHitToT=[-1, -1, -1],
            FEI3BarrelTimingSimTune=[2018,2018,2018,2018],
            FEI3EndcapTimingSimTune=[2018,2018,2018],
            FEI4BarrelHitDiscConfig=[2]
        )

    # mc16e (2018)
    else:
        CondArgs.update(
#            BarrelAnalogThreshold=[-1,-1,-1,-1],
#            EndcapAnalogThreshold=[-1,-1,-1],
#            DBMAnalogThreshold=[-1,-1,-1],
            BarrelToTThreshold=[-1,3,5,5],
            EndcapToTThreshold=[5,5,5],
            DBMToTThreshold=[-1,-1,-1],
            FEI3BarrelLatency=[-1,151,256,256],
            FEI3EndcapLatency=[256,256,256],
            FEI3BarrelHitDuplication=[False,False,False,False],
            FEI3EndcapHitDuplication=[False,False,False],
            FEI3BarrelSmallHitToT=[-1, -1, -1, -1],
            FEI3EndcapSmallHitToT=[-1, -1, -1],
            FEI3BarrelTimingSimTune=[2018,2018,2018,2018],
            FEI3EndcapTimingSimTune=[2018,2018,2018],
            FEI4BarrelHitDiscConfig=[2]
        )

    CondArgs.update(
        BarrelCrossTalk=[0.06,0.06,0.06,0.06],
        BarrelThermalNoise=[160.0,160.0,160.0,160.0],
        BarrelNoiseOccupancy=[5e-8,5e-8,5e-8,5e-8],
        BarrelDisableProbability=[9e-3,9e-3,9e-3,9e-3],
        IBLNoiseShape=[0.0, 0.0330, 0.0, 0.2696, 0.1993, 0.1741, 0.1652, 0.1506, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0078],
        BLayerNoiseShape=[0.0, 0.0, 0.0, 0.0, 0.2204, 0.5311, 0.7493, 0.8954, 0.9980, 1.0],
        PixelNoiseShape=[0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.2418, 0.4397, 0.5858, 0.6949, 0.7737, 0.8414, 0.8959, 0.9414, 0.9828, 1.0],
        # Layer-2 noise [0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.2129, 0.4016, 0.5477, 0.6599, 0.7435, 0.8160, 0.8779, 0.9340, 0.9798, 1.0],
        EndcapCrossTalk=[0.06,0.06,0.06],
        EndcapThermalNoise=[160.0,160.0,160.0],
#        EndcapNoiseOccupancy=[5e-8,5e-8,5e-8],
#        EndcapDisableProbability=[9e-3,9e-3,9e-3],
#        EndcapNoiseShape=[[0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.1748, 0.3409, 0.4760, 0.5850, 0.6754, 0.7538, 0.8264, 0.8962, 0.9655, 1.0],
#                          [0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.1852, 0.3528, 0.4881, 0.5961, 0.6855, 0.7640, 0.8374, 0.9068, 0.9749, 1.0],
#                          [0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.1735, 0.3380, 0.4733, 0.5829, 0.6730, 0.7516, 0.8234, 0.8916, 0.9595, 1.0]],
        DBMCrossTalk=[0.06,0.06,0.06],
        DBMThermalNoise=[160.0,160.0,160.0]
#        DBMNoiseOccupancy=[5e-8,5e-8,5e-8],
#        DBMDisableProbability=[9e-3,9e-3,9e-3],
#        DBMNoiseShape=[[0.0,1.0],[0.0,1.0],[0.0,1.0]]
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
        kwargs.setdefault("ReadKey", "/PIXEL/CablingMap")
    else:
        kwargs.setdefault("ReadKey", "")

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
        runNum = flags.Input.RunNumber[0]
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
    kwargs.setdefault("WriteKey", "PixelCablingCondData")
    kwargs.setdefault("RecordInInitialize", not flags.Detector.OverlayPixel)
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

def PixelChargeLUTCalibCondAlgCfg(flags, name="PixelChargeLUTCalibCondAlg", **kwargs):
    """Return a ComponentAccumulator with configured PixelChargeLUTCalibCondAlg"""
    acc = ComponentAccumulator()
    acc.merge(addFolders(flags, "/PIXEL/PixCalib", "PIXEL_OFL", className="CondAttrListCollection"))
    kwargs.setdefault("PixelDetEleCollKey", "PixelDetectorElementCollection")
    kwargs.setdefault("PixelModuleData", "PixelModuleData")
    kwargs.setdefault("ReadKey", "/PIXEL/ChargeCalibration")
    kwargs.setdefault("WriteKey", "PixelChargeCalibCondData")
    acc.addCondAlgo(CompFactory.PixelChargeLUTCalibCondAlg(name, **kwargs))
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
        kwargs.setdefault("ReadKeyState", "/PIXEL/DCS/FSMSTATE")
    else:
        kwargs.setdefault("ReadKeyState", "")
    kwargs.setdefault("PixelModuleData", "PixelModuleData")
    kwargs.setdefault("WriteKeyState", "PixelDCSStateCondData")
    acc.addCondAlgo(CompFactory.PixelDCSCondStateAlg(name, **kwargs))
    return acc

def PixelDCSCondStatusAlgCfg(flags, name="PixelDCSCondStatusAlg", **kwargs):
    """Return a ComponentAccumulator with configured PixelDCSCondStatusAlg"""
    acc = ComponentAccumulator()
    if not flags.Input.isMC:
        acc.merge(addFolders(flags, "/PIXEL/DCS/FSMSTATUS", "DCS_OFL", className="CondAttrListCollection"))
        kwargs.setdefault("ReadKeyStatus", "/PIXEL/DCS/FSMSTATUS")
    else:
        kwargs.setdefault("ReadKeyStatus", "")
    kwargs.setdefault("PixelModuleData", "PixelModuleData")
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
    # not for Run-1 data/MC
    if flags.GeoModel.IBLLayout in ("noIBL", "UNDEFINED"):
        return acc
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
        kwargs.setdefault("ReadKey", "/TDAQ/Resources/ATLAS/PIXEL/Modules")
    else:
        kwargs.setdefault("ReadKey", "")
    kwargs.setdefault("PixelModuleData", "PixelModuleData")
    kwargs.setdefault("WriteKey", "PixelTDAQCondData")
    acc.addCondAlgo(CompFactory.PixelTDAQCondAlg(name, **kwargs))
    return acc


