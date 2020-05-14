#====================================================================
# MUON2.py
# Bs>J/psiKK 
# It requires the reductionConf flag MUON2 in Reco_tf.py   
#====================================================================

# Set up common services and job object. 
# This should appear in ALL derivation job options
from DerivationFrameworkCore.DerivationFrameworkMaster import *

isSimulation = False
if globalflags.DataSource()=='geant4':
    isSimulation = True

printfunc (isSimulation)

#====================================================================
# SET UP STREAM   
#====================================================================
streamName   = derivationFlags.WriteDAOD_MUON2Stream.StreamName
fileName     = buildFileName( derivationFlags.WriteDAOD_MUON2Stream )
MUON2Stream  = MSMgr.NewPoolRootStream( streamName, fileName )
MUON2Stream.AcceptAlgs(["MUON2Kernel"])

augStream = MSMgr.GetStream( streamName )
evtStream = augStream.GetEventStream()

#====================================================================
# AUGMENTATION TOOLS 
#====================================================================
## 1/ setup vertexing tools and services
#include( "JpsiUpsilonTools/configureServices.py" )

include("DerivationFrameworkBPhys/configureVertexing.py")
MUON2_VertexTools = BPHYVertexTools("MUON2")


from DerivationFrameworkBPhys.DerivationFrameworkBPhysConf import DerivationFramework__AugOriginalCounts
MUON2_AugOriginalCounts = DerivationFramework__AugOriginalCounts(
   name = "MUON2_AugOriginalCounts",
   VertexContainer = "PrimaryVertices",
   TrackContainer = "InDetTrackParticles" )
ToolSvc += MUON2_AugOriginalCounts


#--------------------------------------------------------------------
## 2/ setup JpsiFinder tool
##    These are general tools independent of DerivationFramework that do the 
##    actual vertex fitting and some pre-selection.
from JpsiUpsilonTools.JpsiUpsilonToolsConf import Analysis__JpsiFinder
MUON2JpsiFinder = Analysis__JpsiFinder(name                         = "MUON2JpsiFinder",
                                        OutputLevel                 = INFO,
                                        muAndMu                     = True,
                                        muAndTrack                  = False,
                                        TrackAndTrack               = False,
                                        assumeDiMuons               = True, 
                                        invMassUpper                = 3600.0,
                                        invMassLower                = 2600.0,
                                        Chi2Cut                     = 30.,
                                        oppChargesOnly	            = True,
                                        combOnly		            = True,
                                        atLeastOneComb              = False,
                                        useCombinedMeasurement      = False, # Only takes effect if combOnly=True	
                                        muonCollectionKey           = "Muons",
                                        TrackParticleCollection     = "InDetTrackParticles",
                                        V0VertexFitterTool          = MUON2_VertexTools.TrkV0Fitter,             # V0 vertex fitter
                                        useV0Fitter                 = False,                   # if False a TrkVertexFitterTool will be used
                                        TrkVertexFitterTool         = MUON2_VertexTools.TrkVKalVrtFitter,        # VKalVrt vertex fitter
                                        TrackSelectorTool           = MUON2_VertexTools.InDetTrackSelectorTool,
                                        ConversionFinderHelperTool  = MUON2_VertexTools.InDetConversionHelper,
                                        VertexPointEstimator        = MUON2_VertexTools.VtxPointEstimator,
                                        useMCPCuts                  = False)
ToolSvc += MUON2JpsiFinder
printfunc (     MUON2JpsiFinder)

#--------------------------------------------------------------------
## 3/ setup the vertex reconstruction "call" tool(s). They are part of the derivation framework.
##    These Augmentation tools add output vertex collection(s) into the StoreGate and add basic 
##    decorations which do not depend on the vertex mass hypothesis (e.g. lxy, ptError, etc).
##    There should be one tool per topology, i.e. Jpsi and Psi(2S) do not need two instance of the
##    Reco tool is the JpsiFinder mass window is wide enough.
from DerivationFrameworkBPhys.DerivationFrameworkBPhysConf import DerivationFramework__Reco_mumu
MUON2JpsiSelectAndWrite = DerivationFramework__Reco_mumu(name                 = "MUON2JpsiSelectAndWrite",
                                                       JpsiFinder             = MUON2JpsiFinder,
                                                       OutputVtxContainerName = "MUON2JpsiCandidates",
                                                       PVContainerName        = "PrimaryVertices",
                                                       RefPVContainerName     = "SHOULDNOTBEUSED",
                                                       DoVertexType           =1)
ToolSvc += MUON2JpsiSelectAndWrite
printfunc (MUON2JpsiSelectAndWrite)


from DerivationFrameworkBPhys.DerivationFrameworkBPhysConf import DerivationFramework__Select_onia2mumu

## a/ augment and select Jpsi->mumu candidates
MUON2_Select_Jpsi2mumu = DerivationFramework__Select_onia2mumu(
  name                  = "MUON2_Select_Jpsi2mumu",
  HypothesisName        = "Jpsi",
  InputVtxContainerName = "MUON2JpsiCandidates",
  VtxMassHypo           = 3096.916,
  MassMin               = 2000.0,
  MassMax               = 3600.0,
  Chi2Max               = 200,
  DoVertexType =1)

  
ToolSvc += MUON2_Select_Jpsi2mumu
printfunc (     MUON2_Select_Jpsi2mumu)




## 4/ setup a new vertexing tool (necessary due to use of mass constraint) 
from TrkVKalVrtFitter.TrkVKalVrtFitterConf import Trk__TrkVKalVrtFitter
BsKKVertexFit = Trk__TrkVKalVrtFitter(
                                         name                = "BsKKVertexFit",
                                         Extrapolator        = MUON2_VertexTools.InDetExtrapolator,
                                         FirstMeasuredPoint  = True,
                                         MakeExtendedVertex  = True)
ToolSvc += BsKKVertexFit
printfunc (     BsKKVertexFit)

from TrkVKalVrtFitter.TrkVKalVrtFitterConf import Trk__TrkVKalVrtFitter
BplKplVertexFit = Trk__TrkVKalVrtFitter(
                                         name                = "BplKplVertexFit",
                                         Extrapolator        = MUON2_VertexTools.InDetExtrapolator,
                                         FirstMeasuredPoint  = True,
                                         MakeExtendedVertex  = True)
ToolSvc += BplKplVertexFit
printfunc (     BplKplVertexFit)


## 5/ setup the Jpsi+2 track finder
from JpsiUpsilonTools.JpsiUpsilonToolsConf import Analysis__JpsiPlus2Tracks
MUON2BsJpsiKK = Analysis__JpsiPlus2Tracks(name = "MUON2BsJpsiKK",
                                        OutputLevel = INFO,
kaonkaonHypothesis			= True,
pionpionHypothesis                      = False,
kaonpionHypothesis                      = False,
trkThresholdPt			        = 800.0,
trkMaxEta				    = 3.0,
BMassUpper				    = 5800.0,
BMassLower				    = 5000.0,
#DiTrackMassUpper = 1019.445 + 100.,
#DiTrackMassLower = 1019.445 - 100.,
Chi2Cut                     = 20.0,
TrkQuadrupletMassUpper      = 6000.0,
TrkQuadrupletMassLower      = 4800.0,
JpsiContainerKey		    = "MUON2JpsiCandidates",
TrackParticleCollection     = "InDetTrackParticles",
MuonsUsedInJpsi			    = "Muons",
ExcludeJpsiMuonsOnly        = True,
ExcludeCrossJpsiTracks      = False,
TrkVertexFitterTool		    = BsKKVertexFit,
TrackSelectorTool		    = MUON2_VertexTools.InDetTrackSelectorTool,
UseMassConstraint		    = True)

ToolSvc += MUON2BsJpsiKK
printfunc (     MUON2BsJpsiKK    )

## 5a/ setup the Jpsi+1 track finder
from JpsiUpsilonTools.JpsiUpsilonToolsConf import Analysis__JpsiPlus1Track
MUON2BplJpsiKpl = Analysis__JpsiPlus1Track(name = "MUON2BplJpsiKpl",
OutputLevel             = INFO,#DEBUG,
pionHypothesis			= True,
kaonHypothesis			= True,
trkThresholdPt			= 750.0,
trkMaxEta			= 3.0,
BThresholdPt			= 4000.0,
BMassUpper			= 7000.0,
BMassLower			= 4500.0,
Chi2Cut                         = 60.0,
TrkTrippletMassUpper            = 8000,
TrkTrippletMassLower            = 4000,
JpsiContainerKey		= "MUON2JpsiCandidates",
TrackParticleCollection         = "InDetTrackParticles",
MuonsUsedInJpsi			= "Muons",
TrkVertexFitterTool		= BplKplVertexFit,
TrackSelectorTool		= MUON2_VertexTools.InDetTrackSelectorTool,
UseMassConstraint		= True)
        
ToolSvc += MUON2BplJpsiKpl
printfunc (     MUON2BplJpsiKpl    )


## 6/ setup the combined augmentation/skimming tool for the Bpm
from DerivationFrameworkBPhys.DerivationFrameworkBPhysConf import DerivationFramework__Reco_dimuTrkTrk	
MUON2BsKKSelectAndWrite = DerivationFramework__Reco_dimuTrkTrk(name                 = "MUON2BsKKSelectAndWrite",
                                                           Jpsi2PlusTrackName       = MUON2BsJpsiKK,
                                                           OutputVtxContainerName   = "BsJpsiKKCandidates",
                                                           PVContainerName          = "PrimaryVertices",
                                                           RefPVContainerName       = "MUON2RefittedPrimaryVertices",
                                                           RefitPV                  = True,
                                                           MaxPVrefit               = 10000, DoVertexType = 7)
ToolSvc += MUON2BsKKSelectAndWrite 
printfunc (     MUON2BsKKSelectAndWrite)

from DerivationFrameworkBPhys.DerivationFrameworkBPhysConf import DerivationFramework__Reco_dimuTrk
MUON2BplKplSelectAndWrite = DerivationFramework__Reco_dimuTrk(name				     	= "MUON2BplKplSelectAndWrite",
															  Jpsi1PlusTrackName	    =  MUON2BplJpsiKpl,
															  OutputVtxContainerName 	= "BpmJpsiKpmCandidates",
                                                              PVContainerName           = "PrimaryVertices",
                                                              RefPVContainerName        = "MUON2RefBplJpsiKplPrimaryVertices",                                                              
                                                              RefitPV                   = True,
                                                              MaxPVrefit                = 10000 )
ToolSvc += MUON2BplKplSelectAndWrite
printfunc (     MUON2BplKplSelectAndWrite)


## b/ augment and select Bs->JpsiKK candidates
MUON2_Select_Bs2JpsiKK = DerivationFramework__Select_onia2mumu(
  name                       = "MUON2_Select_Bs2JpsiKK",
  HypothesisName             = "Bs",
  InputVtxContainerName      = "BsJpsiKKCandidates",
  TrkMasses                  = [105.658, 105.658, 493.677, 493.677],
  VtxMassHypo                = 5366.3,
  MassMin                    = 5000.0,
  MassMax                    = 5800.0,
  Chi2Max                    = 200)

ToolSvc += MUON2_Select_Bs2JpsiKK
printfunc (     MUON2_Select_Bs2JpsiKK)

MUON2_Select_Bpl2JpsiKpl     = DerivationFramework__Select_onia2mumu(
  name                       = "MUON2_Select_Bpl2JpsiKpl",
  HypothesisName             = "Bplus",
  InputVtxContainerName      = "BpmJpsiKpmCandidates",
  TrkMasses                  = [105.658, 105.658, 493.677],
  VtxMassHypo                = 5279.26,
  MassMin                    = 5279.26 - 500,
  MassMax                    = 5279.26 + 500,
  Chi2Max                    = MUON2BplJpsiKpl.Chi2Cut)

ToolSvc += MUON2_Select_Bpl2JpsiKpl
printfunc (     MUON2_Select_Bpl2JpsiKpl)

MUON2_Select_Bpl2JpsiPi      = DerivationFramework__Select_onia2mumu(
  name                       = "MUON2_Select_Bpl2JpsiPi",
  HypothesisName             = "Bc",
  InputVtxContainerName      = "BpmJpsiKpmCandidates",
  TrkMasses                  = [105.658, 105.658, 139.570],
  VtxMassHypo                = 6275.1,
  MassMin                    = 6275.1 - 500,
  MassMax                    = 6275.1 + 500,
  Chi2Max                    = MUON2BplJpsiKpl.Chi2Cut)

ToolSvc += MUON2_Select_Bpl2JpsiPi
printfunc (     MUON2_Select_Bpl2JpsiPi)

#expression = "count(BpmJpsiKpmCandidates.passed_Bplus) > 0"
#from DerivationFrameworkTools.DerivationFrameworkToolsConf import DerivationFramework__xAODStringSkimmingTool
#MUON2_SelectEvent = DerivationFramework__xAODStringSkimmingTool(name = "MUON2_SelectEvent",
#                                                                expression = expression)
#ToolSvc += MUON2_SelectEvent
#printfunc (MUON2_SelectEvent)


#from DerivationFrameworkBPhys.DerivationFrameworkBPhysConf import DerivationFramework__SelectEvent

if not isSimulation: #Only Skim Data
   from DerivationFrameworkTools.DerivationFrameworkToolsConf import DerivationFramework__xAODStringSkimmingTool
   MUON2_SelectBsJpsiKKEvent = DerivationFramework__xAODStringSkimmingTool(
     name = "MUON2_SelectBsJpsiKKEvent",
     expression = "count(BsJpsiKKCandidates.passed_Bs > 0) > 0")
                                                    
   ToolSvc += MUON2_SelectBsJpsiKKEvent
   printfunc (MUON2_SelectBsJpsiKKEvent)

   MUON2_SelectBplJpsiKplEvent = DerivationFramework__xAODStringSkimmingTool(name = "MUON2_SelectBplJpsiKplEvent",
                                                                    expression = "count(BpmJpsiKpmCandidates.passed_Bplus>0) > 0")
   ToolSvc += MUON2_SelectBplJpsiKplEvent
   printfunc (     MUON2_SelectBplJpsiKplEvent)

   MUON2_SelectBplJpsiKplEventBc = DerivationFramework__xAODStringSkimmingTool(name = "MUON2_SelectBplJpsiKplEventBc",
                                                                    expression = "count(BpmJpsiKpmCandidates.passed_Bc>0) > 0")
   ToolSvc += MUON2_SelectBplJpsiKplEventBc
   printfunc (     MUON2_SelectBplJpsiKplEventBc)
   
   #====================================================================
   # Make event selection based on an OR of the input skimming tools
   #====================================================================
   from DerivationFrameworkTools.DerivationFrameworkToolsConf import DerivationFramework__FilterCombinationOR
   MUON2SkimmingOR = CfgMgr.DerivationFramework__FilterCombinationOR("MUON2SkimmingOR",
                                                                     FilterList = [MUON2_SelectBsJpsiKKEvent, MUON2_SelectBplJpsiKplEvent, MUON2_SelectBplJpsiKplEventBc])
   ToolSvc += MUON2SkimmingOR
   printfunc (     MUON2SkimmingOR)

from DerivationFrameworkBPhys.DerivationFrameworkBPhysConf import DerivationFramework__Thin_vtxTrk
MUON2_thinningTool_Tracks = DerivationFramework__Thin_vtxTrk(
  name                       = "MUON2_thinningTool_Tracks",
  StreamName                 = streamName,
  TrackParticleContainerName = "InDetTrackParticles",
  VertexContainerNames       = ["BsJpsiKKCandidates", "BpmJpsiKpmCandidates"],
  PassFlags                  = ["passed_Bs", "passed_Bplus", "passed_Bc"] )

ToolSvc += MUON2_thinningTool_Tracks

from DerivationFrameworkBPhys.DerivationFrameworkBPhysConf import DerivationFramework__BPhysPVThinningTool
MUON2_thinningTool_PV = DerivationFramework__BPhysPVThinningTool(
  name                       = "MUON2_thinningTool_PV",
  StreamName                 = streamName,
  CandidateCollections       = ["BsJpsiKKCandidates", "BpmJpsiKpmCandidates"],
  KeepPVTracks  =True
 )

ToolSvc += MUON2_thinningTool_PV


## b) thinning out tracks that are not attached to muons. The final thinning decision is based on the OR operation
##    between decision from this and the previous tools.
from DerivationFrameworkInDet.DerivationFrameworkInDetConf import DerivationFramework__MuonTrackParticleThinning
MUON2MuonTPThinningTool = DerivationFramework__MuonTrackParticleThinning(name                    = "MUON2MuonTPThinningTool",
                                                                         StreamName              = streamName,
                                                                         MuonKey                 = "Muons",
                                                                         InDetTrackParticlesKey  = "InDetTrackParticles")
ToolSvc += MUON2MuonTPThinningTool

from DerivationFrameworkInDet.DerivationFrameworkInDetConf import DerivationFramework__EgammaTrackParticleThinning
MUON2ElectronTPThinningTool = DerivationFramework__EgammaTrackParticleThinning(  
    name                    = "MUON2ElectronTPThinningTool",
    StreamName              = streamName,
    SGKey                   = "Electrons",
    GSFTrackParticlesKey = "GSFTrackParticles",        
    InDetTrackParticlesKey  = "InDetTrackParticles",
    SelectionString = "",
    BestMatchOnly = True,
    ConeSize = 0.3)

ToolSvc+=MUON2ElectronTPThinningTool
#====================================================================
# CREATE THE DERIVATION KERNEL ALGORITHM AND PASS THE ABOVE TOOLS  
#====================================================================

thiningCollection = [] 
printfunc (thiningCollection)

from DerivationFrameworkJetEtMiss.JetCommon import *
bphy5Seq = CfgMgr.AthSequencer("MUON2Sequence")
DerivationFrameworkJob += bphy5Seq

# The name of the kernel (MUON2Kernel in this case) must be unique to this derivation
from DerivationFrameworkCore.DerivationFrameworkCoreConf import DerivationFramework__DerivationKernel
bphy5Seq += CfgMgr.DerivationFramework__DerivationKernel("MUON2Kernel",
                                                                       AugmentationTools = [MUON2JpsiSelectAndWrite,  MUON2_Select_Jpsi2mumu,
                                                                                            MUON2BsKKSelectAndWrite,  MUON2_Select_Bs2JpsiKK,
                                                                                            MUON2BplKplSelectAndWrite,MUON2_Select_Bpl2JpsiKpl, MUON2_Select_Bpl2JpsiPi,
                                                                                            MUON2_AugOriginalCounts],
                                                                       #Only skim if not MC
                                                                       SkimmingTools     = [MUON2SkimmingOR] if not isSimulation else [],
                                                                       ThinningTools     = thiningCollection
                                                                       
                                                                       )

#====================================================================
# Slimming 
#====================================================================

# Added by ASC
from DerivationFrameworkCore.SlimmingHelper import SlimmingHelper
MUON2SlimmingHelper = SlimmingHelper("MUON2SlimmingHelper")
AllVariables  = []
StaticContent = []

# Needed for trigger objects
MUON2SlimmingHelper.IncludeMuonTriggerContent  = TRUE
MUON2SlimmingHelper.IncludeBPhysTriggerContent = TRUE

## primary vertices
AllVariables  += ["PrimaryVertices"]
StaticContent += ["xAOD::VertexContainer#MUON2RefittedPrimaryVertices"]
StaticContent += ["xAOD::VertexAuxContainer#MUON2RefittedPrimaryVerticesAux."]
StaticContent += ["xAOD::VertexContainer#MUON2RefBplJpsiKplPrimaryVertices"]
StaticContent += ["xAOD::VertexAuxContainer#MUON2RefBplJpsiKplPrimaryVerticesAux."]


## combined / extrapolated muon track particles 
## (note: for tagged muons there is no extra TrackParticle collection since the ID tracks
##        are store in InDetTrackParticles collection)
AllVariables += ["CombinedMuonTrackParticles"]
AllVariables += ["ExtrapolatedMuonTrackParticles"]
AllVariables += ["MuonSegments"]

## muon container
AllVariables += ["Muons"] 


## Jpsi candidates 
StaticContent += ["xAOD::VertexContainer#%s"        %                 MUON2JpsiSelectAndWrite.OutputVtxContainerName]
## we have to disable vxTrackAtVertex branch since it is not xAOD compatible
StaticContent += ["xAOD::VertexAuxContainer#%sAux.-vxTrackAtVertex" % MUON2JpsiSelectAndWrite.OutputVtxContainerName]

StaticContent += ["xAOD::VertexContainer#%s"        %                 MUON2BsKKSelectAndWrite.OutputVtxContainerName]
StaticContent += ["xAOD::VertexAuxContainer#%sAux.-vxTrackAtVertex" % MUON2BsKKSelectAndWrite.OutputVtxContainerName]

StaticContent += ["xAOD::VertexContainer#%s"        %                 MUON2BplKplSelectAndWrite.OutputVtxContainerName]
StaticContent += ["xAOD::VertexAuxContainer#%sAux.-vxTrackAtVertex" % MUON2BplKplSelectAndWrite.OutputVtxContainerName]


# Tagging information (in addition to that already requested by usual algorithms)
#AllVariables += ["Electrons"] 
AllVariables += ["GSFTrackParticles", "Electrons" , "Photons", "MuonSpectrometerTrackParticles" ]
tagJetCollections = ['AntiKt4LCTopoJets', 'AntiKt4EMTopoJets', 'AntiKt4PV0TrackJets']

AllVariables += [ "Kt4LCTopoOriginEventShape", "Kt4EMTopoOriginEventShape" ]
SmartVar = [] #[ tagJetCollections ]
SmartVar += ["InDetTrackParticles"]

for jet_collection in tagJetCollections:
    AllVariables   += [jet_collection]
    AllVariables   += ["BTagging_%s"       % (jet_collection[:-4]) ]
    AllVariables   += ["BTagging_%sJFVtx"  % (jet_collection[:-4]) ]
    AllVariables   += ["BTagging_%sSecVtx" % (jet_collection[:-4]) ]

#addStandardJets("AntiKt", 0.4, "PV0Track", 2000, mods="track_ungroomed", algseq=bphy5Seq, outputGroup="MUON2")


# Added by ASC
# Truth information for MC only
if isSimulation:
    AllVariables += ["TruthEvents","TruthParticles","TruthVertices","MuonTruthParticles", "egammaTruthParticles" ]
    AllVariables += ["AntiKt4TruthJets", "AntiKt4TruthWZJets" ]
#    addStandardJets("AntiKt", 0.4, "Truth", 5000, mods="truth_ungroomed", algseq=bphy5Seq, outputGroup="MUON2")
#    addStandardJets("AntiKt", 0.4, "TruthWZ", 5000, mods="truth_ungroomed", algseq=bphy5Seq, outputGroup="MUON2")
    tagJetCollections += [ "AntiKt4TruthJets", "AntiKt4TruthWZJets"  ]

from DerivationFrameworkJetEtMiss.ExtendedJetCommon import replaceAODReducedJets
replaceAODReducedJets(tagJetCollections, bphy5Seq  ,  "MUON2" )


AllVariables = list(set(AllVariables)) # remove duplicates

ExtraVariables = ["InDetTrackParticles.numberOfTRTHits.numberOfTRTOutliers.numberOfTRTHoles.numberOfTRTHighThresholdHits.numberOfTRTHighThresholdHitsTotal.numberOfTRTHighThresholdOutliers.numberOfTRTDeadStraws.numberOfTRTTubeHits.numberOfTRTXenonHits.TRTTrackOccupancy.numberOfTRTSharedHits.vx.vy.vz"]

MUON2SlimmingHelper.AllVariables = AllVariables
MUON2SlimmingHelper.ExtraVariables = ExtraVariables
MUON2SlimmingHelper.StaticContent = StaticContent
MUON2SlimmingHelper.SmartCollections = SmartVar

MUON2SlimmingHelper.AppendContentToStream(MUON2Stream)


