# Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration

"""Define methods to configure SCTLorentzAngleTool
"""
from SiPropertiesTool.PixelSiPropertiesConfig import PixelSiPropertiesCfg
from AthenaConfiguration.ComponentFactory import CompFactory
SiLorentzAngleTool=CompFactory.SiLorentzAngleTool
PixelSiLorentzAngleCondAlg=CompFactory.PixelSiLorentzAngleCondAlg
from MagFieldServices.MagFieldServicesConfig import MagneticFieldSvcCfg

from PixelConditionsAlgorithms.PixelConditionsConfig import (
    PixelDCSCondHVAlgCfg, PixelDCSCondTempAlgCfg,
    PixelDCSCondStateAlgCfg, PixelDCSCondStatusAlgCfg
)

def PixelLorentzAngleTool(flags, name="PixelLorentzAngleTool", **kwargs):
    """Return a SiLorentzAngleTool configured for Pixel"""
    kwargs.setdefault("DetectorName", "Pixel")
    kwargs.setdefault("SiLorentzAngleCondData", "PixelSiLorentzAngleCondData")
    kwargs.setdefault("DetEleCollKey", "PixelDetectorElementCollection")
    kwargs.setdefault("UseMagFieldCache", True)
    return SiLorentzAngleTool(name, **kwargs)

def PixelLorentzAngleCfg(flags, name="PixelSiLorentzAngleCondAlg", **kwargs):
    """Return configured ComponentAccumulator and tool for PixelLorentzAngle

    SiLorentzAngleTool may be provided in kwargs
    """
    acc  = MagneticFieldSvcCfg(flags)
    tool = kwargs.get("SiLorentzAngleTool", PixelLorentzAngleTool(flags))
    acc.merge(PixelDCSCondHVAlgCfg(flags))
    acc.merge(PixelDCSCondTempAlgCfg(flags))
    acc.merge(PixelDCSCondStateAlgCfg(flags))
    acc.merge(PixelDCSCondStatusAlgCfg(flags))
    SiPropAcc = PixelSiPropertiesCfg(flags)
    kwargs.setdefault("SiPropertiesTool", SiPropAcc.popPrivateTools())
    acc.merge(SiPropAcc)
    kwargs.setdefault("UseMagFieldCache", tool.UseMagFieldCache)
    kwargs.setdefault("UseMagFieldDcs", not flags.Common.isOnline)
    acc.addCondAlgo(PixelSiLorentzAngleCondAlg(name, **kwargs))
    acc.setPrivateTools(tool)
    return acc

