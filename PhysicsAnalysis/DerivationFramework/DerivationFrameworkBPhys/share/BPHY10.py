#====================================================================
# BPHY10.py
# Bs>J/psiKK 
# It requires the reductionConf flag BPHY10 in Reco_tf.py   
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
BPHY10_VertexTools = BPHYVertexTools("BPHY10")


from DerivationFrameworkBPhys.DerivationFrameworkBPhysConf import DerivationFramework__AugOriginalCounts
BPHY10_AugOriginalCounts = DerivationFramework__AugOriginalCounts(
   name = "BPHY10_AugOriginalCounts",
   VertexContainer = "PrimaryVertices",
   TrackContainer = "InDetTrackParticles" )
ToolSvc += BPHY10_AugOriginalCounts


#--------------------------------------------------------------------
## 2/ setup JpsiFinder tool
##    These are general tools independent of DerivationFramework that do the 
##    actual vertex fitting and some pre-selection.
from JpsiUpsilonTools.JpsiUpsilonToolsConf import Analysis__JpsiFinder
BPHY10JpsiFinder = Analysis__JpsiFinder(
    name                        = "BPHY10JpsiFinder",
    OutputLevel                 = INFO,
    muAndMu                     = True,
    muAndTrack                  = False,
    TrackAndTrack               = False,
    assumeDiMuons               = True, 
    invMassUpper                = 3600.0,
    invMassLower                = 2600.0,
    Chi2Cut                     = 30.,
    oppChargesOnly	            = True,
    combOnly		    = True,
    atLeastOneComb              = False,
    useCombinedMeasurement      = False, # Only takes effect if combOnly=True	
    muonCollectionKey           = "Muons",
    TrackParticleCollection     = "InDetTrackParticles",
    V0VertexFitterTool          = BPHY10_VertexTools.TrkV0Fitter,             # V0 vertex fitter
    useV0Fitter                 = False,                   # if False a TrkVertexFitterTool will be used
    TrkVertexFitterTool         = BPHY10_VertexTools.TrkVKalVrtFitter,        # VKalVrt vertex fitter
    TrackSelectorTool           = BPHY10_VertexTools.InDetTrackSelectorTool,
    ConversionFinderHelperTool  = BPHY10_VertexTools.InDetConversionHelper,
    VertexPointEstimator        = BPHY10_VertexTools.VtxPointEstimator,
    useMCPCuts                  = False)

ToolSvc += BPHY10JpsiFinder
print      BPHY10JpsiFinder

#--------------------------------------------------------------------
## 3/ setup the vertex reconstruction "call" tool(s). They are part of the derivation framework.
##    These Augmentation tools add output vertex collection(s) into the StoreGate and add basic 
##    decorations which do not depend on the vertex mass hypothesis (e.g. lxy, ptError, etc).
##    There should be one tool per topology, i.e. Jpsi and Psi(2S) do not need two instance of the
##    Reco tool is the JpsiFinder mass window is wide enough.
from DerivationFrameworkBPhys.DerivationFrameworkBPhysConf import DerivationFramework__Reco_mumu
BPHY10JpsiSelectAndWrite   = DerivationFramework__Reco_mumu(
    name                   = "BPHY10JpsiSelectAndWrite",
    JpsiFinder             = BPHY10JpsiFinder,
    OutputVtxContainerName = "BPHY10JpsiCandidates",
    PVContainerName        = "PrimaryVertices",
    RefPVContainerName     = "SHOULDNOTBEUSED",
    #    RefPVContainerName     = "BPHY10RefJpsiPrimaryVertices",
    #    RefitPV                = True,
    #    MaxPVrefit             = 10000,
    DoVertexType = 1)

ToolSvc += BPHY10JpsiSelectAndWrite
print BPHY10JpsiSelectAndWrite

from DerivationFrameworkBPhys.DerivationFrameworkBPhysConf import DerivationFramework__Select_onia2mumu

## a/ augment and select Jpsi->mumu candidates
BPHY10_Select_Jpsi2mumu = DerivationFramework__Select_onia2mumu(
  name                  = "BPHY10_Select_Jpsi2mumu",
  HypothesisName        = "Jpsi",
  InputVtxContainerName = "BPHY10JpsiCandidates",
  VtxMassHypo           = 3096.916,
  MassMin               = 2000.0,
  MassMax               = 3600.0,
  Chi2Max               = 200,
  DoVertexType =1)

  
ToolSvc += BPHY10_Select_Jpsi2mumu
print      BPHY10_Select_Jpsi2mumu


## 4/ setup a new vertexing tool (necessary due to use of mass constraint) 
from TrkVKalVrtFitter.TrkVKalVrtFitterConf import Trk__TrkVKalVrtFitter
BdKstVertexFit = Trk__TrkVKalVrtFitter(
    name                = "BdKstVertexFit",
    Extrapolator        = BPHY10_VertexTools.InDetExtrapolator,
    FirstMeasuredPoint  = True,
    MakeExtendedVertex  = True)

ToolSvc += BdKstVertexFit
print      BdKstVertexFit

## 5/ setup the Jpsi+2 track finder
from JpsiUpsilonTools.JpsiUpsilonToolsConf import Analysis__JpsiPlus2Tracks
BPHY10BdJpsiKst = Analysis__JpsiPlus2Tracks(
    name                    = "BPHY10BdJpsiKst",
    OutputLevel             = INFO,
    kaonkaonHypothesis	    = False,
    pionpionHypothesis      = False,
    kaonpionHypothesis      = True,
    trkThresholdPt          = 700.0,
    trkMaxEta		    = 3.0,
    BThresholdPt            = 9000.,
    BMassUpper		    = 5600.0,
    BMassLower              = 4900.0,
    JpsiContainerKey	    = "BPHY10JpsiCandidates",
    TrackParticleCollection = "InDetTrackParticles",
    #MuonsUsedInJpsi	    = "Muons", #Don't remove all muons, just those in J/psi candidate (see the following cut)
    ExcludeCrossJpsiTracks  = False,   #setting this to False rejects the muons from J/psi candidate
    TrkVertexFitterTool	    = BdKstVertexFit,
    TrackSelectorTool	    = BPHY10_VertexTools.InDetTrackSelectorTool,
    UseMassConstraint	    = True,
    #DiTrackMassUpper        = 1500.,
    #DiTrackMassLower        = 500.,
    Chi2Cut                 = 5.0,
    DiTrackPt               = 1000.,
    TrkQuadrupletMassUpper  = 6000.0,
    TrkQuadrupletMassLower  = 4500.0,
    #FinalDiTrackMassUpper   = 1000.,
    #FinalDiTrackMassLower   = 800.,
    #TrkDeltaZ               = 20., #Normally, this cut should not be used since it is lifetime-dependent
    FinalDiTrackPt          = 1800.
    )

ToolSvc += BPHY10BdJpsiKst
print      BPHY10BdJpsiKst   


## 6/ setup the combined augmentation/skimming tool for the BdKst
from DerivationFrameworkBPhys.DerivationFrameworkBPhysConf import DerivationFramework__Reco_dimuTrkTrk	
BPHY10BdKstSelectAndWrite  = DerivationFramework__Reco_dimuTrkTrk(
    name                   = "BPHY10BdKstSelectAndWrite",
    Jpsi2PlusTrackName     = BPHY10BdJpsiKst,
    OutputVtxContainerName = "BdJpsiKstCandidates",
    PVContainerName        = "PrimaryVertices",
    RefPVContainerName     = "BPHY10RefittedPrimaryVertices",
    RefitPV                = True,
    MaxPVrefit             = 10000,
    DoVertexType = 7)

ToolSvc += BPHY10BdKstSelectAndWrite 
print      BPHY10BdKstSelectAndWrite

## b/ augment and select Bd->JpsiKst candidates
# We set just one mass hypothesis (K pi). The other hypothesis (pi K) should be considered
# at the analysis stage
BPHY10_Select_Bd2JpsiKst = DerivationFramework__Select_onia2mumu(
    name                       = "BPHY10_Select_Bd2JpsiKst",
    HypothesisName             = "Bd",
    InputVtxContainerName      = "BdJpsiKstCandidates",
    TrkMasses                  = [105.658, 105.658, 493.677, 139.570],
    VtxMassHypo                = 5279.6,
    MassMin                    = 100.0,      #no mass cuts here
    MassMax                    = 100000.0,   #no mass cuts here
    Chi2Max                    = 200)

ToolSvc += BPHY10_Select_Bd2JpsiKst
print      BPHY10_Select_Bd2JpsiKst

if not isSimulation: #Only Skim Data
   from DerivationFrameworkTools.DerivationFrameworkToolsConf import DerivationFramework__xAODStringSkimmingTool
   BPHY10_SelectBdJpsiKstEvent = DerivationFramework__xAODStringSkimmingTool(
     name = "BPHY10_SelectBdJpsiKstEvent",
     expression = "count(BdJpsiKstCandidates.passed_Bd > 0) > 0")
   
   ToolSvc += BPHY10_SelectBdJpsiKstEvent
   print BPHY10_SelectBdJpsiKstEvent

   #====================================================================
   # Make event selection based on an OR of the input skimming tools
   #====================================================================
      
   from DerivationFrameworkTools.DerivationFrameworkToolsConf import DerivationFramework__FilterCombinationOR
   BPHY10SkimmingOR = CfgMgr.DerivationFramework__FilterCombinationOR(
       "BPHY10SkimmingOR",
       FilterList = [BPHY10_SelectBdJpsiKstEvent],)
   ToolSvc += BPHY10SkimmingOR
   print      BPHY10SkimmingOR

from DerivationFrameworkBPhys.DerivationFrameworkBPhysConf import DerivationFramework__Thin_vtxTrk
BPHY10_thinningTool_Tracks = DerivationFramework__Thin_vtxTrk(
  name                       = "BPHY10_thinningTool_Tracks",
  ThinningService            = "BPHY10ThinningSvc",
  TrackParticleContainerName = "InDetTrackParticles",
  VertexContainerNames       = ["BdJpsiKstCandidates"],
  PassFlags                  = ["passed_Bd"] )

ToolSvc += BPHY10_thinningTool_Tracks

from DerivationFrameworkBPhys.DerivationFrameworkBPhysConf import DerivationFramework__BPhysPVThinningTool
BPHY10_thinningTool_PV = DerivationFramework__BPhysPVThinningTool(
  name                       = "BPHY10_thinningTool_PV",
  ThinningService            = "BPHY10ThinningSvc",
  CandidateCollections       = ["BdJpsiKstCandidates"],
  KeepPVTracks  =True
 )

ToolSvc += BPHY10_thinningTool_PV


## b) thinning out tracks that are not attached to muons. The final thinning decision is based on the OR operation
##    between decision from this and the previous tools.
from DerivationFrameworkInDet.DerivationFrameworkInDetConf import DerivationFramework__MuonTrackParticleThinning
BPHY10MuonTPThinningTool = DerivationFramework__MuonTrackParticleThinning(
    name                    = "BPHY10MuonTPThinningTool",
    ThinningService         = "BPHY10ThinningSvc",
    MuonKey                 = "Muons",
    InDetTrackParticlesKey  = "InDetTrackParticles")
ToolSvc += BPHY10MuonTPThinningTool

from DerivationFrameworkInDet.DerivationFrameworkInDetConf import DerivationFramework__EgammaTrackParticleThinning
BPHY10ElectronTPThinningTool = DerivationFramework__EgammaTrackParticleThinning(  
    name                    = "BPHY10ElectronTPThinningTool",
    ThinningService         = "BPHY10ThinningSvc",
    SGKey                   = "Electrons",
    GSFTrackParticlesKey    = "GSFTrackParticles",        
    InDetTrackParticlesKey  = "InDetTrackParticles",
    SelectionString = "",
    BestMatchOnly = True,
    ConeSize = 0.3,
    ApplyAnd = False)

ToolSvc+=BPHY10ElectronTPThinningTool
#====================================================================
# CREATE THE DERIVATION KERNEL ALGORITHM AND PASS THE ABOVE TOOLS  
#====================================================================

thiningCollection = [] 

print thiningCollection
# The name of the kernel (BPHY10Kernel in this case) must be unique to this derivation
from DerivationFrameworkCore.DerivationFrameworkCoreConf import DerivationFramework__DerivationKernel
DerivationFrameworkJob += CfgMgr.DerivationFramework__DerivationKernel(
    "BPHY10Kernel",
    AugmentationTools = [BPHY10JpsiSelectAndWrite,  BPHY10_Select_Jpsi2mumu,
                         BPHY10BdKstSelectAndWrite, BPHY10_Select_Bd2JpsiKst,
                         BPHY10_AugOriginalCounts],
    #Only skim if not MC
    SkimmingTools     = [BPHY10SkimmingOR] if not isSimulation else [],
    ThinningTools     = thiningCollection
    )

#====================================================================
# SET UP STREAM   
#====================================================================
streamName   = derivationFlags.WriteDAOD_BPHY10Stream.StreamName
fileName     = buildFileName( derivationFlags.WriteDAOD_BPHY10Stream )
BPHY10Stream  = MSMgr.NewPoolRootStream( streamName, fileName )
BPHY10Stream.AcceptAlgs(["BPHY10Kernel"])

# Special lines for thinning
# Thinning service name must match the one passed to the thinning tools
from AthenaServices.Configurables import ThinningSvc, createThinningSvc
augStream = MSMgr.GetStream( streamName )
evtStream = augStream.GetEventStream()

BPHY10ThinningSvc = createThinningSvc( svcName="BPHY10ThinningSvc", outStreams=[evtStream] )
svcMgr += BPHY10ThinningSvc

#====================================================================
# Slimming 
#====================================================================

# Added by ASC
from DerivationFrameworkCore.SlimmingHelper import SlimmingHelper
BPHY10SlimmingHelper = SlimmingHelper("BPHY10SlimmingHelper")
AllVariables  = []
StaticContent = []

# Needed for trigger objects
BPHY10SlimmingHelper.IncludeMuonTriggerContent  = TRUE
BPHY10SlimmingHelper.IncludeBPhysTriggerContent = TRUE

## primary vertices
AllVariables  += ["PrimaryVertices"]
StaticContent += ["xAOD::VertexContainer#BPHY10RefittedPrimaryVertices"]
StaticContent += ["xAOD::VertexAuxContainer#BPHY10RefittedPrimaryVerticesAux."]

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
StaticContent += ["xAOD::VertexContainer#%s"        %                 BPHY10JpsiSelectAndWrite.OutputVtxContainerName]
## we have to disable vxTrackAtVertex branch since it is not xAOD compatible
StaticContent += ["xAOD::VertexAuxContainer#%sAux.-vxTrackAtVertex" % BPHY10JpsiSelectAndWrite.OutputVtxContainerName]

StaticContent += ["xAOD::VertexContainer#%s"        %                 BPHY10BdKstSelectAndWrite.OutputVtxContainerName]
StaticContent += ["xAOD::VertexAuxContainer#%sAux.-vxTrackAtVertex" % BPHY10BdKstSelectAndWrite.OutputVtxContainerName]

# Tagging information (in addition to that already requested by usual algorithms)
#AllVariables += ["Electrons"] 
AllVariables += ["GSFTrackParticles", "MuonSpectrometerTrackParticles" ] 


# Added by ASC
# Truth information for MC only
if isSimulation:
    AllVariables += ["TruthEvents","TruthParticles","TruthVertices","MuonTruthParticles"]

AllVariables = list(set(AllVariables)) # remove duplicates

BPHY10SlimmingHelper.AllVariables = AllVariables
BPHY10SlimmingHelper.StaticContent = StaticContent
BPHY10SlimmingHelper.SmartCollections = []

BPHY10SlimmingHelper.AppendContentToStream(BPHY10Stream)


