# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

from AthenaCommon import CfgMgr
from LArG4HEC import HECGeometryType

def getLArHECLocalCalculator(name="LArHECLocalCalculator", **kwargs):
    return CfgMgr.LArHECLocalCalculator(name, **kwargs)


def getLocalCalibrationCalculator(name="LocalCalibrationCalculator", **kwargs):
    return CfgMgr.LArG4__HEC__LocalCalibrationCalculator(name, **kwargs)


def getLocalHECGeometry(name="LocalHECGeometry", **kwargs):
    return CfgMgr.LArG4__HEC__LocalGeometry(name, **kwargs)


def getHECWheelCalculator(name="HECWheelCalculator", **kwargs):
    return CfgMgr.LArHECWheelCalculator(name, **kwargs)


def getLArHECCalibrationWheelCalculator(name="LArHECCalibrationWheelCalculator", **kwargs):
    return CfgMgr.LArG4__HEC__LArHECCalibrationWheelCalculator(name, **kwargs)


def getHECCalibrationWheelActiveCalculator(name="HECCalibrationWheelActiveCalculator", **kwargs):
    kwargs.setdefault("GeometryType", HECGeometryType.kWheelActive)
    return getLArHECCalibrationWheelCalculator(name, **kwargs)


def getHECCalibrationWheelInactiveCalculator(name="HECCalibrationWheelInactiveCalculator", **kwargs):
    kwargs.setdefault("GeometryType", HECGeometryType.kWheelInactive)
    return getLArHECCalibrationWheelCalculator(name, **kwargs)


def getHECCalibrationWheelDeadCalculator(name="HECCalibrationWheelDeadCalculator", **kwargs):
    kwargs.setdefault("GeometryType", HECGeometryType.kWheelDead)
    return getLArHECCalibrationWheelCalculator(name, **kwargs)


def getHECGeometry(name="HECGeometry", **kwargs):
    return CfgMgr.LArG4__HEC__HECGeometry(name, **kwargs)

