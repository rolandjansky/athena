"""Geant4 services config for ISF with ComponentAccumulator

Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
"""
from AthenaConfiguration.ComponentAccumulator import ComponentAccumulator
from AthenaConfiguration.ComponentFactory import CompFactory
from ISF_Geant4Tools.ISF_Geant4ToolsConfigNew import (
    Geant4ToolCfg, FullGeant4ToolCfg, LongLivedGeant4ToolCfg, PassBackGeant4ToolCfg,
    AFIIGeant4ToolCfg, AFII_QS_Geant4ToolCfg
)


def Geant4SimCfg(flags, name="ISFG4SimSvc", **kwargs):
    acc = ComponentAccumulator()

    G4_DDDBEnvelopeDefSvc = CompFactory.DetDescrDBEnvelopeSvc("G4EnvelopeDefSvc")
    G4_DDDBEnvelopeDefSvc.DBBeamPipeNode = "BPipeEnvelopeG4"
    G4_DDDBEnvelopeDefSvc.DBInDetNode = "InDetEnvelopeG4" if flags.GeoModel.Run in ['RUN1', 'RUN2', 'RUN3'] else "ITkEnvelopeG4"
    G4_DDDBEnvelopeDefSvc.DBCaloNode = "CaloEnvelopeG4"
    G4_DDDBEnvelopeDefSvc.DBMSNode = "MuonEnvelopeG4"
    G4_DDDBEnvelopeDefSvc.DBCavernNode = "CavernEnvelopeG4"
    acc.addService(G4_DDDBEnvelopeDefSvc)

    if "SimulatorTool" not in kwargs:
        tool = acc.popToolsAndMerge(Geant4ToolCfg(flags))
        acc.addPublicTool(tool)
        kwargs.setdefault("SimulatorTool", acc.getPublicTool(tool.name))
    kwargs.setdefault("Identifier", "Geant4")
    acc.addService(CompFactory.iGeant4.Geant4SimSvc(name, **kwargs), primary = True)
    return acc


def FullGeant4SimCfg(flags, name="ISF_FullGeant4SimSvc", **kwargs):
    accTool = ComponentAccumulator()
    if "SimulatorTool" not in kwargs:
        tool = accTool.popToolsAndMerge(FullGeant4ToolCfg(flags))
        accTool.addPublicTool(tool)
        kwargs.setdefault("SimulatorTool", accTool.getPublicTool(tool.name))
    acc = Geant4SimCfg(flags, name, **kwargs)
    acc.merge(accTool)
    return acc


def LongLivedGeant4SimCfg(flags, name="ISF_LongLivedGeant4SimSvc", **kwargs):
    accTool = ComponentAccumulator()
    tool = accTool.popToolsAndMerge(LongLivedGeant4ToolCfg(flags))
    accTool.addPublicTool(tool)
    kwargs.setdefault("SimulatorTool", accTool.getPublicTool(tool.name))
    acc = FullGeant4SimCfg(flags, name, **kwargs)
    acc.merge(accTool)
    return acc


def PassBackGeant4SimCfg(flags, name="ISF_PassBackGeant4SimSvc", **kwargs):
    acc = ComponentAccumulator()
    if "SimulatorTool" not in kwargs:
        tool = acc.popToolsAndMerge(PassBackGeant4ToolCfg(flags))
        acc.addPublicTool(tool)
        kwargs.setdefault("SimulatorTool", acc.getPublicTool(tool.name))
    acc.merge(Geant4SimCfg(flags, name, **kwargs))
    return acc


def AFIIGeant4SimCfg(flags, name="ISF_AFIIGeant4SimSvc", **kwargs):
    accTool = ComponentAccumulator()
    tool = accTool.popToolsAndMerge(AFIIGeant4ToolCfg(flags))
    accTool.addPublicTool(tool)
    kwargs.setdefault("SimulatorTool", accTool.getPublicTool(tool.name))
    acc = PassBackGeant4SimCfg(flags, name, **kwargs)
    acc.merge(accTool)
    return acc


def AFII_QS_Geant4SimCfg(flags, name="ISF_AFII_QS_Geant4SimSvc", **kwargs):
    accTool = ComponentAccumulator()
    tool = accTool.popToolsAndMerge(AFII_QS_Geant4ToolCfg(flags))
    accTool.addPublicTool(tool)
    kwargs.setdefault("SimulatorTool", accTool.getPublicTool(tool.name))
    acc = PassBackGeant4SimCfg(flags, name, **kwargs)
    acc.merge(accTool)
    return acc
