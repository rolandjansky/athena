# Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration

from AthenaConfiguration.ComponentAccumulator import ComponentAccumulator
from AthenaConfiguration.ComponentFactory import CompFactory
LArFCALCalculatorBase=CompFactory.LArFCALCalculatorBase
LArG4__FCAL__LArFCALCalibCalculatorBase=CompFactory.LArG4.FCAL.LArFCALCalibCalculatorBase
from AthenaCommon.SystemOfUnits import mm,deg,ns
import math

def FCALCalculatorBaseCfg(name="FCALCalculatorBase", **kwargs):
    result = ComponentAccumulator()
    kwargs.setdefault("OOTcut",2.5*ns)
    result.addService(LArFCALCalculatorBase(name, **kwargs), primary = True)
    return result


def FCAL1CalculatorCfg(ConfigFlags, name="FCAL1Calculator", **kwargs):
    kwargs.setdefault("FCALSampling",1)
    return FCALCalculatorBaseCfg(name, **kwargs)


def FCAL2CalculatorCfg(ConfigFlags, name="FCAL2Calculator", **kwargs):
    kwargs.setdefault("FCALSampling",2)
    return FCALCalculatorBaseCfg(name, **kwargs)


def FCAL3CalculatorCfg(ConfigFlags, name="FCAL3Calculator", **kwargs):
    kwargs.setdefault("FCALSampling",3)
    return FCALCalculatorBaseCfg(name, **kwargs)


def FCAL1CalibCalculatorCfg(ConfigFlags, name="FCAL1CalibCalculator", **kwargs):
    result = ComponentAccumulator()
    kwargs.setdefault("FCALdeltaX",7.5*mm)
    kwargs.setdefault("FCALdeltaY",7.5*mm*math.sin(60*deg))
    kwargs.setdefault("FCALSampling",1)
    result.addService(LArG4__FCAL__LArFCALCalibCalculatorBase(name, **kwargs), primary = True)
    return result


def FCAL2CalibCalculatorCfg(ConfigFlags, name="FCAL2CalibCalculator", **kwargs):
    result = ComponentAccumulator()
    kwargs.setdefault("FCALdeltaX",8.179*mm)
    kwargs.setdefault("FCALdeltaY",8.179*mm*math.sin(60*deg))
    kwargs.setdefault("FCALSampling",2)
    result.addService(LArG4__FCAL__LArFCALCalibCalculatorBase(name, **kwargs), primary = True)
    return result


def FCAL3CalibCalculatorCfg(ConfigFlags, name="FCAL3CalibCalculator", **kwargs):
    result = ComponentAccumulator()
    kwargs.setdefault("FCALdeltaX",9.0*mm)
    kwargs.setdefault("FCALdeltaY",9.0*mm*math.sin(60*deg))
    kwargs.setdefault("FCALSampling",3)
    result.addService(LArG4__FCAL__LArFCALCalibCalculatorBase(name, **kwargs), primary = True)
    return result
