# Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
from AthenaConfiguration.ComponentAccumulator import ComponentAccumulator
from AthenaConfiguration.ComponentFactory import CompFactory
MiniFCALCalculator=CompFactory.MiniFCALCalculator
LArG4__MiniFCAL__MiniFCALCalibrationCalculator=CompFactory.LArG4.MiniFCAL.MiniFCALCalibrationCalculator


def MiniFCALCalibrationCalculatorCfg(ConfigFlags, name="MiniFCALCalibrationCalculator", **kwargs):
    result = ComponentAccumulator()
    result.addService(LArG4__MiniFCAL__MiniFCALCalibrationCalculator(name, **kwargs), primary = True)
    return result

def MiniFCALActiveCalibrationCalculatorCfg(ConfigFlags, name="MiniFCALActiveCalibrationCalculator", **kwargs):
    kwargs.setdefault("GeometryType", "ACTIVE")
    return MiniFCALCalibrationCalculatorCfg(ConfigFlags, name, **kwargs)

def MiniFCALInactiveCalibrationCalculatorCfg(ConfigFlags, name="MiniFCALInactiveCalibrationCalculator", **kwargs):
    kwargs.setdefault("GeometryType", "INACTIVE")
    return MiniFCALCalibrationCalculatorCfg(ConfigFlags, name, **kwargs)

def MiniFCALDeadCalibrationCalculatorCfg(ConfigFlags, name="MiniFCALDeadCalibrationCalculator", **kwargs):
    kwargs.setdefault("GeometryType", "DEAD")
    return MiniFCALCalibrationCalculatorCfg(ConfigFlags, name, **kwargs)

def MiniFCALCalculatorCfg(ConfigFlags, name="MiniFCALCalculator", **kwargs):
    return MiniFCALCalculator(name, **kwargs)

