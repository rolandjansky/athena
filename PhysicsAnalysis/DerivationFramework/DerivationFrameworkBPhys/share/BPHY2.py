#====================================================================
# BPHY2.py
# B+->J/psiK+ 
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
## 1/ setup vertexing tools and services
#include( "JpsiUpsilonTools/configureServices.py" )

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
                                        invMassUpper                = 3600.0,
                                        invMassLower                = 2600.0,
                                        Chi2Cut                     = 10.,
                                        oppChargesOnly	            = True,
                                        combOnly		    = True,
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
BPHY2JpsiSelectAndWrite = DerivationFramework__Reco_mumu(name              = "BPHY2JpsiSelectAndWrite",
                                                       JpsiFinder    = BPHY2JpsiFinder,
                                                       OutputVtxContainerName = "BPHY2JpsiCandidates",
                                                       PVContainerName        = "PrimaryVertices",
                                                       RefPVContainerName     = "SHOULDNOTBEUSED")
ToolSvc += BPHY2JpsiSelectAndWrite
print BPHY2JpsiSelectAndWrite


from DerivationFrameworkBPhys.DerivationFrameworkBPhysConf import DerivationFramework__Select_onia2mumu

## a/ augment and select Jpsi->mumu candidates
BPHY2_Select_Jpsi2mumu = DerivationFramework__Select_onia2mumu(
  name                  = "BPHY2_Select_Jpsi2mumu",
  HypothesisName        = "Jpsi",
  InputVtxContainerName = "BPHY2JpsiCandidates",
  VtxMassHypo           = 3096.916,
  MassMin               = 2000.0,
  MassMax               = 3600.0,
  Chi2Max               = 200,
   DoVertexType =1)

  
ToolSvc += BPHY2_Select_Jpsi2mumu
print BPHY2_Select_Jpsi2mumu




## 4/ setup a new vertexing tool (necessary due to use of mass constraint) 
from TrkVKalVrtFitter.TrkVKalVrtFitterConf import Trk__TrkVKalVrtFitter
BpmVertexFit = Trk__TrkVKalVrtFitter(
                                         name                = "BpmVertexFit",
                                         Extrapolator        = BPHY2_VertexTools.InDetExtrapolator,
                                         FirstMeasuredPoint  = True,
                                         MakeExtendedVertex  = True)
ToolSvc += BpmVertexFit
print      BpmVertexFit

## 5/ setup the Jpsi+1 track finder
from JpsiUpsilonTools.JpsiUpsilonToolsConf import Analysis__JpsiPlus1Track
BPHY2BpmJpsiKpm = Analysis__JpsiPlus1Track(		name 					= "BPHY2BpmJpsiKpm",
                                              OutputLevel = INFO,#DEBUG,
												pionHypothesis			= False,
												kaonHypothesis			= True,
												trkThresholdPt			= 800.0,
												trkMaxEta				= 2.5,
												BThresholdPt			= 100.0,
												BMassUpper				= 6000.0,
												BMassLower				= 4500.0,
												JpsiContainerKey		= "BPHY2JpsiCandidates",
												TrackParticleCollection = "InDetTrackParticles",
												MuonsUsedInJpsi			= "Muons",
												TrkVertexFitterTool		= BpmVertexFit,
												TrackSelectorTool		= BPHY2_VertexTools.InDetTrackSelectorTool,
												UseMassConstraint		= True, 
    Chi2Cut = 10,
    TrkTrippletMassUpper = 6500,
    TrkTrippletMassLower = 4000 )
        
ToolSvc += BPHY2BpmJpsiKpm
print      BPHY2BpmJpsiKpm    

## 6/ setup the combined augmentation/skimming tool for the Bpm
from DerivationFrameworkBPhys.DerivationFrameworkBPhysConf import DerivationFramework__Reco_dimuTrk
BPHY2BpmSelectAndWrite = DerivationFramework__Reco_dimuTrk(name                     = "BPHY2BpmSelectAndWrite",
                                                           Jpsi1PlusTrackName       = BPHY2BpmJpsiKpm,
                                                           OutputVtxContainerName   = "BpmJpsiKpmCandidates",
                                                           PVContainerName          = "PrimaryVertices",
                                                           RefPVContainerName       = "BPHY2RefittedPrimaryVertices",
                                                           RefitPV                  = True,
                                                           MaxPVrefit = 1000)
ToolSvc += BPHY2BpmSelectAndWrite 
print BPHY2BpmSelectAndWrite

## b/ augment and select Bplus->JpsiKplus candidates
BPHY2_Select_Bplus2JpsiKplus = DerivationFramework__Select_onia2mumu(
  name                       = "BPHY2_Select_Bplus2JpsiKplus",
  HypothesisName             = "Bplus",
  InputVtxContainerName      = "BpmJpsiKpmCandidates",
  TrkMasses                  = [105.658, 105.658, 493.677],
  VtxMassHypo                = 5279.26,
  MassMin                    = 4000.0,
  MassMax                    = 8000.0,
  Chi2Max                    = 200)

ToolSvc += BPHY2_Select_Bplus2JpsiKplus
print BPHY2_Select_Bplus2JpsiKplus

#expression = "count(BpmJpsiKpmCandidates.passed_Bplus) > 0"
#from DerivationFrameworkTools.DerivationFrameworkToolsConf import DerivationFramework__xAODStringSkimmingTool
#BPHY2_SelectEvent = DerivationFramework__xAODStringSkimmingTool(name = "BPHY2_SelectEvent",
#                                                                expression = expression)
#ToolSvc += BPHY2_SelectEvent
#print BPHY2_SelectEvent


from DerivationFrameworkBPhys.DerivationFrameworkBPhysConf import DerivationFramework__SelectEvent
BPHY2_SelectEvent = DerivationFramework__SelectEvent(
  name = "BPHY2_SelectEvent",
  SelectionExpression = "BpmJpsiKpmCandidates.passed_Bplus > 0")
                                                    
ToolSvc += BPHY2_SelectEvent
print BPHY2_SelectEvent



from DerivationFrameworkBPhys.DerivationFrameworkBPhysConf import DerivationFramework__Thin_vtxTrk
BPHY2_thinningTool_Tracks = DerivationFramework__Thin_vtxTrk(
  name                       = "BPHY2_thinningTool_Tracks",
  ThinningService            = "BPHY2ThinningSvc",
  TrackParticleContainerName = "InDetTrackParticles",
  VertexContainerNames       = ["BpmJpsiKpmCandidates"],
  PassFlags                  = ["passed_Bplus"] )

ToolSvc += BPHY2_thinningTool_Tracks

from DerivationFrameworkBPhys.DerivationFrameworkBPhysConf import DerivationFramework__BPhysPVThinningTool
BPHY2_thinningTool_PV = DerivationFramework__BPhysPVThinningTool(
  name                       = "BPHY2_thinningTool_PV",
  ThinningService            = "BPHY2ThinningSvc",
  CandidateCollections       = ["BpmJpsiKpmCandidates"],
  KeepPVTracks  =False
 )

ToolSvc += BPHY2_thinningTool_PV


## b) thinning out tracks that are not attached to muons. The final thinning decision is based on the OR operation
##    between decision from this and the previous tools.
from DerivationFrameworkInDet.DerivationFrameworkInDetConf import DerivationFramework__MuonTrackParticleThinning
BPHY2MuonTPThinningTool = DerivationFramework__MuonTrackParticleThinning(name                    = "BPHY2MuonTPThinningTool",
                                                                         ThinningService         = "BPHY2ThinningSvc",
                                                                         MuonKey                 = "Muons",
                                                                         InDetTrackParticlesKey  = "InDetTrackParticles")
ToolSvc += BPHY2MuonTPThinningTool


# Added by ASC
# Only save truth informtion directly associated with Onia
#from DerivationFrameworkMCTruth.DerivationFrameworkMCTruthConf import DerivationFramework__GenericTruthThinning
#BPHY2TruthThinTool = DerivationFramework__GenericTruthThinning(name                    = "BPHY2TruthThinTool",
#                                                        ThinningService         = "BPHY2ThinningSvc",
#                                                        ParticleSelectionString = "TruthParticles.pdgId == 443 || TruthParticles.pdgId == 999443 || TruthParticles.pdgId == 531 || TruthParticles.pdgId == 100553 || TruthParticles.pdgId == 200553 || TruthParticles.pdgId == 321 || TruthParticles.pdgId == 13 ",
#                                                        PreserveDescendants     = True,
#                                                        PreserveAncestors      = True)
#ToolSvc += BPHY2TruthThinTool
#print BPHY2TruthThinTool


#====================================================================
# CREATE THE DERIVATION KERNEL ALGORITHM AND PASS THE ABOVE TOOLS  
#====================================================================

# The name of the kernel (BPHY2Kernel in this case) must be unique to this derivation
from DerivationFrameworkCore.DerivationFrameworkCoreConf import DerivationFramework__DerivationKernel
if isSimulation: 
	DerivationFrameworkJob += CfgMgr.DerivationFramework__DerivationKernel("BPHY2Kernel",
                                                                       AugmentationTools = [BPHY2_AugOriginalCounts, BPHY2JpsiSelectAndWrite,BPHY2_Select_Jpsi2mumu,BPHY2BpmSelectAndWrite,BPHY2_Select_Bplus2JpsiKplus] ,
                                                                       ThinningTools     = [BPHY2_thinningTool_Tracks, BPHY2_thinningTool_PV, BPHY2MuonTPThinningTool]
                                                                       )
else:
        DerivationFrameworkJob += CfgMgr.DerivationFramework__DerivationKernel("BPHY2Kernel",
                                                                       AugmentationTools = [BPHY2_AugOriginalCounts, BPHY2JpsiSelectAndWrite,BPHY2_Select_Jpsi2mumu,BPHY2BpmSelectAndWrite,BPHY2_Select_Bplus2JpsiKplus],
                                                                       SkimmingTools     = [BPHY2_SelectEvent],
                                                                       ThinningTools     = [BPHY2_thinningTool_Tracks, BPHY2_thinningTool_PV, BPHY2MuonTPThinningTool]
                                                                       )

#====================================================================
# SET UP STREAM   
#====================================================================
streamName = derivationFlags.WriteDAOD_BPHY2Stream.StreamName
fileName   = buildFileName( derivationFlags.WriteDAOD_BPHY2Stream )
BPHY2Stream = MSMgr.NewPoolRootStream( streamName, fileName )
BPHY2Stream.AcceptAlgs(["BPHY2Kernel"])
# Special lines for thinning
# Thinning service name must match the one passed to the thinning tools
from AthenaServices.Configurables import ThinningSvc, createThinningSvc
augStream = MSMgr.GetStream( streamName )
evtStream = augStream.GetEventStream()
svcMgr += createThinningSvc( svcName="BPHY2ThinningSvc", outStreams=[evtStream] )

#====================================================================
# Slimming 
#====================================================================

# Added by ASC
from DerivationFrameworkCore.SlimmingHelper import SlimmingHelper
BPHY2SlimmingHelper = SlimmingHelper("BPHY2SlimmingHelper")


AllVariables = []
StaticContent = []

# Needed for trigger objects
BPHY2SlimmingHelper.IncludeMuonTriggerContent = True
BPHY2SlimmingHelper.IncludeBPhysTriggerContent = True
#BPHY2SlimmingHelper.TriggerChains = 'HLT_xAOD__TrigBphysContainer_EFBMuMuFex|HLT_xAOD__TrigBphysContainer_EFBMuMuFexAux.|HLT_xAOD__TrigBphysContainer_EFBMuMuXFex|HLT_xAOD__TrigBphysContainer_EFBMuMuXFexAux.|HLT_xAOD__MuonContainer_MuonEFInfo|HLT_xAOD__MuonContainer_MuonEFInfoAux.pt.eta.phi.muonType.author.inDetTrackParticleLink'


## primary vertices
AllVariables += ["PrimaryVertices"]
StaticContent += ["xAOD::VertexContainer#BPHY2RefittedPrimaryVertices"]
StaticContent += ["xAOD::VertexAuxContainer#BPHY2RefittedPrimaryVerticesAux."]

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
StaticContent += ["xAOD::VertexContainer#%s"        % BPHY2JpsiSelectAndWrite.OutputVtxContainerName]
StaticContent += ["xAOD::VertexAuxContainer#%sAux." % BPHY2JpsiSelectAndWrite.OutputVtxContainerName]
## we have to disable vxTrackAtVertex branch since it is not xAOD compatible
StaticContent += ["xAOD::VertexAuxContainer#%sAux.-vxTrackAtVertex" % BPHY2JpsiSelectAndWrite.OutputVtxContainerName]

StaticContent += ["xAOD::VertexContainer#%s"        % BPHY2BpmSelectAndWrite.OutputVtxContainerName]
StaticContent += ["xAOD::VertexAuxContainer#%sAux." % BPHY2BpmSelectAndWrite.OutputVtxContainerName]
StaticContent += ["xAOD::VertexAuxContainer#%sAux.-vxTrackAtVertex" % BPHY2BpmSelectAndWrite.OutputVtxContainerName]

# Added by ASC
# Truth information for MC only
if isSimulation:
    AllVariables += ["TruthEvents","TruthParticles","TruthVertices","MuonTruthParticles"]

BPHY2SlimmingHelper.AllVariables = AllVariables
BPHY2SlimmingHelper.StaticContent = StaticContent
BPHY2SlimmingHelper.AppendContentToStream(BPHY2Stream)


#from DerivationFrameworkCore.SlimmingHelper import SlimmingHelper
#BPHY2SlimmingHelper = SlimmingHelper("BPHY2SlimmingHelper")
#BPHY2SlimmingHelper.SmartCollections = [ "Muons",
#                                         "InDetTrackParticles",
#                                         "PrimaryVertices" ]
#BPHY2SlimmingHelper.AppendContentToStream(BPHY2Stream)
#BPHY2Stream.AddItem("xAOD::VertexContainer#BPHY2JpsiCandidates")
#BPHY2Stream.AddItem("xAOD::VertexContainer#BPHY2JpsiCandidatesAux.")
#BPHY2Stream.AddItem("xAOD::VertexContainer#BPHY2JpsiCandidatesAux.-vxTrackAtVertex")
#BPHY2Stream.AddItem("xAOD::VertexContainer#BpmJpsiKpmCandidates")
#BPHY2Stream.AddItem("xAOD::VertexContainer#BpmJpsiKpmCandidatesAux.")
#BPHY2Stream.AddItem("xAOD::VertexContainer#BpmJpsiKpmCandidatesAux.-vxTrackAtVertex")

