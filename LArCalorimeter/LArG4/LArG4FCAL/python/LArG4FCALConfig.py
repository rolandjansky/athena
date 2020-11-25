# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

from AthenaCommon import CfgMgr
from AthenaCommon.SystemOfUnits import mm,deg,ns
import math

def getFCALCalculatorBase(name="FCALCalculatorBase", **kwargs):
    kwargs.setdefault("OOTcut",2.5*ns)
    return CfgMgr.LArFCALCalculatorBase(name, **kwargs)


def getFCAL1Calculator(name="FCAL1Calculator", **kwargs):
    kwargs.setdefault("FCALSampling",1)
    return getFCALCalculatorBase(name, **kwargs)


def getFCAL2Calculator(name="FCAL2Calculator", **kwargs):
    kwargs.setdefault("FCALSampling",2)
    return getFCALCalculatorBase(name, **kwargs)


def getFCAL3Calculator(name="FCAL3Calculator", **kwargs):
    kwargs.setdefault("FCALSampling",3)
    return getFCALCalculatorBase(name, **kwargs)


def getFCAL1CalibCalculator(name="FCAL1CalibCalculator", **kwargs):
    kwargs.setdefault("FCALdeltaX",7.5*mm)
    kwargs.setdefault("FCALdeltaY",7.5*mm*math.sin(60*deg))
    kwargs.setdefault("FCALSampling",1)
    return CfgMgr.LArG4__FCAL__LArFCALCalibCalculatorBase(name, **kwargs)


def getFCAL2CalibCalculator(name="FCAL2CalibCalculator", **kwargs):
    kwargs.setdefault("FCALdeltaX",8.179*mm)
    kwargs.setdefault("FCALdeltaY",8.179*mm*math.sin(60*deg))
    kwargs.setdefault("FCALSampling",2)
    return CfgMgr.LArG4__FCAL__LArFCALCalibCalculatorBase(name, **kwargs)


def getFCAL3CalibCalculator(name="FCAL3CalibCalculator", **kwargs):
    kwargs.setdefault("FCALdeltaX",9.0*mm)
    kwargs.setdefault("FCALdeltaY",9.0*mm*math.sin(60*deg))
    kwargs.setdefault("FCALSampling",3)
    return CfgMgr.LArG4__FCAL__LArFCALCalibCalculatorBase(name, **kwargs)

