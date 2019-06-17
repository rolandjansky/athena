from eflowRec import eflowRecConf
from InDetTrackSelectionTool import InDetTrackSelectionToolConf
# PFTrackSelector
# This handles the track selection (including lepton veto)
# and extrapolation into the calorimeter.
# Parameters: track & vertex container names (offline, HLT, FTK)
def getPFTrackSel(tracksin, verticesin):

    # Configure the extrapolator
    def getExtrapolator():
        # Set up with trigger extrapolator instance
        # FIXME: reconfigure for lower tolerance to speed up
        # from TrkExTools.AtlasExtrapolator import AtlasExtrapolator
        from TrackToCalo.TrackToCaloConf import Trk__ParticleCaloExtensionTool
        from InDetTrigRecExample.InDetTrigConfigRecLoadTools import InDetTrigExtrapolator
        return Trk__ParticleCaloExtensionTool(Extrapolator = InDetTrigExtrapolator)

    TrackCaloExtensionTool = eflowRecConf.eflowTrackCaloExtensionTool(TrackCaloExtensionTool=getExtrapolator())

    # Configure the track selector
    PFTrackSelector = eflowRecConf.PFTrackSelector("PFTrackSelector")
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

    return PFTrackSelector

# PFAlgorithm
# This runs the cell-level subtraction:
#   1. Match clusters to tracks
#   2. Iteratively remove expected calo energy
#   3. Calculate cluster moments and apply LC calibration
# For HLT purposes, no LC calibration is applied and only
# one essential moment (CENTER_MAG) is computed. This is
# needed for origin corrections.
def getPFAlg(clustersin):

    # The tool to handle cell-level subtraction, default parameters
    CellEOverPTool = eflowRecConf.eflowCellEOverPTool_mc12_JetETMiss()

    # Need a few instances of PFTrackClusterMatchingTool with different distance cuts
    def getPFMatchingTool(name,matchcut):
        matchingtool = eflowRecConf.PFTrackClusterMatchingTool(name)
        matchingtool.TrackPositionType   = 'EM2EtaPhi' # str
        matchingtool.ClusterPositionType = 'PlainEtaPhi' # str
        matchingtool.DistanceType        = 'EtaPhiSquareDistance' # str
        matchingtool.MatchCut = matchcut*matchcut

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
        eflowCellEOverPTool = CellEOverPTool,
        PFTrackClusterMatchingTool = getPFMatchingTool("MatchingTool_RecoverSS",0.2)
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
    PFAlgorithm = eflowRecConf.PFAlgorithm("PFAlgorithm",
        PFClusterSelectorTool = pfClusterSel,
        SubtractionToolList = [PFCellLevelSubtractionTool,PFRecoverSplitShowersTool],
        BaseToolList = [PFMomentCalculatorTool]
        )
    
    return PFAlgorithm

# Convert internal eflowRec track/cluster objects into xAOD neutral/charged
# particle flow objects
def getPFOCreators():
    PFOChargedCreatorAlgorithm = eflowRecConf.PFOChargedCreatorAlgorithm("PFOChargedCreatorAlgorithm",
        PFOOutputName="HLTChargedParticleFlowObjects"
        )

    PFONeutralCreatorAlgorithm =  eflowRecConf.PFONeutralCreatorAlgorithm("PFONeutralCreatorAlgorithm",
        PFOOutputName="HLTNeutralParticleFlowObjects",
        DoClusterMoments=False # Only CENTER_MAG
        )
    return PFOChargedCreatorAlgorithm, PFONeutralCreatorAlgorithm

# Generate the full PF reco sequence, assuming tracks, vertices, clusters
# will be created upstream
def PFHLTSequence(clustersin,tracktype="Offline"):
    trackvtxcontainers = {
        "Offline":  ("InDetTrackParticles","PrimaryVertices"),
        "HLT":      ("",""), # Not set up -- ask MET?
        "FTK":      ("FTK_TrackParticleContainer","FTK_VertexContainer"),
        "FTKRefit": ("FTK_TrackParticleContainer_Refit","FTK_VertexContainer_Refit")
        }

    tracksin,verticesin = trackvtxcontainers[tracktype]

    PFTrkSel = getPFTrackSel(tracksin, verticesin)
    PFAlg = getPFAlg(clustersin)
    PFNCreator, PFCCreator = getPFOCreators()

    # Create HLT "parallel OR" sequence holding the PF algs
    # Can be inserted into the jet building sequence
    from AthenaCommon.CFElements import parOR
    pfSequence = parOR("PFSeq_"+tracktype, [PFTrkSel,PFAlg,PFCCreator,PFNCreator])

    return pfSequence
