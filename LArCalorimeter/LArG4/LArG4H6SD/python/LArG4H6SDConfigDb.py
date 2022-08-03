# Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration

from AthenaCommon.CfgGetter import addTool, addService
addTool("LArG4H6SD.LArG4H6SDTools.LArGeoH62004EventAction", "LArGeoH62004EventAction")
addTool("LArG4H6SD.LArG4H6SDTools.LArGeoH62004SteppingAction", "LArGeoH62004SteppingAction")
addTool("LArG4H6SD.LArG4H6SDTools.LArHitsH6EventAction", "LArHitsH6EventAction")
addTool("LArG4H6SD.LArG4H6SDTools.LArTBH6BeamInfo", "LArTBH6BeamInfo")
addTool("LArG4H6SD.LArG4H6SDTools.LArTBH6TriggerTimeTool", "LArTBH6TriggerTimeTool")
addTool("LArG4H6SD.LArG4H6SDTools.RadLenNtuple", "RadLenNtuple")

# Sensitive detector tools
addTool("LArG4H6SD.LArG4H6SDConfigLegacy.getLArG4TBECBeamChSDTool", "LArG4TBECBeamChSensitiveDetector")
addTool("LArG4H6SD.LArG4H6SDConfigLegacy.getFrontLArG4H6BeamSDTool", "LArG4H6BeamSD::Front")
addTool("LArG4H6SD.LArG4H6SDConfigLegacy.getMovableLArG4H6BeamSDTool", "LArG4H6BeamSD::Movable")

addTool("LArG4H6SD.LArG4H6SDConfigLegacy.getLArG4H6WarmTC_X", "LArG4H6WarmTC::X")
addTool("LArG4H6SD.LArG4H6SDConfigLegacy.getLArG4H6WarmTC_Y", "LArG4H6WarmTC::Y")
addTool("LArG4H6SD.LArG4H6SDConfigLegacy.getLArG4H6WarmTC_Abs", "LArG4H6WarmTC::Abs")

addTool("LArG4H6SD.LArG4H6SDConfigLegacy.getTBECInnerModuleTool", "TBECInnerModuleMult")
addTool("LArG4H6SD.LArG4H6SDConfigLegacy.getTBECOuterModuleTool", "TBECOuterModuleMult")
addTool("LArG4H6SD.LArG4H6SDConfigLegacy.getTBECBackOuterBarretteTool", "TBECBackOuterBarretteMult")

addTool("LArG4H6SD.LArG4H6SDConfigLegacy.getLArG4H62004HECSDTool", "LArG4H62004HECSDTool")
addTool("LArG4H6SD.LArG4H6SDConfigLegacy.getLArG4H62004EMECSDTool", "LArG4H62004EMECSDTool")
addTool("LArG4H6SD.LArG4H6SDConfigLegacy.getLArG4H62004FCALSDTool", "LArG4H62004FCALSDTool")

addTool("LArG4H6SD.LArG4H6SDConfigLegacy.getLArG4H62004ActiveSDTool", "LArG4H62004ActiveSDTool")
addTool("LArG4H6SD.LArG4H6SDConfigLegacy.getLArG4H62004DeadSDTool", "LArG4H62004DeadSDTool")
addTool("LArG4H6SD.LArG4H6SDConfigLegacy.getLArG4H62004InactiveSDTool", "LArG4H62004InactiveSDTool")

# Calculators
addService("LArG4H6SD.LArG4H6SDConfigLegacy.getLArG4H6WarmTCCalculatorWTC_X", "LArG4H6WarmTCCalculatorWTC_X")
addService("LArG4H6SD.LArG4H6SDConfigLegacy.getLArG4H6WarmTCCalculatorWTC_Y", "LArG4H6WarmTCCalculatorWTC_Y")
addService("LArG4H6SD.LArG4H6SDConfigLegacy.getLArG4H6WarmTCCalculatorWTC_ABS", "LArG4H6WarmTCCalculatorWTC_ABS")
addService("LArG4H6SD.LArG4H6SDConfigLegacy.getLArG4H6COLDTCMod0Calculator", "LArG4H6COLDTCMod0Calculator")
addService("LArG4H6SD.LArG4H6SDConfigLegacy.getFCALColdCalculator", "FCALColdCalculator")

addService("LArG4H6SD.LArG4H6SDConfigLegacy.getLArG4H6COLDTCMod0CalibCalculator", "LArG4H6COLDTCMod0CalibCalculator")
addService("LArG4H6SD.LArG4H6SDConfigLegacy.getLArG4H62004DeadCalibrationCalculator", "LArG4H62004DeadCalibrationCalculator")
addService("LArG4H6SD.LArG4H6SDConfigLegacy.getLArFCAL1H62004CalibCalculator", "LArFCAL1H62004CalibCalculator")
addService("LArG4H6SD.LArG4H6SDConfigLegacy.getLArFCAL2H62004CalibCalculator", "LArFCAL2H62004CalibCalculator")

# Calculators from other packages
addService("LArG4H6SD.LArG4H6SDConfigLegacy.getEMECPosOuterWheel_ECOR_GADJCalculator", "EMECPosOuterWheel_ECOR_GADJCalculator")
addService("LArG4H6SD.LArG4H6SDConfigLegacy.getEMECPosOuterWheel_ECOR_GADJ_OLDCalculator", "EMECPosOuterWheel_ECOR_GADJ_OLDCalculator")
addService("LArG4H6SD.LArG4H6SDConfigLegacy.getEMECPosOuterWheel_ECOR_GADJ_ECalculator", "EMECPosOuterWheel_ECOR_GADJ_ECalculator")
addService("LArG4H6SD.LArG4H6SDConfigLegacy.getEMECPosOuterWheel_ECOR_GADJ_SCalculator", "EMECPosOuterWheel_ECOR_GADJ_SCalculator")
addService("LArG4H6SD.LArG4H6SDConfigLegacy.getEMECPosOuterWheel_ECOR_GADJ_SECalculator", "EMECPosOuterWheel_ECOR_GADJ_SECalculator")
addService("LArG4H6SD.LArG4H6SDConfigLegacy.getEMECPosOuterWheel_ECOR_CHCLCalculator", "EMECPosOuterWheel_ECOR_CHCLCalculator")

addService("LArG4H6SD.LArG4H6SDConfigLegacy.getEMECPosInnerWheel_ECOR_GADJCalculator", "EMECPosInnerWheel_ECOR_GADJCalculator")
addService("LArG4H6SD.LArG4H6SDConfigLegacy.getEMECPosInnerWheel_ECOR_GADJ_OLDCalculator", "EMECPosInnerWheel_ECOR_GADJ_OLDCalculator")
addService("LArG4H6SD.LArG4H6SDConfigLegacy.getEMECPosInnerWheel_ECOR_GADJ_ECalculator", "EMECPosInnerWheel_ECOR_GADJ_ECalculator")
addService("LArG4H6SD.LArG4H6SDConfigLegacy.getEMECPosInnerWheel_ECOR_GADJ_SCalculator", "EMECPosInnerWheel_ECOR_GADJ_SCalculator")
addService("LArG4H6SD.LArG4H6SDConfigLegacy.getEMECPosInnerWheel_ECOR_GADJ_SECalculator", "EMECPosInnerWheel_ECOR_GADJ_SECalculator")
addService("LArG4H6SD.LArG4H6SDConfigLegacy.getEMECPosInnerWheel_ECOR_CHCLCalculator", "EMECPosInnerWheel_ECOR_CHCLCalculator")

addService("LArG4H6SD.LArG4H6SDConfigLegacy.getLArH62004HECLocalCalculator","LArH62004HECLocalCalculator")
addService("LArG4H6SD.LArG4H6SDConfigLegacy.getLocalCalibrationInactiveCalculator", "LocalCalibrationInactiveCalculator")
addService("LArG4H6SD.LArG4H6SDConfigLegacy.getLocalCalibrationActiveCalculator", "LocalCalibrationActiveCalculator")
