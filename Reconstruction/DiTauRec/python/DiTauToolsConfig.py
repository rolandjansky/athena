# Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration

from AthenaConfiguration.ComponentAccumulator import ComponentAccumulator
from AthenaConfiguration.ComponentFactory import CompFactory

def SeedJetBuilderCfg(ConfigFlags, name="DiTauRec_SeedJetBuilder", **kwargs):
    """Configure the seed jet builder"""
    acc = ComponentAccumulator()
    acc.setPrivateTools(CompFactory.SeedJetBuilder(name, **kwargs))
    return acc


def SubjetBuilderCfg(ConfigFlags, name="DiTauRec_SubjetBuilder", **kwargs):
    """Configure the subjet builder"""
    acc = ComponentAccumulator()

    kwargs.setdefault("Rsubjet", 0.2)
    kwargs.setdefault("Nsubjet", 4)
    kwargs.setdefault("ptminsubjet", 10000)

    acc.setPrivateTools(CompFactory.SubjetBuilder(name, **kwargs))
    return acc


def ElMuFinderCfg(ConfigFlags, name = "DiTauRec_ElMuFinder", **kwargs):
    """Configure the ElMuFinder"""
    acc = ComponentAccumulator()

    kwargs.setdefault("ElectronContainer", "Electrons")
    kwargs.setdefault("MuonContainer", "Muons")
    kwargs.setdefault("ElectronMinPt", 7000)
    kwargs.setdefault("ElectronMaxEta", 2.47)
    kwargs.setdefault("MuonMinPt", 7000)
    kwargs.setdefault("MuonMaxEta", 2.7)
    kwargs.setdefault("MuonQuality", 3)   # Tight,Medium,Loose,VeryLoose: 0,1,2,3

    acc.setPrivateTools(CompFactory.ElMuFinder(name, **kwargs))
    return acc

def TVAToolCfg(ConfigFlags, name="TVATool_forDiTaus", **kwargs):
    """Configure the TVA tool"""
    acc = ComponentAccumulator()

    kwargs.setdefault("TrackParticleContainer", "InDetTrackParticles")
    kwargs.setdefault("TrackVertexAssociation", "JetTrackVtxAssoc_forDiTaus")
    kwargs.setdefault("VertexContainer", "PrimaryVertices")
    kwargs.setdefault("MaxTransverseDistance", 2.5) # in mm
    kwargs.setdefault("MaxLongitudinalDistance", 2) # in mm

    acc.setPrivateTools(CompFactory.TrackVertexAssociationTool(name, **kwargs))
    return acc

def JetAlgCfg(ConfigFlags, name="DiTauRec_JetAlgorithm", **kwargs): # Name changed wrt legacy config DiTauRec_TVATool
    """Configure the JetAlgorithm"""
    acc = ComponentAccumulator()

    tools = [acc.popToolsAndMerge(TVAToolCfg(ConfigFlags))]
    kwargs.setdefault("Tools", tools)

    acc.addEventAlgo(CompFactory.JetAlgorithm(name, **kwargs))
    return acc

# require TrackVertexAssociation to be produced by TVA tool - see above
def VertexFinderCfg(ConfigFlags, name="DiTauRec_VertexFinder", **kwargs):
    """Configure the vertex finder"""
    acc = ComponentAccumulator()

    kwargs.setdefault("PrimVtxContainerName", "PrimaryVertices")
    kwargs.setdefault("AssociatedTracks", "GhostTrack")
    kwargs.setdefault("TrackVertexAssociation", "JetTrackVtxAssoc_forDiTaus")

    acc.setPrivateTools(CompFactory.VertexFinder(name, **kwargs))
    return acc

def DiTauTrackFinderCfg(ConfigFlags, name="DiTauRec_DiTauTrackFinder", **kwargs):
    """Configure the di-tau track finder"""    
    acc = ComponentAccumulator()
    
    kwargs.setdefault("MaxDrJet", 1.0)
    kwargs.setdefault("MaxDrSubjet", 0.2)
    kwargs.setdefault("TrackParticleContainer", "InDetTrackParticles")

    if "TrackSelectorTool" not in kwargs:
        from tauRec.TauToolHolder import InDetTrackSelectorToolCfg
        InDetTrackSelectorTool = acc.popToolsAndMerge(InDetTrackSelectorToolCfg(ConfigFlags))
        acc.addPublicTool(InDetTrackSelectorTool)
        kwargs.setdefault("TrackSelectorTool", InDetTrackSelectorTool)

    acc.setPrivateTools(CompFactory.DiTauTrackFinder(name, **kwargs))
    return acc

def CellFinderCfg(ConfigFlags, name="DiTauRec_CellFinder", **kwargs):
    """Configure the cell finder"""
    acc = ComponentAccumulator()

    kwargs.setdefault("ClusterContainer", "CaloCalTopoClusters")
    kwargs.setdefault("CellContainer", "AllCalo")
    kwargs.setdefault("Rsubjet", 0.2)

    CellFinder = CompFactory.CellFinder(name, **kwargs)
    CellFinder.writeSubjetCells = CellFinder.writeJetCells
    acc.setPrivateTools(CellFinder)
    return acc


def IDVarCalculatorCfg(ConfigFlags, name="DiTauRec_IDVarCalculator", **kwargs):
    """Configure the IDVarCalculator"""
    acc = ComponentAccumulator()
    acc.setPrivateTools(CompFactory.IDVarCalculator(name, **kwargs))
    return acc
