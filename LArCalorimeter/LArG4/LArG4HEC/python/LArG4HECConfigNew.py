# Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration

from AthenaConfiguration.ComponentAccumulator import ComponentAccumulator
from AthenaConfiguration.ComponentFactory import CompFactory
LArHECLocalCalculator=CompFactory.LArHECLocalCalculator
LArG4__HEC__LocalCalibrationCalculator=CompFactory.LArG4.HEC.LocalCalibrationCalculator
LArG4__HEC__LArHECCalibrationWheelCalculator=CompFactory.LArG4.HEC.LArHECCalibrationWheelCalculator
LArG4__HEC__LocalGeometry=CompFactory.LArG4.HEC.LocalGeometry
LArHECWheelCalculator=CompFactory.LArHECWheelCalculator
LArG4__HEC__HECGeometry=CompFactory.LArG4.HEC.HECGeometry
from LArG4HEC import HECGeometryType

# Not used???
def LArHECLocalCalculatorCfg(ConfigFlags, name="LArHECLocalCalculator", **kwargs):
    return LArHECLocalCalculator(name, **kwargs)


def LocalCalibrationCalculatorCfg(ConfigFlags, name="LocalCalibrationCalculator", **kwargs):
    return LArG4__HEC__LocalCalibrationCalculator(name, **kwargs)


def LocalHECGeometry(name="LocalHECGeometry", **kwargs):
    return LArG4__HEC__LocalGeometry(name, **kwargs)


def HECWheelCalculatorCfg(ConfigFlags, name="HECWheelCalculator", **kwargs):
    result = ComponentAccumulator()
    kwargs.setdefault("GeometryCalculator", result.getPrimaryAndMerge(HECGeometryCfg(ConfigFlags)).name)
    result.addService(LArHECWheelCalculator(name, **kwargs), primary = True)
    return result


def LArHECCalibrationWheelCalculatorCfg(ConfigFlags, name="LArHECCalibrationWheelCalculator", **kwargs):
    result = ComponentAccumulator()
    result.addService( LArG4__HEC__LArHECCalibrationWheelCalculator(name, **kwargs), primary = True )
    return result


def HECCalibrationWheelActiveCalculatorCfg(ConfigFlags, name="HECCalibrationWheelActiveCalculator", **kwargs):
    kwargs.setdefault("GeometryType", HECGeometryType.kWheelActive)
    return LArHECCalibrationWheelCalculatorCfg(ConfigFlags, name, **kwargs)


def HECCalibrationWheelInactiveCalculatorCfg(ConfigFlags, name="HECCalibrationWheelInactiveCalculator", **kwargs):
    kwargs.setdefault("GeometryType", HECGeometryType.kWheelInactive)
    return LArHECCalibrationWheelCalculatorCfg(ConfigFlags, name, **kwargs)


def HECCalibrationWheelDeadCalculatorCfg(ConfigFlags, name="HECCalibrationWheelDeadCalculator", **kwargs):
    kwargs.setdefault("GeometryType", HECGeometryType.kWheelDead)
    return LArHECCalibrationWheelCalculatorCfg(ConfigFlags, name, **kwargs)


def HECGeometryCfg(ConfigFlags, name="HECGeometry", **kwargs):
    result = ComponentAccumulator()
    result.addService(LArG4__HEC__HECGeometry(name, **kwargs), primary = True)
    return result
