# Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
from AthenaConfiguration.ComponentAccumulator import ComponentAccumulator
from AthenaConfiguration.ComponentFactory import CompFactory

def BarrelCryostatCalibrationCalculatorCfg(ConfigFlags, name="BarrelCryostatCalibrationCalculator", **kwargs):
    result = ComponentAccumulator()
    result.addService( CompFactory.LArG4.BarrelCryostat.CalibrationCalculator(name, **kwargs), primary=True )
    return result


def BarrelCryostatCalibrationLArCalculatorCfg(ConfigFlags, name="BarrelCryostatCalibrationLArCalculator", **kwargs):
    result = ComponentAccumulator()
    result.addService( CompFactory.LArG4.BarrelCryostat.CalibrationLArCalculator(name, **kwargs), primary=True )
    return result


def BarrelCryostatCalibrationMixedCalculatorCfg(ConfigFlags, name="BarrelCryostatCalibrationMixedCalculator", **kwargs):
    result = ComponentAccumulator()
    result.addService( CompFactory.LArG4.BarrelCryostat.CalibrationMixedCalculator(name, **kwargs), primary=True )
    return result


def DMCalibrationCalculatorCfg(ConfigFlags, name="DMCalibrationCalculator", **kwargs):
    result = ComponentAccumulator()
    result.addService( CompFactory.LArG4.DM.CalibrationCalculator(name, **kwargs), primary=True )
    return result


def BarrelCalibrationCalculatorCfg(ConfigFlags, name="BarrelCalibrationCalculator", **kwargs):
    result = ComponentAccumulator()
    result.addService( CompFactory.LArG4.Barrel.CalibrationCalculator(name, **kwargs), primary=True )
    return result


def BarrelPresamplerCalibrationCalculatorCfg(ConfigFlags, name="BarrelPresamplerCalibrationCalculator", **kwargs):
    result = ComponentAccumulator()
    result.addService( CompFactory.LArG4.BarrelPresampler.CalibrationCalculator(name, **kwargs), primary=True )
    return result


def EMBCalculatorCfg(ConfigFlags, name="EMBCalculator", **kwargs):
    result = ComponentAccumulator()
    kwargs.setdefault("GeometryCalculator", result.getPrimaryAndMerge(LArBarrelGeometryCfg(ConfigFlags)).name)
    result.addService(CompFactory.LArBarrelCalculator(name, **kwargs), primary=True)
    return result


def EMBPresamplerCalculatorCfg(ConfigFlags, name="EMBPresamplerCalculator", **kwargs):
    result = ComponentAccumulator()
    kwargs.setdefault("GeometryCalculator", result.getPrimaryAndMerge(LArBarrelPresamplerGeometryCfg(ConfigFlags)).name)

    result.addService(CompFactory.LArBarrelPresamplerCalculator(name, **kwargs), primary=True)
    return result


def LArBarrelGeometryCfg(ConfigFlags, name="LArBarrelGeometry", **kwargs):
    result = ComponentAccumulator()
    result.addService(CompFactory.LArG4.Barrel.Geometry(name, **kwargs), primary=True)
    return result


def LArBarrelPresamplerGeometryCfg(ConfigFlags, name="LArBarrelPresamplerGeometry", **kwargs):
    result = ComponentAccumulator()
    result.addService(CompFactory.LArG4.BarrelPresampler.Geometry(name, **kwargs), primary=True)
    return result
