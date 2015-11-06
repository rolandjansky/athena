# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

from AthenaCommon.CfgGetter import addTool
addTool("LArG4H6SD.LArG4H6SDTools.LArGeoH62004EventAction", "LArGeoH62004EventAction")
addTool("LArG4H6SD.LArG4H6SDTools.LArGeoH62004SteppingAction", "LArGeoH62004SteppingAction")
addTool("LArG4H6SD.LArG4H6SDTools.LArHitsH6EventAction", "LArHitsH6EventAction")
addTool("LArG4H6SD.LArG4H6SDTools.LArTBH6BeamInfo", "LArTBH6BeamInfo")
addTool("LArG4H6SD.LArG4H6SDTools.LArTBH6TriggerTimeTool", "LArTBH6TriggerTimeTool")
addTool("LArG4H6SD.LArG4H6SDTools.RadLenNtuple", "RadLenNtuple")

# Sensitive detector tools
addTool("LArG4H6SD.LArG4H6SDConfig.getLArG4TBECBeamChSDTool", "LArG4TBECBeamChSensitiveDetector")
addTool("LArG4H6SD.LArG4H6SDConfig.getFrontLArG4H6BeamSDTool", "LArG4H6BeamSD::Front")
addTool("LArG4H6SD.LArG4H6SDConfig.getMovableLArG4H6BeamSDTool", "LArG4H6BeamSD::Movable")

addTool("LArG4H6SD.LArG4H6SDConfig.getLArG4H6WarmTC_X", "LArG4H6WarmTC::X")
addTool("LArG4H6SD.LArG4H6SDConfig.getLArG4H6WarmTC_Y", "LArG4H6WarmTC::Y")
addTool("LArG4H6SD.LArG4H6SDConfig.getLArG4H6WarmTC_Abs", "LArG4H6WarmTC::Abs")

addTool("LArG4H6SD.LArG4H6SDConfig.getTBECInnerModuleTool", "TBECInnerModuleMult")
addTool("LArG4H6SD.LArG4H6SDConfig.getTBECOuterModuleTool", "TBECOuterModuleMult")
addTool("LArG4H6SD.LArG4H6SDConfig.getTBECBackOuterBarretteTool", "TBECBackOuterBarretteMult")

addTool("LArG4H6SD.LArG4H6SDConfig.getLArG4H62004HECSDTool", "LArG4H62004HECSDTool")
addTool("LArG4H6SD.LArG4H6SDConfig.getLArG4H62004EMECSDTool", "LArG4H62004EMECSDTool")
addTool("LArG4H6SD.LArG4H6SDConfig.getLArG4H62004FCALSDTool", "LArG4H62004FCALSDTool")

addTool("LArG4H6SD.LArG4H6SDConfig.getLArG4H62004ActiveSDTool", "LArG4H62004ActiveSDTool")
addTool("LArG4H6SD.LArG4H6SDConfig.getLArG4H62004DeadSDTool", "LArG4H62004DeadSDTool")
addTool("LArG4H6SD.LArG4H6SDConfig.getLArG4H62004InactiveSDTool", "LArG4H62004InactiveSDTool")

