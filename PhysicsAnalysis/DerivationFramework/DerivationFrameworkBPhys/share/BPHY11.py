#====================================================================
# BPHY11.py
# Lambda_b -> J/psi p K 
# It requires the reductionConf flag BPHY11 in Reco_tf.py   
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
BPHY11_VertexTools = BPHYVertexTools("BPHY11")


from DerivationFrameworkBPhys.DerivationFrameworkBPhysConf import DerivationFramework__AugOriginalCounts
BPHY11_AugOriginalCounts = DerivationFramework__AugOriginalCounts(
    name             = "BPHY11_AugOriginalCounts",
    VertexContainer  = "PrimaryVertices",
    TrackContainer   = "InDetTrackParticles" 
)
    
ToolSvc += BPHY11_AugOriginalCounts


#--------------------------------------------------------------------
## 2/ setup JpsiFinder tool
##    These are general tools independent of DerivationFramework that do the 
##    actual vertex fitting and some pre-selection.
from JpsiUpsilonTools.JpsiUpsilonToolsConf import Analysis__JpsiFinder
BPHY11_JpsiFinder = Analysis__JpsiFinder(
    name                        = "BPHY11_JpsiFinder",
    OutputLevel                 = INFO,
    muAndMu                     = True,
    muAndTrack                  = False,
    TrackAndTrack               = False,
    assumeDiMuons               = True, 
    invMassUpper                = 3600.0,
    invMassLower                = 2600.0,
    Chi2Cut                     = 30.0,
    oppChargesOnly	        = False,
    allChargeCombinations	= True,
    combOnly		        = False,
    atLeastOneComb              = True,
    useCombinedMeasurement      = False, # Only takes effect if combOnly=True	
    muonCollectionKey           = "Muons",
    TrackParticleCollection     = "InDetTrackParticles",
    V0VertexFitterTool          = BPHY11_VertexTools.TrkV0Fitter,             # V0 vertex fitter
    useV0Fitter                 = False,                   # if False a TrkVertexFitterTool will be used
    TrkVertexFitterTool         = BPHY11_VertexTools.TrkVKalVrtFitter,        # VKalVrt vertex fitter
    TrackSelectorTool           = BPHY11_VertexTools.InDetTrackSelectorTool,
    ConversionFinderHelperTool  = BPHY11_VertexTools.InDetConversionHelper,
    VertexPointEstimator        = BPHY11_VertexTools.VtxPointEstimator,
    useMCPCuts                  = False
)

ToolSvc += BPHY11_JpsiFinder
print      BPHY11_JpsiFinder

#--------------------------------------------------------------------
## 3/ setup the vertex reconstruction "call" tool(s). They are part of the derivation framework.
##    These Augmentation tools add output vertex collection(s) into the StoreGate and add basic 
##    decorations which do not depend on the vertex mass hypothesis (e.g. lxy, ptError, etc).
##    There should be one tool per topology, i.e. Jpsi and Psi(2S) do not need two instance of the
##    Reco tool is the JpsiFinder mass window is wide enough.
from DerivationFrameworkBPhys.DerivationFrameworkBPhysConf import DerivationFramework__Reco_mumu
BPHY11_JpsiSelectAndWrite = DerivationFramework__Reco_mumu(
    name                   = "BPHY11_JpsiSelectAndWrite",
    JpsiFinder             = BPHY11_JpsiFinder,
    OutputVtxContainerName = "BPHY11_JpsiCandidates",
    PVContainerName        = "PrimaryVertices",
    RefPVContainerName     = "SHOULDNOTBEUSED",
    DoVertexType           = 1
)

ToolSvc += BPHY11_JpsiSelectAndWrite
print BPHY11_JpsiSelectAndWrite


from DerivationFrameworkBPhys.DerivationFrameworkBPhysConf import DerivationFramework__Select_onia2mumu

## a/ augment and select Jpsi->mumu candidates
BPHY11_Select_Jpsi2mumu = DerivationFramework__Select_onia2mumu(
  name                  = "BPHY11_Select_Jpsi2mumu",
  HypothesisName        = "Jpsi",
  InputVtxContainerName = "BPHY11_JpsiCandidates",
  VtxMassHypo           = 3096.900,
  MassMin               = 2600.0,
  MassMax               = 3600.0,
  Chi2Max               = 30.0,
  DoVertexType          = 1
)

ToolSvc += BPHY11_Select_Jpsi2mumu
print      BPHY11_Select_Jpsi2mumu


## 4/ setup a new vertexing tool (necessary due to use of mass constraint) 
from TrkVKalVrtFitter.TrkVKalVrtFitterConf import Trk__TrkVKalVrtFitter
LbJpsipKVertexFit = Trk__TrkVKalVrtFitter(
    name                = "LbJpsipKVertexFit",
    Extrapolator        = BPHY11_VertexTools.InDetExtrapolator,
    FirstMeasuredPoint  = False,
    MakeExtendedVertex  = True,
    usePassWithTrkErrCnst = True
)

ToolSvc += LbJpsipKVertexFit
print      LbJpsipKVertexFit


## 5/ setup the Jpsi+2 track finder
from JpsiUpsilonTools.JpsiUpsilonToolsConf import Analysis__JpsiPlus2Tracks
BPHY11_LbJpsipK = Analysis__JpsiPlus2Tracks(
  name                      = "BPHY11_LbJpsipK",
  OutputLevel               = INFO,
  kaonkaonHypothesis	    = False,
  pionpionHypothesis        = False,
  kaonpionHypothesis        = False,
  kaonprotonHypothesis      = True,
  oppChargesOnly            = False,
  trkThresholdPt	    = 1500.0,
  trkMaxEta		    = 3.0,
  BMassUpper		    = 6500.0,
  BMassLower		    = 4000.0,
#  DiTrackMassUpper          = 10000.,
  DiTrackMassLower          = 1000.,
  Chi2Cut                   = 200.0,
  TrkQuadrupletMassUpper    = 7000.0,
  TrkQuadrupletMassLower    = 4000.0,
  JpsiContainerKey	    = "BPHY11_JpsiCandidates",
  TrackParticleCollection   = "InDetTrackParticles",
  MuonsUsedInJpsi	    = "Muons",
  TrkVertexFitterTool	    = LbJpsipKVertexFit,
  TrackSelectorTool	    = BPHY11_VertexTools.InDetTrackSelectorTool,
  UseMassConstraint	    = True,
  UseVertexFittingWithPV    = True,
  VertexContainer           = "PrimaryVertices"
)
        
ToolSvc += BPHY11_LbJpsipK
print      BPHY11_LbJpsipK    

## 6/ setup the combined augmentation/skimming tool for the Bpm
from DerivationFrameworkBPhys.DerivationFrameworkBPhysConf import DerivationFramework__Reco_dimuTrkTrk	
BPHY11_LbJpsipKSelectAndWrite = DerivationFramework__Reco_dimuTrkTrk(
  name                      = "BPHY11_LbJpsipKSelectAndWrite",
  Jpsi2PlusTrackName        = BPHY11_LbJpsipK,
  OutputVtxContainerName    = "LbJpsipKCandidates",
  PVContainerName           = "PrimaryVertices",
  RefPVContainerName        = "BPHY11_RefittedPrimaryVertices",
  RefitPV                   = True,
  MaxPVrefit                = 10000, 
  DoVertexType              = 7
)

ToolSvc += BPHY11_LbJpsipKSelectAndWrite 
print      BPHY11_LbJpsipKSelectAndWrite

## b/ augment and select Lb->JpsipK candidates
BPHY11_Select_Lb2JpsipK = DerivationFramework__Select_onia2mumu(
  name                      = "BPHY11_Select_Lb2JpsipK",
  HypothesisName            = "Lb_pK",
  InputVtxContainerName     = "LbJpsipKCandidates",
  TrkMasses                 = [105.658, 105.658, 938.272, 493.677],
  VtxMassHypo               = 5619.6,
  MassMin                   = 4000.0,
  MassMax                   = 6500.0,
  Chi2Max                   = 200,
  LxyMin                    = 0.3
)

ToolSvc += BPHY11_Select_Lb2JpsipK
print      BPHY11_Select_Lb2JpsipK


## b/ augment and select Lb->JpsiKp candidates
BPHY11_Select_Lb2JpsiKp = DerivationFramework__Select_onia2mumu(
  name                      = "BPHY11_Select_Lb2JpsiKp",
  HypothesisName            = "Lb_Kp",
  InputVtxContainerName     = "LbJpsipKCandidates",
  TrkMasses                 = [105.658, 105.658, 493.677, 938.272],
  VtxMassHypo               = 5619.6,
  MassMin                   = 4000.0,
  MassMax                   = 6500.0,
  Chi2Max                   = 200.0,
  LxyMin                    = 0.3
)

ToolSvc += BPHY11_Select_Lb2JpsiKp
print      BPHY11_Select_Lb2JpsiKp

#-------------------------------------------------------
from DerivationFrameworkBPhys.DerivationFrameworkBPhysConf import DerivationFramework__ReVertex
BPHY11_LbPlusTrk             = DerivationFramework__ReVertex(
  name                       = "BPHY11_LbPlusTrk",
  InputVtxContainerName      = "LbJpsipKCandidates",
  HypothesisNames            = [ BPHY11_Select_Lb2JpsipK.HypothesisName, BPHY11_Select_Lb2JpsiKp.HypothesisName ],
  TrackIndices               = [ 0, 1, 2, 3 ],
  UseAdditionalTrack         = True,
  UseMassConstraint          = True,
  UseVertexFittingWithPV     = True,
#  VertexMass                 = 5619.6,
  SubVertexMass              = 3096.900,
  MassInputParticles         = [ 105.658, 105.658, 139.57, 139.57, 139.57 ],
  SubVertexTrackIndices      = [ 1, 2 ],
  BMassUpper                 = 10000.0,
  BMassLower                 = 4000.0,
  Chi2Cut                    = 5.0,
  TrkVertexFitterTool	     = LbJpsipKVertexFit,
  OutputVtxContainerName     = "LbJpsipKTrkCandidates"
)

ToolSvc += BPHY11_LbPlusTrk
print      BPHY11_LbPlusTrk

BPHY11_Select_LbPlusTrk     = DerivationFramework__Select_onia2mumu(
  name                      = "BPHY11_Select_LbPlusTrk",
  HypothesisName            = "LbPlusTrk",
  InputVtxContainerName     = "LbJpsipKTrkCandidates",
  TrkMasses                 = BPHY11_LbPlusTrk.MassInputParticles,
  VtxMassHypo               = 5619.6,
  MassMin                   = 4000.0,
  MassMax                   = 10000.0,
  Chi2Max                   = 50.0
)

ToolSvc += BPHY11_Select_LbPlusTrk
print      BPHY11_Select_LbPlusTrk

#-------------------------------------------------------

BPHY11_Lb_pK_ReFit           = DerivationFramework__ReVertex(
  name                       = "BPHY11_Lb_pK_ReFit",
  InputVtxContainerName      = "LbJpsipKCandidates",
  HypothesisNames            = [ BPHY11_Select_Lb2JpsipK.HypothesisName ],
  TrackIndices               = [ 0, 1, 2, 3 ],
  UseMassConstraint          = True,
  UseVertexFittingWithPV     = True,
  VertexMass                 = 5619.6,
  SubVertexMass              = 3096.900,
  MassInputParticles         = [ 105.658, 105.658, 938.272, 493.677 ],
  SubVertexTrackIndices      = [ 1, 2 ],
  TrkVertexFitterTool	     = LbJpsipKVertexFit,
  OutputVtxContainerName     = "LbJpsipKCandidatesReFit"
)

ToolSvc += BPHY11_Lb_pK_ReFit
print      BPHY11_Lb_pK_ReFit

BPHY11_Select_Lb_pK_ReFit   = DerivationFramework__Select_onia2mumu(
  name                      = "BPHY11_Select_Lb_pK_ReFit",
  HypothesisName            = "Lb_pK_ReFit",
  InputVtxContainerName     = "LbJpsipKCandidatesReFit",
  TrkMasses                 = BPHY11_Lb_pK_ReFit.MassInputParticles,
  VtxMassHypo               = 5619.6,
  MassMin                   = 0.0,
  MassMax                   = 1.0e10,
  Chi2Max                   = 1.0e10
)

ToolSvc += BPHY11_Select_Lb_pK_ReFit
print      BPHY11_Select_Lb_pK_ReFit

BPHY11_Lb_Kp_ReFit           = DerivationFramework__ReVertex(
  name                       = "BPHY11_Lb_Kp_ReFit",
  InputVtxContainerName      = "LbJpsipKCandidates",
  HypothesisNames            = [ BPHY11_Select_Lb2JpsiKp.HypothesisName ],
  TrackIndices               = [ 0, 1, 2, 3 ],
  UseMassConstraint          = True,
  UseVertexFittingWithPV     = True,
  VertexMass                 = 5619.6,
  SubVertexMass              = 3096.900,
  MassInputParticles         = [ 105.658, 105.658, 493.677, 938.272 ],
  SubVertexTrackIndices      = [ 1, 2 ],
  TrkVertexFitterTool	     = LbJpsipKVertexFit,
  OutputVtxContainerName     = "LbJpsiKpCandidatesReFit"
)

ToolSvc += BPHY11_Lb_Kp_ReFit
print      BPHY11_Lb_Kp_ReFit

BPHY11_Select_Lb_Kp_ReFit   = DerivationFramework__Select_onia2mumu(
  name                      = "BPHY11_Select_Lb_Kp_ReFit",
  HypothesisName            = "Lb_Kp_ReFit",
  InputVtxContainerName     = "LbJpsiKpCandidatesReFit",
  TrkMasses                 = BPHY11_Lb_Kp_ReFit.MassInputParticles,
  VtxMassHypo               = 5619.6,
  MassMin                   = 0.0,
  MassMax                   = 1.0e10,
  Chi2Max                   = 1.0e10
)

ToolSvc += BPHY11_Select_Lb_Kp_ReFit
print      BPHY11_Select_Lb_Kp_ReFit
  


#from DerivationFrameworkBPhys.DerivationFrameworkBPhysConf import DerivationFramework__SelectEvent

if not isSimulation: #Only Skim Data
   from DerivationFrameworkTools.DerivationFrameworkToolsConf import DerivationFramework__xAODStringSkimmingTool
   BPHY11_SelectLdJpsipKEvent = DerivationFramework__xAODStringSkimmingTool(
     name        = "BPHY11_SelectLdJpsipKEvent",
     expression  = "count(LbJpsipKCandidates.passed_Lb_pK > 0) > 0"
   )
                                                    
   ToolSvc += BPHY11_SelectLdJpsipKEvent
   print BPHY11_SelectLdJpsipKEvent

   BPHY11_SelectLdJpsiKpEvent = DerivationFramework__xAODStringSkimmingTool(
     name        = "BPHY11_SelectLdJpsiKpEvent",
     expression  = "count(LbJpsipKCandidates.passed_Lb_Kp > 0) > 0"
   )
                                                    
   ToolSvc += BPHY11_SelectLdJpsiKpEvent
   print BPHY11_SelectLdJpsiKpEvent

   #====================================================================
   # Make event selection based on an OR of the input skimming tools
   #====================================================================

   from DerivationFrameworkTools.DerivationFrameworkToolsConf import DerivationFramework__FilterCombinationOR
   BPHY11_SkimmingOR = CfgMgr.DerivationFramework__FilterCombinationOR(
     "BPHY11_SkimmingOR",
     FilterList = [BPHY11_SelectLdJpsipKEvent,BPHY11_SelectLdJpsiKpEvent],)
     
   ToolSvc += BPHY11_SkimmingOR
   print      BPHY11_SkimmingOR

from DerivationFrameworkBPhys.DerivationFrameworkBPhysConf import DerivationFramework__Thin_vtxTrk
BPHY11_thinningTool_Tracks = DerivationFramework__Thin_vtxTrk(
  name                       = "BPHY11_thinningTool_Tracks",
  ThinningService            = "BPHY11_ThinningSvc",
  TrackParticleContainerName = "InDetTrackParticles",
  VertexContainerNames       = ["LbJpsipKCandidates"],
  PassFlags                  = ["passed_Lb_pK","passed_Lb_Kp"] 
)

ToolSvc += BPHY11_thinningTool_Tracks

from DerivationFrameworkBPhys.DerivationFrameworkBPhysConf import DerivationFramework__BPhysPVThinningTool
BPHY11_thinningTool_PV = DerivationFramework__BPhysPVThinningTool(
  name                       = "BPHY11_thinningTool_PV",
  ThinningService            = "BPHY11_ThinningSvc",
  CandidateCollections       = ["LbJpsipKCandidates"],
  KeepPVTracks               = True
)

ToolSvc += BPHY11_thinningTool_PV


## b) thinning out tracks that are not attached to muons. The final thinning decision is based on the OR operation
##    between decision from this and the previous tools.
from DerivationFrameworkInDet.DerivationFrameworkInDetConf import DerivationFramework__MuonTrackParticleThinning
BPHY11_MuonTPThinningTool = DerivationFramework__MuonTrackParticleThinning(
  name                       = "BPHY11_MuonTPThinningTool",
  ThinningService            = "BPHY11_ThinningSvc",
  MuonKey                    = "Muons",
  InDetTrackParticlesKey     = "InDetTrackParticles"
)
  
ToolSvc += BPHY11_MuonTPThinningTool

from DerivationFrameworkInDet.DerivationFrameworkInDetConf import DerivationFramework__EgammaTrackParticleThinning
BPHY11_ElectronTPThinningTool = DerivationFramework__EgammaTrackParticleThinning(
  name                       = "BPHY11_ElectronTPThinningTool",
  ThinningService            = "BPHY11_ThinningSvc",
  SGKey                      = "Electrons",
  GSFTrackParticlesKey       = "GSFTrackParticles",
  InDetTrackParticlesKey     = "InDetTrackParticles",
  SelectionString            = "",
  BestMatchOnly              = True,
  ConeSize                   = 0.3,
  ApplyAnd                   = False
)

ToolSvc+=BPHY11_ElectronTPThinningTool

#====================================================================
# CREATE THE DERIVATION KERNEL ALGORITHM AND PASS THE ABOVE TOOLS  
#====================================================================

BPHY11_ThiningCollection = [BPHY11_thinningTool_Tracks, 
                            BPHY11_thinningTool_PV, 
                            BPHY11_MuonTPThinningTool, 
                            BPHY11_ElectronTPThinningTool] 
print BPHY11_ThiningCollection


# The name of the kernel (BPHY11_Kernel in this case) must be unique to this derivation
from DerivationFrameworkCore.DerivationFrameworkCoreConf import DerivationFramework__DerivationKernel
DerivationFrameworkJob += CfgMgr.DerivationFramework__DerivationKernel(
    "BPHY11_Kernel",
    AugmentationTools = [BPHY11_JpsiSelectAndWrite,     BPHY11_Select_Jpsi2mumu,
                         BPHY11_LbJpsipKSelectAndWrite, BPHY11_Select_Lb2JpsipK, BPHY11_Select_Lb2JpsiKp,
                         BPHY11_LbPlusTrk, BPHY11_Select_LbPlusTrk,
                         BPHY11_Lb_pK_ReFit, BPHY11_Select_Lb_pK_ReFit,
                         BPHY11_Lb_Kp_ReFit, BPHY11_Select_Lb_Kp_ReFit,
                         BPHY11_AugOriginalCounts],
    #Only skim if not MC
    SkimmingTools     = [BPHY11_SkimmingOR] if not isSimulation else [],
    ThinningTools     = BPHY11_ThiningCollection
)

#====================================================================
# SET UP STREAM   
#====================================================================
streamName    = derivationFlags.WriteDAOD_BPHY11Stream.StreamName
fileName      = buildFileName( derivationFlags.WriteDAOD_BPHY11Stream )
BPHY11Stream  = MSMgr.NewPoolRootStream( streamName, fileName )
BPHY11Stream.AcceptAlgs(["BPHY11_Kernel"])

# Special lines for thinning
# Thinning service name must match the one passed to the thinning tools
from AthenaServices.Configurables import ThinningSvc, createThinningSvc
augStream = MSMgr.GetStream( streamName )
evtStream = augStream.GetEventStream()

BPHY11_ThinningSvc = createThinningSvc( svcName="BPHY11_ThinningSvc", outStreams=[evtStream] )
svcMgr += BPHY11_ThinningSvc

#====================================================================
# Slimming 
#====================================================================

# Added by ASC
from DerivationFrameworkCore.SlimmingHelper import SlimmingHelper
BPHY11_SlimmingHelper = SlimmingHelper("BPHY11_SlimmingHelper")
AllVariables  = []
StaticContent = []

# Needed for trigger objects
BPHY11_SlimmingHelper.IncludeMuonTriggerContent  = TRUE
BPHY11_SlimmingHelper.IncludeBPhysTriggerContent = TRUE

## primary vertices
AllVariables  += ["PrimaryVertices"]
StaticContent += ["xAOD::VertexContainer#BPHY11_RefittedPrimaryVertices"]
StaticContent += ["xAOD::VertexAuxContainer#BPHY11_RefittedPrimaryVerticesAux."]

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
StaticContent += ["xAOD::VertexContainer#%s"        %                 BPHY11_JpsiSelectAndWrite.OutputVtxContainerName]
## we have to disable vxTrackAtVertex branch since it is not xAOD compatible
StaticContent += ["xAOD::VertexAuxContainer#%sAux.-vxTrackAtVertex" % BPHY11_JpsiSelectAndWrite.OutputVtxContainerName]

StaticContent += ["xAOD::VertexContainer#%s"        %                 BPHY11_LbJpsipKSelectAndWrite.OutputVtxContainerName]
StaticContent += ["xAOD::VertexAuxContainer#%sAux.-vxTrackAtVertex" % BPHY11_LbJpsipKSelectAndWrite.OutputVtxContainerName]

StaticContent += ["xAOD::VertexContainer#%s"        %                 BPHY11_LbPlusTrk.OutputVtxContainerName]
StaticContent += ["xAOD::VertexAuxContainer#%sAux.-vxTrackAtVertex" % BPHY11_LbPlusTrk.OutputVtxContainerName]

StaticContent += ["xAOD::VertexContainer#%s"        %                 BPHY11_Lb_pK_ReFit.OutputVtxContainerName]
StaticContent += ["xAOD::VertexAuxContainer#%sAux.-vxTrackAtVertex" % BPHY11_Lb_pK_ReFit.OutputVtxContainerName]

StaticContent += ["xAOD::VertexContainer#%s"        %                 BPHY11_Lb_Kp_ReFit.OutputVtxContainerName]
StaticContent += ["xAOD::VertexAuxContainer#%sAux.-vxTrackAtVertex" % BPHY11_Lb_Kp_ReFit.OutputVtxContainerName]



# Tagging information (in addition to that already requested by usual algorithms)
#AllVariables += ["Electrons"] 
AllVariables += ["GSFTrackParticles"] 
tagJetCollections = ['AntiKt4LCTopoJets']

for jet_collection in tagJetCollections:
    AllVariables += [jet_collection]
    AllVariables += ["BTagging_%s"       % (jet_collection[:-4]) ]
    AllVariables += ["BTagging_%sJFVtx"  % (jet_collection[:-4]) ]
    AllVariables += ["BTagging_%sSecVtx" % (jet_collection[:-4]) ]




# Added by ASC
# Truth information for MC only
if isSimulation:
    AllVariables += ["TruthEvents","TruthParticles","TruthVertices","MuonTruthParticles"]
    AllVariables += ["AntiKt4TruthJets","egammaTruthParticles"]

BPHY11_SlimmingHelper.AllVariables = AllVariables
BPHY11_SlimmingHelper.StaticContent = StaticContent
BPHY11_SlimmingHelper.SmartCollections = ["Electrons" , "Photons"]

BPHY11_SlimmingHelper.AppendContentToStream(BPHY11Stream)


