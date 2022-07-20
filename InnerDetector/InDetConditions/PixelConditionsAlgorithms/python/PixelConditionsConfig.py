"""Define functions to configure Pixel conditions algorithms

Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
"""
from AthenaConfiguration.ComponentAccumulator import ComponentAccumulator
from AthenaConfiguration.ComponentFactory import CompFactory
from AthenaConfiguration.Enums import BeamType, LHCPeriod
from IOVDbSvc.IOVDbSvcConfig import addFolders,addFoldersSplitOnline

def PixelConfigCondAlgCfg(flags, name="PixelConfigCondAlg", **kwargs):
    """Return a ComponentAccumulator with configured PixelConfigCondAlg"""
    acc = ComponentAccumulator()
    runNum = flags.Input.RunNumber[0]

    # FIXME commented properties are not currently accepted by PixelConfigCondAlg
    CondArgs = {}
    if flags.Beam.Type is BeamType.Cosmics:
        CondArgs.update(
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

    if not flags.Input.isMC and not flags.Overlay.DataOverlay:
        # for data, make sure no Lorentz angle correction
        CondArgs.update(
            BarrelLorentzAngleCorr2016 = [  1.0,  1.0,  1.0,  1.0],
            EndcapLorentzAngleCorr2016 = [  1.0,  1.0,  1.0],
            BarrelLorentzAngleCorr2017 = [  1.0,  1.0,  1.0,  1.0],
            EndcapLorentzAngleCorr2017 = [  1.0,  1.0,  1.0],
            BarrelLorentzAngleCorr2018 = [  1.0,  1.0,  1.0,  1.0],
            EndcapLorentzAngleCorr2018 = [  1.0,  1.0,  1.0],
            BarrelLorentzAngleCorrRUN1 = [  1.0,  1.0,  1.0],
            EndcapLorentzAngleCorrRUN1 = [  1.0,  1.0,  1.0],
        )

    # Cabling parameters
    IdMappingDat="PixelCabling/Pixels_Atlas_IdMapping_2016.dat"
    if flags.Input.isMC:
        # ITk:
        if flags.GeoModel.Run >= LHCPeriod.Run4:  # RUN4 and beyond
            IdMappingDat = "ITk_Atlas_IdMapping.dat"
        elif flags.GeoModel.Run in [LHCPeriod.Run2, LHCPeriod.Run3]:
            # Planar IBL
            if flags.GeoModel.IBLLayout == "planar":
                if flags.GeoModel.Run is LHCPeriod.Run2:
                    IdMappingDat="PixelCabling/Pixels_Atlas_IdMapping_inclIBL_DBM.dat"
                else:
                    IdMappingDat="PixelCabling/Pixels_Atlas_IdMapping_inclIBL.dat"
            # Hybrid IBL plus DBM
            elif flags.GeoModel.IBLLayout == "3D":
                IdMappingDat="PixelCabling/Pixels_Atlas_IdMapping_Run2.dat"
        else:
            IdMappingDat="PixelCabling/Pixels_Atlas_IdMapping.dat"
    else:
        if runNum < 222222:
            IdMappingDat="PixelCabling/Pixels_Atlas_IdMapping_May08.dat"
        else:
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
                IdMappingDat="PixelCabling/Pixels_Atlas_IdMapping_344494.dat"

    CondArgs.update(
        CablingMapFileName=IdMappingDat
    )

    # set default kwargs
    CondArgs.update(kwargs)
    acc.addCondAlgo(CompFactory.PixelConfigCondAlg(name, **CondArgs))
    return acc


def PixelAlignCondAlgCfg(flags, name="PixelAlignCondAlg", **kwargs):
    """Return a ComponentAccumulator with configured PixelAlignCondAlg"""
    from PixelGeoModel.PixelGeoModelConfig import PixelGeoModelCfg
    acc = PixelGeoModelCfg(flags)

    if flags.GeoModel.Align.Dynamic:
        acc.merge(addFoldersSplitOnline(flags,"INDET","/Indet/Onl/AlignL1/ID","/Indet/AlignL1/ID",className="CondAttrListCollection"))
        acc.merge(addFoldersSplitOnline(flags,"INDET","/Indet/Onl/AlignL2/PIX","/Indet/AlignL2/PIX",className="CondAttrListCollection"))
        acc.merge(addFoldersSplitOnline(flags,"INDET","/Indet/Onl/AlignL3","/Indet/AlignL3",className="AlignableTransformContainer"))
    else:
        acc.merge(addFoldersSplitOnline(flags,"INDET","/Indet/Onl/Align","/Indet/Align",className="AlignableTransformContainer"))

    acc.merge(addFoldersSplitOnline(flags,"INDET","/Indet/Onl/IBLDist","/Indet/IBLDist",className="CondAttrListCollection"))

    kwargs.setdefault("UseDynamicAlignFolders", flags.GeoModel.Align.Dynamic)
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
    acc.merge(PixelConfigCondAlgCfg(flags))
    acc.merge(PixelReadoutSpeedAlgCfg(flags))

    if not flags.Input.isMC and not flags.Overlay.DataOverlay:
        acc.merge(addFoldersSplitOnline(flags, "PIXEL", "/PIXEL/Onl/CablingMap","/PIXEL/CablingMap", className="AthenaAttributeList"))
        kwargs.setdefault("ReadKey", "/PIXEL/CablingMap")
        if flags.Input.RunNumber[0]<222222:
            kwargs.setdefault("ReadKey", "")
    else:
        kwargs.setdefault("ReadKey", "")
    kwargs.setdefault("PixelModuleData", "PixelModuleData")
    kwargs.setdefault("PixelReadoutSpeedData", "PixelReadoutSpeedData")
    kwargs.setdefault("WriteKey", "PixelCablingCondData")
    acc.addCondAlgo(CompFactory.PixelCablingCondAlg(name, **kwargs))
    return acc

def PixelChargeCalibCondAlgCfg(flags, name="PixelChargeCalibCondAlg", **kwargs):
    """Return a ComponentAccumulator with configured PixelChargeCalibCondAlg"""
    acc = ComponentAccumulator()
    acc.merge(PixelConfigCondAlgCfg(flags))
    acc.merge(addFoldersSplitOnline(flags, "PIXEL", "/PIXEL/Onl/PixCalib", "/PIXEL/PixCalib", className="CondAttrListCollection"))
    from PixelGeoModel.PixelGeoModelConfig import PixelReadoutGeometryCfg
    acc.merge(PixelReadoutGeometryCfg(flags))
    kwargs.setdefault("PixelDetEleCollKey", "PixelDetectorElementCollection")
    kwargs.setdefault("PixelModuleData", "PixelModuleData")
    if flags.GeoModel.Run is LHCPeriod.Run2:
        kwargs.setdefault("ReadKey", "/PIXEL/PixCalib")
    else:
        kwargs.setdefault("ReadKey", "")  # FIXME: temporarily disabling DB access for Run 1 due to crashes
    kwargs.setdefault("WriteKey", "PixelChargeCalibCondData")
    acc.addCondAlgo(CompFactory.PixelChargeCalibCondAlg(name, **kwargs))
    return acc

def PixelChargeLUTCalibCondAlgCfg(flags, name="PixelChargeLUTCalibCondAlg", **kwargs):
    """Return a ComponentAccumulator with configured PixelChargeLUTCalibCondAlg"""
    acc = ComponentAccumulator()
    acc.merge(PixelConfigCondAlgCfg(flags))
    acc.merge(addFoldersSplitOnline(flags, "PIXEL", "/PIXEL/Onl/ChargeCalibration", "/PIXEL/ChargeCalibration", className="CondAttrListCollection"))
    from PixelGeoModel.PixelGeoModelConfig import PixelReadoutGeometryCfg
    acc.merge(PixelReadoutGeometryCfg(flags))
    kwargs.setdefault("PixelDetEleCollKey", "PixelDetectorElementCollection")
    kwargs.setdefault("PixelModuleData", "PixelModuleData")
    kwargs.setdefault("ReadKey", "/PIXEL/ChargeCalibration")
    kwargs.setdefault("WriteKey", "PixelChargeCalibCondData")
    acc.addCondAlgo(CompFactory.PixelChargeLUTCalibCondAlg(name, **kwargs))
    return acc

def PixelDCSCondHVAlgCfg(flags, name="PixelDCSCondHVAlg", **kwargs):
    """Return a ComponentAccumulator with configured PixelDCSCondHVAlg"""
    acc = ComponentAccumulator()
    acc.merge(PixelConfigCondAlgCfg(flags))
    if flags.Common.isOnline:
        kwargs.update( ReadKey="/PIXEL/HLT/DCS/HV")
        acc.merge(addFolders(flags, kwargs["ReadKey"], "PIXEL_ONL", className="CondAttrListCollection"))
    else:
        kwargs.update( ReadKey="/PIXEL/DCS/HV")
        acc.merge(addFolders(flags, kwargs["ReadKey"], "DCS_OFL", className="CondAttrListCollection"))

    kwargs.setdefault("WriteKey", "PixelDCSHVCondData")
    acc.addCondAlgo(CompFactory.PixelDCSCondHVAlg(name, **kwargs))
    return acc

def PixelDCSCondStateAlgCfg(flags, name="PixelDCSCondStateAlg", **kwargs):
    """Return a ComponentAccumulator with configured PixelDCSCondStateAlg"""
    acc = ComponentAccumulator()
    if not flags.Input.isMC and not flags.Overlay.DataOverlay and flags.InDet.usePixelDCS:
        acc.merge(addFolders(flags, "/PIXEL/DCS/FSMSTATE", "DCS_OFL", className="CondAttrListCollection"))
        kwargs.setdefault("ReadKeyState", "/PIXEL/DCS/FSMSTATE")
    else:
        kwargs.setdefault("ReadKeyState", "")
    kwargs.setdefault("WriteKeyState", "PixelDCSStateCondData")
    acc.addCondAlgo(CompFactory.PixelDCSCondStateAlg(name, **kwargs))
    return acc

def PixelDCSCondStatusAlgCfg(flags, name="PixelDCSCondStatusAlg", **kwargs):
    """Return a ComponentAccumulator with configured PixelDCSCondStatusAlg"""
    acc = ComponentAccumulator()
    if not flags.Input.isMC and not flags.Overlay.DataOverlay and flags.InDet.usePixelDCS:
        acc.merge(addFolders(flags, "/PIXEL/DCS/FSMSTATUS", "DCS_OFL", className="CondAttrListCollection"))
        kwargs.setdefault("ReadKeyStatus", "/PIXEL/DCS/FSMSTATUS")
    else:
        kwargs.setdefault("ReadKeyStatus", "")
    kwargs.setdefault("WriteKeyStatus", "PixelDCSStatusCondData")
    acc.addCondAlgo(CompFactory.PixelDCSCondStatusAlg(name, **kwargs))
    return acc

def PixelDCSCondTempAlgCfg(flags, name="PixelDCSCondTempAlg", **kwargs):
    """Return a ComponentAccumulator with configured PixelDCSCondTempAlg"""
    acc = ComponentAccumulator()
    acc.merge(PixelConfigCondAlgCfg(flags))
    if flags.Common.isOnline:
        kwargs.setdefault("ReadKey", "/PIXEL/HLT/DCS/TEMPERATURE")
        acc.merge(addFolders(flags, kwargs["ReadKey"], "PIXEL_ONL", className="CondAttrListCollection"))
    else:
        kwargs.setdefault("ReadKey", "/PIXEL/DCS/TEMPERATURE")
        acc.merge(addFolders(flags, kwargs["ReadKey"], "DCS_OFL", className="CondAttrListCollection"))
    kwargs.setdefault("PixelModuleData", "PixelModuleData")
    kwargs.setdefault("WriteKey", "PixelDCSTempCondData")
    acc.addCondAlgo(CompFactory.PixelDCSCondTempAlg(name, **kwargs))
    return acc

def PixelDeadMapCondAlgCfg(flags, name="PixelDeadMapCondAlg", **kwargs):
    """Return a ComponentAccumulator with configured PixelDeadMapCondAlg"""
    acc = ComponentAccumulator()
    acc.merge(PixelConfigCondAlgCfg(flags))
    if flags.GeoModel.Run is LHCPeriod.Run1:
        kwargs.setdefault("ReadKey", "")
    else:
        kwargs.setdefault("ReadKey", "/PIXEL/PixelModuleFeMask")
        acc.merge(addFoldersSplitOnline(flags, "PIXEL", "/PIXEL/Onl/PixelModuleFeMask", "/PIXEL/PixelModuleFeMask", className="CondAttrListCollection"))
    kwargs.setdefault("WriteKey", "PixelDeadMapCondData")
    acc.addCondAlgo(CompFactory.PixelDeadMapCondAlg(name, **kwargs))
    return acc

def PixelDetectorElementCondAlgCfg(flags, name="PixelDetectorElementCondAlg", **kwargs):
    """Return a ComponentAccumulator with configured PixelDetectorElementCondAlg"""
    acc = ComponentAccumulator()
    acc.merge(PixelAlignCondAlgCfg(flags))
    
    kwargs.setdefault("PixelAlignmentStore", "PixelAlignmentStore")
    kwargs.setdefault("WriteKey", "PixelDetectorElementCollection")

    # FIXME
    # add artifical dependencies to SCT, TRT and Muon
    # conditions algs to ensure that the IOV
    # is identical to the IOV of the tracking geometry
    if flags.Detector.GeometryMuon and flags.Muon.enableAlignment:
        from MuonConfig.MuonGeometryConfig import MuonDetectorCondAlgCfg
        acc.merge(MuonDetectorCondAlgCfg(flags))
        kwargs.setdefault("MuonManagerKey", "MuonDetectorManager")
    if flags.Detector.GeometryTRT:
        from TRT_GeoModel.TRT_GeoModelConfig import TRT_ReadoutGeometryCfg
        acc.merge(TRT_ReadoutGeometryCfg(flags))
        kwargs.setdefault("TRT_DetEltContKey", "TRT_DetElementContainer")
    if not flags.GeoModel.Align.LegacyConditionsAccess and flags.Detector.GeometrySCT:
        from SCT_GeoModel.SCT_GeoModelConfig import SCT_AlignmentCfg
        acc.merge(SCT_AlignmentCfg(flags))
        kwargs.setdefault("SCTAlignmentStore", "SCTAlignmentStore")
    # end of hack

    acc.addCondAlgo(CompFactory.PixelDetectorElementCondAlg(name, **kwargs))    
    return acc

def PixelDistortionAlgCfg(flags, name="PixelDistortionAlg", **kwargs):
    """Return a ComponentAccumulator with configured PixelDistortionAlg"""
    acc = ComponentAccumulator()
    acc.merge(PixelConfigCondAlgCfg(flags))
    acc.merge(addFoldersSplitOnline(flags,"INDET","/Indet/Onl/PixelDist","/Indet/PixelDist",className="DetCondCFloat"))
    kwargs.setdefault("PixelModuleData", "PixelModuleData")
    kwargs.setdefault("ReadKey", "/Indet/PixelDist")
    kwargs.setdefault("WriteKey", "PixelDistortionData")
    from RngComps.RandomServices import AthRNGSvcCfg
    kwargs.setdefault("RndmSvc", acc.getPrimaryAndMerge(AthRNGSvcCfg(flags)).name)
    acc.addCondAlgo(CompFactory.PixelDistortionAlg(name, **kwargs))
    return acc

def PixelHitDiscCnfgAlgCfg(flags, name="PixelHitDiscCnfgAlg", **kwargs):
    """Return a ComponentAccumulator with configured PixelHitDiscCnfgAlg"""
    acc = ComponentAccumulator()
    acc.merge(PixelConfigCondAlgCfg(flags))
    # not for Run-1 data/MC
    if flags.GeoModel.IBLLayout in ("noIBL", "UNDEFINED"):
        return acc
    if flags.IOVDb.DatabaseInstance == "CONDBR2":
        acc.merge(addFolders(flags, "/PIXEL/HitDiscCnfg", "PIXEL", className="AthenaAttributeList"))
    elif flags.Input.isMC and flags.GeoModel.Run in [LHCPeriod.Run2, LHCPeriod.Run3]:
        acc.merge(addFoldersSplitOnline(flags,"PIXEL","/PIXEL/HitDiscCnfg","/PIXEL/HitDiscCnfg", className="AthenaAttributeList"))
    kwargs.setdefault("PixelModuleData", "PixelModuleData")
    kwargs.setdefault("ReadKey", "/PIXEL/HitDiscCnfg")
    kwargs.setdefault("WriteKey", "PixelHitDiscCnfgData")
    acc.addCondAlgo(CompFactory.PixelHitDiscCnfgAlg(name, **kwargs))
    return acc

def PixelOfflineCalibCondAlgCfg(flags, name="PixelOfflineCalibCondAlg", **kwargs):
    """Return a ComponentAccumulator with configured PixelOfflineCalibCondAlg"""
    acc = ComponentAccumulator()
    acc.merge(addFoldersSplitOnline(flags, "PIXEL", "/PIXEL/Onl/PixReco", "/PIXEL/PixReco", className="DetCondCFloat"))
    kwargs.setdefault("InputSource", 2)
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

def PixelRadSimFluenceMapAlgCfg(flags, name="PixelRadSimFluenceMapAlg", **kwargs):
    """Return a ComponentAccumulator with configured PixelRadSimFluenceMapAlg"""
    acc = ComponentAccumulator()
    acc.merge(PixelConfigCondAlgCfg(flags))
    kwargs.setdefault("PixelModuleData", "PixelModuleData")
    kwargs.setdefault("WriteRadiationFluenceMapKey", "PixelRadiationDamageFluenceMapData")
    acc.addCondAlgo(CompFactory.PixelRadSimFluenceMapAlg(name, **kwargs))
    return acc

def PixeldEdxAlgCfg(flags, name="PixeldEdxAlg", **kwargs):
    """Return a ComponentAccumulator with configured PixeldEdxAlg"""
    acc = ComponentAccumulator()
    acc.merge(addFoldersSplitOnline(flags, "PIXEL", "/PIXEL/Onl/PixdEdx", "/PIXEL/PixdEdx", className='AthenaAttributeList'))
    acc.addCondAlgo(CompFactory.PixeldEdxAlg(name, ReadFromCOOL = True, **kwargs))
    return acc

def PixelDetectorElementStatusCondAlgActiveOnlyCfg(flags, name = "PixelDetectorElementStatusCondAlgNoByteStreamErrorActiveOnly", **kwargs) :
    '''
    Condition alg to precompute the pixel detector element status.
    this algo does not consider the DCS status (and the byte stream errors which are event data)
    '''
    acc = ComponentAccumulator()
    if 'ConditionsSummaryTool' not in kwargs :
        from PixelConditionsTools.PixelConditionsSummaryConfig import PixelConditionsSummaryToolNoByteStreamErrorsActiveOnlyCfg
        kwargs.setdefault("ConditionsSummaryTool", acc.popToolsAndMerge( PixelConditionsSummaryToolNoByteStreamErrorsActiveOnlyCfg(flags)))
    kwargs.setdefault( "WriteKey", "PixelDetectorElementStatusNoByteStreamActiveOnly")
    acc.addCondAlgo( CompFactory.InDet.SiDetectorElementStatusCondAlg(name, **kwargs) )
    return acc

def PixelDetectorElementStatusCondAlgCfg(flags, name = "PixelDetectorElementStatusCondAlgNoByteStreamError", **kwargs) :
    '''
    Condition alg to precompute the create pixel detector element status which includes the DCS status
    this algo does not consider the byte stream errors which are event data
    '''
    acc = ComponentAccumulator()
    if 'ConditionsSummaryTool' not in kwargs :
        acc.merge( PixelDetectorElementStatusCondAlgActiveOnlyCfg(flags) )
        from PixelConditionsTools.PixelConditionsSummaryConfig import PixelActiveDetectorElementStatusToolCfg
        kwargs.setdefault("ConditionsSummaryTool", acc.popToolsAndMerge( PixelActiveDetectorElementStatusToolCfg(flags,
                                                                                                                 PixelDetElStatusCondDataBaseKey="PixelDetectorElementStatusNoByteStreamActiveOnly")) )
    kwargs.setdefault("WriteKey", "PixelDetectorElementStatusNoByteStream")
    acc.addCondAlgo( CompFactory.InDet.SiDetectorElementStatusCondAlg(name, **kwargs) )
    return acc

def PixelDetectorElementStatusAlgCfg(flags, name = "PixelDetectorElementStatusAlg", **kwargs) :
    '''
    Event alg which extends the pixel detector element status conditions data which does not consider the DCS status by the bytestream errors.
    This alg however does only consider errors concerning the module activity, not general errors.
    '''
    acc = ComponentAccumulator()
    active_only = kwargs.pop("ActiveOnly", False)
    if 'ConditionsSummaryTool' not in kwargs and not active_only :
        element_status_input=None
        if flags.InDet.usePixelDCS:
            acc.merge( PixelDetectorElementStatusCondAlgCfg(flags) )
            element_status_input="PixelDetectorElementStatusNoByteStream"
        else :
            # without DCS PixelDetectorElementStatusNoByteStream and PixelDetectorElementStatusNoByteStreamActiveOnly
            # are identically
            acc.merge( PixelDetectorElementStatusCondAlgActiveOnlyCfg(flags) )
            element_status_input="PixelDetectorElementStatusNoByteStreamActiveOnly"
        from PixelConditionsTools.PixelConditionsSummaryConfig import PixelByteStreamErrorDetectorElementStatusToolCfg
        kwargs.setdefault("ConditionsSummaryTool",
                          acc.popToolsAndMerge(PixelByteStreamErrorDetectorElementStatusToolCfg(flags, PixelDetElStatusCondDataBaseKey=element_status_input) ))

    elif 'ConditionsSummaryTool' not in kwargs and active_only :
        acc.merge( PixelDetectorElementStatusCondAlgActiveOnlyCfg(flags) )
        from PixelConditionsTools.PixelConditionsSummaryConfig import PixelByteStreamErrorDetectorElementStatusToolActiveOnlyCfg
        kwargs.setdefault("ConditionsSummaryTool",
                          acc.popToolsAndMerge(PixelByteStreamErrorDetectorElementStatusToolActiveOnlyCfg(flags, PixelDetElStatusCondDataBaseKey="PixelDetectorElementStatusNoByteStreamActiveOnly")))

    kwargs.setdefault("WriteKey","PixelDetectorElementStatus")

    acc.addEventAlgo( CompFactory.InDet.SiDetectorElementStatusAlg(name, **kwargs) )
    return acc

def PixelDetectorElementStatusAlgActiveOnlyCfg(flags, name = "PixelDetectorElementStatusAlgActiveOnly", **kwargs) :
    '''
    Event alg which extends the pixel detector element status conditions data which does not consider the DCS status by the bytestream errors.
    This alg however does only consider errors concerning the module activity, not general errors.
    '''
    return PixelDetectorElementStatusAlgCfg(flags, name,
                                            WriteKey   = "PixelDetectorElementStatusActiveOnly",
                                            ActiveOnly = True)


if __name__ == '__main__':
    from AthenaConfiguration.MainServicesConfig import MainServicesCfg
    from AthenaConfiguration.AllConfigFlags import ConfigFlags as flags

    flags.fillFromArgs()

    from AthenaConfiguration.TestDefaults import defaultTestFiles
    flags.Input.Files = defaultTestFiles.RAW # need to update this depending on EDMversion
    flags.Exec.MaxEvents=5

    flags.lock()

    acc = MainServicesCfg(flags)
    acc.merge( PixelDetectorElementCondAlgCfg(flags) )
    acc.printConfig(withDetails=True)
    with open("PixelConditions.pkl", "wb") as file:
        acc.store(file)
    # TODO decide if we want to run actually
    sc = acc.run()
    # if sc.isFailure():
    #     import sys
    #     sys.exit(-1)


