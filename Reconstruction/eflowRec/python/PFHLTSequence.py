from __future__ import print_function

from eflowRec import eflowRecConf
from InDetTrackSelectionTool import InDetTrackSelectionToolConf

from AthenaCommon.Logging import logging
log = logging.getLogger('PFHLTSequence')

# Use the appropriate containers based on what config is desired
trackvtxcontainers = {
    "offline":  ("InDetTrackParticles","PrimaryVertices"),
    "ftf":      ("HLT_IDTrack_FS_FTF","HLT_IDVertex_FS"),
    }

# PFTrackSelector
# This handles the track selection (including lepton veto)
# and extrapolation into the calorimeter.
# Parameters: track & vertex container names (offline, HLT, FTK)
def getPFTrackSel(tracktype):
    tracksin,verticesin = trackvtxcontainers[tracktype]

    # Configure the extrapolator
    def getExtrapolator():
        # Set up with trigger extrapolator instance
        # FIXME: reconfigure for lower tolerance to speed up
        # from TrkExTools.AtlasExtrapolator import AtlasExtrapolator
        from TrackToCalo.TrackToCaloConf import Trk__ParticleCaloExtensionTool
        from InDetTrigRecExample.InDetTrigConfigRecLoadTools import InDetTrigExtrapolator
        return Trk__ParticleCaloExtensionTool("HLTPF_ParticleCaloExtension",Extrapolator = InDetTrigExtrapolator)

    TrackCaloExtensionTool = eflowRecConf.eflowTrackCaloExtensionTool("HLTPF_eflowTrkCaloExt",TrackCaloExtensionTool=getExtrapolator())
    # Don't assume the existence of a pre-built cache
    # We expect to run PF at most once per event, and not overlap with
    # other use cases like taus
    TrackCaloExtensionTool.PFParticleCache = ""

    # Configure the track selector
    PFTrackSelector = eflowRecConf.PFTrackSelector("PFTrackSelector_"+tracktype)
    PFTrackSelector.trackExtrapolatorTool = TrackCaloExtensionTool

    TrackSelectionTool = InDetTrackSelectionToolConf.InDet__InDetTrackSelectionTool(
        CutLevel = "TightPrimary",
        minPt = 500.0)
    PFTrackSelector.trackSelectionTool = TrackSelectionTool

    # Disable lepton veto as not reconstructed for hadronic triggers
    PFTrackSelector.electronsName = ""
    PFTrackSelector.muonsName = ""
    # Specify input track and vertex containers
    PFTrackSelector.tracksName = tracksin
    PFTrackSelector.VertexContainer = verticesin

    from eflowRec import PFOnlineMon
    monTool = PFOnlineMon.getMonTool_PFTrackSelector()
    PFTrackSelector.MonTool = monTool

    return PFTrackSelector

# PFAlgorithm
# This runs the cell-level subtraction:
#   1. Match clusters to tracks
#   2. Iteratively remove expected calo energy
#   3. Calculate cluster moments and apply LC calibration
# For HLT purposes, no LC calibration is applied and only
# one essential moment (CENTER_MAG) is computed. This is
# needed for origin corrections.
def getPFAlg(clustersin,tracktype):

    # The tool to handle cell-level subtraction, default parameters
    CellEOverPTool = eflowRecConf.eflowCellEOverPTool_mc12_JetETMiss()

    # Need a few instances of PFTrackClusterMatchingTool with different distance cuts
    def getPFMatchingTool(name,matchcut):
        matchingtool = eflowRecConf.PFTrackClusterMatchingTool(name)
        matchingtool.TrackPositionType   = 'EM2EtaPhi' # str
        matchingtool.ClusterPositionType = 'PlainEtaPhi' # str
        matchingtool.DistanceType        = 'EtaPhiSquareDistance' # str
        matchingtool.MatchCut = matchcut*matchcut
        return matchingtool

    # Default energy subtraction where a single cluster satisfies the expected
    # track calo energy
    PFCellLevelSubtractionTool = eflowRecConf.PFCellLevelSubtractionTool("PFCellLevelSubtractionTool",
        eflowCellEOverPTool        = CellEOverPTool,
        # Uses a deltaR' cut (deltaR corrected for cluster width in eta/phi) to
        # match clusters to tracks
        PFTrackClusterMatchingTool = eflowRecConf.PFTrackClusterMatchingTool("CalObjBldMatchingTool"),
        # Additional matching tools used to determine cluster weights
        # These deal with dense environments in which subtraction is difficult.
        PFTrackClusterMatchingTool_015 = getPFMatchingTool(name="MatchingTool_Pull_015",matchcut=0.15),
        PFTrackClusterMatchingTool_02  = getPFMatchingTool(name="MatchingTool_Pull_02",  matchcut=0.2)
        )

    # A second cell-level subtraction tool that handles cases where more than one
    # cluster is needed to recover the full track expected energy
    # Reuse the default E/P subtraction tool
    PFRecoverSplitShowersTool = eflowRecConf.PFRecoverSplitShowersTool("PFRecoverSplitShowersTool",
        eflowCellEOverPTool = CellEOverPTool
        )

    # Configure moment calculation using topocluster moment calculator
    PFMomentCalculatorTool = eflowRecConf.PFMomentCalculatorTool("PFMomentCalculatorTool")
    # Only need CENTER_MAG (depth in calo) for origin corrections
    from CaloRec import CaloRecConf
    PFMomentCalculatorTool.CaloClusterMomentsMaker = CaloRecConf.CaloClusterMomentsMaker("PFClusterMomentsMaker",
        # Originally from CaloTopoClusterFlags
        TwoGaussianNoise = True,
        # Needed for origin correction
        MomentsNames = ["CENTER_MAG"]
        )
    # Makes a temporary cluster collection for moment calculation
    PFMomentCalculatorTool.PFClusterCollectionTool = eflowRecConf.PFClusterCollectionTool()

    pfClusterSel = eflowRecConf.PFClusterSelectorTool("PFClusterSelectorTool",
        clustersName=clustersin,
        calClustersName="")
    PFAlgorithm = eflowRecConf.PFAlgorithm("PFAlgorithm_"+tracktype,
        PFClusterSelectorTool = pfClusterSel,
        SubtractionToolList = [PFCellLevelSubtractionTool,PFRecoverSplitShowersTool],
        BaseToolList = [PFMomentCalculatorTool]
        )

    from eflowRec import PFOnlineMon
    monTool = PFOnlineMon.getMonTool_PFAlgorithm()
    PFAlgorithm.MonTool = monTool
    
    return PFAlgorithm

# Convert internal eflowRec track/cluster objects into xAOD neutral/charged
# particle flow objects
def getPFOCreators(tracktype):
    PFOChargedCreatorAlgorithm = eflowRecConf.PFOChargedCreatorAlgorithm(
        "PFOChargedCreatorAlgorithm_"+tracktype,
        PFOOutputName="HLT_{}ChargedParticleFlowObjects".format(tracktype)
        )

    PFONeutralCreatorAlgorithm =  eflowRecConf.PFONeutralCreatorAlgorithm(
        "PFONeutralCreatorAlgorithm_"+tracktype,
        PFOOutputName="HLT_{}NeutralParticleFlowObjects".format(tracktype),
        DoClusterMoments=False # Only CENTER_MAG
        )
    return PFOChargedCreatorAlgorithm, PFONeutralCreatorAlgorithm

# Generate the full PF reco sequence, assuming tracks, vertices, clusters
# will be created upstream
def PFHLTSequence(dummyflags,clustersin,tracktype):

    PFTrkSel = getPFTrackSel(tracktype)
    PFAlg = getPFAlg(clustersin,tracktype)
    PFCCreator, PFNCreator = getPFOCreators(tracktype)

    # Create HLT "parallel OR" sequence holding the PF algs
    # Can be inserted into the jet building sequence
    from AthenaCommon.CFElements import parOR
    pfSequence = parOR("HLTPFlow_"+tracktype, [PFTrkSel,PFAlg,PFCCreator,PFNCreator])
    pfoPrefix = "HLT_"+tracktype

    log.debug("Created sequence:\n%s", pfSequence)

    return pfSequence, pfoPrefix
