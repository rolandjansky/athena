#====================================================================
# BPHY2.py
# BPHY2 repurposed for students analysis of Bs->Upsilon phi Analysis
# It requires the reductionConf flag BPHY2 in Reco_tf.py
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

include("DerivationFrameworkBPhys/configureVertexing.py")
BPHY2_VertexTools = BPHYVertexTools("BPHY2")


from DerivationFrameworkBPhys.DerivationFrameworkBPhysConf import DerivationFramework__AugOriginalCounts
BPHY2_AugOriginalCounts = DerivationFramework__AugOriginalCounts(
   name = "BPHY2_AugOriginalCounts",
   VertexContainer = "PrimaryVertices",
   TrackContainer = "InDetTrackParticles" )
ToolSvc += BPHY2_AugOriginalCounts


#--------------------------------------------------------------------
## 2/ setup JpsiFinder tool
##    These are general tools independent of DerivationFramework that do the
##    actual vertex fitting and some pre-selection.
from JpsiUpsilonTools.JpsiUpsilonToolsConf import Analysis__JpsiFinder
BPHY2JpsiFinder = Analysis__JpsiFinder(name                         = "BPHY2JpsiFinder",
                                        OutputLevel                 = INFO,
                                        muAndMu                     = True,
                                        muAndTrack                  = False,
                                        TrackAndTrack               = False,
                                        assumeDiMuons               = True,
                                        invMassUpper                = 4700.0,
                                        invMassLower                = 2600.0,
                                        Chi2Cut                     = 15.,
                                        oppChargesOnly	            = True,
                                        combOnly		            = True,
                                        atLeastOneComb              = False,
                                        useCombinedMeasurement      = False, # Only takes effect if combOnly=True
                                        muonCollectionKey           = "Muons",
                                        TrackParticleCollection     = "InDetTrackParticles",
                                        V0VertexFitterTool          = BPHY2_VertexTools.TrkV0Fitter,             # V0 vertex fitter
                                        useV0Fitter                 = False,                   # if False a TrkVertexFitterTool will be used
                                        TrkVertexFitterTool         = BPHY2_VertexTools.TrkVKalVrtFitter,        # VKalVrt vertex fitter
                                        TrackSelectorTool           = BPHY2_VertexTools.InDetTrackSelectorTool,
                                        ConversionFinderHelperTool  = BPHY2_VertexTools.InDetConversionHelper,
                                        VertexPointEstimator        = BPHY2_VertexTools.VtxPointEstimator,
                                        useMCPCuts                  = False)
ToolSvc += BPHY2JpsiFinder
print      BPHY2JpsiFinder

#--------------------------------------------------------------------
## 3/ setup the vertex reconstruction "call" tool(s). They are part of the derivation framework.
##    These Augmentation tools add output vertex collection(s) into the StoreGate and add basic
##    decorations which do not depend on the vertex mass hypothesis (e.g. lxy, ptError, etc).
##    There should be one tool per topology, i.e. Jpsi and Psi(2S) do not need two instance of the
##    Reco tool is the JpsiFinder mass window is wide enough.
from DerivationFrameworkBPhys.DerivationFrameworkBPhysConf import DerivationFramework__Reco_mumu
BPHY2JpsiSelectAndWrite = DerivationFramework__Reco_mumu(name                 = "BPHY2JpsiSelectAndWrite",
                                                       JpsiFinder             = BPHY2JpsiFinder,
                                                       OutputVtxContainerName = "BPHY2JpsiCandidates",
                                                       PVContainerName        = "PrimaryVertices",
                                                       RefPVContainerName     = "SHOULDNOTBEUSED",
                                                       DoVertexType           =1)
ToolSvc += BPHY2JpsiSelectAndWrite
print BPHY2JpsiSelectAndWrite


from DerivationFrameworkBPhys.DerivationFrameworkBPhysConf import DerivationFramework__Select_onia2mumu



## 4/ setup a new vertexing tool (necessary due to use of mass constraint)
from TrkVKalVrtFitter.TrkVKalVrtFitterConf import Trk__TrkVKalVrtFitter
BPHY2BsKKVertexFit = Trk__TrkVKalVrtFitter(
                                         name                = "BPHY2BsKKVertexFit",
                                         Extrapolator        = BPHY2_VertexTools.InDetExtrapolator,
                                         FirstMeasuredPoint  = True,
                                         MakeExtendedVertex  = True)
ToolSvc += BPHY2BsKKVertexFit
print      BPHY2BsKKVertexFit



## 5/ setup the Jpsi+2 track finder
from JpsiUpsilonTools.JpsiUpsilonToolsConf import Analysis__JpsiPlus2Tracks
BPHY2BsJpsiKK = Analysis__JpsiPlus2Tracks(name = "BPHY2BsJpsiKK",
                                        OutputLevel = INFO,
kaonkaonHypothesis			= True,
pionpionHypothesis                      = False,
kaonpionHypothesis                      = False,
trkThresholdPt			        = 800.0,
trkMaxEta				    = 3.0,
BMassUpper				    = 5800.0,
BMassLower				    = 5000.0,
DiTrackMassUpper = 1019.445 + 100.,
DiTrackMassLower = 1019.445 - 100.,
Chi2Cut                     = 8.0,
TrkQuadrupletMassUpper      = 6000.0,
TrkQuadrupletMassLower      = 4800.0,
JpsiContainerKey		    = "BPHY2JpsiCandidates",
TrackParticleCollection     = "InDetTrackParticles",
MuonsUsedInJpsi			    = "Muons",
TrkVertexFitterTool		    = BPHY2BsKKVertexFit,
TrackSelectorTool		    = BPHY2_VertexTools.InDetTrackSelectorTool,
UseMassConstraint		    = False)

ToolSvc += BPHY2BsJpsiKK
print      BPHY2BsJpsiKK




## 6/ setup the combined augmentation/skimming tool for the Bpm
from DerivationFrameworkBPhys.DerivationFrameworkBPhysConf import DerivationFramework__Reco_dimuTrkTrk
BPHY2BsKKSelectAndWrite = DerivationFramework__Reco_dimuTrkTrk(name                 = "BPHY2BsKKSelectAndWrite",
                                                           Jpsi2PlusTrackName       = BPHY2BsJpsiKK,
                                                           OutputVtxContainerName   = "BsJpsiKKCandidates",
                                                           PVContainerName          = "PrimaryVertices",
                                                           RefPVContainerName       = "BPHY2RefittedPrimaryVertices",
                                                           RefitPV                  = True,
                                                           MaxPVrefit               = 10000, DoVertexType = 7)
ToolSvc += BPHY2BsKKSelectAndWrite
print      BPHY2BsKKSelectAndWrite


## b/ augment and select Psi(2S)->mumu candidates
BPHY2_Select_Psi2mumu = DerivationFramework__Select_onia2mumu(
  name                  = "BPHY2_Select_Psi2mumu",
  HypothesisName        = "Psi",
  InputVtxContainerName = "BPHY2JpsiCandidates",
  VtxMassHypo           = 3686.09,
  MassMin               = 3300.0,
  MassMax               = 4500.0,
  Chi2Max               = 200,
  DoVertexType          = 7)

ToolSvc += BPHY2_Select_Psi2mumu
print BPHY2_Select_Psi2mumu


## a/ augment and select Jpsi->mumu candidates
BPHY2_Select_Jpsi2mumu = DerivationFramework__Select_onia2mumu(
  name                  = "BPHY2_Select_Jpsi2mumu",
  HypothesisName        = "Jpsi",
  InputVtxContainerName = "BPHY2JpsiCandidates",
  VtxMassHypo           = 3096.916,
  MassMin               = 2000.0,
  MassMax               = 3600.0,
  Chi2Max               = 200,
  DoVertexType          = 7)

ToolSvc += BPHY2_Select_Jpsi2mumu
print BPHY2_Select_Jpsi2mumu

## b/ augment and select Bs->JpsiKK candidates
BPHY2_Select_Bs2JpsiKK = DerivationFramework__Select_onia2mumu(
  name                       = "BPHY2_Select_Bs2JpsiKK",
  HypothesisName             = "Bs",
  InputVtxContainerName      = "BsJpsiKKCandidates",
  TrkMasses                  = [105.658, 105.658, 493.677, 493.677],
  VtxMassHypo                = 5366.3,
  MassMin                    = 5000.0,
  MassMax                    = 5800.0,
  Chi2Max                    = 200)

ToolSvc += BPHY2_Select_Bs2JpsiKK
print      BPHY2_Select_Bs2JpsiKK


from DerivationFrameworkBPhys.DerivationFrameworkBPhysConf import DerivationFramework__Thin_vtxTrk
BPHY2_thinningTool_Tracks = DerivationFramework__Thin_vtxTrk(
  name                       = "BPHY2_thinningTool_Tracks",
  ThinningService            = "BPHY2ThinningSvc",
  TrackParticleContainerName = "InDetTrackParticles",
  VertexContainerNames       = ["BsJpsiKKCandidates", "BPHY2JpsiCandidates"],
  PassFlags                  = ["passed_Bs", "passed_Psi", "passed_Jpsi"] )

ToolSvc += BPHY2_thinningTool_Tracks

from DerivationFrameworkBPhys.DerivationFrameworkBPhysConf import DerivationFramework__BPhysPVThinningTool
BPHY2_thinningTool_PV = DerivationFramework__BPhysPVThinningTool(
  name                       = "BPHY2_thinningTool_PV",
  ThinningService            = "BPHY2ThinningSvc",
  CandidateCollections       = ["BsJpsiKKCandidates"],
  KeepPVTracks  =True)

ToolSvc += BPHY2_thinningTool_PV

if not isSimulation: #Only Skim Data
   from DerivationFrameworkTools.DerivationFrameworkToolsConf import DerivationFramework__xAODStringSkimmingTool

   BPHY2_SelectBsJpsiKKEvent = DerivationFramework__xAODStringSkimmingTool(
   name = "BPHY2_SelectBsJpsiKKEvent",
   expression = "count(BsJpsiKKCandidates.passed_Bs > 0) > 0")
   ToolSvc += BPHY2_SelectBsJpsiKKEvent
   print BPHY2_SelectBsJpsiKKEvent


   #====================================================================
   # Make event selection based on an OR of the input skimming tools
   #====================================================================
   from DerivationFrameworkTools.DerivationFrameworkToolsConf import DerivationFramework__FilterCombinationOR
   BPHY2SkimmingOR = CfgMgr.DerivationFramework__FilterCombinationOR("BPHY2SkimmingOR",
                                                                 FilterList = [BPHY2_SelectBsJpsiKKEvent ])#, BPHY2_SelectBplJpsiKplEventBc
   PassFlags = ["passed_Bs"]
   ToolSvc += BPHY2SkimmingOR
   print      BPHY2SkimmingOR



## b) thinning out tracks that are not attached to muons. The final thinning decision is based on the OR operation
##    between decision from this and the previous tools.
from DerivationFrameworkInDet.DerivationFrameworkInDetConf import DerivationFramework__MuonTrackParticleThinning
BPHY2MuonTPThinningTool = DerivationFramework__MuonTrackParticleThinning(name                    = "BPHY2MuonTPThinningTool",
                                                                         ThinningService         = "BPHY2ThinningSvc",
                                                                         MuonKey                 = "Muons",
                                                                         InDetTrackParticlesKey  = "InDetTrackParticles")
ToolSvc += BPHY2MuonTPThinningTool


#====================================================================
# CREATE THE DERIVATION KERNEL ALGORITHM AND PASS THE ABOVE TOOLS
#====================================================================

thiningCollection = [BPHY2_thinningTool_Tracks, BPHY2_thinningTool_PV, BPHY2MuonTPThinningTool]
print thiningCollection

BPHY2Seq = CfgMgr.AthSequencer("BPHY2Sequence")
DerivationFrameworkJob += BPHY2Seq

# The name of the kernel (BPHY2Kernel in this case) must be unique to this derivation
from DerivationFrameworkCore.DerivationFrameworkCoreConf import DerivationFramework__DerivationKernel
BPHY2Seq += CfgMgr.DerivationFramework__DerivationKernel("BPHY2Kernel",
                                                                       AugmentationTools = [ BPHY2JpsiSelectAndWrite,
                                                                                            BPHY2BsKKSelectAndWrite,
                                                                                           BPHY2_Select_Psi2mumu,
                                                                                           BPHY2_Select_Jpsi2mumu,  BPHY2_Select_Bs2JpsiKK,
                                                                                           BPHY2_AugOriginalCounts],
                                                                       #Only skim if not MC
                                                                       SkimmingTools     = [BPHY2SkimmingOR] if not isSimulation else [],
                                                                       ThinningTools     = thiningCollection

                                                                       )

#====================================================================
# SET UP STREAM
#====================================================================
streamName   = derivationFlags.WriteDAOD_BPHY2Stream.StreamName
fileName     = buildFileName( derivationFlags.WriteDAOD_BPHY2Stream )
BPHY2Stream  = MSMgr.NewPoolRootStream( streamName, fileName )
BPHY2Stream.AcceptAlgs(["BPHY2Kernel"])

# Special lines for thinning
# Thinning service name must match the one passed to the thinning tools
from AthenaServices.Configurables import ThinningSvc, createThinningSvc
augStream = MSMgr.GetStream( streamName )
evtStream = augStream.GetEventStream()

BPHY2ThinningSvc = createThinningSvc( svcName="BPHY2ThinningSvc", outStreams=[evtStream] )
svcMgr += BPHY2ThinningSvc

#====================================================================
# Slimming
#====================================================================

# Added by ASC
from DerivationFrameworkCore.SlimmingHelper import SlimmingHelper
BPHY2SlimmingHelper = SlimmingHelper("BPHY2SlimmingHelper")
AllVariables  = []
StaticContent = []

# Needed for trigger objects
BPHY2SlimmingHelper.IncludeMuonTriggerContent  = TRUE
BPHY2SlimmingHelper.IncludeBPhysTriggerContent = TRUE
SmartVar = []
## primary vertices
SmartVar  += ["PrimaryVertices"]
StaticContent += ["xAOD::VertexContainer#BPHY2RefittedPrimaryVertices"]
StaticContent += ["xAOD::VertexAuxContainer#BPHY2RefittedPrimaryVerticesAux."]



## ID track particles
SmartVar += ["InDetTrackParticles"]

## combined / extrapolated muon track particles
## (note: for tagged muons there is no extra TrackParticle collection since the ID tracks
##        are store in InDetTrackParticles collection)
AllVariables += ["CombinedMuonTrackParticles"]


## muon container
SmartVar += ["Muons"]


## Jpsi candidates
StaticContent += ["xAOD::VertexContainer#%s"        %                 BPHY2JpsiSelectAndWrite.OutputVtxContainerName]
## we have to disable vxTrackAtVertex branch since it is not xAOD compatible
StaticContent += ["xAOD::VertexAuxContainer#%sAux.-vxTrackAtVertex" % BPHY2JpsiSelectAndWrite.OutputVtxContainerName]

StaticContent += ["xAOD::VertexContainer#%s"        %                 BPHY2BsKKSelectAndWrite.OutputVtxContainerName]
StaticContent += ["xAOD::VertexAuxContainer#%sAux.-vxTrackAtVertex" % BPHY2BsKKSelectAndWrite.OutputVtxContainerName]


# Tagging information (in addition to that already requested by usual algorithms)
AllVariables += ["MuonSpectrometerTrackParticles" ]





# Added by ASC
# Truth information for MC only
if isSimulation:
    AllVariables += ["TruthEvents","TruthParticles","TruthVertices","MuonTruthParticles" ]


AllVariables = list(set(AllVariables)) # remove duplicates

BPHY2SlimmingHelper.AllVariables = AllVariables
BPHY2SlimmingHelper.StaticContent = StaticContent
BPHY2SlimmingHelper.SmartCollections = SmartVar

BPHY2SlimmingHelper.AppendContentToStream(BPHY2Stream)
