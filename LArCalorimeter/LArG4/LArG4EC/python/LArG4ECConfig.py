# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

from AthenaCommon import CfgMgr
from LArG4EC import LArWheelCalculatorEnum
def getCalibrationCalculator(name="CalibrationCalculator", **kwargs):
    return CfgMgr.LArG4__EC__CalibrationCalculator(name, **kwargs)

def getEMECPosInnerWheelCalibrationCalculator(name="EMECPosInnerWheelCalibrationCalculator", **kwargs):
    kwargs.setdefault("WheelType", LArWheelCalculatorEnum.InnerAbsorberWheel)
    kwargs.setdefault("zSide", 1)
    kwargs.setdefault("GeometryCalculator","EMECPosInnerWheelCorrOffCalculator")
    return getCalibrationCalculator(name, **kwargs)

def getEMECNegInnerWheelCalibrationCalculator(name="EMECNegInnerWheelCalibrationCalculator", **kwargs):
    kwargs.setdefault("WheelType", LArWheelCalculatorEnum.InnerAbsorberWheel)
    kwargs.setdefault("zSide", -1)
    kwargs.setdefault("GeometryCalculator","EMECNegInnerWheelCorrOffCalculator")
    return getCalibrationCalculator(name, **kwargs)

def getEMECPosOuterWheelCalibrationCalculator(name="EMECPosOuterWheelCalibrationCalculator", **kwargs):
    kwargs.setdefault("WheelType", LArWheelCalculatorEnum.OuterAbsorberWheel)
    kwargs.setdefault("zSide", 1)
    kwargs.setdefault("GeometryCalculator","EMECPosOuterWheelCorrOffCalculator")
    return getCalibrationCalculator(name, **kwargs)

def getEMECNegOuterWheelCalibrationCalculator(name="EMECNegOuterWheelCalibrationCalculator", **kwargs):
    kwargs.setdefault("WheelType", LArWheelCalculatorEnum.OuterAbsorberWheel)
    kwargs.setdefault("zSide", -1)
    kwargs.setdefault("GeometryCalculator","EMECNegOuterWheelCorrOffCalculator")
    return getCalibrationCalculator(name, **kwargs)

def getEMECBackOuterBarretteCalibrationCalculator(name="EMECBackOuterBarretteCalibrationCalculator", **kwargs):
    kwargs.setdefault("WheelType", LArWheelCalculatorEnum.BackOuterBarretteWheelCalib)
    kwargs.setdefault("zSide", 1)
    kwargs.setdefault("GeometryCalculator","EMECBackOuterBarretteCorrOffCalculator")
    return getCalibrationCalculator(name, **kwargs)

def getEMECPresamplerCalibrationCalculator(name="EMECPresamplerCalibrationCalculator", **kwargs):
    return CfgMgr.LArG4__EC__PresamplerCalibrationCalculator(name, **kwargs)

def getEndcapCryostatCalibrationCalculator(name="EndcapCryostatCalibrationCalculator", **kwargs):
    return CfgMgr.LArG4__EndcapCryostat__CalibrationCalculator(name, **kwargs)

def getEndcapCryostatCalibrationLArCalculator(name="EndcapCryostatCalibrationLArCalculator", **kwargs):
    return CfgMgr.LArG4__EndcapCryostat__CalibrationLArCalculator(name, **kwargs)

def getEndcapCryostatCalibrationMixedCalculator(name="EndcapCryostatCalibrationMixedCalculator", **kwargs):
    return CfgMgr.LArG4__EndcapCryostat__CalibrationMixedCalculator(name, **kwargs)

def getEMECSupportCalibrationCalculator(name="EMECSupportCalibrationCalculator", **kwargs):
    return CfgMgr.LArG4__EMECSupportCalibrationCalculator(name, **kwargs)

def getEnergyCalculator(name="EnergyCalculator", **kwargs):
    from AthenaCommon.SystemOfUnits import ns
    kwargs.setdefault("OOTcut", 300.0*ns)
    return CfgMgr.LArG4__EC__EnergyCalculator(name, **kwargs)

def getEMECPosInnerWheelCalculator(name="EMECPosInnerWheelCalculator", **kwargs):
    kwargs.setdefault("WheelType", LArWheelCalculatorEnum.InnerAbsorberWheel)
    #kwargs.setdefault("EnergyCorrection", 8) #LArG4::EMEC_ECOR_CHCL1
    kwargs.setdefault("zSide", 1)
    return getEnergyCalculator(name, **kwargs)

def getEMECNegInnerWheelCalculator(name="EMECNegInnerWheelCalculator", **kwargs):
    kwargs.setdefault("WheelType", LArWheelCalculatorEnum.InnerAbsorberWheel)
    #kwargs.setdefault("EnergyCorrection", 8) #LArG4::EMEC_ECOR_CHCL1
    kwargs.setdefault("zSide", -1)
    return getEnergyCalculator(name, **kwargs)

def getEMECPosOuterWheelCalculator(name="EMECPosOuterWheelCalculator", **kwargs):
    kwargs.setdefault("WheelType", LArWheelCalculatorEnum.OuterAbsorberWheel)
    #kwargs.setdefault("EnergyCorrection", 8) #LArG4::EMEC_ECOR_CHCL1
    kwargs.setdefault("zSide", 1)
    return getEnergyCalculator(name, **kwargs)

def getEMECNegOuterWheelCalculator(name="EMECNegOuterWheelCalculator", **kwargs):
    kwargs.setdefault("WheelType", LArWheelCalculatorEnum.OuterAbsorberWheel)
    #kwargs.setdefault("EnergyCorrection", 8) #LArG4::EMEC_ECOR_CHCL1
    kwargs.setdefault("zSide", -1)
    return getEnergyCalculator(name, **kwargs)

def getEMECBackOuterBarretteCalculator(name="EMECBackOuterBarretteCalculator", **kwargs):
    kwargs.setdefault("WheelType", LArWheelCalculatorEnum.BackOuterBarretteWheelCalib)
    #kwargs.setdefault("EnergyCorrection", 8) #LArG4::EMEC_ECOR_CHCL1
    return getEnergyCalculator(name, **kwargs)

def getEMECPosInnerWheelCorrOffCalculator(name="EMECPosInnerWheelCorrOffCalculator", **kwargs):
    kwargs.setdefault("EnergyCorrection", 1) #LArG4::EMEC_ECOR_OFF
    return getEMECPosInnerWheelCalculator(name, **kwargs)

def getEMECNegInnerWheelCorrOffCalculator(name="EMECNegInnerWheelCorrOffCalculator", **kwargs):
    kwargs.setdefault("EnergyCorrection", 1) #LArG4::EMEC_ECOR_OFF
    return getEMECNegInnerWheelCalculator(name, **kwargs)

def getEMECPosOuterWheelCorrOffCalculator(name="EMECPosOuterWheelCorrOffCalculator", **kwargs):
    kwargs.setdefault("EnergyCorrection", 1) #LArG4::EMEC_ECOR_OFF
    return getEMECPosOuterWheelCalculator(name, **kwargs)

def getEMECNegOuterWheelCorrOffCalculator(name="EMECNegOuterWheelCorrOffCalculator", **kwargs):
    kwargs.setdefault("EnergyCorrection", 1) #LArG4::EMEC_ECOR_OFF
    return getEMECNegOuterWheelCalculator(name, **kwargs)

def getEMECBackOuterBarretteCorrOffCalculator(name="EMECBackOuterBarretteCorrOffCalculator", **kwargs):
    kwargs.setdefault("EnergyCorrection", 1) #LArG4::EMEC_ECOR_OFF
    return getEMECBackOuterBarretteCalculator(name, **kwargs)

def getEMECPresamplerCalculator(name="EMECPresamplerCalculator", **kwargs):
    return CfgMgr.LArEndcapPresamplerCalculator(name, **kwargs)

def getEMECPresamplerGeometry(name="EMECPresamplerGeometry", **kwargs):
    return CfgMgr.LArG4__EC__PresamplerGeometry(name, **kwargs)

