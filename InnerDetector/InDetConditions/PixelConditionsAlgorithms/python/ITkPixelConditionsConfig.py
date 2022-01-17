"""Define functions to configure Pixel conditions algorithms

Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
"""
from AthenaConfiguration.ComponentAccumulator import ComponentAccumulator
from AthenaConfiguration.ComponentFactory import CompFactory
from IOVDbSvc.IOVDbSvcConfig import addFolders, addFoldersSplitOnline

def ITkPixelConfigCondAlgCfg(flags, name="ITkPixelConfigCondAlg", **kwargs):
    """Return a ComponentAccumulator with configured PixelConfigCondAlg for ITk"""
    kwargs.setdefault("WriteKey", "ITkPixelModuleData")
    # This one has the highest chance of premature divergence so just take the default Pixel for now
    from PixelConditionsAlgorithms.PixelConditionsConfig import PixelConfigCondAlgCfg
    return PixelConfigCondAlgCfg(flags, name, **kwargs)

def ITkPixelAlignCondAlgCfg(flags, name="ITkPixelAlignCondAlg", **kwargs):
    """Return a ComponentAccumulator with configured PixelAlignCondAlg for ITk"""
    acc = ComponentAccumulator()

    if flags.GeoModel.Align.Dynamic:
        raise RuntimeError("Dynamic alignment not supported for ITk yet")
    else:
        acc.merge(addFoldersSplitOnline(flags, "INDET", "/Indet/Onl/Align", "/Indet/Align", className="AlignableTransformContainer"))

    kwargs.setdefault("DetManagerName", "ITkPixel")
    kwargs.setdefault("UseDynamicAlignFolders", flags.GeoModel.Align.Dynamic)
    kwargs.setdefault("ReadKeyStatic", "/Indet/Align")
    # kwargs.setdefault("ReadKeyDynamicL1", "/Indet/AlignL1/ID")
    # kwargs.setdefault("ReadKeyDynamicL2", "/Indet/AlignL2/PIX")
    # kwargs.setdefault("ReadKeyDynamicL3", "/Indet/AlignL3")
    kwargs.setdefault("ReadKeyIBLDist", "")
    kwargs.setdefault("WriteKey", "ITkPixelAlignmentStore")

    acc.addCondAlgo(CompFactory.PixelAlignCondAlg(name, **kwargs))
    return acc

def ITkPixelChargeCalibCondAlgCfg(flags, name="ITkPixelChargeCalibCondAlg", **kwargs):
    """Return a ComponentAccumulator with configured PixelChargeCalibCondAlg for ITk"""
    acc = ComponentAccumulator()
    acc.merge(ITkPixelConfigCondAlgCfg(flags))
    acc.merge(addFoldersSplitOnline(flags, "PIXEL", "/PIXEL/Onl/PixCalib", "/PIXEL/PixCalib", className="CondAttrListCollection"))
    from PixelGeoModelXml.ITkPixelGeoModelConfig import ITkPixelReadoutGeometryCfg
    acc.merge(ITkPixelReadoutGeometryCfg(flags))
    kwargs.setdefault("PixelDetEleCollKey", "ITkPixelDetectorElementCollection")
    kwargs.setdefault("PixelModuleData", "ITkPixelModuleData")
    kwargs.setdefault("ReadKey", "")  # TODO: enable when ready
    kwargs.setdefault("WriteKey", "ITkPixelChargeCalibCondData")
    acc.addCondAlgo(CompFactory.PixelChargeCalibCondAlg(name, **kwargs))
    return acc

def ITkPixelChargeLUTCalibCondAlgCfg(flags, name="ITkPixelChargeLUTCalibCondAlg", **kwargs):
    """Return a ComponentAccumulator with configured PixelChargeLUTCalibCondAlg for ITk"""
    acc = ComponentAccumulator()
    acc.merge(ITkPixelConfigCondAlgCfg(flags))
    acc.merge(addFoldersSplitOnline(flags, "PIXEL", "/PIXEL/Onl/PixCalib", "/PIXEL/PixCalib", className="CondAttrListCollection"))
    kwargs.setdefault("PixelDetEleCollKey", "ITkPixelDetectorElementCollection")
    kwargs.setdefault("PixelModuleData", "ITkPixelModuleData")
    kwargs.setdefault("ReadKey", "/PIXEL/ChargeCalibration")
    kwargs.setdefault("WriteKey", "ITkPixelChargeCalibCondData")
    acc.addCondAlgo(CompFactory.PixelChargeLUTCalibCondAlg(name, **kwargs))
    return acc

def ITkPixelDCSCondHVAlgCfg(flags, name="ITkPixelDCSCondHVAlg", **kwargs):
    """Return a ComponentAccumulator with configured PixelDCSCondHVAlg for ITk"""
    acc = ComponentAccumulator()
    acc.merge(ITkPixelConfigCondAlgCfg(flags))
    if flags.Common.isOnline:
        kwargs.update( ReadKey="/PIXEL/HLT/DCS/HV")
        acc.merge(addFolders(flags, kwargs["ReadKey"], "PIXEL_ONL", className="CondAttrListCollection"))
    else:
        # kwargs.update( ReadKey="/PIXEL/DCS/HV")
        # acc.merge(addFolders(flags, kwargs["ReadKey"], "DCS_OFL", className="CondAttrListCollection"))
        kwargs.update(ReadKey="")  # disable for ITk for now
    kwargs.setdefault("PixelModuleData", "ITkPixelModuleData")
    kwargs.setdefault("WriteKey", "ITkPixelDCSHVCondData")
    acc.addCondAlgo(CompFactory.PixelDCSCondHVAlg(name, **kwargs))
    return acc

def ITkPixelDCSCondStateAlgCfg(flags, name="ITkPixelDCSCondStateAlg", **kwargs):
    """Return a ComponentAccumulator with configured PixelDCSCondStateAlg for ITk"""
    acc = ComponentAccumulator()
    kwargs.setdefault("ReadKeyState", "") #To be configured when final DCS implementation for ITk becomes available
    kwargs.setdefault("WriteKeyState", "ITkPixelDCSStateCondData")
    acc.addCondAlgo(CompFactory.PixelDCSCondStateAlg(name, **kwargs))
    return acc

def ITkPixelDCSCondStatusAlgCfg(flags, name="ITkPixelDCSCondStatusAlg", **kwargs):
    """Return a ComponentAccumulator with configured PixelDCSCondStatusAlg for ITk"""
    acc = ComponentAccumulator()
    kwargs.setdefault("ReadKeyStatus", "") #To be configured when final DCS implementation for ITk becomes available
    kwargs.setdefault("WriteKeyStatus", "ITkPixelDCSStatusCondData")
    acc.addCondAlgo(CompFactory.PixelDCSCondStatusAlg(name, **kwargs))
    return acc

def ITkPixelDCSCondTempAlgCfg(flags, name="ITkPixelDCSCondTempAlg", **kwargs):
    """Return a ComponentAccumulator with configured PixelDCSCondTempAlg for ITk"""
    acc = ComponentAccumulator()
    acc.merge(ITkPixelConfigCondAlgCfg(flags))
    if flags.Common.isOnline:
        kwargs.setdefault("ReadKey", "/PIXEL/HLT/DCS/TEMPERATURE")
        acc.merge(addFolders(flags, kwargs["ReadKey"], "PIXEL_ONL", className="CondAttrListCollection"))
    else:
        kwargs.setdefault("ReadKey", "/PIXEL/DCS/TEMPERATURE")
        acc.merge(addFolders(flags, kwargs["ReadKey"], "DCS_OFL", className="CondAttrListCollection"))
    kwargs.setdefault("PixelModuleData", "ITkPixelModuleData")
    kwargs.setdefault("WriteKey", "ITkPixelDCSTempCondData")
    acc.addCondAlgo(CompFactory.PixelDCSCondTempAlg(name, **kwargs))
    return acc

def ITkPixelDeadMapCondAlgCfg(flags, name="ITkPixelDeadMapCondAlg", **kwargs):
    """Return a ComponentAccumulator with configured PixelDeadMapCondAlg for ITk"""
    acc = ComponentAccumulator()
    acc.merge(ITkPixelConfigCondAlgCfg(flags))

    # TODO: not enabled for ITk for now
    kwargs.setdefault("ReadKey", "")
    kwargs.setdefault("WriteKey", "ITkPixelDeadMapCondData")
    acc.addCondAlgo(CompFactory.PixelDeadMapCondAlg(name, **kwargs))
    return acc

def ITkPixelDetectorElementCondAlgCfg(flags, name="ITkPixelDetectorElementCondAlg", **kwargs):
    """Return a ComponentAccumulator with configured PixelDetectorElementCondAlg for ITk"""
    acc = ComponentAccumulator()
    acc.merge(ITkPixelAlignCondAlgCfg(flags))
    kwargs.setdefault("DetManagerName", "ITkPixel")
    kwargs.setdefault("PixelAlignmentStore", "ITkPixelAlignmentStore")
    kwargs.setdefault("WriteKey", "ITkPixelDetectorElementCollection")
    acc.addCondAlgo(CompFactory.PixelDetectorElementCondAlg(name, **kwargs))
    return acc

def ITkPixelDistortionAlgCfg(flags, name="ITkPixelDistortionAlg", **kwargs):
    """Return a ComponentAccumulator with configured PixelDistortionAlg for ITk"""
    acc = ComponentAccumulator()
    acc.merge(ITkPixelConfigCondAlgCfg(flags))
    acc.merge(addFoldersSplitOnline(flags,"INDET", "/Indet/Onl/PixelDist", "/Indet/PixelDist", className="DetCondCFloat"))
    kwargs.setdefault("PixelModuleData", "ITkPixelModuleData")
    kwargs.setdefault("ReadKey", "/Indet/PixelDist")
    kwargs.setdefault("WriteKey", "ITkPixelDistortionData")
    from RngComps.RandomServices import AthRNGSvcCfg
    kwargs.setdefault("RndmSvc", acc.getPrimaryAndMerge(AthRNGSvcCfg(flags)).name)
    acc.addCondAlgo(CompFactory.PixelDistortionAlg(name, **kwargs))
    return acc

def ITkPixelOfflineCalibCondAlgCfg(flags, name="ITkPixelOfflineCalibCondAlg", **kwargs):
    """Return a ComponentAccumulator with configured ITkPixelOfflineCalibCondAlg"""
    acc = ComponentAccumulator()

    CoolDataBaseFolder = '/PIXEL/ITkClusterError'
    DetDescrVersion = flags.GeoModel.AtlasVersion
    ctag = 'PixelITkError_v4_' + DetDescrVersion
    cfoldertag = CoolDataBaseFolder+' <tag>'+ctag+'</tag>'
    acc.merge( addFoldersSplitOnline(flags,'PIXEL',[cfoldertag],[cfoldertag],splitMC=True,className="CondAttrListCollection") )

    kwargs.setdefault("ReadKey", "/PIXEL/ITkClusterError")
    kwargs.setdefault("WriteKey", "ITkPixelOfflineCalibData")
    kwargs.setdefault("InputSource", 2)
    acc.addCondAlgo(CompFactory.ITk.PixelOfflineCalibCondAlg(name, **kwargs))
    return acc
