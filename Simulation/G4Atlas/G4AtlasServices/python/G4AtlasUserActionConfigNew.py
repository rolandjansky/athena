# Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration

from AthenaCommon.SystemOfUnits import MeV
from AthenaConfiguration.ComponentAccumulator import ComponentAccumulator
from AthenaConfiguration.ComponentFactory import CompFactory
from AthenaConfiguration.Enums import BeamType, LHCPeriod
from CaloG4Sim.CaloG4SimConfigNew import CalibrationDefaultProcessingToolCfg
from G4AtlasApps.SimEnums import CalibrationRun, CavernBackground, SimulationFlavour
from G4CosmicFilter.G4CosmicFilterConfigNew import CosmicFilterToolCfg
from G4UserActions.G4UserActionsConfigNew import (
    AthenaStackingActionToolCfg, AthenaTrackingActionToolCfg,
    G4SimTimerToolCfg, G4TrackCounterToolCfg, HitWrapperToolCfg,
    LooperKillerToolCfg, StoppedParticleActionToolCfg
)
from ISF_Geant4CommonTools.ISF_Geant4CommonToolsConfigNew import EntryLayerToolCfg, EntryLayerToolMTCfg
from ISF_Services.ISF_ServicesCoreConfigNew import AFIIGeoIDSvcCfg, GeoIDSvcCfg
from ISF_Services.ISF_ServicesConfigNew import (
    AFIIParticleBrokerSvcCfg, ParticleBrokerSvcCfg, TruthServiceCfg
)
from ISF_Tools.ISF_ToolsConfigNew import StoppedParticleFilterToolCfg
from MCTruthBase.MCTruthBaseConfigNew import MCTruthSteppingActionToolCfg


def OptionalUserActionCfg(flags):
    """ flags.Sim.OptionalUserActionList = ['G4UserActions.G4UserActionsConfigNew.FixG4CreatorProcessToolCfg']"""
    result = ComponentAccumulator()
    optionalUserActions = []
    for userActionString in flags.Sim.OptionalUserActionList:
        optionalUserActions += [result.popToolsAndMerge(getOptionalUACfg(flags, userActionString))]
    result.setPrivateTools(optionalUserActions)
    return result


def getOptionalUACfg(flags, userActionString):
    """Execute a function to configure and optional UserAction"""
    parts = userActionString.split('.')
    if len(parts) < 2:
        raise ValueError('OptionalUserAction strings should be of the form Package.Module.Function or Package.Function if defined in __init__.py')
    function = parts[-1]
    module = '.'.join(parts[:-1])
    from importlib import import_module
    loaded_module = import_module(module)
    function_def = getattr(loaded_module, function)
    return function_def(flags)


# Pulled in from ISF G4 to avoid circular dependence
def FullG4TrackProcessorUserActionToolCfg(flags, name="FullG4TrackProcessorUserActionTool", **kwargs):
    result = ComponentAccumulator()
    if flags.Sim.ISF.Simulator in [SimulationFlavour.FullG4MT, SimulationFlavour.FullG4MT_QS]:
        tool = result.popToolsAndMerge(EntryLayerToolMTCfg(flags))
    else:
        tool = result.popToolsAndMerge(EntryLayerToolCfg(flags))
    result.addPublicTool(tool)
    kwargs.setdefault("EntryLayerTool", result.getPublicTool(tool.name))
    kwargs.setdefault("GeoIDSvc", result.getPrimaryAndMerge(GeoIDSvcCfg(flags)).name)
    if flags.Detector.GeometryCavern:
        kwargs.setdefault("TruthVolumeLevel", 2)
    kwargs.setdefault("IsITkGeometry", flags.GeoModel.Run >= LHCPeriod.Run4)
    result.setPrivateTools(CompFactory.G4UA.iGeant4.TrackProcessorUserActionFullG4Tool(name, **kwargs))
    return result


def PhysicsValidationUserActionToolCfg(flags, name="ISFG4PhysicsValidationUserActionTool", **kwargs):
    result = ComponentAccumulator()
    result.setPrivateTools(CompFactory.G4UA.iGeant4.PhysicsValidationUserActionTool(name, **kwargs))
    return result


def MCTruthUserActionToolCfg(flags, name="ISFMCTruthUserActionTool", **kwargs):
    result = ComponentAccumulator()
    kwargs.setdefault("TruthRecordSvc", result.getPrimaryAndMerge(TruthServiceCfg(flags)).name)
    result.setPrivateTools(CompFactory.G4UA.iGeant4.MCTruthUserActionTool(name, **kwargs))
    return result


def TrackProcessorUserActionToolCfg(flags, name="ISFG4TrackProcessorUserActionTool", **kwargs):
    result = ComponentAccumulator()
    if "ParticleBroker" not in kwargs:
        kwargs.setdefault("ParticleBroker", result.getPrimaryAndMerge(ParticleBrokerSvcCfg(flags)).name)
    kwargs.setdefault("GeoIDSvc", result.getPrimaryAndMerge(GeoIDSvcCfg(flags)).name)
    result.setPrivateTools(CompFactory.G4UA.iGeant4.TrackProcessorUserActionPassBackTool(name, **kwargs))
    return result


def PassBackG4TrackProcessorUserActionToolCfg(flags, name="PassBackG4TrackProcessorUserActionTool", **kwargs):
    if flags.Sim.ISF.Simulator in [SimulationFlavour.PassBackG4MT]:
        kwargs.setdefault("ParticleBroker", "")
    return TrackProcessorUserActionToolCfg(flags, name, **kwargs)


def AFII_G4TrackProcessorUserActionToolCfg(flags, name="AFII_G4TrackProcessorUserActionTool", **kwargs):
    result = ComponentAccumulator()
    if flags.Sim.ISF.Simulator in [SimulationFlavour.PassBackG4MT, SimulationFlavour.ATLFASTIIMT, SimulationFlavour.ATLFAST3MT, SimulationFlavour.ATLFAST3MT_QS]:
        kwargs.setdefault("ParticleBroker", "")
    elif flags.Sim.ISF.Simulator in [SimulationFlavour.ATLFASTIIF_G4MS, SimulationFlavour.ATLFAST3F_G4MS]:
        kwargs.setdefault("ParticleBroker", result.getPrimaryAndMerge(AFIIParticleBrokerSvcCfg(flags)).name)
    kwargs.setdefault("GeoIDSvc", result.getPrimaryAndMerge(AFIIGeoIDSvcCfg(flags)).name)
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
    if ConfigFlags.Beam.Type is BeamType.Cosmics and ConfigFlags.Sim.CavernBackground is CavernBackground.Off:
        actions += [CompFactory.G4UA.CosmicPerigeeActionTool()]
    # Cosmic filter
    if ConfigFlags.Beam.Type is BeamType.Cosmics and not ConfigFlags.Sim.ISFRun:
        actions += [result.popToolsAndMerge(CosmicFilterToolCfg(ConfigFlags))]
    if ConfigFlags.Sim.StoppedParticleFile:
        actions += [result.popToolsAndMerge(StoppedParticleFilterToolCfg(ConfigFlags)),
                    result.popToolsAndMerge(StoppedParticleActionToolCfg(ConfigFlags))]
    # Hit wrapper action
    if ConfigFlags.Sim.CavernBackground is CavernBackground.Read:
        actions += [result.popToolsAndMerge(HitWrapperToolCfg(ConfigFlags))]
    # Photon killer
    if ConfigFlags.Sim.PhysicsList == "QGSP_BERT_HP":
        actions += [CompFactory.G4UA.PhotonKillerTool()]
    # Calo calibration default processing
    if ConfigFlags.Sim.CalibrationRun is CalibrationRun.LArTile:
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
    result.addService(CompFactory.G4UA.UserActionSvc(name, **kwargs), primary = True)

    return result


def CTBUserActionSvcCfg(ConfigFlags, name="G4UA::CTBUserActionSvc", **kwargs):
    result = ComponentAccumulator()
    # FIXME migrate an alternative to this
    generalActions = result.popToolsAndMerge(getDefaultActions(ConfigFlags))
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
    result.addService(CompFactory.G4UA.UserActionSvc(name, **kwargs), primary = True)
    return result


def ISFUserActionSvcCfg(ConfigFlags, name="G4UA::ISFUserActionSvc", **kwargs):
    result = ComponentAccumulator()
    TrackProcessorUserAction = kwargs.pop("TrackProcessorUserAction",[])

    PhysicsValidationUserAction = []
    if ConfigFlags.Sim.ISF.ValidationMode:
        PhysicsValidationUserAction = [result.popPrivateTools(PhysicsValidationUserActionToolCfg(ConfigFlags))]

    MCTruthUserAction = kwargs.pop("MCTruthUserAction",
                                   [result.popToolsAndMerge(MCTruthUserActionToolCfg(ConfigFlags))])

    generalActions = ( TrackProcessorUserAction + MCTruthUserAction +
                       result.popToolsAndMerge(getDefaultActions(ConfigFlags)) +
                       result.popToolsAndMerge(OptionalUserActionCfg(ConfigFlags)) +
                       PhysicsValidationUserAction )

    # New user action tools
    kwargs.setdefault("UserActionTools", generalActions)
    result.addService(CompFactory.G4UA.UserActionSvc(name, **kwargs), primary = True)
    return result


def ISFFullUserActionSvcCfg(ConfigFlags, name="G4UA::ISFFullUserActionSvc", **kwargs):
    # this configuration needs ISFMCTruthUserAction
    # and FullG4TrackProcessorUserAction
    tpAcc = FullG4TrackProcessorUserActionToolCfg(ConfigFlags)
    kwargs.setdefault("TrackProcessorUserAction", [tpAcc.popPrivateTools()])
    result = ISFUserActionSvcCfg(ConfigFlags, name, **kwargs)
    result.merge(tpAcc)
    return result


def ISFPassBackUserActionSvcCfg(ConfigFlags, name="G4UA::ISFPassBackUserActionSvc", **kwargs):
    # this configuration needs ISFMCTruthUserAction and
    # PassBackG4TrackProcessorUserAction
    tpAcc = ComponentAccumulator()
    kwargs.setdefault("TrackProcessorUserAction",
                      [tpAcc.popToolsAndMerge(PassBackG4TrackProcessorUserActionToolCfg(ConfigFlags))])
    result = ISFUserActionSvcCfg(ConfigFlags, name, **kwargs)
    result.merge(tpAcc)
    return result


def ISF_AFIIUserActionSvcCfg(ConfigFlags, name="G4UA::ISF_AFIIUserActionSvc", **kwargs):
    # this configuration needs ISFMCTruthUserAction and
    # AFII_G4TrackProcessorUserAction
    tpAcc = ComponentAccumulator()
    kwargs.setdefault("TrackProcessorUserAction",
                      [tpAcc.popToolsAndMerge(AFII_G4TrackProcessorUserActionToolCfg(ConfigFlags))])
    result = ISFUserActionSvcCfg(ConfigFlags, name, **kwargs)
    result.merge(tpAcc)
    return result
