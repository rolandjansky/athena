"""Define methods to configure SiPropertiesTool

Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
"""
from AthenaConfiguration.ComponentAccumulator import ComponentAccumulator
from PixelConditionsTools.PixelDCSConditionsConfig import PixelDCSConditionsCfg
from SiPropertiesTool.SiPropertiesToolConf import SiPropertiesTool
from SiPropertiesTool.SiPropertiesToolConf import PixelSiPropertiesCondAlg

def PixelSiPropertiesToolCfg(flags, name="PixelSiPropertiesTool", **kwargs):
    """Return a SiPropertiesTool configured for Pixel"""
    kwargs.setdefault("DetectorName", "Pixel")
    kwargs.setdefault("ReadKey", "PixelSiliconPropertiesVector")
    return SiPropertiesTool(name=name, **kwargs)

def PixelSiPropertiesCfg(flags, name="PixelSiPropertiesCondAlg", **kwargs):
    """Return configured ComponentAccumulator and tool for PixelSiPropertiesCondAlg

    SiPropertiesTool may be provided in kwargs
    """
    acc = PixelDCSConditionsCfg(flags)
    tool = kwargs.get("SiPropertiesTool", PixelSiPropertiesToolCfg(flags))
    acc.addCondAlgo(PixelSiPropertiesCondAlg(name, **kwargs))
    return acc, tool

