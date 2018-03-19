#====================================================================
# BPHY15.py
# Bc+>J/psiDs+ and Bc+>J/psiD+
# It requires the reductionConf flag BPHY15 in Reco_tf.py   
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
BPHY15_VertexTools = BPHYVertexTools("BPHY15")


from DerivationFrameworkBPhys.DerivationFrameworkBPhysConf import DerivationFramework__AugOriginalCounts
BPHY15_AugOriginalCounts = DerivationFramework__AugOriginalCounts(
   name = "BPHY15_AugOriginalCounts",
   VertexContainer = "PrimaryVertices",
   TrackContainer = "InDetTrackParticles" )
ToolSvc += BPHY15_AugOriginalCounts


#--------------------------------------------------------------------
# 2/ Select J/psi>mu+mu-
#--------------------------------------------------------------------
## a/ setup JpsiFinder tool
##    These are general tools independent of DerivationFramework that do the 
##    actual vertex fitting and some pre-selection.
from JpsiUpsilonTools.JpsiUpsilonToolsConf import Analysis__JpsiFinder
BPHY15JpsiFinder = Analysis__JpsiFinder(
    name                        = "BPHY15JpsiFinder",
    OutputLevel                 = INFO,
    muAndMu                     = True,
    muAndTrack                  = False,
    TrackAndTrack               = False,
    assumeDiMuons               = True, 
    invMassUpper                = 3600.0,
    invMassLower                = 2600.0,
    Chi2Cut                     = 30.,
    oppChargesOnly	        = True,
    combOnly		        = True,
    atLeastOneComb              = False,
    useCombinedMeasurement      = False, # Only takes effect if combOnly=True	
    muonCollectionKey           = "Muons",
    TrackParticleCollection     = "InDetTrackParticles",
    V0VertexFitterTool          = BPHY15_VertexTools.TrkV0Fitter,             # V0 vertex fitter
    useV0Fitter                 = False,                   # if False a TrkVertexFitterTool will be used
    TrkVertexFitterTool         = BPHY15_VertexTools.TrkVKalVrtFitter,        # VKalVrt vertex fitter
    TrackSelectorTool           = BPHY15_VertexTools.InDetTrackSelectorTool,
    ConversionFinderHelperTool  = BPHY15_VertexTools.InDetConversionHelper,
    VertexPointEstimator        = BPHY15_VertexTools.VtxPointEstimator,
    useMCPCuts                  = False)

ToolSvc += BPHY15JpsiFinder
print      BPHY15JpsiFinder

#--------------------------------------------------------------------
## b/ setup the vertex reconstruction "call" tool(s). They are part of the derivation framework.
##    These Augmentation tools add output vertex collection(s) into the StoreGate and add basic 
##    decorations which do not depend on the vertex mass hypothesis (e.g. lxy, ptError, etc).
##    There should be one tool per topology, i.e. Jpsi and Psi(2S) do not need two instance of the
##    Reco tool is the JpsiFinder mass window is wide enough.
from DerivationFrameworkBPhys.DerivationFrameworkBPhysConf import DerivationFramework__Reco_mumu
BPHY15JpsiSelectAndWrite   = DerivationFramework__Reco_mumu(
    name                   = "BPHY15JpsiSelectAndWrite",
    JpsiFinder             = BPHY15JpsiFinder,
    OutputVtxContainerName = "BPHY15JpsiCandidates",
    PVContainerName        = "PrimaryVertices",
    RefPVContainerName     = "SHOULDNOTBEUSED",
    DoVertexType = 1)

ToolSvc += BPHY15JpsiSelectAndWrite
print BPHY15JpsiSelectAndWrite

from DerivationFrameworkBPhys.DerivationFrameworkBPhysConf import DerivationFramework__Select_onia2mumu

#--------------------------------------------------------------------
## c/ augment and select Jpsi->mumu candidates
BPHY15_Select_Jpsi2mumu = DerivationFramework__Select_onia2mumu(
  name                  = "BPHY15_Select_Jpsi2mumu",
  HypothesisName        = "Jpsi",
  InputVtxContainerName = "BPHY15JpsiCandidates",
  VtxMassHypo           = 3096.916,
  MassMin               = 2000.0,
  MassMax               = 3600.0,
  Chi2Max               = 200,
  DoVertexType =1)

  
ToolSvc += BPHY15_Select_Jpsi2mumu
print      BPHY15_Select_Jpsi2mumu

#--------------------------------------------------------------------
# 3/ select Bc+>J/psi pi+
#--------------------------------------------------------------------
## a/ setup a new vertexing tool (necessary due to use of mass constraint) 
from TrkVKalVrtFitter.TrkVKalVrtFitterConf import Trk__TrkVKalVrtFitter
BcJpsipiVertexFit = Trk__TrkVKalVrtFitter(
                                         name                = "BcJpsipiVertexFit",
                                         Extrapolator        = BPHY15_VertexTools.InDetExtrapolator,
                                         FirstMeasuredPoint  = True,
                                         MakeExtendedVertex  = True)
ToolSvc += BcJpsipiVertexFit
print      BcJpsipiVertexFit

#--------------------------------------------------------------------
## b/ setup the Jpsi+1 track finder
from JpsiUpsilonTools.JpsiUpsilonToolsConf import Analysis__JpsiPlus1Track
BPHY15BcJpsipi = Analysis__JpsiPlus1Track(
                                                name                    = "BPHY15BcJpsipi",
                                                OutputLevel             = INFO, #DEBUG,
                                                pionHypothesis          = True, #False,
                                                kaonHypothesis	        = False,#True,
                                               #trkThresholdPt	        = 800.0,
                                                trkThresholdPt	        = 0.0,
                                                trkMaxEta               = 2.5,
                                                BThresholdPt            = 100.0,
                                                BMassUpper	        = 7000.0,
                                                BMassLower	        = 2000.0,
                                               #BMassLower	        = 4500.0,
                                                JpsiContainerKey        = "BPHY15JpsiCandidates",
                                                TrackParticleCollection = "InDetTrackParticles",
                                                MuonsUsedInJpsi		= "Muons",
                                                TrkVertexFitterTool	= BcJpsipiVertexFit,
                                                TrackSelectorTool	= BPHY15_VertexTools.InDetTrackSelectorTool,
                                                UseMassConstraint	= True, 
                                                Chi2Cut 		= 10,
                                                TrkTrippletMassUpper 	= 7000,
                                               #TrkTrippletMassLower 	= 4500 )
                                                TrkTrippletMassLower 	= 2000 )
        
ToolSvc += BPHY15BcJpsipi
print      BPHY15BcJpsipi    

#--------------------------------------------------------------------
## c/ setup the combined augmentation/skimming tool for the Bc+>J/psi pi+
from DerivationFrameworkBPhys.DerivationFrameworkBPhysConf import DerivationFramework__Reco_dimuTrk
BPHY15BcJpsipiSelectAndWrite = DerivationFramework__Reco_dimuTrk(
						name                   	= "BPHY15BcJpsipiSelectAndWrite",
                                               	Jpsi1PlusTrackName     	= BPHY15BcJpsipi,
                                                OutputVtxContainerName 	= "BcJpsipiCandidates",
                                                PVContainerName        	= "PrimaryVertices",
                                                RefPVContainerName     	= "BPHY15RefittedPrimaryVertices",
                                                RefitPV                	= True,
                                                MaxPVrefit		= 1000)
ToolSvc += BPHY15BcJpsipiSelectAndWrite 
print BPHY15BcJpsipiSelectAndWrite

#--------------------------------------------------------------------
## c/ augment and select B_c+>Jpsi pi+ candidates
BPHY15_Select_Bc2Jpsipi = DerivationFramework__Select_onia2mumu(
  name                       = "BPHY15_Select_Bc2Jpsipi",
  HypothesisName             = "Bc",
  InputVtxContainerName      = "BcJpsipiCandidates",
  TrkMasses                  = [105.658, 105.658, 139.571],
  VtxMassHypo                = 6274.9,
  MassMin                    = 4000.0,
  MassMax                    = 8000.0,
  Chi2Max                    = 200)

ToolSvc += BPHY15_Select_Bc2Jpsipi
print BPHY15_Select_Bc2Jpsipi

#--------------------------------------------------------------------

from JpsiUpsilonTools.JpsiUpsilonToolsConf import Analysis__JpsiFinder
BPHY15Ks0Finder = Analysis__JpsiFinder(
  name                        = "BPHY15Ks0Finder",
  OutputLevel                 = INFO,
  muAndMu                     = False,
  muAndTrack                  = False,
  TrackAndTrack               = True,
  assumeDiMuons               = False,    # If true, will assume dimu hypothesis and use PDG value for mu mass
  invMassUpper                = 700.0,
  invMassLower                = 300.0,
  Chi2Cut                     = 100.,
  oppChargesOnly	      = True,
  atLeastOneComb              = False,
  useCombinedMeasurement      = False, # Only takes effect if combOnly=True	
  muonCollectionKey           = "Muons",
  TrackParticleCollection     = "InDetTrackParticles",
  V0VertexFitterTool          = BPHY15_VertexTools.TrkV0Fitter,             # V0 vertex fitter
  useV0Fitter                 = False,                   # if False a TrkVertexFitterTool will be used
  TrkVertexFitterTool         = BPHY15_VertexTools.TrkVKalVrtFitter,        # VKalVrt vertex fitter
  TrackSelectorTool           = BPHY15_VertexTools.InDetTrackSelectorTool,
  ConversionFinderHelperTool  = BPHY15_VertexTools.InDetConversionHelper,
  VertexPointEstimator        = BPHY15_VertexTools.VtxPointEstimator,
  useMCPCuts                  = False,
  track1Mass                  = 139.571, # Not very important, only used to calculate inv. mass cut, leave it loose here
  track2Mass                  = 139.571)
  
ToolSvc += BPHY15Ks0Finder
print      BPHY15Ks0Finder

#--------------------------------------------------------------------
from DerivationFrameworkBPhys.DerivationFrameworkBPhysConf import DerivationFramework__Reco_mumu
BPHY15_Reco_pipi = DerivationFramework__Reco_mumu(
  name                   = "BPHY15_Reco_pipi",
  JpsiFinder             = BPHY15Ks0Finder,
  OutputVtxContainerName = "BPHY15Ks0Candidates",
  PVContainerName        = "PrimaryVertices",
  RefPVContainerName     = "SHOULDNOTBEUSED",
  DoVertexType = 1)
  
ToolSvc += BPHY15_Reco_pipi
print BPHY15_Reco_pipi
#--------------------------------------------------------------------

from DerivationFrameworkBPhys.DerivationFrameworkBPhysConf import DerivationFramework__Select_onia2mumu

#--------------------------------------------------------------------
## a/ augment and select X->K+K- candidates
BPHY15_Select_K0 = DerivationFramework__Select_onia2mumu(
  name                  = "BPHY15_Select_K0",
  HypothesisName        = "KS0",
  InputVtxContainerName = "BPHY15Ks0Candidates",
  TrkMasses             = [139.571, 139.571],
  VtxMassHypo           = 497.61,
  MassMin               = 200.0,
  MassMax               = 800.0,
  Chi2Max               = 100)
  
ToolSvc += BPHY15_Select_K0
print BPHY15_Select_K0


#--------------------------------------------------------------------
# 4/ Select K+K- and K+pi-
#--------------------------------------------------------------------
## a/ Setup the vertex fitter tools
from JpsiUpsilonTools.JpsiUpsilonToolsConf import Analysis__JpsiFinder
BPHY15DiTrkFinder = Analysis__JpsiFinder(
  name                        = "BPHY15DiTrkFinder",
  OutputLevel                 = INFO,
  muAndMu                     = False,
  muAndTrack                  = False,
  TrackAndTrack               = True,
  assumeDiMuons               = False,    # If true, will assume dimu hypothesis and use PDG value for mu mass
  invMassUpper                = 3000.0,
  invMassLower                = 0.0,
  Chi2Cut                     = 100.,
  oppChargesOnly	      = True,
  atLeastOneComb              = False,
  useCombinedMeasurement      = False, # Only takes effect if combOnly=True	
  muonCollectionKey           = "Muons",
  TrackParticleCollection     = "InDetTrackParticles",
  V0VertexFitterTool          = BPHY15_VertexTools.TrkV0Fitter,             # V0 vertex fitter
  useV0Fitter                 = False,                   # if False a TrkVertexFitterTool will be used
  TrkVertexFitterTool         = BPHY15_VertexTools.TrkVKalVrtFitter,        # VKalVrt vertex fitter
  TrackSelectorTool           = BPHY15_VertexTools.InDetTrackSelectorTool,
  ConversionFinderHelperTool  = BPHY15_VertexTools.InDetConversionHelper,
  VertexPointEstimator        = BPHY15_VertexTools.VtxPointEstimator,
  useMCPCuts                  = False,
  track1Mass                  = 139.571, # Not very important, only used to calculate inv. mass cut, leave it loose here
  track2Mass                  = 139.571)
  
ToolSvc += BPHY15DiTrkFinder
print      BPHY15DiTrkFinder

#--------------------------------------------------------------------
## b/ setup the vertex reconstruction "call" tool(s).
from DerivationFrameworkBPhys.DerivationFrameworkBPhysConf import DerivationFramework__Reco_mumu
BPHY15_Reco_diTrk = DerivationFramework__Reco_mumu(
  name                   = "BPHY15_Reco_diTrk",
  JpsiFinder             = BPHY15DiTrkFinder,
  OutputVtxContainerName = "BPHY15DiTrkCandidates",
  PVContainerName        = "PrimaryVertices",
  RefPVContainerName     = "SHOULDNOTBEUSED",
  DoVertexType = 1)
  
ToolSvc += BPHY15_Reco_diTrk
print BPHY15_Reco_diTrk


from DerivationFrameworkBPhys.DerivationFrameworkBPhysConf import DerivationFramework__Select_onia2mumu

#--------------------------------------------------------------------
## c/ augment and select X->K+K- candidates
BPHY15_Select_KK = DerivationFramework__Select_onia2mumu(
  name                  = "BPHY15_Select_KK",
  HypothesisName        = "KK",
  InputVtxContainerName = "BPHY15DiTrkCandidates",
  TrkMasses             = [493.677,493.677],
  VtxMassHypo           = 1019.469,
  MassMin               = 0,
  MassMax               = 3000.0,
  Chi2Max               = 100)
  
ToolSvc += BPHY15_Select_KK
print BPHY15_Select_KK

#--------------------------------------------------------------------
## d/ augment and select X->K+pi- candidates
BPHY15_Select_Kpi = DerivationFramework__Select_onia2mumu(
  name                  = "BPHY15_Select_Kpi",
  HypothesisName        = "Kpi",
  InputVtxContainerName = "BPHY15DiTrkCandidates",
  TrkMasses             = [493.677,139.57],
  VtxMassHypo           = 895.55,
  MassMin               = 0,
  MassMax               = 3000.0,
  Chi2Max               = 100)
  
ToolSvc += BPHY15_Select_Kpi
print BPHY15_Select_Kpi

#--------------------------------------------------------------------
# 5/ select Ds+>K+K-pi+ and D+>K+pi-pi- candidates
#--------------------------------------------------------------------
## a/ setup a new vertexing tool (necessary due to use of mass constraint) 
from TrkVKalVrtFitter.TrkVKalVrtFitterConf import Trk__TrkVKalVrtFitter
DsVertexFit = Trk__TrkVKalVrtFitter(
                                     name                = "DsVertexFit",
                                     Extrapolator        = BPHY15_VertexTools.InDetExtrapolator,
                                     FirstMeasuredPoint  = True,
                                     MakeExtendedVertex  = True)
ToolSvc += DsVertexFit

#--------------------------------------------------------------------
## b/ setup the Jpsi+1 track finder
from JpsiUpsilonTools.JpsiUpsilonToolsConf import Analysis__JpsiPlus1Track
BPHY15DsFinder = Analysis__JpsiPlus1Track(
                                            name                    = "BPHY15DsFinder",
                                            OutputLevel             = INFO,
                                            pionHypothesis          = True,
                                            kaonHypothesis          = False,
                                            trkThresholdPt	    = 1000.0,
                                           #trkMaxEta	            = 2.5, # is this value fine?? default would be 102.5
                                            BThresholdPt            = 1000.0,
                                            BMassUpper              = 5000.0, # What is this??
                                            BMassLower       	    = 0.0,
                                            JpsiContainerKey        = "BPHY15DiTrkCandidates",
                                            TrackParticleCollection = "InDetTrackParticles",
                                            MuonsUsedInJpsi         = "NONE", # ?
                                            ExcludeCrossJpsiTracks  = False,
                                            TrkVertexFitterTool     = DsVertexFit,
                                            TrackSelectorTool       = BPHY15_VertexTools.InDetTrackSelectorTool,
                                            UseMassConstraint       = False, 
                                            Chi2Cut                 = 150) #Cut on chi2/Ndeg_of_freedom, so is very loose
												
        
ToolSvc += BPHY15DsFinder

#--------------------------------------------------------------------
## c/ setup the combined augmentation/skimming tool for the D(s)+
from DerivationFrameworkBPhys.DerivationFrameworkBPhysConf import DerivationFramework__Reco_dimuTrk
BPHY15DsSelectAndWrite = DerivationFramework__Reco_dimuTrk(
                                           name                   = "BPHY15DsSelectAndWrite",
                                           OutputLevel            = INFO,
                                           Jpsi1PlusTrackName     = BPHY15DsFinder,
                                           OutputVtxContainerName = "BPHY15DsCandidates",
                                           PVContainerName        = "PrimaryVertices",
                                           RefPVContainerName     = "SHOULDNOTBEUSED",
                                           MaxPVrefit             = 1000)
ToolSvc += BPHY15DsSelectAndWrite 

#--------------------------------------------------------------------
## d/ augment and select Ds+>K+K-pi+ candidates
BPHY15_Select_Ds = DerivationFramework__Select_onia2mumu(
  name                       = "BPHY15_Select_Ds",
  OutputLevel                = INFO,
  HypothesisName             = "Ds",
  InputVtxContainerName      = "BPHY15DsCandidates",
  TrkMasses                  = [493.677, 493.677, 139.57],
  VtxMassHypo                = 1968.28,
  MassMin                    = 1968.28 - 500.,
  MassMax                    = 1968.28 + 500.,
  Chi2Max                    = 100.)

ToolSvc += BPHY15_Select_Ds

#--------------------------------------------------------------------
## e/ augment and select D+>K+pi-pi+ candidates
BPHY15_Select_Dp = DerivationFramework__Select_onia2mumu(
  name                       = "BPHY15_Select_Dp",
  OutputLevel                = INFO,
  HypothesisName             = "Dp",
  InputVtxContainerName      = "BPHY15DsCandidates",
  TrkMasses                  = [493.677, 139.57, 139.57],
  VtxMassHypo                = 1869.59,
  MassMin                    = 1869.59 - 500.,
  MassMax                    = 1869.59 + 500.,
  Chi2Max                    = 100.)

ToolSvc += BPHY15_Select_Dp



#--------------------------------------------------------------------
# 6/ select Bc+>J/psi D_s+
#--------------------------------------------------------------------
## a/ setup the cascade vertexing tool
from TrkVKalVrtFitter.TrkVKalVrtFitterConf import Trk__TrkVKalVrtFitter
BcJpsiDsVertexFit = Trk__TrkVKalVrtFitter(
    name                 = "BcJpsiDsVertexFit",
   #OutputLevel          = DEBUG,
    Extrapolator         = BPHY15_VertexTools.InDetExtrapolator,
   #FirstMeasuredPoint   = True,
    FirstMeasuredPoint   = False,
    CascadeCnstPrecision = 1e-6,
    MakeExtendedVertex   = True)

ToolSvc += BcJpsiDsVertexFit
print      BcJpsiDsVertexFit

#--------------------------------------------------------------------
## b/ setup the Jpsi+Ds finder
from DerivationFrameworkBPhys.DerivationFrameworkBPhysConf import DerivationFramework__JpsiPlusDsCascade
BPHY15JpsiDs                = DerivationFramework__JpsiPlusDsCascade(
    name                    = "BPHY15JpsiDs",
   #OutputLevel             = DEBUG,
   #ApplyV0MassConstraint   = False,
   #ApplyJpsiMassConstraint = False,
    HypothesisName          = "Bc",
    TrkVertexFitterTool     = BcJpsiDsVertexFit,
    V0Hypothesis            = 431,
   #JpsiMassLowerCut        = 2400.,
   #JpsiMassUpperCut        = 3800.,
    JpsiMassLowerCut        = 2900.,
    JpsiMassUpperCut        = 3300.,
   #V0MassLowerCut          = 1968.28 - 500.,
   #V0MassUpperCut          = 1968.28 + 500.,
    V0MassLowerCut          = 1930.,
    V0MassUpperCut          = 2010.,
    MassLowerCut            = 6274.90 - 500.,
    MassUpperCut            = 6274.90 + 500.,
    RefitPV                 = True,
    RefPVContainerName      = "BPHY15RefittedPrimaryVertices",
    JpsiVertices            = "BPHY15JpsiCandidates",
    CascadeVertexCollections= ["BcJpsiDsCascadeSV2", "BcJpsiDsCascadeSV1"],
    V0Vertices              = "BPHY15DsCandidates")

ToolSvc += BPHY15JpsiDs
print BPHY15JpsiDs

#--------------------------------------------------------------------
# 7/ select Bc+>J/psi D+
#--------------------------------------------------------------------
## a/ setup the cascade vertexing tool
from TrkVKalVrtFitter.TrkVKalVrtFitterConf import Trk__TrkVKalVrtFitter
BcJpsiDpVertexFit = Trk__TrkVKalVrtFitter(
    name                 = "BcJpsiDpVertexFit",
   #OutputLevel          = DEBUG,
    Extrapolator         = BPHY15_VertexTools.InDetExtrapolator,
   #FirstMeasuredPoint   = True,
    FirstMeasuredPoint   = False,
    CascadeCnstPrecision = 1e-6,
    MakeExtendedVertex   = True)

ToolSvc += BcJpsiDpVertexFit
print      BcJpsiDpVertexFit
#--------------------------------------------------------------------
## c/ setup the Jpsi+D finder
from DerivationFrameworkBPhys.DerivationFrameworkBPhysConf import DerivationFramework__JpsiPlusDsCascade
BPHY15JpsiDp                = DerivationFramework__JpsiPlusDsCascade(
    name                    = "BPHY15JpsiDp",
   #OutputLevel             = DEBUG,
    HypothesisName          = "Bc",
    TrkVertexFitterTool     = BcJpsiDpVertexFit,
    V0Hypothesis            = 411,
   #JpsiMassLowerCut        = 2400.,
   #JpsiMassUpperCut        = 3800.,
    JpsiMassLowerCut        = 2900.,
    JpsiMassUpperCut        = 3300.,
   #V0MassLowerCut          = 1869.59 - 500.,
   #V0MassUpperCut          = 1869.59 + 500.,
    V0MassLowerCut          = 1830.,
    V0MassUpperCut          = 1910.,
    MassLowerCut            = 6274.90 - 500.,
    MassUpperCut            = 6274.90 + 500.,
    RefitPV                 = True,
    RefPVContainerName      = "BPHY15RefittedPrimaryVertices",
    JpsiVertices            = "BPHY15JpsiCandidates",
    CascadeVertexCollections= ["BcJpsiDpCascadeSV2", "BcJpsiDpCascadeSV1"],
    V0Vertices              = "BPHY15DsCandidates")

ToolSvc += BPHY15JpsiDp
print BPHY15JpsiDp

#--------------------------------------------------------------------
# 8/ select Bc+>J/psi D*+
#--------------------------------------------------------------------
## a/ setup the cascade vertexing tool
from TrkVKalVrtFitter.TrkVKalVrtFitterConf import Trk__TrkVKalVrtFitter
BcJpsiDpstVertexFit = Trk__TrkVKalVrtFitter(
    name                 = "BcJpsiDpstVertexFit",
   #OutputLevel          = DEBUG,
    Extrapolator         = BPHY15_VertexTools.InDetExtrapolator,
   #FirstMeasuredPoint   = True,
    FirstMeasuredPoint   = False,
    CascadeCnstPrecision = 1e-6,
    MakeExtendedVertex   = True)

ToolSvc += BcJpsiDpstVertexFit
print      BcJpsiDpstVertexFit

from DerivationFrameworkBPhys.DerivationFrameworkBPhysConf import DerivationFramework__JpsiPlusDpstCascade
BPHY15JpsiDpst               = DerivationFramework__JpsiPlusDpstCascade(
    name                     = "BPHY15JpsiDpst",
    OutputLevel              = DEBUG,
    TrkVertexFitterTool      = BcJpsiDpstVertexFit,
   #ApplyJpsiMassConstraint  = False,
   #ApplyV0MassConstraint    = False,
    V0Hypothesis             = 421,
    JpsiMassLowerCut         = 2900.,
    JpsiMassUpperCut         = 3300.,
    JpsipiMassLowerCut       = 2400.,
    JpsipiMassUpperCut       = 6800.,
    D0MassLowerCut           = 1864.83 - 150.,
    D0MassUpperCut           = 1864.83 + 150.,
    DpstMassLowerCut         = 2010.26 - 200.,
    DpstMassUpperCut         = 2010.26 + 200.,
    MassLowerCut             = 6274.90 - 500,
    MassUpperCut             = 6274.90 + 500.,
    RefitPV                  = True,
    RefPVContainerName       = "BPHY15RefittedPrimaryVertices",
    JpsipiVertices           = "BcJpsipiCandidates",
    CascadeVertexCollections = ["BcJpsiDpstCascadeSV2", "BcJpsiDpstCascadeSV1" ] ,
    D0Vertices               = "BPHY15DiTrkCandidates")

ToolSvc += BPHY15JpsiDpst
print BPHY15JpsiDpst

#--------------------------------------------------------------------
# 9/ select Bc+>J/psi D_s1+
#--------------------------------------------------------------------
## a/ setup the cascade vertexing tool
from TrkVKalVrtFitter.TrkVKalVrtFitterConf import Trk__TrkVKalVrtFitter
BcJpsiDs1VertexFit = Trk__TrkVKalVrtFitter(
    name                 = "BcJpsiDs1VertexFit",
   #OutputLevel          = DEBUG,
    Extrapolator         = BPHY15_VertexTools.InDetExtrapolator,
   #FirstMeasuredPoint   = True,
    FirstMeasuredPoint   = False,
    CascadeCnstPrecision = 1e-6,
    MakeExtendedVertex   = True)

ToolSvc += BcJpsiDs1VertexFit
print      BcJpsiDs1VertexFit

from DerivationFrameworkBPhys.DerivationFrameworkBPhysConf import DerivationFramework__JpsiPlusDs1Cascade
BPHY15JpsiDs1               = DerivationFramework__JpsiPlusDs1Cascade(
    name                     = "BPHY15JpsiDs1",
    OutputLevel              = DEBUG,
    TrkVertexFitterTool      = BcJpsiDs1VertexFit,
   #ApplyJpsiMassConstraint  = False,
   #ApplyV0MassConstraint    = False,
    V0Hypothesis             = 421,
    K0Hypothesis            = 310,
    JpsiMassLowerCut         = 2900.,
    JpsiMassUpperCut         = 3300.,
    JpsipiMassLowerCut       = 2400.,
    JpsipiMassUpperCut       = 6800.,
    D0MassLowerCut           = 1864.83 - 150.,
    D0MassUpperCut           = 1864.83 + 150.,
    K0MassLowerCut          = 0.,
    K0MassUpperCut          = 1200.,
    DpstMassLowerCut         = 2010.26 - 200.,
    DpstMassUpperCut         = 2010.26 + 200.,
    MassLowerCut             = 6274.90 - 500,
    MassUpperCut             = 6274.90 + 500.,
    RefitPV                  = True,
    RefPVContainerName       = "BPHY15RefittedPrimaryVertices",
    JpsipiVertices           = "BcJpsipiCandidates",
    CascadeVertexCollections = ["BcJpsiDs1CascadeSV3", "BcJpsiDs1CascadeSV2", "BcJpsiDs1CascadeSV1" ] ,
    K0Vertices               = "BPHY15Ks0Candidates",
    D0Vertices               = "BPHY15DiTrkCandidates")

ToolSvc += BPHY15JpsiDs1
print BPHY15JpsiDs1

#--------------------------------------------------------------------

CascadeCollections = []
CascadeCollections += BPHY15JpsiDs.CascadeVertexCollections
CascadeCollections += BPHY15JpsiDp.CascadeVertexCollections
CascadeCollections += BPHY15JpsiDpst.CascadeVertexCollections
CascadeCollections += BPHY15JpsiDs1.CascadeVertexCollections

#--------------------------------------------------------------------


if not isSimulation: #Only Skim Data
   from DerivationFrameworkTools.DerivationFrameworkToolsConf import DerivationFramework__xAODStringSkimmingTool
   BPHY15_SelectBcJpsipiEvent = DerivationFramework__xAODStringSkimmingTool(
     name = "BPHY15_SelectBcJpsipiEvent",
    #expression = "count(BcJpsipiCandidates.passed_Bc > 0) > 0")
     expression = "(count(BcJpsipiCandidates.passed_Bc > 0) + count(BcJpsiDsCascadeSV1.x > -999) + count(BcJpsiDpCascadeSV1.x > -999) + count(BcJpsiDpstCascadeSV1.x > -999)  + count(BcJpsiDs1CascadeSV1.x > -999)) > 0")
    #expression = "(count(BcJpsipiCandidates.passed_Bc > 0) + count(BcJpsiDsCascadeSV1.x > -999) + count(BcJpsiDpCascadeSV1.x > -999) + count(BcJpsiDpstCascadeSV1.x > -999) ) > 0")
    #expression = "(count(BcJpsipiCandidates.passed_Bc > 0) + count(BcJpsiDsCascadeSV1.x > -999) + count(BcJpsiDpCascadeSV1.x > -999) ) > 0")
   
   ToolSvc += BPHY15_SelectBcJpsipiEvent
   print BPHY15_SelectBcJpsipiEvent

   #====================================================================
   # Make event selection based on an OR of the input skimming tools
   #====================================================================
      
   from DerivationFrameworkTools.DerivationFrameworkToolsConf import DerivationFramework__FilterCombinationOR
   BPHY15SkimmingOR = CfgMgr.DerivationFramework__FilterCombinationOR(
       "BPHY15SkimmingOR",
       FilterList = [BPHY15_SelectBcJpsipiEvent] )
   ToolSvc += BPHY15SkimmingOR
   print      BPHY15SkimmingOR

#--------------------------------------------------------------------
## 7/ track and vertex thinning. We want to remove all reconstructed secondary vertices
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
BPHY15_thinningTool_Tracks = DerivationFramework__Thin_vtxTrk(
  name                       = "BPHY15_thinningTool_Tracks",
  ThinningService            = "BPHY15ThinningSvc",
  TrackParticleContainerName = "InDetTrackParticles",
 #VertexContainerNames       = ["BcJpsipiCandidates", "BcJpsiDsCascadeSV1", "BcJpsiDsCascadeSV2", "BcJpsiDpCascadeSV1", "BcJpsiDpCascadeSV2", "BcJpsiDpstCascadeSV1", "BcJpsiDpstCascadeSV2"],
  VertexContainerNames       = ["BcJpsipiCandidates", "BcJpsiDsCascadeSV1", "BcJpsiDsCascadeSV2", "BcJpsiDpCascadeSV1", "BcJpsiDpCascadeSV2", "BcJpsiDpstCascadeSV1", "BcJpsiDpstCascadeSV2", "BcJpsiDs1CascadeSV1", "BcJpsiDs1CascadeSV2", "BcJpsiDs1CascadeSV3"],
 #VertexContainerNames       = ["BcJpsipiCandidates", "BcJpsiDsCascadeSV1", "BcJpsiDsCascadeSV2", "BcJpsiDpCascadeSV1", "BcJpsiDpCascadeSV2"],
  PassFlags                  = ["passed_Bc"] )

ToolSvc += BPHY15_thinningTool_Tracks

from DerivationFrameworkBPhys.DerivationFrameworkBPhysConf import DerivationFramework__BPhysPVThinningTool
BPHY15_thinningTool_PV = DerivationFramework__BPhysPVThinningTool(
  name                       = "BPHY15_thinningTool_PV",
  ThinningService            = "BPHY15ThinningSvc",
 #CandidateCollections       = ["BcJpsipiCandidates", "BcJpsiDsCascadeSV1", "BcJpsiDsCascadeSV2", "BcJpsiDpCascadeSV1", "BcJpsiDpCascadeSV2", "BcJpsiDpstCascadeSV1", "BcJpsiDpstCascadeSV2"],
  CandidateCollections       = ["BcJpsipiCandidates", "BcJpsiDsCascadeSV1", "BcJpsiDsCascadeSV2", "BcJpsiDpCascadeSV1", "BcJpsiDpCascadeSV2", "BcJpsiDpstCascadeSV1", "BcJpsiDpstCascadeSV2", "BcJpsiDs1CascadeSV1", "BcJpsiDs1CascadeSV2", "BcJpsiDs1CascadeSV3"],
 #CandidateCollections       = ["BcJpsipiCandidates", "BcJpsiDsCascadeSV1", "BcJpsiDsCascadeSV2", "BcJpsiDpCascadeSV1", "BcJpsiDpCascadeSV2"],
  KeepPVTracks  =True
 )

ToolSvc += BPHY15_thinningTool_PV


## b) thinning out tracks that are not attached to muons. The final thinning decision is based on the OR operation
##    between decision from this and the previous tools.
from DerivationFrameworkInDet.DerivationFrameworkInDetConf import DerivationFramework__MuonTrackParticleThinning
BPHY15MuonTPThinningTool = DerivationFramework__MuonTrackParticleThinning(
    name                    = "BPHY15MuonTPThinningTool",
    ThinningService         = "BPHY15ThinningSvc",
    MuonKey                 = "Muons",
    InDetTrackParticlesKey  = "InDetTrackParticles")
ToolSvc += BPHY15MuonTPThinningTool

#from DerivationFrameworkInDet.DerivationFrameworkInDetConf import DerivationFramework__EgammaTrackParticleThinning
#BPHY15ElectronTPThinningTool = DerivationFramework__EgammaTrackParticleThinning(  
#    name                    = "BPHY15ElectronTPThinningTool",
#    ThinningService         = "BPHY15ThinningSvc",
#    SGKey                   = "Electrons",
#    GSFTrackParticlesKey    = "GSFTrackParticles",        
#    InDetTrackParticlesKey  = "InDetTrackParticles",
#    SelectionString = "",
#    BestMatchOnly = True,
#    ConeSize = 0.3,
#    ApplyAnd = False)
#
#ToolSvc+=BPHY15ElectronTPThinningTool

#====================================================================
# CREATE THE DERIVATION KERNEL ALGORITHM AND PASS THE ABOVE TOOLS  
#====================================================================

thiningCollection = [] 
#thiningCollection = [BPHY15_thinningTool_Tracks, BPHY15_thinningTool_PV, BPHY15MuonTPThinningTool] 

print thiningCollection
# The name of the kernel (BPHY15Kernel in this case) must be unique to this derivation
from DerivationFrameworkCore.DerivationFrameworkCoreConf import DerivationFramework__DerivationKernel
DerivationFrameworkJob += CfgMgr.DerivationFramework__DerivationKernel(
    "BPHY15Kernel",
    AugmentationTools = [BPHY15JpsiSelectAndWrite, BPHY15_Select_Jpsi2mumu,
                         BPHY15BcJpsipiSelectAndWrite, BPHY15_Select_Bc2Jpsipi,
                         BPHY15_Reco_pipi,
                         BPHY15_Reco_diTrk,
                         BPHY15_Select_K0,
                         BPHY15_Select_KK, BPHY15_Select_Kpi,
                         BPHY15DsSelectAndWrite, BPHY15_Select_Ds, BPHY15_Select_Dp,
                         BPHY15JpsiDs, BPHY15JpsiDp, BPHY15JpsiDpst, BPHY15JpsiDs1, 
                         BPHY15_AugOriginalCounts],
    #Only skim if not MC
    SkimmingTools     = [BPHY15SkimmingOR] if not isSimulation else [],
    ThinningTools     = thiningCollection
    )

#====================================================================
# SET UP STREAM   
#====================================================================
streamName   = derivationFlags.WriteDAOD_BPHY15Stream.StreamName
fileName     = buildFileName( derivationFlags.WriteDAOD_BPHY15Stream )
BPHY15Stream  = MSMgr.NewPoolRootStream( streamName, fileName )
BPHY15Stream.AcceptAlgs(["BPHY15Kernel"])

# Special lines for thinning
# Thinning service name must match the one passed to the thinning tools
from AthenaServices.Configurables import ThinningSvc, createThinningSvc
augStream = MSMgr.GetStream( streamName )
evtStream = augStream.GetEventStream()

BPHY15ThinningSvc = createThinningSvc( svcName="BPHY15ThinningSvc", outStreams=[evtStream] )
svcMgr += BPHY15ThinningSvc

#====================================================================
# Slimming 
#====================================================================

# Added by ASC
from DerivationFrameworkCore.SlimmingHelper import SlimmingHelper
BPHY15SlimmingHelper = SlimmingHelper("BPHY15SlimmingHelper")
AllVariables  = []
StaticContent = []

# Needed for trigger objects
BPHY15SlimmingHelper.IncludeMuonTriggerContent  = TRUE
BPHY15SlimmingHelper.IncludeBPhysTriggerContent = TRUE

## primary vertices
AllVariables  += ["PrimaryVertices"]
StaticContent += ["xAOD::VertexContainer#BPHY15RefittedPrimaryVertices"]
StaticContent += ["xAOD::VertexAuxContainer#BPHY15RefittedPrimaryVerticesAux."]

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
StaticContent += ["xAOD::VertexContainer#%s"        %                 BPHY15JpsiSelectAndWrite.OutputVtxContainerName]
## we have to disable vxTrackAtVertex branch since it is not xAOD compatible
StaticContent += ["xAOD::VertexAuxContainer#%sAux.-vxTrackAtVertex" % BPHY15JpsiSelectAndWrite.OutputVtxContainerName]

## Bc+>J/psi pi+ candidates
StaticContent += ["xAOD::VertexContainer#%s"        %                 BPHY15BcJpsipiSelectAndWrite.OutputVtxContainerName]
StaticContent += ["xAOD::VertexAuxContainer#%sAux.-vxTrackAtVertex" % BPHY15BcJpsipiSelectAndWrite.OutputVtxContainerName]

## K+K- and K+pi- candidates
#StaticContent += ["xAOD::VertexContainer#%s"        %                 BPHY15_Reco_diTrk.OutputVtxContainerName]
#StaticContent += ["xAOD::VertexAuxContainer#%sAux.-vxTrackAtVertex" % BPHY15_Reco_diTrk.OutputVtxContainerName]

## Ds+>K+K-pi+ and D+>K+pi-pi- candidates 
#StaticContent += ["xAOD::VertexContainer#%s"        %                 BPHY15DsSelectAndWrite.OutputVtxContainerName]
#StaticContent += ["xAOD::VertexAuxContainer#%sAux.-vxTrackAtVertex" % BPHY15DsSelectAndWrite.OutputVtxContainerName]

## Bc+>J/psi Ds+ and Bc+>J/psi D+ candidates
for cascades in CascadeCollections:
   StaticContent += ["xAOD::VertexContainer#%s"   %     cascades]
   StaticContent += ["xAOD::VertexAuxContainer#%sAux.-vxTrackAtVertex" % cascades]

# Tagging information (in addition to that already requested by usual algorithms)
#AllVariables += ["Electrons"] 
AllVariables += ["GSFTrackParticles", "MuonSpectrometerTrackParticles" ] 
#AllVariables += ["GSFTrackParticles", "Electrons" , "Photons", "Kt4LCTopoEventShape" , "MuonSpectrometerTrackParticles" ] 
#tagJetCollections = ['AntiKt4LCTopoJets']
#
#for jet_collection in tagJetCollections:
#    AllVariables += [jet_collection]
#    AllVariables += ["BTagging_%s"       % (jet_collection[:-4]) ]
#    AllVariables += ["BTagging_%sJFVtx"  % (jet_collection[:-4]) ]
#    AllVariables += ["BTagging_%sSecVtx" % (jet_collection[:-4]) ]

# Added by ASC
# Truth information for MC only
if isSimulation:
    AllVariables += ["TruthEvents","TruthParticles","TruthVertices","MuonTruthParticles"]
   #AllVariables += ["AntiKt4TruthJets","egammaTruthParticles", "AntiKt4TruthWZJets" ]

AllVariables = list(set(AllVariables)) # remove duplicates

BPHY15SlimmingHelper.AllVariables = AllVariables
BPHY15SlimmingHelper.StaticContent = StaticContent
BPHY15SlimmingHelper.SmartCollections = []

BPHY15SlimmingHelper.AppendContentToStream(BPHY15Stream)
