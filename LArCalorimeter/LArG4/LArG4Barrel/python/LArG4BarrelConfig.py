# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

from AthenaCommon import CfgMgr

def getBarrelCryostatCalibrationCalculator(name="BarrelCryostatCalibrationCalculator", **kwargs):
    return CfgMgr.LArG4__BarrelCryostat__CalibrationCalculator(name, **kwargs)


def getBarrelCryostatCalibrationLArCalculator(name="BarrelCryostatCalibrationLArCalculator", **kwargs):
    return CfgMgr.LArG4__BarrelCryostat__CalibrationLArCalculator(name, **kwargs)


def getBarrelCryostatCalibrationMixedCalculator(name="BarrelCryostatCalibrationMixedCalculator", **kwargs):
    return CfgMgr.LArG4__BarrelCryostat__CalibrationMixedCalculator(name, **kwargs)


def getDMCalibrationCalculator(name="DMCalibrationCalculator", **kwargs):
    return CfgMgr.LArG4__DM__CalibrationCalculator(name, **kwargs)


def getBarrelCalibrationCalculator(name="BarrelCalibrationCalculator", **kwargs):
    return CfgMgr.LArG4__Barrel__CalibrationCalculator(name, **kwargs)


def getBarrelPresamplerCalibrationCalculator(name="BarrelPresamplerCalibrationCalculator", **kwargs):
    return CfgMgr.LArG4__BarrelPresampler__CalibrationCalculator(name, **kwargs)


def getEMBCalculator(name="EMBCalculator", **kwargs):
    return CfgMgr.LArBarrelCalculator(name, **kwargs)


def getEMBPresamplerCalculator(name="EMBPresamplerCalculator", **kwargs):
    return CfgMgr.LArBarrelPresamplerCalculator(name, **kwargs)


def getLArBarrelGeometry(name="LArBarrelGeometry", **kwargs):
    return CfgMgr.LArG4__Barrel__Geometry(name, **kwargs)


def getLArBarrelPresamplerGeometry(name="LArBarrelPresamplerGeometry", **kwargs):
    return CfgMgr.LArG4__BarrelPresampler__Geometry(name, **kwargs)
