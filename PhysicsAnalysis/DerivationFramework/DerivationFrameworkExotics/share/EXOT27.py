################################################################################
# EXOT27 - MonoHbb tests - candidate for official monoHbb+monoScalar derviation
################################################################################

from DerivationFrameworkCore.DerivationFrameworkMaster import *
from DerivationFrameworkCore.DerivationFrameworkCoreConf import DerivationFramework__DerivationKernel
from DerivationFrameworkTools.DerivationFrameworkToolsConf import DerivationFramework__xAODStringSkimmingTool, DerivationFramework__TriggerSkimmingTool, DerivationFramework__GenericObjectThinning
from TriggerMenu.api.TriggerAPI import TriggerAPI
from TriggerMenu.api.TriggerEnums import TriggerPeriod, TriggerType
from DerivationFrameworkCore.ThinningHelper import ThinningHelper
from DerivationFrameworkCore.SlimmingHelper import SlimmingHelper
from DerivationFrameworkInDet.DerivationFrameworkInDetConf import DerivationFramework__EgammaTrackParticleThinning, DerivationFramework__MuonTrackParticleThinning, DerivationFramework__TauTrackParticleThinning, DerivationFramework__JetTrackParticleThinning

# CP group common variables
import DerivationFrameworkJetEtMiss.JetCommon as JetCommon
import DerivationFrameworkJetEtMiss.ExtendedJetCommon as ExtendedJetCommon
import DerivationFrameworkJetEtMiss.METCommon as METCommon
import DerivationFrameworkEGamma.EGammaCommon as EGammaCommon
import DerivationFrameworkMuons.MuonsCommon as MuonsCommon
import DerivationFrameworkFlavourTag.HbbCommon as HbbCommon
from JetRec.JetRecStandardToolManager import jtm
from DerivationFrameworkMCTruth.DerivationFrameworkMCTruthConf import DerivationFramework__GenericTruthThinning
import DerivationFrameworkMCTruth.MCTruthCommon as MCTruthCommon
from BTagging.BTaggingFlags import BTaggingFlags
from DerivationFrameworkCore.FullListOfSmartContainers import FullListOfSmartContainers
# Create the necessary TauTruthLinks
if DerivationFrameworkIsMonteCarlo: 
  from DerivationFrameworkTau.TauTruthCommon import scheduleTauTruthTools
  scheduleTauTruthTools()

import DerivationFrameworkExotics.EXOT27Utils as EXOT27Utils

# Create a logger for this stream
import AthenaCommon
logger = AthenaCommon.Logging.logging.getLogger("EXOT27")

from collections import defaultdict

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
# As we add extra variables add them to this dictionary (key is container name,
# aux data in the value)
EXOT27ExtraVariables = defaultdict(set)
# As we add extra containers add them to these lists (note that the jets have
# their own way of doing this so anything EXOT27Jets is treated differently)
EXOT27SmartContainers = [
  "Electrons", "Photons", "AntiKt4EMTopoJets", "TauJets", "Muons",
  "PrimaryVertices", "BTagging_AntiKt4EMTopo", "MET_Reference_AntiKt4EMTopo",
  ]
EXOT27AllVariables = [
  ]
if DerivationFrameworkIsMonteCarlo:
  EXOT27AllVariables += [
    "TruthParticles",
    "MET_Truth",
    ]
EXOT27ExtraVariables["TauJets"].update(["truthJetLink", "truthParticleLink", "ptDetectorAxis", "etaDetectorAxis", "mDetectorAxis"])
def outputContainer(container, warnIfNotSmart=True):
  if container in EXOT27SmartContainers + EXOT27AllVariables:
    logger.debug("Container '{0}' already requested for output!")
    return
  if container in FullListOfSmartContainers:
    EXOT27SmartContainers.append(container)
  else:
    if warnIfNotSmart:
      logger.warning(
          "Container '{0}' added but not in smart lists!".format(container) )
      logger.warning("Will be added to AllVariables - could inflate the size!")
    EXOT27AllVariables.append(container)


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
    triggerType = TriggerType.xe | TriggerType.el | TriggerType.mu | TriggerType.g,
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

################################################################################
# Augmenting (add new objects)
################################################################################
# Create the new jet sequence in JetCommon (if it doesn't already exist)
JetCommon.OutputJets.setdefault("EXOT27Jets", [])
# Create the VR track jets
# Use buildVRJets instead of addVRJets so we can manually control the ghost
# association to the large-R jets
# do_ghost is ghost *tagging* - future improvement, not yet calibrated
vrTrackJets, vrTrackJetGhosts = HbbCommon.buildVRJets(
    sequence = EXOT27Seq, do_ghost = False, logger = logger)
JetCommon.OutputJets["EXOT27Jets"].append(vrTrackJets)
vrGhostTagTrackJets, vrGhostTagTrackJetsGhosts = HbbCommon.buildVRJets(
    sequence = EXOT27Seq, do_ghost = True, logger = logger)
JetCommon.OutputJets["EXOT27Jets"].append(vrGhostTagTrackJets)

# We need the AntiKt10LCTopo jets here though so that the trimmed jets are
# produced correctly
# *Something* is asking for the pseudo jet getters for the FR track jets so I'm
# still producing them, just not outputting them.
replace_jet_list = [
  "AntiKt2PV0TrackJets",
  "AntiKt4PV0TrackJets",
  "AntiKt10LCTopoJets"]
if JetCommon.jetFlags.useTruth:
  replace_jet_list += ["AntiKt4TruthJets"]
ExtendedJetCommon.replaceAODReducedJets(
    jetlist=replace_jet_list, sequence=EXOT27Seq, outputlist="EXOT27Jets")


# Includes the 5% pT trimmed R=1.0 jets
ExtendedJetCommon.addDefaultTrimmedJets(EXOT27Seq, "EXOT27Jets")

# Add the default VR calo jets (rho=600 GeV)
HbbCommon.addVRCaloJets(EXOT27Seq, "EXOT27Jets")

# Add the default soft drop collection
# Note that this function is a little eager - builds the jets with the VR ghost
# tag jets already in place!
ExtendedJetCommon.addCSSKSoftDropJets(EXOT27Seq, "EXOT27Jets")

OutputLargeR = [
  "AntiKt10LCTopoTrimmedPtFrac5SmallR20Jets",
  "AntiKtVR600Rmax10Rmin2LCTopoTrimmedPtFrac5SmallR20Jets",
  "AntiKt10LCTopoCSSKSoftDropBeta100Zcut10Jets"
  ]
# XAMPP seems to use the 'Width' variable from these?
for lrj in OutputLargeR:
  EXOT27ExtraVariables[lrj].update([
      "Width",
      "GhostBQuarksFinal",
      ])

# Ghost-associated the track jets to these large-R jets
toBeAssociatedTo = [
  "AntiKt10LCTopoTrimmedPtFrac5SmallR20Jets",
  "AntiKtVR600Rmax10Rmin2LCTopoTrimmedPtFrac5SmallR20Jets",
  ]
toAssociate = {
  vrTrackJetGhosts : vrTrackJetGhosts.lower(),
  vrGhostTagTrackJetsGhosts : vrGhostTagTrackJetsGhosts.lower()
}
for collection in toBeAssociatedTo:
  ungroomed, labels = EXOT27Utils.linkPseudoJetGettersToExistingJetCollection(
      EXOT27Seq, collection, toAssociate)
  EXOT27ExtraVariables[ungroomed].update(labels)
# Link the SoftDrop jets *only* to the non-ghost tag jets. The other links
# already exist and will cause a crash if scheduled again

ungroomed, labels = EXOT27Utils.linkPseudoJetGettersToExistingJetCollection(
    EXOT27Seq, "AntiKt10LCTopoCSSKSoftDropBeta100Zcut10Jets",
    {vrTrackJetGhosts : vrTrackJetGhosts.lower()})
# However, we still want to write out both so add both to the extra variables
EXOT27ExtraVariables[ungroomed].update(labels)
EXOT27ExtraVariables[ungroomed].update(vrGhostTagTrackJetsGhosts)

# Alias b-tagging container for VR track jets
BTaggingFlags.CalibrationChannelAliases += ["AntiKtVR30Rmax4Rmin02Track->AntiKtVR30Rmax4Rmin02Track,AntiKt4EMTopo"]
# Schedule for output
outputContainer("BTagging_AntiKtVR30Rmax4Rmin02Track")

# Add in Xbb tagging variables
HbbCommon.addRecommendedXbbTaggers(EXOT27Seq, ToolSvc, logger=logger)
# Add the new variables too
for extra in HbbCommon.xbbTaggerExtraVariables:
  partition = extra.partition('.')
  EXOT27ExtraVariables[partition[0]].update(partition[2].split('.') )

################################################################################
# Setup thinning (remove objects from collections)
################################################################################
EXOT27ThinningHelper = ThinningHelper("EXOT27ThinningHelper")
# Thin the navigation with the chains we're interested in
EXOT27ThinningHelper.TriggerChains = "|".join(trigger_list)
EXOT27ThinningHelper.AppendToStream(EXOT27Stream)
EXOT27ThinningTools = []

# Apply a pt cut on output large r jet collections
# TODO - revisit if this is harmful/necessary
for large_r in OutputLargeR:
  EXOT27ThinningTools.append(DerivationFramework__GenericObjectThinning(
        "EXOT27{0}ThinningTool".format(large_r),
        ThinningService = EXOT27ThinningHelper.ThinningSvc(),
        SelectionString="{0}.pt > 100*GeV".format(large_r),
        ContainerName = large_r) )

EXOT27ElectronThinning = "Electrons.DFCommonElectronsLHLooseBL"
# EXOT27MuonThinning = "Muons.pt > 10.*GeV && Muons.DFCommonGoodMuon && Muons.DFCommonMuonsPreselection"
EXOT27PhotonThinning = "Photons.pt > 10.*GeV && Photons.DFCommonPhotonsIsEMTight"
EXOT27TauJetThinning = "TauJets.pt > 10.*GeV"

# Set up the standard set of track thinning tools
EXOT27ThinningTools += [
  DerivationFramework__EgammaTrackParticleThinning(
      "EXOT27ElectronTrackParticleThinningTool",
      ThinningService = EXOT27ThinningHelper.ThinningSvc(),
      SGKey           = "Electrons",
      SelectionString = EXOT27ElectronThinning
      ),
  DerivationFramework__EgammaTrackParticleThinning(
      "EXOT27PhotonTrackParticleThinningTool",
      ThinningService = EXOT27ThinningHelper.ThinningSvc(),
      SGKey           = "Photons",
      SelectionString = EXOT27PhotonThinning
      ),
  DerivationFramework__MuonTrackParticleThinning(
      "EXOT27MuonTrackParticleThinningTool",
      ThinningService = EXOT27ThinningHelper.ThinningSvc(),
      MuonKey         = "Muons",
      # SelectionString = "Muons.pt > 10*GeV && Muons.DFCommonGoodMuon && Muons.DFCommonMuonsPreselection"
      ),
  DerivationFramework__TauTrackParticleThinning(
      "EXOT27TauTrackParticleThinningTool",
      ThinningService = EXOT27ThinningHelper.ThinningSvc(),
      TauKey          = "TauJets",
      SelectionString = EXOT27TauJetThinning
      ),
  # I asked TJ and we don't need tracks associated to jets!
  # DerivationFramework__JetTrackParticleThinning(
  #     "EXOT27JetTrackParticleThinningTool",
  #     ThinningService = EXOT27ThinningHelper.ThinningSvc(),
  #     JetKey          = "AntiKt4EMTopoJets",
  #     SelectionString = "AntiKt4EMTopoJets.DFCommonJets_Calib_pt > 15*GeV"
  #     ),
  ]

# Also thin the output objects by the same rules
EXOT27ThinningTools += [
  DerivationFramework__GenericObjectThinning(
      "EXOT27ElectronsThinningTool",
      ThinningService = EXOT27ThinningHelper.ThinningSvc(),
      SelectionString = EXOT27ElectronThinning,
      ContainerName   = "Electrons"),
  DerivationFramework__GenericObjectThinning(
      "EXOT27PhotonsThinningTool",
      ThinningService = EXOT27ThinningHelper.ThinningSvc(),
      SelectionString = EXOT27PhotonThinning,
      ContainerName   = "Photons"),
  DerivationFramework__GenericObjectThinning(
      "EXOT27TauJetsThinningTool",
      ThinningService = EXOT27ThinningHelper.ThinningSvc(),
      SelectionString = EXOT27TauJetThinning,
      ContainerName   = "TauJets"),
  ]


# TODO (perhaps): truth thinning
# What I have here is extremely simplistic - designed to at least have what I
# need for my immediate studies
if DerivationFrameworkIsMonteCarlo:
  truth_sel_list = ["abs(TruthParticles.pdgId) == {0}".format(ii) for ii in range(5,26)]
  truth_sel = "||".join(map("({0})".format, truth_sel_list) )
  EXOT27ThinningTools += [
    DerivationFramework__GenericTruthThinning(
        "EXOT27TruthThinningTool",
        ThinningService = EXOT27ThinningHelper.ThinningSvc(),
        ParticleSelectionString = truth_sel,
        PreserveDescendants     = True),
    ]
  
for tool in EXOT27ThinningTools:
  ToolSvc += tool


################################################################################
# Setup secondary skimming (remove whole events)
################################################################################
EXOT27SkimmingTools = []
# string selection
sel_list = []
# Common SR selection
# Resolved requirement - analysis level selection is 2 central jets with pT > 45
# GeV. Use 30 GeV and |eta| < 2.8 to allow for future differences in calibration
sel_list.append("count((AntiKt4EMTopoJets.DFCommonJets_Calib_pt > 30.*GeV) && " +
    "(abs(AntiKt4EMTopoJets.DFCommonJets_Calib_eta) < 2.8)) >= 2")
# Merged requirement - analysis level selection is 1 central large-R jet with pT
# > 200 GeV. Use 100 GeV and |eta| < 2.4 to allow for future differences in
# calibration. Do this for all of the large-R jet collections that are output
sel_list += ["count(({0}.pt > 100.*GeV) && (abs({0}.eta) < 2.4)) >= 1".format(
    lrj) for lrj in OutputLargeR]

# This incantation gives us an OR'd string, encasing each expression in brackets
# to ensure that everything works as expected
sel_string = " || ".join(map("({0})".format, sel_list) )
if sel_string:
  # Empty strings are falsey so this will only be executed if sel_list
  # contained something
  EXOT27StringSkimmingTool = DerivationFramework__xAODStringSkimmingTool(
      "EXOT27JetSkimmingTool",
      expression = sel_string
      )
  EXOT27SkimmingTools.append(EXOT27StringSkimmingTool)


for tool in EXOT27SkimmingTools:
  ToolSvc += tool

EXOT27Seq += CfgMgr.DerivationFramework__DerivationKernel(
    "EXOT27SecondaryKernel",
    SkimmingTools = EXOT27SkimmingTools,
    ThinningTools = EXOT27ThinningTools
    )




################################################################################
# Setup slimming (remove variables/collections)
################################################################################
EXOT27SlimmingHelper = SlimmingHelper("EXOT27SlimmingHelper")
EXOT27SlimmingHelper.SmartCollections += EXOT27SmartContainers
EXOT27SlimmingHelper.AllVariables += EXOT27AllVariables
JetCommon.addJetOutputs(
    slimhelper = EXOT27SlimmingHelper,
    contentlist=["EXOT27Jets"],
    smartlist = [
      "AntiKt10LCTopoTrimmedPtFrac5SmallR20Jets",
      "AntiKt10LCTopoCSSKSoftDropBeta100Zcut10Jets"
    ],
    vetolist = [
    "AntiKt2PV0TrackJets",
    "AntiKt4PV0TrackJets",
    "AntiKt10LCTopoJets",
    "AntiKt10TruthJets",
    "AntiKtVR600Rmax10Rmin2LCTopoJets",
    "AntiKtVR600Rmax10Rmin2PV0TrackTrimmedPtFrac5SmallR20Jets",
    "AntiKtVR600Rmax10Rmin2TruthTrimmedPtFrac5SmallR20Jets",
    "AntiKt10LCTopoCSSKJets"]
    )

EXOT27SlimmingHelper.ExtraVariables += [
  "{0}.{1}".format(k, '.'.join(v) ) for k, v in EXOT27ExtraVariables.iteritems()
]

EXOT27SlimmingHelper.IncludeMuonTriggerContent = True
EXOT27SlimmingHelper.IncludeEGammaTriggerContent = True
EXOT27SlimmingHelper.InlcudeEtMissTriggerContent = True
EXOT27SlimmingHelper.AppendContentToStream(EXOT27Stream)

################################################################################
# Finalise
################################################################################
# Any remaining tasks, e.g. adding the kernels to the stream  
EXOT27Stream.AcceptAlgs(["EXOT27SecondaryKernel"])
