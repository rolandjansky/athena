"""Geant4 tools config for ISF with ComponentAccumulator

Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
"""
from AthenaConfiguration.ComponentFactory import CompFactory
from RngComps.RandomServices import RNG
from G4AtlasServices.G4AtlasServicesConfigNew import (
    DetectorGeometrySvcCfg, PhysicsListSvcCfg
)
from G4AtlasServices.G4AtlasUserActionConfigNew import (
    ISFUserActionSvcCfg, ISFFullUserActionSvcCfg,
    ISFPassBackUserActionSvcCfg, ISF_AFIIUserActionSvcCfg,
)
from G4AtlasTools.G4AtlasToolsConfigNew import (
    SensitiveDetectorMasterToolCfg, FastSimulationMasterToolCfg
)
from ISF_Services.ISF_ServicesConfigNew import (
    InputConverterCfg, LongLivedInputConverterCfg
)
from ISF_FatrasServices.ISF_FatrasConfig import G4RunManagerHelperCfg


def Geant4ToolCfg(flags, name="ISF_Geant4Tool", **kwargs):
    acc = RNG(flags.Random.Engine, name="AthRNGSvc")
    kwargs.setdefault("RandomNumberService", acc.getService("AthRNGSvc"))

    acc.merge(DetectorGeometrySvcCfg(flags))
    kwargs.setdefault("DetGeoSvc", acc.getService("DetectorGeometrySvc"))

    # Only add it if it's not added already
    if "InputConverter" not in kwargs.keys():
        acc.merge(InputConverterCfg(flags))
        kwargs.setdefault("InputConverter", acc.getService("ISF_InputConverter"))

    if "UserActionSvc" not in kwargs.keys():
        acc.merge(ISFUserActionSvcCfg(flags))
        kwargs.setdefault("UserActionSvc", acc.getService("G4UA::ISFUserActionSvc"))

    kwargs.setdefault("RecordFlux", flags.Sim.RecordFlux)

    kwargs.setdefault("MultiThreading", flags.Concurrency.NumThreads > 0)
    # Set commands for the G4AtlasAlg
    kwargs.setdefault("G4Commands", flags.Sim.G4Commands)
    kwargs.setdefault("PrintTimingInfo", flags.Sim.ISF.DoTimeMonitoring)
    if "SenDetMasterTool" not in kwargs:
        tool = acc.popToolsAndMerge(SensitiveDetectorMasterToolCfg(flags))
        acc.addPublicTool(tool)
        kwargs.setdefault("SenDetMasterTool", acc.getPublicTool(tool.name))
    if "FastSimMasterTool" not in kwargs:
        tool = acc.popToolsAndMerge(FastSimulationMasterToolCfg(flags))
        acc.addPublicTool(tool)
        kwargs.setdefault("FastSimMasterTool", acc.getPublicTool(tool.name))

    #PhysicsListSvc
    acc.merge( PhysicsListSvcCfg(flags) )
    kwargs.setdefault("PhysicsListSvc", acc.getService( "PhysicsListSvc") )

    # Workaround to keep other simulation flavours working while we migrate everything to be AthenaMT-compatible.
    if flags.Sim.ISF.Simulator in ["FullG4", "FullG4MT", "PassBackG4", "PassBackG4MT", "G4FastCalo", "G4FastCaloMT"]:
        acc.setPrivateTools(CompFactory.iGeant4.G4TransportTool(name, **kwargs))
    else:
        acc.merge(G4RunManagerHelperCfg(flags))
        kwargs.setdefault("G4RunManagerHelper", acc.getPublicTool("ISF_G4RunManagerHelper"))
        acc.setPrivateTools(CompFactory.iGeant4.G4LegacyTransportTool(name, **kwargs))
    return acc


def FullGeant4ToolCfg(flags, name="ISF_FullGeant4Tool", **kwargs):
    acc = ISFFullUserActionSvcCfg(flags)
    kwargs.setdefault("UserActionSvc", acc.getService("G4UA::ISFFullUserActionSvc"))
    FullGeant4Tool = acc.popToolsAndMerge(Geant4ToolCfg(flags, name, **kwargs))
    acc.setPrivateTools(FullGeant4Tool)
    return acc


def PassBackGeant4ToolCfg(flags, name="ISF_PassBackGeant4Tool", **kwargs):
    acc = ISFPassBackUserActionSvcCfg(flags)
    kwargs.setdefault("UserActionSvc", acc.getService("G4UA::ISFPassBackUserActionSvc"))
    PassBackGeant4Tool = acc.popToolsAndMerge(Geant4ToolCfg(flags, name, **kwargs))
    acc.setPrivateTools(PassBackGeant4Tool)
    return acc


def AFIIGeant4ToolCfg(flags, name="ISF_AFIIGeant4Tool", **kwargs):
    acc = ISF_AFIIUserActionSvcCfg(flags)
    kwargs.setdefault("UserActionSvc", acc.getService("G4UA::ISF_AFIIUserActionSvc"))
    PassBackGeant4Tool = acc.popToolsAndMerge(Geant4ToolCfg(flags, name, **kwargs))
    acc.setPrivateTools(PassBackGeant4Tool)
    return acc


def LongLivedGeant4ToolCfg(flags, name="ISF_LongLivedGeant4Tool", **kwargs):
    acc = LongLivedInputConverterCfg(flags)
    kwargs.setdefault("InputConverter", acc.getService("ISF_LongLivedInputConverter"))
    FullGeant4Tool = acc.popToolsAndMerge(FullGeant4ToolCfg(flags, name, **kwargs))
    acc.setPrivateTools(FullGeant4Tool)
    return acc


def AFII_QS_Geant4ToolCfg(flags, name="AFII_QS_Geant4Tool", **kwargs):
    acc = LongLivedInputConverterCfg(flags)
    kwargs.setdefault("InputConverter", acc.getService("ISF_LongLivedInputConverter"))
    AFIIGeant4Tool = acc.popToolsAndMerge(AFIIGeant4ToolCfg(flags, name, **kwargs))
    acc.setPrivateTools(AFIIGeant4Tool)
    return acc
