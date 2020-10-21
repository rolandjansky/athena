#====================================================================
# BPHY3.py
# This an example job options script showing how to set up a 
# derivation of the data using the derivation framework.  
# It requires the reductionConf flag BPHY3 in Reco_tf.py   
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
#include( "JpsiUpsilonTools/configureServices.py" )

include("DerivationFrameworkBPhys/configureVertexing.py")
BPHY3_VertexTools = BPHYVertexTools("BPHY3")

#--------------------------------------------------------------------
## 2/ Setup the vertex fitter tools (e.g. JpsiFinder, JpsiPlus1Track, etc).
##    These are general tools independent of DerivationFramework that do the 
##    actual vertex fitting and some pre-selection.
from JpsiUpsilonTools.JpsiUpsilonToolsConf import Analysis__JpsiFinder
BPHY3JpsiFinder = Analysis__JpsiFinder(
  name                        = "BPHY3JpsiFinder",
  OutputLevel                 = INFO,
  muAndMu                     = False,
  muAndTrack                  = False,
  TrackAndTrack               = True,
  assumeDiMuons               = False,    # If true, will assume dimu hypothesis and use PDG value for mu mass
  invMassUpper                = 10000.0,
  invMassLower                = 0.0,
  Chi2Cut                     = 100.,
  oppChargesOnly	            = True,
  atLeastOneComb              = False,
  useCombinedMeasurement      = False, # Only takes effect if combOnly=True	
  muonCollectionKey           = "Muons",
  TrackParticleCollection     = "InDetTrackParticles",
  V0VertexFitterTool          = BPHY3_VertexTools.TrkV0Fitter,             # V0 vertex fitter
  useV0Fitter                 = False,                   # if False a TrkVertexFitterTool will be used
  TrkVertexFitterTool         = BPHY3_VertexTools.TrkVKalVrtFitter,        # VKalVrt vertex fitter
  TrackSelectorTool           = BPHY3_VertexTools.InDetTrackSelectorTool,
  ConversionFinderHelperTool  = BPHY3_VertexTools.InDetConversionHelper,
  VertexPointEstimator        = BPHY3_VertexTools.VtxPointEstimator,
  useMCPCuts                  = False,
  track1Mass                  = 139.57, # Not very important, only used to calculate inv. mass cut, leave it loose here
  track2Mass                  = 139.57)
  
ToolSvc += BPHY3JpsiFinder
print      BPHY3JpsiFinder

#--------------------------------------------------------------------
from DerivationFrameworkBPhys.DerivationFrameworkBPhysConf import DerivationFramework__Reco_mumu
BPHY3_Reco_diTrk = DerivationFramework__Reco_mumu(
  name                   = "BPHY3_Reco_diTrk",
  JpsiFinder             = BPHY3JpsiFinder,
  OutputVtxContainerName = "BPHY3VertexCandidates",
  PVContainerName        = "PrimaryVertices",
  RefPVContainerName     = "BPHY3RefittedPrimaryVertices")
  
ToolSvc += BPHY3_Reco_diTrk
print BPHY3_Reco_diTrk
#--------------------------------------------------------------------


from DerivationFrameworkBPhys.DerivationFrameworkBPhysConf import DerivationFramework__Select_onia2mumu

#--------------------------------------------------------------------
## a/ augment and select X->pi+pi- candidates
BPHY3_Select_PiPi = DerivationFramework__Select_onia2mumu(
  name                  = "BPHY3_Select_PiPi",
  HypothesisName        = "PiPi",
  InputVtxContainerName = "BPHY3VertexCandidates",
  TrkMasses             = [139.57,139.57],
  VtxMassHypo           = 497.614,
  MassMin               = 300.0,
  MassMax               = 700.0,
  Chi2Max               = 20)
  
ToolSvc += BPHY3_Select_PiPi
print BPHY3_Select_PiPi
#--------------------------------------------------------------------

#--------------------------------------------------------------------
## a/ augment and select X->piK candidates
BPHY3_Select_PiK = DerivationFramework__Select_onia2mumu(
  name                  = "BPHY3_Select_PiK",
  HypothesisName        = "PiK",
  InputVtxContainerName = "BPHY3VertexCandidates",
  TrkMasses             = [139.57,493.677],
  VtxMassHypo           = 892.,
  MassMin               = 0.0,
  MassMax               = 3500.0,
  Chi2Max               = 10)
  
ToolSvc += BPHY3_Select_PiK
print BPHY3_Select_PiK
#--------------------------------------------------------------------

#--------------------------------------------------------------------
## a/ augment and select X->KPi candidates
BPHY3_Select_KPi = DerivationFramework__Select_onia2mumu(
  name                  = "BPHY3_Select_KPi",
  HypothesisName        = "KPi",
  InputVtxContainerName = "BPHY3VertexCandidates",
  TrkMasses             = [493.677,139.57],
  VtxMassHypo           = 892.,
  MassMin               = 0.0,
  MassMax               = 3500.0,
  Chi2Max               = 10)
  
ToolSvc += BPHY3_Select_KPi
print BPHY3_Select_KPi
#--------------------------------------------------------------------


#--------------------------------------------------------------------
## a/ augment and select X->K+K- candidates
BPHY3_Select_KK = DerivationFramework__Select_onia2mumu(
  name                  = "BPHY3_Select_KK",
  HypothesisName        = "KK",
  InputVtxContainerName = "BPHY3VertexCandidates",
  TrkMasses             = [493.677,493.677],
  VtxMassHypo           = 1019.461,
  MassMin               = 0.0,
  MassMax               = 1100.0,
  Chi2Max               = 20)
  
ToolSvc += BPHY3_Select_KK
print BPHY3_Select_KK
#--------------------------------------------------------------------

#--------------------------------------------------------------------
## a/ augment and select X->ppbar candidates
BPHY3_Select_PP = DerivationFramework__Select_onia2mumu(
  name                  = "BPHY3_Select_PP",
  HypothesisName        = "PP",
  InputVtxContainerName = "BPHY3VertexCandidates",
  TrkMasses             = [938.272,938.272],
  VtxMassHypo           = 3096.916,
  MassMin               = 2800.0,
  MassMax               = 3600.0,
  Chi2Max               = 1)
  
ToolSvc += BPHY3_Select_PP
print BPHY3_Select_PP
#--------------------------------------------------------------------


#--------------------------------------------------------------------
## 5/ select the event. We only want to keep events that contain certain vertices which passed certain selection.
##    This is specified by the "SelectionExpression" property, which contains the expression in the following format:
##
##       "ContainerName.passed_HypoName > count"
##
##    where "ContainerName" is output container form some Reco_* tool, "HypoName" is the hypothesis name setup in some "Select_*"
##    tool and "count" is the number of candidates passing the selection you want to keep. 

expression = "count(BPHY3VertexCandidates.passed_PiPi) > 0 || count(BPHY3VertexCandidates.passed_KPi) > 0 || count(BPHY3VertexCandidates.passed_PiK) > 0 || count(BPHY3VertexCandidates.passed_KK) > 0 || count(BPHY3VertexCandidates.passed_PP) > 0"

from DerivationFrameworkTools.DerivationFrameworkToolsConf import DerivationFramework__xAODStringSkimmingTool
BPHY3_SelectEvent = DerivationFramework__xAODStringSkimmingTool(name = "BPHY3_SelectEvent",
                                                                expression = expression)
ToolSvc += BPHY3_SelectEvent
print BPHY3_SelectEvent

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
BPHY3Thin_vtxTrk = DerivationFramework__Thin_vtxTrk(
  name                       = "BPHY3Thin_vtxTrk",
  ThinningService            = "BPHY3ThinningSvc",
  TrackParticleContainerName = "InDetTrackParticles",
  VertexContainerNames       = ["BPHY3VertexCandidates"],
  PassFlags                  = ["passed_PiPi","passed_KPi","passed_PiK","passed_KK","passed_PP"])

ToolSvc += BPHY3Thin_vtxTrk



# Added by ASC
# Only save truth informtion directly associated with Onia
#from DerivationFrameworkMCTruth.DerivationFrameworkMCTruthConf import DerivationFramework__GenericTruthThinning
#BPHY1TruthThinTool = DerivationFramework__GenericTruthThinning(name                    = "BPHY1TruthThinTool",
#                                                        ThinningService         = "BPHY1ThinningSvc",
#                                                        ParticleSelectionString = "TruthParticles.pdgId == 443 || TruthParticles.pdgId == 100443",
#                                                        PreserveDescendants     = True,
#                                                        PreserveAncestors      = True)
#ToolSvc += BPHY1TruthThinTool
#print BPHY1TruthThinTool


#====================================================================
# CREATE THE DERIVATION KERNEL ALGORITHM AND PASS THE ABOVE TOOLS  
#====================================================================
## 7/ IMPORTANT bit. Don't forget to pass the tools to the DerivationKernel! If you don't do that, they will not be 
##    be executed!

# Added by ASC
BPHY3ThinningTools = [BPHY3Thin_vtxTrk]
#if globalflags.DataSource()=='geant4':
#    BPHY3ThinningTools.append(BPHY3TruthThinTool)

# The name of the kernel (BPHY1Kernel in this case) must be unique to this derivation
from DerivationFrameworkCore.DerivationFrameworkCoreConf import DerivationFramework__DerivationKernel
DerivationFrameworkJob += CfgMgr.DerivationFramework__DerivationKernel(
  "BPHY3Kernel",
   AugmentationTools = [BPHY3_Reco_diTrk,BPHY3_Select_PiPi,BPHY3_Select_KPi,BPHY3_Select_PiK,BPHY3_Select_KK,BPHY3_Select_PP],
   SkimmingTools     = [BPHY3_SelectEvent],
   ThinningTools     = BPHY3ThinningTools

   )

#====================================================================
# SET UP STREAM   
#====================================================================
streamName = derivationFlags.WriteDAOD_BPHY3Stream.StreamName
fileName   = buildFileName( derivationFlags.WriteDAOD_BPHY3Stream )
BPHY3Stream = MSMgr.NewPoolRootStream( streamName, fileName )
BPHY3Stream.AcceptAlgs(["BPHY3Kernel"])
# Special lines for thinning
# Thinning service name must match the one passed to the thinning tools
from AthenaServices.Configurables import ThinningSvc, createThinningSvc
augStream = MSMgr.GetStream( streamName )
evtStream = augStream.GetEventStream()
svcMgr += createThinningSvc( svcName="BPHY3ThinningSvc", outStreams=[evtStream] )


#====================================================================
# Slimming 
#====================================================================

# Added by ASC
from DerivationFrameworkCore.SlimmingHelper import SlimmingHelper
BPHY3SlimmingHelper = SlimmingHelper("BPHY3SlimmingHelper")
AllVariables = []
StaticContent = []

# Needed for trigger objects
#BPHY3SlimmingHelper.IncludeMuonTriggerContent = True
#BPHY3SlimmingHelper.IncludeBPhysTriggerContent = True

## primary vertices
AllVariables += ["PrimaryVertices"]
#StaticContent += ["xAOD::VertexContainer#BPHY3RefittedPrimaryVertices"]
#StaticContent += ["xAOD::VertexAuxContainer#BPHY3RefittedPrimaryVerticesAux."]

## ID track particles
AllVariables += ["InDetTrackParticles"]

## Vertex candidates 
StaticContent += ["xAOD::VertexContainer#%s"        % BPHY3_Reco_diTrk.OutputVtxContainerName]
StaticContent += ["xAOD::VertexAuxContainer#%sAux." % BPHY3_Reco_diTrk.OutputVtxContainerName]
## we have to disable vxTrackAtVertex branch since it is not xAOD compatible
StaticContent += ["xAOD::VertexAuxContainer#%sAux.-vxTrackAtVertex" % BPHY3_Reco_diTrk.OutputVtxContainerName]

# Added by ASC
# Truth information for MC only
#if isSimulation:
#    AllVariables += ["TruthEvents","TruthParticles","TruthVertices","MuonTruthParticles"]

BPHY3SlimmingHelper.AllVariables = AllVariables
BPHY3SlimmingHelper.StaticContent = StaticContent
BPHY3SlimmingHelper.AppendContentToStream(BPHY3Stream)
