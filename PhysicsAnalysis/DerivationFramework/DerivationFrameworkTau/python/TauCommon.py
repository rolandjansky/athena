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

# VeryLoose
DFCommonTausSelectorVeryLoose = TauAnalysisTools__TauSelectionTool(name="DFCommonTausSelectorVeryLoose")
DFCommonTausSelectorVeryLoose.JetIDWP = 19
DFCommonTausSelectorVeryLoose.SelectionCuts = 1<<6
DFCommonTausSelectorVeryLoose.ConfigPath = ''
#TauAnalysisTools::CutJetIDWP should be used but issue with the dictionnary
ToolSvc += DFCommonTausSelectorVeryLoose
DFCommonTausVeryLooseWrapper = DerivationFramework__AsgSelectionToolWrapper( name = "DFCommonTausVeryLooseWrapper",
                                                                         AsgSelectionTool = DFCommonTausSelectorVeryLoose,
                                                                         StoreGateEntryName   = "DFCommonTausVeryLoose",
                                                                         ContainerName        = "TauJets")
ToolSvc += DFCommonTausVeryLooseWrapper
dftaulog.info(DFCommonTausVeryLooseWrapper)
DFCommonTauWrapperTools.append(DFCommonTausVeryLooseWrapper)

# Loose
DFCommonTausSelectorLoose = TauAnalysisTools__TauSelectionTool(name="DFCommonTausSelectorLoose")
DFCommonTausSelectorLoose.JetIDWP = 20
DFCommonTausSelectorLoose.SelectionCuts = 1<<6
DFCommonTausSelectorLoose.ConfigPath = ''
#TauAnalysisTools::CutJetIDWP should be used but issue with the dictionnary
ToolSvc += DFCommonTausSelectorLoose
DFCommonTausLooseWrapper = DerivationFramework__AsgSelectionToolWrapper( name = "DFCommonTausLooseWrapper",
                                                                         AsgSelectionTool = DFCommonTausSelectorLoose,
                                                                         StoreGateEntryName   = "DFCommonTausLoose",
                                                                         ContainerName        = "TauJets")
ToolSvc += DFCommonTausLooseWrapper
dftaulog.info(DFCommonTausLooseWrapper)
DFCommonTauWrapperTools.append(DFCommonTausLooseWrapper)

# Medium
DFCommonTausSelectorMedium = TauAnalysisTools__TauSelectionTool(name="DFCommonTausSelectorMedium")
DFCommonTausSelectorMedium.JetIDWP = 21
DFCommonTausSelectorMedium.SelectionCuts = 1<<6
DFCommonTausSelectorMedium.ConfigPath = ''
ToolSvc += DFCommonTausSelectorMedium
DFCommonTausMediumWrapper = DerivationFramework__AsgSelectionToolWrapper( name = "DFCommonTausMediumWrapper",
                                                                         AsgSelectionTool = DFCommonTausSelectorMedium,
                                                                         StoreGateEntryName   = "DFCommonTausMedium",
                                                                         ContainerName        = "TauJets")
ToolSvc += DFCommonTausMediumWrapper
dftaulog.info(DFCommonTausMediumWrapper)
DFCommonTauWrapperTools.append(DFCommonTausMediumWrapper)

# Tight
DFCommonTausSelectorTight = TauAnalysisTools__TauSelectionTool(name="DFCommonTausSelectorTight")
DFCommonTausSelectorTight.JetIDWP = 22
DFCommonTausSelectorTight.SelectionCuts = 1<<6
DFCommonTausSelectorTight.ConfigPath = ''
ToolSvc += DFCommonTausSelectorTight
DFCommonTausTightWrapper = DerivationFramework__AsgSelectionToolWrapper( name = "DFCommonTausTightWrapper",
                                                                         AsgSelectionTool = DFCommonTausSelectorTight,
                                                                         StoreGateEntryName   = "DFCommonTausTight",
                                                                         ContainerName        = "TauJets")
ToolSvc += DFCommonTausTightWrapper
dftaulog.info(DFCommonTausTightWrapper)
DFCommonTauWrapperTools.append(DFCommonTausTightWrapper)


#=======================================
# CREATE THE DERIVATION KERNEL ALGORITHM
#=======================================

from DerivationFrameworkCore.DerivationFrameworkCoreConf import DerivationFramework__CommonAugmentation
DerivationFrameworkJob += CfgMgr.DerivationFramework__CommonAugmentation("TauCommonKernel",
                                                                         AugmentationTools = DFCommonTauWrapperTools)

#=================                                                                                                                                                                                                                           # ADD TAU THINNING                                                                                                                                                                                                                           #=================                                                                                                                                                                                                                           

def ThinTau(Name=None, ThinningService=None, Seq=None, SelectionString=None):

    if not Name or not ThinningService or not Seq:
        dftaulog.info("ABORT MISSION !!")
        return

    if not SelectionString:
        SelectionString = "(TauJets.ptFinalCalib >= 15.*GeV) && (TauJets.nTracks<4 && TauJets.nTracks>0)"

    from AthenaCommon.AppMgr import ToolSvc

    # TauJets thinning                                                                                                                                                                                                                        
    from DerivationFrameworkTools.DerivationFrameworkToolsConf import DerivationFramework__GenericObjectThinning
    TauJetsThinningTool = DerivationFramework__GenericObjectThinning(name            = Name+"TauJetsThinningTool",
                                                                     ThinningService = ThinningService,
                                                                     ContainerName   = "TauJets",
                                                                     SelectionString = SelectionString)
    ToolSvc += TauJetsThinningTool
    dftaulog.info(TauJetsThinningTool)

    # Only keep tau tracks (and associated ID tracks) classified as charged tracks                                                                                                                                                            
    from DerivationFrameworkInDet.DerivationFrameworkInDetConf import DerivationFramework__TauTrackParticleThinning
    TauTPThinningTool = DerivationFramework__TauTrackParticleThinning(name                   = Name+"TauTPThinningTool",
                                                                      ThinningService        = ThinningService,
                                                                      TauKey                 = "TauJets",
                                                                      InDetTrackParticlesKey = "InDetTrackParticles",
                                                                      SelectionString        = SelectionString,
                                                                      ApplyAnd               = False,
                                                                      DoTauTracksThinning    = True,
                                                                      TauTracksKey           = "TauTracks")
    ToolSvc += TauTPThinningTool
    dftaulog.info(TauTPThinningTool)


    from DerivationFrameworkCore.DerivationFrameworkCoreConf import DerivationFramework__DerivationKernel
    Seq += CfgMgr.DerivationFramework__DerivationKernel(Name+"TauThinningKernel", ThinningTools = [TauJetsThinningTool,TauTPThinningTool])


#=======================================
# Low-pt di-tau reconstruction
#=======================================
def addDiTauLowPt():

    from AthenaCommon.AppMgr import ToolSvc
    if hasattr(ToolSvc,"DFCommonDiTauLowPtBuilder"):
        return

    from DerivationFrameworkJetEtMiss.JetCommon import addCHSPFlowObjects
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
