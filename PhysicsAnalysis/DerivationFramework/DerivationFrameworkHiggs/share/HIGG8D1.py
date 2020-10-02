#********************************************************************
# HIGG8D1.py 
# reductionConf flag HIGG8D1 in Reco_tf.py   
#********************************************************************

from DerivationFrameworkCore.DerivationFrameworkMaster import *
from DerivationFrameworkInDet.InDetCommon import *
from DerivationFrameworkJetEtMiss.JetCommon import *
from DerivationFrameworkJetEtMiss.METCommon import *
from DerivationFrameworkEGamma.EGammaCommon import *
from DerivationFrameworkMuons.MuonsCommon import *
from DerivationFrameworkTau.TauCommon import *
from DerivationFrameworkHiggs.TruthCategories import *
from DerivationFrameworkFlavourTag.FlavourTagCommon import *
from AthenaCommon.GlobalFlags import globalflags

if DerivationFrameworkHasTruth:
    from DerivationFrameworkMCTruth.MCTruthCommon import addStandardTruthContents
    addStandardTruthContents()
    from DerivationFrameworkMCTruth.HFHadronsCommon import *

#====================================================================
# SET UP STREAM   
#====================================================================
streamName = derivationFlags.WriteDAOD_HIGG8D1Stream.StreamName
fileName   = buildFileName( derivationFlags.WriteDAOD_HIGG8D1Stream )
HIGG8D1Stream = MSMgr.NewPoolRootStream( streamName, fileName )
HIGG8D1Stream.AcceptAlgs(["HIGG8D1Kernel"])

# Thinning service name must match the one passed to the thinning tools
#from AthenaServices.Configurables import ThinningSvc, createThinningSvc
#augStream = MSMgr.GetStream( streamName )
#evtStream = augStream.GetEventStream()
#svcMgr += createThinningSvc( svcName="HIGG8D1ThinningSvc", outStreams=[evtStream] )

#====================================================================
# AUGMENTATION TOOLS
#====================================================================
augmentationTools = []

#====================================================================
# Tau Selector Tool
#====================================================================

from DerivationFrameworkTau.DerivationFrameworkTauConf import DerivationFramework__TauSelectionWrapper
HIGG8D1TauWrapper = DerivationFramework__TauSelectionWrapper(name = "HIGG8D1TauSelectionWrapper",
                                                             IsTauFlag			= 29,
                                                             CollectionName		= "TauJets",
                                                             StoreGateEntryName	= "HIGG8D1JetRNNSigLoose")
ToolSvc += HIGG8D1TauWrapper
augmentationTools.append(HIGG8D1TauWrapper)


#=======================================
# Tau truth matching tool
#=======================================

#truth matching
#if globalflags.DataSource()=='geant4':
#    from TauAnalysisTools.TauAnalysisToolsConf import TauAnalysisTools__TauTruthMatchingTool
#    HIGG8D1TauTruthMatchingTool = TauAnalysisTools__TauTruthMatchingTool(name="HIGG8D1TauTruthMatchingTool",
#                                                                         WriteTruthTaus = True)


#    ToolSvc += HIGG8D1TauTruthMatchingTool

#    from DerivationFrameworkTau.DerivationFrameworkTauConf import DerivationFramework__TauTruthMatchingWrapper
#    HIGG8D1TauTruthMatchingWrapper = DerivationFramework__TauTruthMatchingWrapper( name = "HIGG8D1TauTruthMatchingWrapper",
#                                                                                TauTruthMatchingTool = HIGG8D1TauTruthMatchingTool)
                                                                                #TauContainerName     = "TauJets")

#    ToolSvc += HIGG8D1TauTruthMatchingWrapper
#    augmentationTools.append(HIGG8D1TauTruthMatchingWrapper)

#=======================================
# Tau Overlapping Electron LLH 
#=======================================

#Tau Overlapping Electron LLH Decorator
from TauAnalysisTools.TauAnalysisToolsConf import TauAnalysisTools__TauOverlappingElectronLLHDecorator
HIGG8D1TauOrlElLLHDec = TauAnalysisTools__TauOverlappingElectronLLHDecorator(name="HIGG8D1TauOrlElLLHDec",
                                                                             ElectronContainerName = "Electrons")
    
ToolSvc += HIGG8D1TauOrlElLLHDec
        
#Tau Overlapping Electron LLH Decorator Wrapper
from DerivationFrameworkTau.DerivationFrameworkTauConf import DerivationFramework__TauOverlappingElectronLLHDecoratorWrapper
HIGG8D1TauOrlElLLHDecWrapper = DerivationFramework__TauOverlappingElectronLLHDecoratorWrapper( name = "HIGG8D1TauOrlElLLHDecWrapper",
                                                                                               TauOverlappingElectronLLHDecorator = HIGG8D1TauOrlElLLHDec,
                                                                                               TauContainerName     = "TauJets")
    
ToolSvc += HIGG8D1TauOrlElLLHDecWrapper
augmentationTools.append(HIGG8D1TauOrlElLLHDecWrapper)


#==============================================================================
# BACKGROUND ELECTRON DECORATION TYPE/ORIGIN
#==============================================================================
# PhysicsAnalysis/DerivationFramework/DerivationFrameworkEGamma/trunk/src/BkgElectronClassification.cxx

if DerivationFrameworkHasTruth:
    from MCTruthClassifier.MCTruthClassifierBase import MCTruthClassifier as BkgElectronMCTruthClassifier   
    from DerivationFrameworkEGamma.DerivationFrameworkEGammaConf import DerivationFramework__BkgElectronClassification 
    HIGG8D1BkgElectronClassificationTool = DerivationFramework__BkgElectronClassification (name = "BkgElectronClassificationTool", MCTruthClassifierTool = BkgElectronMCTruthClassifier)
    ToolSvc += HIGG8D1BkgElectronClassificationTool
    augmentationTools.append(HIGG8D1BkgElectronClassificationTool)
    print "BkgElectronClassificationTool: ", HIGG8D1BkgElectronClassificationTool

    from DerivationFrameworkMuons.DerivationFrameworkMuonsConf import DerivationFramework__MuonTruthClassifierFallback
    HIGG8D1MuonTruthClassifierFallback = DerivationFramework__MuonTruthClassifierFallback( name = "MuonTruthClassifierFallback", MCTruthClassifierTool = BkgElectronMCTruthClassifier, ContainerKey="Muons")
    HIGG8D1ElectronTruthClassifierFallback = DerivationFramework__MuonTruthClassifierFallback( name = "ElectronTruthClassifierFallback", MCTruthClassifierTool = BkgElectronMCTruthClassifier, ContainerKey="Electrons")
    ToolSvc += HIGG8D1MuonTruthClassifierFallback
    ToolSvc += HIGG8D1ElectronTruthClassifierFallback
    augmentationTools.append(HIGG8D1MuonTruthClassifierFallback)
    augmentationTools.append(HIGG8D1ElectronTruthClassifierFallback)

#==============================================================================
# HEAVY FLAVOR DECORATION
#==============================================================================
# PhysicsAnalysis/DerivationFramework/DerivationFrameworkTop/trunk/src/TTbarPlusHeavyFlavorFilterTool.cxx
# PhysicsAnalysis/DerivationFramework/DerivationFrameworkTop/trunk/src/TopHeavyFlavorFilterAugmentation.cxx
# these are supposed to mimic the TTbarPlusBFilter, TTbarPlusBBFilter, and TTbarPlusCFilter Filters in https://svnweb.cern.ch/trac/atlasoff/browser/Generators/MC15JobOptions/trunk/common/Filters
if DerivationFrameworkHasTruth:
    from DerivationFrameworkTop.DerivationFrameworkTopConf import DerivationFramework__TTbarPlusHeavyFlavorFilterTool

    HIGG8D1ttbarBfiltertool = DerivationFramework__TTbarPlusHeavyFlavorFilterTool("HIGG8D1TTbarPlusBFilterTool")
    HIGG8D1ttbarBfiltertool.SelectB = True
    HIGG8D1ttbarBfiltertool.BpTMinCut = 5000
    HIGG8D1ttbarBfiltertool.BMultiplicityCut = 1 # >=
    ToolSvc += HIGG8D1ttbarBfiltertool

    HIGG8D1ttbarBBfiltertool = DerivationFramework__TTbarPlusHeavyFlavorFilterTool("HIGG8D1TTbarPlusBBFilterTool")
    HIGG8D1ttbarBBfiltertool.SelectB = True
    HIGG8D1ttbarBBfiltertool.BpTMinCut = 15000
    HIGG8D1ttbarBBfiltertool.BMultiplicityCut = 2 # >=
    ToolSvc += HIGG8D1ttbarBBfiltertool

    HIGG8D1ttbarCfiltertool = DerivationFramework__TTbarPlusHeavyFlavorFilterTool("HIGG8D1TTbarPlusCFilterTool")
    HIGG8D1ttbarCfiltertool.SelectC = True
    HIGG8D1ttbarCfiltertool.CpTMinCut = 15000
    HIGG8D1ttbarCfiltertool.CMultiplicityCut = 1 # >=
    # these two are the default values
    # B-hadrons have precedence; if one B is found, it won't pass the CFilter
    HIGG8D1ttbarCfiltertool.BpTMinCut = 5000
    HIGG8D1ttbarCfiltertool.BMultiplicityCut = 1 # >=
    ToolSvc += HIGG8D1ttbarCfiltertool
   
    from DerivationFrameworkTop.DerivationFrameworkTopConf import DerivationFramework__TopHeavyFlavorFilterAugmentation
    HIGG8D1TopHFFilterAugmentation = DerivationFramework__TopHeavyFlavorFilterAugmentation(name = "HIGG8D1TopHFFilterAugmentation")
    HIGG8D1TopHFFilterAugmentation.BFilterTool = HIGG8D1ttbarBfiltertool
    HIGG8D1TopHFFilterAugmentation.BBFilterTool = HIGG8D1ttbarBBfiltertool
    HIGG8D1TopHFFilterAugmentation.CFilterTool = HIGG8D1ttbarCfiltertool
    ToolSvc += HIGG8D1TopHFFilterAugmentation
    augmentationTools.append(HIGG8D1TopHFFilterAugmentation)
    print "HIGG8D1TopHFFilterAugmentationTool: ", HIGG8D1TopHFFilterAugmentation

#==============================================================================
# HEAVY FLAVOR DECORATIONS (ttbar)
#==============================================================================
# PhysicsAnalysis/DerivationFramework/DerivationFrameworkMCTruth/src/HadronOriginClassifier.cxx -- HOC
# PhysicsAnalysis/DerivationFramework/DerivationFrameworkMCTruth/src/HadronOriginDecorator.cxx
# list of ttbar samples by mc_channel_number
HIGG8D1DSIDList=[
                 410081,
                 410250,#in HOC, Sherpa
                 410251,#in HOC, Sherpa
                 410252,#in HOC, Sherpa
                 410397,
                 410398,
                 410399,
                 410464,#in HOC, Pythia8
                 410465,#in HOC, Pythia8
                 410470,#in HOC, Pythia8
                 410471,#in HOC, Pythia8
                 410472,#in HOC, Pythia8
                 410480,#in HOC, Pythia8
                 410501,#in HOC, Pythia8
                 410503,#in HOC, Pythia8
                 410544,
                 411287
                ]

import PyUtils.AthFile as af
from AthenaCommon.AthenaCommonFlags import athenaCommonFlags
f = af.fopen(athenaCommonFlags.PoolAODInput()[0])
if len(f.mc_channel_number) > 0:
    if(int(f.mc_channel_number[0]) in HIGG8D1DSIDList):
        from DerivationFrameworkMCTruth.DerivationFrameworkMCTruthConf import DerivationFramework__HadronOriginClassifier
        HIGG8D1hadronorigintool = DerivationFramework__HadronOriginClassifier("HIGG8D1HadronOriginClassifier",DSID=int(f.mc_channel_number[0]))
        ToolSvc += HIGG8D1hadronorigintool
        print "HIGG8D1hadronorigintool: ", HIGG8D1hadronorigintool
        from DerivationFrameworkMCTruth.DerivationFrameworkMCTruthConf import DerivationFramework__HadronOriginDecorator
        HIGG8D1hadronorigindecorator = DerivationFramework__HadronOriginDecorator(name = "HIGG8D1HadronOriginDecorator")
        HIGG8D1hadronorigindecorator.ToolName = HIGG8D1hadronorigintool
        ToolSvc += HIGG8D1hadronorigindecorator
        print "HIGG8D1hadronorigindecorator: ", HIGG8D1hadronorigindecorator
        augmentationTools.append(HIGG8D1hadronorigindecorator)


#====================================================================
# THINNING TOOLS
#====================================================================
thinningTools=[]

# Establish the thinning helper (which will set up the services behind the scenes)
from DerivationFrameworkCore.ThinningHelper import ThinningHelper
HIGG8D1ThinningHelper = ThinningHelper( "HIGG8D1ThinningHelper" )
# Trigger Thinning Tool
HIGG8D1ThinningHelper.TriggerChains = 'HLT_e.*|HLT_mu.*|HLT_2e.*|HLT_2mu.*|HLT_tau.*'
HIGG8D1ThinningHelper.AppendToStream( HIGG8D1Stream )


# Jet tracks
from DerivationFrameworkInDet.DerivationFrameworkInDetConf import DerivationFramework__JetTrackParticleThinning
HIGG8D1Akt4PFlowJetTPThinningTool = DerivationFramework__JetTrackParticleThinning( name          = "HIGG8D1Akt4PFlowJetTPThinningTool",
                                                                ThinningService         = HIGG8D1ThinningHelper.ThinningSvc(),
                                                                JetKey                  = "AntiKt4EMPFlowJets",
                                                                InDetTrackParticlesKey  = "InDetTrackParticles",
                                                                ApplyAnd                = False)
ToolSvc += HIGG8D1Akt4PFlowJetTPThinningTool
thinningTools.append(HIGG8D1Akt4PFlowJetTPThinningTool)

HIGG8D1Akt4EMTopoJetTPThinningTool = DerivationFramework__JetTrackParticleThinning( name          = "HIGG8D1Akt4EMTopoJetTPThinningTool",
                                                                ThinningService         = HIGG8D1ThinningHelper.ThinningSvc(),
                                                                JetKey                  = "AntiKt4EMTopoJets",
                                                                InDetTrackParticlesKey  = "InDetTrackParticles",
                                                                ApplyAnd                = False)
ToolSvc += HIGG8D1Akt4EMTopoJetTPThinningTool
thinningTools.append(HIGG8D1Akt4EMTopoJetTPThinningTool)


# Tracks associated with Muons
from DerivationFrameworkInDet.DerivationFrameworkInDetConf import DerivationFramework__MuonTrackParticleThinning
HIGG8D1MuonTPThinningTool = DerivationFramework__MuonTrackParticleThinning(name                    = "HIGG8D1MuonTPThinningTool",
                                                                           ThinningService         = HIGG8D1ThinningHelper.ThinningSvc(),
                                                                           MuonKey                 = "Muons",
                                                                           InDetTrackParticlesKey  = "InDetTrackParticles")
ToolSvc += HIGG8D1MuonTPThinningTool
thinningTools.append(HIGG8D1MuonTPThinningTool)

# Tracks associated with Electrons
from DerivationFrameworkInDet.DerivationFrameworkInDetConf import DerivationFramework__EgammaTrackParticleThinning
HIGG8D1ElectronTPThinningTool = DerivationFramework__EgammaTrackParticleThinning(name                    = "HIGG8D1ElectronTPThinningTool",
                                                                                 ThinningService         = HIGG8D1ThinningHelper.ThinningSvc(),
                                                                                 SGKey                   = "Electrons",
                                                                                 GSFTrackParticlesKey    = "GSFTrackParticles",
                                                                                 InDetTrackParticlesKey  = "InDetTrackParticles",
                                                                                 SelectionString         = "Electrons.pt > 0*GeV",
                                                                                 BestMatchOnly = False,
                                                                                 ConeSize = 0.3,
                                                                                 ApplyAnd = False)
ToolSvc += HIGG8D1ElectronTPThinningTool
thinningTools.append(HIGG8D1ElectronTPThinningTool)

StoreAdditionalAmbiguityContent = True
if StoreAdditionalAmbiguityContent:
    HIGG8D1ElectronAddTPThinningTool = DerivationFramework__EgammaTrackParticleThinning(name                    = "HIGG8D1ElectronAddTPThinningTool",
                                                                                        ThinningService         = HIGG8D1ThinningHelper.ThinningSvc(),
                                                                                        SGKey                   = "Electrons",
                                                                                        SelectionString         = "Electrons.DFCommonAddAmbiguity >= 0",
                                                                                        ConeSize                = 0.3)
    ToolSvc += HIGG8D1ElectronAddTPThinningTool
    thinningTools.append(HIGG8D1ElectronAddTPThinningTool)

# Tracks associated with Taus
#HIGG8D1TauPtSelectionString = "(TauJets.pt > 15*GeV || TauJets.ptFinalCalib > 15.0*GeV)"
from DerivationFrameworkInDet.DerivationFrameworkInDetConf import DerivationFramework__TauTrackParticleThinning
HIGG8D1TauTPThinningTool = DerivationFramework__TauTrackParticleThinning( name                    = "HIGG8D1TauTPThinningTool",
                                                                          ThinningService         = HIGG8D1ThinningHelper.ThinningSvc(),
                                                                          TauKey                  = "TauJets",
                                                                          SelectionString         = "TauJets.pt > 15*GeV || TauJets.ptFinalCalib > 15.0*GeV",
                                                                          ConeSize                = 0.6,
                                                                          InDetTrackParticlesKey  = "InDetTrackParticles")
ToolSvc += HIGG8D1TauTPThinningTool
thinningTools.append(HIGG8D1TauTPThinningTool)

# Electrons - calo cluster thinning
#from DerivationFrameworkEGamma.DerivationFrameworkEGammaConf import DerivationFramework__CaloClusterThinning
#from DerivationFrameworkCalo.DerivationFrameworkCaloConf import DerivationFramework__CaloClusterThinning
#HIGG8D1ElectronCCThinningTool = DerivationFramework__CaloClusterThinning( name                    = "HIGG8D1ElectronCCThinningTool",
#                                                                          ThinningService         = HIGG8D1ThinningHelper.ThinningSvc(),
#                                                                          SGKey                   = "Electrons",
#                                                                          CaloClCollectionSGKey   = "egammaClusters",
#                                                                          SelectionString         = "Electrons.pt >= 5*GeV",
#                                                                          ConeSize                = 0.4)
#ToolSvc += HIGG8D1ElectronCCThinningTool
#thinningTools.append(HIGG8D1ElectronCCThinningTool)

#====================================================================
# Truth Thinning
#====================================================================

#Truth tau/nutau and their ancestors and descendants
truth_cond_tau = " ((abs(TruthParticles.pdgId) == 15 || abs(TruthParticles.pdgId) == 16) && (TruthParticles.pt > 0.01*GeV))" #&& (TruthParticles.barcode<200000)) "
truth_cond_lep = " ((abs(TruthParticles.pdgId) >= 11 && abs(TruthParticles.pdgId) <= 14) && (TruthParticles.pt > 4.0*GeV) && (TruthParticles.status == 1))"# && (TruthParticles.barcode<200000)) "
truth_photon = " ((abs(TruthParticles.pdgId) == 22) && (TruthParticles.pt > 1*GeV)) "
truth_cond_comb = "("+truth_cond_lep+"||"+truth_cond_tau+"||"+truth_photon+")"

# PreserveGeneratorDescendants only keeps particles that came directly from the event generator
# PreserveDescendants keeps all particles including those that come from Geant processes

from DerivationFrameworkMCTruth.DerivationFrameworkMCTruthConf import DerivationFramework__GenericTruthThinning
HIGG8D1TruthTool = DerivationFramework__GenericTruthThinning(name                         = "HIGG8D1TruthTool",
                                                             ThinningService              = HIGG8D1ThinningHelper.ThinningSvc(),
                                                             ParticleSelectionString      = truth_cond_tau,
                                                             PreserveDescendants          = False,
                                                             PreserveGeneratorDescendants = True,
                                                             PreserveAncestors            = False,
                                                             TauHandling                  = False)

HIGG8D1TruthTool2 = DerivationFramework__GenericTruthThinning(name                         = "HIGG8D1TruthTool2",
                                                              ThinningService              = HIGG8D1ThinningHelper.ThinningSvc(),
                                                              ParticleSelectionString      = truth_cond_comb,
                                                              PreserveDescendants          = True, # False
                                                              PreserveGeneratorDescendants = False,
                                                              PreserveAncestors            = True,
                                                              TauHandling                  = False)



from DerivationFrameworkMCTruth.DerivationFrameworkMCTruthConf import DerivationFramework__MenuTruthThinning
HIGG8D1TruthTool3 = DerivationFramework__MenuTruthThinning(name                      = "HIGG8D1TruthTool3",
                                                           ThinningService            = HIGG8D1ThinningHelper.ThinningSvc(),
                                                           #WritePartons               = True,
                                                           WritePartons               = False,
                                                           WriteHadrons               = False,
                                                           WriteBHadrons              = False,
                                                           WriteGeant                 = False,
                                                           GeantPhotonPtThresh        = -1.0,
                                                           WriteTauHad                = True,
                                                           PartonPtThresh             = -1.0,
                                                           WriteBSM                   = False,
                                                           WriteBosons                = True,
                                                           WriteBSMProducts           = False,
                                                           WriteBosonProducts         = True,
                                                           WriteTopAndDecays          = True,
                                                           WriteEverything            = False,
                                                           WriteAllLeptons            = True,
                                                           WriteStatus3               = False,
                                                           #WritettHFHadrons           = True,
                                                           #PreserveGeneratorDescendants = True,
                                                           #PreserveDescendants       = True,
                                                           #PreserveAncestors         = True,
                                                           PreserveParentsSiblingsChildren = True,
                                                           WriteFirstN                = -1)

if DerivationFrameworkHasTruth:
    #ToolSvc += HIGG8D1TruthTool
    #thinningTools.append(HIGG8D1TruthTool)
    ToolSvc += HIGG8D1TruthTool2
    thinningTools.append(HIGG8D1TruthTool2)
    ToolSvc += HIGG8D1TruthTool3
    thinningTools.append(HIGG8D1TruthTool3)

#====================================================================
# Clusters for Tau TES
#====================================================================
                                                                                                                                                                                                                                   
#from DerivationFrameworkCalo.DerivationFrameworkCaloConf import DerivationFramework__CaloClusterThinning
#HIGG8D1CaloClusterThinning  = DerivationFramework__CaloClusterThinning(name                  = "HIGG8D1CaloClusterThinning",
#                            ThinningService       = HIGG8D1ThinningHelper.ThinningSvc(),
#                            SGKey                 = "TauJets",
#                            TopoClCollectionSGKey = "CaloCalTopoClusters",
#                            )
#ToolSvc += HIGG8D1CaloClusterThinning
#thinningTools.append(HIGG8D1CaloClusterThinning)



#====================================================================
# SKIMMING TOOL 
#====================================================================

#to add: MC event info requirements

eleRequirements = '(Electrons.pt > 5*GeV) && (abs(Electrons.eta) < 2.6) && (Electrons.Loose || Electrons.DFCommonElectronsLHLoose)'
leadElectron = eleRequirements + ' && (Electrons.pt > 15*GeV)'
muRequirements = '(Muons.DFCommonGoodMuon) && (Muons.muonType == 0) && (Muons.pt > 5*GeV) && (abs(Muons.eta) < 2.6)'
leadMuon = muRequirements + ' && (Muons.pt > 15*GeV)'
tauRequirements = '(TauJets.pt > 15*GeV || TauJets.ptFinalCalib > 15.0*GeV) && HIGG8D1JetRNNSigLoose && (abs(TauJets.charge)==1.0) && ((TauJets.nTracks == 1) || (TauJets.nTracks == 3))'
#tauRequirements = 'HIGG8D1TauPtSelectionString && HIGG8D1JetRNNSigLoose && (abs(TauJets.charge)==1.0) && ((TauJets.nTracks == 1) || (TauJets.nTracks == 3))'

#=======================================
# DeltaR (tau skimming)
#=======================================

# deltaRTool 
from DerivationFrameworkTools.DerivationFrameworkToolsConf import DerivationFramework__DeltaRTool
HIGG8D1ElTauDeltaRTool = DerivationFramework__DeltaRTool( name = "HIGG8D1ElTauDeltaRTool",
                                                        StoreGateEntryName = "dRElecTau",
                                                        ObjectRequirements = leadElectron,
                                                        SecondObjectRequirements = tauRequirements,
                                                        ContainerName = "Electrons",
                                                        SecondContainerName = "TauJets" )
ToolSvc += HIGG8D1ElTauDeltaRTool
augmentationTools.append(HIGG8D1ElTauDeltaRTool)
HIGG8D1MuTauDeltaRTool = DerivationFramework__DeltaRTool( name = "HIGG8D1MuTauDeltaRTool",
                                                        StoreGateEntryName = "dRMuTau",
                                                        ObjectRequirements = leadMuon,
                                                        SecondObjectRequirements = tauRequirements,
                                                        ContainerName = "Muons",
                                                        SecondContainerName = "TauJets" )
ToolSvc += HIGG8D1MuTauDeltaRTool
augmentationTools.append(HIGG8D1MuTauDeltaRTool)
#===================================

eeSelection = '((count('+eleRequirements+') >= 2) && (count('+leadElectron+') >= 1))'
mmSelection = '((count('+muRequirements+') >= 2) && (count('+leadMuon+') >= 1))'
emSelection = '(((count('+eleRequirements+') >= 1) && (count('+muRequirements+') >= 1)) && ((count('+leadElectron+') >= 1) || (count('+leadMuon+') >= 1)))'

lepSelection = eeSelection+' || '+mmSelection+' || '+emSelection
#tauSelection = '((count('+leadElectron+')+count('+leadMuon+') >= 1) && (count('+tauRequirements+') >= 2))'
leptauSelection = '((count('+leadElectron+')+count('+leadMuon+') >= 1) && ((count('+tauRequirements+') >= 1) && (count(dRElecTau > 0.4 ) + count(dRMuTau > 0.4) >= 1)))'

expression = lepSelection+ ' || ' +leptauSelection

from DerivationFrameworkTools.DerivationFrameworkToolsConf import DerivationFramework__xAODStringSkimmingTool
HIGG8D1SkimmingTool = DerivationFramework__xAODStringSkimmingTool( name = "HIGG8D1SkimmingTool1",
                                                                    expression = expression)
ToolSvc += HIGG8D1SkimmingTool

#=======================================
# CREATE PRIVATE SEQUENCE  
#=======================================

HIGG8D1Seq = CfgMgr.AthSequencer("HIGG8D1Sequence")


#=======================================
# CREATE THE DERIVATION KERNEL ALGORITHM   
#=======================================

from DerivationFrameworkCore.DerivationFrameworkCoreConf import DerivationFramework__DerivationKernel

if DerivationFrameworkHasTruth:
    from DerivationFrameworkCore.LHE3WeightMetadata import *

DerivationFrameworkJob += CfgMgr.DerivationFramework__DerivationKernel("HIGG8D1Kernel",
                                                                       ThinningTools = thinningTools,
                                                                       AugmentationTools = augmentationTools,
                                                                       SkimmingTools = [HIGG8D1SkimmingTool]
                                                                       )


#====================================================================
# LeptonTaggers decorations
#====================================================================
import LeptonTaggers.LeptonTaggersConfig as LepTagConfig

# Build AntiKt4PV0TrackJets and run b-tagging
LepTagConfig.ConfigureAntiKt4PV0TrackJets(HIGG8D1Seq, 'HIGG8D1')

# Add BDT decoration algs
HIGG8D1Seq += LepTagConfig.GetDecoratePromptLeptonAlgs()
HIGG8D1Seq += LepTagConfig.GetDecoratePromptTauAlgs()
HIGG8D1Seq += LepTagConfig.GetDecorateImprovedPromptLeptonAlgs() 

DerivationFrameworkJob += HIGG8D1Seq

#====================================================================
# Jet reconstruction/retagging
#====================================================================
# Adding PFlow b-jets
FlavorTagInit(JetCollections = ['AntiKt4EMPFlowJets'], Sequencer = HIGG8D1Seq) 

# Augment AntiKt4 jets with QG tagging variables
truthjetalg='AntiKt4TruthJets'
if not DerivationFrameworkHasTruth:
	truthjetalg=None
from DerivationFrameworkJetEtMiss.ExtendedJetCommon import addQGTaggerTool
addQGTaggerTool(jetalg="AntiKt4EMTopo",sequence=HIGG8D1Seq,algname="QGTaggerToolAlg",truthjetalg=truthjetalg)
addQGTaggerTool(jetalg="AntiKt4EMPFlow",sequence=HIGG8D1Seq,algname="QGTaggerToolPFAlg",truthjetalg=truthjetalg) 

# MVfJvt #
from DerivationFrameworkJetEtMiss.ExtendedJetCommon import applyMVfJvtAugmentation
applyMVfJvtAugmentation(jetalg='AntiKt4EMTopo',sequence=HIGG8D1Seq, algname='JetForwardJvtToolBDTAlg')

# PFlow fJvt #
from DerivationFrameworkJetEtMiss.ExtendedJetCommon import getPFlowfJVT
getPFlowfJVT(jetalg='AntiKt4EMPFlow',sequence=HIGG8D1Seq, algname='JetForwardPFlowJvtToolAlg')

#====================================================================
# SLIMMING TOOL
#====================================================================
from DerivationFrameworkCore.SlimmingHelper import SlimmingHelper
HIGG8D1SlimmingHelper = SlimmingHelper("HIGG8D1SlimmingHelper")

## Smart Slimming
HIGG8D1SlimmingHelper.SmartCollections = ["AntiKt4EMPFlowJets",
                                          "AntiKt4EMPFlowJets_BTagging201810",
                                          "AntiKt4EMPFlowJets_BTagging201903",
                                          "AntiKt4EMTopoJets",
                                          "AntiKt4EMTopoJets_BTagging201810",
                                          "AntiKt4TruthDressedWZJets",
                                          "AntiKt4TruthJets",
                                          "AntiKt4TruthWZJets",
                                          "BTagging_AntiKt4EMPFlow_201810",
                                          "BTagging_AntiKt4EMPFlow_201903",
                                          "BTagging_AntiKt4EMTopo_201810",
                                          "Electrons",
                                          "InDetTrackParticles",
                                          "MET_Reference_AntiKt4EMPFlow",
                                          "MET_Reference_AntiKt4EMTopo",
                                          "Muons",
                                          "PrimaryVertices",
                                          "TauJets",
                                          "TauMVATESJets"]

# Adding PFlow MET
HIGG8D1SlimmingHelper.AllVariables = ["GSFTrackParticles"] 

HIGG8D1SlimmingHelper.ExtraVariables = [
                                        "Muons.TruthClassifierFallback_truthType.TruthClassifierFallback_truthOrigin.TruthClassifierFallback_dR",
                                        "Electrons.TruthClassifierFallback_truthType.TruthClassifierFallback_truthOrigin.TruthClassifierFallback_dR",
                                        "AntiKt4EMTopoJets.GhostBHadronsFinal.GhostBHadronsFinalCount.GhostBHadronsFinalPt.GhostBHadronsInitial.GhostBHadronsInitialCount.GhostBHadronsInitialPt.GhostBQuarksFinal.GhostBQuarksFinalCount.GhostBQuarksFinalPt.GhostCHadronsFinal.GhostCHadronsFinalCount.GhostCHadronsFinalPt.GhostCHadronsInitial.GhostCHadronsInitialCount.GhostCHadronsInitialPt.GhostCQuarksFinal.GhostCQuarksFinalCount.GhostCQuarksFinalPt.JetEMScaleMomentum_pt.JetEMScaleMomentum_eta.JetEMScaleMomentum_phi.JetEMScaleMomentum_m.ConeTruthLabelID.PartonTruthLabelID.SumPtTrkPt1000.Jvt.JvtJvfcorr.JvtRpt.DFCommonJets_QGTagger_NTracks.DFCommonJets_QGTagger_TracksC1.DFCommonJets_QGTagger_TracksWidth.DFCommonJets_QGTagger_truthjet_eta.DFCommonJets_QGTagger_truthjet_nCharged.DFCommonJets_QGTagger_truthjet_pt",
                                        "AntiKt4EMPFlowJets.GhostBHadronsFinal.GhostBHadronsFinalCount.GhostBHadronsFinalPt.GhostBHadronsInitial.GhostBHadronsInitialCount.GhostBHadronsInitialPt.GhostBQuarksFinal.GhostBQuarksFinalCount.GhostBQuarksFinalPt.GhostCHadronsFinal.GhostCHadronsFinalCount.GhostCHadronsFinalPt.GhostCHadronsInitial.GhostCHadronsInitialCount.GhostCHadronsInitialPt.GhostCQuarksFinal.GhostCQuarksFinalCount.GhostCQuarksFinalPt.JetEMScaleMomentum_pt.JetEMScaleMomentum_eta.JetEMScaleMomentum_phi.JetEMScaleMomentum_m.ConeTruthLabelID.PartonTruthLabelID.SumPtTrkPt1000.Jvt.JvtJvfcorr.JvtRpt.DFCommonJets_QGTagger_NTracks.DFCommonJets_QGTagger_TracksC1.DFCommonJets_QGTagger_TracksWidth.DFCommonJets_QGTagger_truthjet_eta.DFCommonJets_QGTagger_truthjet_nCharged.DFCommonJets_QGTagger_truthjet_pt",
                                        "AntiKt4TruthDressedWZJets.GhostBHadronsFinal.GhostBHadronsFinalCount.GhostBHadronsFinalPt.GhostBHadronsInitial.GhostBHadronsInitialCount.GhostBHadronsInitialPt.GhostBQuarksFinal.GhostBQuarksFinalCount.GhostBQuarksFinalPt.GhostCHadronsFinal.GhostCHadronsFinalCount.GhostCHadronsFinalPt.GhostCHadronsInitial.GhostCHadronsInitialCount.GhostCHadronsInitialPt.GhostCQuarksFinal.GhostCQuarksFinalCount.GhostCQuarksFinalPt",
                                        "AntiKt4TruthJets.GhostBHadronsFinal.GhostBHadronsFinalCount.GhostBHadronsFinalPt.GhostBHadronsInitial.GhostBHadronsInitialCount.GhostBHadronsInitialPt.GhostBQuarksFinal.GhostBQuarksFinalCount.GhostBQuarksFinalPt.GhostCHadronsFinal.GhostCHadronsFinalCount.GhostCHadronsFinalPt.GhostCHadronsInitial.GhostCHadronsInitialCount.GhostCHadronsInitialPt.GhostCQuarksFinal.GhostCQuarksFinalCount.GhostCQuarksFinalPt",
                                        "AntiKt4TruthWZJets.GhostBHadronsFinal.GhostBHadronsFinalCount.GhostBHadronsFinalPt.GhostBHadronsInitial.GhostBHadronsInitialCount.GhostBHadronsInitialPt.GhostBQuarksFinal.GhostBQuarksFinalCount.GhostBQuarksFinalPt.GhostCHadronsFinal.GhostCHadronsFinalCount.GhostCHadronsFinalPt.GhostCHadronsInitial.GhostCHadronsInitialCount.GhostCHadronsInitialPt.GhostCQuarksFinal.GhostCQuarksFinalCount.GhostCQuarksFinalPt",
                                        "CombinedMuonTrackParticles.z0.vz.definingParametersCovMatrix",
                                        "ExtrapolatedMuonTrackParticles.z0.vz.definingParametersCovMatrix",
                                        "PrimaryVertices.x.y",
                                        "BTagging_AntiKt4EMTopo_201810.JetVertexCharge_discriminant",
                                        "BTagging_AntiKt4EMPFlow_201810.JetVertexCharge_discriminant",
                                        "BTagging_AntiKt4EMPFlow_201903.JetVertexCharge_discriminant"
                                       ]

#Adding Egamma details
from DerivationFrameworkEGamma.ElectronsCPDetailedContent import *
HIGG8D1SlimmingHelper.ExtraVariables += ElectronsCPDetailedContent
if StoreAdditionalAmbiguityContent:
		HIGG8D1SlimmingHelper.ExtraVariables += ElectronsAddAmbiguityContent

HIGG8D1SlimmingHelper.ExtraVariables += LepTagConfig.GetExtraPromptVariablesForDxAOD(onlyBDT=False)
HIGG8D1SlimmingHelper.ExtraVariables += LepTagConfig.GetExtraPromptTauVariablesForDxAOD()
HIGG8D1SlimmingHelper.ExtraVariables += LepTagConfig.GetExtraImprovedPromptVariablesForDxAOD() 

ExtraContentTaus=[
        "TauJets."
        "pantau_CellBasedInput_isPanTauCandidate."
        "pantau_CellBasedInput_DecayMode."
        "ptPanTauCellBased."
        "etaPanTauCellBased."
        "phiPanTauCellBased."
        "mPanTauCellBased."
        "pantau_CellBasedInput_BDTValue_1p0n_vs_1p1n."
        "pantau_CellBasedInput_BDTValue_1p1n_vs_1pXn."
        "pantau_CellBasedInput_BDTValue_3p0n_vs_3pXn."
        "ele_match_lhscore." 
        "ele_olr_pass."
        "electronLink.ptDetectorAxis.etaDetectorAxis.phiDetectorAxis.mDetectorAxis"
        "jetLink.seedTrackWidthPt500.seedTrackWidthPt1000."
				#"jetLink.TrackWidthPt1000.TrackWidthPt500"
        #,
        #"TauNeutralParticleFlowObjects."
        #"pt."
        #"eta."
        #"phi."
        #"m."
        #"e."
        #"rapidity."
        #"bdtPi0Score"
        #,
        #"TauChargedParticleFlowObjects.pt.eta.phi.m"
        ]

HIGG8D1SlimmingHelper.ExtraVariables += ExtraContentTaus

#HIGG8D1SlimmingHelper.AllVariables = ["egammaClusters","CaloCalTopoClusters"]

if globalflags.DataSource()=='geant4':
    HIGG8D1SlimmingHelper.AllVariables += ["TruthParticles", "TruthEvents","TruthVertices"]
    HIGG8D1SlimmingHelper.SmartCollections += ["AntiKt4TruthJets"] 
    HIGG8D1SlimmingHelper.ExtraVariables += ["Electrons.truthOrigin.truthType.truthParticleLink.bkgTruthType.bkgTruthOrigin.bkgTruthParticleLink.bkgMotherPdgId.deltaPhi1",
                                             "MuonTruthParticles.truthOrigin.truthType.truthParticleLink",
                                             "InDetTrackParticles.truthOrigin.truthType.truthParticleLink",
                                             "CombinedMuonTrackParticles.truthOrigin.truthType.truthParticleLink",
                                             "TauJets.IsTruthMatched.truthParticleLink.truthJetLink"]
    
    HIGG8D1SlimmingHelper.StaticContent = ["xAOD::TruthParticleContainer#TruthMuons",
                                           "xAOD::TruthParticleAuxContainer#TruthMuonsAux.",
                                           "xAOD::TruthParticleContainer#TruthElectrons",
                                           "xAOD::TruthParticleAuxContainer#TruthElectronsAux.",
                                           "xAOD::TruthParticleContainer#TruthNeutrinos",
                                           "xAOD::TruthParticleAuxContainer#TruthNeutrinosAux.",
                                           "xAOD::TruthParticleContainer#TruthTaus",
                                           "xAOD::TruthParticleAuxContainer#TruthTausAux."]
    
    
HIGG8D1SlimmingHelper.IncludeMuonTriggerContent=True
HIGG8D1SlimmingHelper.IncludeEGammaTriggerContent=True
HIGG8D1SlimmingHelper.IncludeTauTriggerContent=True
 
HIGG8D1SlimmingHelper.AppendContentToStream(HIGG8D1Stream)

# Add MET_RefFinalFix
#addMETOutputs(HIGG8D1Stream)

# Add TOP dedicated MET (LHElectrons)
#from DerivationFrameworkJetEtMiss.TopCustomMET import *
#addMETOutputs(HIGG8D1Stream,["Top"])
