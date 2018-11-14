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
EXOT27AcceptAlgs = []
# As we add extra variables add them to this dictionary (key is container name,
# aux data in the value)
EXOT27ExtraVariables = defaultdict(set)
# As we add extra containers add them to these lists (note that the jets have
# their own way of doing this so anything EXOT27Jets is treated differently)
EXOT27SmartContainers = [
  "Electrons", "Photons", "AntiKt4EMTopoJets", "TauJets", "Muons",
  "InDetTrackParticles", "PrimaryVertices", "BTagging_AntiKt4EMTopo",
  ]
EXOT27AllVariables = [
  "MET_Core_AntiKt4EMTopo",
  "METAssoc_AntiKt4EMTopo",
  ]
if DerivationFrameworkIsMonteCarlo:
  EXOT27AllVariables += [
    "TruthParticles",
    "MET_Truth",
    ]
# Extra variables for MET
EXOT27ExtraVariables["Muons"].update(["clusterLink", "EnergyLoss", "energyLossType"])
EXOT27ExtraVariables["TauJets"].update(["truthJetLink", "truthParticleLink", "ptDetectorAxis", "etaDetectorAxis", "mDetectorAxis"])
EXOT27ExtraVariables["AntiKt4EMTopoJets"].update(["NumTrkPt500", "SumPtTrkPt500", "EnergyPerSampling", "EMFrac"])
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

# I don't know if we actually care about the track jets any more but I'm adding
# them in for now. Same goes for the truth jets
# We need the AntiKt10LCTopo jets here though so that the trimmed jets are
# produced correctly
replace_jet_list = [
  "AntiKt2PV0TrackJets",
  "AntiKt4PV0TrackJets",
  "AntiKt10LCTopoJets"]
if JetCommon.jetFlags.useTruth:
  replace_jet_list += ["AntiKt4TruthJets"]
ExtendedJetCommon.replaceAODReducedJets(
    jetlist=replace_jet_list, sequence=EXOT27Seq, outputlist="EXOT27Jets")
# Add the b-tagging for the AntiKt2 jets
outputContainer("BTagging_AntiKt2Track")


# Includes the 5% pT trimmed R=1.0 jets
ExtendedJetCommon.addDefaultTrimmedJets(EXOT27Seq, "EXOT27Jets")

# Add the default VR calo jets (rho=600 GeV)
HbbCommon.addVRCaloJets(EXOT27Seq, "EXOT27Jets")

# Add the default soft drop collection
# Note that this function is a little eager - builds the jets with the VR ghost
# tag jets already in place!
ExtendedJetCommon.addCSSKSoftDropJets(EXOT27Seq, "EXOT27Jets")

# Add the R-scan jets
JetCommon.addStandardJets(
    "AntiKt", 0.2, "LCTopo", mods="lctopo_ungroomed", calibOpt="none",
    ghostArea=0.1, ptmin=2000, ptminFilter=7000,
    algseq=EXOT27Seq, outputGroup="EXOT27Jets")

# Ghost-associated the track jets to these large-R jets
toBeAssociatedTo = [
  "AntiKt10LCTopoTrimmedPtFrac5SmallR20Jets",
  "AntiKtVR600Rmax10Rmin2LCTopoTrimmedPtFrac5SmallR20Jets",
  "AntiKt2LCTopoJets"
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
for large_r in [
  "AntiKt10LCTopoTrimmedPtFrac5SmallR20Jets",
  "AntiKtVR600Rmax10Rmin2LCTopoTrimmedPtFrac5SmallR20Jets",
  "AntiKt10LCTopoCSSKSoftDropBeta100Zcut10Jets"
  ]:
  EXOT27ThinningTools.append(DerivationFramework__GenericObjectThinning(
        "{0}ThinningTool".format(large_r),
        SelectionString="{0}.pt > 100*GeV".format(large_r),
        ContainerName = large_r) )


# Set up the standard set of track thinning tools
EXOT27ThinningTools += [
  DerivationFramework__EgammaTrackParticleThinning(
      "EXOT27ElectronTrackParticleThinningTool",
      ThinningService = EXOT27ThinningHelper.ThinningSvc(),
      SGKey           = "Electrons",
      # SelectionString = "Electrons.pt > 10*GeV && Electrons.DFCommonElectronsLHLooseBL"
      ),
  DerivationFramework__EgammaTrackParticleThinning(
      "EXOT27PhotonTrackParticleThinningTool",
      ThinningService = EXOT27ThinningHelper.ThinningSvc(),
      SGKey           = "Photons",
      # SelectionString = "Photons.pt > 10*GeV && Photons.DFCommonPhotonsIsEMTight"
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
      # SelectionString = "TauJets.pt > 10*GeV"
      ),
  DerivationFramework__JetTrackParticleThinning(
      "EXOT27JetTrackParticleThinningTool",
      ThinningService = EXOT27ThinningHelper.ThinningSvc(),
      JetKey          = "AntiKt4EMTopoJets",
      SelectionString = "AntiKt4EMTopoJets.DFCommonJets_Calib_pt > 15*GeV"
      ),
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
EXOT27SlimmingHelper.SmartCollections += EXOT27SmartContainers
EXOT27SlimmingHelper.AllVariables += EXOT27AllVariables
JetCommon.addJetOutputs(
    slimhelper = EXOT27SlimmingHelper,
    contentlist=["EXOT27Jets"],
    smartlist = [
      "AntiKt10LCTopoTrimmedPtFrac5SmallR20Jets",
      "AntiKt2LCTopoJets",
      "AntiKt10LCTopoCSSKSoftDropBeta100Zcut10Jets"
    ],
    vetolist = [
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
EXOT27SlimmingHelper.AppendToDictionary.update( {
    "BTagging_AntiKtVR30Rmax4Rmin02Track"    : "xAOD::BTaggingContainer",
    "BTagging_AntiKtVR30Rmax4Rmin02TrackAux" : "xAOD::BTaggingAuxContainer",
    } )

EXOT27SlimmingHelper.IncludeMuonTriggerContent = True
EXOT27SlimmingHelper.IncludeEGammaTriggerContent = True
EXOT27SlimmingHelper.InlcudeEtMissTriggerContent = True
EXOT27SlimmingHelper.AppendContentToStream(EXOT27Stream)

################################################################################
# Finalise
################################################################################
# Any remaining tasks, e.g. adding the kernels to the stream  
EXOT27Stream.AcceptAlgs(EXOT27AcceptAlgs)
