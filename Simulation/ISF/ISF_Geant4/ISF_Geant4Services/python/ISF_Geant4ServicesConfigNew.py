"""Geant4 services config for ISF with ComponentAccumulator

Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
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
    G4_DDDBEnvelopeDefSvc.DBInDetNode = "InDetEnvelopeG4"
    G4_DDDBEnvelopeDefSvc.DBCaloNode = "CaloEnvelopeG4"
    G4_DDDBEnvelopeDefSvc.DBMSNode = "MuonEnvelopeG4"
    G4_DDDBEnvelopeDefSvc.DBCavernNode = "CavernEnvelopeG4"
    acc.addService(G4_DDDBEnvelopeDefSvc)

    if "SimulatorTool" not in kwargs:
        acc.merge(Geant4ToolCfg(flags))
        kwargs.setdefault("SimulatorTool", acc.getPublicTool("ISF_Geant4Tool"))
    kwargs.setdefault("Identifier", "Geant4")
    Geant4SimService = CompFactory.iGeant4.Geant4SimSvc(name, **kwargs)
    acc.addService(Geant4SimService)
    return acc


def FullGeant4SimCfg(flags, name="ISF_FullGeant4SimSvc", **kwargs):
    acc = FullGeant4ToolCfg(flags)
    kwargs.setdefault("SimulatorTool", acc.getPublicTool("ISF_FullGeant4Tool"))
    acc.merge(Geant4SimCfg(flags, name, **kwargs))
    return acc


def LongLivedGeant4SimCfg(flags, name="ISF_LongLivedGeant4SimSvc", **kwargs):
    acc = LongLivedGeant4ToolCfg(flags)
    kwargs.setdefault("SimulatorTool", acc.getPublicTool("ISF_LongLivedGeant4Tool"))
    acc.merge(FullGeant4SimCfg(flags, name, **kwargs))
    return acc


def PassBackGeant4SimCfg(flags, name="ISF_PassBackGeant4SimSvc", **kwargs):
    acc = PassBackGeant4ToolCfg(flags)
    kwargs.setdefault("SimulatorTool", acc.getPublicTool("ISF_PassBackGeant4Tool"))
    acc.merge(Geant4SimCfg(flags, name, **kwargs))
    return acc


def AFIIGeant4SimCfg(flags, name="ISF_AFIIGeant4SimSvc", **kwargs):
    acc = AFIIGeant4ToolCfg(flags)
    kwargs.setdefault("SimulatorTool", acc.getPublicTool("ISF_AFIIGeant4Tool"))
    acc.merge(PassBackGeant4SimCfg(flags, name, **kwargs))
    return acc


def AFII_QS_Geant4SimCfg(flags, name="ISF_AFII_QS_Geant4SimSvc", **kwargs):
    acc = AFII_QS_Geant4ToolCfg(flags)
    kwargs.setdefault("SimulatorTool", acc.getPublicTool("AFII_QS_Geant4Tool"))
    acc.merge(PassBackGeant4SimCfg(flags, name, **kwargs))
    return acc
