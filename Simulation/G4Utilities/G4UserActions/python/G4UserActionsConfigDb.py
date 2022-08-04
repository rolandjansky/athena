# Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration

from AthenaCommon.CfgGetter import addTool

# New tools for multi-threading
addTool("G4UserActions.G4UserActionsConfigLegacy.getAthenaTrackingActionTool", "G4UA::AthenaTrackingActionTool")
addTool("G4UserActions.G4UserActionsConfigLegacy.getAthenaStackingActionTool", "G4UA::AthenaStackingActionTool")

addTool("AthenaCommon.CfgMgr.G4UA__G4SimTimerTool", "G4UA::G4SimTimerTool")
addTool("AthenaCommon.CfgMgr.G4UA__CosmicPerigeeActionTool", "G4UA::CosmicPerigeeActionTool")
addTool("AthenaCommon.CfgMgr.G4UA__G4TrackCounterTool", "G4UA::G4TrackCounterTool")
addTool("AthenaCommon.CfgMgr.G4UA__LengthIntegratorTool", "G4UA::LengthIntegratorTool")
addTool("AthenaCommon.CfgMgr.G4UA__PhotonKillerTool", "G4UA::PhotonKillerTool")

addTool("G4UserActions.G4UserActionsConfigLegacy.getFixG4CreatorProcessTool", "G4UA::FixG4CreatorProcessTool")
addTool("G4UserActions.G4UserActionsConfigLegacy.getFastIDKillerTool", "G4UA::FastIDKillerTool")
addTool("G4UserActions.G4UserActionsConfigLegacy.getFastMBKillerTool", "G4UA::FastMBKillerTool")
addTool("G4UserActions.G4UserActionsConfigLegacy.getHitWrapperTool", "G4UA::HitWrapperTool")
addTool("G4UserActions.G4UserActionsConfigLegacy.getHIPKillerTool", "G4UA::HIPKillerTool")
addTool("G4UserActions.G4UserActionsConfigLegacy.getHIPLArVolumeAcceptTool", "G4UA::HIPLArVolumeAcceptTool")
addTool("G4UserActions.G4UserActionsConfigLegacy.getLooperKillerTool", "G4UA::LooperKillerTool")
addTool("G4UserActions.G4UserActionsConfigLegacy.getMonopoleLooperKillerTool", "G4UA::MonopoleLooperKillerTool")
addTool("G4UserActions.G4UserActionsConfigLegacy.getLooperKillerEventOverlayTool", "G4UA::LooperKillerEventOverlayTool")
addTool("G4UserActions.G4UserActionsConfigLegacy.getMomentumConservationTool", "G4UA::MomentumConservationTool")
addTool("G4UserActions.G4UserActionsConfigLegacy.getScoringVolumeTrackKillerTool", "G4UA::ScoringVolumeTrackKillerTool")

addTool("G4UserActions.G4UserActionsConfigLegacy.getFluxRecorderTool", "G4UA::FluxRecorderTool")
addTool("G4UserActions.G4UserActionsConfigLegacy.getScoringPlaneTool", "G4UA::ScoringPlaneTool")
addTool("G4UserActions.G4UserActionsConfigLegacy.getRadiationMapsMakerTool", "G4UA::RadiationMapsMakerTool")
addTool("G4UserActions.G4UserActionsConfigLegacy.getStoppedParticleActionTool", "G4UA::StoppedParticleActionTool")
addTool("G4UserActions.G4UserActionsConfigLegacy.getRadLengthActionTool", "G4UA::RadLengthActionTool")
addTool("G4UserActions.G4UserActionsConfigLegacy.getLooperThresholdSetTool", "G4UA::LooperThresholdSetTool")
addTool("G4UserActions.G4UserActionsConfigLegacy.getVolumeDumperTool", "G4UA::VolumeDumperTool")
