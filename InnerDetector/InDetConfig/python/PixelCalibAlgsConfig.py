# Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
# Configuration of PixelCalibAlgs package
from AthenaConfiguration.ComponentFactory import CompFactory

def PixelChargeToTConversionCfg(flags, name='InDetPixelChargeToTConversion', **kwargs):
    from PixelReadoutGeometry.PixelReadoutGeometryConfig import PixelReadoutManagerCfg
    acc = PixelReadoutManagerCfg(flags)

    from PixelConditionsAlgorithms.PixelConditionsConfig import PixelConfigCondAlgCfg, PixelChargeCalibCondAlgCfg
    acc.merge(PixelConfigCondAlgCfg(flags))
    acc.merge(PixelChargeCalibCondAlgCfg(flags))
    
    acc.addEventAlgo(CompFactory.PixelChargeToTConversion(name, **kwargs))
    return acc

def ITkPixelChargeToTConversionCfg(flags, name='ITkPixelChargeToTConversion', **kwargs):
    from PixelReadoutGeometry.PixelReadoutGeometryConfig import ITkPixelReadoutManagerCfg
    acc = ITkPixelReadoutManagerCfg(flags)

    from PixelConditionsAlgorithms.ITkPixelConditionsConfig import ITkPixelModuleConfigCondAlgCfg, ITkPixelChargeCalibCondAlgCfg
    acc.merge(ITkPixelModuleConfigCondAlgCfg(flags))
    acc.merge(ITkPixelChargeCalibCondAlgCfg(flags))
    
    kwargs.setdefault("PixelClusterContainer", "ITkPixelClusters")
    kwargs.setdefault("PixelReadoutManager", "ITkPixelReadoutManager")
    kwargs.setdefault("PixelModuleData", "ITkPixelModuleData")
    kwargs.setdefault("PixelChargeCalibCondData", "ITkPixelChargeCalibCondData")
    kwargs.setdefault("PixelDetEleCollKey", "ITkPixelDetectorElementCollection")
    kwargs.setdefault("IBLParameterSvc", "")

    acc.addEventAlgo(CompFactory.PixelChargeToTConversion(name, **kwargs))
    return acc
