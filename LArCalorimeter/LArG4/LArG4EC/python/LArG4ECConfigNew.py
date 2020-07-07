# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

from AthenaConfiguration.ComponentAccumulator import ComponentAccumulator
from AthenaConfiguration.ComponentFactory import CompFactory
from LArG4EC import LArWheelCalculatorEnum
from LArG4SD.LArG4SDToolConfig import CalibrationDefaultCalculatorCfg
def CalibrationCalculatorCfg(name="CalibrationCalculator", **kwargs):
    result = ComponentAccumulator()
    result.addService(CompFactory.LArG4.EC.CalibrationCalculator(name, **kwargs))
    return result

def EMECPosInnerWheelCalibrationCalculatorCfg(ConfigFlags, name="EMECPosInnerWheelCalibrationCalculator", **kwargs):
    kwargs.setdefault("WheelType", LArWheelCalculatorEnum.InnerAbsorberWheel)
    kwargs.setdefault("zSide", 1)
    kwargs.setdefault("GeometryCalculator","EMECPosInnerWheelCorrOffCalculator")
    return CalibrationCalculatorCfg(name, **kwargs)

def EMECNegInnerWheelCalibrationCalculatorCfg(ConfigFlags, name="EMECNegInnerWheelCalibrationCalculator", **kwargs):
    kwargs.setdefault("WheelType", LArWheelCalculatorEnum.InnerAbsorberWheel)
    kwargs.setdefault("zSide", -1)
    kwargs.setdefault("GeometryCalculator","EMECNegInnerWheelCorrOffCalculator")
    return CalibrationCalculatorCfg(name, **kwargs)

def EMECPosOuterWheelCalibrationCalculatorCfg(ConfigFlags, name="EMECPosOuterWheelCalibrationCalculator", **kwargs):
    kwargs.setdefault("WheelType", LArWheelCalculatorEnum.OuterAbsorberWheel)
    kwargs.setdefault("zSide", 1)
    kwargs.setdefault("GeometryCalculator","EMECPosOuterWheelCorrOffCalculator")
    return CalibrationCalculatorCfg(name, **kwargs)

def EMECNegOuterWheelCalibrationCalculatorCfg(ConfigFlags, name="EMECNegOuterWheelCalibrationCalculator", **kwargs):
    kwargs.setdefault("WheelType", LArWheelCalculatorEnum.OuterAbsorberWheel)
    kwargs.setdefault("zSide", -1)
    kwargs.setdefault("GeometryCalculator","EMECNegOuterWheelCorrOffCalculator")
    return CalibrationCalculatorCfg(name, **kwargs)

def EMECPosBackOuterBarretteCalibrationCalculatorCfg(ConfigFlags, name="EMECPosBackOuterBarretteCalibrationCalculator", **kwargs):
    kwargs.setdefault("WheelType", LArWheelCalculatorEnum.BackOuterBarretteWheelCalib)
    kwargs.setdefault("zSide", 1)
    kwargs.setdefault("GeometryCalculator","EMECPosBackOuterBarretteCorrOffCalculator")
    return CalibrationCalculatorCfg(name, **kwargs)

def EMECNegBackOuterBarretteCalibrationCalculatorCfg(ConfigFlags, name="EMECNegBackOuterBarretteCalibrationCalculator", **kwargs):
    kwargs.setdefault("WheelType", LArWheelCalculatorEnum.BackOuterBarretteWheelCalib)
    kwargs.setdefault("zSide", -1)
    kwargs.setdefault("GeometryCalculator","EMECNegBackOuterBarretteCorrOffCalculator")
    return CalibrationCalculatorCfg(name, **kwargs)

def EMECPresamplerCalibrationCalculatorCfg(ConfigFlags, name="EMECPresamplerCalibrationCalculator", **kwargs):
    return CompFactory.LArG4.EC.PresamplerCalibrationCalculator(name, **kwargs)

def EndcapCryostatCalibrationCalculatorCfg(ConfigFlags, name="EndcapCryostatCalibrationCalculator", **kwargs):
    result = ComponentAccumulator()
    result.addService( CompFactory.LArG4.EndcapCryostat.CalibrationCalculator(name, **kwargs) )
    return result

def EndcapCryostatCalibrationLArCalculatorCfg(ConfigFlags, name="EndcapCryostatCalibrationLArCalculator", **kwargs):
    result = CalibrationDefaultCalculatorCfg(ConfigFlags)
    kwargs.setdefault("CalibrationDefaultCalculator", result.getService("CalibrationDefaultCalculator"))
    result.addService( CompFactory.LArG4.EndcapCryostat.CalibrationLArCalculator(name, **kwargs) )
    return result

def EndcapCryostatCalibrationMixedCalculatorCfg(ConfigFlags, name="EndcapCryostatCalibrationMixedCalculator", **kwargs):
    result = ComponentAccumulator()
    result.addService( CompFactory.LArG4.EndcapCryostat.CalibrationMixedCalculator(name, **kwargs) )
    return result

def EMECSupportCalibrationCalculatorCfg(ConfigFlags, name="EMECSupportCalibrationCalculator", **kwargs):
    result = EndcapCryostatCalibrationLArCalculatorCfg(ConfigFlags)
    kwargs.setdefault("BackupCalculator", result.getService("EndcapCryostatCalibrationLArCalculator"))
    result.addService( CompFactory.LArG4.EMECSupportCalibrationCalculator(name, **kwargs))
    return result

def EnergyCalculatorCfg(ConfigFlags, name="EnergyCalculator", **kwargs):
    result = EMECSupportCalibrationCalculatorCfg(ConfigFlags)
    kwargs.setdefault("SupportCalculator", result.getService("EMECSupportCalibrationCalculator"))
    from AthenaCommon.SystemOfUnits import ns
    kwargs.setdefault("OOTcut", 300.0*ns)

    result.addService(CompFactory.LArG4.EC.EnergyCalculator(name, **kwargs))
    return result

def EMECPosInnerWheelCalculatorCfg(ConfigFlags, name="EMECPosInnerWheelCalculator", **kwargs):
    kwargs.setdefault("WheelType", LArWheelCalculatorEnum.InnerAbsorberWheel)
    #kwargs.setdefault("EnergyCorrection", 8) #LArG4::EMEC_ECOR_CHCL1
    kwargs.setdefault("zSide", 1)
    return EnergyCalculatorCfg(ConfigFlags, name, **kwargs)

def EMECNegInnerWheelCalculatorCfg(ConfigFlags, name="EMECNegInnerWheelCalculator", **kwargs):
    kwargs.setdefault("WheelType", LArWheelCalculatorEnum.InnerAbsorberWheel)
    #kwargs.setdefault("EnergyCorrection", 8) #LArG4::EMEC_ECOR_CHCL1
    kwargs.setdefault("zSide", -1)
    return EnergyCalculatorCfg(ConfigFlags, name, **kwargs)

def EMECPosOuterWheelCalculatorCfg(ConfigFlags, name="EMECPosOuterWheelCalculator", **kwargs):
    kwargs.setdefault("WheelType", LArWheelCalculatorEnum.OuterAbsorberWheel)
    #kwargs.setdefault("EnergyCorrection", 8) #LArG4::EMEC_ECOR_CHCL1
    kwargs.setdefault("zSide", 1)
    return EnergyCalculatorCfg(ConfigFlags, name, **kwargs)

def EMECNegOuterWheelCalculatorCfg(ConfigFlags, name="EMECNegOuterWheelCalculator", **kwargs):
    kwargs.setdefault("WheelType", LArWheelCalculatorEnum.OuterAbsorberWheel)
    #kwargs.setdefault("EnergyCorrection", 8) #LArG4::EMEC_ECOR_CHCL1
    kwargs.setdefault("zSide", -1)
    return EnergyCalculatorCfg(ConfigFlags, name, **kwargs)

def EMECPosBackOuterBarretteCalculatorCfg(ConfigFlags, name="EMECPosBackOuterBarretteCalculator", **kwargs):
    kwargs.setdefault("WheelType", LArWheelCalculatorEnum.BackOuterBarretteWheel)
    #kwargs.setdefault("EnergyCorrection", 8) #LArG4::EMEC_ECOR_CHCL1
    return EnergyCalculatorCfg(ConfigFlags, name, **kwargs)

def EMECNegBackOuterBarretteCalculatorCfg(ConfigFlags, name="EMECNegBackOuterBarretteCalculator", **kwargs):
    kwargs.setdefault("WheelType", LArWheelCalculatorEnum.BackOuterBarretteWheel)
    #kwargs.setdefault("EnergyCorrection", 8) #LArG4::EMEC_ECOR_CHCL1
    kwargs.setdefault("zSide", -1)
    return EnergyCalculatorCfg(ConfigFlags, name, **kwargs)

def EMECPosInnerWheelCorrOffCalculatorCfg(ConfigFlags, name="EMECPosInnerWheelCorrOffCalculator", **kwargs):
    kwargs.setdefault("EnergyCorrection", 1) #LArG4::EMEC_ECOR_OFF
    return EMECPosInnerWheelCalculatorCfg(ConfigFlags, name, **kwargs)

def EMECNegInnerWheelCorrOffCalculatorCfg(ConfigFlags, name="EMECNegInnerWheelCorrOffCalculator", **kwargs):
    kwargs.setdefault("EnergyCorrection", 1) #LArG4::EMEC_ECOR_OFF
    return EMECNegInnerWheelCalculatorCfg(ConfigFlags, name, **kwargs)

def EMECPosOuterWheelCorrOffCalculatorCfg(ConfigFlags, name="EMECPosOuterWheelCorrOffCalculator", **kwargs):
    kwargs.setdefault("EnergyCorrection", 1) #LArG4::EMEC_ECOR_OFF
    return EMECPosOuterWheelCalculatorCfg(ConfigFlags, name, **kwargs)

def EMECNegOuterWheelCorrOffCalculatorCfg(ConfigFlags, name="EMECNegOuterWheelCorrOffCalculator", **kwargs):
    kwargs.setdefault("EnergyCorrection", 1) #LArG4::EMEC_ECOR_OFF
    return EMECNegOuterWheelCalculatorCfg(ConfigFlags, name, **kwargs)

def EMECPosBackOuterBarretteCorrOffCalculatorCfg(ConfigFlags, name="EMECPosBackOuterBarretteCorrOffCalculator", **kwargs):
    kwargs.setdefault("EnergyCorrection", 1) #LArG4::EMEC_ECOR_OFF
    return EMECPosBackOuterBarretteCalculatorCfg(ConfigFlags, name, **kwargs)

def EMECNegBackOuterBarretteCorrOffCalculatorCfg(ConfigFlags, name="EMECNegBackOuterBarretteCorrOffCalculator", **kwargs):
    kwargs.setdefault("EnergyCorrection", 1) #LArG4::EMEC_ECOR_OFF
    return EMECNegBackOuterBarretteCalculatorCfg(ConfigFlags, name, **kwargs)

def EMECPresamplerCalculatorCfg(ConfigFlags, name="EMECPresamplerCalculator", **kwargs):
    result = ComponentAccumulator()

    result.addService(EMECPresamplerGeometryCfg(ConfigFlags))
    kwargs.setdefault("GeometryCalculator",result.getService("EMECPresamplerGeometry"))
    result.addService(CompFactory.LArEndcapPresamplerCalculator(name, **kwargs))
    return result

def EMECPresamplerGeometryCfg(ConfigFlags, name="EMECPresamplerGeometry", **kwargs):
    return CompFactory.LArG4.EC.PresamplerGeometry(name, **kwargs)
