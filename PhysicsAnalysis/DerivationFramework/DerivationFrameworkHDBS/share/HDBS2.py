#====================================================================
# HDBS2.py for JPSI filter
# This requires the reductionConf flag HDBS2 in Reco_tf.py   
#====================================================================

# Set up common services and job object. 
# This should appear in ALL derivation job options
from DerivationFrameworkCore.DerivationFrameworkMaster import *
from DerivationFrameworkJetEtMiss.JetCommon import *
from DerivationFrameworkJetEtMiss.ExtendedJetCommon import *
from DerivationFrameworkJetEtMiss.METCommon import *
from DerivationFrameworkEGamma.EGammaCommon import *
from DerivationFrameworkMuons.MuonsCommon import *
from DerivationFrameworkInDet.InDetCommon import *
if DerivationFrameworkHasTruth:
    from DerivationFrameworkMCTruth.MCTruthCommon import addStandardTruthContents,addBosonsAndDownstreamParticles
    addStandardTruthContents(DerivationFrameworkJob)
    addBosonsAndDownstreamParticles(DerivationFrameworkJob, -1)
from DerivationFrameworkCore.WeightMetadata import *
import AthenaCommon.SystemOfUnits as Units

# Add sumOfWeights metadata for LHE3 multiweights =======
from DerivationFrameworkCore.LHE3WeightMetadata import *

from AthenaCommon.Logging import logging
msg = logging.getLogger( "HDBS2" )
_info = msg.info


_info( "DerivationFrameworkHasTruth: %s", DerivationFrameworkHasTruth)
#====================================================================
# SET UP STREAM
#====================================================================
streamName = derivationFlags.WriteDAOD_HDBS2Stream.StreamName
fileName = buildFileName(derivationFlags.WriteDAOD_HDBS2Stream)
HDBS2Stream = MSMgr.NewPoolRootStream( streamName, fileName)
HDBS2Stream.AcceptAlgs(["HDBS2Kernel"])

#====================================================================
# AUGMENTATION TOOLS
# Copied from PhysicsAnalysis/DerivationFramework/DerivationFrameworkBPhys/share/BPHY1.py
#====================================================================
## 1/ setup vertexing tools and services
include("JpsiUpsilonTools/configureServices.py")

#--------------------------------------------------------------------
## 2/ Setup the vertex fitter tools (e.g. JpsiFinder, JpsiPlus1Track, etc).
##    These are general tools independent of DerivationFramework that do the
##    actual vertex fitting and some pre-selection.
from JpsiUpsilonTools.JpsiUpsilonToolsConf import Analysis__JpsiFinder
HDBS2JpsiFinder = Analysis__JpsiFinder(
    name                        = "HDBS2JpsiFinder",
    OutputLevel                 = INFO,
    muAndMu                     = True,
    muAndTrack                  = False,
    TrackAndTrack               = False,
    assumeDiMuons               = True,    # If true, will assume dimu hypothesis and use PDG value for mu mass
    invMassUpper                = 100.0*Units.GeV,
    invMassLower                = 0.0*Units.GeV,
    Chi2Cut                     = 200.,
    oppChargesOnly              = True,
    atLeastOneComb              = True,
    useCombinedMeasurement      = False, # Only takes effect if combOnly=True    
    muonCollectionKey           = "Muons",
    TrackParticleCollection     = "InDetTrackParticles",
    V0VertexFitterTool          = TrkV0Fitter,             # V0 vertex fitter
    useV0Fitter                 = False,                   # if False a TrkVertexFitterTool will be used
    TrkVertexFitterTool         = TrkVKalVrtFitter,        # VKalVrt vertex fitter
    TrackSelectorTool           = InDetTrackSelectorTool,
    ConversionFinderHelperTool  = InDetConversionHelper,
    VertexPointEstimator        = VtxPointEstimator,
    useMCPCuts                  = False)
ToolSvc += HDBS2JpsiFinder

# For phi -> K+ K-
HDBS2PhiFinder = Analysis__JpsiFinder(
    name                        = "HDBS2PhiFinder",
    OutputLevel                 = INFO,
    muAndMu                     = False,
    muAndTrack                  = False,
    TrackAndTrack               = True,
    assumeDiMuons               = False,    # If true, will assume dimu hypothesis and use PDG value for mu mass
    invMassUpper                = 1.2*Units.GeV,
    invMassLower                = 2.*0.493677*Units.GeV,
    Chi2Cut                     = 200.,
    oppChargesOnly              = True,
    atLeastOneComb              = False,
    useCombinedMeasurement      = False,
    track1Mass                  = 0.493677*Units.GeV, # charge kaon PDG mass
    track2Mass                  = 0.493677*Units.GeV, # charge kaon PDG mass
    trackThresholdPt            = 5.*Units.GeV, # 5 GeV pT cut for initial implementation
    muonCollectionKey           = "Muons",
    TrackParticleCollection     = "InDetTrackParticles",
    V0VertexFitterTool          = TrkV0Fitter,             # V0 vertex fitter
    useV0Fitter                 = False,                   # if False a TrkVertexFitterTool will be used
    TrkVertexFitterTool         = TrkVKalVrtFitter,        # VKalVrt vertex fitter
    TrackSelectorTool           = InDetTrackSelectorTool,
    ConversionFinderHelperTool  = InDetConversionHelper,
    VertexPointEstimator        = VtxPointEstimator,
    useMCPCuts                  = False)
ToolSvc += HDBS2PhiFinder

# For rho -> pi+ pi-
HDBS2RhoFinder = Analysis__JpsiFinder(
    name                        = "HDBS2RhoFinder",
    OutputLevel                 = INFO,
    muAndMu                     = False,
    muAndTrack                  = False,
    TrackAndTrack               = True,
    assumeDiMuons               = False,    # If true, will assume dimu hypothesis and use PDG value for mu mass
    invMassUpper                = 1.4*Units.GeV,
    invMassLower                = 2.*0.139570*Units.GeV,
    Chi2Cut                     = 200.,
    oppChargesOnly              = True,
    atLeastOneComb              = False,
    useCombinedMeasurement      = False,
    track1Mass                  = 0.139570*Units.GeV, # charge pion PDG mass
    track2Mass                  = 0.139570*Units.GeV, # charge pion PDG mass
    trackThresholdPt            = 5.*Units.GeV, # 5 GeV pT cut for initial implementation
    muonCollectionKey           = "Muons",
    TrackParticleCollection     = "InDetTrackParticles",
    V0VertexFitterTool          = TrkV0Fitter,             # V0 vertex fitter
    useV0Fitter                 = False,                   # if False a TrkVertexFitterTool will be used
    TrkVertexFitterTool         = TrkVKalVrtFitter,        # VKalVrt vertex fitter
    TrackSelectorTool           = InDetTrackSelectorTool,
    ConversionFinderHelperTool  = InDetConversionHelper,
    VertexPointEstimator        = VtxPointEstimator,
    useMCPCuts                  = False)
ToolSvc += HDBS2RhoFinder

# For Kstar -> K+ pi- and D0
HDBS2KstarFinder = Analysis__JpsiFinder(
    name                        = "HDBS2KstarFinder",
    OutputLevel                 = INFO,
    muAndMu                     = False,
    muAndTrack                  = False,
    TrackAndTrack               = True,
    assumeDiMuons               = False,    # If true, will assume dimu hypothesis and use PDG value for mu mass
    invMassUpper                = 2.5*Units.GeV,
    invMassLower                = (0.139570+0.493677)*Units.GeV,
    Chi2Cut                     = 200.,
    oppChargesOnly              = True,
    atLeastOneComb              = False,
    useCombinedMeasurement      = False,
    track1Mass                  = 0.493677*Units.GeV, # charge pion PDG mass
    track2Mass                  = 0.139570*Units.GeV, # charge pion PDG mass
    trackThresholdPt            = 5.*Units.GeV, # 5 GeV pT cut for initial implementation
    muonCollectionKey           = "Muons",
    TrackParticleCollection     = "InDetTrackParticles",
    V0VertexFitterTool          = TrkV0Fitter,             # V0 vertex fitter
    useV0Fitter                 = False,                   # if False a TrkVertexFitterTool will be used
    TrkVertexFitterTool         = TrkVKalVrtFitter,        # VKalVrt vertex fitter
    TrackSelectorTool           = InDetTrackSelectorTool,
    ConversionFinderHelperTool  = InDetConversionHelper,
    VertexPointEstimator        = VtxPointEstimator,
    useMCPCuts                  = False)
ToolSvc += HDBS2KstarFinder


#--------------------------------------------------------------------
## 3/ setup the vertex reconstruction "call" tool(s). They are part of the derivation framework.
##    These Augmentation tools add output vertex collection(s) into the StoreGate and add basic
##    decorations which do not depend on the vertex mass hypothesis (e.g. lxy, ptError, etc).
##    There should be one tool per topology, i.e. Jpsi and Psi(2S) do not need two instance of the
##    Reco tool is the JpsiFinder mass window is wide enough.
HDBS2RefitPV = False
from DerivationFrameworkBPhys.DerivationFrameworkBPhysConf import DerivationFramework__Reco_mumu
HDBS2Recomumu = DerivationFramework__Reco_mumu(
    name                   = "HDBS2Recomumu",
    JpsiFinder             = HDBS2JpsiFinder,
    OutputVtxContainerName = "HDBS2OniaCandidates",
    PVContainerName        = "PrimaryVertices",
    RefPVContainerName     = "HDBS2OniaRefittedPrimaryVertices",
    RefitPV = HDBS2RefitPV)
ToolSvc += HDBS2Recomumu

HDBS2Recotrktrk = DerivationFramework__Reco_mumu(
    name                   = "HDBS2Recotrktrk",
    JpsiFinder             = HDBS2PhiFinder,
    OutputVtxContainerName = "HDBS2PhiCandidates",
    PVContainerName        = "PrimaryVertices",
    RefPVContainerName     = "HDBS2PhiRefittedPrimaryVertices",
    RefitPV = HDBS2RefitPV)
ToolSvc += HDBS2Recotrktrk

HDBS2Recopipi = DerivationFramework__Reco_mumu(
    name                   = "HDBS2Recopipi",
    JpsiFinder             = HDBS2RhoFinder,
    OutputVtxContainerName = "HDBS2RhoCandidates",
    PVContainerName        = "PrimaryVertices",
    RefPVContainerName     = "HDBS2RhoRefittedPrimaryVertices",
    RefitPV = HDBS2RefitPV)
ToolSvc += HDBS2Recopipi

HDBS2RecoKpi = DerivationFramework__Reco_mumu(
    name                   = "HDBS2RecoKpi",
    JpsiFinder             = HDBS2KstarFinder,
    OutputVtxContainerName = "HDBS2KstarCandidates",
    PVContainerName        = "PrimaryVertices",
    RefPVContainerName     = "HDBS2KstarRefittedPrimaryVertices",
    RefitPV = HDBS2RefitPV)
ToolSvc += HDBS2RecoKpi

#--------------------------------------------------------------------
## 4/ setup the vertex selection and augmentation tool(s). These tools decorate the vertices with
##    variables that depend on the vertex mass hypothesis, e.g. invariant mass, proper decay time, etc.
##    Property HypothesisName is used as a prefix for these decorations.
##    They also perform tighter selection, flagging the vertecis that passed. The flag is a Char_t branch
##    named "passed_"+HypothesisName. It is used later by the "SelectEvent" and "Thin_vtxTrk" tools
##    to determine which events and candidates should be kept in the output stream.
##    Multiple instances of the Select_* tools can be used on a single input collection as long as they
##    use different "HypothesisName" flags.
from DerivationFrameworkBPhys.DerivationFrameworkBPhysConf import DerivationFramework__Select_onia2mumu
## a/ augment and select Jpsi->mumu candidates
HDBS2SelectJpsi2mumu = DerivationFramework__Select_onia2mumu(
    name                  = "HDBS2SelectJpsi2mumu",
    HypothesisName        = "Jpsi",
    InputVtxContainerName = HDBS2Recomumu.OutputVtxContainerName,
    VtxMassHypo           = 3.096916*Units.GeV,
    MassMin               = 2.2*Units.GeV,
    MassMax               = 3.6*Units.GeV,
    Chi2Max               = 200)
ToolSvc += HDBS2SelectJpsi2mumu
## b/ augment and select Psi(2S)->mumu candidates
HDBS2SelectPsi2mumu = DerivationFramework__Select_onia2mumu(
    name                  = "HDBS2SelectPsi2mumu",
    HypothesisName        = "Psi",
    InputVtxContainerName = HDBS2Recomumu.OutputVtxContainerName,
    VtxMassHypo           = 3.68609*Units.GeV,
    MassMin               = 3.3*Units.GeV,
    MassMax               = 4.3*Units.GeV,
    Chi2Max               = 200)
ToolSvc += HDBS2SelectPsi2mumu
## c/ augment and select Upsilon(nS)->mumu candidates
HDBS2SelectUpsi2mumu = DerivationFramework__Select_onia2mumu(
    name                  = "HDBS2SelectUpsi2mumu",
    HypothesisName        = "Upsi",
    InputVtxContainerName = HDBS2Recomumu.OutputVtxContainerName,
    VtxMassHypo           = 9.46030*Units.GeV,
    MassMin               = 8.0*Units.GeV,
    MassMax               = 12.0*Units.GeV,
    Chi2Max               = 200)
ToolSvc += HDBS2SelectUpsi2mumu
## d/ augment and select phi->KK candidates
HDBS2SelectPhi2trktrk = DerivationFramework__Select_onia2mumu(
    name                  = "HDBS2SelectPhi2trktrk",
    HypothesisName        = "Phi",
    InputVtxContainerName = HDBS2Recotrktrk.OutputVtxContainerName,
    TrkMasses             = [0.493677*Units.GeV, 0.493677*Units.GeV], # charge kaon PDG mass
    VtxMassHypo           = 1.019461*Units.GeV, # phi meson PDG mass
    MassMin               = 2.*0.493677*Units.GeV,
    MassMax               = 1.2*Units.GeV,
    Chi2Max               = 200)
ToolSvc += HDBS2SelectPhi2trktrk
## d/ augment and select Kstart->Kpi candidates
HDBS2SelectKstar2Kpi = DerivationFramework__Select_onia2mumu(
    name                  = "HDBS2SelectKstar2Kpi",
    HypothesisName        = "Kstar",
    InputVtxContainerName = HDBS2RecoKpi.OutputVtxContainerName,
    TrkMasses             = [0.493677*Units.GeV, 0.139570*Units.GeV], # charge kaon PDG mass
    VtxMassHypo           = 0.895*Units.GeV, # Kstar meson PDG mass
    MassMin               = (0.139570+0.493677)*Units.GeV,
    MassMax               = 1.3*Units.GeV,
    Chi2Max               = 200)
ToolSvc += HDBS2SelectKstar2Kpi
HDBS2SelectD02Kpi = DerivationFramework__Select_onia2mumu(
    name                  = "HDBS2SelectD02Kpi",
    HypothesisName        = "D0",
    InputVtxContainerName = HDBS2RecoKpi.OutputVtxContainerName,
    TrkMasses             = [0.493677*Units.GeV, 0.139570*Units.GeV], # charge kaon PDG mass
    VtxMassHypo           = 1.864*Units.GeV, # D0 meson PDG mass
    MassMin               = 1.6*Units.GeV,
    MassMax               = 2.5*Units.GeV,
    Chi2Max               = 200)
ToolSvc += HDBS2SelectD02Kpi
## d/ augment and select rho->pipi candidates
HDBS2SelectRho2pipi = DerivationFramework__Select_onia2mumu(
    name                  = "HDBS2SelectRho2pipi",
    HypothesisName        = "Rho",
    InputVtxContainerName = HDBS2Recopipi.OutputVtxContainerName,
    TrkMasses             = [0.139570*Units.GeV, 0.139570*Units.GeV], # charge kaon PDG mass
    VtxMassHypo           = 0.769*Units.GeV, # rho meson PDG mass
    MassMin               = 2.*0.139570*Units.GeV,
    MassMax               = 1.4*Units.GeV,
    Chi2Max               = 200)
ToolSvc += HDBS2SelectRho2pipi
## d/ augment and select K0s->pipi candidates
HDBS2SelectK0s2pipi = DerivationFramework__Select_onia2mumu(
    name                  = "HDBS2SelectK0s2pipi",
    HypothesisName        = "K0s",
    InputVtxContainerName = HDBS2Recopipi.OutputVtxContainerName,
    TrkMasses             = [0.139570*Units.GeV, 0.139570*Units.GeV], # charge kaon PDG mass
    VtxMassHypo           = 0.4976*Units.GeV, # Kshort meson PDG mass
    MassMin               = 2.*0.139570*Units.GeV,
    MassMax               = 0.6*Units.GeV,
    Chi2Max               = 200)
ToolSvc += HDBS2SelectK0s2pipi

## Keep output containers
HDBS2StaticContent = []
if HDBS2RefitPV:
    HDBS2StaticContent += ["xAOD::VertexContainer#%s" % HDBS2Recomumu.RefPVContainerName]
    HDBS2StaticContent += ["xAOD::VertexAuxContainer#%sAux." % HDBS2Recomumu.RefPVContainerName]
    HDBS2StaticContent += ["xAOD::VertexContainer#%s" % HDBS2Recotrktrk.RefPVContainerName]
    HDBS2StaticContent += ["xAOD::VertexAuxContainer#%sAux." % HDBS2Recotrktrk.RefPVContainerName]
## J/psi, psi(2S), Upsilon candidates
HDBS2StaticContent += ["xAOD::VertexContainer#%s"        % HDBS2Recomumu.OutputVtxContainerName]
HDBS2StaticContent += ["xAOD::VertexAuxContainer#%sAux." % HDBS2Recomumu.OutputVtxContainerName]

## we have to disable vxTrackAtVertex branch since it is not xAOD compatible
HDBS2StaticContent += ["xAOD::VertexAuxContainer#%sAux.-vxTrackAtVertex" % HDBS2Recomumu.OutputVtxContainerName]
## Phi candidates
HDBS2StaticContent += ["xAOD::VertexContainer#%s"        % HDBS2Recotrktrk.OutputVtxContainerName]
HDBS2StaticContent += ["xAOD::VertexAuxContainer#%sAux." % HDBS2Recotrktrk.OutputVtxContainerName]
# ## we have to disable vxTrackAtVertex branch since it is not xAOD compatible
HDBS2StaticContent += ["xAOD::VertexAuxContainer#%sAux.-vxTrackAtVertex" % HDBS2Recotrktrk.OutputVtxContainerName]
## Rho candidates
HDBS2StaticContent += ["xAOD::VertexContainer#%s"        % HDBS2Recopipi.OutputVtxContainerName]
HDBS2StaticContent += ["xAOD::VertexAuxContainer#%sAux." % HDBS2Recopipi.OutputVtxContainerName]
# ## we have to disable vxTrackAtVertex branch since it is not xAOD compatible
HDBS2StaticContent += ["xAOD::VertexAuxContainer#%sAux.-vxTrackAtVertex" % HDBS2Recopipi.OutputVtxContainerName]
## Kstar candidates
HDBS2StaticContent += ["xAOD::VertexContainer#%s"        % HDBS2RecoKpi.OutputVtxContainerName]
HDBS2StaticContent += ["xAOD::VertexAuxContainer#%sAux." % HDBS2RecoKpi.OutputVtxContainerName]
# ## we have to disable vxTrackAtVertex branch since it is not xAOD compatible
HDBS2StaticContent += ["xAOD::VertexAuxContainer#%sAux.-vxTrackAtVertex" % HDBS2RecoKpi.OutputVtxContainerName]

if DerivationFrameworkHasTruth:
    HDBS2StaticContent += ["xAOD::TruthParticleContainer#TruthBosonWithDecayParticles"]
    HDBS2StaticContent += ["xAOD::TruthParticleContainer#TruthBosonWithDecayParticlesAux."]
    HDBS2StaticContent += ["xAOD::TruthParticleContainer#TruthBosonWithDecayVerticies"]
    HDBS2StaticContent += ["xAOD::TruthParticleContainer#TruthBosonWithDecayVerticiesAux."]
#================
# THINNING
#================
thinningTools=[]

# Establish the thinning helper (which will set up the services behind the scenes)
from DerivationFrameworkCore.ThinningHelper import ThinningHelper
HDBS2ThinningHelper = ThinningHelper("HDBS2ThinningHelper")
#trigger navigation content
HDBS2ThinningHelper.TriggerChains = 'HLT_e.*|HLT_2e.*|HLT_mu.*|HLT_2mu.*'
HDBS2ThinningHelper.AppendToStream(HDBS2Stream)

# MET/Jet tracks
#thinning_expression = "(InDetTrackParticles.pt > 0.5*GeV) && (InDetTrackParticles.numberOfPixelHits > 0) && (InDetTrackParticles.numberOfSCTHits > 5) && (abs(DFCommonInDetTrackZ0AtPV) < 1.5)"
thinning_expression = "(InDetTrackParticles.pt > 0.5*GeV) && (InDetTrackParticles.numberOfPixelHits > 0) && (InDetTrackParticles.numberOfSCTHits > 5)"

from DerivationFrameworkInDet.DerivationFrameworkInDetConf import DerivationFramework__TrackParticleThinning
HDBS2TPThinningTool = DerivationFramework__TrackParticleThinning(name                   = "HDBS2TPThinningTool",
                                                                   ThinningService        = HDBS2ThinningHelper.ThinningSvc(),
                                                                   SelectionString        = thinning_expression,
                                                                   InDetTrackParticlesKey = "InDetTrackParticles")
ToolSvc += HDBS2TPThinningTool
thinningTools.append(HDBS2TPThinningTool)

from DerivationFrameworkInDet.DerivationFrameworkInDetConf import DerivationFramework__JetTrackParticleThinning
HDBS2JetTPThinningTool = DerivationFramework__JetTrackParticleThinning(name                   = "HDBS2JetTPThinningTool",
                                                                         ThinningService        = HDBS2ThinningHelper.ThinningSvc(),
                                                                         JetKey                 = "AntiKt4EMTopoJets",
                                                                         InDetTrackParticlesKey = "InDetTrackParticles")
ToolSvc += HDBS2JetTPThinningTool
thinningTools.append(HDBS2JetTPThinningTool)

# Tracks associated with Muons
from DerivationFrameworkInDet.DerivationFrameworkInDetConf import DerivationFramework__MuonTrackParticleThinning
HDBS2MuonTPThinningTool = DerivationFramework__MuonTrackParticleThinning(name                   = "HDBS2MuonTPThinningTool",
                                                                           ThinningService        = HDBS2ThinningHelper.ThinningSvc(),
                                                                           MuonKey                = "Muons",
                                                                           InDetTrackParticlesKey = "InDetTrackParticles")
ToolSvc += HDBS2MuonTPThinningTool
thinningTools.append(HDBS2MuonTPThinningTool)

# Tracks associated with Electrons
from DerivationFrameworkInDet.DerivationFrameworkInDetConf import DerivationFramework__EgammaTrackParticleThinning
HDBS2ElectronTPThinningTool = DerivationFramework__EgammaTrackParticleThinning(name                   = "HDBS2ElectronTPThinningTool",
                                                                                 ThinningService        = HDBS2ThinningHelper.ThinningSvc(),
                                                                                 SGKey                  = "Electrons",
                                                                                 InDetTrackParticlesKey = "InDetTrackParticles",
                                                                                 BestMatchOnly          = False)
ToolSvc += HDBS2ElectronTPThinningTool
thinningTools.append(HDBS2ElectronTPThinningTool)

# Tracks associated with Photons
from DerivationFrameworkInDet.DerivationFrameworkInDetConf import DerivationFramework__EgammaTrackParticleThinning
HDBS2PhotonTPThinningTool = DerivationFramework__EgammaTrackParticleThinning(       name                    = "HDBS2PhotonTPThinningTool",
                                                                                      ThinningService         = HDBS2ThinningHelper.ThinningSvc(),
                                                                                      SGKey                   = "Photons",
                                                                                      InDetTrackParticlesKey  = "InDetTrackParticles",
                                                                                      BestMatchOnly           = True)
ToolSvc += HDBS2PhotonTPThinningTool
thinningTools.append(HDBS2PhotonTPThinningTool)

# Tracks associated with taus
from DerivationFrameworkInDet.DerivationFrameworkInDetConf import DerivationFramework__TauTrackParticleThinning
HDBS2TauTPThinningTool = DerivationFramework__TauTrackParticleThinning(name                   = "HDBS2TauTPThinningTool",
                                                                         ThinningService        = HDBS2ThinningHelper.ThinningSvc(),
                                                                         TauKey                 = "TauJets",
                                                                         ConeSize               = 0.6,
                                                                         InDetTrackParticlesKey = "InDetTrackParticles")
ToolSvc += HDBS2TauTPThinningTool
thinningTools.append(HDBS2TauTPThinningTool)

_info( "HDBS2.py thinningTools: %s", thinningTools)

#====================================================================
# SKIMMING TOOLS 
#====================================================================

from AthenaCommon.BeamFlags import jobproperties
_info("HDBS2.py jobproperties.Beam.energy(): %d", jobproperties.Beam.energy())
SkipTriggerRequirement= DerivationFrameworkHasTruth
# no need to apply trigger requirements on MC 
_info("HDBS2.py SkipTriggerRequirement: %s", SkipTriggerRequirement)
TriggerJPSI= []
TriggerPHI = [] 
TriggerRHO = [] 
TriggerKSHORT = []
TriggerKSTAR = []
TriggerD0 = []
TriggerWPIGAMMA = []
if not SkipTriggerRequirement:
    TriggerJPSI               = ["HLT_mu24_imedium", "HLT_mu26_imedium", "HLT_mu50", "HLT_2mu10", "HLT_2mu14", "HLT_mu24_mu8noL1"]
    if jobproperties.Beam.energy()==4000000.0:
        #  8 TeV data
        TriggerJPSI               = ["EF_mu24i_tight", "EF_mu36_tight", "EF_2mu13", "EF_mu18_tight_mu8_EFFS", "EF_mu18_tight_Jpsi_EFFS"]
    if jobproperties.Beam.energy()==6500000.0:
        # 13 TeV data
        # TriggerJPSI               = ["HLT_mu26_imedium", "HLT_mu50", "HLT_2mu14", "HLT_mu24_mu8noL1"]
        # TriggerJPSI               = ["HLT_mu24_imedium", "HLT_mu26_imedium", "HLT_mu50", "HLT_2mu10", "HLT_2mu14", "HLT_mu24_mu8noL1", "HLT_g35_loose_L1EM22VHI_mu18noL1", "HLT_g25_medium_mu24", "HLT_g35_loose_L1EM22VHI_mu15noL1_mu2noL1", "HLT_g15_loose_2mu10_msonly", "HLT_mu18_mu8noL1", "HLT_mu22_mu8noL1"]
        TriggerJPSI = ["HLT_g25_medium_mu24", "HLT_mu26_ivarmedium", "HLT_g35_loose_L1EM22VHI_mu18noL1", "HLT_g35_loose_L1EM22VHI_mu15noL1_mu2noL1",
            "HLT_g35_loose_L1EM24VHI_mu18noL1",#2016
            "HLT_g35_tight_icalotight_L1EM24VHIM_mu18noL1", #2017-18
            "HLT_g35_tight_icalotight_L1EM24VHIM_mu15noL1_mu2noL1", #2017-18
            "HLT_g35_tight_icalotight_L1EM24VHI_mu18noL1", #2017-18
            "HLT_g35_tight_icalotight_L1EM24VHI_mu15noL1_mu2noL1"] #2017-18
        TriggerPHI  = ["HLT_g35_medium_tau25_dikaontight_tracktwo_L1TAU12",  #2015
                       "HLT_g35_medium_tau25_dikaonmasstight_tracktwo_L1TAU12",  #2016
                       "HLT_g25_medium_tau25_dikaonmasstight_tracktwo_60mVis10000", #2016
                       "HLT_g25_medium_tau25_dikaonmass_tracktwo_50mVis10000", #2016 
                       "HLT_g25_medium_L1EM24VHI_tau25_dikaonmass_tracktwo_50mVis10000" #2016+2017
                       "HLT_g25_medium_L1EM24VHIM_tau25_dikaonmass_tracktwo_50mVis10000", #2017 + 2018
                       "HLT_g25_medium_tau25_dikaonmass_tracktwo_50mVis10000_L130M-EM20ITAU12", #2017 (with L1Topo)
                       "HLT_g25_medium_L1EM24VHI_tau25_dikaonmass_tracktwoEF_50mVis10000", #2018
                       "HLT_g25_medium_L1EM24VHI_tau25_dikaonmass_tracktwoMVA_50mVis10000"] #2018
        TriggerRHO  = ["HLT_g35_medium_tau25_dipion1_tracktwo_L1TAU12", #2016
                       "HLT_g25_medium_tau25_dipion1loose_tracktwo_50mVis10000", #2016
                       "HLT_g25_medium_L1EM24VHI_tau25_dipion1loose_tracktwo_50mVis10000", #2016+2017
                       "HLT_g25_medium_L1EM24VHIM_tau25_dipion1loose_tracktwo_50mVis10000", #2017
                       "HLT_g25_medium_tau25_dipion1loose_tracktwo_50mVis10000_L130M-EM20ITAU12", #2017+2018
                       "HLT_g25_medium_L1EM24VHI_tau25_dipion1loose_tracktwoEF_50mVis10000", #2018
                       "HLT_g25_medium_L1EM24VHI_tau25_dipion1loose_tracktwoMVA_50mVis10000"] #2018
        TriggerKSHORT = ["HLT_g35_medium_tau25_dipion2_tracktwo_L1TAU12", #2016
                         "HLT_g25_medium_tau25_dipion2_tracktwo_50mVis10000", #2016
                         "HLT_g25_medium_L1EM24VHI_tau25_dipion2_tracktwo_50mVis10000", #2016+2017
                         "HLT_g25_medium_L1EM24VHIM_tau25_dipion2_tracktwo_50mVis10000", #2017
                         "HLT_g25_medium_tau25_dipion2_tracktwo_50mVis10000_L130M-EM20ITAU12", #2017
                         "HLT_g25_medium_L1EM24VHI_tau25_dipion2_tracktwoEF_50mVis10000", #2018
                         "HLT_g25_medium_L1EM24VHI_tau25_dipion2_tracktwoMVA_50mVis10000"] #2018
        TriggerKSTAR= ["HLT_g35_medium_tau25_kaonpi1_tracktwo_L1TAU12", #2016
                       "HLT_g25_medium_L1EM24VHI_tau25_kaonpi1_tracktwo_50mVis10000",  #2016+2017
                       "HLT_g25_medium_L1EM24VHIM_tau25_kaonpi1_tracktwo_50mVis10000", #2017
                       "HLT_g25_medium_tau25_kaonpi1_tracktwo_50mVis10000_L130M-EM20ITAU12", #2017
                       "HLT_g25_medium_L1EM24VHI_tau25_kaonpi1_tracktwoEF_50mVis10000",
                       "HLT_g25_medium_L1EM24VHI_tau25_kaonpi1_tracktwoMVA_50mVis10000"] #2017
        TriggerD0   = ["HLT_g35_medium_tau25_kaonpi2_tracktwo_L1TAU12", #2016
                       "HLT_g25_medium_L1EM24VHI_tau25_kaonpi2_tracktwo_50mVis10000", #2016+2017
                       "HLT_g25_medium_L1EM24VHIM_tau25_kaonpi2_tracktwo_50mVis10000", #2017
                       "HLT_g25_medium_tau25_kaonpi2_tracktwo_50mVis10000_L130M-EM20ITAU12", #2017
                       "HLT_g25_medium_L1EM24VHI_tau25_kaonpi2_tracktwoEF_50mVis10000",#2018
                       "HLT_g25_medium_L1EM24VHI_tau25_kaonpi2_tracktwoMVA_50mVis10000"] #2018
        TriggerRHO+=TriggerKSHORT
        TriggerRHO+=TriggerKSTAR
        TriggerRHO+=TriggerD0
        TriggerWPIGAMMA  = [ "HLT_g35_medium_tau25_singlepion_tracktwo_L1TAU12",  #2016
                             "HLT_g35_medium_tau25_singlepiontight_tracktwo_L1TAU12", #2016
                             "HLT_g25_medium_tau25_singlepion_tracktwo_50mVis10000", #2016
                             "HLT_g25_medium_L1EM24VHI_tau25_singlepion_tracktwo_50mVis10000", #2016+2017
                             "HLT_g25_medium_L1EM24VHIM_tau25_singlepion_tracktwo_50mVis10000", #2017
                             "HLT_g25_medium_tau25_singlepion_tracktwo_50mVis10000_L130M-EM20ITAU12", #2017
                             "HLT_g25_medium_L1EM24VHI_tau25_singlepion_tracktwoEF_50mVis10000", #2018
                             "HLT_g25_medium_L1EM24VHI_tau25_singlepion_tracktwoMVA_50mVis10000"] #2018



from DerivationFrameworkHDBS.DerivationFrameworkHDBSConf import DerivationFramework__SkimmingToolHDBS2
SkimmingToolHDBS2 = DerivationFramework__SkimmingToolHDBS2(name                       = "SkimmingToolHDBS2",
                                                             SkipTriggerRequirement     = SkipTriggerRequirement,
                                                             FilterType                 = "JPSI", 
                                                             NumberOfLeptons            = 0,
                                                             NumberOfElectrons          = 0,
                                                             NumberOfMuons              = 2,
                                                             NumberOfJets               = 0,
                                                             NumberOfPhotons            = 1,
                                                             NumberOfTracks             = 2,
                                                             MuonQuality                = "inMS",
                                                             TriggerJPSI                = TriggerJPSI,
                                                             TriggerPHI                 = TriggerPHI,
                                                             TriggerRHO                 = TriggerRHO,
                                                             TriggerWPIGAMMA            = TriggerWPIGAMMA,
                                                             MuonPtCut                  =  2.5*Units.GeV,
                                                             PhotonPtCut                = 27.0*Units.GeV,
                                                             LeadingMuonPtCut           = 17.0*Units.GeV,
                                                             DiMuonPtCut                = 27.0*Units.GeV,
                                                             TrackPtCut                 = 5.0*Units.GeV,
                                                             TrackPtCutLoose            =  4.5*Units.GeV,
                                                             DiTrackPtCut               = 24.0*Units.GeV,
                                                             TrackPtForWCut             = 30.0*Units.GeV,
                                                             PhotonPtForWCut            = 30.0*Units.GeV,
                                                             InvariantMassJpsiLowCut    =  2.4*Units.GeV,
                                                             InvariantMassJpsiUpCut     =  4.3*Units.GeV,
                                                             InvariantMassUpsilonLowCut =  8.0*Units.GeV,
                                                             InvariantMassUpsilonUpCut  = 12.0*Units.GeV,
                                                             InvariantMassPhiLowCut     =  2.*0.493677*Units.GeV,
                                                             InvariantMassPhiUpCut      =  1.1*Units.GeV,
                                                             InvariantMassRhoLowCut     =  0.425*Units.GeV, #2.*0.13957*Units.GeV,
                                                             InvariantMassRhoUpCut      =  1.150*Units.GeV,
                                                             InvariantMassKshortLowCut  =  0.400*Units.GeV,
                                                             InvariantMassKshortUpCut   =  0.600*Units.GeV,
                                                             InvariantMassKstarLowCut   =  0.690*Units.GeV,
                                                             InvariantMassKstarUpCut    =  1.090*Units.GeV,
                                                             InvariantMassD0LowCut      =  1.700*Units.GeV,
                                                             InvariantMassD0UpCut       =  2.030*Units.GeV,
                                                             InvariantMassWLowCut       =  55.*Units.GeV)
ToolSvc += SkimmingToolHDBS2
_info( SkimmingToolHDBS2)

#====================================================================
# CREATE THE DERIVATION KERNEL ALGORITHM AND PASS THE ABOVE TOOLS  
#====================================================================

from DerivationFrameworkFlavourTag.FlavourTagCommon import *
FlavorTagInit(JetCollections=['AntiKt4EMPFlowJets'],Sequencer=DerivationFrameworkJob)
# The name of the kernel (LooseSkimKernel in this case) must be unique to this derivation
from DerivationFrameworkCore.DerivationFrameworkCoreConf import DerivationFramework__DerivationKernel
DerivationFrameworkJob += CfgMgr.DerivationFramework__DerivationKernel("HDBS2Kernel",
                                                                       SkimmingTools = [SkimmingToolHDBS2],
                                                                       ThinningTools = thinningTools,
                                                                       AugmentationTools = [HDBS2Recomumu, HDBS2SelectJpsi2mumu,
                                                                                            HDBS2SelectPsi2mumu, HDBS2SelectUpsi2mumu,
                                                                                            HDBS2Recotrktrk, HDBS2SelectPhi2trktrk, 
                                                                                            HDBS2Recopipi, HDBS2RecoKpi, HDBS2SelectRho2pipi, HDBS2SelectK0s2pipi, HDBS2SelectKstar2Kpi, HDBS2SelectD02Kpi
                                                                                            ])
 
#====================================================================
# Add the containers to the output stream - slimming done here
#====================================================================
from DerivationFrameworkHDBS.HDBS2ExtraContent import *
from DerivationFrameworkCore.SlimmingHelper import SlimmingHelper
HDBS2SlimmingHelper = SlimmingHelper("HDBS2SlimmingHelper")
HDBS2SlimmingHelper.SmartCollections = ["Electrons",
                                          "Photons",
                                          "Muons",
                                          "TauJets",
                                          "AntiKt4EMTopoJets",
                                          "MET_Reference_AntiKt4EMPFlow",
                                          "AntiKt4EMPFlowJets",
                                          "AntiKt4EMPFlowJets_BTagging201903",
                                          "BTagging_AntiKt4EMPFlow_201903",
                                          "InDetTrackParticles",
                                          "PrimaryVertices"]

HDBS2SlimmingHelper.ExtraVariables = HDBS2ExtraContent
HDBS2SlimmingHelper.AllVariables = HDBS2ExtraContainers

from DerivationFrameworkEGamma.PhotonsCPDetailedContent import PhotonsCPDetailedContent
HDBS2SlimmingHelper.ExtraVariables += PhotonsCPDetailedContent

# For J/psi vertex augmentation
HDBS2SlimmingHelper.StaticContent = HDBS2StaticContent

if DerivationFrameworkHasTruth:
    HDBS2SlimmingHelper.ExtraVariables += HDBS2ExtraContentTruth
    HDBS2SlimmingHelper.AllVariables += HDBS2ExtraContainersTruth


HDBS2SlimmingHelper.IncludeMuonTriggerContent = True
HDBS2SlimmingHelper.IncludeBPhysTriggerContent = True
HDBS2SlimmingHelper.IncludeTauTriggerContent = True
HDBS2SlimmingHelper.IncludeEGammaTriggerContent = True
HDBS2SlimmingHelper.AppendToDictionary = {
    'TruthEvents':'xAOD::TruthEventContainer','TruthEventsAux':'xAOD::TruthEventAuxContainer',
    'TruthBosonWithDecayParticles':'xAOD::TruthParticleContainer','TruthBosonWithDecayParticlesAux':'xAOD::TruthParticleAuxContainer',
    'TruthBosonWithDecayVertices':'xAOD::TruthVertexContainer','TruthBosonWithDecayVerticesAux':'xAOD::TruthVertexAuxContainer',
    }


HDBS2SlimmingHelper.AppendContentToStream(HDBS2Stream)
if DerivationFrameworkHasTruth:
    HDBS2Stream.AddMetaDataItem( [ "xAOD::TruthMetaDataContainer#TruthMetaData", "xAOD::TruthMetaDataAuxContainer#TruthMetaDataAux." ] )
