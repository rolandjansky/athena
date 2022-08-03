# Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration

"""Define methods to configure SCTLorentzAngleTool
"""
from SiPropertiesTool.PixelSiPropertiesConfig import PixelSiPropertiesCfg
from AthenaConfiguration.ComponentFactory import CompFactory
from MagFieldServices.MagFieldServicesConfig import AtlasFieldCacheCondAlgCfg
from PixelConditionsAlgorithms.PixelConditionsConfig import (
    PixelDCSCondHVAlgCfg, PixelDCSCondTempAlgCfg
)
from PixelGeoModel.PixelGeoModelConfig import PixelReadoutGeometryCfg

def PixelLorentzAngleToolCfg(flags, name="PixelLorentzAngleTool", **kwargs):
    """Return a SiLorentzAngleTool configured for Pixel"""
    acc = PixelLorentzAngleCondAlgCfg(flags)
    kwargs.setdefault("DetectorName", "Pixel")
    kwargs.setdefault("SiLorentzAngleCondData", "PixelSiLorentzAngleCondData")
    kwargs.setdefault("DetEleCollKey", "PixelDetectorElementCollection")
    kwargs.setdefault("UseMagFieldCache", True)
    acc.setPrivateTools(CompFactory.SiLorentzAngleTool(name, **kwargs))
    return acc

def PixelLorentzAngleCondAlgCfg(flags, name="PixelSiLorentzAngleCondAlg", **kwargs):
    acc  = AtlasFieldCacheCondAlgCfg(flags)
    acc.merge(PixelReadoutGeometryCfg(flags)) # To produce PixelDetectorElementCollection
    acc.merge(PixelDCSCondHVAlgCfg(flags))
    acc.merge(PixelDCSCondTempAlgCfg(flags))
    kwargs.setdefault("SiPropertiesTool", acc.popToolsAndMerge(PixelSiPropertiesCfg(flags)))
    kwargs.setdefault("UseMagFieldCache", True)
    kwargs.setdefault("UseMagFieldDcs", not flags.Common.isOnline)
    acc.addCondAlgo(CompFactory.PixelSiLorentzAngleCondAlg(name, **kwargs))
    return acc

