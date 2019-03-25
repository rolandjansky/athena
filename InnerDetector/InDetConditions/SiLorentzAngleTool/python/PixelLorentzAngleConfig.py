"""Define methods to configure SCTLorentzAngleTool

Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
"""
from AthenaConfiguration.ComponentAccumulator import ComponentAccumulator
from SiPropertiesTool.PixelSiPropertiesConfig import PixelSiPropertiesCfg
from SiLorentzAngleTool.SiLorentzAngleToolConf import SiLorentzAngleTool
from SiLorentzAngleTool.SiLorentzAngleToolConf import PixelSiLorentzAngleCondAlg
from PixelConditionsTools.PixelDCSConditionsConfig import PixelDCSConditionsCfg
from MagFieldServices.MagFieldServicesConfig import MagneticFieldSvcCfg

def PixelLorentzAngleToolCfg(flags, name="PixelLorentzAngleTool", **kwargs):
    """Return a SiLorentzAngleTool configured for Pixel"""
    kwargs.setdefault("DetectorName", "Pixel")
    kwargs.setdefault("SiLorentzAngleCondData", "PixelSiLorentzAngleCondData")
    kwargs.setdefault("UseMagFieldSvc", True)
    return SiLorentzAngleTool(name, **kwargs)

def PixelLorentzAngleCfg(flags, name="PixelSiLorentzAngleCondAlg", **kwargs):
    """Return configured ComponentAccumulator and tool for PixelLorentzAngle

    SiLorentzAngleTool may be provided in kwargs
    """
    acc, svc = MagneticFieldSvcCfg(flags)
    tool = kwargs.get("SiLorentzAngleTool", PixelLorentzAngleToolCfg(flags))
    acc.merge(PixelDCSConditionsCfg(flags))
    SiPropAcc, SiPropTool = PixelSiPropertiesCfg(flags)
    acc.merge(SiPropAcc)
    kwargs.setdefault("SiPropertiesTool", SiPropTool)
    kwargs.setdefault("UseMagFieldSvc", tool.UseMagFieldSvc)
    kwargs.setdefault("UseMagFieldDcs", not flags.Common.isOnline)
    acc.addCondAlgo(PixelSiLorentzAngleCondAlg(name, **kwargs))
    return acc, tool

