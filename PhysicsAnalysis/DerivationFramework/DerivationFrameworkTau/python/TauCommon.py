# Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration

#********************************************************************
# TauCommon.py
# Schedules all tools needed for tau object selection and writes
# results into SG. These may then be accessed along the train.
#********************************************************************

from __future__ import print_function
from AthenaCommon import CfgMgr 

# will likely be replaced with generic tau decorator tool
#from tauRec.TauRecAODBuilder import TauRecAODProcessor_FTau
#TauRecAODProcessor_FTau()

#====================================================================
# AUGMENTATION TOOLS
#====================================================================
def AddTauAugmentation(Seq=None, doVeryLoose=None, doLoose=None, doMedium=None, doTight=None):

    if not Seq or hasattr(Seq,"TauAugmentationKernel"+Seq.name()):
        print("Tau augmentation will not be scheduled")
        return

    from AthenaCommon.AppMgr import ToolSvc
    from DerivationFrameworkTools.DerivationFrameworkToolsConf import DerivationFramework__AsgSelectionToolWrapper
    from TauAnalysisTools.TauAnalysisToolsConf import TauAnalysisTools__TauSelectionTool
    import PyUtils.RootUtils as ru
    ROOT = ru.import_root()
    import cppyy
    cppyy.load_library('libTauAnalysisTools')

    TauAugmentationTools = []

    if doVeryLoose:
        if not hasattr(ToolSvc,"TauVeryLooseWrapper"):
            TauSelectorVeryLoose = TauAnalysisTools__TauSelectionTool(name="TauSelectorVeryLoose")
            TauSelectorVeryLoose.JetIDWP = ROOT.TauAnalysisTools.e_JETID.JETIDRNNVERYLOOSE
            TauSelectorVeryLoose.SelectionCuts = ROOT.TauAnalysisTools.SelectionCuts.CutJetIDWP
            TauSelectorVeryLoose.ConfigPath = ''
            ToolSvc += TauSelectorVeryLoose

            TauVeryLooseWrapper = DerivationFramework__AsgSelectionToolWrapper(name               = "TauVeryLooseWrapper",
                                                                               AsgSelectionTool   = TauSelectorVeryLoose,
                                                                               StoreGateEntryName = "DFTauVeryLoose",
                                                                               ContainerName      = "TauJets")
            ToolSvc += TauVeryLooseWrapper
        else:
            TauVeryLooseWrapper = getattr(ToolSvc,"TauVeryLooseWrapper")

        print (TauVeryLooseWrapper)
        TauAugmentationTools.append(TauVeryLooseWrapper)
    
    if doLoose:
        if not hasattr(ToolSvc,"TauLooseWrapper"):
            TauSelectorLoose = TauAnalysisTools__TauSelectionTool(name="TauSelectorLoose")
            TauSelectorLoose.JetIDWP = ROOT.TauAnalysisTools.e_JETID.JETIDRNNLOOSE
            TauSelectorLoose.SelectionCuts = ROOT.TauAnalysisTools.SelectionCuts.CutJetIDWP
            TauSelectorLoose.ConfigPath = ''
            ToolSvc += TauSelectorLoose

            TauLooseWrapper = DerivationFramework__AsgSelectionToolWrapper(name               = "TauLooseWrapper",
                                                                           AsgSelectionTool   = TauSelectorLoose,
                                                                           StoreGateEntryName = "DFTauLoose",
                                                                           ContainerName      = "TauJets")
            ToolSvc += TauLooseWrapper
        else:
            TauLooseWrapper = getattr(ToolSvc,"TauLooseWrapper")

        print (TauLooseWrapper)
        TauAugmentationTools.append(TauLooseWrapper)
    
    if doMedium:
        if not hasattr(ToolSvc,"TauMediumWrapper"):
            TauSelectorMedium = TauAnalysisTools__TauSelectionTool(name="TauSelectorMedium")
            TauSelectorMedium.JetIDWP = ROOT.TauAnalysisTools.e_JETID.JETIDRNNMEDIUM
            TauSelectorMedium.SelectionCuts = ROOT.TauAnalysisTools.SelectionCuts.CutJetIDWP
            TauSelectorMedium.ConfigPath = ''
            ToolSvc += TauSelectorMedium

            TauMediumWrapper = DerivationFramework__AsgSelectionToolWrapper(name               = "TauMediumWrapper",
                                                                            AsgSelectionTool   = TauSelectorMedium,
                                                                            StoreGateEntryName = "DFTauMedium",
                                                                            ContainerName      = "TauJets")
            ToolSvc += TauMediumWrapper
        else:
            TauMediumWrapper = getattr(ToolSvc,"TauMediumWrapper")

        print (TauMediumWrapper)
        TauAugmentationTools.append(TauMediumWrapper)
    
    if doTight:
        if not hasattr(ToolSvc,"TauTightWrapper"):
            TauSelectorTight = TauAnalysisTools__TauSelectionTool(name="TauSelectorTight")
            TauSelectorTight.JetIDWP = ROOT.TauAnalysisTools.e_JETID.JETIDRNNTIGHT
            TauSelectorTight.SelectionCuts = ROOT.TauAnalysisTools.SelectionCuts.CutJetIDWP
            TauSelectorTight.ConfigPath = ''
            ToolSvc += TauSelectorTight

            TauTightWrapper = DerivationFramework__AsgSelectionToolWrapper(name               = "TauTightWrapper",
                                                                           AsgSelectionTool   = TauSelectorTight,
                                                                           StoreGateEntryName = "DFTauTight",
                                                                           ContainerName      = "TauJets")
            ToolSvc += TauTightWrapper
        else:
            TauTightWrapper = getattr(ToolSvc,"TauTightWrapper")

        print (TauTightWrapper)
        TauAugmentationTools.append(TauTightWrapper)
            
    if TauAugmentationTools:
        Seq += CfgMgr.DerivationFramework__DerivationKernel("TauAugmentationKernel"+Seq.name(), AugmentationTools = TauAugmentationTools)

#=================
# TAU THINNING
#=================
def ThinTau(Seq=None, streamName=None, selectionString=None):

    if not Seq or not streamName or hasattr(Seq,"TauThinningKernel"+Seq.name()):
        print ("Tau thinning will not be scheduled")
        return

    if not selectionString:
        selectionString = "(TauJets.pt >= 15.*GeV) && (TauJets.nTracks>0 && TauJets.nTracks<4)"

    print ("Adding tau thinning:", selectionString)

    from AthenaCommon.AppMgr import ToolSvc

    # TauJets thinning
    from DerivationFrameworkTools.DerivationFrameworkToolsConf import DerivationFramework__GenericObjectThinning
    TauJetsThinningTool = DerivationFramework__GenericObjectThinning(name            = "TauJetsThinningTool"+Seq.name(),
                                                                     StreamName      = streamName,
                                                                     ContainerName   = "TauJets",
                                                                     SelectionString = selectionString)
    ToolSvc += TauJetsThinningTool
    print (TauJetsThinningTool)

    # Only keep tau tracks (and associated ID tracks) classified as charged tracks
    from DerivationFrameworkInDet.DerivationFrameworkInDetConf import DerivationFramework__TauTrackParticleThinning
    TauTPThinningTool = DerivationFramework__TauTrackParticleThinning(name                   = "TauTPThinningTool"+Seq.name(),
                                                                      StreamName             = streamName,
                                                                      TauKey                 = "TauJets",
                                                                      InDetTrackParticlesKey = "InDetTrackParticles",
                                                                      SelectionString        = selectionString,
                                                                      DoTauTracksThinning    = True,
                                                                      TauTracksKey           = "TauTracks")
    ToolSvc += TauTPThinningTool
    print (TauTPThinningTool)

    Seq += CfgMgr.DerivationFramework__DerivationKernel("TauThinningKernel"+Seq.name(), ThinningTools = [TauJetsThinningTool,TauTPThinningTool])

#=======================================
# Low-pt di-tau reconstruction
#=======================================
def addDiTauLowPt(Seq=None):

    if not Seq or hasattr(Seq,"DiTauLowPtBuilder"+Seq.name()):
        print("Low-pt ditau reconstruction will not be scheduled")
        return

    print ("Adding low-pt di-tau reconstruction")

    from DerivationFrameworkJetEtMiss.JetCommon import addDAODJets
    from JetRecConfig.StandardLargeRJets import AntiKt10LCTopo
    from JetRecConfig.StandardJetConstits import stdConstitDic as cst

    AntiKt10EMPFlow = AntiKt10LCTopo.clone(inputdef = cst.GPFlow)
    jetList = [AntiKt10EMPFlow]

    addDAODJets(jetList,Seq)

    import DiTauRec.DiTauAlgorithmsHolder as DiTauAlgs
    from DiTauRec.DiTauRecConf import DiTauBuilder
    
    ditauTools = []
    ditauTools.append(DiTauAlgs.getSeedJetBuilder("AntiKt10EMPFlowJets"))
    ditauTools.append(DiTauAlgs.getElMuFinder())
    ditauTools.append(DiTauAlgs.getSubjetBuilder())
    Seq += DiTauAlgs.getTVATool()
    ditauTools.append(DiTauAlgs.getVertexFinder())
    ditauTools.append(DiTauAlgs.getDiTauTrackFinder())
    ditauTools.append(DiTauAlgs.getIDVarCalculator(False))

    from AthenaCommon.AppMgr import ToolSvc
    for tool in ditauTools:
        if not hasattr(ToolSvc,tool.name()):
            ToolSvc += tool            
    
    DiTauLowPtBuilder = DiTauBuilder(name="DiTauLowPtBuilder"+Seq.name(),
                                     DiTauContainer="DiTauJetsLowPt",
                                     SeedJetName="AntiKt10EMPFlowJets",
                                     minPt=50000,
                                     maxEta=2.5,
                                     Rjet=1.0,
                                     Rsubjet=0.2,
                                     Rcore=0.1,
                                     Tools=ditauTools)
    Seq += DiTauLowPtBuilder


#=======================================
# TauWP decoration
#=======================================
def addTauWPDecoration(Seq=None, evetoFixTag=None):

    if not Seq or hasattr(Seq,"TauWPDecorator"+Seq.name()):
        print("TauWP Decoration will not be scheduled")
        return

    print ("Adding Tau WP Decoration")
    TauWPDecorations = []
    from AthenaCommon.AppMgr import ToolSvc
    from DerivationFrameworkTau.DerivationFrameworkTauConf import DerivationFramework__TauWPDecoratorWrapper
    
    if (evetoFixTag == "v1"):
        if not hasattr(ToolSvc,"TauWPDecoratorEvetoWrapper"):
            
            from tauRecTools.tauRecToolsConf import TauWPDecorator

            evetoTauWPDecorator = TauWPDecorator( name = 'TauWPDecoratorEleRNN'+Seq.name(),
                                                  flatteningFile1Prong = "rnneveto_mc16d_flat_1p_fix.root",
                                                  flatteningFile3Prong = "rnneveto_mc16d_flat_3p_fix.root",
                                                  DecorWPNames = [ "EleRNNLoose_"+evetoFixTag, "EleRNNMedium_"+evetoFixTag, "EleRNNTight_"+evetoFixTag ],
                                                  DecorWPCutEffs1P = [0.95, 0.90, 0.85],
                                                  DecorWPCutEffs3P = [0.98, 0.95, 0.90],
                                                  UseEleBDT = True,
                                                  ScoreName = "RNNEleScore",
                                                  NewScoreName = "RNNEleScoreSigTrans_"+evetoFixTag,
                                                  DefineWPs = True )
            ToolSvc += evetoTauWPDecorator

            evetoTauWPDecoratorWrapper = DerivationFramework__TauWPDecoratorWrapper(name               = "TauWPDecoratorEvetoWrapper",
                                                                                    TauContainerName   = "TauJets",
                                                                                    TauWPDecorator     = evetoTauWPDecorator)
            ToolSvc += evetoTauWPDecoratorWrapper
        else:
            evetoTauWPDecoratorWrapper = getattr(ToolSvc,"TauWPDecoratorEvetoWrapper")

        TauWPDecorations.append(evetoTauWPDecoratorWrapper)

    if TauWPDecorations:
        Seq += CfgMgr.DerivationFramework__DerivationKernel("TauWPDecorator"+Seq.name(), AugmentationTools = TauWPDecorations)
