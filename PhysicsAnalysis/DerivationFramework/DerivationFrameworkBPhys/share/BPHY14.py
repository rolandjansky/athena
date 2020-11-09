#====================================================================
# BPHY14.py
# This an example job options script showing how to set up a
# derivation of the data using the derivation framework.
# It requires the reductionConf flag BPHY14 in Reco_tf.py
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
                                        invMassUpper                = 15000.0,
                                        invMassLower                = 2000.,
                                        Chi2Cut                     = 200.,
                                        muonThresholdPt             = 2500.,
                                        oppChargesOnly              = True,
                                        atLeastOneComb              = False,
                                        combOnly                    = True,
                                        useCombinedMeasurement      = False, # Only takes effect if combOnly=True
                                        muonCollectionKey           = "Muons",
                                        TrackParticleCollection     = "InDetTrackParticles",
                                        V0VertexFitterTool          = BPHY14_VertexTools.TrkV0Fitter,             # V0 vertex fitter
                                        useV0Fitter                 = False,                   # if False a TrkVertexFitterTool will be used
                                        TrkVertexFitterTool         = BPHY14_VertexTools.TrkVKalVrtFitter,        # VKalVrt vertex fitter
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
BPHY14_Select_Jpsi2mumu = DerivationFramework__Select_onia2mumu(
                                                                name                  = "BPHY14_Select_Jpsi2mumu",
                                                                HypothesisName        = "Jpsi",
                                                                InputVtxContainerName = "BPHY14OniaCandidates",
                                                                VtxMassHypo           = 3096.916,
                                                                MassMin               = 2000.0,
                                                                MassMax               = 3600.0,
                                                                Chi2Max               = 200,
                                                                DoVertexType          = 7)

ToolSvc += BPHY14_Select_Jpsi2mumu
print BPHY14_Select_Jpsi2mumu

## b/ augment and select Psi(2S)->mumu candidates
BPHY14_Select_Psi2mumu = DerivationFramework__Select_onia2mumu(
                                                               name                  = "BPHY14_Select_Psi2mumu",
                                                               HypothesisName        = "Psi",
                                                               InputVtxContainerName = "BPHY14OniaCandidates",
                                                               VtxMassHypo           = 3686.09,
                                                               MassMin               = 3300.0,
                                                               MassMax               = 4500.0,
                                                               Chi2Max               = 200,
                                                               DoVertexType          = 7)

ToolSvc += BPHY14_Select_Psi2mumu
print BPHY14_Select_Psi2mumu

# Added by ASC
## c/ augment and select Upsilon(nS)->mumu candidates
BPHY14_Select_Upsi2mumu = DerivationFramework__Select_onia2mumu(
                                                                name                  = "BPHY14_Select_Upsi2mumu",
                                                                HypothesisName        = "Upsi",
                                                                InputVtxContainerName = "BPHY14OniaCandidates",
                                                                VtxMassHypo           = 9460.30,
                                                                MassMin               = 7000.0,
                                                                MassMax               = 12500.0,
                                                                Chi2Max               = 200,
                                                                DoVertexType          = 7)

ToolSvc += BPHY14_Select_Upsi2mumu
print BPHY14_Select_Upsi2mumu

#--------------------------------------------------------------------
## 5/ select the event. We only want to keep events that contain certain vertices which passed certain selection.
##    This is specified by the "SelectionExpression" property, which contains the expression in the following format:
##
##       "ContainerName.passed_HypoName > count"
##
##    where "ContainerName" is output container form some Reco_* tool, "HypoName" is the hypothesis name setup in some "Select_*"
##    tool and "count" is the number of candidates passing the selection you want to keep.

#====================================================================
# Photon things
#====================================================================
from DerivationFrameworkCore.DerivationFrameworkMaster import *
from DerivationFrameworkInDet.InDetCommon import *
from DerivationFrameworkMuons.MuonsCommon import *
from DerivationFrameworkJetEtMiss.JetCommon import *
from DerivationFrameworkJetEtMiss.METCommon import *
from DerivationFrameworkEGamma.EGammaCommon import *

#photonRequirements = '(DFCommonPhotons_et >= 5*GeV) && (abs(DFCommonPhotons_eta) < 2.6)'# && (Photons.Loose)'
photonRequirements = 'DFCommonPhotons_et > 5*GeV'


expression = "(count(BPHY14OniaCandidates.passed_Jpsi) > 0 || count(BPHY14OniaCandidates.passed_Psi) > 0 || count(BPHY14OniaCandidates.passed_Upsi) > 0) && count("+photonRequirements+") >0"
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
                                                     PassFlags                  = ["passed_Jpsi", "passed_Psi", "passed_Upsi"] )

ToolSvc += BPHY14Thin_vtxTrk

## b) thinning out tracks that are not attached to muons. The final thinning decision is based on the OR operation
##    between decision from this and the previous tools.
from DerivationFrameworkInDet.DerivationFrameworkInDetConf import DerivationFramework__MuonTrackParticleThinning
BPHY14MuonTPThinningTool = DerivationFramework__MuonTrackParticleThinning(name                    = "BPHY14MuonTPThinningTool",
                                                                          ThinningService         = "BPHY14ThinningSvc",
                                                                          MuonKey                 = "Muons",
                                                                          InDetTrackParticlesKey  = "InDetTrackParticles")
ToolSvc += BPHY14MuonTPThinningTool


# Tracks associated with Photons
from DerivationFrameworkInDet.DerivationFrameworkInDetConf import DerivationFramework__EgammaTrackParticleThinning
BPHY14PhotonTPThinningTool = DerivationFramework__EgammaTrackParticleThinning(name = "BPHY14PhotonTPThinningTool",
                                                                              ThinningService        = "BPHY14ThinningSvc",
                                                                              SGKey                  = "Photons",
                                                                              GSFTrackParticlesKey   = "GSFTrackParticles",
                                                                              InDetTrackParticlesKey = "InDetTrackParticles",
                                                                              SelectionString        = photonRequirements,
                                                                              BestMatchOnly          = False,
                                                                              ConeSize               = 0.6,
                                                                              ApplyAnd               = False)
ToolSvc += BPHY14PhotonTPThinningTool




# Added by ASC
# Only save truth informtion directly associated with Onia
from DerivationFrameworkMCTruth.DerivationFrameworkMCTruthConf import DerivationFramework__GenericTruthThinning
BPHY14TruthThinTool = DerivationFramework__GenericTruthThinning(name                    = "BPHY14TruthThinTool",
                                                                ThinningService         = "BPHY14ThinningSvc",
                                                                ParticleSelectionString = "TruthParticles.pdgId == 22 || TruthParticles.pdgId == 443 || TruthParticles.pdgId == 100443 || TruthParticles.pdgId == 553 || TruthParticles.pdgId == 100553 || TruthParticles.pdgId == 200553",
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
BPHY14ThinningTools = [BPHY14Thin_vtxTrk, BPHY14MuonTPThinningTool,BPHY14PhotonTPThinningTool]
if isSimulation:
    BPHY14ThinningTools.append(BPHY14TruthThinTool)

# The name of the kernel (BPHY14Kernel in this case) must be unique to this derivation
from DerivationFrameworkCore.DerivationFrameworkCoreConf import DerivationFramework__DerivationKernel
DerivationFrameworkJob += CfgMgr.DerivationFramework__DerivationKernel(
                                                                       "BPHY14Kernel",
                                                                       AugmentationTools = [BPHY14_Reco_mumu, BPHY14_Select_Jpsi2mumu, BPHY14_Select_Psi2mumu, BPHY14_Select_Upsi2mumu],
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
BPHY14_AllVariables     = []
BPHY14_StaticContent    = []
BPHY14_SmartCollections = []
BPHY14_ExtraVariables   = []

# Needed for trigger objects
BPHY14SlimmingHelper.IncludeMuonTriggerContent   = True
BPHY14SlimmingHelper.IncludeBPhysTriggerContent  = True
BPHY14SlimmingHelper.IncludeEGammaTriggerContent = True

## primary vertices
BPHY14_SmartCollections  += ["PrimaryVertices"]
BPHY14_StaticContent += ["xAOD::VertexContainer#BPHY14RefittedPrimaryVertices"]
BPHY14_StaticContent += ["xAOD::VertexAuxContainer#BPHY14RefittedPrimaryVerticesAux."]

## ID track particles
BPHY14_SmartCollections += ["InDetTrackParticles"]
BPHY14_ExtraVariables += ["%s.vx.vy.vz" %  "InDetTrackParticles"]
#BPHY14_AllVariables += ["InDetTrackParticles"]


## combined / extrapolated muon track particles
## (note: for tagged muons there is no extra TrackParticle collection since the ID tracks
##        are store in InDetTrackParticles collection)
BPHY14_AllVariables += ["CombinedMuonTrackParticles"]
BPHY14_AllVariables += ["ExtrapolatedMuonTrackParticles"]

## muon container
BPHY14_SmartCollections += ["Muons"]
BPHY14_ExtraVariables   += ["%s.etcone30.etcone40" %  "Muons"
                            +".momentumBalanceSignificance"
                            +".scatteringCurvatureSignificance"
                            +".scatteringNeighbourSignificance"
                            +".msInnerMatchDOF.msInnerMatchChi2"
                            +".msOuterMatchDOF.msOuterMatchChi2"
                            +".EnergyLoss.ParamEnergyLoss.MeasEnergyLoss"
                            +".ET_Core" ]
#BPHY14_AllVariables += ["Muons"]

## Jpsi candidates
BPHY14_StaticContent += ["xAOD::VertexContainer#%s"        % BPHY14_Reco_mumu.OutputVtxContainerName]
BPHY14_StaticContent += ["xAOD::VertexAuxContainer#%sAux." % BPHY14_Reco_mumu.OutputVtxContainerName]
## we have to disable vxTrackAtVertex branch since it is not xAOD compatible
BPHY14_StaticContent += ["xAOD::VertexAuxContainer#%sAux.-vxTrackAtVertex" % BPHY14_Reco_mumu.OutputVtxContainerName]

# Truth information for MC only
if isSimulation:
    BPHY14_StaticContent += ["xAOD::TruthParticleContainer#TruthMuons","xAOD::TruthParticleAuxContainer#TruthMuonsAux."]
    BPHY14_StaticContent += ["xAOD::TruthParticleContainer#TruthPhotons","xAOD::TruthParticleAuxContainer#TruthPhotonsAux."]
    BPHY14_AllVariables  += ["TruthEvents","TruthParticles","TruthVertices","MuonTruthParticles"]

#Photon Information
#AllVariables     += ["Photons"]
BPHY14_SmartCollections += ["Photons"] #,"Muons","InDetTrackParticles","PrimaryVertices"]
from DerivationFrameworkSM.STDMExtraContent import *
BPHY14_ExtraVariables.extend(ExtraContentPhotons)


BPHY14SlimmingHelper.AllVariables     = BPHY14_AllVariables
BPHY14SlimmingHelper.StaticContent    = BPHY14_StaticContent
BPHY14SlimmingHelper.SmartCollections = BPHY14_SmartCollections
BPHY14SlimmingHelper.ExtraVariables   = BPHY14_ExtraVariables
BPHY14SlimmingHelper.AppendContentToStream(BPHY14Stream)

