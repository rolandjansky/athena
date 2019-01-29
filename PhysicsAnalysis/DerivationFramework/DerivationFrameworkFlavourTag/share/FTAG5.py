#====================================================================
# FTAG5.py
# It requires the reductionConf flag FTAG5 in Reco_tf.py
#====================================================================

# Set up common services and job object.

# Framework core
from DerivationFrameworkCore.DerivationFrameworkMaster import (
    DerivationFrameworkJob, buildFileName)
from DerivationFrameworkCore.ThinningHelper import (
    ThinningHelper)
from DerivationFrameworkCore.SlimmingHelper import SlimmingHelper
from DerivationFrameworkCore.DerivationFrameworkCoreConf import (
    DerivationFramework__DerivationKernel)

from DerivationFrameworkTools.DerivationFrameworkToolsConf import (
    DerivationFramework__xAODStringSkimmingTool)

# JetEtMiss
from DerivationFrameworkJetEtMiss.JetCommon import (
    OutputJets, addJetOutputs, addSoftDropJets)
from DerivationFrameworkJetEtMiss.ExtendedJetCommon import (
    addDefaultTrimmedJets, replaceAODReducedJets, addConstModJets,
    addCSSKSoftDropJets)
from JetRec.JetRecStandard import jtm

# tracking
from TrkVertexFitterUtils.TrkVertexFitterUtilsConf import (
    Trk__TrackToVertexIPEstimator)

# flavor tagging
from DerivationFrameworkFlavourTag.HbbCommon import (
    addVRJets, addExKtCoM, addRecommendedXbbTaggers, xbbTaggerExtraVariables)
from DerivationFrameworkFlavourTag import BTaggingContent as bvars
from DerivationFrameworkJetEtMiss.JSSVariables import JSSHighLevelVariables

from FlavorTagDiscriminants.discriminants import complex_jet_discriminants

# logging
from AthenaCommon import Logging
ftag5_log = Logging.logging.getLogger('FTAG5LOG')
def log_setup(algo):
    ftag5_log.info('set up {}'.format(algo))

#====================================================================
# SET UP STREAM
#====================================================================

# The base name (DAOD_FTAG5 here) must match the string in
streamName = derivationFlags.WriteDAOD_FTAG5Stream.StreamName
fileName   = buildFileName( derivationFlags.WriteDAOD_FTAG5Stream )
FTAG5Stream = MSMgr.NewPoolRootStream( streamName, fileName )
# Only events that pass the filters listed below are written out.
# Name must match that of the kernel above
# AcceptAlgs  = logical OR of filters
# RequireAlgs = logical AND of filters
FTAG5Stream.AcceptAlgs(["FTAG5Kernel"])


#====================================================================
# SKIMMING TOOLS
#====================================================================
skim_expr = 'count(AntiKt10LCTopoTrimmedPtFrac5SmallR20Jets.pt > 250 * GeV && (abs(AntiKt10LCTopoTrimmedPtFrac5SmallR20Jets.eta) < 2.0)) > 0'

FTAG5StringSkimmingTool = DerivationFramework__xAODStringSkimmingTool(
    name = "FTAG5StringSkimmingTool",
    expression = skim_expr)

ToolSvc += FTAG5StringSkimmingTool
log_setup(FTAG5StringSkimmingTool)

#=====================================================================
# Thinning tools
#=====================================================================

FTAG5ThinningHelper = ThinningHelper( "FTAG5ThinningHelper" )
FTAG5ThinningHelper.TriggerChains = ''
FTAG5ThinningHelper.AppendToStream( FTAG5Stream )


from DerivationFrameworkFlavourTag.DerivationFrameworkFlavourTagConf import (
    DerivationFramework__HbbTrackThinner as HbbThinner )
FTAG5HbbThinningTool = HbbThinner(
    name = "FTAG5HbbThinningTool",
    thinningService = FTAG5ThinningHelper.ThinningSvc(),
    largeJetPtCut = 200e3,
    largeJetEtaCut = 2.1,
    smallJetPtCut = 7e3,
    nLeadingSubjets = 3,
    addSubjetGhosts = True,
    addConstituents = True,
    addConeAssociated = True)
ToolSvc += FTAG5HbbThinningTool
log_setup(FTAG5HbbThinningTool)

#====================================================================
# TRUTH SETUP
#====================================================================
if globalflags.DataSource()!='data':
    from DerivationFrameworkMCTruth.MCTruthCommon import addStandardTruthContents, addHFAndDownstreamParticles
    addStandardTruthContents()
    addHFAndDownstreamParticles()

#====================================================================
# AUGMENTATION TOOLS
# ( AUGMENTATION = adding information to the output DxAOD that is not found in the input file )
#====================================================================

#make IPE tool for TrackToVertexWrapper
FTAG5IPETool = Trk__TrackToVertexIPEstimator(name = "FTAG5IPETool")
ToolSvc += FTAG5IPETool
log_setup(FTAG5IPETool)

#====================================================================
# CREATE PRIVATE SEQUENCE
#====================================================================

FTAG5Seq = CfgMgr.AthSequencer("FTAG5Sequence");
DerivationFrameworkJob += FTAG5Seq

#====================================================================
# Basic Jet Collections
#====================================================================

#put custom jet names here
FTAG5BTaggedJets = [
    "AntiKtVR30Rmax4Rmin02TrackJets",
    "AntiKtVR30Rmax4Rmin02TrackGhostTagJets",
    "AntiKt10LCTopoTrimmedPtFrac5SmallR20ExKt2SubJets",
    "AntiKt10LCTopoTrimmedPtFrac5SmallR20ExKt3SubJets",
    "AntiKt10LCTopoTrimmedPtFrac5SmallR20ExKt2GASubJets",
    "AntiKt10LCTopoTrimmedPtFrac5SmallR20ExKt3GASubJets",
    "AntiKt10LCTopoTrimmedPtFrac5SmallR20ExCoM2SubJets"]
OutputJets["FTAG5"] = FTAG5BTaggedJets[:]

# I don't understand why we need some of these. We don't use
# AntiKt4PV0TrackJets in the output, but without them we get a crash
reducedJetList = ["AntiKt2PV0TrackJets",
                  "AntiKt4PV0TrackJets", # <- Crashes without this,
                  "AntiKt10LCTopoJets", # <- while building this collection
                  "AntiKt4TruthJets"]

extendedFlag = 0 # --- = 0 for Standard Taggers & =1 for ExpertTaggers
replaceAODReducedJets(reducedJetList,FTAG5Seq,"FTAG5", extendedFlag)

addDefaultTrimmedJets(FTAG5Seq,"FTAG5",dotruth=True)

#===================================================================
# Variable Radius (VR) Jets
#===================================================================

# Create variable-R trackjets and dress AntiKt10LCTopo with ghost VR-trkjet
# Note that the ghost association to the 'AntiKt10LCTopo' jets is
# hardcoded within this function "for now".
addVRJets(FTAG5Seq, logger=ftag5_log)
addVRJets(FTAG5Seq, logger=ftag5_log, do_ghost=True)

# alias for VR
BTaggingFlags.CalibrationChannelAliases += ["AntiKtVR30Rmax4Rmin02Track->AntiKtVR30Rmax4Rmin02Track,AntiKt4EMTopo"]

#===================================================================
# Add SoftDrop Jets
#===================================================================

addCSSKSoftDropJets(FTAG5Seq, "FTAG5")

#===================================================================
# ExKt subjets for each trimmed large-R jet
#===================================================================
ExKtJetCollection__FatJet = "AntiKt10LCTopoTrimmedPtFrac5SmallR20Jets"

addExKtCoM(FTAG5Seq, ToolSvc, ExKtJetCollection__FatJet, 2, doGhostAssoc=True, doTrackSubJet=False)
addExKtCoM(FTAG5Seq, ToolSvc, ExKtJetCollection__FatJet, 3, doGhostAssoc=True, doTrackSubJet=False)

addExKtCoM(FTAG5Seq, ToolSvc, ExKtJetCollection__FatJet, 2, doTrackSubJet=False)
addExKtCoM(FTAG5Seq, ToolSvc, ExKtJetCollection__FatJet, 3, doTrackSubJet=False)

addExKtCoM(FTAG5Seq, ToolSvc, ExKtJetCollection__FatJet, 2, doTrackSubJet=False, subjetAlgName = "CoM")

BTaggingFlags.CalibrationChannelAliases += ["AntiKt10LCTopoTrimmedPtFrac5SmallR20ExKt2Sub->AntiKt4LCTopo,AntiKt4TopoEM,AntiKt4EMTopo",
                                            "AntiKt10LCTopoTrimmedPtFrac5SmallR20ExKt3Sub->AntiKt4LCTopo,AntiKt4TopoEM,AntiKt4EMTopo",
                                            "AntiKt10LCTopoTrimmedPtFrac5SmallR20ExKt2GASub->AntiKt4LCTopo,AntiKt4TopoEM,AntiKt4EMTopo",
                                            "AntiKt10LCTopoTrimmedPtFrac5SmallR20ExKt3GASub->AntiKt4LCTopo,AntiKt4TopoEM,AntiKt4EMTopo",
                                            "AntiKt10LCTopoTrimmedPtFrac5SmallR20ExCoM2Sub->AntiKt4LCTopo,AntiKt4TopoEM,AntiKt4EMTopo"]

#==================================================================
# Augment tracks and jets with additional information
#==================================================================

FTAG5Seq += CfgMgr.BTagVertexAugmenter()
FTAG5Seq += CfgMgr.BTagTrackAugmenter(
    "BTagTrackAugmenter",
    OutputLevel=INFO,
    TrackToVertexIPEstimator = FTAG5IPETool,
    SaveTrackVectors = True,
)

for jc in FTAG5BTaggedJets:
    FTAG5Seq += CfgMgr.BTagJetAugmenterAlg(
           "FTAG5JetAugmenter_"+jc,
           JetCollectionName=jc
    )

#================================================================
# Add Hbb tagger
#================================================================

addRecommendedXbbTaggers(FTAG5Seq, ToolSvc, ftag5_log)

#====================================================================
# CREATE THE DERIVATION KERNEL ALGORITHM AND PASS THE ABOVE TOOLS
#====================================================================

FTAG5Seq += CfgMgr.DerivationFramework__DerivationKernel(
    "FTAG5Kernel",
    SkimmingTools = [FTAG5StringSkimmingTool],
    ThinningTools = [FTAG5HbbThinningTool],
    AugmentationTools = []
)


#====================================================================
# Add slimming tools
#====================================================================

FTAG5SlimmingHelper = SlimmingHelper("FTAG5SlimmingHelper")

fatJetCollections = [
    "AntiKt10LCTopoTrimmedPtFrac5SmallR20Jets",
    "AntiKt10LCTopoCSSKSoftDropBeta100Zcut10Jets",
]
FTAG5SlimmingHelper.SmartCollections = [
    "Muons",
    "InDetTrackParticles",
    "BTagging_AntiKtVR30Rmax4Rmin02Track_expert",
    "BTagging_AntiKtVR30Rmax4Rmin02TrackGhostTag_expert",
    "BTagging_AntiKt10LCTopoTrimmedPtFrac5SmallR20ExKt2Sub_expert",
    "BTagging_AntiKt10LCTopoTrimmedPtFrac5SmallR20ExKt3Sub_expert", 
    "BTagging_AntiKt10LCTopoTrimmedPtFrac5SmallR20ExKt2GASub_expert",
    "BTagging_AntiKt10LCTopoTrimmedPtFrac5SmallR20ExKt3GASub_expert",   
    "BTagging_AntiKt10LCTopoTrimmedPtFrac5SmallR20ExCoM2Sub_expert"]
FTAG5SlimmingHelper.SmartCollections += fatJetCollections

for fatJetCollection in fatJetCollections:
    jssVariables = ['.'.join([fatJetCollection] + JSSHighLevelVariables) ]
    FTAG5SlimmingHelper.ExtraVariables += jssVariables

FTAG5SlimmingHelper.ExtraVariables += [
    "InDetTrackParticles.truthMatchProbability.x.y.z.vx.vy.vz",
    "InDetTrackParticles.numberOfContribPixelLayers.numberOfTRTHits.numberOfInnermostPixelLayerSharedHits.numberOfNextToInnermostPixelLayerSharedHits",
    "InDetTrackParticles.numberOfPixelSplitHits.numberOfInnermostPixelLayerSplitHits.numberOfNextToInnermostPixelLayerSplitHits",
    "InDetTrackParticles.hitPattern.radiusOfFirstHit",
    "AntiKt10LCTopoJets.GhostVR30Rmax4Rmin02TrackJet",
    "AntiKt10LCTopoJets.GhostVR30Rmax4Rmin02TrackJetGhostTag",
    "InDetTrackParticles.btag_z0.btag_d0.btag_ip_d0.btag_ip_z0.btag_ip_phi.btag_ip_d0_sigma.btag_ip_z0_sigma.btag_track_displacement.btag_track_momentum",
]
FTAG5SlimmingHelper.ExtraVariables += xbbTaggerExtraVariables

# add the extra variables that come from the BTagJetAugmenterAlg
extra_btag = list(complex_jet_discriminants)
extra_btag += ['JetFitter_JFvertices', 'SV1_vertices']
for jc in FTAG5BTaggedJets:
    FTAG5SlimmingHelper.ExtraVariables.append('.'.join([
    	"BTagging_"+jc.replace("Jets","")] + extra_btag))

# add some more extra variables for ghost associated particles
ghost_particles = [
    'HBosons',
    'WBosons',
    'ZBosons',
    'TQuarksFinal',
    'BHadronsFinal',
    'CHadronsFinal',
]
ghost_counts = ['Ghost' + gp + 'Count' for gp in ghost_particles]
ghost_pts = ['Ghost' + gp + 'Pt' for gp in ghost_particles]
ghost_subjets = ['GhostVR30Rmax4Rmin02TrackJetGhostTag']
for jc in ['AntiKt10LCTopoJets', 'AntiKt10LCTopoCSSKJets']:
    FTAG5SlimmingHelper.ExtraVariables.append(
        '.'.join([jc] + ghost_counts + ghost_pts + ghost_subjets))

FTAG5SlimmingHelper.IncludeMuonTriggerContent = False
FTAG5SlimmingHelper.IncludeEGammaTriggerContent = False
FTAG5SlimmingHelper.IncludeJetTriggerContent = False
FTAG5SlimmingHelper.IncludeEtMissTriggerContent = False
FTAG5SlimmingHelper.IncludeBJetTriggerContent = False

FTAG5SlimmingHelper.AppendContentToStream(FTAG5Stream)
