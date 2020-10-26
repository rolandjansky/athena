#====================================================================
# BPHY16.py
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
BPHY16_VertexTools = BPHYVertexTools("BPHY16")

#--------------------------------------------------------------------
## 2/ Setup the vertex fitter tools (e.g. JpsiFinder, JpsiPlus1Track, etc).
##    These are general tools independent of DerivationFramework that do the 
##    actual vertex fitting and some pre-selection.
from JpsiUpsilonTools.JpsiUpsilonToolsConf import Analysis__JpsiFinder
BPHY16JpsiFinder = Analysis__JpsiFinder(
    name                        = "BPHY16JpsiFinder",
    OutputLevel                 = INFO,
    muAndMu                     = True,
    muAndTrack                  = False,
    TrackAndTrack               = False,
    assumeDiMuons               = True,    # If true, will assume dimu hypothesis and use PDG value for mu mass
    invMassUpper                = 12000.0,
    invMassLower                = 8000.,
    Chi2Cut                     = 20.,
    oppChargesOnly	        = True,
    atLeastOneComb              = True,
    useCombinedMeasurement      = False, # Only takes effect if combOnly=True	
    muonCollectionKey           = "Muons",
    TrackParticleCollection     = "InDetTrackParticles",
    V0VertexFitterTool          = BPHY16_VertexTools.TrkV0Fitter, # V0 vertex fitter
    useV0Fitter                 = False, # if False a TrkVertexFitterTool will be used
    TrkVertexFitterTool         = BPHY16_VertexTools.TrkVKalVrtFitter, # VKalVrt vertex fitter
    TrackSelectorTool           = BPHY16_VertexTools.InDetTrackSelectorTool,
    ConversionFinderHelperTool  = BPHY16_VertexTools.InDetConversionHelper,
    VertexPointEstimator        = BPHY16_VertexTools.VtxPointEstimator,
    useMCPCuts                  = False )

ToolSvc += BPHY16JpsiFinder
print      BPHY16JpsiFinder

#--------------------------------------------------------------------
## 3/ setup the vertex reconstruction "call" tool(s). They are part of the derivation framework.
##    These Augmentation tools add output vertex collection(s) into the StoreGate and add basic 
##    decorations which do not depend on the vertex mass hypothesis (e.g. lxy, ptError, etc).
##    There should be one tool per topology, i.e. Jpsi and Psi(2S) do not need two instance of the
##    Reco tool is the JpsiFinder mass window is wide enough.

from DerivationFrameworkBPhys.DerivationFrameworkBPhysConf import DerivationFramework__Reco_mumu
BPHY16_Reco_mumu = DerivationFramework__Reco_mumu(
  name                   = "BPHY16_Reco_mumu",
  JpsiFinder             = BPHY16JpsiFinder,
  OutputVtxContainerName = "BPHY16OniaCandidates",
  PVContainerName        = "PrimaryVertices",
  RefPVContainerName     = "BPHY16RefittedPrimaryVertices",
  RefitPV                = True,
  MaxPVrefit             = 100000,
  DoVertexType           = 7)
  
ToolSvc += BPHY16_Reco_mumu
print BPHY16_Reco_mumu


from DerivationFrameworkBPhys.DerivationFrameworkBPhysConf import DerivationFramework__Select_onia2mumu

## a/ augment and select Jpsi->mumu candidates
BPHY16_Select_Upsi = DerivationFramework__Select_onia2mumu(
  name                  = "BPHY16_Select_Upsi",
  HypothesisName        = "Upsilon",
  InputVtxContainerName = "BPHY16OniaCandidates",
  VtxMassHypo           = 9460.30,
  MassMin               = 8000.,
  MassMax               = 12000.,
  Chi2Max               = 200,
  DoVertexType          = 7)
  
ToolSvc += BPHY16_Select_Upsi
print BPHY16_Select_Upsi


## 4/ setup a new vertexing tool (necessary due to use of mass constraint) 
from TrkVKalVrtFitter.TrkVKalVrtFitterConf import Trk__TrkVKalVrtFitter
BPHY16VertexFit = Trk__TrkVKalVrtFitter(
                                         name                = "BPHY16VertexFit",
                                         Extrapolator        = BPHY16_VertexTools.InDetExtrapolator,
                                         FirstMeasuredPoint  = True,
                                         MakeExtendedVertex  = True)
ToolSvc += BPHY16VertexFit
print      BPHY16VertexFit

## 5/ setup the Jpsi+2 track finder
from JpsiUpsilonTools.JpsiUpsilonToolsConf import Analysis__JpsiPlus2Tracks
BPHY16Plus2Tracks = Analysis__JpsiPlus2Tracks(name = "BPHY16Plus2Tracks",
#                                        OutputLevel = DEBUG,
kaonkaonHypothesis			= False,
pionpionHypothesis                      = False,
kaonpionHypothesis                      = False,
ManualMassHypo                          = [ 105.658, 105.658, 105.658, 105.658 ],
trkThresholdPt			        = 0.0,
trkMaxEta				= 3.0,
BMassUpper		                = 50000.0,
BMassLower			        = 0,
oppChargesOnly                          = False,
#DiTrackMassUpper = 1019.445 + 100.,
#DiTrackMassLower = 1019.445 - 100.,
Chi2Cut                             = 100.0,
#TrkQuadrupletMassUpper      = 60000.0,
#TrkQuadrupletMassLower      = 0.01,
JpsiContainerKey                    = "BPHY16OniaCandidates",
TrackParticleCollection             = "InDetTrackParticles",
MuonsUsedInJpsi			    = "Muons",
ExcludeJpsiMuonsOnly                = True,
RequireNMuonTracks                  = 1,
TrkVertexFitterTool		    = BPHY16VertexFit,
TrackSelectorTool		    = BPHY16_VertexTools.InDetTrackSelectorTool,
UseMassConstraint		    = False)

ToolSvc += BPHY16Plus2Tracks
print      BPHY16Plus2Tracks    

## 6/ setup the combined augmentation/skimming tool for the Bpm
from DerivationFrameworkBPhys.DerivationFrameworkBPhysConf import DerivationFramework__Reco_dimuTrkTrk	
BPHY16FourTrackSelectAndWrite = DerivationFramework__Reco_dimuTrkTrk(name           = "BPHY16FourTrackSelectAndWrite",
                                                           Jpsi2PlusTrackName       = BPHY16Plus2Tracks,
                                                           OutputVtxContainerName   = "BPHY16FourTrack",
                                                           PVContainerName          = "PrimaryVertices",
                                                           RefPVContainerName       = "BPHY16RefittedPrimaryVertices",
                                                           RefitPV                  = True,
                                                           MaxPVrefit               = 10000, DoVertexType = 7)
ToolSvc += BPHY16FourTrackSelectAndWrite 
print      BPHY16FourTrackSelectAndWrite


BPHY16_Select_FourTrack      = DerivationFramework__Select_onia2mumu(
  name                       = "BPHY16_Select_FourTracks",
  HypothesisName             = "FourTracks",
  InputVtxContainerName      = "BPHY16FourTrack",
  TrkMasses                  = [105.658, 105.658, 105.658, 105.658],
  VtxMassHypo                = 18100.0,
  MassMin                    = 0,
  MassMax                    = 500000,
  Chi2Max                    = BPHY16Plus2Tracks.Chi2Cut)

ToolSvc += BPHY16_Select_FourTrack
print      BPHY16_Select_FourTrack

from DerivationFrameworkBPhys.DerivationFrameworkBPhysConf import DerivationFramework__ReVertex
BPHY16_Revertex      = DerivationFramework__ReVertex(
  name                       = "BPHY16_ReVertex",
  InputVtxContainerName      = "BPHY16FourTrack",
  TrackIndices               = [ 2, 3 ],
  TrkVertexFitterTool		    = BPHY16VertexFit,
  OutputVtxContainerName     = "BPHY16TwoTrack"
)

ToolSvc += BPHY16_Revertex
print      BPHY16_Revertex

BPHY16_Select_TwoTrack      = DerivationFramework__Select_onia2mumu(
  name                       = "BPHY16_Select_TwoTracks",
  HypothesisName             = "TwoTracks",
  InputVtxContainerName      = "BPHY16TwoTrack",
  TrkMasses                  = [105.658, 105.658],
  VtxMassHypo                = 18100.0,
  MassMin                    = 1,
  MassMax                    = 500000,
  Chi2Max                    = 90)

ToolSvc += BPHY16_Select_TwoTrack
print      BPHY16_Select_TwoTrack

expression = "count(BPHY16FourTrack.passed_FourTracks) > 0"

from DerivationFrameworkTools.DerivationFrameworkToolsConf import DerivationFramework__xAODStringSkimmingTool
BPHY16_SelectEvent = DerivationFramework__xAODStringSkimmingTool(name = "BPHY16_SelectEvent",
                                                                expression = expression)
ToolSvc += BPHY16_SelectEvent
print BPHY16_SelectEvent

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


#====================================================================
# CREATE THE DERIVATION KERNEL ALGORITHM AND PASS THE ABOVE TOOLS  
#====================================================================
## 7/ IMPORTANT bit. Don't forget to pass the tools to the DerivationKernel! If you don't do that, they will not be 
##    be executed!


# The name of the kernel (BPHY16Kernel in this case) must be unique to this derivation
from DerivationFrameworkCore.DerivationFrameworkCoreConf import DerivationFramework__DerivationKernel
DerivationFrameworkJob += CfgMgr.DerivationFramework__DerivationKernel(
  "BPHY16Kernel",
   AugmentationTools = [BPHY16_Reco_mumu, BPHY16_Select_Upsi, BPHY16FourTrackSelectAndWrite, BPHY16_Select_FourTrack, BPHY16_Revertex, BPHY16_Select_TwoTrack],
   SkimmingTools     = [BPHY16_SelectEvent]
   )

#====================================================================
# SET UP STREAM   
#====================================================================
streamName = derivationFlags.WriteDAOD_BPHY16Stream.StreamName
fileName   = buildFileName( derivationFlags.WriteDAOD_BPHY16Stream )
BPHY16Stream = MSMgr.NewPoolRootStream( streamName, fileName )
BPHY16Stream.AcceptAlgs(["BPHY16Kernel"])
# Special lines for thinning
# Thinning service name must match the one passed to the thinning tools
from AthenaServices.Configurables import ThinningSvc, createThinningSvc
augStream = MSMgr.GetStream( streamName )
evtStream = augStream.GetEventStream()
svcMgr += createThinningSvc( svcName="BPHY16ThinningSvc", outStreams=[evtStream] )


#====================================================================
# Slimming 
#====================================================================

from DerivationFrameworkCore.SlimmingHelper import SlimmingHelper
BPHY16SlimmingHelper = SlimmingHelper("BPHY16SlimmingHelper")
AllVariables = []
StaticContent = []

# Needed for trigger objects
BPHY16SlimmingHelper.IncludeMuonTriggerContent = True
BPHY16SlimmingHelper.IncludeBPhysTriggerContent = True

## primary vertices
AllVariables += ["PrimaryVertices"]
StaticContent += ["xAOD::VertexContainer#BPHY16RefittedPrimaryVertices"]
StaticContent += ["xAOD::VertexAuxContainer#BPHY16RefittedPrimaryVerticesAux."]

## ID track particles
AllVariables += ["InDetTrackParticles"]

## combined / extrapolated muon track particles 
## (note: for tagged muons there is no extra TrackParticle collection since the ID tracks
##        are store in InDetTrackParticles collection)
AllVariables += ["CombinedMuonTrackParticles"]
AllVariables += ["ExtrapolatedMuonTrackParticles"]

## muon container
AllVariables += ["Muons"]


StaticContent += ["xAOD::VertexContainer#%s"        % BPHY16_Reco_mumu.OutputVtxContainerName]
StaticContent += ["xAOD::VertexAuxContainer#%sAux." % BPHY16_Reco_mumu.OutputVtxContainerName]
## we have to disable vxTrackAtVertex branch since it is not xAOD compatible
StaticContent += ["xAOD::VertexAuxContainer#%sAux.-vxTrackAtVertex" % BPHY16_Reco_mumu.OutputVtxContainerName]

StaticContent += ["xAOD::VertexContainer#%s"        % BPHY16FourTrackSelectAndWrite.OutputVtxContainerName]
StaticContent += ["xAOD::VertexAuxContainer#%sAux." % BPHY16FourTrackSelectAndWrite.OutputVtxContainerName]
## we have to disable vxTrackAtVertex branch since it is not xAOD compatible
StaticContent += ["xAOD::VertexAuxContainer#%sAux.-vxTrackAtVertex" % BPHY16FourTrackSelectAndWrite.OutputVtxContainerName]

StaticContent += ["xAOD::VertexContainer#%s"        % BPHY16_Revertex.OutputVtxContainerName]
StaticContent += ["xAOD::VertexAuxContainer#%sAux." % BPHY16_Revertex.OutputVtxContainerName]
## we have to disable vxTrackAtVertex branch since it is not xAOD compatible
StaticContent += ["xAOD::VertexAuxContainer#%sAux.-vxTrackAtVertex" % BPHY16_Revertex.OutputVtxContainerName]


# Truth information for MC only
if isSimulation:
    AllVariables += ["TruthEvents","TruthParticles","TruthVertices","MuonTruthParticles"]

BPHY16SlimmingHelper.AllVariables = AllVariables
BPHY16SlimmingHelper.StaticContent = StaticContent
BPHY16SlimmingHelper.AppendContentToStream(BPHY16Stream)

