#2019/11/18
#====================================================================
# BPHY21.py
# W -> (J/psi + D_s) 
# It requires the reductionConf flag BPHY21 in Reco_tf.py   
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
BPHY21_VertexTools = BPHYVertexTools("BPHY21")


from DerivationFrameworkBPhys.DerivationFrameworkBPhysConf import DerivationFramework__AugOriginalCounts
BPHY21_AugOriginalCounts = DerivationFramework__AugOriginalCounts(
   name = "BPHY21_AugOriginalCounts",
   VertexContainer = "PrimaryVertices",
   TrackContainer = "InDetTrackParticles" )
ToolSvc += BPHY21_AugOriginalCounts

#====================================================================
# TriggerCounting for Kernel1 
#====================================================================
#List of trigggers to be counted 
BPHY21_triggersToMetadata= [
                "HLT_2mu10",
                "HLT_2mu10_nomucomb",
                "HLT_2mu14",
                "HLT_2mu14_nomucomb",
                "HLT_mu18_mu8noL1",
                "HLT_mu18_nomucomb_mu8noL1",
                "HLT_mu20_mu8noL1",
                "HLT_mu20_nomucomb_mu8noL1",
                "HLT_mu22_mu8noL1",
                "HLT_mu22_nomucomb_mu8noL1",
                "HLT_mu20_mu8noL1",
                "HLT_mu24_mu8noL1",
                "HLT_mu10_mu6_bJpsimumu",
                "HLT_mu22_mu8noL1_calotag_0eta010_L1MU1"
              ]


from DerivationFrameworkBPhys.DerivationFrameworkBPhysConf import DerivationFramework__TriggerCountToMetadata
BPHY21_TriggerCountToMetadata = DerivationFramework__TriggerCountToMetadata(name = "BPHY21_TriggerCount",
                                                                          TriggerList = BPHY21_triggersToMetadata,
                                                                          FolderName = "BPHY21")

ToolSvc += BPHY21_TriggerCountToMetadata

#====================================================================


#====================================================================
#====================================================================
## 1/ Setup the skimming based on triggers
##     

BPHY21_triggerList = [ 
                "HLT_2mu10",
                "HLT_2mu10_nomucomb",
                "HLT_2mu14",
                "HLT_2mu14_nomucomb",
                "HLT_mu18_mu8noL1",
                "HLT_mu18_nomucomb_mu8noL1",
                "HLT_mu20_mu8noL1",
                "HLT_mu20_nomucomb_mu8noL1",
                "HLT_mu22_mu8noL1",
                "HLT_mu22_nomucomb_mu8noL1",
                "HLT_mu20_mu8noL1",
                "HLT_mu24_mu8noL1",
                "HLT_mu10_mu6_bJpsimumu",
                "HLT_mu22_mu8noL1_calotag_0eta010_L1MU1"
               ]  

from DerivationFrameworkTools.DerivationFrameworkToolsConf import DerivationFramework__TriggerSkimmingTool
BPHY21_TriggerSkim = DerivationFramework__TriggerSkimmingTool(name = "BPHY21_TriggerSkim",
                                                            TriggerListOR = BPHY21_triggerList)

ToolSvc += BPHY21_TriggerSkim


#--------------------------------------------------------------------
# 2/ Select J/psi>mu+mu-
#--------------------------------------------------------------------
## a/ setup JpsiFinder tool
##    These are general tools independent of DerivationFramework that do the 
##    actual vertex fitting and some pre-selection.
from JpsiUpsilonTools.JpsiUpsilonToolsConf import Analysis__JpsiFinder
BPHY21_JpsiFinder = Analysis__JpsiFinder(
    name                       = "BPHY21_JpsiFinder",
    OutputLevel                = INFO,
    muAndMu                    = True,
    muAndTrack                 = False,
    TrackAndTrack              = False,
    assumeDiMuons              = True, 
    muonThresholdPt            = 2700,
    invMassUpper               = 3400.0,
    invMassLower               = 2800.0,
    Chi2Cut                    = 10.,
    oppChargesOnly             = True,
    combOnly                   = True,
    atLeastOneComb             = False,
    useCombinedMeasurement     = False, # Only takes effect if combOnly=True    
    muonCollectionKey          = "Muons",
    TrackParticleCollection    = "InDetTrackParticles",
    V0VertexFitterTool         = BPHY21_VertexTools.TrkV0Fitter,             # V0 vertex fitter
    useV0Fitter                = False,                   # if False a TrkVertexFitterTool will be used
    TrkVertexFitterTool        = BPHY21_VertexTools.TrkVKalVrtFitter,        # VKalVrt vertex fitter
    TrackSelectorTool          = BPHY21_VertexTools.InDetTrackSelectorTool,
    ConversionFinderHelperTool = BPHY21_VertexTools.InDetConversionHelper,
    VertexPointEstimator       = BPHY21_VertexTools.VtxPointEstimator,
    useMCPCuts                 = False)

ToolSvc += BPHY21_JpsiFinder
print      BPHY21_JpsiFinder

#--------------------------------------------------------------------
## b/ setup the vertex reconstruction "call" tool(s). They are part of the derivation framework.
##    These Augmentation tools add output vertex collection(s) into the StoreGate and add basic 
##    decorations which do not depend on the vertex mass hypothesis (e.g. lxy, ptError, etc).
##    There should be one tool per topology, i.e. Jpsi and Psi(2S) do not need two instance of the
##    Reco tool is the JpsiFinder mass window is wide enough.
from DerivationFrameworkBPhys.DerivationFrameworkBPhysConf import DerivationFramework__Reco_mumu
BPHY21_JpsiSelectAndWrite = DerivationFramework__Reco_mumu(
    name                   = "BPHY21_JpsiSelectAndWrite",
    JpsiFinder             = BPHY21_JpsiFinder,
    OutputVtxContainerName = "BPHY21_JpsiCandidates",
    PVContainerName        = "PrimaryVertices",
    RefPVContainerName     = "SHOULDNOTBEUSED",
    DoVertexType           = 1)

ToolSvc += BPHY21_JpsiSelectAndWrite
print BPHY21_JpsiSelectAndWrite

#--------------------------------------------------------------------
## c/ augment and select Jpsi->mumu candidates
from DerivationFrameworkBPhys.DerivationFrameworkBPhysConf import DerivationFramework__Select_onia2mumu
BPHY21_Select_Jpsi2mumu = DerivationFramework__Select_onia2mumu(
    name                  = "BPHY21_Select_Jpsi2mumu",
    HypothesisName        = "Jpsi",
    InputVtxContainerName = "BPHY21_JpsiCandidates",
    VtxMassHypo           = 3096.900,
    MassMin               = 2600.0,
    MassMax               = 3600.0,
    Chi2Max               = 200,
    LxyMin                = 0.1,
    DoVertexType          = 1)
  
ToolSvc += BPHY21_Select_Jpsi2mumu
print      BPHY21_Select_Jpsi2mumu


#--------------------------------------------------------------------

BPHY21_CascadeCollections = []


#--------------------------------------------------------------------


if not isSimulation: #Only Skim Data
   from DerivationFrameworkTools.DerivationFrameworkToolsConf import DerivationFramework__xAODStringSkimmingTool
   BPHY21_SelectJpsiEvent = DerivationFramework__xAODStringSkimmingTool(
     name = "BPHY21_SelectJpsiEvent",
     expression = "count(BPHY21_JpsiCandidates.passed_Jpsi) > 0")
   
   ToolSvc += BPHY21_SelectJpsiEvent
   print      BPHY21_SelectJpsiEvent

   #====================================================================
   # Make event selection based on an OR of the input skimming tools
   #====================================================================
      
   from DerivationFrameworkTools.DerivationFrameworkToolsConf import DerivationFramework__FilterCombinationOR
   BPHY21_SkimmingOR = CfgMgr.DerivationFramework__FilterCombinationOR(
       "BPHY21_SkimmingOR",
       FilterList = [ BPHY21_TriggerSkim, BPHY21_SelectJpsiEvent] )
   ToolSvc += BPHY21_SkimmingOR
   print      BPHY21_SkimmingOR

#--------------------------------------------------------------------
##10/ track and vertex thinning. We want to remove all reconstructed secondary vertices
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
BPHY21_thinningTool_Tracks = DerivationFramework__Thin_vtxTrk(
    name                       = "BPHY21_thinningTool_Tracks",
    ThinningService            = "BPHY21_ThinningSvc",
    TrackParticleContainerName = "InDetTrackParticles",
    VertexContainerNames       = ["BPHY21_JpsiCandidates"],
    PassFlags                  = ["passed_Jpsi"])

ToolSvc += BPHY21_thinningTool_Tracks
print      BPHY21_thinningTool_Tracks
'''
from DerivationFrameworkBPhys.DerivationFrameworkBPhysConf import DerivationFramework__BPhysPVThinningTool
BPHY21_thinningTool_PV = DerivationFramework__BPhysPVThinningTool(
    name                 = "BPHY21_thinningTool_PV",
    ThinningService      = "BPHY21_ThinningSvc",
    CandidateCollections = ["BPHY21_JpsiCandidates"],
    KeepPVTracks         = True)

ToolSvc += BPHY21_thinningTool_PV
print      BPHY21_thinningTool_PV
'''
## b) thinning out tracks that are not attached to muons. The final thinning decision is based on the OR operation
##    between decision from this and the previous tools.
from DerivationFrameworkInDet.DerivationFrameworkInDetConf import DerivationFramework__MuonTrackParticleThinning
BPHY21_MuonTPThinningTool = DerivationFramework__MuonTrackParticleThinning(
    name                   = "BPHY21_MuonTPThinningTool",
    ThinningService        = "BPHY21_ThinningSvc",
    MuonKey                = "Muons",
    InDetTrackParticlesKey = "InDetTrackParticles")

ToolSvc += BPHY21_MuonTPThinningTool
print      BPHY21_MuonTPThinningTool

#====================================================================
# CREATE THE DERIVATION KERNEL ALGORITHM AND PASS THE ABOVE TOOLS  
#====================================================================

BPHY21_thiningCollection = [] 

print BPHY21_thiningCollection

# The name of the kernel (BPHY21_Kernel in this case) must be unique to this derivation
from DerivationFrameworkCore.DerivationFrameworkCoreConf import DerivationFramework__DerivationKernel
DerivationFrameworkJob += CfgMgr.DerivationFramework__DerivationKernel(
    "BPHY21_Kernel",
    AugmentationTools = [BPHY21_JpsiSelectAndWrite, BPHY21_Select_Jpsi2mumu,
                        
                         BPHY21_AugOriginalCounts],
    #Only skim if not MC
    SkimmingTools     = [BPHY21_SkimmingOR] if not isSimulation else [],
    ThinningTools     = BPHY21_thiningCollection
    )

#====================================================================
# SET UP STREAM   
#====================================================================
BPHY21_streamName   = derivationFlags.WriteDAOD_BPHY21Stream.StreamName
BPHY21_fileName     = buildFileName( derivationFlags.WriteDAOD_BPHY21Stream )
BPHY21_Stream  = MSMgr.NewPoolRootStream( BPHY21_streamName, BPHY21_fileName )
BPHY21_Stream.AcceptAlgs(["BPHY21_Kernel"])

# Special lines for thinning
# Thinning service name must match the one passed to the thinning tools
from AthenaServices.Configurables import ThinningSvc, createThinningSvc
BPHY21_augStream = MSMgr.GetStream( BPHY21_streamName )
BPHY21_evtStream = BPHY21_augStream.GetEventStream()

BPHY21_ThinningSvc = createThinningSvc( svcName="BPHY21_ThinningSvc", outStreams=[BPHY21_evtStream] )
svcMgr += BPHY21_ThinningSvc

#====================================================================
# Slimming 
#====================================================================

# Added by ASC
from DerivationFrameworkCore.SlimmingHelper import SlimmingHelper
BPHY21_SlimmingHelper = SlimmingHelper("BPHY21_SlimmingHelper")
BPHY21_AllVariables  = []
BPHY21_StaticContent = []

# Needed for trigger objects
BPHY21_SlimmingHelper.IncludeMuonTriggerContent  = TRUE
BPHY21_SlimmingHelper.IncludeBPhysTriggerContent = TRUE

## primary vertices
BPHY21_AllVariables  += ["PrimaryVertices"]
BPHY21_StaticContent += ["xAOD::VertexContainer#BPHY21_RefittedPrimaryVertices"]
BPHY21_StaticContent += ["xAOD::VertexAuxContainer#BPHY21_RefittedPrimaryVerticesAux."]

## ID track particles
BPHY21_AllVariables += ["InDetTrackParticles"]

## combined / extrapolated muon track particles 
## (note: for tagged muons there is no extra TrackParticle collection since the ID tracks
##        are store in InDetTrackParticles collection)
BPHY21_AllVariables += ["CombinedMuonTrackParticles"]
BPHY21_AllVariables += ["ExtrapolatedMuonTrackParticles"]

## muon container
BPHY21_AllVariables += ["Muons"] 


## Jpsi candidates 
BPHY21_StaticContent += ["xAOD::VertexContainer#%s"        %                 BPHY21_JpsiSelectAndWrite.OutputVtxContainerName]
## we have to disable vxTrackAtVertex branch since it is not xAOD compatible
BPHY21_StaticContent += ["xAOD::VertexAuxContainer#%sAux.-vxTrackAtVertex" % BPHY21_JpsiSelectAndWrite.OutputVtxContainerName]


# Tagging information (in addition to that already requested by usual algorithms)
#AllVariables += ["GSFTrackParticles", "MuonSpectrometerTrackParticles" ] 

# Added by ASC
# Truth information for MC only
if isSimulation:
    BPHY21_AllVariables += ["TruthEvents","TruthParticles","TruthVertices","MuonTruthParticles"]


BPHY21_AllVariables = list(set(BPHY21_AllVariables)) # remove duplicates

BPHY21_SlimmingHelper.AllVariables = BPHY21_AllVariables
BPHY21_SlimmingHelper.StaticContent = BPHY21_StaticContent
BPHY21_SlimmingHelper.SmartCollections = []

BPHY21_SlimmingHelper.AppendContentToStream(BPHY21_Stream)

#====================================================================
# END OF BPHY21.py
#====================================================================