#2018/11/24 BPHYS22
#====================================================================
# BPHY22.py

# B -> mu D_s+, B -> mu D+, B -> mu D*+, B -> mu Lambda_c
# It requires the reductionConf flag BPHY22 in Reco_tf.py
#====================================================================

# Set up common services and job object.
# This should appear in ALL derivation job options
from DerivationFrameworkCore.DerivationFrameworkMaster import *

# data or simulation?
isSimulation = False
if globalflags.DataSource()=='geant4':
    isSimulation = True

print isSimulation


#====================================================================
# AUGMENTATION TOOLS
#====================================================================
## 1/ setup vertexing tools and services
include("DerivationFrameworkBPhys/configureVertexing.py")
BPHY22_VertexTools = BPHYVertexTools("BPHY22")


from DerivationFrameworkBPhys.DerivationFrameworkBPhysConf import DerivationFramework__AugOriginalCounts
BPHY22_AugOriginalCounts = DerivationFramework__AugOriginalCounts(
   name = "BPHY22_AugOriginalCounts",
   VertexContainer = "PrimaryVertices",
   TrackContainer = "InDetTrackParticles" )
ToolSvc += BPHY22_AugOriginalCounts

#===============================================================================================
#--------------------------------------------------------------------
# 1/ Select  mu pi
#--------------------------------------------------------------------
## a/ setup JpsiFinder tool
##    These are general tools independent of DerivationFramework that do the
##    actual vertex fitting and some pre-selection.
from JpsiUpsilonTools.JpsiUpsilonToolsConf import Analysis__JpsiFinder
BPHY22MuPiFinder = Analysis__JpsiFinder(
    name                       = "BPHY22MuPiFinder",
    OutputLevel                = INFO,
    muAndMu                    = False,
    muAndTrack                 = True,  #need doTagAndProbe flag
    TrackAndTrack              = False,
    assumeDiMuons              = False,
    muonThresholdPt            = 2700,
    trackThresholdPt           = 250.0, # MeV
    invMassUpper               = 8200.0,
    invMassLower               = 200.0,
    Chi2Cut                    = 10.,
    oppChargesOnly             = False,
    allChargeCombinations      = True,
   # combOnly                   = False,
    atLeastOneComb             = False, # True by default
    useCombinedMeasurement     = False, # Only takes effect if combOnly=True
    muonCollectionKey          = "Muons",
    TrackParticleCollection    = "InDetTrackParticles",
    V0VertexFitterTool         = BPHY22_VertexTools.TrkV0Fitter,             # V0 vertex fitter
    useV0Fitter                = False,                   # if False a TrkVertexFitterTool will be used
    TrkVertexFitterTool        = BPHY22_VertexTools.TrkVKalVrtFitter,        # VKalVrt vertex fitter
    TrackSelectorTool          = BPHY22_VertexTools.InDetTrackSelectorTool,
    ConversionFinderHelperTool = BPHY22_VertexTools.InDetConversionHelper,
    VertexPointEstimator       = BPHY22_VertexTools.VtxPointEstimator,
    useMCPCuts                 = False,
    doTagAndProbe              = True, #won't work with all/same charges combs
    forceTagAndProbe           = True) #force T&P to work with any charges combs

ToolSvc += BPHY22MuPiFinder
print      BPHY22MuPiFinder

#--------------------------------------------------------------------
## b/ setup the vertex reconstruction "call" tool(s). They are part of the derivation framework.
##    These Augmentation tools add output vertex collection(s) into the StoreGate and add basic
##    decorations which do not depend on the vertex mass hypothesis (e.g. lxy, ptError, etc).
##    There should be one tool per topology, i.e. Jpsi and Psi(2S) do not need two instance of the
##    Reco tool is the JpsiFinder mass window is wide enough.
from DerivationFrameworkBPhys.DerivationFrameworkBPhysConf import DerivationFramework__Reco_mumu
BPHY22MuPiSelectAndWrite = DerivationFramework__Reco_mumu(
    name                   = "BPHY22MuPiSelectAndWrite",
    JpsiFinder             = BPHY22MuPiFinder,
    OutputVtxContainerName = "BPHY22MuPiCandidates",
    PVContainerName        = "PrimaryVertices",
    RefPVContainerName     = "SHOULDNOTBEUSED")

ToolSvc +=  BPHY22MuPiSelectAndWrite
print       BPHY22MuPiSelectAndWrite

#--------------------------------------------------------------------
## c/ augment and select mu pi candidates
from DerivationFrameworkBPhys.DerivationFrameworkBPhysConf import DerivationFramework__Select_onia2mumu
BPHY22_Select_MuPi = DerivationFramework__Select_onia2mumu(
    name                  = "BPHY22_Select_MuPi",
    HypothesisName        = "MuPi",
    InputVtxContainerName = "BPHY22MuPiCandidates",
 #   TrkMasses             = [105.658, 139.571],
    VtxMassHypo           = 5279.64,
    MassMin               = 200.0,
    MassMax               = 8200.0,
    Chi2Max               = 200)
    #LxyMin                = -100, #default lowest of Double

ToolSvc += BPHY22_Select_MuPi
print      BPHY22_Select_MuPi
##
#===============================================================================================


#===============================================================================================
##
#--------------------------------------------------------------------
# 2/ Select K+K-, pi+K- and K+pi-      for D0, Ds, Dm
#--------------------------------------------------------------------
## a/ Setup the vertex fitter tools
BPHY22DiTrkFinder = Analysis__JpsiFinder(
    name                       = "BPHY22DiTrkFinder",
    OutputLevel                = INFO,
    muAndMu                    = False,
    muAndTrack                 = False,
    TrackAndTrack              = True,
    assumeDiMuons              = False,    # If true, will assume dimu hypothesis and use PDG value for mu mass
    trackThresholdPt           = 900,
    invMassUpper               = 2100.0,
    invMassLower               = 275,
    Chi2Cut                    = 20., #chi2
    oppChargesOnly             = True,
    atLeastOneComb             = False,
    useCombinedMeasurement     = False, # Only takes effect if combOnly=True
    muonCollectionKey          = "Muons",
    TrackParticleCollection    = "InDetTrackParticles",
    V0VertexFitterTool         = BPHY22_VertexTools.TrkV0Fitter,             # V0 vertex fitter
    useV0Fitter                = False,                   # if False a TrkVertexFitterTool will be used
    TrkVertexFitterTool        = BPHY22_VertexTools.TrkVKalVrtFitter,        # VKalVrt vertex fitter
    TrackSelectorTool          = BPHY22_VertexTools.InDetTrackSelectorTool,
    ConversionFinderHelperTool = BPHY22_VertexTools.InDetConversionHelper,
    VertexPointEstimator       = BPHY22_VertexTools.VtxPointEstimator,
    useMCPCuts                 = False,
    track1Mass                 = 139.571, # Not very important, only used to calculate inv. mass cut, leave it loose here
    track2Mass                 = 139.571)
  
ToolSvc += BPHY22DiTrkFinder
print      BPHY22DiTrkFinder

#--------------------------------------------------------------------
## b/ setup the vertex reconstruction "call" tool(s).
BPHY22DiTrkSelectAndWrite = DerivationFramework__Reco_mumu(
    name                   = "BPHY22DiTrkSelectAndWrite",
    JpsiFinder             = BPHY22DiTrkFinder,
    OutputVtxContainerName = "BPHY22DiTrkCandidates",
    PVContainerName        = "PrimaryVertices",
    #RefPVContainerName     = "BPHY22RefittedPrimaryVertices",#"SHOULDNOTBEUSED",
    RefPVContainerName     = "SHOULDNOTBEUSED",
    #RefitPV                = True,
    #MaxPVrefit             = 100000,
    CheckCollections       = True,
    CheckVertexContainers  = ['BPHY22MuPiCandidates'])
  
ToolSvc += BPHY22DiTrkSelectAndWrite
print      BPHY22DiTrkSelectAndWrite

#--------------------------------------------------------------------
## c/ augment and select D0 candidates
BPHY22_Select_D0 = DerivationFramework__Select_onia2mumu(
    name                  = "BPHY22_Select_D0",
    HypothesisName        = "D0",
    InputVtxContainerName = "BPHY22DiTrkCandidates",
    TrkMasses             = [139.571, 493.677],
    VtxMassHypo           = 1864.83,
    MassMin               = 1864.83-200,
    MassMax               = 1864.83+200,
    #LxyMin                = 0.0,#0.15,
    Chi2Max               = 50)

ToolSvc += BPHY22_Select_D0
print      BPHY22_Select_D0
##
#--------------------------------------------------------------------
## d/ augment and select D0bar candidates
BPHY22_Select_D0b = DerivationFramework__Select_onia2mumu(
    name                  = "BPHY22_Select_D0b",
    HypothesisName        = "D0b",
    InputVtxContainerName = "BPHY22DiTrkCandidates",
    TrkMasses             = [493.677, 139.571],
    VtxMassHypo           = 1864.83,
    MassMin               = 1864.83-200,
    MassMax               = 1864.83+200,
    #LxyMin                = 0.0,
    Chi2Max               = 50)

ToolSvc += BPHY22_Select_D0b
print      BPHY22_Select_D0b
#==============================================================================================

#===============================================================================================

#--------------------------------------------------------------------
# 3/ select B -> mu pi D*
#--------------------------------------------------------------------
## a/ setup the cascade vertexing tool
from TrkVKalVrtFitter.TrkVKalVrtFitterConf import Trk__TrkVKalVrtFitter
BcMuDstVertexFit = Trk__TrkVKalVrtFitter(
    name                 = "BcMuDstVertexFit",
    Extrapolator         = BPHY22_VertexTools.InDetExtrapolator,
    FirstMeasuredPoint   = False,
    CascadeCnstPrecision = 1e-6,
    MakeExtendedVertex   = True)

ToolSvc += BcMuDstVertexFit
print      BcMuDstVertexFit

#--------------------------------------------------------------------
## b/ setup Jpsi D*+ finder
from DerivationFrameworkBPhys.DerivationFrameworkBPhysConf import DerivationFramework__MuPlusDpstCascade
BPHY22MuDpst = DerivationFramework__MuPlusDpstCascade(
    name                     = "BPHY22MuDpst",
    HypothesisName           = "Bc",
    TrkVertexFitterTool      = BcMuDstVertexFit,
    DxHypothesis             = 421, # MC PID for D0
    ApplyD0MassConstraint    = True,
    MuPiMassLowerCut         = 200.,
    MuPiMassUpperCut         = 8200.,
    D0MassLowerCut           = 1864.83 - 200.,
    D0MassUpperCut           = 1864.83 + 200.,
    DstMassLowerCut          = 2010.26 - 300.,
    DstMassUpperCut          = 2010.26 + 300.,
    DstMassUpperCutAft       = 2010.26 + 25., #mass cut after cascade fit
    MassLowerCut             = 0.,
    MassUpperCut             = 12500.,
    Chi2Cut                  = 5, #chi2/ndf
    RefitPV                  = True,
    RefPVContainerName       = "BPHY22RefittedPrimaryVertices",
    MuPiVertices             = "BPHY22MuPiCandidates",
    CascadeVertexCollections = ["BcMuDpstCascadeSV2", "BcMuDpstCascadeSV1"],
    D0Vertices               = "BPHY22DiTrkCandidates",
    DoVertexType             = 15 )

ToolSvc += BPHY22MuDpst
print      BPHY22MuDpst
#===============================================================================================


#===============================================================================================
#--------------------------------------------------------------------
# 4/ select D_s+>K+K-pi+ and D+>K+pi-pi- candidates
#--------------------------------------------------------------------
## a/ setup a new vertexing tool (necessary due to use of mass constraint)
from TrkVKalVrtFitter.TrkVKalVrtFitterConf import Trk__TrkVKalVrtFitter
Dh3VertexFit = Trk__TrkVKalVrtFitter(
    name                = "Dh3VertexFit",
    Extrapolator        = BPHY22_VertexTools.InDetExtrapolator,
    FirstMeasuredPoint  = False,
    MakeExtendedVertex  = True)

ToolSvc += Dh3VertexFit
print      Dh3VertexFit

#--------------------------------------------------------------------
## b/ setup the Jpsi+1 track finder
from JpsiUpsilonTools.JpsiUpsilonToolsConf import Analysis__JpsiPlus1Track
BPHY22Dh3Finder = Analysis__JpsiPlus1Track(
    name                    = "BPHY22Dh3Finder",
    OutputLevel             = INFO,
    pionHypothesis          = True,     #false by default
    kaonHypothesis          = False,    #true by default
    trkThresholdPt          = 900.0,
    trkMaxEta               = 2.7, # is this value fine?? default would be 102.5
    BThresholdPt            = 2000.0,
    BMassUpper              = 2100.0, # What is this??
    BMassLower              = 500.0,
    TrkDeltaZ               = 20.,
    TrkTrippletMassUpper    = 2200, #2100
    TrkTrippletMassLower    = 500,
    TrkQuadrupletPt         = 2000,
    JpsiContainerKey        = "BPHY22DiTrkCandidates",
    TrackParticleCollection = "InDetTrackParticles",
    MuonsUsedInJpsi         = "NONE", # ?
    ExcludeCrossJpsiTracks  = False,
    TrkVertexFitterTool     = Dh3VertexFit,
    TrackSelectorTool       = BPHY22_VertexTools.InDetTrackSelectorTool,
    UseMassConstraint       = False,
    Chi2Cut                 = 7) #Cut on chi2/Ndeg_of_freedom 5->7
 
ToolSvc += BPHY22Dh3Finder
print      BPHY22Dh3Finder

# do not affect cascade fit
#--------------------------------------------------------------------
## c/ setup the combined augmentation/skimming tool for the D(s)+
from DerivationFrameworkBPhys.DerivationFrameworkBPhysConf import DerivationFramework__Reco_dimuTrk
BPHY22Dh3SelectAndWrite = DerivationFramework__Reco_dimuTrk(
    name                   = "BPHY22Dh3SelectAndWrite",
    OutputLevel            = INFO,
    Jpsi1PlusTrackName     = BPHY22Dh3Finder,
    OutputVtxContainerName = "BPHY22Dh3Candidates",
    PVContainerName        = "PrimaryVertices",
    RefPVContainerName     = "SHOULDNOTBEUSED",
    MaxPVrefit             = 1000)

ToolSvc += BPHY22Dh3SelectAndWrite
print      BPHY22Dh3SelectAndWrite

#--------------------------------------------------------------------
## d/ augment and select D_s+/- candidates
BPHY22_Select_Ds = DerivationFramework__Select_onia2mumu(
    name                  = "BPHY22_Select_Ds",
    HypothesisName        = "Ds",
    TrkMasses             = [493.677, 493.677, 139.571],
    InputVtxContainerName = "BPHY22Dh3Candidates",
    VtxMassHypo           = 1968.28,
    MassMin               = 1968.28 - 300.,
    MassMax               = 1968.28 + 200.,
    Chi2Max               = 40, 
    LxyMin                = 0.0,
    DoVertexType          = 1)
  
ToolSvc += BPHY22_Select_Ds
print      BPHY22_Select_Ds

#--------------------------------------------------------------------
## e/ augment and select D+ candidates
BPHY22_Select_Dp = DerivationFramework__Select_onia2mumu(
    name                  = "BPHY22_Select_Dp",
    HypothesisName        = "Dp",
    TrkMasses             = [139.571, 493.677, 139.571],
    InputVtxContainerName = "BPHY22Dh3Candidates",
    VtxMassHypo           = 1869.59,
    MassMin               = 1869.59 - 180.,
    MassMax               = 1869.59 + 250.,
    Chi2Max               = 200,
    LxyMin                = 0.05,
    DoVertexType          = 1)
  
ToolSvc += BPHY22_Select_Dp
print      BPHY22_Select_Dp
#--------------------------------------------------------------------
## f/ augment and select D- candidates
BPHY22_Select_Dm = DerivationFramework__Select_onia2mumu(
    name                  = "BPHY22_Select_Dm",
    HypothesisName        = "Dm",
    TrkMasses             = [493.677, 139.571, 139.571],
    InputVtxContainerName = "BPHY22Dh3Candidates",
    VtxMassHypo           = 1869.59,
    MassMin               = 1869.59 - 180.,
    MassMax               = 1869.59 + 250.,
    Chi2Max               = 200,
    LxyMin                = 0.05,
    DoVertexType          = 1)
  
ToolSvc += BPHY22_Select_Dm
print      BPHY22_Select_Dm
#--------------------------------------------------------------------
## g/ augment and select Lambda_c+ candidates
BPHY22_Select_LambdaCp = DerivationFramework__Select_onia2mumu(
    name                  = "BPHY22_Select_LambdaCp",
    HypothesisName        = "Lambda_c",
    TrkMasses             = [139.571, 493.677, 938.272],
    InputVtxContainerName = "BPHY22Dh3Candidates",
    VtxMassHypo           = 2286.46,
    MassMin               = 2286.46 - 200.,
    MassMax               = 2286.46 + 220.,
    Chi2Max               = 200,
    LxyMin                = 0.0,
    DoVertexType          = 1)
  
ToolSvc += BPHY22_Select_LambdaCp
print      BPHY22_Select_LambdaCp
#--------------------------------------------------------------------
## h/ augment and select Lambda_c- candidates
BPHY22_Select_LambdaCm = DerivationFramework__Select_onia2mumu(
    name                  = "BPHY22_Select_LambdaCm",
    HypothesisName        = "Lambda_c",
    TrkMasses             = [493.677, 139.571, 938.272],
    InputVtxContainerName = "BPHY22Dh3Candidates",
    VtxMassHypo           = 2286.46,
    MassMin               = 2286.46 - 200.,
    MassMax               = 2286.46 + 220.,
    Chi2Max               = 200,
    LxyMin                = 0.0,
    DoVertexType          = 1)
  
ToolSvc += BPHY22_Select_LambdaCm
print      BPHY22_Select_LambdaCm
#===============================================================================================

#--------------------------------------------------------------------
# 5/ select B -> mu D+ / mu Ds / mu Lambda_c
#--------------------------------------------------------------------
## a/ setup the cascade vertexing tool
BcMuDxVertexFit = Trk__TrkVKalVrtFitter(
    name                 = "BcMuDxVertexFit",
    Extrapolator         = BPHY22_VertexTools.InDetExtrapolator,
    FirstMeasuredPoint   = False,
    CascadeCnstPrecision = 1e-6,
    MakeExtendedVertex   = True)

ToolSvc += BcMuDxVertexFit
print      BcMuDxVertexFit

#--------------------------------------------------------------------
## b/ setup the mu Ds finder
from DerivationFrameworkBPhys.DerivationFrameworkBPhysConf import DerivationFramework__MuPlusDsCascade
BPHY22MuDs = DerivationFramework__MuPlusDsCascade(
    name                        = "BPHY22MuDs",
    HypothesisName              = "Bc",
    TrkVertexFitterTool         = BcMuDxVertexFit,
    DxHypothesis                = 431,
    ApplyDxMassConstraint       = False,
    DxMassLowerCut              = 1968.28 - 300.,
    DxMassUpperCut              = 1968.28 + 200.,
    MassLowerCut                = 1000,
    MassUpperCut                = 12500,
    Chi2Cut                     = 10,
    RefitPV                     = True,
    combOnly                    = True,
    TrackSelectorTool           = BPHY22_VertexTools.InDetTrackSelectorTool,
    useMCPCuts                  = False,
    muonThresholdPt             = 2700,
    muonCollectionKey           = "Muons",
    useCombinedMeasurement      = False, # Only takes effect if combOnly=True
    RefPVContainerName          = "BPHY22RefittedPrimaryVertices",
    CascadeVertexCollections    = ["BcMuDsCascadeSV2", "BcMuDsCascadeSV1"],
    DxVertices                  = "BPHY22Dh3Candidates")

ToolSvc += BPHY22MuDs
print      BPHY22MuDs

#--------------------------------------------------------------------
## c/ setup the mu D+ finder
BPHY22MuDp = DerivationFramework__MuPlusDsCascade(
    name                        = "BPHY22MuDp",
    HypothesisName              = "Bc",
    TrkVertexFitterTool         = BcMuDxVertexFit,
    DxHypothesis                = 411,
    ApplyDxMassConstraint       = False,
    DxMassLowerCut              = 1869.59 - 180.,
    DxMassUpperCut              = 1869.59 + 250.,
    MassLowerCut                = 1000,
    MassUpperCut                = 12500,
    Chi2Cut                     = 10,
    RefitPV                     = True,
    combOnly                    = True,
    TrackSelectorTool           = BPHY22_VertexTools.InDetTrackSelectorTool,
    useMCPCuts                  = False,
    muonThresholdPt             = 2700,
    muonCollectionKey           = "Muons",
    useCombinedMeasurement      = False, # Only takes effect if combOnly=True
    RefPVContainerName          = "BPHY22RefittedPrimaryVertices",
    CascadeVertexCollections    = ["BcMuDpCascadeSV2", "BcMuDpCascadeSV1"],
    DxVertices                  = "BPHY22Dh3Candidates")

ToolSvc += BPHY22MuDp
print      BPHY22MuDp

#--------------------------------------------------------------------
## d/ setup the mu Lambds_c+ finder
BPHY22MuLambdaC = DerivationFramework__MuPlusDsCascade(
    name                        = "BPHY22MuLambdaC",
    HypothesisName              = "Bc",
    TrkVertexFitterTool         = BcMuDxVertexFit,
    DxHypothesis                = 4122,
    ApplyDxMassConstraint       = False,
    DxMassLowerCut              = 2286.46 - 200,
    DxMassUpperCut              = 2286.46 + 220,
    MassLowerCut                = 1000,
    MassUpperCut                = 12500,
    Chi2Cut                     = 10,
    RefitPV                     = True,
    combOnly                    = True,
    TrackSelectorTool           = BPHY22_VertexTools.InDetTrackSelectorTool,
    useMCPCuts                  = False,
    muonThresholdPt             = 2700,
    muonCollectionKey           = "Muons",
    useCombinedMeasurement      = False, # Only takes effect if combOnly=True
    RefPVContainerName          = "BPHY22RefittedPrimaryVertices",
    CascadeVertexCollections    = ["BcMuLambdaCCascadeSV2", "BcMuLambdaCCascadeSV1"],
    DxVertices                  = "BPHY22Dh3Candidates")

ToolSvc += BPHY22MuLambdaC
print      BPHY22MuLambdaC


#===============================================================================================

#--------------------------------------------------------------------

CascadeCollections = []
CascadeCollections += BPHY22MuDpst.CascadeVertexCollections
CascadeCollections += BPHY22MuDp.CascadeVertexCollections
CascadeCollections += BPHY22MuDs.CascadeVertexCollections
CascadeCollections += BPHY22MuLambdaC.CascadeVertexCollections

#--------------------------------------------------------------------

if not isSimulation: #Only Skim Data
   from DerivationFrameworkTools.DerivationFrameworkToolsConf import DerivationFramework__xAODStringSkimmingTool
   BPHY22_SelectBcMuDxEvent = DerivationFramework__xAODStringSkimmingTool(
     name = "BPHY22_SelectBcMuDxEvent",
     expression = "(count(BcMuDpstCascadeSV1.x > -999) + count(BcMuDsCascadeSV1.x > -999) + count(BcMuDpCascadeSV1.x > -999) + count(BcMuLambdaCCascadeSV1.x > -999) ) > 0")
   
   ToolSvc += BPHY22_SelectBcMuDxEvent
   print      BPHY22_SelectBcMuDxEvent

   #====================================================================
   # Make event selection based on an OR of the input skimming tools
   #====================================================================
      
   from DerivationFrameworkTools.DerivationFrameworkToolsConf import DerivationFramework__FilterCombinationOR
   BPHY22SkimmingOR = CfgMgr.DerivationFramework__FilterCombinationOR(
       "BPHY22SkimmingOR",
       FilterList = [BPHY22_SelectBcMuDxEvent] )
   ToolSvc += BPHY22SkimmingOR
   print      BPHY22SkimmingOR

#--------------------------------------------------------------------
##10/ track and vertex thinning. We want to remove all reconstructed secondary vertices
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
BPHY22_thinningTool_Tracks = DerivationFramework__Thin_vtxTrk(
    name                       = "BPHY22_thinningTool_Tracks",
    ThinningService            = "BPHY22ThinningSvc",
    TrackParticleContainerName = "InDetTrackParticles",
    VertexContainerNames       = ["BcMuDpstCascadeSV1", "BcMuDpstCascadeSV2, BcMuDpCascadeSV1, BcMuDpCascadeSV2, BcMuDsCascadeSV1, BcMuDsCascadeSV2, BcMuLambdaCCascadeSV1, BcMuLambdaCCascadeSV2"],
    PassFlags                  = ["passed_Bc"])

ToolSvc += BPHY22_thinningTool_Tracks
print      BPHY22_thinningTool_Tracks

from DerivationFrameworkBPhys.DerivationFrameworkBPhysConf import DerivationFramework__BPhysPVThinningTool
BPHY22_thinningTool_PV = DerivationFramework__BPhysPVThinningTool(
    name                 = "BPHY22_thinningTool_PV",
    ThinningService      = "BPHY22ThinningSvc",
    CandidateCollections = ["BcMuDpstCascadeSV1", "BcMuDpstCascadeSV2, BcMuDpCascadeSV1, BcMuDpCascadeSV2, BcMuDsCascadeSV1, BcMuDsCascadeSV2, BcMuLambdaCCascadeSV1, BcMuLambdaCCascadeSV2"],
    KeepPVTracks         = True)

ToolSvc += BPHY22_thinningTool_PV
print      BPHY22_thinningTool_PV

## b) thinning out tracks that are not attached to muons. The final thinning decision is based on the OR operation
##    between decision from this and the previous tools.
from DerivationFrameworkInDet.DerivationFrameworkInDetConf import DerivationFramework__MuonTrackParticleThinning
BPHY22MuonTPThinningTool = DerivationFramework__MuonTrackParticleThinning(
    name                   = "BPHY22MuonTPThinningTool",
    ThinningService        = "BPHY22ThinningSvc",
    MuonKey                = "Muons",
    InDetTrackParticlesKey = "InDetTrackParticles")

ToolSvc += BPHY22MuonTPThinningTool
print      BPHY22MuonTPThinningTool

#====================================================================
# CREATE THE DERIVATION KERNEL ALGORITHM AND PASS THE ABOVE TOOLS
#====================================================================

thiningCollection = []

print thiningCollection

# The name of the kernel (BPHY22Kernel in this case) must be unique to this derivation
from DerivationFrameworkCore.DerivationFrameworkCoreConf import DerivationFramework__DerivationKernel
DerivationFrameworkJob += CfgMgr.DerivationFramework__DerivationKernel(
    "BPHY22Kernel",
    AugmentationTools = [BPHY22MuPiSelectAndWrite, BPHY22_Select_MuPi,
                         BPHY22DiTrkSelectAndWrite, BPHY22_Select_D0, BPHY22_Select_D0b,
                         BPHY22Dh3SelectAndWrite, BPHY22_Select_Ds, BPHY22_Select_Dp, BPHY22_Select_Dm, BPHY22_Select_LambdaCp, BPHY22_Select_LambdaCm,
                         BPHY22MuDpst,
                         BPHY22MuDs,
                         BPHY22MuDp,
                         BPHY22MuLambdaC,
                         BPHY22_AugOriginalCounts],
    #Only skim if not MC
    SkimmingTools     = [BPHY22SkimmingOR] if not isSimulation else [],
    ThinningTools     = thiningCollection
    )

#====================================================================
# SET UP STREAM
#====================================================================
streamName   = derivationFlags.WriteDAOD_BPHY22Stream.StreamName
fileName     = buildFileName( derivationFlags.WriteDAOD_BPHY22Stream )
BPHY22Stream  = MSMgr.NewPoolRootStream( streamName, fileName )
BPHY22Stream.AcceptAlgs(["BPHY22Kernel"])

# Special lines for thinning
# Thinning service name must match the one passed to the thinning tools
from AthenaServices.Configurables import ThinningSvc, createThinningSvc
augStream = MSMgr.GetStream( streamName )
evtStream = augStream.GetEventStream()

BPHY22ThinningSvc = createThinningSvc( svcName="BPHY22ThinningSvc", outStreams=[evtStream] )
svcMgr += BPHY22ThinningSvc


#====================================================================
# Slimming
#====================================================================

# Added by ASC
from DerivationFrameworkCore.SlimmingHelper import SlimmingHelper
BPHY22SlimmingHelper = SlimmingHelper("BPHY22SlimmingHelper")
AllVariables  = []
StaticContent = []

# Needed for trigger objects
BPHY22SlimmingHelper.IncludeMuonTriggerContent  = TRUE
BPHY22SlimmingHelper.IncludeBPhysTriggerContent = TRUE

## primary vertices
AllVariables  += ["PrimaryVertices"]
StaticContent += ["xAOD::VertexContainer#BPHY22RefittedPrimaryVertices"]
StaticContent += ["xAOD::VertexAuxContainer#BPHY22RefittedPrimaryVerticesAux."]

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
StaticContent += ["xAOD::VertexContainer#%s"        %                 BPHY22MuPiSelectAndWrite.OutputVtxContainerName]
## we have to disable vxTrackAtVertex branch since it is not xAOD compatible
StaticContent += ["xAOD::VertexAuxContainer#%sAux.-vxTrackAtVertex" % BPHY22MuPiSelectAndWrite.OutputVtxContainerName]


## K+K-, Kpi, D0/D0bar candidates
StaticContent += ["xAOD::VertexContainer#%s"        %                 BPHY22DiTrkSelectAndWrite.OutputVtxContainerName]
StaticContent += ["xAOD::VertexAuxContainer#%sAux.-vxTrackAtVertex" % BPHY22DiTrkSelectAndWrite.OutputVtxContainerName]


## D+ / Ds candidates
StaticContent += ["xAOD::VertexContainer#%s"        %                 BPHY22Dh3SelectAndWrite.OutputVtxContainerName]
StaticContent += ["xAOD::VertexAuxContainer#%sAux.-vxTrackAtVertex" % BPHY22Dh3SelectAndWrite.OutputVtxContainerName]

## Bc+>J/psi D_(s)+/-, J/psi D*+/- and J/psi D_s1+/- candidates
for cascades in CascadeCollections:
   StaticContent += ["xAOD::VertexContainer#%s"   %     cascades]
   StaticContent += ["xAOD::VertexAuxContainer#%sAux.-vxTrackAtVertex" % cascades]

# Tagging information (in addition to that already requested by usual algorithms)
AllVariables += ["GSFTrackParticles", "MuonSpectrometerTrackParticles" ]

# Added by ASC
# Truth information for MC only
if isSimulation:
    AllVariables += ["TruthEvents","TruthParticles","TruthVertices","MuonTruthParticles"]

AllVariables = list(set(AllVariables)) # remove duplicates

BPHY22SlimmingHelper.AllVariables = AllVariables
BPHY22SlimmingHelper.StaticContent = StaticContent
BPHY22SlimmingHelper.SmartCollections = []

BPHY22SlimmingHelper.AppendContentToStream(BPHY22Stream)


#====================================================================
# END OF BPHY22.py
#====================================================================
