# Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration

from AthenaConfiguration.ComponentAccumulator import ComponentAccumulator
from MCTruthBase.MCTruthBaseConfigNew import MCTruthSteppingActionToolCfg
from AthenaConfiguration.ComponentFactory import CompFactory
from G4UserActions.G4UserActionsConfigNew import AthenaStackingActionToolCfg, AthenaTrackingActionToolCfg, LooperKillerToolCfg, G4SimTimerToolCfg, G4TrackCounterToolCfg


# New function for all user action types
def getDefaultActions(ConfigFlags):
    result = ComponentAccumulator()

    actions = []

    # System stacking action
    actions += [result.popToolsAndMerge( AthenaStackingActionToolCfg(ConfigFlags)  )]

    # Some truth handling actions (and timing)
    if not ConfigFlags.Sim.ISFRun:
        actions += [
                    result.popToolsAndMerge( AthenaTrackingActionToolCfg(ConfigFlags) ),
                    result.popToolsAndMerge( MCTruthSteppingActionToolCfg(ConfigFlags) ),
                    result.popToolsAndMerge( G4SimTimerToolCfg(ConfigFlags))
                    ]
    # Track counter
    actions += [result.popToolsAndMerge( G4TrackCounterToolCfg(ConfigFlags) ) ]

    # Cosmic Perigee action
    if ConfigFlags.Beam.Type == "cosmics" and ConfigFlags.Sim.CavernBG:
        actions += ["G4UA::CosmicPerigeeActionTool"]
    # Cosmic filter
    if ConfigFlags.Beam.Type == "cosmics" and not ConfigFlags.Sim.ISFRun:
        actions += ["G4UA::G4CosmicFilterTool"]
    if ConfigFlags.Sim.StoppedParticleFile:
        actions += ["G4UA::StoppedParticleFilterTool",
                    "G4UA::StoppedParticleActionTool"]
    # Hit wrapper action
    if ConfigFlags.Sim.CavernBG == "Read":
        actions += ["G4UA::HitWrapperTool"]
    # Photon killer
    if ConfigFlags.Sim.PhysicsList == "QGSP_BERT_HP":
        actions += ["G4UA::PhotonKillerTool"]
    # Calo calibration default processing
    if ConfigFlags.Sim.CalibrationRun == "LAr+Tile":
        actions+=["G4UA::CaloG4::CalibrationDefaultProcessingTool"]

    actions += [ result.popToolsAndMerge( LooperKillerToolCfg(ConfigFlags) ) ]
    return actions


def UserActionSvcCfg(ConfigFlags, name="G4UA::UserActionSvc", **kwargs):
    """
    Get the standard UA svc configurable with all default actions added.
    This function is normally called by the configured factory, not users.
    """
    result = ComponentAccumulator()

    # new user action tools
    kwargs.setdefault("UserActionTools", getDefaultActions(ConfigFlags))

    # placeholder for more advanced config, if needed
    result.addService ( CompFactory.G4UA.UserActionSvc(name, **kwargs) )

    return result


def CTBUserActionSvcCfg(ConfigFlags, name="G4UA::CTBUserActionSvc", **kwargs):
    result = ComponentAccumulator()
    # FIXME migrate an alternative to this    
    generalActions = getDefaultActions() + ["G4UA::LooperKillerTool"]
    # This comment carried over from old style:
    # FIXME: ADS these actions are not yet migrated to Hive
    #if simFlags.SimLayout.get_Value()=="tb_LArH6_2004":
    #    eoe=["LArHitsH6EventAction"]+eoe
    #    eoe+=["LArGeoH62004EventAction"]
    #    if simFlags.LArTB_H6Step.statusOn:
    #        if simFlags.LArTB_H6Step.get_Value():
    #            stepping+=["LArGeoH62004SteppingAction"]
    #            event+=["RadLenNtuple"]
    #            eoe+=["RadLenNtuple"]
    #            stepping+=["RadLenNtuple"]

    # New user action tools
    kwargs.setdefault("UserActionTools", generalActions)

    # placeholder for more advanced config, if needed
    result.addService(CompFactory.G4UA.UserActionSvc(name, **kwargs))
    return result


def ISFUserActionSvcCfg(ConfigFlags, name="G4UA::ISFUserActionSvc", **kwargs):
    result = ComponentAccumulator()
    TrackProcessorUserAction = kwargs.pop("TrackProcessorUserAction",[])
    PhysicsValidationUserAction = []
    if ConfigFlags.Sim.ISF.ValidationMode:
        PhysicsValidationUserAction = ["ISFG4PhysicsValidationUserActionTool"]
    MCTruthUserAction = kwargs.pop("MCTruthUserAction",["ISFMCTruthUserActionTool"])
    # FIXME migrate an alternative to this
    #from G4AtlasApps.SimFlags import simFlags
    #optActions = simFlags.OptionalUserActionList.get_Value()
    #generalActions = (
        #TrackProcessorUserAction + MCTruthUserAction +
        #getDefaultActions() + optActions["General"] +
        #PhysicsValidationUserAction
    #)
    #optActions = simFlags.OptionalUserActionList.get_Value()
    generalActions = (
        TrackProcessorUserAction + MCTruthUserAction +
        getDefaultActions() + ["G4UA::LooperKillerTool"] +
        PhysicsValidationUserAction
    )

    # New user action tools
    kwargs.setdefault("UserActionTools", generalActions)
    result.addService(CompFactory.G4UA.UserActionSvc(name, **kwargs))
    return result


def ISFFullUserActionSvcCfg(ConfigFlags, name="G4UA::ISFFullUserActionSvc", **kwargs):
    # this configuration needs ISFMCTruthUserAction
    # and FullG4TrackProcessorUserAction
    kwargs.setdefault("TrackProcessorUserAction", ["FullG4TrackProcessorUserActionTool"])
    return ISFUserActionSvcCfg(name, **kwargs)


def ISFPassBackUserActionSvcCfg(ConfigFlags, name="G4UA::ISFPassBackUserActionSvc", **kwargs):
    # this configuration needs ISFMCTruthUserAction and
    # PassBackG4TrackProcessorUserAction
    kwargs.setdefault("TrackProcessorUserAction", ["PassBackG4TrackProcessorUserActionTool"])
    return ISFUserActionSvcCfg(name, **kwargs)


def ISF_AFIIUserActionSvcCfg(ConfigFlags, name="G4UA::ISF_AFIIUserActionSvc", **kwargs):
    # this configuration needs ISFMCTruthUserAction and
    # AFII_G4TrackProcessorUserAction
    kwargs.setdefault("TrackProcessorUserAction", ["AFII_G4TrackProcessorUserActionTool"])
    return ISFUserActionSvcCfg(name, **kwargs)
