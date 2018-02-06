#====================================================================
# HION3.py
# author: Aaron Angerami <angerami@cern.ch>
# Heavily based on BPHY1
# Application: Quarkonium
# Triggers: Low pT di-muon
# Selection: Offline MB event selection
# Content: ES summary, electrons, muons and associated tracks
# Additions: Muon vertex reconstruction
# Needs HI-specific trigger info
# Needs HIEventShape summary container
#====================================================================

# Set up common services and job object.
# This should appear in ALL derivation job options
from DerivationFrameworkCore.DerivationFrameworkMaster import *
from DerivationFrameworkHI.HISkimmingTools import *
from DerivationFrameworkHI.HIDerivationFlags import HIDerivationFlags

GetConditionsFromMetaData()
isSimulation = False
if HIDerivationFlags.isSimulation() or globalflags.DataSource()=='geant4':
    isSimulation = True

print '++++++++++++++++++++++++++++++++++ Start Checking Flags +++++++++++++++++++++++++++++++++++'
print HIDerivationFlags
print globalflags
print isSimulation
print '+++++++++++++++++++++++++++++++++++ End Checking Flags +++++++++++++++++++++++++++++++++++'

#====================================================================
# AUGMENTATION TOOLS
#====================================================================
# 1/ setup vertexing tools and services
#include( "JpsiUpsilonTools/configureServices.py" )

include("DerivationFrameworkBPhys/configureVertexing.py")
HION3_VertexTools = BPHYVertexTools("HION3")

#--------------------------------------------------------------------
# 2/ Setup the vertex fitter tools (e.g. JpsiFinder, JpsiPlus1Track, etc).
#    These are general tools independent of DerivationFramework that do the
#    actual vertex fitting and some pre-selection.
from JpsiUpsilonTools.JpsiUpsilonToolsConf import Analysis__JpsiFinder
HION3JpsiFinder = Analysis__JpsiFinder(
    name                        = "HION3JpsiFinder",
    OutputLevel                 = INFO,
    muAndMu                     = True,
    muAndTrack                  = False,
    TrackAndTrack               = False,
    assumeDiMuons               = True,    # If true, will assume dimu hypothesis and use PDG value for mu mass
    invMassUpper                = 100000.0,
    invMassLower                = 0.0,
    Chi2Cut                     = 200.,
    oppChargesOnly              = True,
    atLeastOneComb              = True,
    useCombinedMeasurement      = False, # Only takes effect if combOnly=True     
    muonCollectionKey           = "Muons",
    TrackParticleCollection     = "InDetTrackParticles",
    V0VertexFitterTool          = HION3_VertexTools.TrkV0Fitter,             # V0 vertex fitter
    useV0Fitter                 = False,                   # if False a TrkVertexFitterTool will be used
    TrkVertexFitterTool         = HION3_VertexTools.TrkVKalVrtFitter,        # VKalVrt vertex fitter
    TrackSelectorTool           = HION3_VertexTools.InDetTrackSelectorTool,
    ConversionFinderHelperTool  = HION3_VertexTools.InDetConversionHelper,
    VertexPointEstimator        = HION3_VertexTools.VtxPointEstimator,
    useMCPCuts                  = False )
 
ToolSvc += HION3JpsiFinder
print      HION3JpsiFinder

#--------------------------------------------------------------------
# 3/ setup the vertex reconstruction "call" tool(s). They are part of the derivation framework.
#    These Augmentation tools add output vertex collection(s) into the StoreGate and add basic
#    decorations which do not depend on the vertex mass hypothesis (e.g. lxy, ptError, etc).
#    There should be one tool per topology, i.e. Jpsi and Psi(2S) do not need two instance of the
#    Reco tool is the JpsiFinder mass window is wide enough.

from DerivationFrameworkBPhys.DerivationFrameworkBPhysConf import DerivationFramework__Reco_mumu
HION3_Reco_mumu = DerivationFramework__Reco_mumu(
    name                   = "HION3_Reco_mumu",
    JpsiFinder             = HION3JpsiFinder,
    OutputVtxContainerName = "HION3OniaCandidates",
    PVContainerName        = "PrimaryVertices",
    RefPVContainerName     = "HION3RefittedPrimaryVertices",
    RefitPV                = True,
    MaxPVrefit             = 100000,
    DoVertexType           = 7)
 
ToolSvc += HION3_Reco_mumu
print HION3_Reco_mumu

#--------------------------------------------------------------------
# 4/ setup the vertex selection and augmentation tool(s). These tools decorate the vertices with
#    variables that depend on the vertex mass hypothesis, e.g. invariant mass, proper decay time, etc.
#    Property HypothesisName is used as a prefix for these decorations.
#    They also perform tighter selection, flagging the vertecis that passed. The flag is a Char_t branch
#    named "passed_"+HypothesisName. It is used later by the "SelectEvent" and "Thin_vtxTrk" tools
#    to determine which events and candidates should be kept in the output stream.
#    Multiple instances of the Select_* tools can be used on a single input collection as long as they
#    use different "HypothesisName" flags.

from DerivationFrameworkBPhys.DerivationFrameworkBPhysConf import DerivationFramework__Select_onia2mumu

# a/ augment and select Jpsi->mumu candidates
HION3_Select_Jpsi2mumu = DerivationFramework__Select_onia2mumu(
    name                  = "HION3_Select_Jpsi2mumu",
    HypothesisName        = "Jpsi",
    InputVtxContainerName = "HION3OniaCandidates",
    VtxMassHypo           = 3096.916,
    MassMin               = 2000.0,
    MassMax               = 3600.0,
    Chi2Max               = 200,
    DoVertexType          = 7)
 
ToolSvc += HION3_Select_Jpsi2mumu
print HION3_Select_Jpsi2mumu

# b/ augment and select Psi(2S)->mumu candidates
HION3_Select_Psi2mumu = DerivationFramework__Select_onia2mumu(
    name                  = "HION3_Select_Psi2mumu",
    HypothesisName        = "Psi",
    InputVtxContainerName = "HION3OniaCandidates",
    VtxMassHypo           = 3686.09,
    MassMin               = 3300.0,
    MassMax               = 4500.0,
    Chi2Max               = 200,
    DoVertexType          = 7)
 
ToolSvc += HION3_Select_Psi2mumu
print HION3_Select_Psi2mumu

# c/ augment and select Upsilon(nS)->mumu candidates
HION3_Select_Upsi2mumu = DerivationFramework__Select_onia2mumu(
  name                  = "HION3_Select_Upsi2mumu",
  HypothesisName        = "Upsi",
  InputVtxContainerName = "HION3OniaCandidates",
  VtxMassHypo           = 9460.30,
  MassMin               = 7000.0,
  MassMax               = 12500.0,
  Chi2Max               = 200,
  DoVertexType          = 7)
 
ToolSvc += HION3_Select_Upsi2mumu
print HION3_Select_Upsi2mumu

# d/ augment and selection Psi(2S)->Jpsipipi->mumupipi candidates
# running only in p+Pb for now
if (HIDerivationFlags.isPPb() or HIDerivationFlags.isPP()):
    from TrkVKalVrtFitter.TrkVKalVrtFitterConf import Trk__TrkVKalVrtFitter
    Psi2JpsiPiPiVertexFit = Trk__TrkVKalVrtFitter( 
        name                = "Psi2JpsiPiPiVertexFit",
        Extrapolator        = HION3_VertexTools.InDetExtrapolator,
        FirstMeasuredPoint  = True,
        MakeExtendedVertex  = True)
   
    ToolSvc += Psi2JpsiPiPiVertexFit
    print      Psi2JpsiPiPiVertexFit

# 6/ setup the Jpsi+2 track finder
if (HIDerivationFlags.isPPb() or HIDerivationFlags.isPP()):
    from JpsiUpsilonTools.JpsiUpsilonToolsConf import Analysis__JpsiPlus2Tracks
    HION3Psi2JpsiPiPi = Analysis__JpsiPlus2Tracks(name = "HION3Psi2JpsiPiPi",
        OutputLevel                 = INFO,
        kaonkaonHypothesis          = False,
        pionpionHypothesis          = True,
        kaonpionHypothesis          = False,
        trkThresholdPt              = 400.0,
        trkMaxEta                   = 2.5,
        BMassUpper                  = 4000.0,
        BMassLower                  = 3500.0,
        Chi2Cut                     = 100.0,
        TrkQuadrupletMassUpper      = 4500.0,
        TrkQuadrupletMassLower      = 3000.0,
        JpsiContainerKey                    = "HION3OniaCandidates",
        TrackParticleCollection     = "InDetTrackParticles",
        MuonsUsedInJpsi                     = "Muons",
        TrkVertexFitterTool                 = Psi2JpsiPiPiVertexFit,
        TrackSelectorTool                   = HION3_VertexTools.InDetTrackSelectorTool,
        UseMassConstraint                   = True)
       
    ToolSvc += HION3Psi2JpsiPiPi
    print      HION3Psi2JpsiPiPi

# 7/ setup the combined augmentation/skimming tool for the Bpm
if (HIDerivationFlags.isPPb() or HIDerivationFlags.isPP()):
    from DerivationFrameworkBPhys.DerivationFrameworkBPhysConf import DerivationFramework__Reco_dimuTrkTrk     
    HION3Psi2SelectAndWrite = DerivationFramework__Reco_dimuTrkTrk(
        name                   = "HION3Psi2SelectAndWrite",
        Jpsi2PlusTrackName     = HION3Psi2JpsiPiPi,
        OutputVtxContainerName = "HION3Psi2JpsiPiPiCandidates",
        PVContainerName        = "PrimaryVertices",
        RefPVContainerName     = "HION3RefittedPrimaryVertices",
        RefitPV                = True,
        MaxPVrefit             = 10000, 
        DoVertexType = 7)
    ToolSvc += HION3Psi2SelectAndWrite
    print      HION3Psi2SelectAndWrite

# b/ augment and select Psi2->JpsiPi candidates
if (HIDerivationFlags.isPPb() or HIDerivationFlags.isPP()):
    HION3_Select_Psi2JpsiPi   = DerivationFramework__Select_onia2mumu(
        name                    = "HION3_Select_Psi2JpsiPi",
        HypothesisName          = "Psi2",
        InputVtxContainerName   = "HION3Psi2JpsiPiPiCandidates",
        TrkMasses               = [105.658, 105.658, 139.570, 139.570],
        VtxMassHypo             = 3686.09,
        MassMax                 = 4000.0,
        MassMin                 = 3500.0,
        Chi2Max                 = 200)
    ToolSvc += HION3_Select_Psi2JpsiPi
    print HION3_Select_Psi2JpsiPi

#--------------------------------------------------------------------
# 8/ select the event. We only want to keep events that contain certain vertices which passed certain selection.
if (HIDerivationFlags.isPPb() or HIDerivationFlags.isPP()): 
    onia_cut = "count(HION3OniaCandidates.passed_Jpsi) > 0 || count(HION3OniaCandidates.passed_Psi) > 0 || count(HION3OniaCandidates.passed_Upsi) > 0 || count(HION3Psi2JpsiPiPiCandidates.passed_Psi2) > 0"
    MBTriggers = [ 
    	"HLT_mb_mbts_L1MBTS_1"]
    MuonTriggers = [ 
   	"L1_MU4", 
   	"L1_MU6", 
   	"HLT_mu4", 
   	"HLT_mu6", 
   	"HLT_2mu4", 
   	"HLT_mu4_mu4noL1", 
   	"HLT_2mu4_nomucomb", 
   	"HLT_mu4_bJpsi_Trkloose", 
   	"HLT_mu6_bJpsi_Trkloose" ]

else: 
    onia_cut = "count(HION3OniaCandidates.passed_Jpsi) > 0 || count(HION3OniaCandidates.passed_Psi) > 0 || count(HION3OniaCandidates.passed_Upsi) > 0"
    MBTriggers = [ 
    	"HLT_mb_mbts_L1MBTS_1"]
    MuonTriggers = [ 
   	"L1_MU4", 
   	"L1_MU6", 
   	"HLT_mu4", 
   	"HLT_mu6", 
   	"HLT_2mu4", 
   	"HLT_mu4_mu4noL1", 
   	"HLT_2mu4_nomucomb"]

triggers=MBTriggers+MuonTriggers
triggers='(' + ' || '.join(triggers) + ')'
req_total =triggers+'&&('+onia_cut+')'
if isSimulation:
    triggers=""
    req_total='('+onia_cut+')'

from DerivationFrameworkTools.DerivationFrameworkToolsConf import DerivationFramework__xAODStringSkimmingTool
HION3_SelectEvent = DerivationFramework__xAODStringSkimmingTool(
    name = "HION3_SelectEvent",
    expression = req_total)
   
print '++++++++++++++++++++++++++++++++++ Start printing event selection +++++++++++++++++++++++++++++++++++'
print req_total
print '++++++++++++++++++++++++++++++++++ End printing event selection +++++++++++++++++++++++++++++++++++'

ToolSvc += HION3_SelectEvent
print HION3_SelectEvent

#--------------------------------------------------------------------
# 5.3/ HIEventSelectorTool Selection: Event selected based in MBTS, primaryvertex and tracks.
#      Not clear when these tool should be implemented. Not applied and one should do it at later stage.
#--------------------------------------------------------------------

#====================================================================
# SET UP STREAM   
#====================================================================
streamName = derivationFlags.WriteDAOD_HION3Stream.StreamName
fileName   = buildFileName( derivationFlags.WriteDAOD_HION3Stream )
HION3Stream = MSMgr.NewPoolRootStream( streamName, fileName )
HION3Stream.AcceptAlgs(["HION3Kernel"])

# 6/ track and vertex thinning. Remove this part in order to do correlation study
#    in case of wanting to add them back, one can check BPHY.py
#    Trigger thinning has also to be defined here.
from DerivationFrameworkCore.ThinningHelper import ThinningHelper
HION3ThinningSvc = ThinningHelper( "HION3ThinningSvc" )
HION3ThinningSvc.TriggerChains = 'HLT_mu.*|L1_MU.*|HLT_2mu.*|L1_2MU.*'
HION3ThinningSvc.AppendToStream( HION3Stream  )

if not( HIDerivationFlags.isPPb() or HIDerivationFlags.isPP() ) : 
    from DerivationFrameworkBPhys.DerivationFrameworkBPhysConf import DerivationFramework__Thin_vtxTrk
    HION3Thin_vtxTrk = DerivationFramework__Thin_vtxTrk(
        name                       = "HION3Thin_vtxTrk",
        ThinningService            = HION3ThinningSvc.ThinningSvc(),
        TrackParticleContainerName = "InDetTrackParticles",
        VertexContainerNames       = ["HION3OniaCandidates"],
        PassFlags                  = ["passed_Jpsi", "passed_Psi", "passed_Upsi"] )
    ToolSvc += HION3Thin_vtxTrk

#   from DerivationFrameworkInDet.DerivationFrameworkInDetConf import DerivationFramework__MuonTrackParticleThinning
#   HION3MuonTPThinningTool = DerivationFramework__MuonTrackParticleThinning(
#       name                	= "BPHY1MuonTPThinningTool",
#       ThinningService         = HION3ThinningSvc.ThinningSvc(),
#       MuonKey                 = "Muons",
#       InDetTrackParticlesKey  = "InDetTrackParticles")
#   ToolSvc += HION3MuonTPThinningTool


# Only save truth informtion directly associated with Onia
from DerivationFrameworkMCTruth.DerivationFrameworkMCTruthConf import DerivationFramework__GenericTruthThinning
HION3TruthThinTool = DerivationFramework__GenericTruthThinning(
    name                    = "HION3TruthThinTool",
    ThinningService         = HION3ThinningSvc.ThinningSvc(),
    ParticleSelectionString = "TruthParticles.pdgId == 443 || TruthParticles.pdgId == 100443 || TruthParticles.pdgId == 553 || TruthParticles.pdgId == 100553 || TruthParticles.pdgId == 200553",
    PreserveDescendants     = True,
    PreserveAncestors       = True )
   
ToolSvc += HION3TruthThinTool
print HION3TruthThinTool

HION3ThinningTools = []
if not (HIDerivationFlags.isPPb() or HIDerivationFlags.isPP()):
    HION3ThinningTools.append(HION3Thin_vtxTrk)
if globalflags.DataSource()=='geant4':
    HION3ThinningTools.append(HION3TruthThinTool)

# The name of the kernel (HION3Kernel in this case) must be unique to this derivation
from DerivationFrameworkCore.DerivationFrameworkCoreConf import DerivationFramework__DerivationKernel
if (HIDerivationFlags.isPPb() or HIDerivationFlags.isPP()):
    DerivationFrameworkJob += CfgMgr.DerivationFramework__DerivationKernel(
        "HION3Kernel",
        AugmentationTools = [HION3_Reco_mumu, HION3_Select_Jpsi2mumu, HION3_Select_Psi2mumu, HION3_Select_Upsi2mumu, HION3Psi2SelectAndWrite, HION3_Select_Psi2JpsiPi],
        SkimmingTools     = [HION3_SelectEvent],
        ThinningTools     = HION3ThinningTools )

else:
    DerivationFrameworkJob += CfgMgr.DerivationFramework__DerivationKernel(
        "HION3Kernel",
        AugmentationTools = [HION3_Reco_mumu, HION3_Select_Jpsi2mumu, HION3_Select_Psi2mumu, HION3_Select_Upsi2mumu],
        SkimmingTools     = [HION3_SelectEvent],
        ThinningTools     = HION3ThinningTools )

#====================================================================
# Slimming
#====================================================================
from DerivationFrameworkCore.SlimmingHelper import SlimmingHelper
HION3SlimmingHelper = SlimmingHelper("HION3SlimmingHelper")
# Needed for trigger objects
HION3SlimmingHelper.IncludeMuonTriggerContent = True
HION3SlimmingHelper.IncludeBPhysTriggerContent = True

# primary vertices
HION3SlimmingHelper.AllVariables = ["Muons"]
HION3SlimmingHelper.AllVariables += ["PrimaryVertices"]
HION3SlimmingHelper.AllVariables += ["InDetTrackParticles"]
HION3SlimmingHelper.AllVariables += ["CombinedMuonTrackParticles"]
HION3SlimmingHelper.AllVariables += ["ExtrapolatedMuonTrackParticles"]
HION3SlimmingHelper.AllVariables += ["CaloSums"]
HION3SlimmingHelper.AllVariables += ["HIEventShape"]
HION3SlimmingHelper.AllVariables += ["ZdcModules"]

# Jpsi candidates
HION3SlimmingHelper.StaticContent = ["xAOD::VertexContainer#%s"        % HION3_Reco_mumu.OutputVtxContainerName]
HION3SlimmingHelper.StaticContent += ["xAOD::VertexAuxContainer#%sAux." % HION3_Reco_mumu.OutputVtxContainerName]
# we have to disable vxTrackAtVertex branch since it is not xAOD compatible
HION3SlimmingHelper.StaticContent += ["xAOD::VertexAuxContainer#%sAux.-vxTrackAtVertex" % HION3_Reco_mumu.OutputVtxContainerName]
# Psi2 candidates
if (HIDerivationFlags.isPPb() or HIDerivationFlags.isPP()):
    HION3SlimmingHelper.StaticContent += ["xAOD::VertexContainer#%s"        % HION3Psi2SelectAndWrite.OutputVtxContainerName]
    HION3SlimmingHelper.StaticContent += ["xAOD::VertexAuxContainer#%sAux." % HION3Psi2SelectAndWrite.OutputVtxContainerName]
# we have to disable vxTrackAtVertex branch since it is not xAOD compatible
    HION3SlimmingHelper.StaticContent += ["xAOD::VertexAuxContainer#%sAux.-vxTrackAtVertex" % HION3Psi2SelectAndWrite.OutputVtxContainerName]

if isSimulation:
    HION3SlimmingHelper.AllVariables += ["TruthEvents","TruthParticles","TruthVertices","MuonTruthParticles"]

HION3SlimmingHelper.AppendContentToStream(HION3Stream)
