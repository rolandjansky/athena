# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

from AthenaCommon import CfgMgr

def getMiniFCALCalibrationCalculator(name="MiniFCALCalibrationCalculator", **kwargs):
    return CfgMgr.LArG4__MiniFCAL__MiniFCALCalibrationCalculator(name, **kwargs)

def getMiniFCALActiveCalibrationCalculator(name="MiniFCALActiveCalibrationCalculator", **kwargs):
    kwargs.setdefault("GeometryType", "ACTIVE")
    return getMiniFCALCalibrationCalculator(name, **kwargs)

def getMiniFCALInactiveCalibrationCalculator(name="MiniFCALInactiveCalibrationCalculator", **kwargs):
    kwargs.setdefault("GeometryType", "INACTIVE")
    return getMiniFCALCalibrationCalculator(name, **kwargs)

def getMiniFCALDeadCalibrationCalculator(name="MiniFCALDeadCalibrationCalculator", **kwargs):
    kwargs.setdefault("GeometryType", "DEAD")
    return getMiniFCALCalibrationCalculator(name, **kwargs)

def getMiniFCALCalculator(name="MiniFCALCalculator", **kwargs):
    return CfgMgr.MiniFCALCalculator(name, **kwargs)

