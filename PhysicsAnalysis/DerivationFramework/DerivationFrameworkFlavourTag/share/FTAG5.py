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
    DerivationFramework__xAODStringSkimmingTool,
    DerivationFramework__GenericObjectThinning)

from ThinningUtils.ThinningUtilsConf import (
    EleLinkThinningTool,
    ThinAssociatedObjectsTool)

# JetEtMiss
from DerivationFrameworkJetEtMiss.ExtendedJetCommon import (
    addDefaultTrimmedJets, addTCCTrimmedJets, replaceAODReducedJets,
    addDefaultUFOSoftDropJets, nameJetsFromAlg)
from JetRec.JetRecStandard import jtm
from TrackCaloClusterRecTools.TrackCaloClusterConfig import runTCCReconstruction
from JetRecTools.ConstModHelpers import getConstModSeq, xAOD
from TrackCaloClusterRecTools.TrackCaloClusterConfig import runUFOReconstruction

# tracking
from TrkVertexFitterUtils.TrkVertexFitterUtilsConf import (
    Trk__TrackToVertexIPEstimator)

# flavor tagging
from DerivationFrameworkFlavourTag.HbbCommon import (
    addVRJets, addRecommendedXbbTaggers, xbbTaggerExtraVariables)
from DerivationFrameworkFlavourTag import BTaggingContent as bvars
from DerivationFrameworkMCTruth.MCTruthCommon import (
    addTruth3ContentToSlimmerTool)
from DerivationFrameworkJetEtMiss.JSSVariables import JSSHighLevelVariables
from BTagging.BTaggingConf import Analysis__BTagNNAlg as BTagNNAlg

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
# Common collections etc
#====================================================================

# These are the groomed large-R jet collections considered below for adding links to track-jets, smart slimming lists and extra JSS variables, etc
fatJetCollections = [
    "AntiKt10LCTopoTrimmedPtFrac5SmallR20Jets",
    "AntiKt10TrackCaloClusterTrimmedPtFrac5SmallR20Jets",
    "AntiKt10UFOCSSKSoftDropBeta100Zcut10Jets"
]


#====================================================================
# SKIMMING TOOLS
#====================================================================
skim_tmp = 'count({jc}.pt > 250 * GeV && (abs({jc}.eta) < 2.0)) > 0'
skim_expr = ' || '.join(skim_tmp.format(jc=jc) for jc in fatJetCollections)

FTAG5StringSkimmingTool = DerivationFramework__xAODStringSkimmingTool(
    name = "FTAG5StringSkimmingTool",
    expression = skim_expr)

ToolSvc += FTAG5StringSkimmingTool
log_setup(FTAG5StringSkimmingTool)

#=====================================================================
# Thinning tool pre-setup
#=====================================================================

FTAG5ThinningHelper = ThinningHelper( "FTAG5ThinningHelper" )
FTAG5ThinningHelper.TriggerChains = ''
FTAG5ThinningHelper.AppendToStream( FTAG5Stream )

# this thinning threshold is used below to select all large-R jets
largeRJetThinningPtThresholdGeV = 200


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

# ============================
# running TCC reconstruction
# ============================
import AthenaCommon.AtlasUnixStandardJob
include("RecExCond/AllDet_detDescr.py")
runTCCReconstruction(FTAG5Seq, ToolSvc, "LCOriginTopoClusters", "InDetTrackParticles",outputTCCName="TrackCaloClustersCombinedAndNeutral")

#==============================
# running UFO reconstruction
#==============================

## Add PFlow constituents
pflowCSSKSeq = getConstModSeq(["CS","SK"], "EMPFlow")

# add the pflow cssk sequence to the main jetalg if not already there :
if pflowCSSKSeq.getFullName() not in [t.getFullName() for t in DerivationFrameworkJob.jetalg.Tools]:
  DerivationFrameworkJob.jetalg.Tools += [pflowCSSKSeq]

# Add UFO constituents
emcsskufoAlg = runUFOReconstruction(FTAG5Seq, ToolSvc, PFOPrefix="CSSK",caloClusterName="LCOriginTopoClusters")

#==============================

# I don't understand why we need some of these. We don't use
# AntiKt4PV0TrackJets in the output, but without them we get a crash
reducedJetList = ["AntiKt2PV0TrackJets",
                  "AntiKt4PV0TrackJets", # <- Crashes without this,
                  "AntiKt10LCTopoJets", # <- while building this collection
                  "AntiKt10TrackCaloClusterJets",
                  "AntiKt10UFOCSSKJets",
                  "AntiKt4TruthJets",
                  "AntiKt10TruthJets"]

extendedFlag = 0 # --- = 0 for Standard Taggers & =1 for ExpertTaggers
replaceAODReducedJets(reducedJetList,FTAG5Seq,"FTAG5", extendedFlag)

#===================================================================
# Add groomed large-R jets
#===================================================================
addDefaultTrimmedJets(FTAG5Seq,"FTAG5",dotruth=True)
addTCCTrimmedJets(FTAG5Seq,"FTAG5",dotruth=True)
addDefaultUFOSoftDropJets(FTAG5Seq,"FTAG5",dotruth=True)

#====================================================================
# TRUTH in excess of TRUTH3
#====================================================================
if globalflags.DataSource()!='data':

    from DerivationFrameworkMCTruth.MCTruthCommon import (
        addStandardTruthContents,
        addHFAndDownstreamParticles,
        addWbosonsAndDownstreamParticles,
        addBosonsAndDownstreamParticles,
        addTopQuarkAndDownstreamParticles,
    )
    addStandardTruthContents()
    addHFAndDownstreamParticles()
    addWbosonsAndDownstreamParticles()
    addBosonsAndDownstreamParticles() # H/W/Z bosons
    addTopQuarkAndDownstreamParticles() # top

#===================================================================
# Variable Radius (VR) Jets
#===================================================================

# alias for VR
BTaggingFlags.CalibrationChannelAliases += ["AntiKtVR30Rmax4Rmin02Track->AntiKtVR30Rmax4Rmin02Track,AntiKt4EMTopo",
                                            "AntiKtVR30Rmax4Rmin02TrackGhostTag->AntiKtVR30Rmax4Rmin02TrackGhostTag,AntiKt4EMTopo",]

VRJetInfo = {}
for do_ghost in [False, True]:
    for training in ['201810','201903']:
        VRJetInfo[do_ghost, training] = addVRJets(
            sequence=FTAG5Seq,
            largeRColls=fatJetCollections,
            do_ghost=do_ghost,
            training=training,
            logger=ftag5_log)


FTAG5VRJets = [nameJetsFromAlg(i['VRJetName']) for i in VRJetInfo.values()]

#==================================================================
# Thinning setup for VR jets and tracks
#==================================================================

# first we set up the link thinning: we'll keep any subjets or b-taggig
# objects associated to high-pt large-R jets, and any tracks that are
# either ghost or cone associated to these associated subjets.

FTAG5LinkThinningTools = []
for (do_ghost, training), vr_info in VRJetInfo.items():

    assoc_name = 'Ghost' if do_ghost else 'Cone'
    jet_name = vr_info['VRJetName']
    ghost_link_name = vr_info['ghostJetLinkName']

    # thin jets and b-tagging objects
    FTAG5LinkThinningTools.append(EleLinkThinningTool(
        "{}BTag{}ThinningTool".format(assoc_name, training),
        LinkName = "Parent.{glink}({jets}).btaggingLink({btag})".format(
            glink=ghost_link_name,
            jets=nameJetsFromAlg(jet_name),
            btag='BTagging_{}'.format(jet_name.replace('BTagging',''))),
        ThinningService = FTAG5ThinningHelper.ThinningSvc() ))

    # keep the cone associated tracks
    FTAG5LinkThinningTools.append(EleLinkThinningTool(
        "{}{}TrackConeThinningTool".format(assoc_name, training),
        LinkName="Parent.{}.btaggingLink.BTagTrackToJetAssociator(InDetTrackParticles)".format(ghost_link_name),
        ThinningService = FTAG5ThinningHelper.ThinningSvc() ))

    # keep the ghost associated tracks as well
    FTAG5LinkThinningTools.append(EleLinkThinningTool(
        "{}{}TrackGhostThinningTool".format(assoc_name, training),
        LinkName="Parent.{}.GhostTrack(InDetTrackParticles,GSFTrackParticles)".format(
            ghost_link_name),
        ThinningService = FTAG5ThinningHelper.ThinningSvc() ))

for tool in FTAG5LinkThinningTools:
    ToolSvc += tool


# Now we thin the large R jets by pt
#
FTAG5HbbThinningTools = []
for large_r in fatJetCollections:
  FTAG5HbbThinningTools.append(DerivationFramework__GenericObjectThinning(
        "FTAG5{0}ThinningTool".format(large_r),
        ThinningService = FTAG5ThinningHelper.ThinningSvc(),
        SelectionString = "{0}.pt > {1}*GeV".format(
            large_r, largeRJetThinningPtThresholdGeV),
        ContainerName = large_r) )

  # propigate the thinning to the associated objects
  FTAG5HbbThinningTools.append(
    ThinAssociatedObjectsTool(
        "FTAG5{}SubjetAssocThinningTool".format(large_r),
        ThinningService = FTAG5ThinningHelper.ThinningSvc(),
        SGKey = large_r,
        ChildThinningTools = FTAG5LinkThinningTools ) )

for tool in FTAG5HbbThinningTools:
    ToolSvc += tool


# Now we apply the same pt threshold to thinning TCCs and UFOs
#
# TCC thinning
#
from DerivationFrameworkInDet.DerivationFrameworkInDetConf import (
    DerivationFramework__TCCTrackParticleThinning)
FTAG5TCCThinningTool = DerivationFramework__TCCTrackParticleThinning(
    name                         = "FTAG5TCCThinningTool",
    ThinningService              = FTAG5ThinningHelper.ThinningSvc(),
    JetKey                       = "AntiKt10TrackCaloClusterJets",
    TCCKey                       = "TrackCaloClustersCombinedAndNeutral",
    InDetTrackParticlesKey       = "InDetTrackParticles",
    CaloCalTopoClustersKey       = "CaloCalTopoClusters",
    ThinOriginCorrectedClusters  = True,
    OriginCaloCalTopoClustersKey = "LCOriginTopoClusters",
    SelectionString = "AntiKt10TrackCaloClusterJets.pt > {0}*GeV".format(
        largeRJetThinningPtThresholdGeV))
ToolSvc += FTAG5TCCThinningTool
log_setup(FTAG5TCCThinningTool)


# Tracks and CaloClusters associated with UFOs
#
from DerivationFrameworkInDet.DerivationFrameworkInDetConf import (
    DerivationFramework__UFOTrackParticleThinning)
FTAG5EMCSSKUFOTPThinningTool = DerivationFramework__UFOTrackParticleThinning(
    name                   = "FTAG5CSSKUFOTPThinningTool",
    ThinningService        = "FTAG5ThinningSvc",
    JetKey                 = "AntiKt10UFOCSSKJets",
    UFOKey                 = "CSSKUFO",
    InDetTrackParticlesKey = "InDetTrackParticles",
    PFOCollectionSGKey     = "JetETMiss",
    AdditionalPFOKey       = ["CSSK"],
    SelectionString = "AntiKt10UFOCSSKJets.pt > {0}*GeV".format(
        largeRJetThinningPtThresholdGeV))

ToolSvc += FTAG5EMCSSKUFOTPThinningTool
log_setup(FTAG5EMCSSKUFOTPThinningTool)


#==================================================================
# Augment tracks and jets with additional information
#==================================================================

FTAG5Seq += CfgMgr.BTagVertexAugmenter()

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
    ThinningTools = [FTAG5TCCThinningTool]+FTAG5HbbThinningTools,
    AugmentationTools = []
)


#====================================================================
# Add slimming tools
#====================================================================

FTAG5SlimmingHelper = SlimmingHelper("FTAG5SlimmingHelper")

FTAG5SlimmingHelper.SmartCollections = [
    "Muons",
    "InDetTrackParticles",
    "BTagging_AntiKtVR30Rmax4Rmin02Track_201903_expert",
    "BTagging_AntiKtVR30Rmax4Rmin02Track_201903GhostTag_expert",
    "BTagging_AntiKtVR30Rmax4Rmin02Track_201810_expert",
    "BTagging_AntiKtVR30Rmax4Rmin02Track_201810GhostTag_expert"]
FTAG5SlimmingHelper.SmartCollections += fatJetCollections
FTAG5SlimmingHelper.SmartCollections += FTAG5VRJets

for fatJetCollection in fatJetCollections:
    jssVariables = ['.'.join([fatJetCollection] + JSSHighLevelVariables) ]
    FTAG5SlimmingHelper.ExtraVariables += jssVariables

FTAG5SlimmingHelper.ExtraVariables += [
    "InDetTrackParticles.truthMatchProbability.x.y.z.vx.vy.vz",
    "InDetTrackParticles.numberOfContribPixelLayers.numberOfTRTHits.numberOfInnermostPixelLayerSharedHits.numberOfNextToInnermostPixelLayerSharedHits",
    "InDetTrackParticles.numberOfPixelSplitHits.numberOfInnermostPixelLayerSplitHits.numberOfNextToInnermostPixelLayerSplitHits",
    "InDetTrackParticles.hitPattern.radiusOfFirstHit",
    "InDetTrackParticles.btagIp_d0.btagIp_z0SinTheta.btagIp_d0Uncertainty.btagIp_z0SinThetaUncertainty.btagIp_trackDisplacement.btagIp_trackMomentum",
    "TrackCaloClustersCombinedAndNeutral.m.pt.phi.eta.taste.trackParticleLink.DetectorEta.iparticleLinks",
    "CSSKUFO.m.pt.phi.eta.taste.trackParticleLink.DetectorEta.iparticleLinks"
]
FTAG5SlimmingHelper.ExtraVariables += xbbTaggerExtraVariables

# adding default b-tag alg varibles for antikt2 collection (smart collection no longer supported)
akt2variables_btag = ['.'.join(["BTagging_AntiKt2Track"] + bvars.BTaggingStandardAux)]
akt2variables_kin = ['.'.join(["AntiKt2PV0TrackJets"] + bvars.JetStandardAux)]
FTAG5SlimmingHelper.ExtraVariables += akt2variables_btag
FTAG5SlimmingHelper.ExtraVariables += akt2variables_kin

# add the extra variables that come from the BTagJetAugmenterAlg
extra_btag = list(complex_jet_discriminants)
extra_btag += ['JetFitter_JFvertices', 'SV1_vertices']
for jc in FTAG5VRJets:
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
ghost_subjets = [
    "GhostAntiKt2TrackJet",
    'GhostVR30Rmax4Rmin02TrackJet_BTagging201810',
    'GhostVR30Rmax4Rmin02TrackJet_BTagging201810GhostTag',
    'GhostVR30Rmax4Rmin02TrackJet_BTagging201903',
    'GhostVR30Rmax4Rmin02TrackJet_BTagging201903GhostTag']
for jc in ['AntiKt10LCTopoJets', 'AntiKt10TrackCaloClusterJets', 'AntiKt10UFOCSSKJets']:
    FTAG5SlimmingHelper.ExtraVariables.append(
        '.'.join([jc] + ghost_counts + ghost_pts + ghost_subjets))

FTAG5SlimmingHelper.IncludeMuonTriggerContent = False
FTAG5SlimmingHelper.IncludeEGammaTriggerContent = False
FTAG5SlimmingHelper.IncludeJetTriggerContent = False
FTAG5SlimmingHelper.IncludeEtMissTriggerContent = False
FTAG5SlimmingHelper.IncludeBJetTriggerContent = False

# Add truth3
addTruth3ContentToSlimmerTool(FTAG5SlimmingHelper)
# extra for H/W/Z/t taggers
FTAG5SlimmingHelper.AllVariables += [
    "AntiKt10TruthTrimmedPtFrac5SmallR20Jets",
    "AntiKt10TruthSoftDropBeta100Zcut10Jets",
]

# Dan Guest: I have no idea why this works, or why it's needed, but
# they are doing this in EXOT8. What is a static content? Why doesn't
# adding these containers to "AllVariables" (in the
# "addTruth3ContentToSlimmerTool" function above) work?
#
# In fact, if you can understand why this works, let me know! I'm
# dguest (at cern.ch).
#
if globalflags.DataSource()!='data':
    for truthc in ["TruthBosons", "TruthTopQuark"]:
        sc = [
            "xAOD::TruthParticleContainer#"+truthc+"WithDecayParticles",
            "xAOD::TruthParticleAuxContainer#"+truthc+"WithDecayParticlesAux.",
            "xAOD::TruthVertexContainer#"+truthc+"WithDecayVertices",
            "xAOD::TruthVertexAuxContainer#"+truthc+"WithDecayVerticesAux."]
        FTAG5SlimmingHelper.StaticContent += sc

FTAG5SlimmingHelper.AppendContentToStream(FTAG5Stream)
