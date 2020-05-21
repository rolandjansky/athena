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
    OutputJets, addJetOutputs)
from DerivationFrameworkJetEtMiss.ExtendedJetCommon import (
    addDefaultTrimmedJets, addTCCTrimmedJets, replaceAODReducedJets, addConstModJets)
from JetRec.JetRecStandard import jtm
from TrackCaloClusterRecTools.TrackCaloClusterConfig import runTCCReconstruction

# tracking
from TrkVertexFitterUtils.TrkVertexFitterUtilsConf import (
    Trk__TrackToVertexIPEstimator)

# flavor tagging
from DerivationFrameworkFlavourTag.HbbCommon import (
    addVRJets, addExKtCoM, addRecommendedXbbTaggers, xbbTaggerExtraVariables)
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

# Run HbbThinner on large-R jet collections
FTAG5HbbThinningTools = []
for collection in ["AntiKt10LCTopoTrimmedPtFrac5SmallR20Jets", "AntiKt10TrackCaloClusterTrimmedPtFrac5SmallR20Jets"]:
    FTAG5HbbThinningTools.append(HbbThinner(
        name = "FTAG5HbbThinningTool_%s"%collection,
        StreamName = streamName,
        jetCollectionName = collection,
        largeJetPtCut = 200e3,
        largeJetEtaCut = 2.1,
        smallJetPtCut = 7e3,
        nLeadingSubjets = 100, # save all the subjets in the jet
        addSubjetGhosts = True,
        addConstituents = True,
        addConeAssociated = True))

    ToolSvc += FTAG5HbbThinningTools[-1]
    log_setup(FTAG5HbbThinningTools[-1])

# Tracks and CaloClusters associated with TCCs
from DerivationFrameworkInDet.DerivationFrameworkInDetConf import DerivationFramework__TCCTrackParticleThinning
FTAG5TCCThinningTool = DerivationFramework__TCCTrackParticleThinning(name                         = "FTAG5TCCThinningTool",
                                                                     StreamName                   = streamName,
                                                                     JetKey                       = "AntiKt10TrackCaloClusterJets",
                                                                     TCCKey                       = "TrackCaloClustersCombinedAndNeutral",
                                                                     InDetTrackParticlesKey       = "InDetTrackParticles",
                                                                     CaloCalTopoClustersKey       = "CaloCalTopoClusters",
                                                                     ThinOriginCorrectedClusters  = True,
                                                                     OriginCaloCalTopoClustersKey = "LCOriginTopoClusters")
ToolSvc += FTAG5TCCThinningTool
log_setup(FTAG5TCCThinningTool)



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
    "AntiKt2PV0TrackJets",
    "AntiKtVR30Rmax4Rmin02TrackJets_BTagging201810",
    "AntiKtVR30Rmax4Rmin02TrackJets_BTagging201903",
    "AntiKtVR30Rmax4Rmin02TrackJets_BTagging201810GhostTag",
    "AntiKtVR30Rmax4Rmin02TrackJets_BTagging201903",
    "AntiKt10LCTopoTrimmedPtFrac5SmallR20ExKt2SubJets",
    "AntiKt10LCTopoTrimmedPtFrac5SmallR20ExKt3SubJets",
    "AntiKt10LCTopoTrimmedPtFrac5SmallR20ExKt2GASubJets",
    "AntiKt10LCTopoTrimmedPtFrac5SmallR20ExKt3GASubJets",
    "AntiKt10LCTopoTrimmedPtFrac5SmallR20ExCoM2SubJets"]
OutputJets["FTAG5"] = FTAG5BTaggedJets[:]

# running TCC reconstruction
import AthenaCommon.AtlasUnixStandardJob
include("RecExCond/AllDet_detDescr.py")
runTCCReconstruction(FTAG5Seq, ToolSvc, "LCOriginTopoClusters", "InDetTrackParticles",outputTCCName="TrackCaloClustersCombinedAndNeutral")

# I don't understand why we need some of these. We don't use
# AntiKt4PV0TrackJets in the output, but without them we get a crash
reducedJetList = ["AntiKt2PV0TrackJets",
                  "AntiKt4PV0TrackJets", # <- Crashes without this,
                  "AntiKt10LCTopoJets", # <- while building this collection
                  "AntiKt10TrackCaloClusterJets",
                  "AntiKt4TruthJets",
                  "AntiKt10TruthJets"]

extendedFlag = 0 # --- = 0 for Standard Taggers & =1 for ExpertTaggers
replaceAODReducedJets(reducedJetList,FTAG5Seq,"FTAG5", extendedFlag)

#===================================================================
# Add trimmed large-R jets
#===================================================================
addDefaultTrimmedJets(FTAG5Seq,"FTAG5",dotruth=True)
addTCCTrimmedJets(FTAG5Seq,"FTAG5")

# These are the jet collections considered below for adding links to track-jets, smart slimming lists and extra JSS variables
fatJetCollections = [
    "AntiKt10LCTopoTrimmedPtFrac5SmallR20Jets",
    "AntiKt10TrackCaloClusterTrimmedPtFrac5SmallR20Jets",
]

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

addVRJets(
    sequence=FTAG5Seq,
    largeRColls=fatJetCollections,
    do_ghost=False,
    logger=ftag5_log)

addVRJets(
    sequence=FTAG5Seq,
    largeRColls=fatJetCollections,
    do_ghost=True,
    logger=ftag5_log)

addVRJets(
    sequence=FTAG5Seq,
    largeRColls=fatJetCollections,
    do_ghost=False,
    training='201903',
    logger=ftag5_log)

# Add new DL1 and RNN
newtag_collections = [
    'AntiKtVR30Rmax4Rmin02TrackJets_BTagging201810',
    'AntiKtVR30Rmax4Rmin02TrackJets_BTagging201903',
    'AntiKtVR30Rmax4Rmin02TrackJets_BTagging201810GhostTag'
]
rnn_remap = {'rnnip_p' + x: 'rnnipT_p' + x for x in 'bcu'}
FTAG5Seq += BTagNNAlg(
    "BTagRNNForTrackJets",
    nnPath='BTagging/201903/rnnip/antiktvr30rmax4rmin02track/network.json',
    jetCollections=newtag_collections,
    variableRemapping=rnn_remap)
dl1_remap = {'DL1r_p' + x: 'DL1Tr_p' + x for x in 'bcu'}
dl1_remap.update(rnn_remap)
FTAG5Seq += BTagNNAlg(
    "BTagDL1rForTrackJets",
    nnPath='BTagging/201903/dl1r/antiktvr30rmax4rmin02track/network.json',
    jetCollections=newtag_collections,
    variableRemapping=dl1_remap)


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
    "BTagging_AntiKtVR30Rmax4Rmin02Track_201903",
    "BTagging_AntiKtVR30Rmax4Rmin02Track_201810_expert",
    "BTagging_AntiKtVR30Rmax4Rmin02Track_201810GhostTag_expert",
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
    "AntiKt10LCTopoJets.GhostAntiKt2TrackJet",
    "AntiKt10TrackCaloClusterJets.GhostVR30Rmax4Rmin02TrackJet_BTagging201810",
    "AntiKt10TrackCaloClusterJets.GhostVR30Rmax4Rmin02TrackJet_BTagging201810GhostTag",
    "AntiKt10LCTopoJets.GhostVR30Rmax4Rmin02TrackJet_BTagging201810",
    "AntiKt10LCTopoJets.GhostVR30Rmax4Rmin02TrackJet_BTagging201810GhostTag",
    "InDetTrackParticles.btagIp_d0.btagIp_z0SinTheta.btagIp_d0Uncertainty.btagIp_z0SinThetaUncertainty.btagIp_trackDisplacement.btagIp_trackMomentum",
    "TrackCaloClustersCombinedAndNeutral.m.pt.phi.eta.taste.trackParticleLink.DetectorEta.iparticleLinks"
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
ghost_subjets = [
    'GhostVR30Rmax4Rmin02TrackJet_BTagging201810GhostTag',
    'GhostVR30Rmax4Rmin02TrackJet_BTagging201903']
for jc in ['AntiKt10LCTopoJets', 'AntiKt10TrackCaloClusterJets']:
    FTAG5SlimmingHelper.ExtraVariables.append(
        '.'.join([jc] + ghost_counts + ghost_pts + ghost_subjets))

# add some extra retrained taggers
augmented_btag_containers = [
    'BTagging_AntiKtVR30Rmax4Rmin02Track_201810',
    'BTagging_AntiKtVR30Rmax4Rmin02Track_201810GhostTag']
for cont in augmented_btag_containers:
    FTAG5SlimmingHelper.ExtraVariables.append(
        '.'.join([cont] + dl1_remap.values()))

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
]

FTAG5SlimmingHelper.AppendContentToStream(FTAG5Stream)
