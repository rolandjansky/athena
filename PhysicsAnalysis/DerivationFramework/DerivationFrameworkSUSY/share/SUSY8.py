#====================================================================
# Derivation targeting searches for charged stable massive particles
# reductionConf flag SUSY8 in Reco_tf.py
#====================================================================

from AthenaCommon import Logging
susy8log = Logging.logging.getLogger('SUSY8')

from DerivationFrameworkCore.DerivationFrameworkMaster import *
from DerivationFrameworkJetEtMiss.JetCommon import *
from DerivationFrameworkJetEtMiss.ExtendedJetCommon import *
from DerivationFrameworkEGamma.EGammaCommon import *
from DerivationFrameworkMuons.MuonsCommon import *
from DerivationFrameworkTau.TauCommon import *
from DerivationFrameworkInDet.InDetCommon import *
from DerivationFrameworkJetEtMiss.METCommon import *
from DerivationFrameworkFlavourTag.FlavourTagCommon import *

if DerivationFrameworkHasTruth:
  from DerivationFrameworkMCTruth.MCTruthCommon import addStandardTruthContents
  addStandardTruthContents()

### Set up stream
streamName = derivationFlags.WriteDAOD_SUSY8Stream.StreamName
fileName = buildFileName(derivationFlags.WriteDAOD_SUSY8Stream)
SUSY8Stream = MSMgr.NewPoolRootStream(streamName, fileName)
SUSY8Stream.AcceptAlgs(["SUSY8KernelSkim"])

### Init
from DerivationFrameworkCore.ThinningHelper import ThinningHelper
SUSY8ThinningHelper = ThinningHelper("SUSY8ThinningHelper")
thinningTools = []
AugmentationTools = []

# stream-specific sequence for on-the-fly jet building
SeqSUSY8 = CfgMgr.AthSequencer("SeqSUSY8")
DerivationFrameworkJob += SeqSUSY8

#====================================================================
# TRIGGER THINNING AND SKIMMING
#====================================================================
from DerivationFrameworkSUSY.SUSY8TriggerList import *

allTriggers = SUSY8JetMETTriggers + SUSY8MuonTriggers + SUSY8DimuonTriggers + SUSY8METTriggers + SUSY8LateMuonTriggers
# susy8log.info("SUSY8TriggerList: {!s}".format(allTriggers))

SUSY8ThinningHelper.TriggerChains = '|'.join(allTriggers)
SUSY8ThinningHelper.AppendToStream(SUSY8Stream)

from DerivationFrameworkTools.DerivationFrameworkToolsConf import DerivationFramework__TriggerSkimmingTool
SUSY8TriggerSkimmingTool = DerivationFramework__TriggerSkimmingTool(name          = "SUSY8TriggerSkimmingTool",
                                                                    TriggerListOR = allTriggers)
ToolSvc += SUSY8TriggerSkimmingTool
# from DerivationFrameworkTools.DerivationFrameworkToolsConf import DerivationFramework__xAODStringSkimmingTool
# SUSY8SkimmingTool = DerivationFramework__xAODStringSkimmingTool(name = "SUSY8SkimmingTool",
#                                                                 expression = ('(' + ' || '.join(allTriggers) + ')'))
# ToolSvc += SUSY8SkimmingTool

#====================================================================
# THINNING TOOL
#====================================================================\

thinning_expression = "(InDetTrackParticles.pt > 10.0*GeV) && (InDetTrackParticles.numberOfPixelHits > 0) && (abs(DFCommonInDetTrackZ0AtPV) < 10.0)"
from DerivationFrameworkInDet.DerivationFrameworkInDetConf import DerivationFramework__TrackParticleThinning
SUSY8MetTPThinningTool = DerivationFramework__TrackParticleThinning(name                    = "SUSY8MetTPThinningTool",
                                                                    ThinningService         = SUSY8ThinningHelper.ThinningSvc(),
                                                                    SelectionString         = thinning_expression,
                                                                    InDetTrackParticlesKey  = "InDetTrackParticles",
                                                                    ApplyAnd                = True)
ToolSvc += SUSY8MetTPThinningTool
thinningTools.append(SUSY8MetTPThinningTool)

# TrackParticles associated with jets
from DerivationFrameworkInDet.DerivationFrameworkInDetConf import DerivationFramework__JetTrackParticleThinning
SUSY8JetTPThinningTool = DerivationFramework__JetTrackParticleThinning( name                    = "SUSY8JetTPThinningTool",
                                                                        ThinningService         = SUSY8ThinningHelper.ThinningSvc(),
                                                                        JetKey                  = "AntiKt4EMTopoJets",
                                                                        InDetTrackParticlesKey  = "InDetTrackParticles",
                                                                        ApplyAnd                = True)
ToolSvc += SUSY8JetTPThinningTool
thinningTools.append(SUSY8JetTPThinningTool)

# TrackParticles associated with Muons
from DerivationFrameworkInDet.DerivationFrameworkInDetConf import DerivationFramework__MuonTrackParticleThinning
SUSY8MuonTPThinningTool = DerivationFramework__MuonTrackParticleThinning(name                    = "SUSY8MuonTPThinningTool",
                                                                         ThinningService         = SUSY8ThinningHelper.ThinningSvc(),
                                                                         MuonKey                 = "Muons",
                                                                         InDetTrackParticlesKey  = "InDetTrackParticles")
ToolSvc += SUSY8MuonTPThinningTool
thinningTools.append(SUSY8MuonTPThinningTool)

# TrackParticles associated with electrons
from DerivationFrameworkInDet.DerivationFrameworkInDetConf import DerivationFramework__EgammaTrackParticleThinning
SUSY8ElectronTPThinningTool = DerivationFramework__EgammaTrackParticleThinning(name                    = "SUSY8ElectronTPThinningTool",
                                                                               ThinningService         = SUSY8ThinningHelper.ThinningSvc(),
                                                                               SGKey                   = "Electrons",
                                                                               InDetTrackParticlesKey  = "InDetTrackParticles")
ToolSvc += SUSY8ElectronTPThinningTool
thinningTools.append(SUSY8ElectronTPThinningTool)

# TrackParticles associated with photons
from DerivationFrameworkInDet.DerivationFrameworkInDetConf import DerivationFramework__EgammaTrackParticleThinning
SUSY8PhotonTPThinningTool = DerivationFramework__EgammaTrackParticleThinning(name                    = "SUSY8PhotonTPThinningTool",
                                                                             ThinningService         = SUSY8ThinningHelper.ThinningSvc(),
                                                                             SGKey                   = "Photons",
                                                                             InDetTrackParticlesKey  = "InDetTrackParticles")
ToolSvc += SUSY8PhotonTPThinningTool
thinningTools.append(SUSY8PhotonTPThinningTool)

# TrackParticles associated with taus
from DerivationFrameworkInDet.DerivationFrameworkInDetConf import DerivationFramework__TauTrackParticleThinning
SUSY8TauTPThinningTool = DerivationFramework__TauTrackParticleThinning(name                   = "SUSY8TauTPThinningTool",
                                                                       ThinningService        = SUSY8ThinningHelper.ThinningSvc(),
                                                                       TauKey                 = "TauJets",
                                                                       InDetTrackParticlesKey = "InDetTrackParticles")
ToolSvc += SUSY8TauTPThinningTool
thinningTools.append(SUSY8TauTPThinningTool)

#====================================================================
# TRUTH THINNING
#====================================================================
if DerivationFrameworkHasTruth:
  from DerivationFrameworkMCTruth.DerivationFrameworkMCTruthConf import DerivationFramework__MenuTruthThinning
  SUSY8TruthThinningTool = DerivationFramework__MenuTruthThinning(name              = "SUSY8TruthThinningTool",
                                                       ThinningService              = SUSY8ThinningHelper.ThinningSvc(),
                                                       WritePartons                 = False,
                                                       WriteHadrons                 = False,
                                                       WriteBHadrons                = True,
                                                       WriteGeant                   = False,
                                                       GeantPhotonPtThresh          = 20000,
                                                       WriteTauHad                  = True,
                                                       PartonPtThresh               = -1.0,
                                                       WriteBSM                     = True,
                                                       WriteBosons                  = True,
                                                       WriteBosonProducts           = True,
                                                       WriteBSMProducts             = True,
                                                       WriteTopAndDecays            = True,
                                                       WriteEverything              = False,
                                                       WriteAllLeptons              = False,
                                                       WriteLeptonsNotFromHadrons   = False,
                                                       WriteStatus3                 = False,
                                                       WriteFirstN                  = -1,
                                                       PreserveAncestors            = True,
                                                       PreserveGeneratorDescendants = False,
                                                       SimBarcodeOffset             = DerivationFrameworkSimBarcodeOffset)
  ToolSvc += SUSY8TruthThinningTool
  thinningTools.append(SUSY8TruthThinningTool)

#====================================================================
# TrackParticleCaloCellDecoration
#====================================================================

from DerivationFrameworkSUSY.DerivationFrameworkSUSYConf import DerivationFramework__TrackParticleCaloCellDecorator
SUSY8_TrackParticleCaloCellDecorator = DerivationFramework__TrackParticleCaloCellDecorator(name             = "SUSY8_TrackParticleCaloCellDecorator",
                                                                                           DecorationPrefix = "SUSY8",
                                                                                           ContainerName    = "InDetTrackParticles")

ToolSvc += SUSY8_TrackParticleCaloCellDecorator
AugmentationTools.append(SUSY8_TrackParticleCaloCellDecorator)

#=======================================
# CREATE THE DERIVATION KERNEL ALGORITHM
#=======================================

from DerivationFrameworkCore.DerivationFrameworkCoreConf import DerivationFramework__DerivationKernel
# Add sumOfWeights metadata for LHE3 multiweights =======
from DerivationFrameworkCore.LHE3WeightMetadata import *

#==============================================================================
# SUSY signal augmentation (before skimming!)
#==============================================================================

from DerivationFrameworkSUSY.DecorateSUSYProcess import IsSUSYSignal
if IsSUSYSignal():
  from DerivationFrameworkSUSY.DecorateSUSYProcess import DecorateSUSYProcess
  SeqSUSY8 += CfgMgr.DerivationFramework__DerivationKernel("SUSY8KernelSigAug",
                                                          AugmentationTools = DecorateSUSYProcess("SUSY8"))
  from DerivationFrameworkSUSY.SUSYWeightMetadata import *

#==============================================================================
# SUSY skimming selection
#==============================================================================
SeqSUSY8 += CfgMgr.DerivationFramework__DerivationKernel("SUSY8KernelSkim",
                                                         SkimmingTools = [SUSY8TriggerSkimmingTool])

#==============================================================================
# Augment after skim
#==============================================================================

SeqSUSY8 += CfgMgr.DerivationFramework__DerivationKernel(
  "SUSY8KernelAug",
  AugmentationTools = AugmentationTools,
  ThinningTools = thinningTools)



## Adding decorations for fJVT PFlow jets                                                                                                                                                                   #re-tag PFlow jets so they have b-tagging info.                                                                                                                                                             
FlavorTagInit(JetCollections = ['AntiKt4EMPFlowJets'], Sequencer = SeqSUSY8)


getPFlowfJVT(jetalg='AntiKt4EMPFlow',sequence=SeqSUSY8, algname='JetForwardPFlowJvtToolAlg')
applyMVfJvtAugmentation(jetalg='AntiKt4EMTopo',sequence=SeqSUSY8, algname='JetForwardJvtToolBDTAlg')

#====================================================================
# CONTENT LIST
#====================================================================

from DerivationFrameworkCore.SlimmingHelper import SlimmingHelper

SUSY8SlimmingHelper = SlimmingHelper("SUSY8SlimmingHelper")
SUSY8SlimmingHelper.SmartCollections = [
  "AntiKt4EMTopoJets",
  "Muons",
  "InDetTrackParticles",
  "Photons",
  "MET_Reference_AntiKt4EMTopo",
  "AntiKt4EMPFlowJets",
  "MET_Reference_AntiKt4EMPFlow",
  "AntiKt4EMPFlowJets_BTagging201903",
  "BTagging_AntiKt4EMPFlow_201903",
  "PrimaryVertices"
]
SUSY8SlimmingHelper.AllVariables = [
  # "CaloCalTopoClusters",
  "CombinedMuonTrackParticles",
  "CombinedStauTrackParticles",
  # "ExtrapolatedMuonTrackParticles",
  # "GSFTrackParticles",
  # "InDetTrackParticlesAssociatedClusters",
  # "InDetTrackParticlesClusterAssociations",
  # "InDetTrackParticles",
  "METAssoc_AntiKt4EMTopo",
  "MET_Calo",
  "MET_Core_AntiKt4EMTopo",
  # "MET_Core_AntiKt4LCTopo",
  "MET_EMTopo",
  "MET_LocHadTopo",
  # "MET_Reference_AntiKt4EMTopo",
  # "MET_Reference_AntiKt4LCTopo",
  "MET_Track",
  "MET_Truth",
  "MSonlyTracklets",
  "MuonSegments",
  "MuonSpectrometerTrackParticles",
  # "Muons",
  "SlowMuons",
  "Staus",
  "TruthEvents",
  "TruthParticles",
  "TruthVertices"
]
SUSY8SlimmingHelper.ExtraVariables = [
  "Muons.CaloLRLikelihood.CaloMuonIDTag.charge.InnerDetectorPt.MuonSpectrometerPt.ptcone20.ptcone30.quality",
  "Photons.author.Loose.Tight",
  "AntiKt4EMTopoJets.NumTrkPt500.NumTrkPt1000.TrackWidthPt1000.DFCommonJets_jetClean_VeryLooseBadLLP",
  "InDetTrackParticles.numberOfIBLOverflowsdEdx.numberOfPixelSplitHits.numberOfUsedHitsdEdx.pixeldEdx.SUSY8_CaloCelldEta.SUSY8_CaloCelldPhi.SUSY8_CaloCelldR.SUSY8_CaloCelldX.SUSY8_CaloCelldY.SUSY8_CaloCelldZ.SUSY8_CaloCellE.SUSY8_CaloCellEta.SUSY8_CaloCellGain.SUSY8_CaloCellID.SUSY8_CaloCellPhi.SUSY8_CaloCellProvenance.SUSY8_CaloCellQuality.SUSY8_CaloCellR.SUSY8_CaloCellSampling.SUSY8_CaloCellTime.SUSY8_CaloCellX.SUSY8_CaloCellY.SUSY8_CaloCellZ.TRTdEdx.TRTdEdxUsedHits.truthOrigin.truthType",
  # "CombinedMuonTrackParticles.d0.z0.vz.definingParametersCovMatrix.truthOrigin.truthType",
  "ExtrapolatedMuonTrackParticles.d0.z0.vz.definingParametersCovMatrix.truthOrigin.truthType",
  "TauJets.pt.eta.phi.m.IsTruthMatched.pt_vis.truthParticleLink.truthOrigin.truthType.truthJetLink.DFCommonTausLoose",
  "MuonTruthParticles.barcode.decayVtxLink.e.m.pdgId.prodVtxLink.px.py.pz.recoMuonLink.status.truthOrigin.truthType",
  "AntiKt4TruthJets.pt.eta.phi.m.TruthLabelDeltaR_B.TruthLabelDeltaR_C.TruthLabelDeltaR_T.TruthLabelID.ConeTruthLabelID.PartonTruthLabelID.HadronConeExclTruthLabelID"
]
SUSY8SlimmingHelper.IncludeMuonTriggerContent = True
SUSY8SlimmingHelper.IncludeJetTriggerContent = True
SUSY8SlimmingHelper.IncludeEtMissTriggerContent = True
SUSY8SlimmingHelper.AppendToDictionary = {
  'TruthTop':'xAOD::TruthParticleContainer',
  'TruthTopAux':'xAOD::TruthParticleAuxContainer',
  'TruthBSM':'xAOD::TruthParticleContainer',
  'TruthBSMAux':'xAOD::TruthParticleAuxContainer',
  'TruthBoson':'xAOD::TruthParticleContainer',
  'TruthBosonAux':'xAOD::TruthParticleAuxContainer'
}
if DerivationFrameworkHasTruth:
  SUSY8SlimmingHelper.AllVariables += [
    "TruthElectrons",
    "TruthMuons",
    "TruthTaus",
    "TruthPhotons",
    "TruthNeutrinos",
    "TruthTop",
    "TruthBSM",
    "TruthBoson"
  ]
SUSY8SlimmingHelper.AppendContentToStream(SUSY8Stream)
