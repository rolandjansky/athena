"""Define methods to configure SiPropertiesTool

Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
"""
from AthenaConfiguration.ComponentAccumulator import ComponentAccumulator
from AthenaConfiguration.ComponentFactory import CompFactory
SiPropertiesTool=CompFactory.SiPropertiesTool
PixelSiPropertiesCondAlg=CompFactory.PixelSiPropertiesCondAlg

from PixelConditionsAlgorithms.PixelConditionsConfig import (
    PixelDCSCondHVAlgCfg, PixelDCSCondTempAlgCfg,
    PixelDCSCondStateAlgCfg, PixelDCSCondStatusAlgCfg
)

def PixelSiPropertiesToolCfg(flags, name="PixelSiPropertiesTool", **kwargs):
    """Return a SiPropertiesTool configured for Pixel"""
    kwargs.setdefault("DetectorName", "Pixel")
    kwargs.setdefault("ReadKey", "PixelSiliconPropertiesVector")
    return SiPropertiesTool(name=name, **kwargs)

def PixelSiPropertiesCfg(flags, name="PixelSiPropertiesCondAlg", **kwargs):
    """Return configured ComponentAccumulator and tool for PixelSiPropertiesCondAlg

    SiPropertiesTool may be provided in kwargs
    """
    acc = ComponentAccumulator()
    acc.merge(PixelDCSCondHVAlgCfg(flags))
    acc.merge(PixelDCSCondTempAlgCfg(flags))
    acc.merge(PixelDCSCondStateAlgCfg(flags))
    acc.merge(PixelDCSCondStatusAlgCfg(flags))
    tool = kwargs.get("SiPropertiesTool", PixelSiPropertiesToolCfg(flags))
    acc.addCondAlgo(PixelSiPropertiesCondAlg(name, **kwargs))
    acc.setPrivateTools(tool)
    return acc

