#====================================================================
# BPHY5.py
# Bs>J/psiKK 
# It requires the reductionConf flag BPHY5 in Reco_tf.py   
#====================================================================

# Set up common services and job object. 
# This should appear in ALL derivation job options
from DerivationFrameworkCore.DerivationFrameworkMaster import *

isSimulation = False
if globalflags.DataSource()=='geant4':
    isSimulation = True

print isSimulation


#====================================================================
# AUGMENTATION TOOLS 
#====================================================================
## 1/ setup vertexing tools and services
#include( "JpsiUpsilonTools/configureServices.py" )

include("DerivationFrameworkBPhys/configureVertexing.py")
BPHY5_VertexTools = BPHYVertexTools("BPHY5")


from DerivationFrameworkBPhys.DerivationFrameworkBPhysConf import DerivationFramework__AugOriginalCounts
BPHY5_AugOriginalCounts = DerivationFramework__AugOriginalCounts(
   name = "BPHY5_AugOriginalCounts",
   VertexContainer = "PrimaryVertices",
   TrackContainer = "InDetTrackParticles" )
ToolSvc += BPHY5_AugOriginalCounts


#--------------------------------------------------------------------
## 2/ setup JpsiFinder tool
##    These are general tools independent of DerivationFramework that do the 
##    actual vertex fitting and some pre-selection.
from JpsiUpsilonTools.JpsiUpsilonToolsConf import Analysis__JpsiFinder
BPHY5JpsiFinder = Analysis__JpsiFinder(name                         = "BPHY5JpsiFinder",
                                        OutputLevel                 = INFO,
                                        muAndMu                     = True,
                                        muAndTrack                  = False,
                                        TrackAndTrack               = False,
                                        assumeDiMuons               = True, 
                                        invMassUpper                = 3600.0,
                                        invMassLower                = 2600.0,
                                        Chi2Cut                     = 30.,
                                        oppChargesOnly	            = True,
                                        combOnly		            = True,
                                        atLeastOneComb              = False,
                                        useCombinedMeasurement      = False, # Only takes effect if combOnly=True	
                                        muonCollectionKey           = "Muons",
                                        TrackParticleCollection     = "InDetTrackParticles",
                                        V0VertexFitterTool          = BPHY5_VertexTools.TrkV0Fitter,             # V0 vertex fitter
                                        useV0Fitter                 = False,                   # if False a TrkVertexFitterTool will be used
                                        TrkVertexFitterTool         = BPHY5_VertexTools.TrkVKalVrtFitter,        # VKalVrt vertex fitter
                                        TrackSelectorTool           = BPHY5_VertexTools.InDetTrackSelectorTool,
                                        ConversionFinderHelperTool  = BPHY5_VertexTools.InDetConversionHelper,
                                        VertexPointEstimator        = BPHY5_VertexTools.VtxPointEstimator,
                                        useMCPCuts                  = False)
ToolSvc += BPHY5JpsiFinder
print      BPHY5JpsiFinder

#--------------------------------------------------------------------
## 3/ setup the vertex reconstruction "call" tool(s). They are part of the derivation framework.
##    These Augmentation tools add output vertex collection(s) into the StoreGate and add basic 
##    decorations which do not depend on the vertex mass hypothesis (e.g. lxy, ptError, etc).
##    There should be one tool per topology, i.e. Jpsi and Psi(2S) do not need two instance of the
##    Reco tool is the JpsiFinder mass window is wide enough.
from DerivationFrameworkBPhys.DerivationFrameworkBPhysConf import DerivationFramework__Reco_mumu
BPHY5JpsiSelectAndWrite = DerivationFramework__Reco_mumu(name                 = "BPHY5JpsiSelectAndWrite",
                                                       JpsiFinder             = BPHY5JpsiFinder,
                                                       OutputVtxContainerName = "BPHY5JpsiCandidates",
                                                       PVContainerName        = "PrimaryVertices",
                                                       RefPVContainerName     = "SHOULDNOTBEUSED",
                                                       DoVertexType           =1)
ToolSvc += BPHY5JpsiSelectAndWrite
print BPHY5JpsiSelectAndWrite


from DerivationFrameworkBPhys.DerivationFrameworkBPhysConf import DerivationFramework__Select_onia2mumu

## a/ augment and select Jpsi->mumu candidates
BPHY5_Select_Jpsi2mumu = DerivationFramework__Select_onia2mumu(
  name                  = "BPHY5_Select_Jpsi2mumu",
  HypothesisName        = "Jpsi",
  InputVtxContainerName = "BPHY5JpsiCandidates",
  VtxMassHypo           = 3096.916,
  MassMin               = 2000.0,
  MassMax               = 3600.0,
  Chi2Max               = 200, Do3d = False,
  DoVertexType =1)

  
ToolSvc += BPHY5_Select_Jpsi2mumu
print      BPHY5_Select_Jpsi2mumu




## 4/ setup a new vertexing tool (necessary due to use of mass constraint) 
from TrkVKalVrtFitter.TrkVKalVrtFitterConf import Trk__TrkVKalVrtFitter
BsKKVertexFit = Trk__TrkVKalVrtFitter(
                                         name                = "BsKKVertexFit",
                                         Extrapolator        = BPHY5_VertexTools.InDetExtrapolator,
                                         FirstMeasuredPoint  = False,
                                         MakeExtendedVertex  = True)
ToolSvc += BsKKVertexFit
print      BsKKVertexFit

from TrkVKalVrtFitter.TrkVKalVrtFitterConf import Trk__TrkVKalVrtFitter
BplKplVertexFit = Trk__TrkVKalVrtFitter(
                                         name                = "BplKplVertexFit",
                                         Extrapolator        = BPHY5_VertexTools.InDetExtrapolator,
                                         FirstMeasuredPoint  = False,
                                         MakeExtendedVertex  = True)
ToolSvc += BplKplVertexFit
print      BplKplVertexFit

#Add the B to pi pi Jpsi X final states
from TrkVKalVrtFitter.TrkVKalVrtFitterConf import Trk__TrkVKalVrtFitter
BpipiXVertexFit = Trk__TrkVKalVrtFitter(
                                         name                = "BpipiXVertexFit",
                                         Extrapolator        = BPHY5_VertexTools.InDetExtrapolator,
                                         FirstMeasuredPoint  = False,
                                         MakeExtendedVertex  = True)
ToolSvc += BpipiXVertexFit
print      BpipiXVertexFit

## 5/ setup the Jpsi+2 track finder
from JpsiUpsilonTools.JpsiUpsilonToolsConf import Analysis__JpsiPlus2Tracks
BPHY5BsJpsiKK = Analysis__JpsiPlus2Tracks(name = "BPHY5BsJpsiKK",
                                        OutputLevel = INFO,
kaonkaonHypothesis			= True,
pionpionHypothesis                      = False,
kaonpionHypothesis                      = False,
trkThresholdPt			        = 800.0,
trkMaxEta				    = 3.0,
BMassUpper				    = 5800.0,
BMassLower				    = 5000.0,
#DiTrackMassUpper = 1019.445 + 100.,
#DiTrackMassLower = 1019.445 - 100.,
Chi2Cut                     = 15.0,
TrkQuadrupletMassUpper      = 6000.0,
TrkQuadrupletMassLower      = 4800.0,
JpsiContainerKey		    = "BPHY5JpsiCandidates",
TrackParticleCollection     = "InDetTrackParticles",
MuonsUsedInJpsi			    = "Muons",
TrkVertexFitterTool		    = BsKKVertexFit,
TrackSelectorTool		    = BPHY5_VertexTools.InDetTrackSelectorTool,
UseMassConstraint		    = True)
        
ToolSvc += BPHY5BsJpsiKK
print      BPHY5BsJpsiKK    

from JpsiUpsilonTools.JpsiUpsilonToolsConf import Analysis__JpsiPlus2Tracks
BPHY5BdJpsiKst = Analysis__JpsiPlus2Tracks(
    name                    = "BPHY5BdJpsiKst",
    OutputLevel             = INFO,
    kaonkaonHypothesis      = False,
    pionpionHypothesis      = False,
    kaonpionHypothesis      = True,
    trkThresholdPt          = 800.0,
    trkMaxEta               = 3.0,
    BThresholdPt            = 5000.,
    BMassLower              = 4300.0,
    BMassUpper              = 6300.0,
    JpsiContainerKey        = "BPHY5JpsiCandidates",
    TrackParticleCollection = "InDetTrackParticles",
    #MuonsUsedInJpsi      = "Muons", #Don't remove all muons, just those in J/psi candidate (see the following cut)
    ExcludeCrossJpsiTracks  = False,   #setting this to False rejects the muons from J/psi candidate
    TrkVertexFitterTool     = BsKKVertexFit,
    TrackSelectorTool     = BPHY5_VertexTools.InDetTrackSelectorTool,
    VertexPointEstimator        = BPHY5_VertexTools.VtxPointEstimator,
    UseMassConstraint     = True,
    Chi2Cut                 = 15.0,
    TrkQuadrupletMassLower  = 3500.0,
    TrkQuadrupletMassUpper  = 6800.0,
    )

ToolSvc += BPHY5BdJpsiKst
print      BPHY5BdJpsiKst


## 5a/ setup the Jpsi+1 track finder
from JpsiUpsilonTools.JpsiUpsilonToolsConf import Analysis__JpsiPlus1Track
BPHY5BplJpsiKpl = Analysis__JpsiPlus1Track(name = "BPHY5BplJpsiKpl",
OutputLevel             = INFO,#DEBUG,
pionHypothesis			= True,
kaonHypothesis			= True,
trkThresholdPt			= 750.0,
trkMaxEta			= 3.0,
BThresholdPt			= 4000.0,
BMassUpper			= 7000.0,
BMassLower			= 4500.0,
Chi2Cut                         = 15.0,
TrkTrippletMassUpper            = 8000,
TrkTrippletMassLower            = 4000,
JpsiContainerKey		= "BPHY5JpsiCandidates",
TrackParticleCollection         = "InDetTrackParticles",
MuonsUsedInJpsi			= "Muons",
TrkVertexFitterTool		= BplKplVertexFit,
TrackSelectorTool		= BPHY5_VertexTools.InDetTrackSelectorTool,
UseMassConstraint		= True,
ExcludeCrossJpsiTracks              = False,
ExcludeJpsiMuonsOnly                = True)
        
ToolSvc += BPHY5BplJpsiKpl
print      BPHY5BplJpsiKpl    

## 5b/ setup the Jpsi+pi+pi+X track finder
from JpsiUpsilonTools.JpsiUpsilonToolsConf import Analysis__JpsiPlus2Tracks
BPHY5BJpsipipiX = Analysis__JpsiPlus2Tracks(name = "BPHY5BJpsipipiX",
                                        OutputLevel = INFO,
kaonkaonHypothesis			= False,
pionpionHypothesis                      = True,
kaonpionHypothesis                      = False,
trkThresholdPt			        = 800.0,
trkMaxEta				    = 3.0,
BMassUpper				    = 5800.0,
BMassLower				    = 3400.0,
#DiTrackMassUpper = 1019.445 + 100.,
#DiTrackMassLower = 1019.445 - 100.,
Chi2Cut                     = 15.0,
TrkQuadrupletMassUpper      = 5800.0,
TrkQuadrupletMassLower      = 3400.0,
JpsiContainerKey		    = "BPHY5JpsiCandidates",
TrackParticleCollection     = "InDetTrackParticles",
MuonsUsedInJpsi			    = "Muons",
TrkVertexFitterTool		    = BpipiXVertexFit,
TrackSelectorTool		    = BPHY5_VertexTools.InDetTrackSelectorTool,
UseMassConstraint		    = True,
ExcludeCrossJpsiTracks              = False,
ExcludeJpsiMuonsOnly                = True)

ToolSvc += BPHY5BJpsipipiX
print      BPHY5BJpsipipiX

## 6/ setup the combined augmentation/skimming tool for the Bpm
from DerivationFrameworkBPhys.DerivationFrameworkBPhysConf import DerivationFramework__Reco_dimuTrkTrk	
BPHY5BsKKSelectAndWrite = DerivationFramework__Reco_dimuTrkTrk(name                 = "BPHY5BsKKSelectAndWrite",
                                                           Jpsi2PlusTrackName       = BPHY5BsJpsiKK,
                                                           OutputVtxContainerName   = "BPHY5BsJpsiKKCandidates",
                                                           PVContainerName          = "PrimaryVertices",
                                                           RefPVContainerName       = "BPHY5RefittedPrimaryVertices",
                                                           RefitPV                  = True, Do3d = False,
                                                           MaxPVrefit               = 10000, DoVertexType = 7)
ToolSvc += BPHY5BsKKSelectAndWrite 
print      BPHY5BsKKSelectAndWrite

from DerivationFrameworkBPhys.DerivationFrameworkBPhysConf import DerivationFramework__Reco_dimuTrk
BPHY5BplKplSelectAndWrite = DerivationFramework__Reco_dimuTrk(name				     	= "BPHY5BplKplSelectAndWrite",
															  Jpsi1PlusTrackName	    =  BPHY5BplJpsiKpl,
															  OutputVtxContainerName 	= "BPHY5BpmJpsiKpmCandidates",
                                                              PVContainerName           = "PrimaryVertices",
                                                              RefPVContainerName        = "BPHY5RefBplJpsiKplPrimaryVertices",                                                              
                                                              RefitPV                   = True,
                                                              MaxPVrefit                = 10000 )
ToolSvc += BPHY5BplKplSelectAndWrite
print      BPHY5BplKplSelectAndWrite

from DerivationFrameworkBPhys.DerivationFrameworkBPhysConf import DerivationFramework__Reco_dimuTrkTrk	
BPHY5BpipiXSelectAndWrite = DerivationFramework__Reco_dimuTrkTrk(name                 = "BPHY5BpipiXSelectAndWrite",
                                                           Jpsi2PlusTrackName       = BPHY5BJpsipipiX,
                                                           OutputVtxContainerName   = "BPHY5BJpsipipiXCandidates",
                                                           PVContainerName          = "PrimaryVertices",
                                                           RefPVContainerName       = "BPHY5RefittedBPipiPrimaryVertices",
                                                           RefitPV                  = True, Do3d = False,
                                                           MaxPVrefit               = 10000, DoVertexType = 7)
ToolSvc += BPHY5BpipiXSelectAndWrite
print      BPHY5BpipiXSelectAndWrite

BPHY5BdKstSelectAndWrite  = DerivationFramework__Reco_dimuTrkTrk(
    name                   = "BPHY5BdKstSelectAndWrite",
    Jpsi2PlusTrackName     = BPHY5BdJpsiKst,
    OutputVtxContainerName = "BPHY5BdJpsiKstCandidates",
    PVContainerName        = "PrimaryVertices",
    RefPVContainerName     = "BPHY5RefittedKstPrimaryVertices",
    RefitPV                = True,
    MaxPVrefit             = 10000,
    DoVertexType = 7)
ToolSvc += BPHY5BdKstSelectAndWrite
print BPHY5BdKstSelectAndWrite
## b/ augment and select Bd->JpsiKst candidates
#  set mass hypothesis (K pi)
BPHY5_Select_Bd2JpsiKst = DerivationFramework__Select_onia2mumu(
    name                       = "BPHY5_Select_Bd2JpsiKst",
    HypothesisName             = "Bd",
    InputVtxContainerName      = "BPHY5BdJpsiKstCandidates",
    TrkMasses                  = [105.658, 105.658, 493.677, 139.570],
    VtxMassHypo                = 5279.6,
    MassMin                    = 100.0,      #no mass cuts here
    MassMax                    = 100000.0,   #no mass cuts here
    Chi2Max                    = 200)

ToolSvc += BPHY5_Select_Bd2JpsiKst
print      BPHY5_Select_Bd2JpsiKst

## c/ augment and select Bdbar->JpsiKstbar candidates
# set mass hypothesis (pi K)
BPHY5_Select_Bd2JpsiKstbar = DerivationFramework__Select_onia2mumu(
    name                       = "BPHY5_Select_Bd2JpsiKstbar",
    HypothesisName             = "Bdbar",
    InputVtxContainerName      = "BPHY5BdJpsiKstCandidates",
    TrkMasses                  = [105.658, 105.658, 139.570, 493.677],
    VtxMassHypo                = 5279.6,
    MassMin                    = 100.0,      #no mass cuts here
    MassMax                    = 100000.0,   #no mass cuts here
    Chi2Max                    = 200)

ToolSvc += BPHY5_Select_Bd2JpsiKstbar
print      BPHY5_Select_Bd2JpsiKstbar


## b/ augment and select Bs->JpsiKK candidates
BPHY5_Select_Bs2JpsiKK = DerivationFramework__Select_onia2mumu(
  name                       = "BPHY5_Select_Bs2JpsiKK",
  HypothesisName             = "Bs",
  InputVtxContainerName      = "BPHY5BsJpsiKKCandidates",
  TrkMasses                  = [105.658, 105.658, 493.677, 493.677],
  VtxMassHypo                = 5366.3,
  MassMin                    = 5000.0,
  MassMax                    = 5800.0, Do3d = False,
  Chi2Max                    = 200)

ToolSvc += BPHY5_Select_Bs2JpsiKK
print      BPHY5_Select_Bs2JpsiKK

BPHY5_Select_Bpl2JpsiKpl     = DerivationFramework__Select_onia2mumu(
  name                       = "BPHY5_Select_Bpl2JpsiKpl",
  HypothesisName             = "Bplus",
  InputVtxContainerName      = "BPHY5BpmJpsiKpmCandidates",
  TrkMasses                  = [105.658, 105.658, 493.677],
  VtxMassHypo                = 5279.26,
  MassMin                    = 5279.26 - 500, Do3d = False,
  MassMax                    = 5279.26 + 500,
  Chi2Max                    = 200 )

ToolSvc += BPHY5_Select_Bpl2JpsiKpl
print      BPHY5_Select_Bpl2JpsiKpl

BPHY5_Select_Bpl2JpsiPi      = DerivationFramework__Select_onia2mumu(
  name                       = "BPHY5_Select_Bpl2JpsiPi",
  HypothesisName             = "Bc",
  InputVtxContainerName      = "BPHY5BpmJpsiKpmCandidates",
  TrkMasses                  = [105.658, 105.658, 139.570],
  VtxMassHypo                = 6275.1, Do3d = False,
  MassMin                    = 6275.1 - 500,
  MassMax                    = 6275.1 + 500,
  Chi2Max                    = 200 )

ToolSvc += BPHY5_Select_Bpl2JpsiPi
print      BPHY5_Select_Bpl2JpsiPi

BPHY5_Select_B2JpsipipiX = DerivationFramework__Select_onia2mumu(
  name                       = "BPHY5_Select_B2JpsipipiX",
  HypothesisName             = "pipiJpsi",
  InputVtxContainerName      = "BPHY5BJpsipipiXCandidates",
  TrkMasses                  = [105.658, 105.658, 139.570, 139.570],
  VtxMassHypo                = 4260,
  MassMin                    = 3400.0,
  MassMax                    = 5800.0, Do3d = False,
  Chi2Max                    = 200)

ToolSvc += BPHY5_Select_B2JpsipipiX
print      BPHY5_Select_B2JpsipipiX

#expression = "count(BPHY5BpmJpsiKpmCandidates.passed_Bplus) > 0"
#from DerivationFrameworkTools.DerivationFrameworkToolsConf import DerivationFramework__xAODStringSkimmingTool
#BPHY5_SelectEvent = DerivationFramework__xAODStringSkimmingTool(name = "BPHY5_SelectEvent",
#                                                                expression = expression)
#ToolSvc += BPHY5_SelectEvent
#print BPHY5_SelectEvent


#from DerivationFrameworkBPhys.DerivationFrameworkBPhysConf import DerivationFramework__SelectEvent

if not isSimulation: #Only Skim Data
   from DerivationFrameworkTools.DerivationFrameworkToolsConf import DerivationFramework__xAODStringSkimmingTool
   BPHY5_SelectBsJpsiKKEvent = DerivationFramework__xAODStringSkimmingTool(
     name = "BPHY5_SelectBsJpsiKKEvent",
     expression = "count(BPHY5BsJpsiKKCandidates.passed_Bs > 0) > 0")
                                                    
   ToolSvc += BPHY5_SelectBsJpsiKKEvent
   print BPHY5_SelectBsJpsiKKEvent

   BPHY5_SelectBplJpsiKplEvent = DerivationFramework__xAODStringSkimmingTool(name = "BPHY5_SelectBplJpsiKplEvent",
                                                                    expression = "count(BPHY5BpmJpsiKpmCandidates.passed_Bplus>0) > 0")
   ToolSvc += BPHY5_SelectBplJpsiKplEvent
   print      BPHY5_SelectBplJpsiKplEvent

   BPHY5_SelectBplJpsiKplEventBc = DerivationFramework__xAODStringSkimmingTool(name = "BPHY5_SelectBplJpsiKplEventBc",
                                                                    expression = "count(BPHY5BpmJpsiKpmCandidates.passed_Bc>0) > 0")
   ToolSvc += BPHY5_SelectBplJpsiKplEventBc
   print      BPHY5_SelectBplJpsiKplEventBc
   
   BPHY5_SelectBdKstarEventBd = DerivationFramework__xAODStringSkimmingTool(name = "BPHY5_SelectBdKstarEventBd",
                                                                    expression = "count(BPHY5BdJpsiKstCandidates.passed_Bd>0) > 0")
   ToolSvc += BPHY5_SelectBdKstarEventBd
   print      BPHY5_SelectBdKstarEventBd

   BPHY5_SelectBdKstarEventBdBar = DerivationFramework__xAODStringSkimmingTool(name = "BPHY5_SelectBdKstarEventBdbar",
                                                                    expression = "count(BPHY5BdJpsiKstCandidates.passed_Bdbar>0) > 0")
   ToolSvc += BPHY5_SelectBdKstarEventBdBar
   print      BPHY5_SelectBdKstarEventBdBar
   #====================================================================
   # Make event selection based on an OR of the input skimming tools
   #====================================================================
   from DerivationFrameworkTools.DerivationFrameworkToolsConf import DerivationFramework__FilterCombinationOR
   BPHY5SkimmingOR = CfgMgr.DerivationFramework__FilterCombinationOR("BPHY5SkimmingOR",
                                         FilterList = [BPHY5_SelectBsJpsiKKEvent, BPHY5_SelectBplJpsiKplEvent, BPHY5_SelectBplJpsiKplEventBc,
                                         BPHY5_SelectBdKstarEventBd, BPHY5_SelectBdKstarEventBdBar])
   ToolSvc += BPHY5SkimmingOR
   print      BPHY5SkimmingOR

from DerivationFrameworkBPhys.DerivationFrameworkBPhysConf import DerivationFramework__Thin_vtxTrk
BPHY5_thinningTool_Tracks = DerivationFramework__Thin_vtxTrk(
  name                       = "BPHY5_thinningTool_Tracks",
  ThinningService            = "BPHY5ThinningSvc",
  TrackParticleContainerName = "InDetTrackParticles",
  VertexContainerNames       = ["BPHY5BsJpsiKKCandidates", "BPHY5BpmJpsiKpmCandidates"],
  PassFlags                  = ["passed_Bs", "passed_Bplus", "passed_Bc"] )

ToolSvc += BPHY5_thinningTool_Tracks

from DerivationFrameworkBPhys.DerivationFrameworkBPhysConf import DerivationFramework__BPhysPVThinningTool
BPHY5_thinningTool_PV = DerivationFramework__BPhysPVThinningTool(
  name                       = "BPHY5_thinningTool_PV",
  ThinningService            = "BPHY5ThinningSvc",
  CandidateCollections       = ["BPHY5BsJpsiKKCandidates", "BPHY5BpmJpsiKpmCandidates"],
  KeepPVTracks  =True
 )

ToolSvc += BPHY5_thinningTool_PV


## b) thinning out tracks that are not attached to muons. The final thinning decision is based on the OR operation
##    between decision from this and the previous tools.
from DerivationFrameworkInDet.DerivationFrameworkInDetConf import DerivationFramework__MuonTrackParticleThinning
BPHY5MuonTPThinningTool = DerivationFramework__MuonTrackParticleThinning(name                    = "BPHY5MuonTPThinningTool",
                                                                         ThinningService         = "BPHY5ThinningSvc",
                                                                         MuonKey                 = "Muons",
                                                                         InDetTrackParticlesKey  = "InDetTrackParticles")
ToolSvc += BPHY5MuonTPThinningTool

from DerivationFrameworkInDet.DerivationFrameworkInDetConf import DerivationFramework__EgammaTrackParticleThinning
BPHY5ElectronTPThinningTool = DerivationFramework__EgammaTrackParticleThinning(  
    name                    = "BPHY5ElectronTPThinningTool",
    ThinningService         = "BPHY5ThinningSvc",
    SGKey                   = "Electrons",
    GSFTrackParticlesKey = "GSFTrackParticles",        
    InDetTrackParticlesKey  = "InDetTrackParticles",
    SelectionString = "",
    BestMatchOnly = True,
    ConeSize = 0.3,
    ApplyAnd = False)

ToolSvc+=BPHY5ElectronTPThinningTool
#====================================================================
# CREATE THE DERIVATION KERNEL ALGORITHM AND PASS THE ABOVE TOOLS  
#====================================================================

thiningCollection = [] 
print thiningCollection

import DerivationFrameworkJetEtMiss.JetCommon
bphy5Seq = CfgMgr.AthSequencer("BPHY5Sequence")
DerivationFrameworkJob += bphy5Seq

# The name of the kernel (BPHY5Kernel in this case) must be unique to this derivation
from DerivationFrameworkCore.DerivationFrameworkCoreConf import DerivationFramework__DerivationKernel
bphy5Seq += CfgMgr.DerivationFramework__DerivationKernel("BPHY5Kernel",
                                                                       AugmentationTools = [BPHY5JpsiSelectAndWrite,  BPHY5_Select_Jpsi2mumu,
                                                                                            BPHY5BsKKSelectAndWrite,  BPHY5_Select_Bs2JpsiKK,
                                                                                            BPHY5BplKplSelectAndWrite, BPHY5BpipiXSelectAndWrite, BPHY5_Select_Bpl2JpsiKpl, BPHY5_Select_Bpl2JpsiPi, BPHY5_Select_B2JpsipipiX,
                                                                                            BPHY5BdKstSelectAndWrite, BPHY5_Select_Bd2JpsiKst, BPHY5_Select_Bd2JpsiKstbar,
                                                                                            BPHY5_AugOriginalCounts],
                                                                       #Only skim if not MC
                                                                       SkimmingTools     = [BPHY5SkimmingOR] if not isSimulation else [],
                                                                       ThinningTools     = thiningCollection
                                                                       
                                                                       )

#====================================================================
# SET UP STREAM   
#====================================================================
streamName   = derivationFlags.WriteDAOD_BPHY5Stream.StreamName
fileName     = buildFileName( derivationFlags.WriteDAOD_BPHY5Stream )
BPHY5Stream  = MSMgr.NewPoolRootStream( streamName, fileName )
BPHY5Stream.AcceptAlgs(["BPHY5Kernel"])

# Special lines for thinning
# Thinning service name must match the one passed to the thinning tools
from AthenaServices.Configurables import ThinningSvc, createThinningSvc
augStream = MSMgr.GetStream( streamName )
evtStream = augStream.GetEventStream()

BPHY5ThinningSvc = createThinningSvc( svcName="BPHY5ThinningSvc", outStreams=[evtStream] )
svcMgr += BPHY5ThinningSvc

#====================================================================
# Slimming 
#====================================================================

# Added by ASC
from DerivationFrameworkCore.SlimmingHelper import SlimmingHelper
BPHY5SlimmingHelper = SlimmingHelper("BPHY5SlimmingHelper")
AllVariables  = []
StaticContent = []

# Needed for trigger objects
BPHY5SlimmingHelper.IncludeMuonTriggerContent  = TRUE
BPHY5SlimmingHelper.IncludeBPhysTriggerContent = TRUE

## primary vertices
AllVariables  += ["PrimaryVertices"]
StaticContent += ["xAOD::VertexContainer#BPHY5RefittedPrimaryVertices"]
StaticContent += ["xAOD::VertexAuxContainer#BPHY5RefittedPrimaryVerticesAux."]
StaticContent += ["xAOD::VertexContainer#BPHY5RefBplJpsiKplPrimaryVertices"]
StaticContent += ["xAOD::VertexAuxContainer#BPHY5RefBplJpsiKplPrimaryVerticesAux."]
StaticContent += ["xAOD::VertexContainer#BPHY5RefittedBPipiPrimaryVertices"]
StaticContent += ["xAOD::VertexAuxContainer#BPHY5RefittedBPipiPrimaryVerticesAux."]
StaticContent += ["xAOD::VertexContainer#BPHY5RefittedKstPrimaryVertices"]
StaticContent += ["xAOD::VertexAuxContainer#BPHY5RefittedKstPrimaryVerticesAux."]

## ID track particles
AllVariables += ["InDetTrackParticles"]

## combined / extrapolated muon track particles 
## (note: for tagged muons there is no extra TrackParticle collection since the ID tracks
##        are store in InDetTrackParticles collection)
AllVariables += ["CombinedMuonTrackParticles"]
AllVariables += ["ExtrapolatedMuonTrackParticles"]

## muon container
AllVariables += ["Muons"] 


## Jpsi candidates 
StaticContent += ["xAOD::VertexContainer#%s"        %                 BPHY5JpsiSelectAndWrite.OutputVtxContainerName]
## we have to disable vxTrackAtVertex branch since it is not xAOD compatible
StaticContent += ["xAOD::VertexAuxContainer#%sAux.-vxTrackAtVertex" % BPHY5JpsiSelectAndWrite.OutputVtxContainerName]

StaticContent += ["xAOD::VertexContainer#%s"        %                 BPHY5BsKKSelectAndWrite.OutputVtxContainerName]
StaticContent += ["xAOD::VertexAuxContainer#%sAux.-vxTrackAtVertex" % BPHY5BsKKSelectAndWrite.OutputVtxContainerName]

StaticContent += ["xAOD::VertexContainer#%s"        %                 BPHY5BplKplSelectAndWrite.OutputVtxContainerName]
StaticContent += ["xAOD::VertexAuxContainer#%sAux.-vxTrackAtVertex" % BPHY5BplKplSelectAndWrite.OutputVtxContainerName]

StaticContent += ["xAOD::VertexContainer#%s"        %                 BPHY5BpipiXSelectAndWrite.OutputVtxContainerName]
StaticContent += ["xAOD::VertexAuxContainer#%sAux.-vxTrackAtVertex" % BPHY5BpipiXSelectAndWrite.OutputVtxContainerName]

StaticContent += ["xAOD::VertexContainer#%s"        %                 BPHY5BdKstSelectAndWrite.OutputVtxContainerName]
StaticContent += ["xAOD::VertexAuxContainer#%sAux.-vxTrackAtVertex" % BPHY5BdKstSelectAndWrite.OutputVtxContainerName]

# Tagging information (in addition to that already requested by usual algorithms)
#AllVariables += ["Electrons"] 
AllVariables += ["GSFTrackParticles", "Electrons" , "Photons", "MuonSpectrometerTrackParticles" ]
tagJetCollections = ['AntiKt4LCTopoJets', 'AntiKt4EMTopoJets', 'AntiKt4PV0TrackJets']

AllVariables += [ "Kt4LCTopoOriginEventShape", "Kt4EMTopoOriginEventShape" ]
SmartVar = ["Photons" ] #[ tagJetCollections ]




for jet_collection in tagJetCollections:
    AllVariables   += [jet_collection]
    AllVariables   += ["BTagging_%s"       % (jet_collection[:-4]) ]
    AllVariables   += ["BTagging_%sJFVtx"  % (jet_collection[:-4]) ]
    AllVariables   += ["BTagging_%sSecVtx" % (jet_collection[:-4]) ]

#addStandardJets("AntiKt", 0.4, "PV0Track", 2000, mods="track_ungroomed", algseq=bphy5Seq, outputGroup="BPHY5")


# Added by ASC
# Truth information for MC only
if isSimulation:
    AllVariables += ["TruthEvents","TruthParticles","TruthVertices","MuonTruthParticles", "egammaTruthParticles" ]
    AllVariables += ["AntiKt4TruthJets", "AntiKt4TruthWZJets" ]
#    addStandardJets("AntiKt", 0.4, "Truth", 5000, mods="truth_ungroomed", algseq=bphy5Seq, outputGroup="BPHY5")
#    addStandardJets("AntiKt", 0.4, "TruthWZ", 5000, mods="truth_ungroomed", algseq=bphy5Seq, outputGroup="BPHY5")
    tagJetCollections += [ "AntiKt4TruthJets", "AntiKt4TruthWZJets"  ]

from DerivationFrameworkJetEtMiss.ExtendedJetCommon import replaceAODReducedJets
replaceAODReducedJets(tagJetCollections, bphy5Seq  ,  "BPHY5" )


AllVariables = list(set(AllVariables)) # remove duplicates

BPHY5SlimmingHelper.AllVariables = AllVariables
BPHY5SlimmingHelper.StaticContent = StaticContent
BPHY5SlimmingHelper.SmartCollections = SmartVar

BPHY5SlimmingHelper.AppendContentToStream(BPHY5Stream)


