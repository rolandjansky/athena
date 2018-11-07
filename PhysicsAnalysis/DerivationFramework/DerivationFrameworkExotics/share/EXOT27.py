################################################################################
# EXOT27 - MonoHbb tests (private only!)
################################################################################

from DerivationFrameworkCore.DerivationFrameworkMaster import *
from DerivationFrameworkCore.DerivationFrameworkCoreConf import DerivationFramework__DerivationKernel
from DerivationFrameworkTools.DerivationFrameworkToolsConf import DerivationFramework__xAODStringSkimmingTool, DerivationFramework__TriggerSkimmingTool
from TriggerMenu.api.TriggerAPI import TriggerAPI
from TriggerMenu.api.TriggerEnums import TriggerPeriod, TriggerType
from DerivationFrameworkCore.ThinningHelper import ThinningHelper
from DerivationFrameworkCore.SlimmingHelper import SlimmingHelper
from DerivationFrameworkInDet.DerivationFrameworkInDetConf import DerivationFramework__EgammaTrackParticleThinning, DerivationFramework__MuonTrackParticleThinning, DerivationFramework__TauTrackParticleThinning, DerivationFramework__JetTrackParticleThinning

# CP group common variables
from DerivationFrameworkJetEtMiss.JetCommon import *
from DerivationFrameworkJetEtMiss.ExtendedJetCommon import *
# from DerivationFrameworkJetEtMiss.PFlowJetCommon import *
from DerivationFrameworkJetEtMiss.METCommon import *
from DerivationFrameworkEGamma.EGammaCommon import *
from DerivationFrameworkMuons.MuonsCommon import *


################################################################################
# Setup the stream and our private sequence
################################################################################
streamName   = derivationFlags.WriteDAOD_EXOT27Stream.StreamName
fileName     = buildFileName( derivationFlags.WriteDAOD_EXOT27Stream )
EXOT27Stream = MSMgr.NewPoolRootStream( streamName, fileName )
# Create a private sequence to run everything in.
# The advantage of this is that we can run any time-intensive operations (e.g
# jet finding) only if the kernel's selections pass.
EXOT27Seq = CfgMgr.AthSequencer("EXOT27Sequence")
DerivationFrameworkJob += EXOT27Seq
# As we create skimming algs, etc add them to this list
EXOT27AcceptAlgs = []

################################################################################
# Setup preliminary skimming (remove whole events)
################################################################################
# NOTE:
# I've split the skimming into two parts, one which can be run before the more
# CPU intensive operations (e.g. custom jet finding) and one which has to run
# after those
# This section is just for the preliminary skimming
EXOT27PreliminarySkimmingTools = []

# trigger selection
trigger_all_periods = (
      TriggerPeriod.y2015 | TriggerPeriod.y2016 | TriggerPeriod.y2017
      | TriggerPeriod.y2018 | TriggerPeriod.future2e34)
# Set live fraction to 0.95 to catch any short, accidental prescales
trigger_list = TriggerAPI.getLowestUnprescaledAnyPeriod(
    trigger_all_periods,
    triggerType = TriggerType.xe | TriggerType.el | TriggerType.mu,
    livefraction = 0.95)
EXOT27TriggerSkimmingTool = DerivationFramework__TriggerSkimmingTool(
    "EXOT27TriggerSkimmingTool",
    TriggerListOR = trigger_list
    )
EXOT27PreliminarySkimmingTools.append(EXOT27TriggerSkimmingTool)

# Add the tools to the ToolSvc
for tool in EXOT27PreliminarySkimmingTools:
  ToolSvc += tool

EXOT27Seq += CfgMgr.DerivationFramework__DerivationKernel(
    "EXOT27PreliminaryKernel",
    SkimmingTools = EXOT27PreliminarySkimmingTools
    )
EXOT27AcceptAlgs.append("EXOT27PreliminaryKernel")


################################################################################
# Setup thinning (remove objects from collections)
################################################################################
EXOT27ThinningHelper = ThinningHelper("EXOT27ThinningHelper")
EXOT27ThinningHelper.AppendToStream(EXOT27Stream)
EXOT27ThinningTools = []

# Set up the standard set of track thinning tools
EXOT27ThinningTools += [
  DerivationFramework__EgammaTrackParticleThinning(
      "EXOT27ElectronTrackParticleThinningTool",
      ThinningService = EXOT27ThinningHelper.ThinningSvc(),
      SGKey           = "Electrons",
      SelectionString = "Electrons.pt > 10*GeV"
      # SelectionString = "Electrons.pt > 10*GeV && Electrons.DFCommonElectronsLooseBL"
      ),
  DerivationFramework__EgammaTrackParticleThinning(
      "EXOT27PhotonTrackParticleThinningTool",
      ThinningService = EXOT27ThinningHelper.ThinningSvc(),
      SGKey           = "Photons",
      SelectionString = "Photons.pt > 10*GeV && Photons.DFCommonPhotonsIsEMTight"
      ),
  DerivationFramework__MuonTrackParticleThinning(
      "EXOT27MuonTrackParticleThinningTool",
      ThinningService = EXOT27ThinningHelper.ThinningSvc(),
      MuonKey         = "Muons",
      SelectionString = "Muons.pt > 10*GeV && Muons.DFCommonGoodMuon && Muons.DFCommonMuonsPreselection"
      ),
  DerivationFramework__TauTrackParticleThinning(
      "EXOT27TauTrackParticleThinningTool",
      ThinningService = EXOT27ThinningHelper.ThinningSvc(),
      TauKey          = "TauJets",
      SelectionString = "TauJets.pt > 10*GeV"
      ),
  DerivationFramework__JetTrackParticleThinning(
      "EXOT27JetTrackParticleThinningTool",
      ThinningService = EXOT27ThinningHelper.ThinningSvc(),
      JetKey          = "AntiKt4EMTopoJets",
      SelectionString = "AntiKt4EMTopoJets.DFCommonJets_Calib_pt > 10*GeV"
      )
  ]

ToolSvc += EXOT27ThinningTools

# TODO (perhaps): truth thinning


################################################################################
# Setup secondary skimming (remove whole events)
################################################################################
EXOT27SkimmingTools = []
# jet selection
jet_sel_list = []
# This incantation gives us an OR'd string, encasing each expression in brackets
# to ensure that everything works as expected
jet_sel = " || ".join(map("({0})".format, jet_sel_list) )
if jet_sel:
  # Empty strings are falsey so this will only be executed if jet_sel_list
  # contained something
  EXOT27JetSkimmingTool = DerivationFramework__xAODStringSkimmingTool(
      "EXOT27JetSkimmingTool",
      expression = jet_sel
      )
  EXOT27SkimmingTools.append(EXOT27JetSkimmingTool)

EXOT27Seq += CfgMgr.DerivationFramework__DerivationKernel(
    "EXOT27SecondaryKernel",
    SkimmingTools = EXOT27SkimmingTools,
    ThinningTools = EXOT27ThinningTools
    )
EXOT27AcceptAlgs.append("EXOT27SecondaryKernel")

################################################################################
# Setup slimming (remove variables/collections)
################################################################################
EXOT27SlimmingHelper = SlimmingHelper("EXOT27SlimmingHelper")
EXOT27SlimmingHelper.SmartCollections = [
  "Electrons",
  "Photons",
  "AntiKt4EMTopoJets",
  "TauJets",
  "Muons",
  "InDetTrackParticles",
  "PrimaryVertices",
  "BTagging_AntiKt4EMTopo"
  ]
EXOT27SlimmingHelper.AllVariables = [
  "MET_Core_AntiKt4EMTopo",
  "METAssoc_AntiKt4EMTopo"
  ]
EXOT27SlimmingHelper.AppendContentToStream(EXOT27Stream)

################################################################################
# Finalise
################################################################################
# Any remaining tasks, e.g. adding the kernels to the stream  
EXOT27Stream.AcceptAlgs(EXOT27AcceptAlgs)
