"""Define methods to configure SiPropertiesTool

Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
"""
from PixelConditionsTools.PixelDCSConditionsConfig import PixelDCSConditionsCfg
from AthenaConfiguration.ComponentFactory import CompFactory
SiPropertiesTool=CompFactory.SiPropertiesTool
PixelSiPropertiesCondAlg=CompFactory.PixelSiPropertiesCondAlg

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
    acc.setPrivateTools(tool)
    return acc

