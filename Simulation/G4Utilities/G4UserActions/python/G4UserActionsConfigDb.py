# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

from AthenaCommon.CfgGetter import addTool

# New tools for multi-threading
addTool("G4UserActions.G4UserActionsConf.G4UA__G4SimTimerTool", "G4UA::G4SimTimerTool")
addTool("G4UserActions.G4UserActionsConf.G4UA__CosmicPerigeeActionTool", "G4UA::CosmicPerigeeActionTool")
addTool("G4UserActions.G4UserActionsConf.G4UA__G4TrackCounterTool", "G4UA::G4TrackCounterTool")
addTool("G4UserActions.G4UserActionsConf.G4UA__HitWrapperTool", "G4UA::HitWrapperTool")
addTool("G4UserActions.G4UserActionsConf.G4UA__LengthIntegratorTool", "G4UA::LengthIntegratorTool")
addTool("G4UserActions.G4UserActionsConf.G4UA__PhotonKillerTool", "G4UA::PhotonKillerTool")
addTool("G4UserActions.G4UserActionsConf.G4UA__StoppedParticleActionTool", "G4UA::StoppedParticleActionTool")

addTool("G4UserActions.G4UserActionsConfig.getFastIDKillerTool", "G4UA::FastIDKillerTool")
addTool("G4UserActions.G4UserActionsConfig.getHIPKillerTool", "G4UA::HIPKillerTool")
addTool("G4UserActions.G4UserActionsConfig.getHIPLArVolumeAcceptTool", "G4UA::HIPLArVolumeAcceptTool")
addTool("G4UserActions.G4UserActionsConfig.getLooperKillerTool", "G4UA::LooperKillerTool")
addTool("G4UserActions.G4UserActionsConfig.getMomentumConservationTool", "G4UA::MomentumConservationTool")
addTool("G4UserActions.G4UserActionsConfig.getScoringVolumeTrackKillerTool", "G4UA::ScoringVolumeTrackKillerTool")

addTool("G4UserActions.G4UserActionsConfig.getFluxRecorderTool", "G4UA::FluxRecorderTool")
addTool("G4UserActions.G4UserActionsConfig.getScoringPlaneTool", "G4UA::ScoringPlaneTool")


# old tools
addTool("G4UserActions.G4UserActionsConf.G4SimTimer", "G4SimTimer")
addTool("G4UserActions.G4UserActionsConf.G4TrackCounter", "G4TrackCounter")
addTool("G4UserActions.G4UserActionsConf.CosmicPerigeeAction", "CosmicPerigeeAction")
addTool("G4UserActions.G4UserActionsConf.PhotonKiller", "PhotonKiller")
addTool("G4UserActions.G4UserActionsConf.HitWrapper", "HitWrapper")
addTool("G4UserActions.G4UserActionsConf.StoppedParticleAction", "StoppedParticleAction")
addTool("G4UserActions.G4UserActionsConf.LooperKiller", "LooperKiller")
addTool("G4UserActions.G4UserActionsConf.HIPKiller", "HIPKiller")
addTool("G4UserActions.G4UserActionsConf.LengthIntegrator", "LengthIntegrator")
addTool("G4UserActions.G4UserActionsConf.FastIDKiller", "FastIDKiller")
addTool("G4UserActions.G4UserActionsConf.G4SimMem", "G4SimMem")
addTool("G4UserActions.G4UserActionsConf.HIPLArVolumeAccept", "HIPLArVolumeAccept")
addTool("G4UserActions.G4UserActionsConf.MomentumConservation", "MomentumConservation")
addTool("G4UserActions.G4UserActionsConf.FluxRecorder", "FluxRecorder")
addTool("G4UserActions.G4UserActionsConf.ScoringPlane", "ScoringPlane")
addTool("G4UserActions.G4UserActionsConf.ScoringVolumeTrackKiller", "ScoringVolumeTrackKiller")
