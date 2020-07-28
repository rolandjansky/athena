#********************************************************************
# SUSY3.py
# reductionConf flag SUSY3 in Reco_tf.py
#********************************************************************

from DerivationFrameworkCore.DerivationFrameworkMaster import *
from DerivationFrameworkJetEtMiss.JetCommon import *
from DerivationFrameworkJetEtMiss.ExtendedJetCommon import *
from DerivationFrameworkEGamma.EGammaCommon import *
from DerivationFrameworkMuons.MuonsCommon import *
from DerivationFrameworkTau.TauCommon import *
if DerivationFrameworkIsMonteCarlo:
  from DerivationFrameworkMCTruth.MCTruthCommon import addStandardTruthContents,addMiniTruthCollectionLinks,addPVCollection
  addStandardTruthContents()
  addMiniTruthCollectionLinks()
  addPVCollection()
from DerivationFrameworkInDet.InDetCommon import *
from DerivationFrameworkJetEtMiss.METCommon import *
from DerivationFrameworkFlavourTag.FlavourTagCommon import *

### Set up stream
streamName = derivationFlags.WriteDAOD_SUSY3Stream.StreamName
fileName   = buildFileName( derivationFlags.WriteDAOD_SUSY3Stream )
SUSY3Stream = MSMgr.NewPoolRootStream( streamName, fileName )
SUSY3Stream.AcceptAlgs(["SUSY3KernelSkim"])

### Init
from DerivationFrameworkCore.ThinningHelper import ThinningHelper
SUSY3ThinningHelper = ThinningHelper( "SUSY3ThinningHelper" )
thinningTools       = []
AugmentationTools   = []

# stream-specific sequence for on-the-fly jet building
SeqSUSY3 = CfgMgr.AthSequencer("SeqSUSY3")
DerivationFrameworkJob += SeqSUSY3


#====================================================================
# Trigger navigation thinning
#====================================================================
from DerivationFrameworkSUSY.SUSY3TriggerList import triggerRegEx, triggerRegExThinning
SUSY3ThinningHelper.TriggerChains = '|'.join(triggerRegExThinning)

from DerivationFrameworkTools.DerivationFrameworkToolsConf import DerivationFramework__TriggerSkimmingTool
SUSY3TriggerSkimmingTool = DerivationFramework__TriggerSkimmingTool(
    name          = "SUSY3TriggerSkimmingTool",
    TriggerListOR = triggerRegEx )
ToolSvc += SUSY3TriggerSkimmingTool

SUSY3ThinningHelper.AppendToStream( SUSY3Stream ) # needs to go after SUSY3ThinningHelper.TriggerChains = ... but before accesses to SUSY3ThinningHelper.ThinningSvc()


#====================================================================
# THINNING TOOLS
#====================================================================

# TrackParticles associated with Muons
from DerivationFrameworkInDet.DerivationFrameworkInDetConf import DerivationFramework__MuonTrackParticleThinning
SUSY3MuonTPThinningTool = DerivationFramework__MuonTrackParticleThinning(name                    = "SUSY3MuonTPThinningTool",
                                                                         ThinningService         = SUSY3ThinningHelper.ThinningSvc(),
                                                                         MuonKey                 = "Muons",
                                                                         InDetTrackParticlesKey  = "InDetTrackParticles")
ToolSvc += SUSY3MuonTPThinningTool
thinningTools.append(SUSY3MuonTPThinningTool)

# TrackParticles associated with electrons
from DerivationFrameworkInDet.DerivationFrameworkInDetConf import DerivationFramework__EgammaTrackParticleThinning
SUSY3ElectronTPThinningTool = DerivationFramework__EgammaTrackParticleThinning(	name                    = "SUSY3ElectronTPThinningTool",
                                                                                ThinningService         = SUSY3ThinningHelper.ThinningSvc(),
                                                                                SGKey             	= "Electrons",
                                                                                InDetTrackParticlesKey  = "InDetTrackParticles")
ToolSvc += SUSY3ElectronTPThinningTool
thinningTools.append(SUSY3ElectronTPThinningTool)

# TrackParticles associated with photons
from DerivationFrameworkInDet.DerivationFrameworkInDetConf import DerivationFramework__EgammaTrackParticleThinning
SUSY3PhotonTPThinningTool = DerivationFramework__EgammaTrackParticleThinning(name			 = "SUSY3PhotonTPThinningTool",
										 ThinningService	 = SUSY3ThinningHelper.ThinningSvc(),
										 SGKey			 = "Photons",
										 InDetTrackParticlesKey  = "InDetTrackParticles")
ToolSvc += SUSY3PhotonTPThinningTool
thinningTools.append(SUSY3PhotonTPThinningTool)

# TrackParticles associated with taus, only keep charged tau tracks
from DerivationFrameworkInDet.DerivationFrameworkInDetConf import DerivationFramework__TauTrackParticleThinning
SUSY3TauTPThinningTool = DerivationFramework__TauTrackParticleThinning(name                   = "SUSY3TauTPThinningTool",
                                                                       ThinningService        = SUSY3ThinningHelper.ThinningSvc(),
                                                                       TauKey                 = "TauJets",
                                                                       InDetTrackParticlesKey = "InDetTrackParticles",
                                                                       SelectionString        = "(TauJets.ptFinalCalib >= 15.*GeV) && (TauJets.nTracks==1 || TauJets.nTracks==3)",
                                                                       ApplyAnd               = False,
                                                                       DoTauTracksThinning    = True,
                                                                       TauTracksKey           = "TauTracks")
ToolSvc += SUSY3TauTPThinningTool
thinningTools.append(SUSY3TauTPThinningTool)

# TauJets thinning
from DerivationFrameworkTools.DerivationFrameworkToolsConf import DerivationFramework__GenericObjectThinning
SUSY3TauJetsThinningTool = DerivationFramework__GenericObjectThinning(name            = "SUSY3TauJetsThinningTool",
                                                                      ThinningService = SUSY3ThinningHelper.ThinningSvc(),
                                                                      ContainerName   = "TauJets",
                                                                      SelectionString = "(TauJets.ptFinalCalib >= 15.*GeV) && (TauJets.nTracks==1 || TauJets.nTracks==3)")
ToolSvc += SUSY3TauJetsThinningTool
thinningTools.append(SUSY3TauJetsThinningTool)


#====================================================================
# TRUTH THINNING
#====================================================================
if DerivationFrameworkIsMonteCarlo:

  # Decorate Electron with bkg electron type/origin
  from MCTruthClassifier.MCTruthClassifierBase import MCTruthClassifier as BkgElectronMCTruthClassifier
  from DerivationFrameworkEGamma.DerivationFrameworkEGammaConf import DerivationFramework__BkgElectronClassification
  BkgElectronClassificationTool = DerivationFramework__BkgElectronClassification (name = "BkgElectronClassificationTool", MCTruthClassifierTool = BkgElectronMCTruthClassifier)
  ToolSvc += BkgElectronClassificationTool
  AugmentationTools.append(BkgElectronClassificationTool)


#====================================================================
# SKIMMING TOOL
#====================================================================

TauRequirements = '(abs(TauJets.eta) < 2.6) && (TauJets.ptFinalCalib >= 15.*GeV)'
TauBDT = '(TauJets.nTracks == 1 || TauJets.nTracks == 3) && (TauJets.DFCommonTausLoose)'
# prepare for RNN Tau ID, JetRNNSigLoose not available yet in DFTau, need to cut on the flattened score
#TauRNN = '(TauJets.nTracks == 1 && TauJets.RNNJetScoreSigTrans>0.15) || (TauJets.nTracks == 3 && TauJets.RNNJetScoreSigTrans>0.25)'
TauRNN = '(TauJets.nTracks == 1 || TauJets.nTracks == 3) && (TauJets.RNNJetScoreSigTrans>0.05)'

import sys
import PyUtils.AthFile
from AthenaCommon.AthenaCommonFlags import athenaCommonFlags
from AthenaCommon import Logging
susy3log = Logging.logging.getLogger('SUSY3')
useRNN = False
# RNN ID present in offline reconstruction for Athena-21.0.63 or higher
try:
  fileinfo = PyUtils.AthFile.fopen(athenaCommonFlags.FilesInput()[0])
  release = fileinfo.infos['metadata']['/TagInfo']['AtlasRelease']
  rel_split = release.split('.')
  if not '21.0.' in release or len(rel_split) < 3:
    susy3log.info("Incorrect AtlasRelease retrieved from metadata: {}. Expected 21.0.X".format(release))
  elif int(rel_split[2]) >= 63:
    susy3log.info("RNN tau ID present in {}. Allow JetRNNSigLoose taus in the skimming!".format(release))
    useRNN = True
except:
  susy3log.info("Could not retrieve AtlasRelease from metadata: {}".format(sys.exc_info()[0]))

# the autodetection must be extended if RNN is deployed via an AODFix
if not useRNN:
  from AODFix import AODFix
  if AODFix_willDoAODFix() and "tauid" in AODFix._aodFixInstance.latestAODFixVersion():
    susy3log.info("Tau ID AODFix is scheduled. Allow JetRNNSigLoose taus in the skimming!")
    useRNN = True

if useRNN:
  TauRequirements += ' && ( (' + TauRNN + ') )'
else:
  TauRequirements += ' && ( (' + TauBDT + ') )'

susy3log.info("Applying the following skimming to offline taus (if not in passthrough mode):")
susy3log.info(TauRequirements)

objectSelection = 'count('+TauRequirements+') >= 1'

from DerivationFrameworkTools.DerivationFrameworkToolsConf import DerivationFramework__xAODStringSkimmingTool
SUSY3SkimmingTool = DerivationFramework__xAODStringSkimmingTool( name = "SUSY3SkimmingTool",
                                                                expression = objectSelection)

ToolSvc += SUSY3SkimmingTool


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
   SeqSUSY3 += CfgMgr.DerivationFramework__DerivationKernel("SUSY3KernelSigAug",
                                                            AugmentationTools = DecorateSUSYProcess("SUSY3")
                                                            )

   from DerivationFrameworkSUSY.SUSYWeightMetadata import *


#==============================================================================
# SUSY skimming selection
#==============================================================================
SeqSUSY3 += CfgMgr.DerivationFramework__DerivationKernel(
    "SUSY3KernelSkim",
    SkimmingTools = [SUSY3SkimmingTool, SUSY3TriggerSkimmingTool]
    )


#==============================================================================
# Jet building
#==============================================================================
#re-tag PFlow jets so they have b-tagging info.
FlavorTagInit(JetCollections = ['AntiKt4EMPFlowJets'], Sequencer = SeqSUSY3)

## Adding decorations for fJVT PFlow jets                                                                                                                                                                   
getPFlowfJVT(jetalg='AntiKt4EMPFlow',sequence=SeqSUSY3, algname='JetForwardPFlowJvtToolAlg')
applyMVfJvtAugmentation(jetalg='AntiKt4EMTopo',sequence=SeqSUSY3, algname='JetForwardJvtToolBDTAlg')
#==============================================================================
# Tau truth building/matching
#==============================================================================
# now part of MCTruthCommon
if DerivationFrameworkIsMonteCarlo:
  ToolSvc.DFCommonTauTruthMatchingTool.WriteInvisibleFourMomentum = True

#==============================================================================
# Augment after skim
#==============================================================================

SeqSUSY3 += CfgMgr.DerivationFramework__DerivationKernel(
	"SUSY3KernelAug",
	AugmentationTools = AugmentationTools,
	ThinningTools = thinningTools,
)


#====================================================================
# CONTENT LIST
#====================================================================
from DerivationFrameworkCore.SlimmingHelper import SlimmingHelper
SUSY3SlimmingHelper = SlimmingHelper("SUSY3SlimmingHelper")
SUSY3SlimmingHelper.SmartCollections = ["Electrons",
                                        "Photons",
                                        "Muons",
                                        "MET_Reference_AntiKt4EMTopo",
                                        "MET_Reference_AntiKt4EMPFlow",
                                        "TauJets",
                                        "AntiKt4EMTopoJets",
                                        "AntiKt4EMPFlowJets",
                                        "AntiKt4EMPFlowJets_BTagging201810",
                                        "AntiKt4EMPFlowJets_BTagging201903",
                                        "BTagging_AntiKt4EMPFlow_201810",
                                        "BTagging_AntiKt4EMPFlow_201903",
                                        "AntiKt4EMTopoJets_BTagging201810",
                                        "BTagging_AntiKt4EMTopo_201810",
                                        "InDetTrackParticles",
                                        "PrimaryVertices",
                                        "AntiKt4TruthJets"]
SUSY3SlimmingHelper.AllVariables = ["MET_Truth", "MET_Track"]
SUSY3SlimmingHelper.ExtraVariables = [
  "BTagging_AntiKt4EMTopo_201810.MV1_discriminant.MV1c_discriminant",
  "Electrons.author.charge.ptcone20.truthOrigin.truthType.bkgMotherPdgId.bkgTruthOrigin.bkgTruthType.firstEgMotherTruthType.firstEgMotherTruthOrigin.firstEgMotherPdgId.TruthLink",
  "Muons.ptcone30.ptcone20.charge.quality.InnerDetectorPt.MuonSpectrometerPt.CaloLRLikelihood.CaloMuonIDTag.TruthLink",
  "Photons.author.Loose.Tight.TruthLink",
  "AntiKt4EMTopoJets.NumTrkPt1000.TrackWidthPt1000.NumTrkPt500.N90Constituents.Timing.Width.DFCommonJets_jetClean_VeryLooseBadLLP",
  "GSFTrackParticles.z0.d0.vz.definingParametersCovMatrix.truthOrigin.truthType",
  "InDetTrackParticles.truthOrigin.truthType",
  "CombinedMuonTrackParticles.d0.z0.vz.definingParametersCovMatrix.truthOrigin.truthType",
  "ExtrapolatedMuonTrackParticles.d0.z0.vz.definingParametersCovMatrix.truthOrigin.truthType",
  "TauJets.IsTruthMatched.truthParticleLink.truthOrigin.truthType.truthJetLink.seedTrackWidthPt500.seedTrackWidthPt1000",
  "MuonTruthParticles.barcode.decayVtxLink.e.m.pdgId.prodVtxLink.px.py.pz.recoMuonLink.status.truthOrigin.truthType",
  "HLT_xAOD__JetContainer_SplitJet.pt.eta.phi.m",
  "HLT_xAOD__BTaggingContainer_HLTBjetFex.MV2c20_discriminant.MV2c10_discriminant"
  ]


SUSY3SlimmingHelper.IncludeMuonTriggerContent = True
SUSY3SlimmingHelper.IncludeEGammaTriggerContent = True
#SUSY3SlimmingHelper.IncludeBPhysTriggerContent = True
#SUSY3SlimmingHelper.IncludeJetTauEtMissTriggerContent = True
SUSY3SlimmingHelper.IncludeJetTriggerContent = True
SUSY3SlimmingHelper.IncludeTauTriggerContent = True
SUSY3SlimmingHelper.IncludeEtMissTriggerContent = True
SUSY3SlimmingHelper.IncludeBJetTriggerContent = False


# All standard truth particle collections are provided by DerivationFrameworkMCTruth (TruthDerivationTools.py)
# Most of the new containers are centrally added to SlimmingHelper via DerivationFrameworkCore ContainersOnTheFly.py
if DerivationFrameworkIsMonteCarlo:

  SUSY3SlimmingHelper.AppendToDictionary = {'TruthTop':'xAOD::TruthParticleContainer','TruthTopAux':'xAOD::TruthParticleAuxContainer',
                                            'TruthBSM':'xAOD::TruthParticleContainer','TruthBSMAux':'xAOD::TruthParticleAuxContainer',
                                            'TruthBoson':'xAOD::TruthParticleContainer','TruthBosonAux':'xAOD::TruthParticleAuxContainer',
                                            'TruthPrimaryVertices':'xAOD::TruthVertexContainer','TruthPrimaryVerticesAux':'xAOD::TruthVertexAuxContainer'}

  SUSY3SlimmingHelper.AllVariables += ["TruthElectrons", "TruthMuons", "TruthTaus", "TruthPhotons", "TruthNeutrinos", "TruthTop", "TruthBSM", "TruthBoson", "TruthPrimaryVertices"]

SUSY3SlimmingHelper.AppendContentToStream(SUSY3Stream)
