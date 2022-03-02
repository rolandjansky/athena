# Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
from eflowRec import eflowRecConf
from InDetTrackSelectionTool import InDetTrackSelectionToolConf
from InDetTrigRecExample.InDetTrigConfigRecLoadTools import InDetTrigExtrapolator
from AthenaConfiguration.ComponentFactory import CompFactory

from AthenaCommon.Logging import logging

log = logging.getLogger("PFHLTSequence")

# Use the appropriate containers based on what config is desired
# collections for the jet trigger need to be set correction in the 
# jet trigger configuration
trackvtxcontainers = {
    "offline": ("InDetTrackParticles",    "PrimaryVertices")
}



# Configure the extrapolator
def getExtrapolator():
    # Set up with trigger extrapolator instance
    # FIXME: reconfigure for lower tolerance to speed up
    # from TrkExTools.AtlasExtrapolator import AtlasExtrapolator
    from TrackToCalo.TrackToCaloConf import Trk__ParticleCaloExtensionTool

    return Trk__ParticleCaloExtensionTool(
        "HLTPF_ParticleCaloExtension", Extrapolator=InDetTrigExtrapolator
    )

def PFTrackExtension(tracktype):
    """ Get the track extension after a preselection

    Returns the preselected track selection, the extension cache and the list of algorithms
    """
    
    tracksin, _ = trackvtxcontainers[tracktype]
    from eflowRec.eflowRecConf import PFTrackPreselAlg
    from TrackToCalo.TrackToCaloConf import Trk__PreselCaloExtensionBuilderAlg

    presel_alg = PFTrackPreselAlg(
        f"HLTPFTrackPresel_{tracktype}",
        InputTracks=tracksin,
        OutputTracks=f"HLTPFPreselTracks_{tracktype}",
        TrackSelTool=InDetTrackSelectionToolConf.InDet__InDetTrackSelectionTool(
            CutLevel="TightPrimary", minPt=500.0
        ),
    )

    extension_alg = Trk__PreselCaloExtensionBuilderAlg(
        f"HLTPFTrackExtension_{tracktype}",
        ParticleCaloExtensionTool=getExtrapolator(),
        InputTracks=presel_alg.OutputTracks,
        OutputCache=f"HLTPFTrackExtensionCache_{tracktype}",
    )

    return (
        presel_alg.OutputTracks,
        extension_alg.OutputCache,
        [presel_alg, extension_alg]
    )

def muonCaloTagSeq(flags, tracktype, tracksin, extcache, cellsin):
    from eflowRec.eflowRecConf import PFTrackMuonCaloTaggingAlg
    from TrackToCalo.TrackToCaloConf import Rec__ParticleCaloCellAssociationTool
    from CaloTrkMuIdTools.CaloTrkMuIdToolsConf import (
        CaloMuonScoreTool,
        CaloMuonTag,
        TrackDepositInCaloTool,
    )
    tag_alg = PFTrackMuonCaloTaggingAlg(
        f"PFTrackMuonCaloTaggingAlg_{tracktype}",
        InputTracks = tracksin,
        InputCaloExtension = extcache,
        InputCells = cellsin,
        OutputTracks=f"PFMuonCaloTagTracks_{tracktype}",
        MinPt = flags.Trigger.FSHad.PFOMuonRemovalMinPt,
        MuonScoreTool=CaloMuonScoreTool(
            CaloMuonEtaCut=3,
            ParticleCaloCellAssociationTool=Rec__ParticleCaloCellAssociationTool(
                ParticleCaloExtensionTool=getExtrapolator(),
                CaloCellContainer=""
            ),
        ),
        LooseTagTool=CaloMuonTag("LooseCaloMuonTag", TagMode="Loose"),
        TightTagTool=CaloMuonTag("TightCaloMuonTag", TagMode="Tight"),
        DepositInCaloTool=TrackDepositInCaloTool(
            ExtrapolatorHandle=InDetTrigExtrapolator,
            ParticleCaloCellAssociationTool=Rec__ParticleCaloCellAssociationTool(
                ParticleCaloExtensionTool=getExtrapolator(),
                CaloCellContainer=""
            ),
            ParticleCaloExtensionTool=getExtrapolator(),
        ),
    )
    return tag_alg.OutputTracks, tag_alg

def muonIsoTagSeq(flags, tracktype, tracksin, extcache, clustersin):
    from eflowRec.eflowRecConf import PFTrackMuonIsoTaggingAlg
    from IsolationTool.IsolationToolConf import xAOD__CaloIsolationTool, xAOD__TrackIsolationTool

    tag_alg = PFTrackMuonIsoTaggingAlg(
        f"PFTrackMuonIsoTaggingAlg_{tracktype}",
        InputTracks = tracksin,
        InputClusters = clustersin,
        InputVertices = trackvtxcontainers[tracktype][1],
        OutputTracks = f"PFMuonIsoTagTracks_{tracktype}",
        MinPt = flags.Trigger.FSHad.PFOMuonRemovalMinPt,
        TrackIsoTool = xAOD__TrackIsolationTool(TrackParticleLocation=tracksin, VertexLocation=""),
        CaloIsoTool = xAOD__CaloIsolationTool(
            ParticleCaloExtensionTool=getExtrapolator(),
            InputCaloExtension=extcache,
            ParticleCaloCellAssociationTool="",
            saveOnlyRequestedCorrections=True,
        ),
    )
    return tag_alg.OutputTracks, tag_alg

# PFTrackSelector
# This handles the track selection (including lepton veto)
# and extrapolation into the calorimeter.
# Parameters: track & vertex container names (offline, HLT, FTK)
def getPFTrackSel(tracktype, extensionCache="", trackname=None):

    tracksin, verticesin = trackvtxcontainers[tracktype]
    if trackname is not None:
        tracksin = trackname

    TrackCaloExtensionTool = eflowRecConf.eflowTrackCaloExtensionTool(
        "HLTPF_eflowTrkCaloExt", TrackCaloExtensionTool=getExtrapolator()
    )
    # Set the extension cache - if this is "" then the tool will run the extension
    TrackCaloExtensionTool.PFParticleCache = extensionCache
    
    # Set monitoring tool
    from eflowRec import PFOnlineMon
    monTool_extrapolator = PFOnlineMon.getMonTool_eflowTrackCaloExtensionTool()
    monTool_extrapolator.HistPath = 'TrackExtrapolator'
    TrackCaloExtensionTool.MonTool_TrackCaloExtension = monTool_extrapolator
    

    # Configure the track selector
    PFTrackSelector = eflowRecConf.PFTrackSelector("PFTrackSelector_" + tracktype)
    PFTrackSelector.trackExtrapolatorTool = TrackCaloExtensionTool

    TrackSelectionTool = InDetTrackSelectionToolConf.InDet__InDetTrackSelectionTool(
        CutLevel="TightPrimary", minPt=500.0
    )
    PFTrackSelector.trackSelectionTool = TrackSelectionTool

    # Disable lepton veto as not reconstructed for hadronic triggers
    PFTrackSelector.electronsName = ""
    PFTrackSelector.muonsName = ""
    # Specify input track and vertex containers
    PFTrackSelector.tracksName = tracksin
    PFTrackSelector.VertexContainer = verticesin

    monTool_selector = PFOnlineMon.getMonTool_PFTrackSelector()
    monTool_selector.HistPath = 'PFTrackSelector'
    PFTrackSelector.MonTool = monTool_selector

    return PFTrackSelector


# PFAlgorithm
# This runs the cell-level subtraction:
#   1. Match clusters to tracks
#   2. Iteratively remove expected calo energy
#   3. Calculate cluster moments and apply LC calibration
# For HLT purposes, no LC calibration is applied and only
# one essential moment (CENTER_MAG) is computed. This is
# needed for origin corrections.
def getPFAlg(flags, clustersin, tracktype):
    assert (flags is not None), "Impossible to create PFlow HLT algorithm with empty flags"

    # The tool to handle cell-level subtraction, default parameters
    if flags.Trigger.Jet.doMC20_EOverP:
        from eflowRec.eflowRecConf import eflowCellEOverPTool_Run2_mc20_JetETMiss
        CellEOverPTool= eflowCellEOverPTool_Run2_mc20_JetETMiss()
    else:
        from eflowRec.eflowRecConf import eflowCellEOverPTool_mc12_JetETMiss
        CellEOverPTool = eflowCellEOverPTool_mc12_JetETMiss()



    # Need a few instances of PFTrackClusterMatchingTool with different distance cuts
    def getPFMatchingTool(name, matchcut):
        matchingtool = eflowRecConf.PFTrackClusterMatchingTool(name)
        matchingtool.TrackPositionType = "EM2EtaPhi"  # str
        matchingtool.ClusterPositionType = "PlainEtaPhi"  # str
        matchingtool.DistanceType = "EtaPhiSquareDistance"  # str
        matchingtool.MatchCut = matchcut * matchcut
        return matchingtool

    from eflowRec import PFOnlineMon
    PFTrackClusterMatchingTool_1 = CompFactory.PFTrackClusterMatchingTool("CalObjBldMatchingTool")
    monTool_matching = PFOnlineMon.getMonTool_PFTrackClusterMatching()
    monTool_matching.HistPath = 'PFTrackClusterMatchingTool_1'
    PFTrackClusterMatchingTool_1.MonTool_ClusterMatching = monTool_matching

    # Default energy subtraction where a single cluster satisfies the expected
    # track calo energy
    PFCellLevelSubtractionTool = eflowRecConf.PFSubtractionTool(
        "PFCellLevelSubtractionTool",
        eflowCellEOverPTool=CellEOverPTool,
        # Uses a deltaR' cut (deltaR corrected for cluster width in eta/phi) to
        # match clusters to tracks
        PFTrackClusterMatchingTool=eflowRecConf.PFTrackClusterMatchingTool(
            "CalObjBldMatchingTool"
        ),
        # Additional matching tools used to determine cluster weights
        # These deal with dense environments in which subtraction is difficult.
        PFTrackClusterMatchingTool_015=getPFMatchingTool(
            name="MatchingTool_Pull_015", matchcut=0.15
        ),
        PFTrackClusterMatchingTool_02=getPFMatchingTool(
            name="MatchingTool_Pull_02", matchcut=0.2
        ),
    )

    # A second cell-level subtraction tool that handles cases where more than one
    # cluster is needed to recover the full track expected energy
    # Reuse the default E/P subtraction tool
    PFRecoverSplitShowersTool = eflowRecConf.PFSubtractionTool(
        "PFRecoverSplitShowersTool", eflowCellEOverPTool=CellEOverPTool,
        RecoverSplitShowers = True
    )

    # Configure moment calculation using topocluster moment calculator
    PFMomentCalculatorTool = eflowRecConf.PFMomentCalculatorTool(
        "PFMomentCalculatorTool"
    )
    # Only need CENTER_MAG (depth in calo) for origin corrections
    from CaloRec import CaloRecConf

    PFMomentCalculatorTool.CaloClusterMomentsMaker = CaloRecConf.CaloClusterMomentsMaker(
        "PFClusterMomentsMaker",
        # Originally from CaloTopoClusterFlags
        TwoGaussianNoise=True,
        # Needed for origin correction
        MomentsNames=["CENTER_MAG"],
    )
    # Makes a temporary cluster collection for moment calculation
    PFMomentCalculatorTool.PFClusterCollectionTool = (
        eflowRecConf.PFClusterCollectionTool()
    )

    pfClusterSel = eflowRecConf.PFClusterSelectorTool(
        "PFClusterSelectorTool", clustersName=clustersin, calClustersName=""
    )
    PFAlgorithm = eflowRecConf.PFAlgorithm(
        "PFAlgorithm_" + tracktype,
        PFClusterSelectorTool=pfClusterSel,
        SubtractionToolList=[PFCellLevelSubtractionTool, PFRecoverSplitShowersTool],
        BaseToolList=[PFMomentCalculatorTool],
    )

    monTool_pfalg = PFOnlineMon.getMonTool_PFAlgorithm()
    monTool_pfalg.HistPath = 'PFAlgorithm'
    PFAlgorithm.MonTool = monTool_pfalg
    
    return PFAlgorithm


# Convert internal eflowRec track/cluster objects into xAOD neutral/charged
# particle flow objects
def getPFOCreators(flags,tracktype):

    # flag for ATR-24619 (to remove flag and use FlowElement after)
    if flags.Trigger.usexAODFlowElements:
        log.debug("Using eflowRec with xAODType FlowElement") # TODO Remove after ATR-24619 complete
        PFOChargedCreatorAlgorithm = eflowRecConf.PFChargedFlowElementCreatorAlgorithm(
            "PFChargedCreatorAlgorithm_" + tracktype,
            FlowElementOutputName="HLT_{}ChargedParticleFlowObjects".format(tracktype),
        )

        PFONeutralCreatorAlgorithm = eflowRecConf.PFNeutralFlowElementCreatorAlgorithm(
            "PFNeutralCreatorAlgorithm_" + tracktype,
            FlowElementOutputName="HLT_{}NeutralParticleFlowObjects".format(tracktype),
        )
    else:
        log.debug("Using eflowRec with xAODType ParticleFlow") # TODO Remove after ATR-24619 complete
        PFOChargedCreatorAlgorithm = eflowRecConf.PFOChargedCreatorAlgorithm(
            "PFOChargedCreatorAlgorithm_" + tracktype,
            PFOOutputName="HLT_{}ChargedParticleFlowObjects".format(tracktype),
        )

        PFONeutralCreatorAlgorithm = eflowRecConf.PFONeutralCreatorAlgorithm(
            "PFONeutralCreatorAlgorithm_" + tracktype,
            PFOOutputName="HLT_{}NeutralParticleFlowObjects".format(tracktype),
            DoClusterMoments=False,  # Only CENTER_MAG
        )
    return PFOChargedCreatorAlgorithm, PFONeutralCreatorAlgorithm


# Generate the full PF reco sequence, assuming tracks, vertices, clusters
# will be created upstream
def PFHLTSequence(flags, clustersin, tracktype, cellsin=None):
    if flags is None:
        from AthenaConfiguration.AllConfigFlags import ConfigFlags as flags

    muon_mode = flags.Trigger.FSHad.PFOMuonRemoval
    if muon_mode == "None":
        tracks = trackvtxcontainers[tracktype][0]
        algs = []
        extension = ""
    else:
        pretracks, extension, algs = PFTrackExtension(tracktype)
        if muon_mode == "Calo":
            if cellsin is None:
                raise ValueError("Cells must be provided for the 'Calo' muon mode!")
            tracks, tag_alg = muonCaloTagSeq(flags, tracktype, pretracks, extension, cellsin)
        elif muon_mode == "Iso":
            tracks, tag_alg = muonIsoTagSeq(flags, tracktype, pretracks, extension, clustersin)
        else:
            raise ValueError(f"Invalid muon removal mode '{muon_mode}'")
        algs.append(tag_alg)
    
    PFTrkSel = getPFTrackSel(tracktype, extension, tracks)
    PFAlg = getPFAlg(flags,clustersin, tracktype)
    PFCCreator, PFNCreator = getPFOCreators(flags,tracktype)

    # Create HLT "parallel OR" sequence holding the PF algs
    # Can be inserted into the jet building sequence
    from AthenaCommon.CFElements import parOR

    pfSequence = parOR(
        "HLTPFlow_" + tracktype, algs + [PFTrkSel, PFAlg, PFCCreator, PFNCreator]
    )
    pfoPrefix = "HLT_" + tracktype

    log.debug("Created sequence:\n%s", pfSequence)

    return pfSequence, pfoPrefix
