"""Define methods to configure SiPropertiesTool

Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
"""
from AthenaConfiguration.ComponentAccumulator import ComponentAccumulator
from AthenaConfiguration.ComponentFactory import CompFactory

from PixelConditionsAlgorithms.ITkPixelConditionsConfig import (
    ITkPixelDCSCondHVAlgCfg, ITkPixelDCSCondTempAlgCfg
)

def ITkPixelSiPropertiesToolCfg(flags, name="ITkPixelSiPropertiesTool", **kwargs):
    """Return a SiPropertiesTool configured for ITk Pixel"""
    kwargs.setdefault("DetectorName", "Pixel")
    kwargs.setdefault("ReadKey", "ITkPixelSiliconPropertiesVector")
    SiPropertiesTool=CompFactory.SiPropertiesTool
    return SiPropertiesTool(name=name, **kwargs)

def ITkPixelSiPropertiesCfg(flags, name="ITkPixelSiPropertiesCondAlg", **kwargs):
    """Return configured ComponentAccumulator and tool for ITkPixelSiPropertiesCondAlg

    SiPropertiesTool may be provided in kwargs
    """
    acc = ComponentAccumulator()
    acc.merge(ITkPixelDCSCondHVAlgCfg(flags))
    acc.merge(ITkPixelDCSCondTempAlgCfg(flags))
    tool = kwargs.get("SiPropertiesTool", ITkPixelSiPropertiesToolCfg(flags))
    kwargs.setdefault("PixelDetEleCollKey", "ITkPixelDetectorElementCollection")
    kwargs.setdefault("ReadKeyeTemp", "ITkPixelDCSTempCondData")
    kwargs.setdefault("ReadKeyHV", "ITkPixelDCSHVCondData")
    kwargs.setdefault("WriteKey", "ITkPixelSiliconPropertiesVector")
    PixelSiPropertiesCondAlg=CompFactory.PixelSiPropertiesCondAlg
    acc.addCondAlgo(PixelSiPropertiesCondAlg(name, **kwargs))
    acc.setPrivateTools(tool)
    return acc
