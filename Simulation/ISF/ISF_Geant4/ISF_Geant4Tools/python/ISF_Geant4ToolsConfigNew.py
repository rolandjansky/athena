"""Geant4 tools config for ISF with ComponentAccumulator

Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
"""
from AthenaConfiguration.ComponentAccumulator import ComponentAccumulator
from AthenaConfiguration.ComponentFactory import CompFactory
from AthenaCommon.SystemOfUnits import MeV
from ISF_Services.ISF_ServicesCoreConfigNew import GeoIDSvcCfg, AFIIGeoIDSvcCfg
from ISF_Services.ISF_ServicesConfigNew import (
    TruthServiceCfg,
    ParticleBrokerSvcCfg, InputConverterCfg,
    LongLivedInputConverterCfg
)
from ISF_Geant4CommonTools.ISF_Geant4CommonToolsConfigNew import EntryLayerToolCfg, EntryLayerToolMTCfg
from G4AtlasServices.G4AtlasUserActionConfigNew import (
    ISFUserActionSvcCfg, ISFFullUserActionSvcCfg,
    ISFPassBackUserActionSvcCfg, ISF_AFIIUserActionSvcCfg,
)
from G4AtlasTools.G4AtlasToolsConfigNew import (
    SensitiveDetectorMasterToolCfg, FastSimulationMasterToolCfg
)
from RngComps.RandomServices import RNG

# Basic versions
def MCTruthUserActionToolCfg(flags, name="ISFMCTruthUserActionTool", **kwargs):
    acc = ComponentAccumulator()
    truthacc = TruthServiceCfg(flags)
    kwargs.setdefault("TruthRecordSvc", truthacc.getPrimary())
    acc.merge(truthacc)
    acc.setPrivateTools(CompFactory.G4UA.iGeant4.MCTruthUserActionTool(name, **kwargs))
    return acc


def PhysicsValidationUserActionToolCfg(flags, name="ISFG4PhysicsValidationUserActionTool", **kwargs):
    acc = ComponentAccumulator()
    acc.setPrivateTools(CompFactory.G4UA.iGeant4.PhysicsValidationUserActionTool(name, **kwargs))
    return acc


def TrackProcessorUserActionToolCfg(flags, name="ISFG4TrackProcessorUserActionTool", **kwargs):
    acc = ParticleBrokerSvcCfg(flags)
    kwargs.setdefault("ParticleBroker", acc.getService("ISF_ParticleBrokerSvc"))
    acc.merge(GeoIDSvcCfg(flags))
    kwargs.setdefault("GeoIDSvc", acc.getService("ISF_GeoIDSvc"))
    acc.setPrivateTools(CompFactory.G4UA.iGeant4.TrackProcessorUserActionPassBackTool(name, **kwargs))
    return acc


# Specialized Versions
def FullG4TrackProcessorUserActionToolCfg(flags, name="FullG4TrackProcessorUserActionTool", **kwargs):
    acc = ComponentAccumulator()
    if flags.Sim.Simulator in ["FullG4MT"]:
        tool = acc.popToolsAndMerge(EntryLayerToolMTCfg(flags))
    else:
        tool = acc.popToolsAndMerge(EntryLayerToolCfg(flags))
    kwargs.setdefault("EntryLayerTool", tool)
    acc.merge(GeoIDSvcCfg(flags))
    kwargs.setdefault("GeoIDSvc", acc.getService("ISF_GeoIDSvc"))
    if flags.Detector.SimulateCavern:
        kwargs.setdefault("TruthVolumeLevel", 2)
    acc.setPrivateTools(CompFactory.G4UA.iGeant4.TrackProcessorUserActionFullG4Tool(name, **kwargs))
    return acc


def PassBackG4TrackProcessorUserActionToolCfg(flags, name="PassBackG4TrackProcessorUserActionTool", **kwargs):
    return TrackProcessorUserActionToolCfg(name, **kwargs)


def AFII_G4TrackProcessorUserActionToolCfg(flags, name="AFII_G4TrackProcessorUserActionTool", **kwargs):
    acc = ComponentAccumulator()
    if flags.Sim.Simulator in ["PassBackG4MT", "ATLFASTIIMT", "G4FastCaloMT"]:
        kwargs.setdefault("ParticleBroker", "")
    acc.merge(AFIIGeoIDSvcCfg(flags))
    kwargs.setdefault("GeoIDSvc", acc.getService("ISF_AFIIGeoIDSvc"))
    kwargs.setdefault("PassBackEkinThreshold", 0.05*MeV)
    kwargs.setdefault("KillBoundaryParticlesBelowThreshold", True)
    acc.merge(PassBackG4TrackProcessorUserActionToolCfg(name, **kwargs))
    return acc


def Geant4ToolCfg(flags, name="ISF_Geant4Tool", **kwargs):
    acc = RNG(flags.Random.Engine)
    kwargs.setdefault("RandomNumberService", acc.getService("AthRNGSvc"))
    acc.merge(InputConverterCfg(flags))
    kwargs.setdefault("InputConverter", acc.getService("ISF_InputConverter"))
    acc.merge(ISFUserActionSvcCfg(flags))
    kwargs.setdefault("UserActionSvc", acc.getService("G4UA::ISFUserActionSvc"))
    kwargs.setdefault("RecordFlux", flags.Sim.RecordFlux)

    kwargs.setdefault("MultiThreading", flags.Concurrency.NumThreads > 0)
    # Set commands for the G4AtlasAlg
    kwargs.setdefault("G4Commands", flags.Sim.G4Commands)
    kwargs.setdefault("PrintTimingInfo", flags.Sim.ISF.DoTimeMonitoring)
    tool = acc.popToolsAndMerge(SensitiveDetectorMasterToolCfg(flags))
    kwargs.setdefault("SenDetMasterTool", tool)
    tool = acc.popToolsAndMerge(FastSimulationMasterToolCfg(flags))
    kwargs.setdefault("FastSimMasterTool", tool)
    # Workaround to keep other simulation flavours working while we migrate everything to be AthenaMT-compatible.
    if flags.Sim.Simulator in ["FullG4", "FullG4MT", "PassBackG4", "PassBackG4MT", "G4FastCalo", "G4FastCaloMT"]:
        acc.setPrivateTools(CompFactory.iGeant4.G4TransportTool(name, **kwargs))
    else:
        acc.setPrivateTools(CompFactory.iGeant4.G4LegacyTransportTool(name, **kwargs))
    
    return acc


def FullGeant4ToolCfg(flags, name="ISF_FullGeant4Tool", **kwargs):
    acc = ISFFullUserActionSvcCfg(flags)
    kwargs.setdefault("UserActionSvc", acc.getService("G4UA::ISFFullUserActionSvc"))
    acc.merge(Geant4ToolCfg(name, **kwargs))
    return acc


def PassBackGeant4ToolCfg(flags, name="ISF_PassBackGeant4Tool", **kwargs):
    acc = ISFPassBackUserActionSvcCfg(flags)
    kwargs.setdefault("UserActionSvc", acc.getService("G4UA::ISFPassBackUserActionSvc"))
    return Geant4ToolCfg(name, **kwargs)


def AFIIGeant4ToolCfg(flags, name="ISF_AFIIGeant4Tool", **kwargs):
    acc = ISF_AFIIUserActionSvcCfg(flags)
    kwargs.setdefault("UserActionSvc", acc.getService("G4UA::ISF_AFIIUserActionSvc"))
    acc.merge(PassBackGeant4ToolCfg(name, **kwargs))
    return acc


def LongLivedGeant4ToolCfg(flags, name="ISF_LongLivedGeant4Tool", **kwargs):
    acc = LongLivedInputConverterCfg(flags)
    kwargs.setdefault("InputConverter", acc.getService("ISF_LongLivedInputConverter"))
    acc.merge(FullGeant4ToolCfg(name, **kwargs))
    return acc


def AFII_QS_Geant4ToolCfg(flags, name="AFII_QS_Geant4Tool", **kwargs):
    acc = LongLivedInputConverterCfg(flags)
    kwargs.setdefault("InputConverter", acc.getService("ISF_LongLivedInputConverter"))
    acc.merge(AFIIGeant4ToolCfg(name, **kwargs))
    return acc
