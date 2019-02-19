#====================================================================
# HIGG2D5.py for JPSI filter
# This requires the reductionConf flag HIGG2D5 in Reco_tf.py   
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
if DerivationFrameworkIsMonteCarlo:
    from DerivationFrameworkMCTruth.MCTruthCommon import addStandardTruthContents
    addStandardTruthContents()
from DerivationFrameworkCore.WeightMetadata import *
import AthenaCommon.SystemOfUnits as Units

# Add sumOfWeights metadata for LHE3 multiweights =======
from DerivationFrameworkCore.LHE3WeightMetadata import *

#====================================================================
# SET UP STREAM
#====================================================================
streamName = derivationFlags.WriteDAOD_HIGG2D5Stream.StreamName
fileName = buildFileName(derivationFlags.WriteDAOD_HIGG2D5Stream)
HIGG2D5Stream = MSMgr.NewPoolRootStream( streamName, fileName)
HIGG2D5Stream.AcceptAlgs(["HIGG2D5Kernel"])

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
HIGG2D5JpsiFinder = Analysis__JpsiFinder(
    name                        = "HIGG2D5JpsiFinder",
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
ToolSvc += HIGG2D5JpsiFinder

# For phi -> K+ K-
HIGG2D5PhiFinder = Analysis__JpsiFinder(
    name                        = "HIGG2D5PhiFinder",
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
ToolSvc += HIGG2D5PhiFinder

# For rho -> pi+ pi-
HIGG2D5RhoFinder = Analysis__JpsiFinder(
    name                        = "HIGG2D5RhoFinder",
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
ToolSvc += HIGG2D5RhoFinder

# For Kstar -> K+ pi- and D0
HIGG2D5KstarFinder = Analysis__JpsiFinder(
    name                        = "HIGG2D5KstarFinder",
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
ToolSvc += HIGG2D5KstarFinder


#--------------------------------------------------------------------
## 3/ setup the vertex reconstruction "call" tool(s). They are part of the derivation framework.
##    These Augmentation tools add output vertex collection(s) into the StoreGate and add basic
##    decorations which do not depend on the vertex mass hypothesis (e.g. lxy, ptError, etc).
##    There should be one tool per topology, i.e. Jpsi and Psi(2S) do not need two instance of the
##    Reco tool is the JpsiFinder mass window is wide enough.
HIGG2D5RefitPV = False
from DerivationFrameworkBPhys.DerivationFrameworkBPhysConf import DerivationFramework__Reco_mumu
HIGG2D5Recomumu = DerivationFramework__Reco_mumu(
    name                   = "HIGG2D5Recomumu",
    JpsiFinder             = HIGG2D5JpsiFinder,
    OutputVtxContainerName = "HIGG2D5OniaCandidates",
    PVContainerName        = "PrimaryVertices",
    RefPVContainerName     = "HIGG2D5OniaRefittedPrimaryVertices",
    RefitPV = HIGG2D5RefitPV)
ToolSvc += HIGG2D5Recomumu

HIGG2D5Recotrktrk = DerivationFramework__Reco_mumu(
    name                   = "HIGG2D5Recotrktrk",
    JpsiFinder             = HIGG2D5PhiFinder,
    OutputVtxContainerName = "HIGG2D5PhiCandidates",
    PVContainerName        = "PrimaryVertices",
    RefPVContainerName     = "HIGG2D5PhiRefittedPrimaryVertices",
    RefitPV = HIGG2D5RefitPV)
ToolSvc += HIGG2D5Recotrktrk

HIGG2D5Recopipi = DerivationFramework__Reco_mumu(
    name                   = "HIGG2D5Recopipi",
    JpsiFinder             = HIGG2D5RhoFinder,
    OutputVtxContainerName = "HIGG2D5RhoCandidates",
    PVContainerName        = "PrimaryVertices",
    RefPVContainerName     = "HIGG2D5RhoRefittedPrimaryVertices",
    RefitPV = HIGG2D5RefitPV)
ToolSvc += HIGG2D5Recopipi

HIGG2D5RecoKpi = DerivationFramework__Reco_mumu(
    name                   = "HIGG2D5RecoKpi",
    JpsiFinder             = HIGG2D5KstarFinder,
    OutputVtxContainerName = "HIGG2D5KstarCandidates",
    PVContainerName        = "PrimaryVertices",
    RefPVContainerName     = "HIGG2D5KstarRefittedPrimaryVertices",
    RefitPV = HIGG2D5RefitPV)
ToolSvc += HIGG2D5RecoKpi

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
HIGG2D5SelectJpsi2mumu = DerivationFramework__Select_onia2mumu(
    name                  = "HIGG2D5SelectJpsi2mumu",
    HypothesisName        = "Jpsi",
    InputVtxContainerName = HIGG2D5Recomumu.OutputVtxContainerName,
    VtxMassHypo           = 3.096916*Units.GeV,
    MassMin               = 2.2*Units.GeV,
    MassMax               = 3.6*Units.GeV,
    Chi2Max               = 200)
ToolSvc += HIGG2D5SelectJpsi2mumu
## b/ augment and select Psi(2S)->mumu candidates
HIGG2D5SelectPsi2mumu = DerivationFramework__Select_onia2mumu(
    name                  = "HIGG2D5SelectPsi2mumu",
    HypothesisName        = "Psi",
    InputVtxContainerName = HIGG2D5Recomumu.OutputVtxContainerName,
    VtxMassHypo           = 3.68609*Units.GeV,
    MassMin               = 3.3*Units.GeV,
    MassMax               = 4.3*Units.GeV,
    Chi2Max               = 200)
ToolSvc += HIGG2D5SelectPsi2mumu
## c/ augment and select Upsilon(nS)->mumu candidates
HIGG2D5SelectUpsi2mumu = DerivationFramework__Select_onia2mumu(
    name                  = "HIGG2D5SelectUpsi2mumu",
    HypothesisName        = "Upsi",
    InputVtxContainerName = HIGG2D5Recomumu.OutputVtxContainerName,
    VtxMassHypo           = 9.46030*Units.GeV,
    MassMin               = 8.0*Units.GeV,
    MassMax               = 12.0*Units.GeV,
    Chi2Max               = 200)
ToolSvc += HIGG2D5SelectUpsi2mumu
## d/ augment and select phi->KK candidates
HIGG2D5SelectPhi2trktrk = DerivationFramework__Select_onia2mumu(
    name                  = "HIGG2D5SelectPhi2trktrk",
    HypothesisName        = "Phi",
    InputVtxContainerName = HIGG2D5Recotrktrk.OutputVtxContainerName,
    TrkMasses             = [0.493677*Units.GeV, 0.493677*Units.GeV], # charge kaon PDG mass
    VtxMassHypo           = 1.019461*Units.GeV, # phi meson PDG mass
    MassMin               = 2.*0.493677*Units.GeV,
    MassMax               = 1.2*Units.GeV,
    Chi2Max               = 200)
ToolSvc += HIGG2D5SelectPhi2trktrk
## d/ augment and select Kstart->Kpi candidates
HIGG2D5SelectKstar2Kpi = DerivationFramework__Select_onia2mumu(
    name                  = "HIGG2D5SelectKstar2Kpi",
    HypothesisName        = "Kstar",
    InputVtxContainerName = HIGG2D5RecoKpi.OutputVtxContainerName,
    TrkMasses             = [0.493677*Units.GeV, 0.139570*Units.GeV], # charge kaon PDG mass
    VtxMassHypo           = 0.895*Units.GeV, # Kstar meson PDG mass
    MassMin               = (0.139570+0.493677)*Units.GeV,
    MassMax               = 1.3*Units.GeV,
    Chi2Max               = 200)
ToolSvc += HIGG2D5SelectKstar2Kpi
HIGG2D5SelectD02Kpi = DerivationFramework__Select_onia2mumu(
    name                  = "HIGG2D5SelectD02Kpi",
    HypothesisName        = "D0",
    InputVtxContainerName = HIGG2D5RecoKpi.OutputVtxContainerName,
    TrkMasses             = [0.493677*Units.GeV, 0.139570*Units.GeV], # charge kaon PDG mass
    VtxMassHypo           = 1.864*Units.GeV, # D0 meson PDG mass
    MassMin               = 1.6*Units.GeV,
    MassMax               = 2.5*Units.GeV,
    Chi2Max               = 200)
ToolSvc += HIGG2D5SelectD02Kpi
## d/ augment and select rho->pipi candidates
HIGG2D5SelectRho2pipi = DerivationFramework__Select_onia2mumu(
    name                  = "HIGG2D5SelectRho2pipi",
    HypothesisName        = "Rho",
    InputVtxContainerName = HIGG2D5Recopipi.OutputVtxContainerName,
    TrkMasses             = [0.139570*Units.GeV, 0.139570*Units.GeV], # charge kaon PDG mass
    VtxMassHypo           = 0.769*Units.GeV, # rho meson PDG mass
    MassMin               = 2.*0.139570*Units.GeV,
    MassMax               = 1.4*Units.GeV,
    Chi2Max               = 200)
ToolSvc += HIGG2D5SelectRho2pipi
## d/ augment and select K0s->pipi candidates
HIGG2D5SelectK0s2pipi = DerivationFramework__Select_onia2mumu(
    name                  = "HIGG2D5SelectK0s2pipi",
    HypothesisName        = "K0s",
    InputVtxContainerName = HIGG2D5Recopipi.OutputVtxContainerName,
    TrkMasses             = [0.139570*Units.GeV, 0.139570*Units.GeV], # charge kaon PDG mass
    VtxMassHypo           = 0.4976*Units.GeV, # Kshort meson PDG mass
    MassMin               = 2.*0.139570*Units.GeV,
    MassMax               = 0.6*Units.GeV,
    Chi2Max               = 200)
ToolSvc += HIGG2D5SelectK0s2pipi

## Keep output containers
HIGG2D5StaticContent = []
if HIGG2D5RefitPV:
    HIGG2D5StaticContent += ["xAOD::VertexContainer#%s" % HIGG2D5Recomumu.RefPVContainerName]
    HIGG2D5StaticContent += ["xAOD::VertexAuxContainer#%sAux." % HIGG2D5Recomumu.RefPVContainerName]
    HIGG2D5StaticContent += ["xAOD::VertexContainer#%s" % HIGG2D5Recotrktrk.RefPVContainerName]
    HIGG2D5StaticContent += ["xAOD::VertexAuxContainer#%sAux." % HIGG2D5Recotrktrk.RefPVContainerName]
## J/psi, psi(2S), Upsilon candidates
HIGG2D5StaticContent += ["xAOD::VertexContainer#%s"        % HIGG2D5Recomumu.OutputVtxContainerName]
HIGG2D5StaticContent += ["xAOD::VertexAuxContainer#%sAux." % HIGG2D5Recomumu.OutputVtxContainerName]

## we have to disable vxTrackAtVertex branch since it is not xAOD compatible
HIGG2D5StaticContent += ["xAOD::VertexAuxContainer#%sAux.-vxTrackAtVertex" % HIGG2D5Recomumu.OutputVtxContainerName]
## Phi candidates
HIGG2D5StaticContent += ["xAOD::VertexContainer#%s"        % HIGG2D5Recotrktrk.OutputVtxContainerName]
HIGG2D5StaticContent += ["xAOD::VertexAuxContainer#%sAux." % HIGG2D5Recotrktrk.OutputVtxContainerName]
# ## we have to disable vxTrackAtVertex branch since it is not xAOD compatible
HIGG2D5StaticContent += ["xAOD::VertexAuxContainer#%sAux.-vxTrackAtVertex" % HIGG2D5Recotrktrk.OutputVtxContainerName]
## Rho candidates
HIGG2D5StaticContent += ["xAOD::VertexContainer#%s"        % HIGG2D5Recopipi.OutputVtxContainerName]
HIGG2D5StaticContent += ["xAOD::VertexAuxContainer#%sAux." % HIGG2D5Recopipi.OutputVtxContainerName]
# ## we have to disable vxTrackAtVertex branch since it is not xAOD compatible
HIGG2D5StaticContent += ["xAOD::VertexAuxContainer#%sAux.-vxTrackAtVertex" % HIGG2D5Recopipi.OutputVtxContainerName]
## Kstar candidates
HIGG2D5StaticContent += ["xAOD::VertexContainer#%s"        % HIGG2D5RecoKpi.OutputVtxContainerName]
HIGG2D5StaticContent += ["xAOD::VertexAuxContainer#%sAux." % HIGG2D5RecoKpi.OutputVtxContainerName]
# ## we have to disable vxTrackAtVertex branch since it is not xAOD compatible
HIGG2D5StaticContent += ["xAOD::VertexAuxContainer#%sAux.-vxTrackAtVertex" % HIGG2D5RecoKpi.OutputVtxContainerName]

#================
# THINNING
#================
thinningTools=[]

# Establish the thinning helper (which will set up the services behind the scenes)
from DerivationFrameworkCore.ThinningHelper import ThinningHelper
HIGG2D5ThinningHelper = ThinningHelper("HIGG2D5ThinningHelper")
#trigger navigation content
HIGG2D5ThinningHelper.TriggerChains = 'HLT_e.*|HLT_2e.*|HLT_mu.*|HLT_2mu.*'
HIGG2D5ThinningHelper.AppendToStream(HIGG2D5Stream)

# MET/Jet tracks
#thinning_expression = "(InDetTrackParticles.pt > 0.5*GeV) && (InDetTrackParticles.numberOfPixelHits > 0) && (InDetTrackParticles.numberOfSCTHits > 5) && (abs(DFCommonInDetTrackZ0AtPV) < 1.5)"
thinning_expression = "(InDetTrackParticles.pt > 0.5*GeV) && (InDetTrackParticles.numberOfPixelHits > 0) && (InDetTrackParticles.numberOfSCTHits > 5)"

from DerivationFrameworkInDet.DerivationFrameworkInDetConf import DerivationFramework__TrackParticleThinning
HIGG2D5TPThinningTool = DerivationFramework__TrackParticleThinning(name                   = "HIGG2D5TPThinningTool",
                                                                   ThinningService        = HIGG2D5ThinningHelper.ThinningSvc(),
                                                                   SelectionString        = thinning_expression,
                                                                   InDetTrackParticlesKey = "InDetTrackParticles")
ToolSvc += HIGG2D5TPThinningTool
thinningTools.append(HIGG2D5TPThinningTool)

from DerivationFrameworkInDet.DerivationFrameworkInDetConf import DerivationFramework__JetTrackParticleThinning
HIGG2D5JetTPThinningTool = DerivationFramework__JetTrackParticleThinning(name                   = "HIGG2D5JetTPThinningTool",
                                                                         ThinningService        = HIGG2D5ThinningHelper.ThinningSvc(),
                                                                         JetKey                 = "AntiKt4EMTopoJets",
                                                                         InDetTrackParticlesKey = "InDetTrackParticles")
ToolSvc += HIGG2D5JetTPThinningTool
thinningTools.append(HIGG2D5JetTPThinningTool)

# Tracks associated with Muons
from DerivationFrameworkInDet.DerivationFrameworkInDetConf import DerivationFramework__MuonTrackParticleThinning
HIGG2D5MuonTPThinningTool = DerivationFramework__MuonTrackParticleThinning(name                   = "HIGG2D5MuonTPThinningTool",
                                                                           ThinningService        = HIGG2D5ThinningHelper.ThinningSvc(),
                                                                           MuonKey                = "Muons",
                                                                           InDetTrackParticlesKey = "InDetTrackParticles")
ToolSvc += HIGG2D5MuonTPThinningTool
thinningTools.append(HIGG2D5MuonTPThinningTool)

# Tracks associated with Electrons
from DerivationFrameworkInDet.DerivationFrameworkInDetConf import DerivationFramework__EgammaTrackParticleThinning
HIGG2D5ElectronTPThinningTool = DerivationFramework__EgammaTrackParticleThinning(name                   = "HIGG2D5ElectronTPThinningTool",
                                                                                 ThinningService        = HIGG2D5ThinningHelper.ThinningSvc(),
                                                                                 SGKey                  = "Electrons",
                                                                                 InDetTrackParticlesKey = "InDetTrackParticles",
                                                                                 BestMatchOnly          = False)
ToolSvc += HIGG2D5ElectronTPThinningTool
thinningTools.append(HIGG2D5ElectronTPThinningTool)

# Tracks associated with Photons
from DerivationFrameworkInDet.DerivationFrameworkInDetConf import DerivationFramework__EgammaTrackParticleThinning
HIGG2D5PhotonTPThinningTool = DerivationFramework__EgammaTrackParticleThinning(       name                    = "HIGG2D5PhotonTPThinningTool",
                                                                                      ThinningService         = HIGG2D5ThinningHelper.ThinningSvc(),
                                                                                      SGKey                   = "Photons",
                                                                                      InDetTrackParticlesKey  = "InDetTrackParticles",
                                                                                      BestMatchOnly           = True)
ToolSvc += HIGG2D5PhotonTPThinningTool
thinningTools.append(HIGG2D5PhotonTPThinningTool)

# Tracks associated with taus
from DerivationFrameworkInDet.DerivationFrameworkInDetConf import DerivationFramework__TauTrackParticleThinning
HIGG2D5TauTPThinningTool = DerivationFramework__TauTrackParticleThinning(name                   = "HIGG2D5TauTPThinningTool",
                                                                         ThinningService        = HIGG2D5ThinningHelper.ThinningSvc(),
                                                                         TauKey                 = "TauJets",
                                                                         ConeSize               = 0.6,
                                                                         InDetTrackParticlesKey = "InDetTrackParticles")
ToolSvc += HIGG2D5TauTPThinningTool
thinningTools.append(HIGG2D5TauTPThinningTool)

# Truth particles
useGenericTruthThinning = True
if useGenericTruthThinning:
    truth_cond_WZH    = "((abs(TruthParticles.pdgId) >= 23) && (abs(TruthParticles.pdgId) <= 25))" # W, Z and Higgs
    truth_cond_Lepton = "((abs(TruthParticles.pdgId) >= 11) && (abs(TruthParticles.pdgId) <= 16))" # Leptons
    truth_cond_Quark  = "((abs(TruthParticles.pdgId) ==  6))"                                      # Top quark
    truth_cond_Photon = "((abs(TruthParticles.pdgId) == 22) && (TruthParticles.pt > 1*GeV))"       # Photon

    truth_expression = '('+truth_cond_WZH+' || '+truth_cond_Lepton +' || '+truth_cond_Quark +' || '+truth_cond_Photon+')'

    from DerivationFrameworkMCTruth.DerivationFrameworkMCTruthConf import DerivationFramework__GenericTruthThinning
    HIGG2D5TruthThinningTool = DerivationFramework__GenericTruthThinning(name                         = "HIGG2D5TruthThinningTool", 
                                                                         ThinningService              = HIGG2D5ThinningHelper.ThinningSvc(),
                                                                         ParticleSelectionString      = truth_expression,
                                                                         PreserveDescendants          = False,
                                                                         PreserveGeneratorDescendants = True,
                                                                         PreserveAncestors            = True)
else:
    from DerivationFrameworkMCTruth.DerivationFrameworkMCTruthConf import DerivationFramework__MenuTruthThinning
    HIGG2D5TruthThinningTool = DerivationFramework__MenuTruthThinning(name                         = "HIGG2D5TruthThinningTool",
                                                                      ThinningService              = "HIGG2D5ThinningSvc",
                                                                      WritePartons                 = False,
                                                                      WriteHadrons                 = False,
                                                                      WriteBHadrons                = True,
                                                                      WriteGeant                   = False,
                                                                      GeantPhotonPtThresh          = -1.0,
                                                                      WriteTauHad                  = True,
                                                                      PartonPtThresh               = -1.0,
                                                                      WriteBSM                     = True,
                                                                      WriteBosons                  = True,
                                                                      WriteBSMProducts             = True,
                                                                      WriteBosonProducts           = True,
                                                                      WriteTopAndDecays            = True,
                                                                      WriteEverything              = False,
                                                                      WriteAllLeptons              = True,
                                                                      WriteStatus3                 = False,
                                                                      PreserveGeneratorDescendants = True,
                                                                      WriteFirstN                  = -1)

if DerivationFrameworkIsMonteCarlo:
    ToolSvc += HIGG2D5TruthThinningTool
    thinningTools.append(HIGG2D5TruthThinningTool)
print "HIGG2D5.py thinningTools", thinningTools

#====================================================================
# SKIMMING TOOLS 
#====================================================================

from AthenaCommon.BeamFlags import jobproperties
print "HIGG2D5.py jobproperties.Beam.energy()", jobproperties.Beam.energy()
SkipTriggerRequirement= DerivationFrameworkIsMonteCarlo
# no need to apply trigger requirements on MC 
print "HIGG2D5.py SkipTriggerRequirement", SkipTriggerRequirement
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
        TriggerJPSI = ["HLT_g25_medium_mu24", "HLT_mu26_ivarmedium", "HLT_g35_loose_L1EM22VHI_mu18noL1", "HLT_g35_loose_L1EM22VHI_mu15noL1_mu2noL1"]
        TriggerPHI  = ["HLT_g35_medium_tau25_dikaontight_tracktwo_L1TAU12",  #2015
                       "HLT_g35_medium_tau25_dikaonmasstight_tracktwo_L1TAU12",  #2016
                       "HLT_g25_medium_tau25_dikaonmasstight_tracktwo_60mVis10000", #2016
                       "HLT_g25_medium_tau25_dikaonmass_tracktwo_50mVis10000", #2016 
                       "HLT_g25_medium_L1EM24VHI_tau25_dikaonmass_tracktwo_50mVis10000" #2016+2017
                       "HLT_g25_medium_L1EM24VHIM_tau25_dikaonmass_tracktwo_50mVis10000", #2017
                       "HLT_g25_medium_tau25_dikaonmass_tracktwo_50mVis10000_L130M-EM20ITAU12"] #2017
        TriggerRHO  = ["HLT_g35_medium_tau25_dipion1_tracktwo_L1TAU12", #2016
                       "HLT_g25_medium_tau25_dipion1loose_tracktwo_50mVis10000", #2016
                       "HLT_g25_medium_L1EM24VHI_tau25_dipion1loose_tracktwo_50mVis10000", #2016+2017
                       "HLT_g25_medium_L1EM24VHIM_tau25_dipion1loose_tracktwo_50mVis10000", #2017
                       "HLT_g25_medium_tau25_dipion1loose_tracktwo_50mVis10000_L130M-EM20ITAU12"] #2017
        TriggerKSHORT = ["HLT_g35_medium_tau25_dipion2_tracktwo_L1TAU12", #2016
                         "HLT_g25_medium_tau25_dipion2_tracktwo_50mVis10000", #2016
                         "HLT_g25_medium_L1EM24VHI_tau25_dipion2_tracktwo_50mVis10000", #2016+2017
                         "HLT_g25_medium_L1EM24VHIM_tau25_dipion2_tracktwo_50mVis10000", #2017
                         "HLT_g25_medium_tau25_dipion2_tracktwo_50mVis10000_L130M-EM20ITAU12"] #2017
        TriggerKSTAR= ["HLT_g35_medium_tau25_kaonpi1_tracktwo_L1TAU12", #2016
                       "HLT_g25_medium_L1EM24VHI_tau25_kaonpi1_tracktwo_50mVis10000",  #2016+2017
                       "HLT_g25_medium_L1EM24VHIM_tau25_kaonpi1_tracktwo_50mVis10000", #2017
                       "HLT_g25_medium_tau25_kaonpi1_tracktwo_50mVis10000_L130M-EM20ITAU12"] #2017
        TriggerD0   = ["HLT_g35_medium_tau25_kaonpi2_tracktwo_L1TAU12", #2016
                       "HLT_g25_medium_L1EM24VHI_tau25_kaonpi2_tracktwo_50mVis10000", #2016+2017
                       "HLT_g25_medium_L1EM24VHIM_tau25_kaonpi2_tracktwo_50mVis10000", #2017
                       "HLT_g25_medium_tau25_kaonpi2_tracktwo_50mVis10000_L130M-EM20ITAU12"] #2017
        TriggerRHO+=TriggerKSHORT
        TriggerRHO+=TriggerKSTAR
        TriggerRHO+=TriggerD0
        TriggerWPIGAMMA  = [ "HLT_g35_medium_tau25_singlepion_tracktwo_L1TAU12",  #2016
                             "HLT_g35_medium_tau25_singlepiontight_tracktwo_L1TAU12", #2016
                             "HLT_g25_medium_tau25_singlepion_tracktwo_50mVis10000", #2016
                             "HLT_g25_medium_L1EM24VHI_tau25_singlepion_tracktwo_50mVis10000", #2016+2017
                             "HLT_g25_medium_L1EM24VHIM_tau25_singlepion_tracktwo_50mVis10000", #2017
                             "HLT_g25_medium_tau25_singlepion_tracktwo_50mVis10000_L130M-EM20ITAU12"] #2017


#print "HIGG2D5.py TriggerJPSI", TriggerJPSI

from DerivationFrameworkHiggs.DerivationFrameworkHiggsConf import DerivationFramework__SkimmingToolHIGG2
SkimmingToolHIGG2D5 = DerivationFramework__SkimmingToolHIGG2(name                       = "SkimmingToolHIGG2D5",
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
ToolSvc += SkimmingToolHIGG2D5
print SkimmingToolHIGG2D5

#====================================================================
# CREATE THE DERIVATION KERNEL ALGORITHM AND PASS THE ABOVE TOOLS  
#====================================================================

# The name of the kernel (LooseSkimKernel in this case) must be unique to this derivation
from DerivationFrameworkCore.DerivationFrameworkCoreConf import DerivationFramework__DerivationKernel
DerivationFrameworkJob += CfgMgr.DerivationFramework__DerivationKernel("HIGG2D5Kernel",
                                                                       SkimmingTools = [SkimmingToolHIGG2D5],
                                                                       ThinningTools = thinningTools,
                                                                       AugmentationTools = [HIGG2D5Recomumu, HIGG2D5SelectJpsi2mumu,
                                                                                            HIGG2D5SelectPsi2mumu, HIGG2D5SelectUpsi2mumu,
                                                                                            HIGG2D5Recotrktrk, HIGG2D5SelectPhi2trktrk, 
                                                                                            HIGG2D5Recopipi, HIGG2D5RecoKpi, HIGG2D5SelectRho2pipi, HIGG2D5SelectK0s2pipi, HIGG2D5SelectKstar2Kpi, HIGG2D5SelectD02Kpi
                                                                                            ])
 
#====================================================================
# Add the containers to the output stream - slimming done here
#====================================================================
from DerivationFrameworkHiggs.HIGG2D5ExtraContent import *
from DerivationFrameworkCore.SlimmingHelper import SlimmingHelper
HIGG2D5SlimmingHelper = SlimmingHelper("HIGG2D5SlimmingHelper")

HIGG2D5SlimmingHelper.SmartCollections = ["Electrons",
                                          "Photons",
                                          "Muons",
                                          "TauJets",
                                          "MET_Reference_AntiKt4EMTopo",
                                          "AntiKt4EMTopoJets",
                                          "BTagging_AntiKt4EMTopo",
                                          "InDetTrackParticles",
                                          "PrimaryVertices"]

HIGG2D5SlimmingHelper.ExtraVariables = HIGG2D5ExtraContent
HIGG2D5SlimmingHelper.AllVariables = HIGG2D5ExtraContainers

from DerivationFrameworkEGamma.PhotonsCPDetailedContent import PhotonsCPDetailedContent
HIGG2D5SlimmingHelper.ExtraVariables += PhotonsCPDetailedContent

if DerivationFrameworkIsMonteCarlo:
    HIGG2D5SlimmingHelper.ExtraVariables += HIGG2D5ExtraContentTruth
    HIGG2D5SlimmingHelper.AllVariables += HIGG2D5ExtraContainersTruth

# For J/psi vertex augmentation
HIGG2D5SlimmingHelper.StaticContent = HIGG2D5StaticContent

HIGG2D5SlimmingHelper.IncludeMuonTriggerContent = True
HIGG2D5SlimmingHelper.IncludeBPhysTriggerContent = True
HIGG2D5SlimmingHelper.IncludeTauTriggerContent = True
HIGG2D5SlimmingHelper.IncludeEGammaTriggerContent = True

HIGG2D5SlimmingHelper.AppendContentToStream(HIGG2D5Stream)
