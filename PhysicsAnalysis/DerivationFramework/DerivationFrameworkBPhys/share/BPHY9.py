#********************************************************************
# BPHY9.py
# reductionConf flag BPHY9 in Reco_tf.py   
#********************************************************************

from DerivationFrameworkCore.DerivationFrameworkMaster import *
from DerivationFrameworkInDet.InDetCommon import *
from DerivationFrameworkJetEtMiss.JetCommon import *
#from DerivationFrameworkJetEtMiss.METCommon import *
from DerivationFrameworkEGamma.EGammaCommon import *
from DerivationFrameworkMuons.MuonsCommon import *
from DerivationFrameworkHiggs.TruthCategories import *
from AthenaCommon.GlobalFlags import globalflags

if globalflags.DataSource()=='geant4':
    from DerivationFrameworkMCTruth.MCTruthCommon import addStandardTruthContents
    addStandardTruthContents()
    from DerivationFrameworkMCTruth.HFHadronsCommon import *

# testing globalflags
is_MC = (globalflags.DataSource()=='geant4')
print "is_MC = ",is_MC

#====================================================================
# SET UP STREAM   
#====================================================================
streamName  = derivationFlags.WriteDAOD_BPHY9Stream.StreamName
fileName    = buildFileName( derivationFlags.WriteDAOD_BPHY9Stream )
BPHY9Stream = MSMgr.NewPoolRootStream( streamName, fileName )
BPHY9Stream.AcceptAlgs(["BPHY9Kernel"])

#====================================================================
# AUGMENTATION TOOLS
#====================================================================
augmentationTools = [];
## 1/ setup vertexing tools and services
include( "JpsiUpsilonTools/configureServices.py" ) #this was commented-out?
include("DerivationFrameworkBPhys/configureVertexing.py")
BPHY9_VertexTools = BPHYVertexTools("BPHY9")

#--------------------------------------------------------------------
## 2/ Setup the vertex fitter tools (e.g. JpsiFinder, JpsiPlus1Track, etc).
##    These are general tools independent of DerivationFramework that do the
##    actual vertex fitting and some pre-selection.
from JpsiUpsilonTools.JpsiUpsilonToolsConf import Analysis__JpsiFinder
BPHY9JpsiFinder = Analysis__JpsiFinder(
                                       name                        = "BPHY9JpsiFinder",
                                       OutputLevel                 = INFO,
                                       muAndMu                     = True,
                                       muAndTrack                  = False,
                                       TrackAndTrack               = False,
                                       assumeDiMuons               = True,    # If true, will assume dimu hypothesis and use PDG value for mu mass
                                       invMassUpper                = 100000.0,
                                       invMassLower                = 0.0,
                                       Chi2Cut                     = 200.,
                                       oppChargesOnly	            = True,
                                       atLeastOneComb              = True,
                                       useCombinedMeasurement      = False, # Only takes effect if combOnly=True
                                       muonCollectionKey           = "Muons",
                                       TrackParticleCollection     = "InDetTrackParticles",
                                       V0VertexFitterTool          = BPHY9_VertexTools.TrkV0Fitter,             # V0 vertex fitter
                                       useV0Fitter                 = False,                   # if False a TrkVertexFitterTool will be used
                                       TrkVertexFitterTool         = BPHY9_VertexTools.TrkVKalVrtFitter,        # VKalVrt vertex fitter
                                       TrackSelectorTool           = BPHY9_VertexTools.InDetTrackSelectorTool,
                                       ConversionFinderHelperTool  = BPHY9_VertexTools.InDetConversionHelper,
                                       VertexPointEstimator        = BPHY9_VertexTools.VtxPointEstimator,
                                       useMCPCuts                  = False )

ToolSvc += BPHY9JpsiFinder
print      BPHY9JpsiFinder

#--------------------------------------------------------------------
## 3/ setup the vertex reconstruction "call" tool(s). They are part of the derivation framework.
##    These Augmentation tools add output vertex collection(s) into the StoreGate and add basic
##    decorations which do not depend on the vertex mass hypothesis (e.g. lxy, ptError, etc).
##    There should be one tool per topology, i.e. Jpsi and Psi(2S) do not need two instance of the
##    Reco tool is the JpsiFinder mass window is wide enough.

from DerivationFrameworkBPhys.DerivationFrameworkBPhysConf import DerivationFramework__Reco_mumu
BPHY9_Reco_mumu = DerivationFramework__Reco_mumu(
                                                 name                   = "BPHY9_Reco_mumu",
                                                 JpsiFinder             = BPHY9JpsiFinder,
                                                 OutputVtxContainerName = "BPHY9OniaCandidates",
                                                 PVContainerName        = "PrimaryVertices",
                                                 RefPVContainerName     = "BPHY9RefittedPrimaryVertices",
                                                 RefitPV                = True,
                                                 MaxPVrefit             = 100000,
                                                 DoVertexType           = 7)

ToolSvc += BPHY9_Reco_mumu
print BPHY9_Reco_mumu

#--------------------------------------------------------------------
## 4/ setup the vertex selection and augmentation tool(s). These tools decorate the vertices with
##    variables that depend on the vertex mass hypothesis, e.g. invariant mass, proper decay time, etc.
##    Property HypothesisName is used as a prefix for these decorations.
##    They also perform tighter selection, flagging the vertecis that passed. The flag is a Char_t branch
##    named "passed_"+HypothesisName. It is used later by the "SelectEvent" and "Thin_vtxTrk" tools
##    to determine which events and candidates should be kept in the output stream.
##    Multiple instances of the Select_* tools can be used on a single input collection as long as they
##    use different "HypothesisName" flags.

from DerivationFrameworkBPhys.DerivationFrameworkBPhysConf import DerivationFramework__Select_onia2mumu

## a/ augment and select Jpsi->mumu candidates
BPHY9_Select_Jpsi2mumu = DerivationFramework__Select_onia2mumu(
                                                               name                  = "BPHY9_Select_Jpsi2mumu",
                                                               HypothesisName        = "Jpsi",
                                                               InputVtxContainerName = "BPHY9OniaCandidates",
                                                               VtxMassHypo           = 3096.916,
                                                               MassMin               = 2000.0,
                                                               MassMax               = 3600.0,
                                                               Chi2Max               = 200,
                                                               DoVertexType          = 7)

ToolSvc += BPHY9_Select_Jpsi2mumu
print BPHY9_Select_Jpsi2mumu

## b/ augment and select Psi(2S)->mumu candidates
BPHY9_Select_Psi2mumu = DerivationFramework__Select_onia2mumu(
                                                              name                  = "BPHY9_Select_Psi2mumu",
                                                              HypothesisName        = "Psi",
                                                              InputVtxContainerName = "BPHY9OniaCandidates",
                                                              VtxMassHypo           = 3686.09,
                                                              MassMin               = 3300.0,
                                                              MassMax               = 4500.0,
                                                              Chi2Max               = 200,
                                                              DoVertexType          = 7)

ToolSvc += BPHY9_Select_Psi2mumu
print BPHY9_Select_Psi2mumu

# Added by ASC
## c/ augment and select Upsilon(nS)->mumu candidates
BPHY9_Select_Upsi2mumu = DerivationFramework__Select_onia2mumu(
                                                               name                  = "BPHY9_Select_Upsi2mumu",
                                                               HypothesisName        = "Upsi",
                                                               InputVtxContainerName = "BPHY9OniaCandidates",
                                                               VtxMassHypo           = 9460.30,
                                                               MassMin               = 7000.0,
                                                               MassMax               = 12500.0,
                                                               Chi2Max               = 200,
                                                               DoVertexType          = 7)

ToolSvc += BPHY9_Select_Upsi2mumu
print BPHY9_Select_Upsi2mumu

#--------------------------------------------------------------------
## 5/ select the event. We only want to keep events that contain certain vertices which passed certain selection.
##    This is specified by the "SelectionExpression" property, which contains the expression in the following format:
##
##       "ContainerName.passed_HypoName > count"
##
##    where "ContainerName" is output container form some Reco_* tool, "HypoName" is the hypothesis name setup in some "Select_*"
##    tool and "count" is the number of candidates passing the selection you want to keep.


## SKIMMING TOOLS
ptSelection = '( count(Electrons.pt > 20*GeV) > 0 || count(Muons.pt > 20*GeV) > 0 )'

threelepSelection = '( count(Muons.pt > 0) + count(Electrons.pt > 0) >= 3 )'

oniaSelection = '( count(BPHY9OniaCandidates.passed_Jpsi) > 0 || count(BPHY9OniaCandidates.passed_Psi) > 0 || count(BPHY9OniaCandidates.passed_Upsi) > 0 )'

expression = oniaSelection+ ' && ' +threelepSelection+ ' && ' +ptSelection

from DerivationFrameworkTools.DerivationFrameworkToolsConf import DerivationFramework__xAODStringSkimmingTool
BPHY9_SelectEvent = DerivationFramework__xAODStringSkimmingTool(name = "BPHY9_SelectEvent",
                                                                expression = expression)
ToolSvc += BPHY9_SelectEvent

#--------------------------------------------------------------------
## 6/ track and vertex thinning. We want to remove all reconstructed secondary vertices
##    which hasn't passed any of the selections defined by (Select_*) tools.
##    We also want to keep only tracks which are associates with either muons or any of the
##    vertices that passed the selection. Multiple thinning tools can perform the
##    selection. The final thinning decision is based OR of all the decisions (by default,
##    although it can be changed by the JO).

## a) thining out vertices that didn't pass any selection and idetifying tracks associated with 
##    selected vertices. The "VertexContainerNames" is a list of the vertex containers, and "PassFlags"
##    contains all pass flags for Select_* tools that must be satisfied. The vertex is kept is it 
##    satisfy any of the listed selections.

from DerivationFrameworkBPhys.DerivationFrameworkBPhysConf import DerivationFramework__Thin_vtxTrk
BPHY9Thin_vtxTrk = DerivationFramework__Thin_vtxTrk(
                                                    name                       = "BPHY9Thin_vtxTrk",
                                                    ThinningService            = "BPHY9ThinningSvc",
                                                    TrackParticleContainerName = "InDetTrackParticles",
                                                    VertexContainerNames       = ["BPHY9OniaCandidates"],
                                                    PassFlags                  = ["passed_Jpsi", "passed_Psi", "passed_Upsi"] )

ToolSvc += BPHY9Thin_vtxTrk

# b) thinning out tracks that are not attached to muons. The final thinning decision is based on the OR operation
#    between decision from this and the previous tools.
from DerivationFrameworkInDet.DerivationFrameworkInDetConf import DerivationFramework__MuonTrackParticleThinning
BPHY9MuonTPThinningTool = DerivationFramework__MuonTrackParticleThinning(name                    = "BPHY9MuonTPThinningTool",
                                                                         ThinningService         = "BPHY9ThinningSvc",
                                                                         MuonKey                 = "Muons",
                                                                         InDetTrackParticlesKey  = "InDetTrackParticles")
ToolSvc += BPHY9MuonTPThinningTool

# Added by ASC
# Only save truth informtion directly associated with Onia
from DerivationFrameworkMCTruth.DerivationFrameworkMCTruthConf import DerivationFramework__GenericTruthThinning
BPHY9TruthThinTool = DerivationFramework__GenericTruthThinning(name                    = "BPHY9TruthThinTool",
                                                               ThinningService         = "BPHY9ThinningSvc",
                                                               ParticleSelectionString = "TruthParticles.pdgId == 443 || TruthParticles.pdgId == 100443 || TruthParticles.pdgId == 553 || TruthParticles.pdgId == 100553 || TruthParticles.pdgId == 200553",
                                                               PreserveDescendants     = True,
                                                               PreserveAncestors      = True)
ToolSvc += BPHY9TruthThinTool
print BPHY9TruthThinTool

#====================================================================
# Tau Selector Tool
#====================================================================

from DerivationFrameworkTau.DerivationFrameworkTauConf import DerivationFramework__TauSelectionWrapper
BPHY9TauWrapper = DerivationFramework__TauSelectionWrapper(name = "BPHY9TauSelectionWrapper",
                                                             IsTauFlag			= 19,
                                                             CollectionName		= "TauJets",
                                                             StoreGateEntryName	= "BPHY9JetBDTSigLoose")
ToolSvc += BPHY9TauWrapper
augmentationTools.append(BPHY9TauWrapper)

#=======================================
# Tau Overlapping Electron LLH
#=======================================
#Tau Overlapping Electron LLH Decorator
from TauAnalysisTools.TauAnalysisToolsConf import TauAnalysisTools__TauOverlappingElectronLLHDecorator
BPHY9TauOrlElLLHDec = TauAnalysisTools__TauOverlappingElectronLLHDecorator(name="BPHY9TauOrlElLLHDec",
                                                                             ElectronContainerName = "Electrons")

ToolSvc += BPHY9TauOrlElLLHDec

#Tau Overlapping Electron LLH Decorator Wrapper
from DerivationFrameworkTau.DerivationFrameworkTauConf import DerivationFramework__TauOverlappingElectronLLHDecoratorWrapper
BPHY9TauOrlElLLHDecWrapper = DerivationFramework__TauOverlappingElectronLLHDecoratorWrapper( name = "BPHY9TauOrlElLLHDecWrapper",
                                                                                              TauOverlappingElectronLLHDecorator = BPHY9TauOrlElLLHDec,
                                                                                              TauContainerName     = "TauJets")

ToolSvc += BPHY9TauOrlElLLHDecWrapper
augmentationTools.append(BPHY9TauOrlElLLHDecWrapper)


#==============================================================================
# BACKGROUND ELECTRON DECORATION TYPE/ORIGIN
#==============================================================================
# PhysicsAnalysis/DerivationFramework/DerivationFrameworkEGamma/trunk/src/BkgElectronClassification.cxx

if globalflags.DataSource()=='geant4':
    from MCTruthClassifier.MCTruthClassifierBase import MCTruthClassifier as BkgElectronMCTruthClassifier   
    from DerivationFrameworkEGamma.DerivationFrameworkEGammaConf import DerivationFramework__BkgElectronClassification 
    BPHY9BkgElectronClassificationTool = DerivationFramework__BkgElectronClassification (name = "BkgElectronClassificationTool",
                                                                                           MCTruthClassifierTool = BkgElectronMCTruthClassifier)
    ToolSvc += BPHY9BkgElectronClassificationTool
    augmentationTools.append(BPHY9BkgElectronClassificationTool)
    print "BkgElectronClassificationTool: ", BPHY9BkgElectronClassificationTool


#==============================================================================
# HEAVY FLAVOR DECORATION
#==============================================================================
# PhysicsAnalysis/DerivationFramework/DerivationFrameworkTop/trunk/src/TTbarPlusHeavyFlavorFilterTool.cxx
# PhysicsAnalysis/DerivationFramework/DerivationFrameworkTop/trunk/src/TopHeavyFlavorFilterAugmentation.cxx
if globalflags.DataSource()=='geant4':
    from DerivationFrameworkTop.DerivationFrameworkTopConf import DerivationFramework__TTbarPlusHeavyFlavorFilterTool
    BPHY9tthffiltertool = DerivationFramework__TTbarPlusHeavyFlavorFilterTool("BPHY9TTbarPlusHeavyFlavorFilterTool")
    ToolSvc += BPHY9tthffiltertool
   
    from DerivationFrameworkTop.DerivationFrameworkTopConf import DerivationFramework__TopHeavyFlavorFilterAugmentation
    BPHY9TopHFFilterAugmentation = DerivationFramework__TopHeavyFlavorFilterAugmentation(name = "BPHY9TopHFFilterAugmentation")
    BPHY9TopHFFilterAugmentation.FilterTool = BPHY9tthffiltertool
    ToolSvc += BPHY9TopHFFilterAugmentation
    augmentationTools.append(BPHY9TopHFFilterAugmentation)
    print "BPHY9TopHFFilterAugmentationTool: ", BPHY9TopHFFilterAugmentation


#==============================================================================
# HEAVY FLAVOR DECORATIONS (ttbar)
#==============================================================================
# PhysicsAnalysis/DerivationFramework/DerivationFrameworkMCTruth/trunk/src/HadronOriginClassifier.cxx
# PhysicsAnalysis/DerivationFramework/DerivationFrameworkMCTruth/trunk/src/HadronOriginDecorator.cxx
# list of ttbar samples by mc_channel_number
BPHY9DSIDList=[
    410000,
    410001,
    410002,
    410003,
    410004,
    410007,
    410008,
    410009,
    301528,
    301529,
    301530,
    301531,
    301532,
    303722,
    303723,
    303724,
    303725,
    303726,
    407009,
    407010,
    407011,
    407012,
    410120,
    410121,
    426090,
    426091,
    426092,
    426093,
    426094,
    426095,
    426096,
    426097,
    429007,
    ]

import PyUtils.AthFile as af
from AthenaCommon.AthenaCommonFlags import athenaCommonFlags
f = af.fopen(athenaCommonFlags.PoolAODInput()[0])
if len(f.mc_channel_number) > 0:
    if(int(f.mc_channel_number[0]) in BPHY9DSIDList):
        from DerivationFrameworkMCTruth.DerivationFrameworkMCTruthConf import DerivationFramework__HadronOriginClassifier
        BPHY9hadronorigintool = DerivationFramework__HadronOriginClassifier("BPHY9HadronOriginClassifier",DSID=int(f.mc_channel_number[0]))
        ToolSvc += BPHY9hadronorigintool
        print "BPHY9hadronorigintool: ", BPHY9hadronorigintool
        from DerivationFrameworkMCTruth.DerivationFrameworkMCTruthConf import DerivationFramework__HadronOriginDecorator
        BPHY9hadronorigindecorator = DerivationFramework__HadronOriginDecorator(name = "BPHY9HadronOriginDecorator")
        BPHY9hadronorigindecorator.ToolName = BPHY9hadronorigintool
        ToolSvc += BPHY9hadronorigindecorator
        print "BPHY9hadronorigindecorator: ", BPHY9hadronorigindecorator
        augmentationTools.append(BPHY9hadronorigindecorator)


#====================================================================
# THINNING TOOLS
#====================================================================
thinningTools=[]

# Establish the thinning helper (which will set up the services behind the scenes)
from DerivationFrameworkCore.ThinningHelper import ThinningHelper
BPHY9ThinningHelper = ThinningHelper( "BPHY9ThinningHelper" )
# Trigger Thinning Tool
BPHY9ThinningHelper.TriggerChains = 'HLT_e.*|HLT_mu.*|HLT_2e.*|HLT_2mu.*|HLT_tau.*'
BPHY9ThinningHelper.AppendToStream( BPHY9Stream )


# Jet tracks
from DerivationFrameworkInDet.DerivationFrameworkInDetConf import DerivationFramework__JetTrackParticleThinning
BPHY9JetTPThinningTool = DerivationFramework__JetTrackParticleThinning( name          = "BPHY9JetTPThinningTool",
                                                                ThinningService         = BPHY9ThinningHelper.ThinningSvc(),
                                                                JetKey                  = "AntiKt4EMTopoJets",
                                                                InDetTrackParticlesKey  = "InDetTrackParticles",
                                                                ApplyAnd                = True)
ToolSvc += BPHY9JetTPThinningTool
thinningTools.append(BPHY9JetTPThinningTool)


# Tracks associated with Muons
from DerivationFrameworkInDet.DerivationFrameworkInDetConf import DerivationFramework__MuonTrackParticleThinning
BPHY9MuonTPThinningTool = DerivationFramework__MuonTrackParticleThinning(name                    = "BPHY9MuonTPThinningTool",
                                                                           ThinningService         = BPHY9ThinningHelper.ThinningSvc(),
                                                                           MuonKey                 = "Muons",
                                                                           InDetTrackParticlesKey  = "InDetTrackParticles")
ToolSvc += BPHY9MuonTPThinningTool
thinningTools.append(BPHY9MuonTPThinningTool)

# Tracks associated with Electrons
from DerivationFrameworkInDet.DerivationFrameworkInDetConf import DerivationFramework__EgammaTrackParticleThinning
BPHY9ElectronTPThinningTool = DerivationFramework__EgammaTrackParticleThinning(name                    = "BPHY9ElectronTPThinningTool",
                                                                                 ThinningService         = BPHY9ThinningHelper.ThinningSvc(),
                                                                                 SGKey                   = "Electrons",
                                                                                 InDetTrackParticlesKey  = "InDetTrackParticles")
ToolSvc += BPHY9ElectronTPThinningTool
thinningTools.append(BPHY9ElectronTPThinningTool)

# Tracks associated with Taus
from DerivationFrameworkInDet.DerivationFrameworkInDetConf import DerivationFramework__TauTrackParticleThinning
BPHY9TauTPThinningTool = DerivationFramework__TauTrackParticleThinning( name                    = "BPHY9TauTPThinningTool",
                                                                         ThinningService         = BPHY9ThinningHelper.ThinningSvc(),
                                                                         TauKey                  = "TauJets",
                                                                         SelectionString         = "TauJets.pt > 15*GeV",
                                                                         ConeSize                = 0.6,
                                                                         InDetTrackParticlesKey  = "InDetTrackParticles")
ToolSvc += BPHY9TauTPThinningTool
thinningTools.append(BPHY9TauTPThinningTool)


# Electrons - calo cluster thinning
#from DerivationFrameworkEGamma.DerivationFrameworkEGammaConf import DerivationFramework__CaloClusterThinning
from DerivationFrameworkCalo.DerivationFrameworkCaloConf import DerivationFramework__CaloClusterThinning
BPHY9ElectronCCThinningTool = DerivationFramework__CaloClusterThinning( name                    = "BPHY9ElectronCCThinningTool",
                                                                          ThinningService         = BPHY9ThinningHelper.ThinningSvc(),
                                                                          SGKey                   = "Electrons",
                                                                          CaloClCollectionSGKey   = "egammaClusters",
                                                                          SelectionString         = "Electrons.pt >= 5*GeV",
                                                                          ConeSize                = 0.4)
ToolSvc += BPHY9ElectronCCThinningTool
thinningTools.append(BPHY9ElectronCCThinningTool)

#====================================================================
# Truth Thinning
#====================================================================

#Truth tau/nutau and their ancestors and descendants
truth_cond_tau = " ((abs(TruthParticles.pdgId) == 15 || abs(TruthParticles.pdgId) == 16) && (TruthParticles.pt > 0.01*GeV) && (TruthParticles.barcode<200000)) "
truth_cond_lep = " ((abs(TruthParticles.pdgId) >= 11 && abs(TruthParticles.pdgId) <= 14) && (TruthParticles.pt > 4.0*GeV) && (TruthParticles.status == 1) && (TruthParticles.barcode<200000)) "
# cdb
truth_photon = " ((abs(TruthParticles.pdgId) == 22) && (TruthParticles.pt > 1*GeV)) "
truth_cond_comb = "("+truth_cond_lep+"||"+truth_cond_tau+"||"+truth_photon+")"

# PreserveGeneratorDescendants only keeps particles that came directly from the event generator
# PreserveDescendants keeps all particles including those that come from Geant processes

from DerivationFrameworkMCTruth.DerivationFrameworkMCTruthConf import DerivationFramework__GenericTruthThinning
BPHY9TruthTool = DerivationFramework__GenericTruthThinning(name                         = "BPHY9TruthTool",
                                                             ThinningService              = BPHY9ThinningHelper.ThinningSvc(),
                                                             ParticleSelectionString      = truth_cond_tau,
                                                             PreserveDescendants          = False,
                                                             PreserveGeneratorDescendants = True,
                                                             PreserveAncestors            = False,
                                                             TauHandling                  = False)

BPHY9TruthTool2 = DerivationFramework__GenericTruthThinning(name                         = "BPHY9TruthTool2",
                                                              ThinningService              = BPHY9ThinningHelper.ThinningSvc(),
                                                              ParticleSelectionString      = truth_cond_comb,
                                                              PreserveDescendants          = True, # False
                                                              PreserveGeneratorDescendants = False,
                                                              PreserveAncestors            = True,
                                                              TauHandling                  = False)



from DerivationFrameworkMCTruth.DerivationFrameworkMCTruthConf import DerivationFramework__MenuTruthThinning
BPHY9TruthTool3 = DerivationFramework__MenuTruthThinning(name                      = "BPHY9TruthTool3",
                                                           ThinningService            = BPHY9ThinningHelper.ThinningSvc(),
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
                                                           WriteFirstN                = -1)

if globalflags.DataSource()=='geant4':
    #ToolSvc += BPHY9TruthTool
    #thinningTools.append(BPHY9TruthTool)
    ToolSvc += BPHY9TruthTool2
    thinningTools.append(BPHY9TruthTool2)
    ToolSvc += BPHY9TruthTool3
    thinningTools.append(BPHY9TruthTool3)

#====================================================================
# Clusters for Tau TES
#====================================================================

from DerivationFrameworkCalo.DerivationFrameworkCaloConf import DerivationFramework__CaloClusterThinning
BPHY9CaloClusterThinning  = DerivationFramework__CaloClusterThinning(name                  = "BPHY9CaloClusterThinning",
                                                                       ThinningService       = BPHY9ThinningHelper.ThinningSvc(),
                                                                       SGKey                 = "TauJets",
                                                                       TopoClCollectionSGKey = "CaloCalTopoClusters",
                                                                       )
ToolSvc += BPHY9CaloClusterThinning
thinningTools.append(BPHY9CaloClusterThinning)

#=======================================
# CREATE PRIVATE SEQUENCE  
#=======================================

BPHY9Seq = CfgMgr.AthSequencer("BPHY9Sequence")


#=======================================
# CREATE THE DERIVATION KERNEL ALGORITHM   
#=======================================

BPHY9ThinningTools = [BPHY9Thin_vtxTrk, BPHY9MuonTPThinningTool]
if globalflags.DataSource()=='geant4':
    BPHY9ThinningTools.append(BPHY9TruthThinTool)

from DerivationFrameworkCore.DerivationFrameworkCoreConf import DerivationFramework__DerivationKernel
DerivationFrameworkJob += CfgMgr.DerivationFramework__DerivationKernel(
    "BPHY9Kernel",
    AugmentationTools = [BPHY9_Reco_mumu, BPHY9_Select_Jpsi2mumu, BPHY9_Select_Psi2mumu, BPHY9_Select_Upsi2mumu],
    SkimmingTools = [BPHY9_SelectEvent]
)

# restore jet collections removed in r21 
from DerivationFrameworkJetEtMiss.ExtendedJetCommon import replaceAODReducedJets
reducedJetList = ["AntiKt4PV0TrackJets"]
replaceAODReducedJets(reducedJetList,BPHY9Seq,"BPHY9")

#====================================================================
# JetTagNonPromptLepton decorations
#====================================================================

import JetTagNonPromptLepton.JetTagNonPromptLeptonConfig as Config
BPHY9Seq += Config.GetDecoratePromptLeptonAlgs()

DerivationFrameworkJob += BPHY9Seq


#====================================================================
# SLIMMING TOOL
#====================================================================
from DerivationFrameworkCore.SlimmingHelper import SlimmingHelper
BPHY9SlimmingHelper = SlimmingHelper("BPHY9SlimmingHelper")

StaticContent=[]

## Smart Slimming
BPHY9SlimmingHelper.SmartCollections = [  "Electrons",
                                          "Muons",
                                          "TauJets",
                                          "TauMVATESJets",
                                          "MET_Reference_AntiKt4EMTopo",
                                          "AntiKt4EMTopoJets",
                                          "BTagging_AntiKt4EMTopo",
                                          "InDetTrackParticles",
                                          "PrimaryVertices"]

BPHY9SlimmingHelper.ExtraVariables = ["Muons.clusterLink.allAuthors.charge.extrapolatedMuonSpectrometerTrackParticleLink.scatteringCurvatureSignificance.scatteringNeighbourSignificance.PromptLepton_TagWeight.PromptLepton_TrackJetNTrack.PromptLepton_sv1_ntkv.PromptLepton_jf_ntrkv.PromptLepton_ip2.PromptLepton_ip2_cu.PromptLepton_ip3.PromptLepton_ip3_cu.PromptLepton_EtTopoCone20Rel",
                                        "Electrons.author.charge.PromptLepton_TagWeight.PromptLepton_TrackJetNTrack.PromptLepton_sv1_ntkv.PromptLepton_jf_ntrkv.PromptLepton_ip2.PromptLepton_ip2_cu.PromptLepton_ip3.PromptLepton_ip3_cu.PromptLepton_EtTopoCone20Rel",
                                        "AntiKt4EMTopoJets.JetEMScaleMomentum_pt.JetEMScaleMomentum_eta.JetEMScaleMomentum_phi.JetEMScaleMomentum_m.ConeTruthLabelID.PartonTruthLabelID.SumPtTrkPt1000.Jvt.JvtJvfcorr.JvtRpt",
                                        "GSFTrackParticles.z0.vz.definingParametersCovMatrix",
                                        "CombinedMuonTrackParticles.z0.vz.definingParametersCovMatrix",
                                        "ExtrapolatedMuonTrackParticles.z0.vz.definingParametersCovMatrix",
                                        "PrimaryVertices.x.y"]

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
                  ,
                  "TauNeutralParticleFlowObjects."
                  "pt."
                  "eta."
                  "phi."
                  "m."
                  "e."
                  "rapidity."
                  "bdtPi0Score"
                  ,
                  "TauChargedParticleFlowObjects.pt.eta.phi.m"
                  ]

BPHY9SlimmingHelper.ExtraVariables += ExtraContentTaus

BPHY9SlimmingHelper.AllVariables = ["egammaClusters","CaloCalTopoClusters","BPHY9OniaCandidates"]

if globalflags.DataSource()=='geant4':
    BPHY9SlimmingHelper.AllVariables += ["TruthParticles", "TruthEvents","TruthVertices","AntiKt4TruthJets"]
    BPHY9SlimmingHelper.ExtraVariables += ["Electrons.truthOrigin.truthType.truthParticleLink.bkgTruthType.bkgTruthOrigin.bkgTruthParticleLink.bkgMotherPdgId",
                                             "MuonTruthParticles.truthOrigin.truthType.truthParticleLink",
                                             "InDetTrackParticles.truthOrigin.truthType.truthParticleLink",
                                             "CombinedMuonTrackParticles.truthOrigin.truthType.truthParticleLink",
                                             "TauJets.IsTruthMatched.truthParticleLink.truthJetLink"]

    BPHY9SlimmingHelper.StaticContent = [  "xAOD::TruthParticleContainer#TruthMuons",
                                           "xAOD::TruthParticleAuxContainer#TruthMuonsAux.",
                                           "xAOD::TruthParticleContainer#TruthElectrons",
                                           "xAOD::TruthParticleAuxContainer#TruthElectronsAux.",
                                           "xAOD::TruthParticleContainer#TruthNeutrinos",
                                           "xAOD::TruthParticleAuxContainer#TruthNeutrinosAux.",
                                           "xAOD::TruthParticleContainer#TruthTaus",
                                           "xAOD::TruthParticleAuxContainer#TruthTausAux.",
                                           ]

BPHY9SlimmingHelper.StaticContent += ["xAOD::VertexContainer#%s"        % BPHY9_Reco_mumu.OutputVtxContainerName,
                                     "xAOD::VertexAuxContainer#%sAux." % BPHY9_Reco_mumu.OutputVtxContainerName,
                                     "xAOD::VertexAuxContainer#%sAux.-vxTrackAtVertex" % BPHY9_Reco_mumu.OutputVtxContainerName]

BPHY9SlimmingHelper.IncludeMuonTriggerContent=True
BPHY9SlimmingHelper.IncludeEGammaTriggerContent=True
BPHY9SlimmingHelper.IncludeTauTriggerContent=True

BPHY9SlimmingHelper.AppendContentToStream(BPHY9Stream)

# Add MET_RefFinalFix
#addMETOutputs(BPHY9Stream)

# Add TOP dedicated MET (LHElectrons)
#from DerivationFrameworkJetEtMiss.TopCustomMET import *
#addMETOutputs(BPHY9Stream,["Top"])
