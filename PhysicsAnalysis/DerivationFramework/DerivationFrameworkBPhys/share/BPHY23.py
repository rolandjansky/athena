#====================================================================
# BPHY23.py
# Contact: xin.chen@cern.ch
#====================================================================

# Set up common services and job object. 
# This should appear in ALL derivation job options
from DerivationFrameworkCore.DerivationFrameworkMaster import *

from DerivationFrameworkCore.DerivationFrameworkMaster import DerivationFrameworkHasTruth
isSimulation = DerivationFrameworkHasTruth

print isSimulation

#====================================================================
# AUGMENTATION TOOLS 
#====================================================================
## 1/ setup vertexing tools and services
include("DerivationFrameworkBPhys/configureVertexing.py")
BPHY23_VertexTools = BPHYVertexTools("BPHY23")

#--------------------------------------------------------------------
## 2/ Setup the vertex fitter tools (e.g. JpsiFinder, JpsiPlus1Track, etc).
##    These are general tools independent of DerivationFramework that do the 
##    actual vertex fitting and some pre-selection.
from JpsiUpsilonTools.JpsiUpsilonToolsConf import Analysis__JpsiFinder
BPHY23JpsiFinder = Analysis__JpsiFinder(
    name                        = "BPHY23JpsiFinder",
    OutputLevel                 = INFO,
    muAndMu                     = True,
    muAndTrack                  = False,
    TrackAndTrack               = False,
    assumeDiMuons               = True,  # If true, will assume dimu hypothesis and use PDG value for mu mass
    trackThresholdPt            = 1800.,
    invMassLower                = 2500.,
    invMassUpper                = 10000.,
    Chi2Cut                     = 200.,
    oppChargesOnly	        = True,
    atLeastOneComb              = True,
    useCombinedMeasurement      = False, # Only takes effect if combOnly=True	
    muonCollectionKey           = "Muons",
    TrackParticleCollection     = "InDetTrackParticles",
    V0VertexFitterTool          = BPHY23_VertexTools.TrkV0Fitter, # V0 vertex fitter
    useV0Fitter                 = False, # if False a TrkVertexFitterTool will be used
    TrkVertexFitterTool         = BPHY23_VertexTools.TrkVKalVrtFitter, # VKalVrt vertex fitter
    TrackSelectorTool           = BPHY23_VertexTools.InDetTrackSelectorTool,
    ConversionFinderHelperTool  = BPHY23_VertexTools.InDetConversionHelper,
    VertexPointEstimator        = BPHY23_VertexTools.VtxPointEstimator,
    useMCPCuts                  = False )

ToolSvc += BPHY23JpsiFinder
print      BPHY23JpsiFinder

#--------------------------------------------------------------------
## 3/ setup the vertex reconstruction "call" tool(s). They are part of the derivation framework.
##    These Augmentation tools add output vertex collection(s) into the StoreGate and add basic 
##    decorations which do not depend on the vertex mass hypothesis (e.g. lxy, ptError, etc).
##    There should be one tool per topology, i.e. Jpsi and Psi(2S) do not need two instance of the
##    Reco tool if the JpsiFinder mass window is wide enough.

# https://gitlab.cern.ch/atlas/athena/-/blob/21.2/PhysicsAnalysis/DerivationFramework/DerivationFrameworkBPhys/src/Reco_mumu.cxx
from DerivationFrameworkBPhys.DerivationFrameworkBPhysConf import DerivationFramework__Reco_mumu
BPHY23_Reco_mumu = DerivationFramework__Reco_mumu(
    name                   = "BPHY23_Reco_mumu",
    JpsiFinder             = BPHY23JpsiFinder,
    OutputVtxContainerName = "BPHY23OniaCandidates",
    PVContainerName        = "PrimaryVertices",
    RefPVContainerName     = "SHOULDNOTBEUSED",
    DoVertexType           = 1)

ToolSvc += BPHY23_Reco_mumu
print BPHY23_Reco_mumu

## 4/ setup a new vertexing tool (necessary due to use of mass constraint) 
from TrkVKalVrtFitter.TrkVKalVrtFitterConf import Trk__TrkVKalVrtFitter
BPHY23VertexFit = Trk__TrkVKalVrtFitter(
    name                = "BPHY23VertexFit",
    Extrapolator        = BPHY23_VertexTools.InDetExtrapolator,
    FirstMeasuredPoint  = False,  # use Perigee strategy
    MakeExtendedVertex  = True)
ToolSvc += BPHY23VertexFit
print      BPHY23VertexFit

## 5/ setup the Jpsi+2 track finder
# https://gitlab.cern.ch/atlas/athena/-/blob/21.2/PhysicsAnalysis/JpsiUpsilonTools/src/JpsiPlus2Tracks.cxx
from JpsiUpsilonTools.JpsiUpsilonToolsConf import Analysis__JpsiPlus2Tracks
BPHY23JpsiPlus2Tracks = Analysis__JpsiPlus2Tracks(
    name = "BPHY23JpsiPlus2Tracks",
    kaonkaonHypothesis		        = False,
    pionpionHypothesis                  = True,
    kaonpionHypothesis                  = False,
    trkThresholdPt			= 280.,
    trkMaxEta		 	        = 2.6,
    oppChargesOnly                      = False,
    JpsiMassLower                       = 2500.,
    JpsiMassUpper                       = 3600.,
    TrkQuadrupletMassLower              = 3200.,
    TrkQuadrupletMassUpper              = 4200.,
    Chi2Cut                             = 200.,
    JpsiContainerKey                    = "BPHY23OniaCandidates",
    TrackParticleCollection             = "InDetTrackParticles",
    MuonsUsedInJpsi			= "Muons",
    ExcludeJpsiMuonsOnly                = True,
    TrkVertexFitterTool		        = BPHY23VertexFit,
    TrackSelectorTool		        = BPHY23_VertexTools.InDetTrackSelectorTool,
    UseMassConstraint		        = False)

ToolSvc += BPHY23JpsiPlus2Tracks
print      BPHY23JpsiPlus2Tracks

BPHY23UpsiPlus2Tracks = Analysis__JpsiPlus2Tracks(
    name = "BPHY23UpsiPlus2Tracks",
    kaonkaonHypothesis		        = False,
    pionpionHypothesis                  = True,
    kaonpionHypothesis                  = False,
    trkThresholdPt			= 280.,
    trkMaxEta		 	        = 2.6,
    oppChargesOnly                      = False,
    JpsiMassLower                       = 8800.,
    JpsiMassUpper                       = 10000.,
    TrkQuadrupletMassLower              = 9400.,
    TrkQuadrupletMassUpper              = 11000.,
    Chi2Cut                             = 200.,
    JpsiContainerKey                    = "BPHY23OniaCandidates",
    TrackParticleCollection             = "InDetTrackParticles",
    MuonsUsedInJpsi			= "Muons",
    ExcludeJpsiMuonsOnly                = True,
    TrkVertexFitterTool		        = BPHY23VertexFit,
    TrackSelectorTool		        = BPHY23_VertexTools.InDetTrackSelectorTool,
    UseMassConstraint		        = False)

ToolSvc += BPHY23UpsiPlus2Tracks
print      BPHY23UpsiPlus2Tracks

## 6/ setup the combined augmentation/skimming tool
from DerivationFrameworkBPhys.DerivationFrameworkBPhysConf import DerivationFramework__Reco_dimuTrkTrk	
BPHY23FourTrackSelect_JpsiTrkTrk = DerivationFramework__Reco_dimuTrkTrk(
    name                     = "BPHY23FourTrackSelect_JpsiTrkTrk",
    Jpsi2PlusTrackName       = BPHY23JpsiPlus2Tracks,
    OutputVtxContainerName   = "BPHY23FourTrack_JpsiTrkTrk",
    PVContainerName          = "PrimaryVertices",
    RefitPV                  = False)

ToolSvc += BPHY23FourTrackSelect_JpsiTrkTrk
print      BPHY23FourTrackSelect_JpsiTrkTrk

BPHY23FourTrackSelect_UpsiTrkTrk = DerivationFramework__Reco_dimuTrkTrk(
    name                     = "BPHY23FourTrackSelect_UpsiTrkTrk",
    Jpsi2PlusTrackName       = BPHY23UpsiPlus2Tracks,
    OutputVtxContainerName   = "BPHY23FourTrack_UpsiTrkTrk",
    PVContainerName          = "PrimaryVertices",
    RefitPV                  = False)

ToolSvc += BPHY23FourTrackSelect_UpsiTrkTrk
print      BPHY23FourTrackSelect_UpsiTrkTrk


from DerivationFrameworkBPhys.DerivationFrameworkBPhysConf import DerivationFramework__Select_onia2mumu

BPHY23_Select_Jpsi             = DerivationFramework__Select_onia2mumu(
    name                       = "BPHY23_Select_Jpsi",
    HypothesisName             = "Jpsi",
    InputVtxContainerName      = "BPHY23OniaCandidates",
    TrkMasses                  = [105.658, 105.658],
    VtxMassHypo                = 3096.916,
    MassMin                    = 2500.,
    MassMax                    = 3600.,
    Chi2Max                    = 200)

ToolSvc += BPHY23_Select_Jpsi
print      BPHY23_Select_Jpsi

BPHY23_Select_Upsi             = DerivationFramework__Select_onia2mumu(
    name                       = "BPHY23_Select_Upsi",
    HypothesisName             = "Upsi",
    InputVtxContainerName      = "BPHY23OniaCandidates",
    TrkMasses                  = [105.658, 105.658],
    VtxMassHypo                = 9460.30,
    MassMin                    = 8800.,
    MassMax                    = 10000.,
    Chi2Max                    = 200)

ToolSvc += BPHY23_Select_Upsi
print      BPHY23_Select_Upsi

BPHY23_Select_Psi              = DerivationFramework__Select_onia2mumu(
    name                       = "BPHY23_Select_Psi",
    HypothesisName             = "Psi",
    InputVtxContainerName      = "BPHY23FourTrack_JpsiTrkTrk",
    TrkMasses                  = [105.658, 105.658, 139.570, 139.570],
    VtxMassHypo                = 3686.10, # for decay time
    MassMin                    = 3200.,
    MassMax                    = 4200.,
    Chi2Max                    = 200.)

ToolSvc += BPHY23_Select_Psi
print      BPHY23_Select_Psi

BPHY23_Select_Upsi2S           = DerivationFramework__Select_onia2mumu(
    name                       = "BPHY23_Select_Upsi2S",
    HypothesisName             = "Upsi2S",
    InputVtxContainerName      = "BPHY23FourTrack_UpsiTrkTrk",
    TrkMasses                  = [105.658, 105.658, 139.570, 139.570],
    VtxMassHypo                = 10023.26, # for decay time
    MassMin                    = 9400.,
    MassMax                    = 11000.,
    Chi2Max                    = 200.)

ToolSvc += BPHY23_Select_Upsi2S
print      BPHY23_Select_Upsi2S

#--------------------------------------------------------------------
# set up TQ -> Jpsi Psi (Psi -> Jpsi pi pi) finder
#--------------------------------------------------------------------
from DerivationFrameworkBPhys.DerivationFrameworkBPhysConf import DerivationFramework__JpsiPlusPsiCascade
BPHY23JpsiPsi = DerivationFramework__JpsiPlusPsiCascade(
    name                     = "BPHY23JpsiPsi",
    HypothesisName           = "JpsiPsi",
    TrkVertexFitterTool      = BPHY23VertexFit,
    JpsiVertices             = "BPHY23OniaCandidates",
    JpsiVtxHypoName          = "Jpsi",
    PsiVertices              = "BPHY23FourTrack_JpsiTrkTrk",
    PsiVtxHypoName           = "Psi",
    Jpsi1MassLowerCut        = 2500., # Jpsi (from Psi) low mass
    Jpsi1MassUpperCut        = 3600., # Jpsi (from Psi) high mass
    MassLowerCut             = 0.,
    MassUpperCut             = 31000.,
    Jpsi1Mass                = 3096.916,
    Jpsi2Mass                = 3096.916,
    PsiMass                  = 3686.10,
    ApplyJpsi1MassConstraint = True,
    ApplyJpsi2MassConstraint = True,
    ApplyPsiMassConstraint   = True,
    VxPrimaryCandidateName   = "PrimaryVertices",
    RefPVContainerName       = "BPHY23JpsiPsiRefPrimaryVertices",
    CascadeVertexCollections = [ "BPHY23JpsiPsiCascadeVtx1", "BPHY23JpsiPsiCascadeVtx2" ],
    RefitPV                  = True,
    MaxnPV                   = 2000,
    DoVertexType             = 7)

ToolSvc += BPHY23JpsiPsi
print      BPHY23JpsiPsi

#--------------------------------------------------------------------
# set up TQ -> Jpsi Upsi2S (Upsi2S -> Upsi pi pi) finder
#--------------------------------------------------------------------
BPHY23JpsiUpsi2S = DerivationFramework__JpsiPlusPsiCascade(
    name                     = "BPHY23JpsiUpsi2S",
    HypothesisName           = "JpsiUpsi2S",
    TrkVertexFitterTool      = BPHY23VertexFit,
    JpsiVertices             = "BPHY23OniaCandidates",
    JpsiVtxHypoName          = "Jpsi",
    PsiVertices              = "BPHY23FourTrack_UpsiTrkTrk",
    PsiVtxHypoName           = "Upsi2S",
    Jpsi1MassLowerCut        = 8800., # Upsi (from Upsi2S) low mass
    Jpsi1MassUpperCut        = 10000., # Upsi (from Upsi2S) high mass
    MassLowerCut             = 0.,
    MassUpperCut             = 31000.,
    Jpsi1Mass                = 9460.30,
    Jpsi2Mass                = 3096.916,
    PsiMass                  = 10023.26,
    ApplyJpsi1MassConstraint = True,
    ApplyJpsi2MassConstraint = True,
    ApplyPsiMassConstraint   = True,
    VxPrimaryCandidateName   = "PrimaryVertices",
    RefPVContainerName       = "BPHY23JpsiUpsi2SRefPrimaryVertices",
    CascadeVertexCollections = [ "BPHY23JpsiUpsi2SCascadeVtx1", "BPHY23JpsiUpsi2SCascadeVtx2" ],
    RefitPV                  = True,
    MaxnPV                   = 2000,
    DoVertexType             = 7)

ToolSvc += BPHY23JpsiUpsi2S
print      BPHY23JpsiUpsi2S

#--------------------------------------------------------------------
# set up TQ -> Upsi Psi (Psi -> Jpsi pi pi) finder
#--------------------------------------------------------------------
BPHY23UpsiPsi = DerivationFramework__JpsiPlusPsiCascade(
    name                     = "BPHY23UpsiPsi",
    HypothesisName           = "UpsiPsi",
    TrkVertexFitterTool      = BPHY23VertexFit,
    JpsiVertices             = "BPHY23OniaCandidates",
    JpsiVtxHypoName          = "Upsi",
    PsiVertices              = "BPHY23FourTrack_JpsiTrkTrk",
    PsiVtxHypoName           = "Psi",
    Jpsi1MassLowerCut        = 2500., # Jpsi (from Psi) low mass
    Jpsi1MassUpperCut        = 3600., # Jpsi (from Psi) high mass
    MassLowerCut             = 0.,
    MassUpperCut             = 31000.,
    Jpsi1Mass                = 3096.916,
    Jpsi2Mass                = 9460.30,
    PsiMass                  = 3686.10,
    ApplyJpsi1MassConstraint = True,
    ApplyJpsi2MassConstraint = True,
    ApplyPsiMassConstraint   = True,
    VxPrimaryCandidateName   = "PrimaryVertices",
    RefPVContainerName       = "BPHY23UpsiPsiRefPrimaryVertices",
    CascadeVertexCollections = [ "BPHY23UpsiPsiCascadeVtx1", "BPHY23UpsiPsiCascadeVtx2" ],
    RefitPV                  = True,
    MaxnPV                   = 2000,
    DoVertexType             = 7)

ToolSvc += BPHY23UpsiPsi
print      BPHY23UpsiPsi

#--------------------------------------------------------------------
# set up TQ -> Upsi Upsi2S (Upsi2S -> Upsi pi pi) finder
#--------------------------------------------------------------------
BPHY23UpsiUpsi2S = DerivationFramework__JpsiPlusPsiCascade(
    name                     = "BPHY23UpsiUpsi2S",
    HypothesisName           = "UpsiUpsi2S",
    TrkVertexFitterTool      = BPHY23VertexFit,
    JpsiVertices             = "BPHY23OniaCandidates",
    JpsiVtxHypoName          = "Upsi",
    PsiVertices              = "BPHY23FourTrack_UpsiTrkTrk",
    PsiVtxHypoName           = "Upsi2S",
    Jpsi1MassLowerCut        = 8800., # Upsi (from Upsi2S) low mass
    Jpsi1MassUpperCut        = 10000., # Upsi (from Upsi2S) high mass
    MassLowerCut             = 0.,
    MassUpperCut             = 31000.,
    Jpsi1Mass                = 9460.30,
    Jpsi2Mass                = 9460.30,
    PsiMass                  = 10023.26,
    ApplyJpsi1MassConstraint = True,
    ApplyJpsi2MassConstraint = True,
    ApplyPsiMassConstraint   = True,
    VxPrimaryCandidateName   = "PrimaryVertices",
    RefPVContainerName       = "BPHY23UpsiUpsi2SRefPrimaryVertices",
    CascadeVertexCollections = [ "BPHY23UpsiUpsi2SCascadeVtx1", "BPHY23UpsiUpsi2SCascadeVtx2" ],
    RefitPV                  = True,
    MaxnPV                   = 2000,
    DoVertexType             = 7)

ToolSvc += BPHY23UpsiUpsi2S
print      BPHY23UpsiUpsi2S


#====================================================================
# Revertex with mass constraint
#====================================================================

from DerivationFrameworkBPhys.DerivationFrameworkBPhysConf import DerivationFramework__ReVertex

BPHY23Revertex_JpsiPsi_RV1     = DerivationFramework__ReVertex(
    name                       = "BPHY23Revertex_JpsiPsi_RV1",
    InputVtxContainerName      = "BPHY23JpsiPsiCascadeVtx1",
    TrackIndices               = [ 0, 1, 2, 3 ],
    SubVertexTrackIndices      = [ 1, 2 ],
    RefitPV                    = True,
    RefPVContainerName         = "BPHY23JpsiPsiRefPrimaryVertices", # use existing refitted PVs
    UseMassConstraint          = True,
    VertexMass                 = 3686.10,
    SubVertexMass              = 3096.916,
    MassInputParticles         = [105.658, 105.658, 139.570, 139.570],
    TrkVertexFitterTool	       = BPHY23VertexFit,
    OutputVtxContainerName     = "BPHY23JpsiPsi_RV1")

ToolSvc += BPHY23Revertex_JpsiPsi_RV1
print      BPHY23Revertex_JpsiPsi_RV1

BPHY23Select_JpsiPsi_RV1       = DerivationFramework__Select_onia2mumu(
    name                       = "BPHY23Select_JpsiPsi_RV1",
    HypothesisName             = "Psi",
    InputVtxContainerName      = "BPHY23JpsiPsi_RV1",
    TrkMasses                  = [105.658, 105.658, 139.570, 139.570],
    VtxMassHypo                = 3686.10,
    MassMin                    = 3200.,
    MassMax                    = 4200.,
    Chi2Max                    = 1.e6)

ToolSvc += BPHY23Select_JpsiPsi_RV1
print      BPHY23Select_JpsiPsi_RV1

BPHY23Revertex_JpsiPsi_RV2     = DerivationFramework__ReVertex(
    name                       = "BPHY23Revertex_JpsiPsi_RV2",
    InputVtxContainerName      = "BPHY23JpsiPsiCascadeVtx2",
    TrackIndices               = [ 0, 1 ],
    RefitPV                    = True,
    RefPVContainerName         = "BPHY23JpsiPsiRefPrimaryVertices", # use existing refitted PVs
    UseMassConstraint          = True,
    VertexMass                 = 3096.916,
    MassInputParticles         = [105.658, 105.658],
    TrkVertexFitterTool	       = BPHY23VertexFit,
    OutputVtxContainerName     = "BPHY23JpsiPsi_RV2")

ToolSvc += BPHY23Revertex_JpsiPsi_RV2
print      BPHY23Revertex_JpsiPsi_RV2

BPHY23Select_JpsiPsi_RV2       = DerivationFramework__Select_onia2mumu(
    name                       = "BPHY23Select_JpsiPsi_RV2",
    HypothesisName             = "Jpsi",
    InputVtxContainerName      = "BPHY23JpsiPsi_RV2",
    TrkMasses                  = [105.658, 105.658],
    VtxMassHypo                = 3096.916,
    MassMin                    = 2500.,
    MassMax                    = 3600.,
    Chi2Max                    = 1.e6)

ToolSvc += BPHY23Select_JpsiPsi_RV2
print      BPHY23Select_JpsiPsi_RV2


BPHY23Revertex_UpsiPsi_RV1     = DerivationFramework__ReVertex(
    name                       = "BPHY23Revertex_UpsiPsi_RV1",
    InputVtxContainerName      = "BPHY23UpsiPsiCascadeVtx1",
    TrackIndices               = [ 0, 1, 2, 3 ],
    SubVertexTrackIndices      = [ 1, 2 ],
    RefitPV                    = True,
    RefPVContainerName         = "BPHY23UpsiPsiRefPrimaryVertices",  # use existing refitted PVs
    UseMassConstraint          = True,
    VertexMass                 = 3686.10,
    SubVertexMass              = 3096.916,
    MassInputParticles         = [105.658, 105.658, 139.570, 139.570],
    TrkVertexFitterTool	       = BPHY23VertexFit,
    OutputVtxContainerName     = "BPHY23UpsiPsi_RV1")

ToolSvc += BPHY23Revertex_UpsiPsi_RV1
print      BPHY23Revertex_UpsiPsi_RV1

BPHY23Select_UpsiPsi_RV1       = DerivationFramework__Select_onia2mumu(
    name                       = "BPHY23Select_UpsiPsi_RV1",
    HypothesisName             = "Psi",
    InputVtxContainerName      = "BPHY23UpsiPsi_RV1",
    TrkMasses                  = [105.658, 105.658, 139.570, 139.570],
    VtxMassHypo                = 3686.10,
    MassMin                    = 3200.,
    MassMax                    = 4200.,
    Chi2Max                    = 1.e6)

ToolSvc += BPHY23Select_UpsiPsi_RV1
print      BPHY23Select_UpsiPsi_RV1

BPHY23Revertex_UpsiPsi_RV2     = DerivationFramework__ReVertex(
    name                       = "BPHY23Revertex_UpsiPsi_RV2",
    InputVtxContainerName      = "BPHY23UpsiPsiCascadeVtx2",
    TrackIndices               = [ 0, 1 ],
    RefitPV                    = True,
    RefPVContainerName         = "BPHY23UpsiPsiRefPrimaryVertices", # use existing refitted PVs
    UseMassConstraint          = True,
    VertexMass                 = 9460.30,
    MassInputParticles         = [105.658, 105.658],
    TrkVertexFitterTool	       = BPHY23VertexFit,
    OutputVtxContainerName     = "BPHY23UpsiPsi_RV2")

ToolSvc += BPHY23Revertex_UpsiPsi_RV2
print      BPHY23Revertex_UpsiPsi_RV2

BPHY23Select_UpsiPsi_RV2       = DerivationFramework__Select_onia2mumu(
    name                       = "BPHY23Select_UpsiPsi_RV2",
    HypothesisName             = "Upsi",
    InputVtxContainerName      = "BPHY23UpsiPsi_RV2",
    TrkMasses                  = [105.658, 105.658],
    VtxMassHypo                = 9460.30,
    MassMin                    = 8800.,
    MassMax                    = 10000.,
    Chi2Max                    = 1.e6)

ToolSvc += BPHY23Select_UpsiPsi_RV2
print      BPHY23Select_UpsiPsi_RV2


BPHY23Revertex_JpsiUpsi2S_RV1  = DerivationFramework__ReVertex(
    name                       = "BPHY23Revertex_JpsiUpsi2S_RV1",
    InputVtxContainerName      = "BPHY23JpsiUpsi2SCascadeVtx1",
    TrackIndices               = [ 0, 1, 2, 3 ],
    SubVertexTrackIndices      = [ 1, 2 ],
    RefitPV                    = True,
    RefPVContainerName         = "BPHY23JpsiUpsi2SRefPrimaryVertices", # use existing refitted PVs
    UseMassConstraint          = True,
    VertexMass                 = 10023.26,
    SubVertexMass              = 9460.30,
    MassInputParticles         = [105.658, 105.658, 139.570, 139.570],
    TrkVertexFitterTool	       = BPHY23VertexFit,
    OutputVtxContainerName     = "BPHY23JpsiUpsi2S_RV1")

ToolSvc += BPHY23Revertex_JpsiUpsi2S_RV1
print      BPHY23Revertex_JpsiUpsi2S_RV1

BPHY23Select_JpsiUpsi2S_RV1    = DerivationFramework__Select_onia2mumu(
    name                       = "BPHY23Select_JpsiUpsi2S_RV1",
    HypothesisName             = "Upsi2S",
    InputVtxContainerName      = "BPHY23JpsiUpsi2S_RV1",
    TrkMasses                  = [105.658, 105.658, 139.570, 139.570],
    VtxMassHypo                = 10023.26,
    MassMin                    = 9400.,
    MassMax                    = 11000.,
    Chi2Max                    = 1.e6)

ToolSvc += BPHY23Select_JpsiUpsi2S_RV1
print      BPHY23Select_JpsiUpsi2S_RV1

BPHY23Revertex_JpsiUpsi2S_RV2  = DerivationFramework__ReVertex(
    name                       = "BPHY23Revertex_JpsiUpsi2S_RV2",
    InputVtxContainerName      = "BPHY23JpsiUpsi2SCascadeVtx2",
    TrackIndices               = [ 0, 1 ],
    RefitPV                    = True,
    RefPVContainerName         = "BPHY23JpsiUpsi2SRefPrimaryVertices", # use existing refitted PVs
    UseMassConstraint          = True,
    VertexMass                 = 3096.916,
    MassInputParticles         = [105.658, 105.658],
    TrkVertexFitterTool	       = BPHY23VertexFit,
    OutputVtxContainerName     = "BPHY23JpsiUpsi2S_RV2")

ToolSvc += BPHY23Revertex_JpsiUpsi2S_RV2
print      BPHY23Revertex_JpsiUpsi2S_RV2

BPHY23Select_JpsiUpsi2S_RV2    = DerivationFramework__Select_onia2mumu(
    name                       = "BPHY23Select_JpsiUpsi2S_RV2",
    HypothesisName             = "Jpsi",
    InputVtxContainerName      = "BPHY23JpsiUpsi2S_RV2",
    TrkMasses                  = [105.658, 105.658],
    VtxMassHypo                = 3096.916,
    MassMin                    = 2500.,
    MassMax                    = 3600.,
    Chi2Max                    = 1.e6)

ToolSvc += BPHY23Select_JpsiUpsi2S_RV2
print      BPHY23Select_JpsiUpsi2S_RV2


BPHY23Revertex_UpsiUpsi2S_RV1  = DerivationFramework__ReVertex(
    name                       = "BPHY23Revertex_UpsiUpsi2S_RV1",
    InputVtxContainerName      = "BPHY23UpsiUpsi2SCascadeVtx1",
    TrackIndices               = [ 0, 1, 2, 3 ],
    SubVertexTrackIndices      = [ 1, 2 ],
    RefitPV                    = True,
    RefPVContainerName         = "BPHY23UpsiUpsi2SRefPrimaryVertices", # use existing refitted PVs
    UseMassConstraint          = True,
    VertexMass                 = 10023.26,
    SubVertexMass              = 9460.30,
    MassInputParticles         = [105.658, 105.658, 139.570, 139.570],
    TrkVertexFitterTool	       = BPHY23VertexFit,
    OutputVtxContainerName     = "BPHY23UpsiUpsi2S_RV1")

ToolSvc += BPHY23Revertex_UpsiUpsi2S_RV1
print      BPHY23Revertex_UpsiUpsi2S_RV1

BPHY23Select_UpsiUpsi2S_RV1    = DerivationFramework__Select_onia2mumu(
    name                       = "BPHY23Select_UpsiUpsi2S_RV1",
    HypothesisName             = "Upsi2S",
    InputVtxContainerName      = "BPHY23UpsiUpsi2S_RV1",
    TrkMasses                  = [105.658, 105.658, 139.570, 139.570],
    VtxMassHypo                = 10023.26,
    MassMin                    = 9400.,
    MassMax                    = 11000.,
    Chi2Max                    = 1.e6)

ToolSvc += BPHY23Select_UpsiUpsi2S_RV1
print      BPHY23Select_UpsiUpsi2S_RV1

BPHY23Revertex_UpsiUpsi2S_RV2  = DerivationFramework__ReVertex(
    name                       = "BPHY23Revertex_UpsiUpsi2S_RV2",
    InputVtxContainerName      = "BPHY23UpsiUpsi2SCascadeVtx2",
    TrackIndices               = [ 0, 1 ],
    RefitPV                    = True,
    RefPVContainerName         = "BPHY23UpsiUpsi2SRefPrimaryVertices", # use existing refitted PVs
    UseMassConstraint          = True,
    VertexMass                 = 9460.30,
    MassInputParticles         = [105.658, 105.658],
    TrkVertexFitterTool	       = BPHY23VertexFit,
    OutputVtxContainerName     = "BPHY23UpsiUpsi2S_RV2")

ToolSvc += BPHY23Revertex_UpsiUpsi2S_RV2
print      BPHY23Revertex_UpsiUpsi2S_RV2

BPHY23Select_UpsiUpsi2S_RV2    = DerivationFramework__Select_onia2mumu(
    name                       = "BPHY23Select_UpsiUpsi2S_RV2",
    HypothesisName             = "Upsi",
    InputVtxContainerName      = "BPHY23UpsiUpsi2S_RV2",
    TrkMasses                  = [105.658, 105.658],
    VtxMassHypo                = 9460.30,
    MassMin                    = 8800.,
    MassMax                    = 10000.,
    Chi2Max                    = 1.e6)

ToolSvc += BPHY23Select_UpsiUpsi2S_RV2
print      BPHY23Select_UpsiUpsi2S_RV2


CascadeCollections = []

CascadeCollections += BPHY23JpsiPsi.CascadeVertexCollections
CascadeCollections += BPHY23JpsiUpsi2S.CascadeVertexCollections
CascadeCollections += BPHY23UpsiPsi.CascadeVertexCollections
CascadeCollections += BPHY23UpsiUpsi2S.CascadeVertexCollections


#--------------------------------------------------------------------
## 7/ select the event. We only want to keep events that contain certain vertices which passed certain selection.
##    This is specified by the "SelectionExpression" property, which contains the expression in the following format:
##
##       "ContainerName.passed_HypoName > count"
##
##    where "ContainerName" is output container from some Reco_* tool, "HypoName" is the hypothesis name setup in some "Select_*"
##    tool and "count" is the number of candidates passing the selection you want to keep. 

from DerivationFrameworkTools.DerivationFrameworkToolsConf import DerivationFramework__xAODStringSkimmingTool
BPHY23_SelectEvent = DerivationFramework__xAODStringSkimmingTool(name = "BPHY23_SelectEvent", expression = "( count(BPHY23JpsiPsiCascadeVtx2.passed_JpsiPsi) + count(BPHY23JpsiUpsi2SCascadeVtx2.passed_JpsiUpsi2S) + count(BPHY23UpsiPsiCascadeVtx2.passed_UpsiPsi) + count(BPHY23UpsiUpsi2SCascadeVtx2.passed_UpsiUpsi2S) ) > 0")

ToolSvc += BPHY23_SelectEvent
print BPHY23_SelectEvent

#--------------------------------------------------------------------
## 8/ track and vertex thinning. We want to remove all reconstructed secondary vertices
##    which hasn't passed any of the selections defined by (Select_*) tools.
##    We also want to keep only tracks which are associates with either muons or any of the
##    vertices that passed the selection. Multiple thinning tools can perform the 
##    selection. The final thinning decision is based OR of all the decisions (by default,
##    although it can be changed by the JO).

## a) thining out vertices that didn't pass any selection and idetifying tracks associated with 
##    selected vertices. The "VertexContainerNames" is a list of the vertex containers, and "PassFlags"
##    contains all pass flags for Select_* tools that must be satisfied. The vertex is kept is it 
##    satisfy any of the listed selections.


#====================================================================
# CREATE THE DERIVATION KERNEL ALGORITHM AND PASS THE ABOVE TOOLS  
#====================================================================
## 9/ IMPORTANT bit. Don't forget to pass the tools to the DerivationKernel! If you don't do that, they will not be 
##    be executed!


# The name of the kernel (BPHY23Kernel in this case) must be unique to this derivation
from DerivationFrameworkCore.DerivationFrameworkCoreConf import DerivationFramework__DerivationKernel
augmentation_tools = [BPHY23_Reco_mumu, BPHY23FourTrackSelect_JpsiTrkTrk, BPHY23FourTrackSelect_UpsiTrkTrk, BPHY23_Select_Jpsi, BPHY23_Select_Upsi, BPHY23_Select_Psi, BPHY23_Select_Upsi2S, BPHY23JpsiPsi, BPHY23JpsiUpsi2S, BPHY23UpsiPsi, BPHY23UpsiUpsi2S, BPHY23Revertex_JpsiPsi_RV1, BPHY23Select_JpsiPsi_RV1, BPHY23Revertex_JpsiPsi_RV2, BPHY23Select_JpsiPsi_RV2, BPHY23Revertex_UpsiPsi_RV1, BPHY23Select_UpsiPsi_RV1, BPHY23Revertex_UpsiPsi_RV2, BPHY23Select_UpsiPsi_RV2, BPHY23Revertex_JpsiUpsi2S_RV1, BPHY23Select_JpsiUpsi2S_RV1, BPHY23Revertex_JpsiUpsi2S_RV2, BPHY23Select_JpsiUpsi2S_RV2, BPHY23Revertex_UpsiUpsi2S_RV1, BPHY23Select_UpsiUpsi2S_RV1, BPHY23Revertex_UpsiUpsi2S_RV2, BPHY23Select_UpsiUpsi2S_RV2]

DerivationFrameworkJob += CfgMgr.DerivationFramework__DerivationKernel(
    "BPHY23Kernel",
    AugmentationTools = augmentation_tools,
    SkimmingTools     = [BPHY23_SelectEvent]
)

#====================================================================
# SET UP STREAM   
#====================================================================
streamName = derivationFlags.WriteDAOD_BPHY23Stream.StreamName
fileName   = buildFileName( derivationFlags.WriteDAOD_BPHY23Stream )
BPHY23Stream = MSMgr.NewPoolRootStream( streamName, fileName )
BPHY23Stream.AcceptAlgs(["BPHY23Kernel"])
# Special lines for thinning
# Thinning service name must match the one passed to the thinning tools
from AthenaServices.Configurables import ThinningSvc, createThinningSvc
augStream = MSMgr.GetStream( streamName )
evtStream = augStream.GetEventStream()
svcMgr += createThinningSvc( svcName="BPHY23ThinningSvc", outStreams=[evtStream] )


#====================================================================
# Slimming 
#====================================================================

from DerivationFrameworkCore.SlimmingHelper import SlimmingHelper
BPHY23SlimmingHelper = SlimmingHelper("BPHY23SlimmingHelper")
BPHY23_AllVariables = []
BPHY23_StaticContent = []

# Needed for trigger objects
BPHY23SlimmingHelper.IncludeMuonTriggerContent = True
BPHY23SlimmingHelper.IncludeBPhysTriggerContent = True

## primary vertices
BPHY23_AllVariables += ["PrimaryVertices"]
BPHY23_StaticContent += ["xAOD::VertexContainer#BPHY23JpsiPsiRefPrimaryVertices", "xAOD::VertexContainer#BPHY23JpsiUpsi2SRefPrimaryVertices", "xAOD::VertexContainer#BPHY23UpsiPsiRefPrimaryVertices", "xAOD::VertexContainer#BPHY23UpsiUpsi2SRefPrimaryVertices"]
BPHY23_StaticContent += ["xAOD::VertexAuxContainer#BPHY23JpsiPsiRefPrimaryVerticesAux.", "xAOD::VertexAuxContainer#BPHY23JpsiUpsi2SRefPrimaryVerticesAux.", "xAOD::VertexAuxContainer#BPHY23UpsiPsiRefPrimaryVerticesAux.", "xAOD::VertexAuxContainer#BPHY23UpsiUpsi2SRefPrimaryVerticesAux."]

## ID track particles
BPHY23_AllVariables += ["InDetTrackParticles"]

## combined / extrapolated muon track particles 
## (note: for tagged muons there is no extra TrackParticle collection since the ID tracks
##        are stored in InDetTrackParticles collection)
BPHY23_AllVariables += ["CombinedMuonTrackParticles"]
BPHY23_AllVariables += ["ExtrapolatedMuonTrackParticles"]

## muon container
BPHY23_AllVariables += ["Muons"]


for cascades in CascadeCollections:
   BPHY23_StaticContent += ["xAOD::VertexContainer#%s"   %   cascades]
## we have to disable vxTrackAtVertex branch since it is not xAOD compatible
   BPHY23_StaticContent += ["xAOD::VertexAuxContainer#%sAux.-vxTrackAtVertex" % cascades]

BPHY23_StaticContent += ["xAOD::VertexContainer#%s"        % BPHY23Revertex_JpsiPsi_RV1.OutputVtxContainerName]
BPHY23_StaticContent += ["xAOD::VertexAuxContainer#%sAux.-vxTrackAtVertex" % BPHY23Revertex_JpsiPsi_RV1.OutputVtxContainerName]

BPHY23_StaticContent += ["xAOD::VertexContainer#%s"        % BPHY23Revertex_JpsiPsi_RV2.OutputVtxContainerName]
BPHY23_StaticContent += ["xAOD::VertexAuxContainer#%sAux.-vxTrackAtVertex" % BPHY23Revertex_JpsiPsi_RV2.OutputVtxContainerName]

BPHY23_StaticContent += ["xAOD::VertexContainer#%s"        % BPHY23Revertex_UpsiPsi_RV1.OutputVtxContainerName]
BPHY23_StaticContent += ["xAOD::VertexAuxContainer#%sAux.-vxTrackAtVertex" % BPHY23Revertex_UpsiPsi_RV1.OutputVtxContainerName]

BPHY23_StaticContent += ["xAOD::VertexContainer#%s"        % BPHY23Revertex_UpsiPsi_RV2.OutputVtxContainerName]
BPHY23_StaticContent += ["xAOD::VertexAuxContainer#%sAux.-vxTrackAtVertex" % BPHY23Revertex_UpsiPsi_RV2.OutputVtxContainerName]

BPHY23_StaticContent += ["xAOD::VertexContainer#%s"        % BPHY23Revertex_JpsiUpsi2S_RV1.OutputVtxContainerName]
BPHY23_StaticContent += ["xAOD::VertexAuxContainer#%sAux.-vxTrackAtVertex" % BPHY23Revertex_JpsiUpsi2S_RV1.OutputVtxContainerName]

BPHY23_StaticContent += ["xAOD::VertexContainer#%s"        % BPHY23Revertex_JpsiUpsi2S_RV2.OutputVtxContainerName]
BPHY23_StaticContent += ["xAOD::VertexAuxContainer#%sAux.-vxTrackAtVertex" % BPHY23Revertex_JpsiUpsi2S_RV2.OutputVtxContainerName]

BPHY23_StaticContent += ["xAOD::VertexContainer#%s"        % BPHY23Revertex_UpsiUpsi2S_RV1.OutputVtxContainerName]
BPHY23_StaticContent += ["xAOD::VertexAuxContainer#%sAux.-vxTrackAtVertex" % BPHY23Revertex_UpsiUpsi2S_RV1.OutputVtxContainerName]

BPHY23_StaticContent += ["xAOD::VertexContainer#%s"        % BPHY23Revertex_UpsiUpsi2S_RV2.OutputVtxContainerName]
BPHY23_StaticContent += ["xAOD::VertexAuxContainer#%sAux.-vxTrackAtVertex" % BPHY23Revertex_UpsiUpsi2S_RV2.OutputVtxContainerName]


# Truth information for MC only
if isSimulation:
    BPHY23_AllVariables += ["TruthEvents","TruthParticles","TruthVertices","MuonTruthParticles"]

BPHY23SlimmingHelper.AllVariables = BPHY23_AllVariables
BPHY23SlimmingHelper.StaticContent = BPHY23_StaticContent
BPHY23SlimmingHelper.AppendContentToStream(BPHY23Stream)
