# Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
from AthenaConfiguration.ComponentAccumulator import ComponentAccumulator
from AthenaConfiguration.ComponentFactory import CompFactory
from DiTauRec.DiTauToolsConfig import SeedJetBuilderCfg, SubjetBuilderCfg, ElMuFinderCfg, JetAlgCfg, VertexFinderCfg, DiTauTrackFinderCfg, CellFinderCfg, IDVarCalculatorCfg

def DiTauBuilderCfg(flags, name="DiTauBuilder", **kwargs):
    acc = ComponentAccumulator()

    tools = [
        acc.popToolsAndMerge(SeedJetBuilderCfg(flags, JetCollection="AntiKt10LCTopoJets")),
        acc.popToolsAndMerge(ElMuFinderCfg(flags)),
        acc.popToolsAndMerge(SubjetBuilderCfg(flags))
    ]

    if flags.InDet.PriVertex.doVertexFinding: # Simplified wrt old config
        acc.merge(JetAlgCfg(flags)) # To run TVA tool for VertexFinder
        tools.append(acc.popToolsAndMerge(VertexFinderCfg(flags)))

    tools.append(acc.popToolsAndMerge(DiTauTrackFinderCfg(flags)))
    tools.append(acc.popToolsAndMerge(CellFinderCfg(flags)))
    tools.append(acc.popToolsAndMerge(IDVarCalculatorCfg(flags)))

    kwargs.setdefault("DiTauContainer", "DiTauJets")
    kwargs.setdefault("Tools", tools)
    kwargs.setdefault("SeedJetName", "AntiKt10LCTopoJets")
    kwargs.setdefault("minPt", 300000) # diTauFlags.diTauRecJetSeedPt
    kwargs.setdefault("maxEta", 2.5)
    kwargs.setdefault("Rjet", 1.0)
    kwargs.setdefault("Rsubjet", 0.2)
    kwargs.setdefault("Rcore", 0.1)

    acc.addEventAlgo(CompFactory.DiTauBuilder(name, **kwargs))
    return acc
        

def DiTauBuilderLowPtCfg(flags, name="DiTauLowPtBuilder", **kwargs):

    acc = ComponentAccumulator()

    tools = [
        acc.popToolsAndMerge(SeedJetBuilderCfg(flags, JetCollection="AntiKt10EMPFlowJets")),
        acc.popToolsAndMerge(ElMuFinderCfg(flags)),
        acc.popToolsAndMerge(SubjetBuilderCfg(flags))
    ]

    if flags.InDet.PriVertex.doVertexFinding: # Simplified wrt old config
        acc.merge(JetAlgCfg(flags)) # To run TVA tool for VertexFinder
        tools.append(acc.popToolsAndMerge(VertexFinderCfg(flags)))

    tools.append(acc.popToolsAndMerge(DiTauTrackFinderCfg(flags)))
    # No CellFinder as run in derivation
    tools.append(acc.popToolsAndMerge(IDVarCalculatorCfg(flags)))

    kwargs.setdefault("DiTauContainer", "DiTauJetsLowPt")
    kwargs.setdefault("Tools", tools)
    kwargs.setdefault("SeedJetName", "AntiKt10EMPFlowJets")
    kwargs.setdefault("minPt", 50000)
    
    acc.merge(DiTauBuilderCfg(flags, name, **kwargs))
    return acc
