#
#  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
#

from AthenaConfiguration.ComponentAccumulator import ComponentAccumulator


def TauDQAPrimitiveTauSelectionToolCfg(flags, **kwargs):
    acc = ComponentAccumulator()

    kwargs.setdefault("ConfigPath", "")
    from ROOT import TauAnalysisTools
    selectioncuts = TauAnalysisTools.SelectionCuts 
    kwargs.setdefault("SelectionCuts", int(selectioncuts.CutAbsEta | selectioncuts.CutAbsCharge | selectioncuts.CutNTrack))
    kwargs.setdefault("PtMin", 0.0)
    kwargs.setdefault("JetIDWP", TauAnalysisTools.JETIDNONE)
    kwargs.setdefault("NTracks", (0, 1, 2, 3, 4, 5))
    kwargs.setdefault("AbsCharges", (0, 1, 2, 3))
    kwargs.setdefault("AbsEtaRegion", (0.0, 10.0))
    from TauAnalysisTools.TauAnalysisToolsConfig import TauSelectionToolCfg
    primaryseltool = acc.popToolsAndMerge(TauSelectionToolCfg(flags, "PrimativeTauSelectionTool", **kwargs))

    return primaryseltool

def TauDQANominalTauSelectionToolCfg(flags, **kwargs):
    acc = ComponentAccumulator()

    kwargs.setdefault("ConfigPath", "")
    from ROOT import TauAnalysisTools
    selectioncuts = TauAnalysisTools.SelectionCuts 
    kwargs.setdefault("SelectionCuts", int(selectioncuts.CutPt | selectioncuts.CutAbsEta | selectioncuts.CutAbsCharge | selectioncuts.CutNTrack))
    kwargs.setdefault("PtMin", 20.0)
    kwargs.setdefault("JetIDWP", TauAnalysisTools.JETIDNONE)
    kwargs.setdefault("NTracks", (0, 1, 2, 3, 4, 5))
    kwargs.setdefault("AbsCharges", (0, 1, 2, 3))
    kwargs.setdefault("AbsEtaRegion", (0.0, 1.37, 1.52, 2.5))
    from TauAnalysisTools.TauAnalysisToolsConfig import TauSelectionToolCfg
    nominalseltool = acc.popToolsAndMerge(TauSelectionToolCfg(flags, "NominalTauSelectionTool", **kwargs))

    return nominalseltool

def TauDQATauTruthMatchingToolCfg(flags, **kwargs):
    acc = ComponentAccumulator()
    
    kwargs.setdefault("TruthElectronContainerName", "TruthElectrons")
    kwargs.setdefault("TruthMuonContainerName", "MuonTruthParticles")
    kwargs.setdefault("WriteTruthTaus", True)
    from TauAnalysisTools.TauAnalysisToolsConfig import TauTruthMatchingToolCfg
    matchingtool = acc.popToolsAndMerge(TauTruthMatchingToolCfg(flags, "TauTruthMatchingTool", **kwargs))

    return matchingtool


