from DerivationFrameworkCore.DerivationFrameworkMaster import *
from DerivationFrameworkJetEtMiss.JetCommon import *
from DerivationFrameworkJetEtMiss.ExtendedJetCommon import *
from DerivationFrameworkJetEtMiss.METCommon import *

from DerivationFrameworkCore.ThinningHelper import ThinningHelper
from DerivationFrameworkInDet.DerivationFrameworkInDetConf import DerivationFramework__TrackParticleThinning, DerivationFramework__MuonTrackParticleThinning, DerivationFramework__EgammaTrackParticleThinning, DerivationFramework__TauTrackParticleThinning
from DerivationFrameworkCore.SlimmingHelper import SlimmingHelper

class METTriggerDerivationContentManager(object):
  def __init__(self, streamName, stream, jetAlgorithms = ["AntiKt4EMTopo", "AntiKt4EMPFlow"], trackThreshold = 10):
    self.streamName = streamName
    self.trackThreshold = trackThreshold
    self.makeThinningTools(stream)
    self.makeSlimmingTools(stream, jetAlgorithms)

##################
# Thinning Tools #
##################
  def makeThinningTools(self, stream):
    streamName = self.streamName
    self.thinningHelper = ThinningHelper(streamName + "ThinningHelper")
    self.thinningHelper.AppendToStream(stream)
    self.thinningTools = [
      DerivationFramework__TrackParticleThinning(
          streamName + "TPThinningTool",
          ThinningService        = self.thinningHelper.ThinningSvc(),
          SelectionString        = "InDetTrackParticles.pt > {0}*GeV".format(self.trackThreshold),
          InDetTrackParticlesKey = "InDetTrackParticles"),
      DerivationFramework__MuonTrackParticleThinning(
          streamName + "MuonTPThinningTool",
          ThinningService        = self.thinningHelper.ThinningSvc(),
          MuonKey                = "Muons",
          InDetTrackParticlesKey = "InDetTrackParticles"),
      DerivationFramework__EgammaTrackParticleThinning(
          streamName + "ElectronTPThinningTool",
          ThinningService        = self.thinningHelper.ThinningSvc(),
          SGKey                  = "Electrons",
          InDetTrackParticlesKey = "InDetTrackParticles"),
      DerivationFramework__EgammaTrackParticleThinning(
          streamName + "PhotonTPThinningTool",
          ThinningService        = self.thinningHelper.ThinningSvc(),
          SGKey                  = "Photons",
          InDetTrackParticlesKey = "InDetTrackParticles"),
      DerivationFramework__TauTrackParticleThinning(
          streamName + "TauTPThinningTool",
          ThinningService        = self.thinningHelper.ThinningSvc(),
          TauKey                 = "TauJets",
          InDetTrackParticlesKey = "InDetTrackParticles")
    ]

##################
# Slimming Tools #
##################
  def makeSlimmingTools(self, stream, jetAlgorithms):
    hltJets = ["a4tclcwsubjesFS", "a4tclcwsubjesISFS"]
    # Remove the hlt jets from the default list - they're too large
    hltJets = []
    streamName = self.streamName
    self.slimmingHelper = SlimmingHelper(streamName + "SlimmingHelper")
    self.slimmingHelper.SmartCollections = ["Electrons", "Muons", "Photons", "TauJets", "PrimaryVertices"] + ["{0}Jets".format(a) for a in jetAlgorithms] + ["BTagging_{0}".format(a) for a in jetAlgorithms] + ["MET_Reference_{0}".format(a) for a in jetAlgorithms]
    self.slimmingHelper.AllVariables = [
      # "CaloCalTopoClusters",
      "HLT_xAOD__TrigMissingETContainer_TrigEFMissingET",
      "HLT_xAOD__TrigMissingETContainer_TrigEFMissingET_mht",
      "HLT_xAOD__TrigMissingETContainer_TrigEFMissingET_topocl_PS",
      "HLT_xAOD__TrigMissingETContainer_TrigEFMissingET_topocl_PUC",
      "HLT_xAOD__TrigMissingETContainer_TrigEFMissingET_topocl",
      "LVL1EnergySumRoI",
      "LVL1JetRoIs",
      "LVL1JetEtRoI"] + ["MET_Core_{0}".format(a) for a in jetAlgorithms] + ["METAssoc_{0}".format(a) for a in jetAlgorithms] + ["HLT_xAOD__JetContainer_{0}".format(j) for j in hltJets]
    # self.slimmingHelper.AppendContentToStream(stream)
