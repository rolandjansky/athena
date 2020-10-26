#====================================================================
# BPHY13.py (Based on BPHY8, BPHY16, and the old BPHY13)
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
BPHY13_VertexTools = BPHYVertexTools("BPHY13")

#--------------------------------------------------------------------
## 2/ Setup the vertex fitter tools (e.g. JpsiFinder, JpsiPlus1Track, etc).
##    These are general tools independent of DerivationFramework that do the 
##    actual vertex fitting and some pre-selection.
from JpsiUpsilonTools.JpsiUpsilonToolsConf import Analysis__JpsiFinder
BPHY13JpsiFinder = Analysis__JpsiFinder(
    name                        = "BPHY13JpsiFinder",
    OutputLevel                 = INFO,
    muAndMu                     = True,
    muAndTrack                  = False,
    TrackAndTrack               = False,
    assumeDiMuons               = True,  # If true, will assume dimu hypothesis and use PDG value for mu mass
    trackThresholdPt            = 2500.,
    invMassUpper                = 12500.,
    invMassLower                = 2000.,
    Chi2Cut                     = 200.,
    oppChargesOnly	        = True,
    atLeastOneComb              = True,
    useCombinedMeasurement      = False, # Only takes effect if combOnly=True	
    muonCollectionKey           = "Muons",
    TrackParticleCollection     = "InDetTrackParticles",
    V0VertexFitterTool          = BPHY13_VertexTools.TrkV0Fitter, # V0 vertex fitter
    useV0Fitter                 = False, # if False a TrkVertexFitterTool will be used
    TrkVertexFitterTool         = BPHY13_VertexTools.TrkVKalVrtFitter, # VKalVrt vertex fitter
    TrackSelectorTool           = BPHY13_VertexTools.InDetTrackSelectorTool,
    ConversionFinderHelperTool  = BPHY13_VertexTools.InDetConversionHelper,
    VertexPointEstimator        = BPHY13_VertexTools.VtxPointEstimator,
    useMCPCuts                  = False )

ToolSvc += BPHY13JpsiFinder
print      BPHY13JpsiFinder

#--------------------------------------------------------------------
## 3/ setup the vertex reconstruction "call" tool(s). They are part of the derivation framework.
##    These Augmentation tools add output vertex collection(s) into the StoreGate and add basic 
##    decorations which do not depend on the vertex mass hypothesis (e.g. lxy, ptError, etc).
##    There should be one tool per topology, i.e. Jpsi and Psi(2S) do not need two instance of the
##    Reco tool if the JpsiFinder mass window is wide enough.

# https://gitlab.cern.ch/atlas/athena/-/blob/21.2/PhysicsAnalysis/DerivationFramework/DerivationFrameworkBPhys/src/Reco_mumu.cxx
from DerivationFrameworkBPhys.DerivationFrameworkBPhysConf import DerivationFramework__Reco_mumu
BPHY13_Reco_mumu = DerivationFramework__Reco_mumu(
    name                   = "BPHY13_Reco_mumu",
    JpsiFinder             = BPHY13JpsiFinder,
    OutputVtxContainerName = "BPHY13OniaCandidates",
    PVContainerName        = "PrimaryVertices",
    RefPVContainerName     = "SHOULDNOTBEUSED",
#    RefPVContainerName     = "BPHY13RefittedPrimaryVertices",
#    RefitPV                = True,
#    MaxPVrefit             = 10000,
#https://gitlab.cern.ch/atlas/athena/-/blob/21.2/PhysicsAnalysis/DerivationFramework/DerivationFrameworkBPhys/src/BPhysPVTools.cxx#L259
# bit pattern: doZ0BA|doZ0|doA0|doPt
    DoVertexType           = 1)
  
ToolSvc += BPHY13_Reco_mumu
print BPHY13_Reco_mumu

## 4/ setup a new vertexing tool (necessary due to use of mass constraint) 
from TrkVKalVrtFitter.TrkVKalVrtFitterConf import Trk__TrkVKalVrtFitter
BPHY13VertexFit = Trk__TrkVKalVrtFitter(
    name                = "BPHY13VertexFit",
    Extrapolator        = BPHY13_VertexTools.InDetExtrapolator,
#    FirstMeasuredPoint  = True,
    FirstMeasuredPoint  = False,
    MakeExtendedVertex  = True)
ToolSvc += BPHY13VertexFit
print      BPHY13VertexFit

## 5/ setup the Jpsi+2 track finder
# https://gitlab.cern.ch/atlas/athena/-/blob/21.2/PhysicsAnalysis/JpsiUpsilonTools/src/JpsiPlus2Tracks.cxx
from JpsiUpsilonTools.JpsiUpsilonToolsConf import Analysis__JpsiPlus2Tracks
BPHY13Plus2Tracks = Analysis__JpsiPlus2Tracks(
    name = "BPHY13Plus2Tracks",
    #           OutputLevel = DEBUG,
    kaonkaonHypothesis		        = False,
    pionpionHypothesis                  = False,
    kaonpionHypothesis                  = False,
    ManualMassHypo                      = [ 105.658, 105.658, 105.658, 105.658 ],
    trkThresholdPt			= 1500.,
    trkMaxEta			        = 2.5,
    oppChargesOnly                      = False,
    DiTrackMassUpper                    = 12500.,
    DiTrackMassLower                    = 2000.,
    TrkQuadrupletMassUpper              = 25000.,
    TrkQuadrupletMassLower              = 0.,
    Chi2Cut                             = 200.,
    JpsiContainerKey                    = "BPHY13OniaCandidates",
    TrackParticleCollection             = "InDetTrackParticles",
    MuonsUsedInJpsi			= "Muons",
    ExcludeJpsiMuonsOnly                = True,
    RequireNMuonTracks                  = 1,
    TrkVertexFitterTool		        = BPHY13VertexFit,
    TrackSelectorTool		        = BPHY13_VertexTools.InDetTrackSelectorTool,
    UseMassConstraint		        = False)

ToolSvc += BPHY13Plus2Tracks
print      BPHY13Plus2Tracks    

## 6/ setup the combined augmentation/skimming tool
from DerivationFrameworkBPhys.DerivationFrameworkBPhysConf import DerivationFramework__Reco_dimuTrkTrk	
BPHY13FourTrackSelectAndWrite = DerivationFramework__Reco_dimuTrkTrk(
    name                     = "BPHY13FourTrackSelectAndWrite",
    Jpsi2PlusTrackName       = BPHY13Plus2Tracks,
    OutputVtxContainerName   = "BPHY13FourTrack",
    PVContainerName          = "PrimaryVertices",
    RefPVContainerName       = "BPHY13RefittedPrimaryVertices",
    RefitPV                  = True,
    MaxPVrefit               = 10000,
    DoVertexType             = 7)

ToolSvc += BPHY13FourTrackSelectAndWrite 
print      BPHY13FourTrackSelectAndWrite


from DerivationFrameworkBPhys.DerivationFrameworkBPhysConf import DerivationFramework__Select_onia2mumu

BPHY13_Select_FourTrack      = DerivationFramework__Select_onia2mumu(
    name                       = "BPHY13_Select_FourTrack",
    HypothesisName             = "FourTracks",
    InputVtxContainerName      = "BPHY13FourTrack",
    TrkMasses                  = [105.658, 105.658, 105.658, 105.658],
    VtxMassHypo                = 6900.0, # for decay time
    MassMin                    = 0.,
    MassMax                    = 25000.,
    Chi2Max                    = 200.)

ToolSvc += BPHY13_Select_FourTrack
print      BPHY13_Select_FourTrack


#====================================================================
# Isolation
#====================================================================

#Track isolation for candidates
from DerivationFrameworkBPhys.DerivationFrameworkBPhysConf import DerivationFramework__VertexTrackIsolation
BPHY13TrackIsolationDecorator = DerivationFramework__VertexTrackIsolation(
  name                            = "BPHY13TrackIsolationDecorator",
  OutputLevel                     = INFO,
  TrackIsoTool                    = "xAOD::TrackIsolationTool",
  TrackContainer                  = "InDetTrackParticles",
  InputVertexContainer            = "BPHY13FourTrack",
  PassFlags                       = ["passed_FourTracks"],
  DoIsoPerTrk                     = True,
  RemoveDuplicate                 = 2
)

ToolSvc += BPHY13TrackIsolationDecorator
print      BPHY13TrackIsolationDecorator


#====================================================================
# Revertex with mass constraint
#====================================================================

from DerivationFrameworkBPhys.DerivationFrameworkBPhysConf import DerivationFramework__ReVertex
BPHY13_Revertex_2mu            = DerivationFramework__ReVertex(
    name                       = "BPHY13_Revertex_2mu",
    InputVtxContainerName      = "BPHY13FourTrack",
    TrackIndices               = [ 0, 1 ],
    RefitPV                    = True,
    RefPVContainerName         = "BPHY13RefittedPrimaryVertices", # use existing refitted PVs
    UseMassConstraint          = True,
    VertexMass                 = 3096.916,
    MassInputParticles         = [105.658, 105.658],
    TrkVertexFitterTool	       = BPHY13VertexFit,
    OutputVtxContainerName     = "BPHY13TwoMuon")

ToolSvc += BPHY13_Revertex_2mu
print      BPHY13_Revertex_2mu

BPHY13_Select_TwoMuon          = DerivationFramework__Select_onia2mumu(
    name                       = "BPHY13_Select_TwoMuon",
    HypothesisName             = "TwoMuons",
    InputVtxContainerName      = "BPHY13TwoMuon",
    TrkMasses                  = [105.658, 105.658],
    VtxMassHypo                = 3096.916,
    MassMin                    = 2000.,
    MassMax                    = 3600.,
    Chi2Max                    = 200)

ToolSvc += BPHY13_Select_TwoMuon
print      BPHY13_Select_TwoMuon

BPHY13_Revertex_2trk           = DerivationFramework__ReVertex(
    name                       = "BPHY13_Revertex_2trk",
    InputVtxContainerName      = "BPHY13FourTrack",
    TrackIndices               = [ 2, 3 ],
    RefitPV                    = True,
    RefPVContainerName         = "BPHY13RefittedPrimaryVertices", # use existing refitted PVs
    UseMassConstraint          = True,
    VertexMass                 = 3096.916,
    MassInputParticles         = [105.658, 105.658],
    TrkVertexFitterTool	       = BPHY13VertexFit,
    OutputVtxContainerName     = "BPHY13TwoTrack")

ToolSvc += BPHY13_Revertex_2trk
print      BPHY13_Revertex_2trk

BPHY13_Select_TwoTrack         = DerivationFramework__Select_onia2mumu(
    name                       = "BPHY13_Select_TwoTrack",
    HypothesisName             = "TwoTracks",
    InputVtxContainerName      = "BPHY13TwoTrack",
    TrkMasses                  = [105.658, 105.658],
    VtxMassHypo                = 3096.916,
    MassMin                    = 2000.,
    MassMax                    = 3600.,
    Chi2Max                    = 200)

ToolSvc += BPHY13_Select_TwoTrack
print      BPHY13_Select_TwoTrack


BPHY13_Revertex_2muHi          = DerivationFramework__ReVertex(
    name                       = "BPHY13_Revertex_2muHi",
    InputVtxContainerName      = "BPHY13FourTrack",
    TrackIndices               = [ 0, 1 ],
    RefitPV                    = True,
    RefPVContainerName         = "BPHY13RefittedPrimaryVertices", # use existing refitted PVs
    UseMassConstraint          = True,
    VertexMass                 = 9460.30,
    MassInputParticles         = [105.658, 105.658],
    TrkVertexFitterTool	       = BPHY13VertexFit,
    OutputVtxContainerName     = "BPHY13TwoMuonHi")

ToolSvc += BPHY13_Revertex_2muHi
print      BPHY13_Revertex_2muHi

BPHY13_Select_TwoMuonHi        = DerivationFramework__Select_onia2mumu(
    name                       = "BPHY13_Select_TwoMuonHi",
    HypothesisName             = "TwoMuonsHi",
    InputVtxContainerName      = "BPHY13TwoMuonHi",
    TrkMasses                  = [105.658, 105.658],
    VtxMassHypo                = 9460.30,
    MassMin                    = 8500.,
    MassMax                    = 11000.,
    Chi2Max                    = 200)

ToolSvc += BPHY13_Select_TwoMuonHi
print      BPHY13_Select_TwoMuonHi

BPHY13_Revertex_2trkHi         = DerivationFramework__ReVertex(
    name                       = "BPHY13_Revertex_2trkHi",
    InputVtxContainerName      = "BPHY13FourTrack",
    TrackIndices               = [ 2, 3 ],
    RefitPV                    = True,
    RefPVContainerName         = "BPHY13RefittedPrimaryVertices", # use existing refitted PVs
    UseMassConstraint          = True,
    VertexMass                 = 9460.30,
    MassInputParticles         = [105.658, 105.658],
    TrkVertexFitterTool	       = BPHY13VertexFit,
    OutputVtxContainerName     = "BPHY13TwoTrackHi")

ToolSvc += BPHY13_Revertex_2trkHi
print      BPHY13_Revertex_2trkHi

BPHY13_Select_TwoTrackHi       = DerivationFramework__Select_onia2mumu(
    name                       = "BPHY13_Select_TwoTrackHi",
    HypothesisName             = "TwoTracksHi",
    InputVtxContainerName      = "BPHY13TwoTrackHi",
    TrkMasses                  = [105.658, 105.658],
    VtxMassHypo                = 9460.30,
    MassMin                    = 8500.,
    MassMax                    = 11000.,
    Chi2Max                    = 200)

ToolSvc += BPHY13_Select_TwoTrackHi
print      BPHY13_Select_TwoTrackHi


BPHY13_Revertex_2muMed         = DerivationFramework__ReVertex(
    name                       = "BPHY13_Revertex_2muMed",
    InputVtxContainerName      = "BPHY13FourTrack",
    TrackIndices               = [ 0, 1 ],
    RefitPV                    = True,
    RefPVContainerName         = "BPHY13RefittedPrimaryVertices", # use existing refitted PVs
    UseMassConstraint          = True,
    VertexMass                 = 3686.10,
    MassInputParticles         = [105.658, 105.658],
    TrkVertexFitterTool	       = BPHY13VertexFit,
    OutputVtxContainerName     = "BPHY13TwoMuonMed")

ToolSvc += BPHY13_Revertex_2muMed
print      BPHY13_Revertex_2muMed

BPHY13_Select_TwoMuonMed       = DerivationFramework__Select_onia2mumu(
    name                       = "BPHY13_Select_TwoMuonMed",
    HypothesisName             = "TwoMuonsMed",
    InputVtxContainerName      = "BPHY13TwoMuonMed",
    TrkMasses                  = [105.658, 105.658],
    VtxMassHypo                = 3686.10,
    MassMin                    = 3300.0,
    MassMax                    = 4500.0,
    Chi2Max                    = 200)

ToolSvc += BPHY13_Select_TwoMuonMed
print      BPHY13_Select_TwoMuonMed

BPHY13_Revertex_2trkMed        = DerivationFramework__ReVertex(
    name                       = "BPHY13_Revertex_2trkMed",
    InputVtxContainerName      = "BPHY13FourTrack",
    TrackIndices               = [ 2, 3 ],
    RefitPV                    = True,
    RefPVContainerName         = "BPHY13RefittedPrimaryVertices", # use existing refitted PVs
    UseMassConstraint          = True,
    VertexMass                 = 3686.10,
    MassInputParticles         = [105.658, 105.658],
    TrkVertexFitterTool	       = BPHY13VertexFit,
    OutputVtxContainerName     = "BPHY13TwoTrackMed")

ToolSvc += BPHY13_Revertex_2trkMed
print      BPHY13_Revertex_2trkMed

BPHY13_Select_TwoTrackMed      = DerivationFramework__Select_onia2mumu(
    name                       = "BPHY13_Select_TwoTrackMed",
    HypothesisName             = "TwoTracksMed",
    InputVtxContainerName      = "BPHY13TwoTrackMed",
    TrkMasses                  = [105.658, 105.658],
    VtxMassHypo                = 3686.10,
    MassMin                    = 3300.,
    MassMax                    = 4500.,
    Chi2Max                    = 200)

ToolSvc += BPHY13_Select_TwoTrackMed
print      BPHY13_Select_TwoTrackMed

#--------------------------------------------------------------------
## 7/ select the event. We only want to keep events that contain certain vertices which passed certain selection.
##    This is specified by the "SelectionExpression" property, which contains the expression in the following format:
##
##       "ContainerName.passed_HypoName > count"
##
##    where "ContainerName" is output container from some Reco_* tool, "HypoName" is the hypothesis name setup in some "Select_*"
##    tool and "count" is the number of candidates passing the selection you want to keep. 

#expression = "count(BPHY13FourTrack.passed_FourTracks) > 0"
expression = "count(BPHY13FourTrack.passed_FourTracks) > 0 && ( count(BPHY13TwoMuon.passed_TwoMuons) + count(BPHY13TwoTrack.passed_TwoTracks) > 1 || count(BPHY13TwoMuonMed.passed_TwoMuonsMed) + count(BPHY13TwoTrackMed.passed_TwoTracksMed) > 1 || count(BPHY13TwoMuon.passed_TwoMuons) + count(BPHY13TwoTrackMed.passed_TwoTracksMed) > 1 || count(BPHY13TwoMuonMed.passed_TwoMuonsMed) + count(BPHY13TwoTrack.passed_TwoTracks) > 1 || count(BPHY13TwoMuonHi.passed_TwoMuonsHi) + count(BPHY13TwoTrackHi.passed_TwoTracksHi) > 0 )"

from DerivationFrameworkTools.DerivationFrameworkToolsConf import DerivationFramework__xAODStringSkimmingTool
BPHY13_SelectEvent = DerivationFramework__xAODStringSkimmingTool(name = "BPHY13_SelectEvent",
                                                                 expression = expression)
ToolSvc += BPHY13_SelectEvent
print BPHY13_SelectEvent

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


# The name of the kernel (BPHY13Kernel in this case) must be unique to this derivation
from DerivationFrameworkCore.DerivationFrameworkCoreConf import DerivationFramework__DerivationKernel
DerivationFrameworkJob += CfgMgr.DerivationFramework__DerivationKernel(
  "BPHY13Kernel",
   AugmentationTools = [BPHY13_Reco_mumu, BPHY13FourTrackSelectAndWrite, BPHY13_Select_FourTrack, BPHY13TrackIsolationDecorator, BPHY13_Revertex_2mu, BPHY13_Select_TwoMuon, BPHY13_Revertex_2trk, BPHY13_Select_TwoTrack, BPHY13_Revertex_2muHi, BPHY13_Select_TwoMuonHi, BPHY13_Revertex_2trkHi, BPHY13_Select_TwoTrackHi, BPHY13_Revertex_2muMed, BPHY13_Select_TwoMuonMed, BPHY13_Revertex_2trkMed, BPHY13_Select_TwoTrackMed],
   SkimmingTools     = [BPHY13_SelectEvent]
   )

#====================================================================
# SET UP STREAM   
#====================================================================
streamName = derivationFlags.WriteDAOD_BPHY13Stream.StreamName
fileName   = buildFileName( derivationFlags.WriteDAOD_BPHY13Stream )
BPHY13Stream = MSMgr.NewPoolRootStream( streamName, fileName )
BPHY13Stream.AcceptAlgs(["BPHY13Kernel"])
# Special lines for thinning
# Thinning service name must match the one passed to the thinning tools
from AthenaServices.Configurables import ThinningSvc, createThinningSvc
augStream = MSMgr.GetStream( streamName )
evtStream = augStream.GetEventStream()
svcMgr += createThinningSvc( svcName="BPHY13ThinningSvc", outStreams=[evtStream] )


#====================================================================
# Slimming 
#====================================================================

from DerivationFrameworkCore.SlimmingHelper import SlimmingHelper
BPHY13SlimmingHelper = SlimmingHelper("BPHY13SlimmingHelper")
BPHY13_AllVariables = []
BPHY13_StaticContent = []

# Needed for trigger objects
BPHY13SlimmingHelper.IncludeMuonTriggerContent = True
BPHY13SlimmingHelper.IncludeBPhysTriggerContent = True

## primary vertices
BPHY13_AllVariables += ["PrimaryVertices"]
BPHY13_StaticContent += ["xAOD::VertexContainer#BPHY13RefittedPrimaryVertices"]
BPHY13_StaticContent += ["xAOD::VertexAuxContainer#BPHY13RefittedPrimaryVerticesAux."]

## ID track particles
BPHY13_AllVariables += ["InDetTrackParticles"]

## combined / extrapolated muon track particles 
## (note: for tagged muons there is no extra TrackParticle collection since the ID tracks
##        are store in InDetTrackParticles collection)
BPHY13_AllVariables += ["CombinedMuonTrackParticles"]
BPHY13_AllVariables += ["ExtrapolatedMuonTrackParticles"]

## muon container
BPHY13_AllVariables += ["Muons"]


BPHY13_StaticContent += ["xAOD::VertexContainer#%s"        % BPHY13FourTrackSelectAndWrite.OutputVtxContainerName]
BPHY13_StaticContent += ["xAOD::VertexAuxContainer#%sAux." % BPHY13FourTrackSelectAndWrite.OutputVtxContainerName]
## we have to disable vxTrackAtVertex branch since it is not xAOD compatible
BPHY13_StaticContent += ["xAOD::VertexAuxContainer#%sAux.-vxTrackAtVertex" % BPHY13FourTrackSelectAndWrite.OutputVtxContainerName]

BPHY13_StaticContent += ["xAOD::VertexContainer#%s"        % BPHY13_Revertex_2mu.OutputVtxContainerName]
BPHY13_StaticContent += ["xAOD::VertexAuxContainer#%sAux." % BPHY13_Revertex_2mu.OutputVtxContainerName]
## we have to disable vxTrackAtVertex branch since it is not xAOD compatible
BPHY13_StaticContent += ["xAOD::VertexAuxContainer#%sAux.-vxTrackAtVertex" % BPHY13_Revertex_2mu.OutputVtxContainerName]

BPHY13_StaticContent += ["xAOD::VertexContainer#%s"        % BPHY13_Revertex_2trk.OutputVtxContainerName]
BPHY13_StaticContent += ["xAOD::VertexAuxContainer#%sAux." % BPHY13_Revertex_2trk.OutputVtxContainerName]
## we have to disable vxTrackAtVertex branch since it is not xAOD compatible
BPHY13_StaticContent += ["xAOD::VertexAuxContainer#%sAux.-vxTrackAtVertex" % BPHY13_Revertex_2trk.OutputVtxContainerName]

BPHY13_StaticContent += ["xAOD::VertexContainer#%s"        % BPHY13_Revertex_2muHi.OutputVtxContainerName]
BPHY13_StaticContent += ["xAOD::VertexAuxContainer#%sAux." % BPHY13_Revertex_2muHi.OutputVtxContainerName]
## we have to disable vxTrackAtVertex branch since it is not xAOD compatible
BPHY13_StaticContent += ["xAOD::VertexAuxContainer#%sAux.-vxTrackAtVertex" % BPHY13_Revertex_2muHi.OutputVtxContainerName]

BPHY13_StaticContent += ["xAOD::VertexContainer#%s"        % BPHY13_Revertex_2trkHi.OutputVtxContainerName]
BPHY13_StaticContent += ["xAOD::VertexAuxContainer#%sAux." % BPHY13_Revertex_2trkHi.OutputVtxContainerName]
## we have to disable vxTrackAtVertex branch since it is not xAOD compatible
BPHY13_StaticContent += ["xAOD::VertexAuxContainer#%sAux.-vxTrackAtVertex" % BPHY13_Revertex_2trkHi.OutputVtxContainerName]

BPHY13_StaticContent += ["xAOD::VertexContainer#%s"        % BPHY13_Revertex_2muMed.OutputVtxContainerName]
BPHY13_StaticContent += ["xAOD::VertexAuxContainer#%sAux." % BPHY13_Revertex_2muMed.OutputVtxContainerName]
## we have to disable vxTrackAtVertex branch since it is not xAOD compatible
BPHY13_StaticContent += ["xAOD::VertexAuxContainer#%sAux.-vxTrackAtVertex" % BPHY13_Revertex_2muMed.OutputVtxContainerName]

BPHY13_StaticContent += ["xAOD::VertexContainer#%s"        % BPHY13_Revertex_2trkMed.OutputVtxContainerName]
BPHY13_StaticContent += ["xAOD::VertexAuxContainer#%sAux." % BPHY13_Revertex_2trkMed.OutputVtxContainerName]
## we have to disable vxTrackAtVertex branch since it is not xAOD compatible
BPHY13_StaticContent += ["xAOD::VertexAuxContainer#%sAux.-vxTrackAtVertex" % BPHY13_Revertex_2trkMed.OutputVtxContainerName]


# Truth information for MC only
if isSimulation:
    BPHY13_AllVariables += ["TruthEvents","TruthParticles","TruthVertices","MuonTruthParticles"]

BPHY13SlimmingHelper.AllVariables = BPHY13_AllVariables
BPHY13SlimmingHelper.StaticContent = BPHY13_StaticContent
BPHY13SlimmingHelper.AppendContentToStream(BPHY13Stream)

