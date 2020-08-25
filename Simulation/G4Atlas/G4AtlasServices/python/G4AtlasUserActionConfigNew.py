# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

from AthenaConfiguration.ComponentAccumulator import ComponentAccumulator
from AthenaConfiguration.ComponentFactory import CompFactory
from AthenaCommon.SystemOfUnits import MeV
from MCTruthBase.MCTruthBaseConfigNew import MCTruthSteppingActionToolCfg
from G4UserActions.G4UserActionsConfigNew import (
    AthenaStackingActionToolCfg, AthenaTrackingActionToolCfg,
    LooperKillerToolCfg, G4SimTimerToolCfg, G4TrackCounterToolCfg,
    StoppedParticleActionToolCfg, HitWrapperToolCfg
)
from CaloG4Sim.CaloG4SimConfigNew import CalibrationDefaultProcessingToolCfg
from ISF_Tools.ISF_ToolsConfigNew import StoppedParticleFilterToolCfg
from ISF_Services.ISF_ServicesCoreConfigNew import GeoIDSvcCfg, AFIIGeoIDSvcCfg
from ISF_Services.ISF_ServicesConfigNew import (
    TruthServiceCfg, ParticleBrokerSvcCfg, 
)
from ISF_Geant4CommonTools.ISF_Geant4CommonToolsConfigNew import EntryLayerToolCfg, EntryLayerToolMTCfg


# Pulled in from ISF G4 to avoid circular dependence
def FullG4TrackProcessorUserActionToolCfg(flags, name="FullG4TrackProcessorUserActionTool", **kwargs):
    result = ComponentAccumulator()
    if flags.Sim.ISF.Simulator in ["FullG4MT"]:
        tool = result.popToolsAndMerge(EntryLayerToolMTCfg(flags))
    else:
        tool = result.popToolsAndMerge(EntryLayerToolCfg(flags))
    kwargs.setdefault("EntryLayerTool", tool)
    result.merge(GeoIDSvcCfg(flags))
    kwargs.setdefault("GeoIDSvc", result.getService("ISF_GeoIDSvc"))
    if flags.Detector.SimulateCavern:
        kwargs.setdefault("TruthVolumeLevel", 2)
    result.setPrivateTools(CompFactory.G4UA.iGeant4.TrackProcessorUserActionFullG4Tool(name, **kwargs))
    return result


def PhysicsValidationUserActionToolCfg(flags, name="ISFG4PhysicsValidationUserActionTool", **kwargs):
    result = ComponentAccumulator()
    result.setPrivateTools(CompFactory.G4UA.iGeant4.PhysicsValidationUserActionTool(name, **kwargs))
    return result


def MCTruthUserActionToolCfg(flags, name="ISFMCTruthUserActionTool", **kwargs):
    result = ComponentAccumulator()
    truthacc = TruthServiceCfg(flags)
    kwargs.setdefault("TruthRecordSvc", truthacc.getPrimary())
    result.merge(truthacc)
    result.setPrivateTools(CompFactory.G4UA.iGeant4.MCTruthUserActionTool(name, **kwargs))
    return result


def TrackProcessorUserActionToolCfg(flags, name="ISFG4TrackProcessorUserActionTool", **kwargs):
    result = ParticleBrokerSvcCfg(flags)
    kwargs.setdefault("ParticleBroker", result.getService("ISF_ParticleBrokerSvc"))
    result.merge(GeoIDSvcCfg(flags))
    kwargs.setdefault("GeoIDSvc", result.getService("ISF_GeoIDSvc"))
    result.setPrivateTools(CompFactory.G4UA.iGeant4.TrackProcessorUserActionPassBackTool(name, **kwargs))
    return result


def PassBackG4TrackProcessorUserActionToolCfg(flags, name="PassBackG4TrackProcessorUserActionTool", **kwargs):
    return TrackProcessorUserActionToolCfg(name, **kwargs)


def AFII_G4TrackProcessorUserActionToolCfg(flags, name="AFII_G4TrackProcessorUserActionTool", **kwargs):
    result = ComponentAccumulator()
    if flags.Sim.ISF.Simulator in ["PassBackG4MT", "ATLFASTIIMT", "G4FastCaloMT"]:
        kwargs.setdefault("ParticleBroker", "")
    result.merge(AFIIGeoIDSvcCfg(flags))
    kwargs.setdefault("GeoIDSvc", result.getService("ISF_AFIIGeoIDSvc"))
    kwargs.setdefault("PassBackEkinThreshold", 0.05*MeV)
    kwargs.setdefault("KillBoundaryParticlesBelowThreshold", True)
    tool = result.popToolsAndMerge(PassBackG4TrackProcessorUserActionToolCfg(flags, name, **kwargs))
    result.setPrivateTools(tool)
    return result


# New function for all user action types
def getDefaultActions(ConfigFlags):
    result = ComponentAccumulator()

    actions = []

    # System stacking action
    actions += [result.popToolsAndMerge(AthenaStackingActionToolCfg(ConfigFlags))]

    # Some truth handling actions (and timing)
    if not ConfigFlags.Sim.ISFRun:
        actions += [result.popToolsAndMerge(AthenaTrackingActionToolCfg(ConfigFlags)),
                    result.popToolsAndMerge(MCTruthSteppingActionToolCfg(ConfigFlags)),
                    result.popToolsAndMerge(G4SimTimerToolCfg(ConfigFlags))]
    # Track counter
    actions += [result.popToolsAndMerge(G4TrackCounterToolCfg(ConfigFlags))]

    # Cosmic Perigee action
    if ConfigFlags.Beam.Type == "cosmics" and ConfigFlags.Sim.CavernBG:
        actions += [CompFactory.G4UA.CosmicPerigeeActionTool()]
    # Cosmic filter
    if ConfigFlags.Beam.Type == "cosmics" and not ConfigFlags.Sim.ISFRun:
        actions += [CompFactory.G4UA.G4CosmicFilterTool()]
    if ConfigFlags.Sim.StoppedParticleFile:
        actions += [result.popToolsAndMerge(StoppedParticleFilterToolCfg(ConfigFlags)),
                    result.popToolsAndMerge(StoppedParticleActionToolCfg(ConfigFlags))]
    # Hit wrapper action
    if ConfigFlags.Sim.CavernBG == "Read":
        actions += [result.popToolsAndMerge(HitWrapperToolCfg(ConfigFlags))]
    # Photon killer
    if ConfigFlags.Sim.PhysicsList == "QGSP_BERT_HP":
        actions += [CompFactory.G4UA.PhotonKillerTool()]
    # Calo calibration default processing
    if ConfigFlags.Sim.CalibrationRun == "LAr+Tile":
        actions += [result.popToolsAndMerge(CalibrationDefaultProcessingToolCfg(ConfigFlags))]

    actions += [result.popToolsAndMerge(LooperKillerToolCfg(ConfigFlags))]
    
    result.setPrivateTools(actions)
    return result


def UserActionSvcCfg(ConfigFlags, name="G4UA::UserActionSvc", **kwargs):
    """
    Get the standard UA svc configurable with all default actions added.
    This function is normally called by the configured factory, not users.
    """
    result = ComponentAccumulator()

    # new user action tools
    kwargs.setdefault("UserActionTools", result.popToolsAndMerge(getDefaultActions(ConfigFlags)))

    # placeholder for more advanced config, if needed
    result.addService(CompFactory.G4UA.UserActionSvc(name, **kwargs))

    return result


def CTBUserActionSvcCfg(ConfigFlags, name="G4UA::CTBUserActionSvc", **kwargs):
    result = ComponentAccumulator()
    # FIXME migrate an alternative to this
    generalActions = result.popToolsAndMerge(getDefaultActions(ConfigFlags))
    generalActions += [result.popToolsAndMerge(LooperKillerToolCfg(ConfigFlags))]
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
        PhysicsValidationUserAction = [result.popPrivateTools(PhysicsValidationUserActionToolCfg(ConfigFlags))]

    MCTruthUserAction = kwargs.pop("MCTruthUserAction",
                                   [result.popToolsAndMerge(MCTruthUserActionToolCfg(ConfigFlags))])

    # FIXME migrate an alternative to this
    #from G4AtlasApps.SimFlags import simFlags
    #optActions = simFlags.OptionalUserActionList.get_Value()

    generalActions = (
        TrackProcessorUserAction + MCTruthUserAction +
        result.popToolsAndMerge(getDefaultActions(ConfigFlags)) +
        [result.popToolsAndMerge(LooperKillerToolCfg(ConfigFlags))] +
        PhysicsValidationUserAction
    )

    # New user action tools
    kwargs.setdefault("UserActionTools", generalActions)
    result.addService(CompFactory.G4UA.UserActionSvc(name, **kwargs))
    return result


def ISFFullUserActionSvcCfg(ConfigFlags, name="G4UA::ISFFullUserActionSvc", **kwargs):
    # this configuration needs ISFMCTruthUserAction
    # and FullG4TrackProcessorUserAction
    result = FullG4TrackProcessorUserActionToolCfg(ConfigFlags)
    kwargs.setdefault("TrackProcessorUserAction", [result.popPrivateTools()])
    result.merge(ISFUserActionSvcCfg(ConfigFlags, name, **kwargs))
    return result


def ISFPassBackUserActionSvcCfg(ConfigFlags, name="G4UA::ISFPassBackUserActionSvc", **kwargs):
    # this configuration needs ISFMCTruthUserAction and
    # PassBackG4TrackProcessorUserAction
    result = ComponentAccumulator()
    kwargs.setdefault("TrackProcessorUserAction",
                      [result.popToolsAndMerge(PassBackG4TrackProcessorUserActionToolCfg(ConfigFlags))])
    result.merge(ISFUserActionSvcCfg(ConfigFlags, name, **kwargs))
    return result


def ISF_AFIIUserActionSvcCfg(ConfigFlags, name="G4UA::ISF_AFIIUserActionSvc", **kwargs):
    # this configuration needs ISFMCTruthUserAction and
    # AFII_G4TrackProcessorUserAction
    result = ComponentAccumulator()
    kwargs.setdefault("TrackProcessorUserAction",
                      [result.popToolsAndMerge(AFII_G4TrackProcessorUserActionToolCfg(ConfigFlags))])
    result.merge(ISFUserActionSvcCfg(ConfigFlags, name, **kwargs))
    return result
