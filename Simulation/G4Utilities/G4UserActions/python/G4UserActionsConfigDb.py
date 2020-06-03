# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

from AthenaCommon.CfgGetter import addTool

# New tools for multi-threading
addTool("G4UserActions.G4UserActionsConf.G4UA__G4SimTimerTool", "G4UA::G4SimTimerTool")
addTool("G4UserActions.G4UserActionsConf.G4UA__CosmicPerigeeActionTool", "G4UA::CosmicPerigeeActionTool")
addTool("G4UserActions.G4UserActionsConf.G4UA__G4TrackCounterTool", "G4UA::G4TrackCounterTool")
addTool("G4UserActions.G4UserActionsConf.G4UA__LengthIntegratorTool", "G4UA::LengthIntegratorTool")
addTool("G4UserActions.G4UserActionsConf.G4UA__PhotonKillerTool", "G4UA::PhotonKillerTool")

addTool("G4UserActions.G4UserActionsConfig.getFastIDKillerTool", "G4UA::FastIDKillerTool")
addTool("G4UserActions.G4UserActionsConfig.getFastMBKillerTool", "G4UA::FastMBKillerTool")
addTool("G4UserActions.G4UserActionsConfig.getHitWrapperTool", "G4UA::HitWrapperTool")
addTool("G4UserActions.G4UserActionsConfig.getHIPKillerTool", "G4UA::HIPKillerTool")
addTool("G4UserActions.G4UserActionsConfig.getHIPLArVolumeAcceptTool", "G4UA::HIPLArVolumeAcceptTool")
addTool("G4UserActions.G4UserActionsConfig.getLooperKillerTool", "G4UA::LooperKillerTool")
addTool("G4UserActions.G4UserActionsConfig.getMonopoleLooperKillerTool", "G4UA::MonopoleLooperKillerTool")
addTool("G4UserActions.G4UserActionsConfig.getLooperKillerEventOverlayTool", "G4UA::LooperKillerEventOverlayTool")
addTool("G4UserActions.G4UserActionsConfig.getMomentumConservationTool", "G4UA::MomentumConservationTool")
addTool("G4UserActions.G4UserActionsConfig.getScoringVolumeTrackKillerTool", "G4UA::ScoringVolumeTrackKillerTool")

addTool("G4UserActions.G4UserActionsConfig.getFluxRecorderTool", "G4UA::FluxRecorderTool")
addTool("G4UserActions.G4UserActionsConfig.getScoringPlaneTool", "G4UA::ScoringPlaneTool")
addTool("G4UserActions.G4UserActionsConfig.getRadiationMapsMakerTool", "G4UA::RadiationMapsMakerTool")
addTool("G4UserActions.G4UserActionsConfig.getStoppedParticleActionTool", "G4UA::StoppedParticleActionTool")
addTool("G4UserActions.G4UserActionsConfig.getRadLengthActionTool", "G4UA::RadLengthActionTool")
addTool("G4UserActions.G4UserActionsConfig.getVolumeDumperTool", "G4UA::VolumeDumperTool")
