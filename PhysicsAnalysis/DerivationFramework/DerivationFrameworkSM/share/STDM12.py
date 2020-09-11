#====================================================================
# STDM12.py
# J/psi, Upsilon, Bpm with jets and photons (J. Llorente)
# It requires the reductionConf flag STDM12 in Reco_tf.py   
#====================================================================

# Set up common services and job object. 
# This should appear in ALL derivation job options
from DerivationFrameworkCore.DerivationFrameworkMaster import *

isSimulation = False
if globalflags.DataSource()=='geant4':
    isSimulation = True

print isSimulation

# Add Truth MetaData
if DerivationFrameworkHasTruth:
    from DerivationFrameworkMCTruth.MCTruthCommon import *


#====================================================================
# AUGMENTATION TOOLS 
#====================================================================
## 1/ setup vertexing tools and services
#include( "JpsiUpsilonTools/configureServices.py" )

include("DerivationFrameworkBPhys/configureVertexing.py")
STDM12_VertexTools = BPHYVertexTools("STDM12")


from DerivationFrameworkBPhys.DerivationFrameworkBPhysConf import DerivationFramework__AugOriginalCounts
STDM12_AugOriginalCounts = DerivationFramework__AugOriginalCounts(
   name = "STDM12_AugOriginalCounts",
   VertexContainer = "PrimaryVertices",
   TrackContainer = "InDetTrackParticles" )
ToolSvc += STDM12_AugOriginalCounts


#--------------------------------------------------------------------
## 2/ setup JpsiFinder tool
##    These are general tools independent of DerivationFramework that do the 
##    actual vertex fitting and some pre-selection.
from JpsiUpsilonTools.JpsiUpsilonToolsConf import Analysis__JpsiFinder
STDM12JpsiFinder = Analysis__JpsiFinder(name                         = "STDM12JpsiFinder",
                                        OutputLevel                 = INFO,
                                        muAndMu                     = True,
                                        muAndTrack                  = False,
                                        TrackAndTrack               = False,
                                        assumeDiMuons               = True, 
                                        invMassUpper                = 100000.0,
                                        invMassLower                = 0.0,
                                        Chi2Cut                     = 30.,
                                        oppChargesOnly	            = True,
                                        combOnly		            = True,
                                        atLeastOneComb              = False,
                                        useCombinedMeasurement      = False, # Only takes effect if combOnly=True	
                                        muonCollectionKey           = "Muons",
                                        TrackParticleCollection     = "InDetTrackParticles",
                                        V0VertexFitterTool          = STDM12_VertexTools.TrkV0Fitter,             # V0 vertex fitter
                                        useV0Fitter                 = False,                   # if False a TrkVertexFitterTool will be used
                                        TrkVertexFitterTool         = STDM12_VertexTools.TrkVKalVrtFitter,        # VKalVrt vertex fitter
                                        TrackSelectorTool           = STDM12_VertexTools.InDetTrackSelectorTool,
                                        ConversionFinderHelperTool  = STDM12_VertexTools.InDetConversionHelper,
                                        VertexPointEstimator        = STDM12_VertexTools.VtxPointEstimator,
                                        useMCPCuts                  = False)
ToolSvc += STDM12JpsiFinder
print      STDM12JpsiFinder

#--------------------------------------------------------------------
## 3/ setup the vertex reconstruction "call" tool(s). They are part of the derivation framework.
##    These Augmentation tools add output vertex collection(s) into the StoreGate and add basic 
##    decorations which do not depend on the vertex mass hypothesis (e.g. lxy, ptError, etc).
##    There should be one tool per topology, i.e. Jpsi and Psi(2S) do not need two instance of the
##    Reco tool is the JpsiFinder mass window is wide enough.
from DerivationFrameworkBPhys.DerivationFrameworkBPhysConf import DerivationFramework__Reco_mumu
STDM12JpsiSelectAndWrite = DerivationFramework__Reco_mumu(name                 = "STDM12JpsiSelectAndWrite",
                                                       JpsiFinder             = STDM12JpsiFinder,
                                                       OutputVtxContainerName = "STDM12OniaCandidates",
                                                       PVContainerName        = "PrimaryVertices",
                                                       RefPVContainerName     = "SHOULDNOTBEUSED",
                                                       DoVertexType           =1)
ToolSvc += STDM12JpsiSelectAndWrite
print STDM12JpsiSelectAndWrite


from DerivationFrameworkBPhys.DerivationFrameworkBPhysConf import DerivationFramework__Select_onia2mumu

## a/ augment and select Jpsi->mumu candidates
STDM12_Select_Jpsi2mumu = DerivationFramework__Select_onia2mumu(
  name                  = "STDM12_Select_Jpsi2mumu",
  HypothesisName        = "Jpsi",
  InputVtxContainerName = "STDM12OniaCandidates",
  VtxMassHypo           = 3096.916,
  MassMin               = 2000.0,
  MassMax               = 3600.0,
  Chi2Max               = 200, Do3d = False,
  DoVertexType =1)

  
ToolSvc += STDM12_Select_Jpsi2mumu
print      STDM12_Select_Jpsi2mumu

## b/ augment and select Upsilon(nS)->mumu candidates
STDM12_Select_Upsi2mumu = DerivationFramework__Select_onia2mumu(
  name                  = "STDM12_Select_Upsi2mumu",
  HypothesisName        = "Upsi",
  InputVtxContainerName = "STDM12OniaCandidates",
  VtxMassHypo           = 9460.30,
  MassMin               = 7000.0,
  MassMax               = 12500.0,
  Chi2Max               = 200,
  DoVertexType          = 7)

ToolSvc += STDM12_Select_Upsi2mumu
print STDM12_Select_Upsi2mumu

## c/augment and select Phi->mumu candidates
STDM12_Select_Phi2mumu = DerivationFramework__Select_onia2mumu(
  name                  = "STDM12_Select_Phi2mumu",
  HypothesisName        = "Phi",
  InputVtxContainerName = "STDM12OniaCandidates",
  VtxMassHypo           = 1019.455,
  MassMin               = 1019.455-100.,
  MassMax               = 1019.455+100.,
  Chi2Max               = 200, Do3d = False,
  DoVertexType =1)

ToolSvc += STDM12_Select_Phi2mumu
print      STDM12_Select_Phi2mumu

## 4/ setup a new vertexing tool (necessary due to use of mass constraint) 
from TrkVKalVrtFitter.TrkVKalVrtFitterConf import Trk__TrkVKalVrtFitter
BplKplVertexFit = Trk__TrkVKalVrtFitter(
                                         name                = "BplKplVertexFit",
                                         Extrapolator        = STDM12_VertexTools.InDetExtrapolator,
                                         FirstMeasuredPoint  = True,
                                         MakeExtendedVertex  = True)
ToolSvc += BplKplVertexFit
print      BplKplVertexFit

## 5a/ setup the Jpsi+1 track finder
from JpsiUpsilonTools.JpsiUpsilonToolsConf import Analysis__JpsiPlus1Track
STDM12BplJpsiKpl = Analysis__JpsiPlus1Track(name = "STDM12BplJpsiKpl",
OutputLevel             = INFO,#DEBUG,
pionHypothesis			= True,
kaonHypothesis			= True,
trkThresholdPt			= 750.0,
trkMaxEta			= 3.0,
BThresholdPt			= 4000.0,
BMassUpper			= 7000.0,
BMassLower			= 4500.0,
Chi2Cut                         = 8.0,
TrkTrippletMassUpper            = 8000,
TrkTrippletMassLower            = 4000,
JpsiContainerKey		= "STDM12OniaCandidates",
TrackParticleCollection         = "InDetTrackParticles",
MuonsUsedInJpsi			= "Muons",
TrkVertexFitterTool		= BplKplVertexFit,
TrackSelectorTool		= STDM12_VertexTools.InDetTrackSelectorTool,
UseMassConstraint		= True,
ExcludeCrossJpsiTracks              = False,
ExcludeJpsiMuonsOnly                = True)
        
ToolSvc += STDM12BplJpsiKpl
print      STDM12BplJpsiKpl    

## 6/ setup the combined augmentation/skimming tool for the Bpm
from DerivationFrameworkBPhys.DerivationFrameworkBPhysConf import DerivationFramework__Reco_dimuTrk
STDM12BplKplSelectAndWrite = DerivationFramework__Reco_dimuTrk(name				     	= "STDM12BplKplSelectAndWrite",
															  Jpsi1PlusTrackName	    =  STDM12BplJpsiKpl,
															  OutputVtxContainerName 	= "STDM12BpmJpsiKpmCandidates",
                                                              PVContainerName           = "PrimaryVertices",
                                                              RefPVContainerName        = "STDM12RefBplJpsiKplPrimaryVertices",                                                              
                                                              RefitPV                   = True,
                                                              MaxPVrefit                = 10000 )
ToolSvc += STDM12BplKplSelectAndWrite
print      STDM12BplKplSelectAndWrite

## b/ augment and select Bs->JpsiKK candidates
STDM12_Select_Bpl2JpsiKpl     = DerivationFramework__Select_onia2mumu(
  name                       = "STDM12_Select_Bpl2JpsiKpl",
  HypothesisName             = "Bplus",
  InputVtxContainerName      = "STDM12BpmJpsiKpmCandidates",
  TrkMasses                  = [105.658, 105.658, 493.677],
  VtxMassHypo                = 5279.26,
  MassMin                    = 5279.26 - 500, Do3d = False,
  MassMax                    = 5279.26 + 500,
  Chi2Max                    = STDM12BplJpsiKpl.Chi2Cut)

ToolSvc += STDM12_Select_Bpl2JpsiKpl
print      STDM12_Select_Bpl2JpsiKpl

STDM12_Select_Bpl2JpsiPi      = DerivationFramework__Select_onia2mumu(
  name                       = "STDM12_Select_Bpl2JpsiPi",
  HypothesisName             = "Bc",
  InputVtxContainerName      = "STDM12BpmJpsiKpmCandidates",
  TrkMasses                  = [105.658, 105.658, 139.570],
  VtxMassHypo                = 6275.1, Do3d = False,
  MassMin                    = 6275.1 - 500,
  MassMax                    = 6275.1 + 500,
  Chi2Max                    = STDM12BplJpsiKpl.Chi2Cut)

ToolSvc += STDM12_Select_Bpl2JpsiPi
print      STDM12_Select_Bpl2JpsiPi

#expression = "count(STDM12BpmJpsiKpmCandidates.passed_Bplus) > 0"
#from DerivationFrameworkTools.DerivationFrameworkToolsConf import DerivationFramework__xAODStringSkimmingTool
#STDM12_SelectEvent = DerivationFramework__xAODStringSkimmingTool(name = "STDM12_SelectEvent",
#                                                                expression = expression)
#ToolSvc += STDM12_SelectEvent
#print STDM12_SelectEvent


#from DerivationFrameworkBPhys.DerivationFrameworkBPhysConf import DerivationFramework__SelectEvent

if not isSimulation: #Only Skim Data
   from DerivationFrameworkTools.DerivationFrameworkToolsConf import DerivationFramework__xAODStringSkimmingTool
   STDM12_SelectBplJpsiKplEvent = DerivationFramework__xAODStringSkimmingTool(name = "STDM12_SelectBplJpsiKplEvent",
                                                                    expression = "count(STDM12BpmJpsiKpmCandidates.passed_Bplus>0) > 0")
   ToolSvc += STDM12_SelectBplJpsiKplEvent
   print      STDM12_SelectBplJpsiKplEvent
   
   STDM12_SelectBplJpsiKplEventBc = DerivationFramework__xAODStringSkimmingTool(name = "STDM12_SelectBplJpsiKplEventBc",
                                                                    expression = "count(STDM12BpmJpsiKpmCandidates.passed_Bc>0) > 0")
   ToolSvc += STDM12_SelectBplJpsiKplEventBc
   print      STDM12_SelectBplJpsiKplEventBc

   STDM12_SelectJpsiEvent = DerivationFramework__xAODStringSkimmingTool(name = "STDM12_SelectJpsiEvent",
                                                                       expression = "count(STDM12OniaCandidates.passed_Jpsi>0) > 0")
   ToolSvc += STDM12_SelectJpsiEvent
   print      STDM12_SelectJpsiEvent
   
   STDM12_SelectUpsiEvent = DerivationFramework__xAODStringSkimmingTool(name = "STDM12_SelectUpsiEvent",
                                                                       expression = "count(STDM12OniaCandidates.passed_Upsi>0) > 0")
   ToolSvc += STDM12_SelectUpsiEvent
   print      STDM12_SelectUpsiEvent

   STDM12_SelectPhiEvent = DerivationFramework__xAODStringSkimmingTool(name = "STDM12_SelectPhiEvent",
                                                                       expression = "count(STDM12OniaCandidates.passed_Phi>0) > 0")
   ToolSvc += STDM12_SelectPhiEvent
   print      STDM12_SelectPhiEvent

   #====================================================================
   # Make event selection based on an OR of the input skimming tools
   #====================================================================
   from DerivationFrameworkTools.DerivationFrameworkToolsConf import DerivationFramework__FilterCombinationOR
   STDM12SkimmingOR = CfgMgr.DerivationFramework__FilterCombinationOR("STDM12SkimmingOR",
                                                                      FilterList = [STDM12_SelectBplJpsiKplEvent, STDM12_SelectBplJpsiKplEventBc,
                                                                                    STDM12_SelectJpsiEvent, STDM12_SelectUpsiEvent, STDM12_SelectPhiEvent])
   ToolSvc += STDM12SkimmingOR
   print      STDM12SkimmingOR

from DerivationFrameworkBPhys.DerivationFrameworkBPhysConf import DerivationFramework__Thin_vtxTrk
STDM12_thinningTool_Tracks = DerivationFramework__Thin_vtxTrk(
  name                       = "STDM12_thinningTool_Tracks",
  ThinningService            = "STDM12ThinningSvc",
  TrackParticleContainerName = "InDetTrackParticles",
  VertexContainerNames       = ["STDM12BpmJpsiKpmCandidates", "STDM12OniaCandidates"],
  PassFlags                  = ["passed_Bplus", "passed_Bc", "passed_Jpsi", "passed_Upsi", "passed_Phi"] )

ToolSvc += STDM12_thinningTool_Tracks

from DerivationFrameworkBPhys.DerivationFrameworkBPhysConf import DerivationFramework__BPhysPVThinningTool
STDM12_thinningTool_PV = DerivationFramework__BPhysPVThinningTool(
  name                       = "STDM12_thinningTool_PV",
  ThinningService            = "STDM12ThinningSvc",
  CandidateCollections       = ["STDM12BpmJpsiKpmCandidates"],
  KeepPVTracks  =True
 )

ToolSvc += STDM12_thinningTool_PV


## b) thinning out tracks that are not attached to muons. The final thinning decision is based on the OR operation
##    between decision from this and the previous tools.
from DerivationFrameworkInDet.DerivationFrameworkInDetConf import DerivationFramework__MuonTrackParticleThinning
STDM12MuonTPThinningTool = DerivationFramework__MuonTrackParticleThinning(name                    = "STDM12MuonTPThinningTool",
                                                                         ThinningService         = "STDM12ThinningSvc",
                                                                         MuonKey                 = "Muons",
                                                                         InDetTrackParticlesKey  = "InDetTrackParticles")
ToolSvc += STDM12MuonTPThinningTool

from DerivationFrameworkInDet.DerivationFrameworkInDetConf import DerivationFramework__EgammaTrackParticleThinning
STDM12ElectronTPThinningTool = DerivationFramework__EgammaTrackParticleThinning(  
    name                    = "STDM12ElectronTPThinningTool",
    ThinningService         = "STDM12ThinningSvc",
    SGKey                   = "Electrons",
    GSFTrackParticlesKey    = "GSFTrackParticles",        
    InDetTrackParticlesKey  = "InDetTrackParticles",
    SelectionString = "",
    BestMatchOnly = True,
    ConeSize = 0.3,
    ApplyAnd = False)

ToolSvc+=STDM12ElectronTPThinningTool
#====================================================================
# CREATE THE DERIVATION KERNEL ALGORITHM AND PASS THE ABOVE TOOLS  
#====================================================================

thiningCollection = [STDM12_thinningTool_Tracks,STDM12MuonTPThinningTool] 
print thiningCollection

from DerivationFrameworkJetEtMiss.JetCommon import *
stdm12Seq = CfgMgr.AthSequencer("STDM12Sequence")
DerivationFrameworkJob += stdm12Seq

# The name of the kernel (STDM12Kernel in this case) must be unique to this derivation
from DerivationFrameworkCore.DerivationFrameworkCoreConf import DerivationFramework__DerivationKernel
stdm12Seq += CfgMgr.DerivationFramework__DerivationKernel("STDM12Kernel",
                                                                       AugmentationTools = [STDM12JpsiSelectAndWrite,  STDM12_Select_Jpsi2mumu, STDM12_Select_Upsi2mumu, STDM12_Select_Phi2mumu,
                                                                                            STDM12BplKplSelectAndWrite, STDM12_Select_Bpl2JpsiKpl, STDM12_Select_Bpl2JpsiPi,
                                                                                            STDM12_AugOriginalCounts],
                                                                       #Only skim if not MC
                                                                       SkimmingTools     = [STDM12SkimmingOR] if not isSimulation else [],
                                                                       ThinningTools     = thiningCollection
                                                                       
                                                                       )

#====================================================================
# SET UP STREAM   
#====================================================================
streamName   = derivationFlags.WriteDAOD_STDM12Stream.StreamName
fileName     = buildFileName( derivationFlags.WriteDAOD_STDM12Stream )
STDM12Stream  = MSMgr.NewPoolRootStream( streamName, fileName )
STDM12Stream.AcceptAlgs(["STDM12Kernel"])

# Special lines for thinning
# Thinning service name must match the one passed to the thinning tools
from AthenaServices.Configurables import ThinningSvc, createThinningSvc
augStream = MSMgr.GetStream( streamName )
evtStream = augStream.GetEventStream()

STDM12ThinningSvc = createThinningSvc( svcName="STDM12ThinningSvc", outStreams=[evtStream] )
svcMgr += STDM12ThinningSvc

#====================================================================
# Slimming 
#====================================================================

# Added by ASC
from DerivationFrameworkCore.SlimmingHelper import SlimmingHelper
STDM12SlimmingHelper = SlimmingHelper("STDM12SlimmingHelper")
AllVariables  = []
StaticContent = []

# Needed for trigger objects
STDM12SlimmingHelper.IncludeMuonTriggerContent  = TRUE
STDM12SlimmingHelper.IncludeBPhysTriggerContent = TRUE

## primary vertices
AllVariables  += ["PrimaryVertices"]
StaticContent += ["xAOD::VertexContainer#STDM12RefittedPrimaryVertices"]
StaticContent += ["xAOD::VertexAuxContainer#STDM12RefittedPrimaryVerticesAux."]
StaticContent += ["xAOD::VertexContainer#STDM12RefBplJpsiKplPrimaryVertices"]
StaticContent += ["xAOD::VertexAuxContainer#STDM12RefBplJpsiKplPrimaryVerticesAux."]

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
StaticContent += ["xAOD::VertexContainer#%s"        %                 STDM12JpsiSelectAndWrite.OutputVtxContainerName]
## we have to disable vxTrackAtVertex branch since it is not xAOD compatible
StaticContent += ["xAOD::VertexAuxContainer#%sAux.-vxTrackAtVertex" % STDM12JpsiSelectAndWrite.OutputVtxContainerName]

StaticContent += ["xAOD::VertexContainer#%s"        %                 STDM12BplKplSelectAndWrite.OutputVtxContainerName]
StaticContent += ["xAOD::VertexAuxContainer#%sAux.-vxTrackAtVertex" % STDM12BplKplSelectAndWrite.OutputVtxContainerName]

# Tagging information (in addition to that already requested by usual algorithms)
#AllVariables += ["Electrons"] 
AllVariables += ["Photons", "MuonSpectrometerTrackParticles"]
tagJetCollections = ['AntiKt4PV0TrackJets', 'AntiKt4EMPFlowJets']

AllVariables += [ "Kt4EMPFlowEventShape" ]
SmartVar = ["Photons"] #[ tagJetCollections ]


for jet_collection in tagJetCollections:
    AllVariables   += [jet_collection]
    AllVariables   += ["BTagging_%s"       % (jet_collection[:-4]) ]
    AllVariables   += ["BTagging_%sJFVtx"  % (jet_collection[:-4]) ]
    AllVariables   += ["BTagging_%sSecVtx" % (jet_collection[:-4]) ]

#addStandardJets("AntiKt", 0.4, "PV0Track", 2000, mods="track_ungroomed", algseq=stdm12Seq, outputGroup="STDM12")


# Added by ASC
# Truth information for MC only
if isSimulation:
    AllVariables += ["TruthEvents","TruthParticles","TruthVertices","MuonTruthParticles", "egammaTruthParticles" ]
    AllVariables += ["AntiKt4TruthJets", "AntiKt4TruthWZJets" ]
#    addStandardJets("AntiKt", 0.4, "Truth", 5000, mods="truth_ungroomed", algseq=stdm12Seq, outputGroup="STDM12")
#    addStandardJets("AntiKt", 0.4, "TruthWZ", 5000, mods="truth_ungroomed", algseq=stdm12Seq, outputGroup="STDM12")
    tagJetCollections += [ "AntiKt4TruthJets", "AntiKt4TruthWZJets"  ]

from DerivationFrameworkJetEtMiss.ExtendedJetCommon import replaceAODReducedJets
replaceAODReducedJets(tagJetCollections, stdm12Seq  ,  "STDM12" )


AllVariables = list(set(AllVariables)) # remove duplicates

STDM12SlimmingHelper.AllVariables = AllVariables
STDM12SlimmingHelper.StaticContent = StaticContent
STDM12SlimmingHelper.SmartCollections = SmartVar

STDM12SlimmingHelper.AppendContentToStream(STDM12Stream)


