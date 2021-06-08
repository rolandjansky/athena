# Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration

"""Define methods to configure ITkPixelLorentzAngleTool
"""
from AthenaConfiguration.ComponentFactory import CompFactory
SiLorentzAngleTool=CompFactory.SiLorentzAngleTool
PixelSiLorentzAngleCondAlg=CompFactory.PixelSiLorentzAngleCondAlg
from MagFieldServices.MagFieldServicesConfig import MagneticFieldSvcCfg
from PixelConditionsAlgorithms.ITkPixelConditionsConfig import (
    ITkPixelDCSCondHVAlgCfg, ITkPixelDCSCondTempAlgCfg
)
from SiPropertiesTool.ITkPixelSiPropertiesConfig import ITkPixelSiPropertiesCfg

def ITkPixelLorentzAngleTool(flags, name="ITkPixelLorentzAngleTool", **kwargs):
    """Return a SiLorentzAngleTool configured for ITk Pixel"""
    kwargs.setdefault("DetectorName", "Pixel")
    kwargs.setdefault("SiLorentzAngleCondData", "ITkPixelSiLorentzAngleCondData")
    kwargs.setdefault("DetEleCollKey", "ITkPixelDetectorElementCollection")
    kwargs.setdefault("UseMagFieldCache", True)
    return SiLorentzAngleTool(name, **kwargs)

def ITkPixelLorentzAngleCfg(flags, name="ITkPixelSiLorentzAngleCondAlg", **kwargs):
    """Return configured ComponentAccumulator and tool for ITkPixelLorentzAngle

    SiLorentzAngleTool may be provided in kwargs
    """
    acc  = MagneticFieldSvcCfg(flags)
    tool = kwargs.get("SiLorentzAngleTool", ITkPixelLorentzAngleTool(flags))
    acc.merge(ITkPixelDCSCondHVAlgCfg(flags))
    acc.merge(ITkPixelDCSCondTempAlgCfg(flags))
    SiPropAcc = ITkPixelSiPropertiesCfg(flags)
    kwargs.setdefault("SiPropertiesTool", SiPropAcc.popPrivateTools())
    acc.merge(SiPropAcc)
    kwargs.setdefault("UseMagFieldCache", tool.UseMagFieldCache)
    kwargs.setdefault("UseMagFieldDcs", not flags.Common.isOnline)
    kwargs.setdefault("PixelModuleData", "ITkPixelModuleData")
    kwargs.setdefault("ReadKeyeTemp", "ITkPixelDCSTempCondData")
    kwargs.setdefault("ReadKeyHV", "ITkPixelDCSHVCondData")
    kwargs.setdefault("PixelDetEleCollKey", "ITkPixelDetectorElementCollection")
    kwargs.setdefault("WriteKey", "ITkPixelSiLorentzAngleCondData")
    acc.addCondAlgo(PixelSiLorentzAngleCondAlg(name, **kwargs))
    acc.setPrivateTools(tool)
    return acc

