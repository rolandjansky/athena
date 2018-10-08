#====================================================================
# BPHY17.py
# This an example job options script showing how to set up a 
# derivation of the data using the derivation framework.  
# It requires the reductionConf flag BPHY17 in Reco_tf.py   
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
BPHY17_VertexTools = BPHYVertexTools("BPHY17")

#--------------------------------------------------------------------
## 2/ Setup the vertex fitter tools (e.g. JpsiFinder, JpsiPlus1Track, etc).
##    These are general tools independent of DerivationFramework that do the 
##    actual vertex fitting and some pre-selection.
from JpsiUpsilonTools.JpsiUpsilonToolsConf import Analysis__JpsiFinder
BPHY17JpsiFinder = Analysis__JpsiFinder(
    name                        = "BPHY17JpsiFinder",
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
    V0VertexFitterTool          = BPHY17_VertexTools.TrkV0Fitter, # V0 vertex fitter
    useV0Fitter                 = False, # if False a TrkVertexFitterTool will be used
    TrkVertexFitterTool         = BPHY17_VertexTools.TrkVKalVrtFitter, # VKalVrt vertex fitter
    TrackSelectorTool           = BPHY17_VertexTools.InDetTrackSelectorTool,
    ConversionFinderHelperTool  = BPHY17_VertexTools.InDetConversionHelper,
    VertexPointEstimator        = BPHY17_VertexTools.VtxPointEstimator,
    useMCPCuts                  = False )

ToolSvc += BPHY17JpsiFinder
print      BPHY17JpsiFinder

#--------------------------------------------------------------------
## 3/ setup the vertex reconstruction "call" tool(s). They are part of the derivation framework.
##    These Augmentation tools add output vertex collection(s) into the StoreGate and add basic 
##    decorations which do not depend on the vertex mass hypothesis (e.g. lxy, ptError, etc).
##    There should be one tool per topology, i.e. Jpsi and Psi(2S) do not need two instance of the
##    Reco tool is the JpsiFinder mass window is wide enough.

from DerivationFrameworkBPhys.DerivationFrameworkBPhysConf import DerivationFramework__Reco_mumu
BPHY17_Reco_mumu = DerivationFramework__Reco_mumu(
  name                   = "BPHY17_Reco_mumu",
  JpsiFinder             = BPHY17JpsiFinder,
  OutputVtxContainerName = "BPHY17OniaCandidates",
  PVContainerName        = "PrimaryVertices",
  RefPVContainerName     = "BPHY17RefittedPrimaryVertices",
  RefitPV                = True,
  MaxPVrefit             = 100000,
  DoVertexType           = 7)
  
ToolSvc += BPHY17_Reco_mumu
print BPHY17_Reco_mumu

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
BPHY17_Select_B2mumu = DerivationFramework__Select_onia2mumu(
  name                  = "BPHY17_Select_B2mumu",
  HypothesisName        = "Bmumu",
  InputVtxContainerName = "BPHY17OniaCandidates",
  VtxMassHypo           = 5366.79,
  MassMin               = 4000.0,
  MassMax               = 7000.0,
  Chi2Max               = 200,
  DoVertexType          = 7)
  
ToolSvc += BPHY17_Select_B2mumu
print BPHY17_Select_B2mumu

#--------------------------------------------------------------------
## 5/ select the event. We only want to keep events that contain certain vertices which passed certain selection.
##    This is specified by the "SelectionExpression" property, which contains the expression in the following format:
##
##       "ContainerName.passed_HypoName > count"
##
##    where "ContainerName" is output container form some Reco_* tool, "HypoName" is the hypothesis name setup in some "Select_*"
##    tool and "count" is the number of candidates passing the selection you want to keep. 

expression = "count(BPHY17OniaCandidates.passed_Bmumu) > 0"
from DerivationFrameworkTools.DerivationFrameworkToolsConf import DerivationFramework__xAODStringSkimmingTool
BPHY17_SelectEvent = DerivationFramework__xAODStringSkimmingTool(name = "BPHY17_SelectEvent",
                                                                expression = expression)
ToolSvc += BPHY17_SelectEvent
print BPHY17_SelectEvent

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
BPHY17Thin_vtxTrk = DerivationFramework__Thin_vtxTrk(
  name                       = "BPHY17Thin_vtxTrk",
  ThinningService            = "BPHY17ThinningSvc",
  TrackParticleContainerName = "InDetTrackParticles",
  VertexContainerNames       = ["BPHY17OniaCandidates"],
  PassFlags                  = ["passed_Bmumu"] )

ToolSvc += BPHY17Thin_vtxTrk

## b) thinning out tracks that are not attached to muons. The final thinning decision is based on the OR operation
##    between decision from this and the previous tools.
from DerivationFrameworkInDet.DerivationFrameworkInDetConf import DerivationFramework__MuonTrackParticleThinning
BPHY17MuonTPThinningTool = DerivationFramework__MuonTrackParticleThinning(name                    = "BPHY17MuonTPThinningTool",
                                                                         ThinningService         = "BPHY17ThinningSvc",
                                                                         MuonKey                 = "Muons",
                                                                         InDetTrackParticlesKey  = "InDetTrackParticles")
ToolSvc += BPHY17MuonTPThinningTool

# Added by ASC
# Only save truth informtion directly associated with Onia
from DerivationFrameworkMCTruth.DerivationFrameworkMCTruthConf import DerivationFramework__GenericTruthThinning
BPHY17TruthThinTool = DerivationFramework__GenericTruthThinning(name                    = "BPHY17TruthThinTool",
                                                        ThinningService         = "BPHY17ThinningSvc",
                                                        ParticleSelectionString = "TruthParticles.pdgId == 511 || TruthParticles.pdgId == -511 || TruthParticles.pdgId == 531 || TruthParticles.pdgId == -531",
                                                        PreserveDescendants     = True,
                                                        PreserveAncestors      = True)
ToolSvc += BPHY17TruthThinTool
print BPHY17TruthThinTool


#====================================================================
# CREATE THE DERIVATION KERNEL ALGORITHM AND PASS THE ABOVE TOOLS  
#====================================================================
## 7/ IMPORTANT bit. Don't forget to pass the tools to the DerivationKernel! If you don't do that, they will not be 
##    be executed!

# Added by ASC
BPHY17ThinningTools = [BPHY17Thin_vtxTrk, BPHY17MuonTPThinningTool]
if globalflags.DataSource()=='geant4':
    BPHY17ThinningTools.append(BPHY17TruthThinTool)

# The name of the kernel (BPHY17Kernel in this case) must be unique to this derivation
from DerivationFrameworkCore.DerivationFrameworkCoreConf import DerivationFramework__DerivationKernel
DerivationFrameworkJob += CfgMgr.DerivationFramework__DerivationKernel(
  "BPHY17Kernel",
   AugmentationTools = [BPHY17_Reco_mumu, BPHY17_Select_B2mumu],
   SkimmingTools     = [BPHY17_SelectEvent],
   ThinningTools     = BPHY17ThinningTools
   )

#====================================================================
# SET UP STREAM   
#====================================================================
streamName = derivationFlags.WriteDAOD_BPHY17Stream.StreamName
fileName   = buildFileName( derivationFlags.WriteDAOD_BPHY17Stream )
BPHY17Stream = MSMgr.NewPoolRootStream( streamName, fileName )
BPHY17Stream.AcceptAlgs(["BPHY17Kernel"])
# Special lines for thinning
# Thinning service name must match the one passed to the thinning tools
from AthenaServices.Configurables import ThinningSvc, createThinningSvc
augStream = MSMgr.GetStream( streamName )
evtStream = augStream.GetEventStream()
svcMgr += createThinningSvc( svcName="BPHY17ThinningSvc", outStreams=[evtStream] )


#====================================================================
# Slimming 
#====================================================================

# Added by ASC
from DerivationFrameworkCore.SlimmingHelper import SlimmingHelper
BPHY17SlimmingHelper = SlimmingHelper("BPHY17SlimmingHelper")
AllVariables = []
StaticContent = []

# Needed for trigger objects
BPHY17SlimmingHelper.IncludeMuonTriggerContent = True
BPHY17SlimmingHelper.IncludeBPhysTriggerContent = True

## primary vertices
AllVariables += ["PrimaryVertices"]
StaticContent += ["xAOD::VertexContainer#BPHY17RefittedPrimaryVertices"]
StaticContent += ["xAOD::VertexAuxContainer#BPHY17RefittedPrimaryVerticesAux."]

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
StaticContent += ["xAOD::VertexContainer#%s"        % BPHY17_Reco_mumu.OutputVtxContainerName]
StaticContent += ["xAOD::VertexAuxContainer#%sAux." % BPHY17_Reco_mumu.OutputVtxContainerName]
## we have to disable vxTrackAtVertex branch since it is not xAOD compatible
StaticContent += ["xAOD::VertexAuxContainer#%sAux.-vxTrackAtVertex" % BPHY17_Reco_mumu.OutputVtxContainerName]

# Added by ASC
# Truth information for MC only
if isSimulation:
    AllVariables += ["TruthEvents","TruthParticles","TruthVertices","MuonTruthParticles"]

BPHY17SlimmingHelper.AllVariables = AllVariables
BPHY17SlimmingHelper.StaticContent = StaticContent
BPHY17SlimmingHelper.AppendContentToStream(BPHY17Stream)
