"""Geant4 tools config for ISF with ComponentAccumulator

Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
"""
from AthenaConfiguration.ComponentAccumulator import ComponentAccumulator
from AthenaConfiguration.ComponentFactory import CompFactory
from RngComps.RandomServices import AthRNGSvcCfg
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


def G4RunManagerHelperCfg(flags, name="G4RunManagerHelper", **kwargs):
    acc = ComponentAccumulator()
    acc.setPrivateTools(CompFactory.iGeant4.G4RunManagerHelper(name, **kwargs))
    return acc


def Geant4ToolCfg(flags, name="ISF_Geant4Tool", **kwargs):
    acc = ComponentAccumulator()
    kwargs.setdefault("DetGeoSvc", acc.getPrimaryAndMerge(DetectorGeometrySvcCfg(flags)).name)

    kwargs.setdefault("RandomNumberService", acc.getPrimaryAndMerge(AthRNGSvcCfg(flags)).name)

    # Only add it if it's not added already
    if "InputConverter" not in kwargs.keys():
        kwargs.setdefault("InputConverter", acc.getPrimaryAndMerge(InputConverterCfg(flags)).name)

    if "UserActionSvc" not in kwargs.keys():
        kwargs.setdefault("UserActionSvc", acc.getPrimaryAndMerge(ISFUserActionSvcCfg(flags)).name)

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

    # PhysicsListSvc
    kwargs.setdefault("PhysicsListSvc", acc.getPrimaryAndMerge(PhysicsListSvcCfg(flags)).name)

    # Workaround to keep other simulation flavours working while we migrate everything to be AthenaMT-compatible.
    if flags.Sim.ISF.Simulator in ["FullG4MT", "FullG4MT_QS", "PassBackG4MT", "ATLFAST3MT", "ATLFAST3MT_QS"]:
        acc.setPrivateTools(CompFactory.iGeant4.G4TransportTool(name, **kwargs))
    else:
        tool = acc.popToolsAndMerge(G4RunManagerHelperCfg(flags))
        acc.addPublicTool(tool)
        kwargs.setdefault("G4RunManagerHelper", acc.getPublicTool(tool.name))
        acc.setPrivateTools(CompFactory.iGeant4.G4LegacyTransportTool(name, **kwargs))
    return acc


def FullGeant4ToolCfg(flags, name="ISF_FullGeant4Tool", **kwargs):
    acc = ComponentAccumulator()
    kwargs.setdefault("UserActionSvc", acc.getPrimaryAndMerge(ISFFullUserActionSvcCfg(flags)).name)
    FullGeant4Tool = acc.popToolsAndMerge(Geant4ToolCfg(flags, name, **kwargs))
    acc.setPrivateTools(FullGeant4Tool)
    return acc


def PassBackGeant4ToolCfg(flags, name="ISF_PassBackGeant4Tool", **kwargs):
    acc = ComponentAccumulator()
    kwargs.setdefault("UserActionSvc", acc.getPrimaryAndMerge(ISFPassBackUserActionSvcCfg(flags)).name)
    PassBackGeant4Tool = acc.popToolsAndMerge(Geant4ToolCfg(flags, name, **kwargs))
    acc.setPrivateTools(PassBackGeant4Tool)
    return acc


def AFIIGeant4ToolCfg(flags, name="ISF_AFIIGeant4Tool", **kwargs):
    acc = ComponentAccumulator()
    kwargs.setdefault("UserActionSvc", acc.getPrimaryAndMerge(ISF_AFIIUserActionSvcCfg(flags)).name)
    PassBackGeant4Tool = acc.popToolsAndMerge(Geant4ToolCfg(flags, name, **kwargs))
    acc.setPrivateTools(PassBackGeant4Tool)
    return acc


def LongLivedGeant4ToolCfg(flags, name="ISF_LongLivedGeant4Tool", **kwargs):
    acc = ComponentAccumulator()
    kwargs.setdefault("InputConverter", acc.getPrimaryAndMerge(LongLivedInputConverterCfg(flags)).name)
    FullGeant4Tool = acc.popToolsAndMerge(FullGeant4ToolCfg(flags, name, **kwargs))
    acc.setPrivateTools(FullGeant4Tool)
    return acc


def AFII_QS_Geant4ToolCfg(flags, name="AFII_QS_Geant4Tool", **kwargs):
    acc = ComponentAccumulator()
    kwargs.setdefault("InputConverter", acc.getPrimaryAndMerge(LongLivedInputConverterCfg(flags)).name)
    AFIIGeant4Tool = acc.popToolsAndMerge(AFIIGeant4ToolCfg(flags, name, **kwargs))
    acc.setPrivateTools(AFIIGeant4Tool)
    return acc
