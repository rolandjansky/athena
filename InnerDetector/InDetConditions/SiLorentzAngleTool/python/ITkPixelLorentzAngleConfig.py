# Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration

"""Define methods to configure ITkPixelLorentzAngleTool
"""
from AthenaConfiguration.ComponentFactory import CompFactory
from MagFieldServices.MagFieldServicesConfig import AtlasFieldCacheCondAlgCfg
from PixelConditionsAlgorithms.ITkPixelConditionsConfig import (
    ITkPixelDCSCondHVAlgCfg, ITkPixelDCSCondTempAlgCfg
)
from PixelGeoModelXml.ITkPixelGeoModelConfig import ITkPixelReadoutGeometryCfg
from SiPropertiesTool.ITkPixelSiPropertiesConfig import ITkPixelSiPropertiesCfg

def ITkPixelLorentzAngleToolCfg(flags, name="ITkPixelLorentzAngleTool", **kwargs):
    """Return a SiLorentzAngleTool configured for ITk Pixel"""
    acc = ITkPixelLorentzAngleCondAlgCfg(flags)
    kwargs.setdefault("DetectorName", "Pixel")
    kwargs.setdefault("SiLorentzAngleCondData", "ITkPixelSiLorentzAngleCondData")
    kwargs.setdefault("DetEleCollKey", "ITkPixelDetectorElementCollection")
    kwargs.setdefault("UseMagFieldCache", True)
    acc.setPrivateTools(CompFactory.SiLorentzAngleTool(name, **kwargs))
    return acc

def ITkPixelLorentzAngleCondAlgCfg(flags, name="ITkPixelSiLorentzAngleCondAlg", **kwargs):
    acc  = AtlasFieldCacheCondAlgCfg(flags)
    acc.merge(ITkPixelReadoutGeometryCfg(flags)) # To produce ITkPixelDetectorElementCollection
    acc.merge(ITkPixelDCSCondHVAlgCfg(flags))
    acc.merge(ITkPixelDCSCondTempAlgCfg(flags))
    kwargs.setdefault("SiPropertiesTool", acc.popToolsAndMerge(ITkPixelSiPropertiesCfg(flags)))
    kwargs.setdefault("UseMagFieldCache", True)
    kwargs.setdefault("UseMagFieldDcs", not flags.Common.isOnline)
    kwargs.setdefault("Disable3DCorrection", True) # ITk-specific
    kwargs.setdefault("PixelModuleData", "ITkPixelModuleData")
    kwargs.setdefault("ReadKeyeTemp", "ITkPixelDCSTempCondData")
    kwargs.setdefault("ReadKeyHV", "ITkPixelDCSHVCondData")
    kwargs.setdefault("PixelDetEleCollKey", "ITkPixelDetectorElementCollection")
    kwargs.setdefault("WriteKey", "ITkPixelSiLorentzAngleCondData")
    acc.addCondAlgo(CompFactory.PixelSiLorentzAngleCondAlg(name, **kwargs))
    return acc

