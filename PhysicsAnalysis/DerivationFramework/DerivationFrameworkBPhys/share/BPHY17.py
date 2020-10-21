#====================================================================
# BPHY17.py
# This an example job options script showing how to set up a
# derivation of the data using the derivation framework.
# It requires the reductionConf flag BPHY17 in Reco_tf.py
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
BPHY17_VertexTools = BPHYVertexTools("BPHY17")

from InDetTrackSelectorTool.InDetTrackSelectorToolConf import InDet__InDetDetailedTrackSelectorTool
InDetTrackSelectorTool = InDet__InDetDetailedTrackSelectorTool(name = "BPHY17_CascadeTrackSelectorTool",
        pTMin                = 1000.0,
        IPd0Max              = 10000.0,
        IPz0Max              = 10000.0,
        z0Max                = 10000.0,
        sigIPd0Max           = 10000.0,
        sigIPz0Max           = 10000.0,
        d0significanceMax    = -1.,
        z0significanceMax    = -1.,
        etaMax               = 9999.,
        useTrackSummaryInfo  = True,
        nHitBLayer           = 0,
        nHitPix              = 1,
        nHitBLayerPlusPix    = 1,
        nHitSct              = 2,
        nHitSi               = 3,
        nHitTrt              = 0,
        nHitTrtHighEFractionMax = 10000.0,
        useSharedHitInfo     = False,
        useTrackQualityInfo  = True,
        fitChi2OnNdfMax      = 10000.0,
        TrtMaxEtaAcceptance  = 1.9,
        TrackSummaryTool     = BPHY17_VertexTools.InDetTrackSummaryTool,
        Extrapolator         = BPHY17_VertexTools.InDetExtrapolator
       )

ToolSvc += InDetTrackSelectorTool
print InDetTrackSelectorTool

#BPHY17_VertexTools.TrkVKalVrtFitter.OutputLevel = DEBUG

from DerivationFrameworkBPhys.DerivationFrameworkBPhysConf import DerivationFramework__Cascade3Plus1
BPHY17BsDsPi            = DerivationFramework__Cascade3Plus1(
    name                    = "BPHY17BsDsPi",
    TrackMassHyp            = [ 493.677, 493.677, 139.57018, 139.57018 ],
    PTCutPerTrack           = [ 0,0,1500, 1500],
    HypothesisName          = "Bs",
    Track3Name              = "Ds",
    TrackSelectorTool       = InDetTrackSelectorTool,
    TrkVertexFitterTool     = BPHY17_VertexTools.TrkVKalVrtFitter,
    PVContainerName          = "PrimaryVertices",
    RefitPV                 = True,
    #OutputLevel             = DEBUG,
    RefPVContainerName      = "BPHY17RefittedPrimaryVertices",
    CascadeVertexCollections= ["BPHY17CascadeVtx1", "BPHY17CascadeVtx2"],
    ThreeVertexOutputContainer = "BPHY17DsKaonVertexes", EliminateBad3Tracksfrom4Track = True,
    ThreeTrackChi2NDF = 6,
    TwoTrackMassMin	= 1009.0,
    TwoTrackMassMax	= 1031.0,
    ThreeTrackMassMin = 1800.47,
    ThreeTrackMassMax = 2100.0,
    FourTrackMassMin = 5100.0,
    FourTrackMassMax = 5600.0,
    ThreeTracksMass	= 1968.47,
    FourTracksMass	= 5366.79,
    Chi2NDFCut     = 10,
    FourTrackMassFinalMin = 5150.,
    FourTrackMassFinalMax = 5650.0,
    ThreeTrackMassConstraint = False,
    CopyAllVertices  = False
    )

ToolSvc += BPHY17BsDsPi
print BPHY17BsDsPi

###
BPHY17BsDsPiMuons            = DerivationFramework__Cascade3Plus1(
    name                    = "BPHY17BsDsPiMuons",
    TrackMassHyp            = [ 105.658374, 105.658374, 139.57018, 139.57018 ],
    PTCutPerTrack           = [ 0,0,1500, 1500],
    HypothesisName          = "Bs",
    Track3Name              = "Ds",
    TrackSelectorTool       = InDetTrackSelectorTool,
    TrkVertexFitterTool     = BPHY17_VertexTools.TrkVKalVrtFitter,
    PVContainerName          = "PrimaryVertices",
    RefitPV                 = True,
    #OutputLevel             = DEBUG,
    RefPVContainerName      = "BPHY17RefittedPrimaryVerticesMuons",
    CascadeVertexCollections= ["BPHY17CascadeMuonVtx1", "BPHY17CascadeMuonVtx2"],
    ThreeVertexOutputContainer = "BPHY17DsMuonVertexes", EliminateBad3Tracksfrom4Track = True,
    ThreeTrackChi2NDF = 10,
    TwoTrackMassMin     =  860.0,
    TwoTrackMassMax     = 1180.0,
    ThreeTrackMassMin = 1800.47,
    ThreeTrackMassMax = 2100.0,
    FourTrackMassMin = 5100.0,
    FourTrackMassMax = 5550.0,
    ThreeTracksMass	= 1968.47,
    FourTracksMass	= 5366.79,
    Chi2NDFCut     = 10,
    FourTrackMassFinalMin = 5150.,
    FourTrackMassFinalMax = 5500.0,
    ThreeTrackMassConstraint = False,
    UseMuonsForTracks = [0, 1],
    CopyAllVertices  = False
    )

ToolSvc += BPHY17BsDsPiMuons
print BPHY17BsDsPiMuons

###
BPHY17BsDsMuSemiLepMuons            = DerivationFramework__Cascade3Plus1(
    name                    = "BPHY17BsDsMuSemiLepMuons",
    TrackMassHyp            = [ 105.658374, 105.658374, 139.57018, 105.658374 ],
    PTCutPerTrack           = [ 0,0,1500, 0],
    HypothesisName          = "Bs",
    Track3Name              = "Ds",
    TrackSelectorTool       = InDetTrackSelectorTool,
    TrkVertexFitterTool     = BPHY17_VertexTools.TrkVKalVrtFitter,
    PVContainerName          = "PrimaryVertices",
    RefitPV                 = True,
    #OutputLevel             = DEBUG,
    RefPVContainerName      = "BPHY17RefittedPrimaryVerticesMuonsSemiLep",
    CascadeVertexCollections= ["BPHY17CascadeMuonSemiLepVtx1", "BPHY17CascadeMuonSemiLepVtx2"],
    ThreeVertexOutputContainer = "BPHY17DsMuonSemiLepVertexes", EliminateBad3Tracksfrom4Track = True,
    ThreeTrackChi2NDF = 10,
    TwoTrackMassMin     =  860.0,
    TwoTrackMassMax     = 1180.0,
    ThreeTrackMassMin = 1800.47,
    ThreeTrackMassMax = 2100.0,
    FourTrackMassMin = 0.,
    FourTrackMassMax = 999999.0,
    ThreeTracksMass	= 1968.47,
    FourTracksMass	= 5366.79,
    Chi2NDFCut     = 10,
    FourTrackMassFinalMin = 0,
    FourTrackMassFinalMax = 999999.0,
    ThreeTrackMassConstraint = False,
    UseMuonsForTracks = [0, 1, 3],
    CopyAllVertices  = False
    )

ToolSvc += BPHY17BsDsMuSemiLepMuons
print BPHY17BsDsMuSemiLepMuons


from DerivationFrameworkBPhys.DerivationFrameworkBPhysConf import DerivationFramework__Select_onia2mumu
BPHY17_Select_DsPhiKK = DerivationFramework__Select_onia2mumu(
  name                       = "BPHY17_Select_DsPhiKK",
  HypothesisName             = "DsPhiKK",
  InputVtxContainerName      = BPHY17BsDsPi.ThreeVertexOutputContainer,
  TrkMasses                  = [493.677, 493.677, 139.57018],
  VtxMassHypo                = 1968.47,
  MassMin                    = 1000.0,
  MassMax                    = 3000.0, Do3d = False, DoVertexType = 1,
  Chi2Max                    = 200)

ToolSvc += BPHY17_Select_DsPhiKK
print      BPHY17_Select_DsPhiKK

BPHY17_Select_DsPhiMM = DerivationFramework__Select_onia2mumu(
  name                       = "BPHY17_Select_DsPhiMM",
  HypothesisName             = "DsPhiMM",
  InputVtxContainerName      = BPHY17BsDsPiMuons.ThreeVertexOutputContainer,
  TrkMasses                  = [105.658374, 105.658374, 139.57018],
  VtxMassHypo                = 1968.47,
  MassMin                    = 1000.0,
  MassMax                    = 3000.0, Do3d = False, DoVertexType = 1,
  Chi2Max                    = 200)

ToolSvc += BPHY17_Select_DsPhiMM
print      BPHY17_Select_DsPhiMM


BPHY17_Select_DsPhiMMSemi = DerivationFramework__Select_onia2mumu(
  name                       = "BPHY17_Select_DsPhiMMSemi",
  HypothesisName             = "DsPhiMMSemiLep",
  InputVtxContainerName      = BPHY17BsDsMuSemiLepMuons.ThreeVertexOutputContainer,
  TrkMasses                  = [105.658374, 105.658374, 139.57018],
  VtxMassHypo                = 1968.47,
  MassMin                    = 1000.0,
  MassMax                    = 3000.0, Do3d = False, DoVertexType = 1,
  Chi2Max                    = 200)

ToolSvc += BPHY17_Select_DsPhiMMSemi
print      BPHY17_Select_DsPhiMMSemi

###

#--------------------------------------------------------------------
## 5/ select the event. We only want to keep events that contain certain vertices which passed certain selection.
##    This is specified by the "SelectionExpression" property, which contains the expression in the following format:
##
##       "ContainerName.passed_HypoName > count"
##
##    where "ContainerName" is output container form some Reco_* tool, "HypoName" is the hypothesis name setup in some "Select_*"
##    tool and "count" is the number of candidates passing the selection you want to keep.

expression = "count(%s.x > -999) > 0" % BPHY17BsDsPi.CascadeVertexCollections[0]
expression += " || count(%s.x > -999) > 0" % BPHY17BsDsPiMuons.CascadeVertexCollections[0]
expression += " || count(%s.x > -999) > 0" % BPHY17BsDsPi.ThreeVertexOutputContainer
expression += " || count(%s.x > -999) > 0" % BPHY17BsDsPiMuons.ThreeVertexOutputContainer
expression += " || count(%s.x > -999) > 0" % BPHY17BsDsMuSemiLepMuons.CascadeVertexCollections[0]
expression += " || count(%s.x > -999) > 0" % BPHY17BsDsMuSemiLepMuons.ThreeVertexOutputContainer

from DerivationFrameworkTools.DerivationFrameworkToolsConf import DerivationFramework__xAODStringSkimmingTool
BPHY17_SelectEvent = DerivationFramework__xAODStringSkimmingTool(name = "BPHY17_SelectEvent",
                                                                expression = expression)
ToolSvc += BPHY17_SelectEvent
print BPHY17_SelectEvent

MyVertexCollections = BPHY17BsDsPi.CascadeVertexCollections + BPHY17BsDsPiMuons.CascadeVertexCollections + BPHY17BsDsMuSemiLepMuons.CascadeVertexCollections + \
                 [ BPHY17BsDsPi.ThreeVertexOutputContainer, BPHY17BsDsPiMuons.ThreeVertexOutputContainer, BPHY17BsDsMuSemiLepMuons.ThreeVertexOutputContainer ]

from DerivationFrameworkBPhys.DerivationFrameworkBPhysConf import DerivationFramework__Thin_vtxTrk
BPHY17_thinningTool_Tracks = DerivationFramework__Thin_vtxTrk(
  name                       = "BPHY17_thinningTool_Tracks",
  ThinningService            = "BPHY17ThinningSvc",
  TrackParticleContainerName = "InDetTrackParticles",
  IgnoreFlags = True,
  VertexContainerNames       = MyVertexCollections,
  PassFlags                  = ["passed_DsPhiMM", "passed_DsPhiKK"] )

ToolSvc += BPHY17_thinningTool_Tracks


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
BPHY17ThinningTools = [BPHY17_thinningTool_Tracks]
if isSimulation:
    BPHY17ThinningTools.append(BPHY17TruthThinTool)

# The name of the kernel (BPHY17Kernel in this case) must be unique to this derivation
from DerivationFrameworkCore.DerivationFrameworkCoreConf import DerivationFramework__DerivationKernel
DerivationFrameworkJob += CfgMgr.DerivationFramework__DerivationKernel(
  "BPHY17Kernel",
   AugmentationTools = [BPHY17BsDsPi, BPHY17BsDsPiMuons, BPHY17BsDsMuSemiLepMuons, BPHY17_Select_DsPhiKK, BPHY17_Select_DsPhiMM, BPHY17_Select_DsPhiMMSemi],
   SkimmingTools     = [BPHY17_SelectEvent],
   ThinningTools     = BPHY17ThinningTools,
#   OutputLevel             = DEBUG
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
BPHY17SlimmingHelper.IncludeBPhysTriggerContent = False
BPHY17SlimmingHelper.IncludeMuonTriggerContent = False
## primary vertices
AllVariables += ["PrimaryVertices"]

for f in MyVertexCollections + [BPHY17BsDsPi.RefPVContainerName, BPHY17BsDsPiMuons.RefPVContainerName + BPHY17BsDsMuSemiLepMuons.RefPVContainerName ]:
   StaticContent += ["xAOD::VertexContainer#%s"        %                 f]
   StaticContent += ["xAOD::VertexAuxContainer#%sAux.-vxTrackAtVertex" % f]

## ID track particles
AllVariables += ["InDetTrackParticles"]
AllVariables += ["Muons"]

# Truth information for MC only
if isSimulation:
    AllVariables += ["TruthEvents","TruthParticles","TruthVertices"]

BPHY17SlimmingHelper.AllVariables = AllVariables
BPHY17SlimmingHelper.StaticContent = StaticContent
BPHY17SlimmingHelper.AppendContentToStream(BPHY17Stream)
