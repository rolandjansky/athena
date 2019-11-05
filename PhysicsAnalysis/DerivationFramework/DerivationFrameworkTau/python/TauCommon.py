# Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration

#********************************************************************
# TauCommon.py
# Schedules all tools needed for tau object selection and writes
# results into SG. These may then be accessed along the train.
#********************************************************************
from DerivationFrameworkCore.DerivationFrameworkMaster import *
from AthenaCommon import Logging
dftaulog = Logging.logging.getLogger('TauCommon')

#====================================================================
# MAKE TAU ENUMS AVAILABLE
#====================================================================
#from ROOT import xAOD__TauJetParameters__IsTauFlag

#====================================================================
# AUGMENTATION TOOLS
#====================================================================
from DerivationFrameworkTools.DerivationFrameworkToolsConf import DerivationFramework__AsgSelectionToolWrapper
from TauAnalysisTools.TauAnalysisToolsConf import TauAnalysisTools__TauSelectionTool

from tauRec.TauRecAODBuilder import TauRecAODProcessor_FTau
TauRecAODProcessor_FTau()

DFCommonTauWrapperTools = []

# Loose
DFCommonTausSelectorLoose = TauAnalysisTools__TauSelectionTool(name="DFCommonTausSelectorLoose")
DFCommonTausSelectorLoose.JetIDWP = 2
DFCommonTausSelectorLoose.SelectionCuts = 1<<6
#TauAnalysisTools::CutJetIDWP should be used but issue with the dictionnary
ToolSvc += DFCommonTausSelectorLoose
DFCommonTausLooseWrapper = DerivationFramework__AsgSelectionToolWrapper( name = "DFCommonTausLooseWrapper",
                                                                         AsgSelectionTool = DFCommonTausSelectorLoose,
                                                                         StoreGateEntryName   = "DFCommonTausLoose",
                                                                         ContainerName        = "TauJets")
ToolSvc += DFCommonTausLooseWrapper
print DFCommonTausLooseWrapper
DFCommonTauWrapperTools.append(DFCommonTausLooseWrapper)

# Medium
DFCommonTausSelectorMedium = TauAnalysisTools__TauSelectionTool(name="DFCommonTausSelectorMedium")
DFCommonTausSelectorMedium.JetIDWP = 3
DFCommonTausSelectorMedium.SelectionCuts = 1<<6
ToolSvc += DFCommonTausSelectorMedium
DFCommonTausMediumWrapper = DerivationFramework__AsgSelectionToolWrapper( name = "DFCommonTausMediumWrapper",
                                                                         AsgSelectionTool = DFCommonTausSelectorMedium,
                                                                         StoreGateEntryName   = "DFCommonTausMedium",
                                                                         ContainerName        = "TauJets")
ToolSvc += DFCommonTausMediumWrapper
print DFCommonTausMediumWrapper
DFCommonTauWrapperTools.append(DFCommonTausMediumWrapper)

# Tight
DFCommonTausSelectorTight = TauAnalysisTools__TauSelectionTool(name="DFCommonTausSelectorTight")
DFCommonTausSelectorTight.JetIDWP = 4
DFCommonTausSelectorTight.SelectionCuts = 1<<6
ToolSvc += DFCommonTausSelectorTight
DFCommonTausTightWrapper = DerivationFramework__AsgSelectionToolWrapper( name = "DFCommonTausTightWrapper",
                                                                         AsgSelectionTool = DFCommonTausSelectorTight,
                                                                         StoreGateEntryName   = "DFCommonTausTight",
                                                                         ContainerName        = "TauJets")
ToolSvc += DFCommonTausTightWrapper
print DFCommonTausTightWrapper
DFCommonTauWrapperTools.append(DFCommonTausTightWrapper)

#=======================================
# CREATE THE DERIVATION KERNEL ALGORITHM
#=======================================

from DerivationFrameworkCore.DerivationFrameworkCoreConf import DerivationFramework__CommonAugmentation
DerivationFrameworkJob += CfgMgr.DerivationFramework__CommonAugmentation("TauCommonKernel",
                                                                         AugmentationTools = DFCommonTauWrapperTools)



#=======================================
# Low-pt di-tau reconstruction
#=======================================
def addDiTauLowPt():

    from AthenaCommon.AppMgr import ToolSvc
    if hasattr(ToolSvc,"DFCommonDiTauLowPtBuilder"):
        return

    from DerivationFrameworkJetEtMiss.ExtendedJetCommon import addCHSPFlowObjects
    addCHSPFlowObjects()

    from DerivationFrameworkCore.DerivationFrameworkMaster import DerivationFrameworkJob
    from DerivationFrameworkJetEtMiss.JetCommon import addStandardJets
    addStandardJets("AntiKt", 1.0, "EMPFlow", ptmin=40000, ptminFilter=50000, mods="pflow_ungroomed", calibOpt="none", algseq=DerivationFrameworkJob, outputGroup="TauCommon")

    import DiTauRec.DiTauAlgorithmsHolder as DiTauAlgs
    from DiTauRec.DiTauRecConf import DiTauBuilder
    
    ditauTools = []
    ditauTools.append(DiTauAlgs.getSeedJetBuilder("AntiKt10EMPFlowJets"))
    ditauTools.append(DiTauAlgs.getElMuFinder())
    ditauTools.append(DiTauAlgs.getSubjetBuilder())
    ditauTools.append(DiTauAlgs.getVertexFinder())
    ditauTools.append(DiTauAlgs.getDiTauTrackFinder())
    ditauTools.append(DiTauAlgs.getIDVarCalculator(False))

    for tool in ditauTools:
        if not hasattr(ToolSvc,tool.name()):
            ToolSvc += tool            
    
    DiTauLowPtBuilder = DiTauBuilder(name="DFCommonDiTauLowPtBuilder",
                                     DiTauContainer="DiTauJetsLowPt",
                                     DiTauAuxContainer="DiTauJetsLowPtAux.",
                                     SeedJetName="AntiKt10EMPFlowJets",
                                     minPt=50000,
                                     maxEta=2.5,
                                     Rjet=1.0,
                                     Rsubjet=0.2,
                                     Rcore=0.1,
                                     Tools=ditauTools)
    ToolSvc += DiTauLowPtBuilder
    DerivationFrameworkJob += DiTauLowPtBuilder

    dftaulog.info( "Low-pt di-tau building has been scheduled")
