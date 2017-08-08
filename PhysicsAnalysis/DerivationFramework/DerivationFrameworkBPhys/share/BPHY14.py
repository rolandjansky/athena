#====================================================================
# BPHY14.py
# This an example job options script showing how to set up a 
# derivation of the data using the derivation framework.  
# It requires the reductionConf flag BPHY14 in Reco_tf.py   
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
include("DerivationFrameworkBPhys/configureVertexing.py")
BPHY14_VertexTools = BPHYVertexTools("BPHY14")

#--------------------------------------------------------------------
## 2/ Setup the vertex fitter tools (e.g. JpsiFinder, JpsiPlus1Track, etc).
##    These are general tools independent of DerivationFramework that do the 
##    actual vertex fitting and some pre-selection.
from JpsiUpsilonTools.JpsiUpsilonToolsConf import Analysis__JpsiFinder
BPHY14JpsiFinder = Analysis__JpsiFinder(
    name                        = "BPHY14JpsiFinder",
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
    V0VertexFitterTool          = BPHY14_VertexTools.TrkV0Fitter, # V0 vertex fitter
    useV0Fitter                 = False, # if False a TrkVertexFitterTool will be used
    TrkVertexFitterTool         = BPHY14_VertexTools.TrkVKalVrtFitter, # VKalVrt vertex fitter
    TrackSelectorTool           = BPHY14_VertexTools.InDetTrackSelectorTool,
    ConversionFinderHelperTool  = BPHY14_VertexTools.InDetConversionHelper,
    VertexPointEstimator        = BPHY14_VertexTools.VtxPointEstimator,
    useMCPCuts                  = False )

ToolSvc += BPHY14JpsiFinder
print      BPHY14JpsiFinder

#--------------------------------------------------------------------
## 3/ setup the vertex reconstruction "call" tool(s). They are part of the derivation framework.
##    These Augmentation tools add output vertex collection(s) into the StoreGate and add basic 
##    decorations which do not depend on the vertex mass hypothesis (e.g. lxy, ptError, etc).
##    There should be one tool per topology, i.e. Jpsi and Psi(2S) do not need two instance of the
##    Reco tool is the JpsiFinder mass window is wide enough.

from DerivationFrameworkBPhys.DerivationFrameworkBPhysConf import DerivationFramework__Reco_mumu
BPHY14_Reco_mumu = DerivationFramework__Reco_mumu(
  name                   = "BPHY14_Reco_mumu",
  JpsiFinder             = BPHY14JpsiFinder,
  OutputVtxContainerName = "BPHY14OniaCandidates",
  PVContainerName        = "PrimaryVertices",
  RefPVContainerName     = "BPHY14RefittedPrimaryVertices",
  RefitPV                = True,
  MaxPVrefit             = 100000,
  DoVertexType           = 7)
  
ToolSvc += BPHY14_Reco_mumu
print BPHY14_Reco_mumu

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
BPHY14_Select_B2mumu = DerivationFramework__Select_onia2mumu(
  name                  = "BPHY14_Select_B2mumu",
  HypothesisName        = "Bmumu",
  InputVtxContainerName = "BPHY14OniaCandidates",
  VtxMassHypo           = 5366.79,
  MassMin               = 4000.0,
  MassMax               = 7000.0,
  Chi2Max               = 200,
  DoVertexType          = 7)
  
ToolSvc += BPHY14_Select_B2mumu
print BPHY14_Select_B2mumu

#--------------------------------------------------------------------
## 5/ select the event. We only want to keep events that contain certain vertices which passed certain selection.
##    This is specified by the "SelectionExpression" property, which contains the expression in the following format:
##
##       "ContainerName.passed_HypoName > count"
##
##    where "ContainerName" is output container form some Reco_* tool, "HypoName" is the hypothesis name setup in some "Select_*"
##    tool and "count" is the number of candidates passing the selection you want to keep. 

expression = "count(BPHY14OniaCandidates.passed_Bmumu) > 0"
from DerivationFrameworkTools.DerivationFrameworkToolsConf import DerivationFramework__xAODStringSkimmingTool
BPHY14_SelectEvent = DerivationFramework__xAODStringSkimmingTool(name = "BPHY14_SelectEvent",
                                                                expression = expression)
ToolSvc += BPHY14_SelectEvent
print BPHY14_SelectEvent

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
BPHY14Thin_vtxTrk = DerivationFramework__Thin_vtxTrk(
  name                       = "BPHY14Thin_vtxTrk",
  ThinningService            = "BPHY14ThinningSvc",
  TrackParticleContainerName = "InDetTrackParticles",
  VertexContainerNames       = ["BPHY14OniaCandidates"],
  PassFlags                  = ["passed_Bmumu"] )

ToolSvc += BPHY14Thin_vtxTrk

## b) thinning out tracks that are not attached to muons. The final thinning decision is based on the OR operation
##    between decision from this and the previous tools.
from DerivationFrameworkInDet.DerivationFrameworkInDetConf import DerivationFramework__MuonTrackParticleThinning
BPHY14MuonTPThinningTool = DerivationFramework__MuonTrackParticleThinning(name                    = "BPHY14MuonTPThinningTool",
                                                                         ThinningService         = "BPHY14ThinningSvc",
                                                                         MuonKey                 = "Muons",
                                                                         InDetTrackParticlesKey  = "InDetTrackParticles")
ToolSvc += BPHY14MuonTPThinningTool

# Added by ASC
# Only save truth informtion directly associated with Onia
from DerivationFrameworkMCTruth.DerivationFrameworkMCTruthConf import DerivationFramework__GenericTruthThinning
BPHY14TruthThinTool = DerivationFramework__GenericTruthThinning(name                    = "BPHY14TruthThinTool",
                                                        ThinningService         = "BPHY14ThinningSvc",
                                                        ParticleSelectionString = "TruthParticles.pdgId == 511 || TruthParticles.pdgId == -511 || TruthParticles.pdgId == 531 || TruthParticles.pdgId == -531",
                                                        PreserveDescendants     = True,
                                                        PreserveAncestors      = True)
ToolSvc += BPHY14TruthThinTool
print BPHY14TruthThinTool


#====================================================================
# CREATE THE DERIVATION KERNEL ALGORITHM AND PASS THE ABOVE TOOLS  
#====================================================================
## 7/ IMPORTANT bit. Don't forget to pass the tools to the DerivationKernel! If you don't do that, they will not be 
##    be executed!

# Added by ASC
BPHY14ThinningTools = [BPHY14Thin_vtxTrk, BPHY14MuonTPThinningTool]
if globalflags.DataSource()=='geant4':
    BPHY14ThinningTools.append(BPHY14TruthThinTool)

# The name of the kernel (BPHY14Kernel in this case) must be unique to this derivation
from DerivationFrameworkCore.DerivationFrameworkCoreConf import DerivationFramework__DerivationKernel
DerivationFrameworkJob += CfgMgr.DerivationFramework__DerivationKernel(
  "BPHY14Kernel",
   AugmentationTools = [BPHY14_Reco_mumu, BPHY14_Select_B2mumu],
   SkimmingTools     = [BPHY14_SelectEvent],
   ThinningTools     = BPHY14ThinningTools
   )

#====================================================================
# SET UP STREAM   
#====================================================================
streamName = derivationFlags.WriteDAOD_BPHY14Stream.StreamName
fileName   = buildFileName( derivationFlags.WriteDAOD_BPHY14Stream )
BPHY14Stream = MSMgr.NewPoolRootStream( streamName, fileName )
BPHY14Stream.AcceptAlgs(["BPHY14Kernel"])
# Special lines for thinning
# Thinning service name must match the one passed to the thinning tools
from AthenaServices.Configurables import ThinningSvc, createThinningSvc
augStream = MSMgr.GetStream( streamName )
evtStream = augStream.GetEventStream()
svcMgr += createThinningSvc( svcName="BPHY14ThinningSvc", outStreams=[evtStream] )


#====================================================================
# Slimming 
#====================================================================

# Added by ASC
from DerivationFrameworkCore.SlimmingHelper import SlimmingHelper
BPHY14SlimmingHelper = SlimmingHelper("BPHY14SlimmingHelper")
AllVariables = []
StaticContent = []

# Needed for trigger objects
BPHY14SlimmingHelper.IncludeMuonTriggerContent = True
BPHY14SlimmingHelper.IncludeBPhysTriggerContent = True

## primary vertices
AllVariables += ["PrimaryVertices"]
StaticContent += ["xAOD::VertexContainer#BPHY14RefittedPrimaryVertices"]
StaticContent += ["xAOD::VertexAuxContainer#BPHY14RefittedPrimaryVerticesAux."]

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
StaticContent += ["xAOD::VertexContainer#%s"        % BPHY14_Reco_mumu.OutputVtxContainerName]
StaticContent += ["xAOD::VertexAuxContainer#%sAux." % BPHY14_Reco_mumu.OutputVtxContainerName]
## we have to disable vxTrackAtVertex branch since it is not xAOD compatible
StaticContent += ["xAOD::VertexAuxContainer#%sAux.-vxTrackAtVertex" % BPHY14_Reco_mumu.OutputVtxContainerName]

# Added by ASC
# Truth information for MC only
if isSimulation:
    AllVariables += ["TruthEvents","TruthParticles","TruthVertices","MuonTruthParticles"]

BPHY14SlimmingHelper.AllVariables = AllVariables
BPHY14SlimmingHelper.StaticContent = StaticContent
BPHY14SlimmingHelper.AppendContentToStream(BPHY14Stream)
