#====================================================================
#
# Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
#
# BPHY20.py
#
# R_Jpsi analysis (currently muon channel only)
#
#====================================================================
from AthenaCommon.AppMgr import ServiceMgr as svcMgr
if not hasattr(svcMgr, 'ItemListSvc'): svcMgr += CfgMgr.ItemListSvc()
svcMgr.ItemListSvc.OutputLevel = INFO

#====================================================================
# FLAGS TO PERSONALIZE THE DERIVATION
#====================================================================

onlyAugmentations = False  # Set to True to deactivate thinning and skimming, and only keep augmentations (to generate a sample with full xAOD plus all the extra)
thinTruth = True
addMuExtrapolationForTrigger = True

from DerivationFrameworkCore.DerivationFrameworkMaster import *
from DerivationFrameworkCore.DerivationFrameworkMaster import DerivationFrameworkHasTruth
isSimulation = DerivationFrameworkHasTruth

print isSimulation

from DerivationFrameworkJetEtMiss.JetCommon import *
from DerivationFrameworkJetEtMiss.METCommon import *
from DerivationFrameworkEGamma.EGammaCommon import *
from DerivationFrameworkEGamma.ElectronsCPContent import ElectronsCPContent


#====================================================================
# SET UP STREAM   
#====================================================================
streamName = derivationFlags.WriteDAOD_BPHY20Stream.StreamName
fileName   = buildFileName( derivationFlags.WriteDAOD_BPHY20Stream )

BPHY20Stream = MSMgr.NewPoolRootStream( streamName, fileName )
BPHY20Stream.AcceptAlgs(["BPHY20Kernel"])

#====================================================================
# AUGMENTATION TOOLS 
#====================================================================
## 1/ setup vertexing tools and services

include("DerivationFrameworkBPhys/configureVertexing.py")
BPHY20_VertexTools = BPHYVertexTools("BPHY20")

from DerivationFrameworkBPhys.DerivationFrameworkBPhysConf import DerivationFramework__AugOriginalCounts
BPHY20_AugOriginalCounts = DerivationFramework__AugOriginalCounts(
   name = "BPHY20_AugOriginalCounts",
   VertexContainer = "PrimaryVertices",
   TrackContainer = "InDetTrackParticles" )
ToolSvc += BPHY20_AugOriginalCounts

#--------------------------------------------------------------------
# 2/ Select J/psi>mu+mu-
#--------------------------------------------------------------------
## a/ setup JpsiFinder tool
##    These are general tools independent of DerivationFramework that do the 
##    actual vertex fitting and some pre-selection.
from JpsiUpsilonTools.JpsiUpsilonToolsConf import Analysis__JpsiFinder
BPHY20JpsiFinder = Analysis__JpsiFinder(
    name                       = "BPHY20JpsiFinder",
    OutputLevel                = INFO,
    muAndMu                    = True,
    muAndTrack                 = False,
    TrackAndTrack              = False,
    assumeDiMuons              = True, 
    muonThresholdPt            = 1000,
    invMassUpper               = 4500.0,
    invMassLower               = 2000.0,
    Chi2Cut                    = 20.,
    oppChargesOnly	        = True,
    allMuons                   = True,
#    allChargeCombinations	   = True,
    combOnly                   = False,
    atLeastOneComb             = False,
    useCombinedMeasurement     = False, # Only takes effect if combOnly=True    
    muonCollectionKey          = "Muons",
    TrackParticleCollection    = "InDetTrackParticles",
    V0VertexFitterTool         = BPHY20_VertexTools.TrkV0Fitter,             # V0 vertex fitter
    useV0Fitter                = False,                   # if False a TrkVertexFitterTool will be used
    TrkVertexFitterTool        = BPHY20_VertexTools.TrkVKalVrtFitter,        # VKalVrt vertex fitter
    TrackSelectorTool          = BPHY20_VertexTools.InDetTrackSelectorTool,
    ConversionFinderHelperTool = BPHY20_VertexTools.InDetConversionHelper,
    VertexPointEstimator       = BPHY20_VertexTools.VtxPointEstimator,
    useMCPCuts                 = False)

ToolSvc += BPHY20JpsiFinder
print      BPHY20JpsiFinder

#--------------------------------------------------------------------
## b/ setup the vertex reconstruction "call" tool(s). They are part of the derivation framework.
##    These Augmentation tools add output vertex collection(s) into the StoreGate and add basic 
##    decorations which do not depend on the vertex mass hypothesis (e.g. lxy, ptError, etc).
##    There should be one tool per topology, i.e. Jpsi and Psi(2S) do not need two instance of the
##    Reco tool is the JpsiFinder mass window is wide enough.
from DerivationFrameworkBPhys.DerivationFrameworkBPhysConf import DerivationFramework__Reco_mumu
BPHY20JpsiSelectAndWrite = DerivationFramework__Reco_mumu(
    name                   = "BPHY20JpsiSelectAndWrite",
    JpsiFinder             = BPHY20JpsiFinder,
    OutputVtxContainerName = "BPHY20JpsiCandidates",
    PVContainerName        = "PrimaryVertices",
    RefPVContainerName     = "SHOULDNOTBEUSED",
    DoVertexType           = 7)

ToolSvc += BPHY20JpsiSelectAndWrite
print BPHY20JpsiSelectAndWrite

#--------------------------------------------------------------------
## c/ augment and select Jpsi->mumu candidates
from DerivationFrameworkBPhys.DerivationFrameworkBPhysConf import DerivationFramework__Select_onia2mumu
BPHY20_Select_Jpsi2mumu = DerivationFramework__Select_onia2mumu(
    name                  = "BPHY20_Select_Jpsi2mumu",
    HypothesisName        = "Jpsi",
    InputVtxContainerName = "BPHY20JpsiCandidates",
    VtxMassHypo           = 3096.900,
    MassMin               = 2000.0,
    MassMax               = 4500.0,
    Chi2Max               = 20,
    DoVertexType          = 7)
  
ToolSvc += BPHY20_Select_Jpsi2mumu
print      BPHY20_Select_Jpsi2mumu

#--------------------------------------------------------------------
# 3/ select B_c+->J/psi mu
#--------------------------------------------------------------------
## a/ setup a new vertexing tool (necessary due to use of mass constraint) 
from TrkVKalVrtFitter.TrkVKalVrtFitterConf import Trk__TrkVKalVrtFitter
BcJpsiMuVertexFit = Trk__TrkVKalVrtFitter(
    name               = "BcJpsiMuVertexFit",
    Extrapolator       = BPHY20_VertexTools.InDetExtrapolator,
    FirstMeasuredPoint = False,
    MakeExtendedVertex = True)

ToolSvc += BcJpsiMuVertexFit
print      BcJpsiMuVertexFit

#--------------------------------------------------------------------
## b/ setup the Jpsi+1 track finder
from JpsiUpsilonTools.JpsiUpsilonToolsConf import Analysis__JpsiPlus1Track
BPHY20BcJpsiMu = Analysis__JpsiPlus1Track(
    name                    = "BPHY20BcJpsiMu",
    OutputLevel             = INFO, #DEBUG,
    pionHypothesis          = True, #False,
    kaonHypothesis        = True,#True,
    trkThresholdPt        = 1000,
    trkMaxEta               = 3.0,
    BThresholdPt            = 1000.0,
    BMassUpper                = 10000.0,
    BMassLower                = 2000.0,
    JpsiContainerKey        = "BPHY20JpsiCandidates",
    TrackParticleCollection = "InDetTrackParticles",
    MuonsUsedInJpsi         = "Muons",
    ExcludeCrossJpsiTracks		= False,
    TrkVertexFitterTool     = BcJpsiMuVertexFit,
    TrackSelectorTool       = BPHY20_VertexTools.InDetTrackSelectorTool,
    UseMassConstraint       = True, 
    RequireNMuonTracks      = 1,
    Chi2Cut                 = 1000, #5
    TrkTrippletMassUpper    = 10000.0,
    TrkTrippletMassLower    = 2000.0)
        
ToolSvc += BPHY20BcJpsiMu
print      BPHY20BcJpsiMu    

#--------------------------------------------------------------------
## c/ setup the combined augmentation/skimming tool for the Bc+>J/psi mu
from DerivationFrameworkBPhys.DerivationFrameworkBPhysConf import DerivationFramework__Reco_dimuTrk
BPHY20BcJpsiMuSelectAndWrite = DerivationFramework__Reco_dimuTrk(
    name                   = "BPHY20BcJpsiMuSelectAndWrite",
    OutputLevel            = INFO,
    Jpsi1PlusTrackName     = BPHY20BcJpsiMu,
    OutputVtxContainerName = "BPHY20BcJpsiMuCandidates",
    PVContainerName        = "PrimaryVertices",
    RefPVContainerName     = "BPHY20RefittedPrimaryVertices",
    RefitPV                = True,
    MaxPVrefit           = 1000)

ToolSvc += BPHY20BcJpsiMuSelectAndWrite 
print      BPHY20BcJpsiMuSelectAndWrite

#--------------------------------------------------------------------
## c/ augment and select B_c+>Jpsi mu candidates
BPHY20_Select_Bc2JpsiMu = DerivationFramework__Select_onia2mumu(
    name                  = "BPHY20_Select_Bc2JpsiMu",
    HypothesisName        = "Bc",
    InputVtxContainerName = "BPHY20BcJpsiMuCandidates",
    TrkMasses             = [105.658, 105.658, 105.658],
    VtxMassHypo           = 6274.9,
    MassMin               = 2000.0,
    MassMax               = 10000.0,
    Chi2Max               = 1000)

ToolSvc += BPHY20_Select_Bc2JpsiMu
print      BPHY20_Select_Bc2JpsiMu

#--------------------------------------------------------------------
# 4/ select B_c+->J/psi e
#--------------------------------------------------------------------
## a/ setup a new vertexing tool (necessary due to use of mass constraint) 
from TrkVKalVrtFitter.TrkVKalVrtFitterConf import Trk__TrkVKalVrtFitter
BcJpsiEVertexFit = Trk__TrkVKalVrtFitter(
    name               = "BcJpsiEVertexFit",
    Extrapolator       = BPHY20_VertexTools.InDetExtrapolator,
    FirstMeasuredPoint = False,
    MakeExtendedVertex = True)

ToolSvc += BcJpsiEVertexFit
print      BcJpsiEVertexFit

#--------------------------------------------------------------------
## b/ setup the Jpsi+1 track finder for Bc->Jpsie+
from JpsiUpsilonTools.JpsiUpsilonToolsConf import Analysis__JpsiPlus1Track
BPHY20BcJpsiE = Analysis__JpsiPlus1Track(
    name                    = "BPHY20BcJpsiE",
    OutputLevel             = INFO, #DEBUG,
    pionHypothesis          = True, #False,
    kaonHypothesis        = True,#True,
    trkThresholdPt        = 500.0,
    trkMaxEta               = 3.0,
    BThresholdPt            = 1000.0,
    BMassUpper                = 10000.0,
    BMassLower                = 2000.0,
    JpsiContainerKey        = "BPHY20JpsiCandidates",
    TrackParticleCollection = "InDetTrackParticles",
    MuonsUsedInJpsi         = "Muons",
    ExcludeCrossJpsiTracks		= False,
    ElectronCollection      = "Electrons",
    TrkVertexFitterTool     = BcJpsiEVertexFit,
    TrackSelectorTool       = BPHY20_VertexTools.InDetTrackSelectorTool,
    UseMassConstraint       = True, 
    RequireNElectronTracks      = 1,
    Chi2Cut                 = 1000, #5
    TrkTrippletMassUpper    = 10000.0,
    TrkTrippletMassLower    = 2000.0,
    GSFCollection           = "GSFTrackParticles",
    UseGSFTrackIndices      = [2])
        
ToolSvc += BPHY20BcJpsiE
print      BPHY20BcJpsiE    

#--------------------------------------------------------------------
## c/ setup the combined augmentation/skimming tool for the Bc+>J/psi e
from DerivationFrameworkBPhys.DerivationFrameworkBPhysConf import DerivationFramework__Reco_dimuTrk
BPHY20BcJpsiESelectAndWrite = DerivationFramework__Reco_dimuTrk(
    name                   = "BPHY20BcJpsiESelectAndWrite",
    OutputLevel            = INFO,
    Jpsi1PlusTrackName     = BPHY20BcJpsiE,
    OutputVtxContainerName = "BPHY20BcJpsiECandidates",
    PVContainerName        = "PrimaryVertices",
    RefPVContainerName     = "BPHY20RefittedPrimaryVertices",
    RefitPV                = True,
    MaxPVrefit           = 1000)

ToolSvc += BPHY20BcJpsiESelectAndWrite 
print      BPHY20BcJpsiESelectAndWrite

#--------------------------------------------------------------------
## c/ augment and select B_c+>Jpsi e candidates
BPHY20_Select_Bc2JpsiE = DerivationFramework__Select_onia2mumu(
    name                  = "BPHY20_Select_Bc2JpsiE",
    HypothesisName        = "Bc",
    InputVtxContainerName = "BPHY20BcJpsiECandidates",
    TrkMasses             = [105.658, 105.658, 0.511],
    VtxMassHypo           = 6274.9,
    MassMin               = 2000.0,
    MassMax               = 10000.0,
    Chi2Max               = 1000)

ToolSvc += BPHY20_Select_Bc2JpsiE
print      BPHY20_Select_Bc2JpsiE


#--------------------------------------------------------------------
# Configure the conversion finder
#--------------------------------------------------------------------
## a/ setup a new vertexing tool (necessary due to use of mass constraint) 
from TrkVKalVrtFitter.TrkVKalVrtFitterConf import Trk__TrkVKalVrtFitter
GammaVertexFit = Trk__TrkVKalVrtFitter(
    name               = "GammaVertexFit",
    Extrapolator       = BPHY20_VertexTools.InDetExtrapolator,
    FirstMeasuredPoint = False,
    Robustness          = 6,
    InputParticleMasses = [0.511,0.511],
    FirstMeasuredPointLimit = True,
    MakeExtendedVertex = True
    )

ToolSvc += GammaVertexFit
print      GammaVertexFit


from DerivationFrameworkBPhys.DerivationFrameworkBPhysConf import DerivationFramework__BPhysBGammaFinder
BPHY20_GammaFinder  		= DerivationFramework__BPhysBGammaFinder(
    name 					= "BPHY20_GammaFinder",
    OutputLevel				= INFO,
    VertexFitterTool 		= GammaVertexFit,
    VertexEstimator 		= BPHY20_VertexTools.VtxPointEstimator,
    InputTrackParticleContainerName = "InDetTrackParticles",
    ConversionContainerName = "BPHY20ConversionCandidates",
    BVertexContainers 	  	= ["BPHY20BcJpsiMuCandidates", "BPHY20BcJpsiECandidates"],
    PassFlagsToCheck  		= ["passed_Bc"],
    RequireDeltaQ 			= True, 
    Use_low_pT              = False,
    MaxDeltaQ 				= 500.0,
    MaxGammaMass            = 110.0,
    Chi2Cut                 = 20.0 )

ToolSvc += BPHY20_GammaFinder
print	 BPHY20_GammaFinder

#====================================================================
# Isolation
#====================================================================


#Track isolation for candidates
from DerivationFrameworkBPhys.DerivationFrameworkBPhysConf import DerivationFramework__VertexTrackIsolation
BPHY20TrackIsolationDecorator = DerivationFramework__VertexTrackIsolation(
  name                            = "BPHY20TrackIsolationDecorator",
  OutputLevel                     = INFO,
  TrackIsoTool                       = "xAOD::TrackIsolationTool",
  TrackContainer                  = "InDetTrackParticles",
  InputVertexContainer            = "BPHY20BcJpsiMuCandidates",
  PassFlags                       = ["passed_Bc"] )

ToolSvc += BPHY20TrackIsolationDecorator

#CaloIsolationTool explicitly declared to avoid pointless warnings (it works!!!)
from IsolationTool.IsolationToolConf import xAOD__CaloIsolationTool
BPHY20CaloIsolationTool = xAOD__CaloIsolationTool(
  name                            = "BPHY20CaloIsolationTool",
  OutputLevel                     = WARNING,                  
  saveOnlyRequestedCorrections    = True,
  IsoLeakCorrectionTool           = "" ) #Workaround for a bug in older versions

ToolSvc += BPHY20CaloIsolationTool

#Calo isolation for candidates
from DerivationFrameworkBPhys.DerivationFrameworkBPhysConf import DerivationFramework__VertexCaloIsolation
BPHY20CaloIsolationDecorator = DerivationFramework__VertexCaloIsolation(
  name                            = "BPHY20CaloIsolationDecorator",
  OutputLevel                     = INFO,                  
  CaloIsoTool                     = BPHY20CaloIsolationTool,  #"xAOD::CaloIsolationTool",
  TrackContainer                  = "InDetTrackParticles",
  InputVertexContainer            = "BPHY20BcJpsiMuCandidates",
  CaloClusterContainer            = "CaloCalTopoClusters",
  ParticleCaloExtensionTool       = "Trk::ParticleCaloExtensionTool/ParticleCaloExtensionTool",
  PassFlags                       = ["passed_Bc"] )

ToolSvc += BPHY20CaloIsolationDecorator



#====================================================================
# Skimming tool to select only events with the correct vertices
#====================================================================

#--------------------------------------------------------------------
## 9/ select the event. We only want to keep events that contain certain three-mu vertices which passed certain selection.
##    Exactly like in the preselection, where only 2mu vertices are treated.

#
#expression = "count(BPHY20JpsiCandidates.x > -999.0)+count(BPHY20BcJpsiMuCandidates.x > -999.0)+ count(BPHY20BcJpsiMuCandidates.passed_Bc) >0 "

from DerivationFrameworkTools.DerivationFrameworkToolsConf import DerivationFramework__xAODStringSkimmingTool
BPHY20_SelectBcJpsiMuEvent = DerivationFramework__xAODStringSkimmingTool(
     name = "BPHY20_SelectBcJpsiMuEvent",
     expression = "count(BPHY20BcJpsiMuCandidates.passed_Bc) >= 1 ")
   
ToolSvc += BPHY20_SelectBcJpsiMuEvent
print      BPHY20_SelectBcJpsiMuEvent

from DerivationFrameworkTools.DerivationFrameworkToolsConf import DerivationFramework__xAODStringSkimmingTool
BPHY20_SelectBcJpsiEEvent = DerivationFramework__xAODStringSkimmingTool(
     name = "BPHY20_SelectBcJpsiEEvent",
     expression = "count(BPHY20BcJpsiECandidates.passed_Bc) >= 1 ")
   
ToolSvc += BPHY20_SelectBcJpsiEEvent
print      BPHY20_SelectBcJpsiEEvent


#====================================================================
# Make event selection based on an OR of the input skimming tools
#====================================================================
      
from DerivationFrameworkTools.DerivationFrameworkToolsConf import DerivationFramework__FilterCombinationOR
BPHY20SkimmingOR = CfgMgr.DerivationFramework__FilterCombinationOR(
       "BPHY20SkimmingOR",
       FilterList = [BPHY20_SelectBcJpsiMuEvent, BPHY20_SelectBcJpsiEEvent] )
       
ToolSvc += BPHY20SkimmingOR
print      BPHY20SkimmingOR   
   

#====================================================================
# Add Extrapolation of muons to trigger layers
#====================================================================

from DerivationFrameworkBPhys.DerivationFrameworkBPhysConf import DerivationFramework__MuonExtrapolationTool 
BPHY20_Extrap_Tool = DerivationFramework__MuonExtrapolationTool(   name = "BPHY20_ExtrapolationTool",   OutputLevel = INFO ) 

ToolSvc += BPHY20_Extrap_Tool





#====================================================================
# Thinning Helper and various thinning tools
#====================================================================

#--------------------------------------------------------------------
## 10/ Setup the thinning helper, only tool able to perform thinning of trigger navigation information

from DerivationFrameworkCore.ThinningHelper import ThinningHelper
BPHY20ThinningHelper = ThinningHelper( "BPHY20ThinningHelper" )
BPHY20ThinningHelper.AppendToStream( BPHY20Stream )


#--------------------------------------------------------------------
## 11/ track and vertex thinning. We want to remove all reconstructed secondary vertices
##    which haven't passed any of the selections defined by (Select_*) tools.
##    We also want to keep only tracks which are associates with either muons or any of the
##    vertices that passed the selection. Multiple thinning tools can perform the 
##    selection. The final thinning decision is based OR of all the decisions (by default,
##    although it can be changed by the JO).

## a) thining out vertices that didn't pass any selection and idetifying tracks associated with 
##    selected vertices. The "VertexContainerNames" is a list of the vertex containers, and "PassFlags"
##    contains all pass flags for Select_* tools that must be satisfied. The vertex is kept is it 
##    satisfy any of the listed selections.

from DerivationFrameworkBPhys.DerivationFrameworkBPhysConf import DerivationFramework__Thin_vtxTrk
BPHY20Thin_vtxTrk = DerivationFramework__Thin_vtxTrk(
  name                       = "BPHY20Thin_vtxTrk",
  OutputLevel                = INFO,
  ThinningService            = BPHY20ThinningHelper.ThinningSvc(),
  TrackParticleContainerName = "InDetTrackParticles",
  AcceptanceRadius         = 1.,
  VertexContainerNames       = ["BPHY20BcJpsiMuCandidates", "BPHY20BcJpsiECandidates"],
  PassFlags                  = ["passed_Bc"],
  ApplyAnd                   = True )  # "and" requirement for Vertices

ToolSvc += BPHY20Thin_vtxTrk


## 12/ thinning out tracks that are not attached to muons. The final thinning decision is based on the OR operation
##     between decision from this and the previous tools.
from DerivationFrameworkInDet.DerivationFrameworkInDetConf import DerivationFramework__MuonTrackParticleThinning
BPHY20MuonTPThinningTool = DerivationFramework__MuonTrackParticleThinning(name                    = "BPHY20MuonTPThinningTool",
                                                                         ThinningService         = BPHY20ThinningHelper.ThinningSvc(),
                                                                         MuonKey                 = "Muons",
                                                                         InDetTrackParticlesKey  = "InDetTrackParticles")
ToolSvc += BPHY20MuonTPThinningTool

from DerivationFrameworkInDet.DerivationFrameworkInDetConf import DerivationFramework__EgammaTrackParticleThinning
BPHY20EgammaTPThinningTool = DerivationFramework__EgammaTrackParticleThinning(
    name                   = "BPHY20EgammaTPThinningTool",
    ThinningService        = "BPHY20ThinningSvc",
    SGKey                  = "Electrons",
    GSFTrackParticlesKey = "GSFTrackParticles",
    SelectionString = "",
    InDetTrackParticlesKey = "InDetTrackParticles")  
ToolSvc += BPHY20EgammaTPThinningTool

from DerivationFrameworkBPhys.DerivationFrameworkBPhysConf import DerivationFramework__BPhysPVThinningTool
BPHY20_thinningTool_PV = DerivationFramework__BPhysPVThinningTool(name                       = "BPHY20_thinningTool_PV",
                                                                 ThinningService            = BPHY20ThinningHelper.ThinningSvc(),
                                                                 CandidateCollections       = ["BPHY20BcJpsiMuCandidates", "BPHY20BcJpsiECandidates"],
                                                                 KeepPVTracks  =True)

ToolSvc += BPHY20_thinningTool_PV

from DerivationFrameworkInDet.DerivationFrameworkInDetConf import DerivationFramework__TauTrackParticleThinning
BPHY20TauTPThinningTool = DerivationFramework__TauTrackParticleThinning(name                    = "BPHY20TauTPThinningTool",
                                                                       ThinningService         = BPHY20ThinningHelper.ThinningSvc(),
                                                                       TauKey                 = "TauJets",
                                                                       InDetTrackParticlesKey  = "InDetTrackParticles")
ToolSvc += BPHY20TauTPThinningTool

# Only save truth informtion directly associated with: mu Ds+ D+ D*+ Ds*+ D0 D*0 B+ B*+ B0 B*0 
from DerivationFrameworkMCTruth.DerivationFrameworkMCTruthConf import DerivationFramework__GenericTruthThinning
BPHY20TruthThinTool = DerivationFramework__GenericTruthThinning(name                    = "BPHY20TruthThinTool",
                                                               ThinningService         = BPHY20ThinningHelper.ThinningSvc(),
                                                               ParticleSelectionString = "abs(TruthParticles.pdgId) == 11 || abs(TruthParticles.pdgId) == 13 || abs(TruthParticles.pdgId) == 15 || abs(TruthParticles.pdgId) == 541 || abs(TruthParticles.pdgId) == 543 || abs(TruthParticles.pdgId) == 100541 || abs(TruthParticles.pdgId) == 100543 || abs(TruthParticles.pdgId) == 431 || abs(TruthParticles.pdgId) == 411 || abs(TruthParticles.pdgId) == 413 || abs(TruthParticles.pdgId) == 433 || TruthParticles.pdgId == 421 || TruthParticles.pdgId == 423 || abs(TruthParticles.pdgId) == 521 || abs(TruthParticles.pdgId) == 523 || TruthParticles.pdgId == 511 || TruthParticles.pdgId == 513 || TruthParticles.pdgId == 531 || TruthParticles.pdgId == 533 || TruthParticles.pdgId == 443 || TruthParticles.pdgId == 100443",
                                                               PreserveDescendants     = True,
                                                               PreserveAncestors      = True)
ToolSvc += BPHY20TruthThinTool

# Only save truth neutrino and b/c quarks information
from DerivationFrameworkMCTruth.DerivationFrameworkMCTruthConf import DerivationFramework__GenericTruthThinning
BPHY20TruthThinNoChainTool = DerivationFramework__GenericTruthThinning(name                    = "BPHY20TruthThinNoChainTool",
                                                              ThinningService         = BPHY20ThinningHelper.ThinningSvc(),
                                                              ParticleSelectionString = "abs(TruthParticles.pdgId) == 4 || abs(TruthParticles.pdgId) == 5 || abs(TruthParticles.pdgId) == 12 || abs(TruthParticles.pdgId) == 14 || abs(TruthParticles.pdgId) == 16",
                                                              PreserveDescendants     = False,
                                                              PreserveAncestors      = False)
ToolSvc += BPHY20TruthThinNoChainTool

# Keep tracks from conversions
from DerivationFrameworkBPhys.DerivationFrameworkBPhysConf import DerivationFramework__Thin_vtxTrk
BPHY20Thin_ConvTrk = DerivationFramework__Thin_vtxTrk(
  name                       = "BPHY20Thin_ConvTrk",
  ThinningService            = "BPHY20ThinningSvc",
  TrackParticleContainerName = "InDetTrackParticles",
  VertexContainerNames       = ["BPHY20ConversionCandidates"],
  PassFlags                  = ["passed_Gamma"] 
  )

ToolSvc += BPHY20Thin_ConvTrk


#====================================================================
# CREATE THE DERIVATION KERNEL ALGORITHM AND PASS THE ABOVE TOOLS  
#====================================================================

BPHY20ThinningTools = [ BPHY20MuonTPThinningTool, BPHY20EgammaTPThinningTool, BPHY20Thin_ConvTrk,
                        BPHY20Thin_vtxTrk, BPHY20_thinningTool_PV,  
                        BPHY20TauTPThinningTool]

BPHY20SkimmingTools = [BPHY20SkimmingOR]

BPHY20AugmentationTools = [BPHY20JpsiSelectAndWrite, BPHY20_Select_Jpsi2mumu,
                           BPHY20BcJpsiMuSelectAndWrite, BPHY20_Select_Bc2JpsiMu,
                           BPHY20BcJpsiESelectAndWrite, BPHY20_Select_Bc2JpsiE, 
                           BPHY20_GammaFinder,
                           BPHY20_AugOriginalCounts,
                           BPHY20TrackIsolationDecorator, BPHY20CaloIsolationDecorator]

if addMuExtrapolationForTrigger:
    BPHY20AugmentationTools.append(BPHY20_Extrap_Tool)

Kernel1Tools = []

if isSimulation:
    #BPHY20AugmentationTools.append(DFCommonTauTruthMatchingWrapper)
    if thinTruth:
       BPHY20ThinningTools.append(BPHY20TruthThinTool)
       BPHY20ThinningTools.append(BPHY20TruthThinNoChainTool)

#The sequence object. Is in principle just a wrapper which allows to run two kernels in sequence
BPHY20_Sequence = CfgMgr.AthSequencer("BPHY20_Sequence")
from DerivationFrameworkFlavourTag.FlavourTagCommon import FlavorTagInit
FlavorTagInit(JetCollections=['AntiKt4EMPFlowJets'], Sequencer=BPHY20_Sequence)

#onlyAugmentations implementation
if onlyAugmentations:
    Kernel1Tools = []
    BPHY20SkimmingTools = []
    BPHY20ThinningTools = []

# Kernel n1 PRESELECTION
# Kernel n2 deep Derivation
# The name of the kernel (BPHY20Kernel2 in this case) must be unique to this derivation
from DerivationFrameworkCore.DerivationFrameworkCoreConf import DerivationFramework__DerivationKernel
BPHY20_Sequence += CfgMgr.DerivationFramework__DerivationKernel("BPHY20Kernel",
                                                               AugmentationTools = BPHY20AugmentationTools,
                                                               SkimmingTools     = BPHY20SkimmingTools, 
                                                               ThinningTools     = BPHY20ThinningTools)

#Vital, replaces the adding of kernels directly
DerivationFrameworkJob += BPHY20_Sequence

#====================================================================
# Slimming 
#====================================================================

from DerivationFrameworkCore.SlimmingHelper import SlimmingHelper
BPHY20SlimmingHelper = SlimmingHelper("BPHY20SlimmingHelper")
AllVariables  = []
StaticContent = []



SmartCollections = [
                    "Photons",
                    "Electrons", 
                    "TauJets", 
                    "AntiKt4EMTopoJets_BTagging201810", 
                    "BTagging_AntiKt4EMTopo_201810", 
                    "Muons", 
                    "InDetTrackParticles", 
                    "MET_Reference_AntiKt4EMTopo"
                    ]


AllVariables = ["METAssoc_AntiKt4EMTopo",
                 "MET_Core_AntiKt4EMTopo",
                 "MET_Truth",
                 "MET_Track",
                 "PrimaryVertices",
                 "MET_LocHadTopo"]

AllVariables += ["Kt4EMTopoOriginEventShape",
                 "Kt4EMTopoEventShape"]

AllVariables += ["CombinedMuonTrackParticles",
                 "ExtrapolatedMuonTrackParticles",
                 "MuonSpectrometerTrackParticles"]
                 
AllVariables += ["GSFTrackParticles"]

ExtraVariables = ["Photons.pt.eta.phi.m",
                  "Electrons.pt.eta.phi.m.Tight.Medium.Loose",
                  "TauJets.pt.eta.phi.m.IsTruthMatched.truthJetLink.truthParticleLink",
                  "AntiKt4EMTopoJets_BTagging201810.JetPileupScaleMomentum_pt.JetPileupScaleMomentum_eta.JetPileupScaleMomentum_phi.JetPileupScaleMomentum_m", 
                  "AntiKt4EMTopoJets_BTagging201810.JvtJvfcorr.HECFrac.LArQuality.HECQuality.NegativeE.AverageLArQF", 
                  "AntiKt4EMTopoJets_BTagging201810.JetEtaJESScaleMomentum_pt.JetEtaJESScaleMomentum_eta.JetEtaJESScaleMomentum_phi.JetEtaJESScaleMomentum_m"]

ExtraVariables += ["Muons.etaLayer1Hits.etaLayer2Hits.etaLayer3Hits.etaLayer4Hits.phiLayer1Hits.phiLayer2Hits.phiLayer3Hits.phiLayer4Hits",
                   "Muons.numberOfTriggerEtaLayers.numberOfPhiLayers",
                   "CombinedMuonTrackParticles.numberOfTRTHits.numberOfTRTHighThresholdHits", 
                   "InDetTrackParticles.numberOfTRTHits.numberOfTRTHighThresholdHits.vx.vy.vz",
                   "PrimaryVertices.chiSquared.covariance"]


StaticContent =  ["xAOD::VertexContainer#BPHY20RefittedPrimaryVertices",
                  "xAOD::VertexAuxContainer#BPHY20RefittedPrimaryVerticesAux."]


## Jpsi candidates 
StaticContent += ["xAOD::VertexContainer#%s"        %                 BPHY20JpsiSelectAndWrite.OutputVtxContainerName]
## we have to disable vxTrackAtVertex branch since it is not xAOD compatible
StaticContent += ["xAOD::VertexAuxContainer#%sAux.-vxTrackAtVertex" % BPHY20JpsiSelectAndWrite.OutputVtxContainerName]

## Bc+>J/psi Mu+ candidates
StaticContent += ["xAOD::VertexContainer#%s"        %                 BPHY20BcJpsiMuSelectAndWrite.OutputVtxContainerName]
StaticContent += ["xAOD::VertexAuxContainer#%sAux.-vxTrackAtVertex" % BPHY20BcJpsiMuSelectAndWrite.OutputVtxContainerName]

## Bc+>J/psi E+ candidates
StaticContent += ["xAOD::VertexContainer#%s"        %                 BPHY20BcJpsiESelectAndWrite.OutputVtxContainerName]
StaticContent += ["xAOD::VertexAuxContainer#%sAux.-vxTrackAtVertex" % BPHY20BcJpsiESelectAndWrite.OutputVtxContainerName]

## gamma>e+e- candidates
StaticContent += ["xAOD::VertexContainer#%s"        %                 BPHY20_GammaFinder.ConversionContainerName]
StaticContent += ["xAOD::VertexAuxContainer#%sAux.-vxTrackAtVertex" % BPHY20_GammaFinder.ConversionContainerName]

# Truth information for MC only
if isSimulation:
    AllVariables += ["TruthEvents","TruthParticles","TruthVertices","MuonTruthParticles", "METMap_Truth"]
    SmartCollections += ["AntiKt4TruthJets"] 

# Needed for trigger objects
BPHY20SlimmingHelper.IncludeMuonTriggerContent = True
BPHY20SlimmingHelper.IncludeBPhysTriggerContent = True

# Pass all lists to the SlimmingHelper
BPHY20SlimmingHelper.ExtraVariables = ExtraVariables
BPHY20SlimmingHelper.AllVariables = AllVariables
BPHY20SlimmingHelper.StaticContent = StaticContent
BPHY20SlimmingHelper.SmartCollections = SmartCollections
BPHY20SlimmingHelper.AppendContentToStream(BPHY20Stream)

