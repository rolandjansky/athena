# Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
from AthenaConfiguration.ComponentAccumulator import ComponentAccumulator
from AthenaConfiguration.ComponentFactory import CompFactory
from AthenaConfiguration.Enums import BeamType

def ITkSiSpacePointsSeedMakerCfg(flags, name="ITkSpSeedsMaker", InputCollections = None, **kwargs) :
    acc = ComponentAccumulator()
    #
    # --- Space points seeds maker, use different ones for cosmics and collisions
    #
    SiSpacePointsSeedMaker = CompFactory.ITk.SiSpacePointsSeedMaker

    kwargs.setdefault("pTmin", flags.ITk.Tracking.ActivePass.minPTSeed )
    kwargs.setdefault("maxdImpact", flags.ITk.Tracking.ActivePass.maxPrimaryImpactSeed )
    kwargs.setdefault("maxZ", flags.ITk.Tracking.ActivePass.maxZImpactSeed )
    kwargs.setdefault("minZ", -flags.ITk.Tracking.ActivePass.maxZImpactSeed )
    kwargs.setdefault("usePixel", flags.ITk.Tracking.ActivePass.useITkPixel and flags.ITk.Tracking.ActivePass.useITkPixelSeeding)
    kwargs.setdefault("SpacePointsPixelName", 'ITkPixelSpacePoints')
    kwargs.setdefault("useStrip", flags.ITk.Tracking.ActivePass.useITkStrip and flags.ITk.Tracking.ActivePass.useITkStripSeeding)
    kwargs.setdefault("SpacePointsStripName", 'ITkStripSpacePoints')
    kwargs.setdefault("useOverlapSpCollection", flags.ITk.Tracking.ActivePass.useITkStrip and flags.ITk.Tracking.ActivePass.useITkStripSeeding )
    kwargs.setdefault("SpacePointsOverlapName", 'ITkOverlapSpacePoints')
    kwargs.setdefault("radMax", flags.ITk.Tracking.ActivePass.radMax)
    kwargs.setdefault("etaMax", flags.ITk.Tracking.ActivePass.maxEta )

    if (len(InputCollections) > 0) and flags.ITk.Tracking.ActivePass.usePrdAssociationTool:
        # not all classes have that property !!!
        kwargs.setdefault("PRDtoTrackMap", 'ITkPRDtoTrackMap'+ flags.ITk.Tracking.ActivePass.extension)
    if flags.Beam.Type is not BeamType.Cosmics:
        kwargs.setdefault("maxRadius1", 0.75*flags.ITk.Tracking.ActivePass.radMax)
        kwargs.setdefault("maxRadius2", flags.ITk.Tracking.ActivePass.radMax)
        kwargs.setdefault("maxRadius3", flags.ITk.Tracking.ActivePass.radMax)

    if flags.ITk.Tracking.doFastTracking :
        kwargs.setdefault("useFastTracking", True)
        kwargs.setdefault("maxSeedsForSpacePoint", 3)

    if flags.ITk.Tracking.ActivePass.extension == "LargeD0":
        kwargs.setdefault("maxSeedsForSpacePoint", 5)
        kwargs.setdefault("isLRT", True)
        kwargs.setdefault("maxZPPP", flags.ITk.Tracking.ActivePass.maxZSpacePointsPPPSeeds)
        kwargs.setdefault("maxZSSS", flags.ITk.Tracking.ActivePass.maxZSpacePointsSSSSeeds)

    if flags.ITk.Tracking.writeSeedValNtuple:
        kwargs.setdefault("WriteNtuple", True)
        HistService = CompFactory.THistSvc(Output = ["valNtuples DATAFILE='SeedMakerValidation.root' OPT='RECREATE'"])
        acc.addService(HistService)

    ITkSiSpacePointsSeedMaker = SiSpacePointsSeedMaker (name = name+flags.ITk.Tracking.ActivePass.extension, **kwargs)

    acc.setPrivateTools(ITkSiSpacePointsSeedMaker)
    return acc

def ITkSiTrackMaker_xkCfg(flags, name="ITkSiTrackMaker", InputCollections = None, **kwargs) :
    acc = ComponentAccumulator()

    from InDetConfig.SiDetElementsRoadToolConfig import ITkSiDetElementsRoadMaker_xkCfg
    ITkSiDetElementsRoadMaker = acc.popToolsAndMerge(ITkSiDetElementsRoadMaker_xkCfg(flags))

    from InDetConfig.SiCombinatorialTrackFinderToolConfig import ITkSiCombinatorialTrackFinder_xkCfg
    track_finder = acc.popToolsAndMerge(ITkSiCombinatorialTrackFinder_xkCfg(flags))

    kwargs.setdefault("useSCT", flags.ITk.Tracking.ActivePass.useITkStrip)
    kwargs.setdefault("usePixel", flags.ITk.Tracking.ActivePass.useITkPixel)
    kwargs.setdefault("RoadTool", ITkSiDetElementsRoadMaker)
    kwargs.setdefault("CombinatorialTrackFinder", track_finder)
    kwargs.setdefault("etaBins", flags.ITk.Tracking.ActivePass.etaBins)
    kwargs.setdefault("pTBins", flags.ITk.Tracking.ActivePass.minPT)
    kwargs.setdefault("pTmin", flags.ITk.Tracking.ActivePass.minPT[0])
    kwargs.setdefault("pTminBrem", flags.ITk.Tracking.ActivePass.minPTBrem[0])
    kwargs.setdefault("nClustersMin", min(flags.ITk.Tracking.ActivePass.minClusters))
    kwargs.setdefault("nHolesMax", flags.ITk.Tracking.ActivePass.nHolesMax[0])
    kwargs.setdefault("nHolesGapMax", flags.ITk.Tracking.ActivePass.nHolesGapMax[0])
    kwargs.setdefault("SeedsFilterLevel", flags.ITk.Tracking.ActivePass.seedFilterLevel)
    kwargs.setdefault("Xi2max", flags.ITk.Tracking.ActivePass.Xi2max[0])
    kwargs.setdefault("Xi2maxNoAdd", flags.ITk.Tracking.ActivePass.Xi2maxNoAdd[0])
    kwargs.setdefault("nWeightedClustersMin", flags.ITk.Tracking.ActivePass.nWeightedClustersMin[0])
    kwargs.setdefault("CosmicTrack", flags.Beam.Type is BeamType.Cosmics)
    kwargs.setdefault("Xi2maxMultiTracks", flags.ITk.Tracking.ActivePass.Xi2max[0])
    kwargs.setdefault("doMultiTracksProd", True)
    kwargs.setdefault("useBremModel", flags.Detector.EnableCalo and flags.ITk.Tracking.doBremRecovery and flags.ITk.Tracking.ActivePass.extension == "") # Disabled for second passes in reco
    kwargs.setdefault("doCaloSeededBrem", flags.ITk.Tracking.doCaloSeededBrem and flags.Detector.EnableCalo)
    kwargs.setdefault("doHadCaloSeedSSS", flags.ITk.Tracking.doHadCaloSeededSSS and flags.Detector.EnableCalo)
    if kwargs["useBremModel"] and kwargs["doCaloSeededBrem"]:
        from InDetConfig.InDetCaloClusterROISelectorConfig import ITkCaloClusterROIPhiRZContainerMakerCfg
        acc.merge(ITkCaloClusterROIPhiRZContainerMakerCfg(flags))
    if kwargs["doHadCaloSeedSSS"]:
        from InDetConfig.InDetCaloClusterROISelectorConfig import ITkHadCaloClusterROIPhiRZContainerMakerCfg
        acc.merge(ITkHadCaloClusterROIPhiRZContainerMakerCfg(flags))
    kwargs.setdefault("phiWidth", flags.ITk.Tracking.ActivePass.phiWidthBrem[0])
    kwargs.setdefault("etaWidth", flags.ITk.Tracking.ActivePass.etaWidthBrem[0])
    kwargs.setdefault("EMROIPhiRZContainer", "ITkCaloClusterROIPhiRZ0GeV")
    kwargs.setdefault("HadROIPhiRZContainer", "ITkHadCaloClusterROIPhiRZ")

    kwargs.setdefault("UseAssociationTool", (len(InputCollections) > 0) and (flags.ITk.Tracking.ActivePass.usePrdAssociationTool))
    kwargs.setdefault("ITKGeometry", True)

    if flags.Beam.Type is BeamType.Cosmics:
        kwargs.setdefault("TrackPatternRecoInfo", 'SiSpacePointsSeedMaker_Cosmic')

    elif flags.ITk.Tracking.ActivePass.extension == "ConversionFinding":
        kwargs.setdefault("TrackPatternRecoInfo", 'SiSpacePointsSeedMaker_ITkConversionTracks')

    elif flags.ITk.Tracking.ActivePass.extension == "LargeD0":
        kwargs.setdefault("TrackPatternRecoInfo", 'SiSpacePointsSeedMaker_LargeD0')

    else:
        kwargs.setdefault("TrackPatternRecoInfo", 'SiSPSeededFinder')

    if flags.ITk.Tracking.doStoreTrackSeeds:
        from TrkConfig.AtlasExtrapolatorConfig import AtlasExtrapolatorCfg
        extrapolator = acc.popToolsAndMerge(AtlasExtrapolatorCfg(flags))
        acc.addPublicTool(extrapolator)  # TODO: migrate to private?

        kwargs.setdefault("SeedToTrackConversion", CompFactory.InDet.SeedToTrackConversionTool(
            name="ITkSeedToTrackConversion",
            Extrapolator=extrapolator,
            OutputName=f"SiSPSeedSegments{flags.ITk.Tracking.ActivePass.extension}"))
        kwargs.setdefault("SeedSegmentsWrite", True)

    ITkSiTrackMaker = CompFactory.InDet.SiTrackMaker_xk(name = name+flags.ITk.Tracking.ActivePass.extension, **kwargs)
    acc.setPrivateTools(ITkSiTrackMaker)
    return acc

def ITkSiSPSeededTrackFinderCfg(flags, name="ITkSiSpTrackFinder", InputCollections = None, SiSPSeededTrackCollectionKey = None, **kwargs) :
    acc = ComponentAccumulator()

    # set output track collection name
    #
    SiTrackCollection = SiSPSeededTrackCollectionKey

    ITkSiTrackMaker = acc.popToolsAndMerge(ITkSiTrackMaker_xkCfg(flags,
                                                                 InputCollections = InputCollections ))
    from TrkConfig.TrkExRungeKuttaPropagatorConfig import ITkPropagatorCfg
    ITkPropagator = acc.popToolsAndMerge(ITkPropagatorCfg(flags))
    acc.addPublicTool(ITkPropagator)
    from TrkConfig.TrkTrackSummaryToolConfig import ITkTrackSummaryToolNoHoleSearchCfg
    ITkTrackSummaryToolNoHoleSearch = acc.getPrimaryAndMerge(ITkTrackSummaryToolNoHoleSearchCfg(flags))
    ITkSiSpacePointsSeedMaker = None
    if flags.ITk.Tracking.ActivePass.extension != "ConversionFinding" and flags.Acts.TrackFinding.useSiSpacePointSeedMaker:
        from ActsTrkSeedingTool.ActsTrkSeedingToolConfig import ActsTrkSiSpacePointsSeedMakerCfg
        ITkSiSpacePointsSeedMaker = acc.popToolsAndMerge(ActsTrkSiSpacePointsSeedMakerCfg(flags,
                                                                                          InputCollections = InputCollections))
    else:
        ITkSiSpacePointsSeedMaker = acc.popToolsAndMerge(ITkSiSpacePointsSeedMakerCfg(flags,
                                                                                      InputCollections = InputCollections))
    #
    # --- Setup Track finder using space points seeds
    #
    kwargs.setdefault("TrackTool", ITkSiTrackMaker)
    kwargs.setdefault("PropagatorTool", ITkPropagator)
    if (len(InputCollections) > 0) and flags.ITk.Tracking.ActivePass.usePrdAssociationTool:
        # not all classes have that property !!!
        kwargs.setdefault("PRDtoTrackMap", 'ITkPRDtoTrackMap'+ flags.ITk.Tracking.ActivePass.extension)
    kwargs.setdefault("TrackSummaryTool", ITkTrackSummaryToolNoHoleSearch)
    kwargs.setdefault("TracksLocation", SiTrackCollection)
    kwargs.setdefault("SeedsTool", ITkSiSpacePointsSeedMaker)
    kwargs.setdefault("useZvertexTool", False)
    kwargs.setdefault("useZBoundFinding", flags.ITk.Tracking.ActivePass.doZBoundary)
    kwargs.setdefault("ITKGeometry", True)
    kwargs.setdefault("SpacePointsSCTName", "ITkStripSpacePoints")
    kwargs.setdefault("SpacePointsPixelName", "ITkPixelSpacePoints")

    if flags.ITk.Tracking.doFastTracking :
        kwargs.setdefault("doFastTracking", True)

        if 'InDetEtaDependentCutsSvc' not in kwargs :
            from InDet.InDetEtaDependentCutsConfig import ITkEtaDependentCutsSvcCfg
            acc.merge(ITkEtaDependentCutsSvcCfg(flags))
            kwargs.setdefault("InDetEtaDependentCutsSvc", acc.getService("ITkEtaDependentCutsSvc"+flags.ITk.Tracking.ActivePass.extension))

    ITkSiSPSeededTrackFinder = CompFactory.InDet.SiSPSeededTrackFinder(name = name+flags.ITk.Tracking.ActivePass.extension, **kwargs)
    acc.addEventAlgo(ITkSiSPSeededTrackFinder)
    return acc

def ITkSiSPSeededTrackFinderROIConvCfg(flags, name="ITkSiSpTrackFinderROIConv", InputCollections = None, SiSPSeededTrackCollectionKey = None, **kwargs) :
    from InDetConfig.InDetCaloClusterROISelectorConfig import ITkCaloClusterROIPhiRZContainerMakerCfg
    acc = ITkCaloClusterROIPhiRZContainerMakerCfg(flags)

    from RegionSelector.RegSelToolConfig import regSelTool_ITkStrip_Cfg
    RegSelTool_ITkStrip   = acc.popToolsAndMerge(regSelTool_ITkStrip_Cfg(flags))

    kwargs.setdefault("RegSelTool_Strip", RegSelTool_ITkStrip)
    kwargs.setdefault("useITkConvSeeded", True)
    kwargs.setdefault("EMROIPhiRZContainer", "ITkCaloClusterROIPhiRZ15GeVUnordered")

    acc.merge(ITkSiSPSeededTrackFinderCfg(flags, name = name,
                                          InputCollections = InputCollections,
                                          SiSPSeededTrackCollectionKey = SiSPSeededTrackCollectionKey,
                                          **kwargs))
    return acc

def ITkCopyAlgForAmbiCfg(flags, name="ITkCopyAlgForAmbi", InputTrackCollection = None, OutputTrackCollection = None, **kwargs) :
    acc = ComponentAccumulator()

    kwargs.setdefault("CollectionName", InputTrackCollection)
    kwargs.setdefault("AliasName", OutputTrackCollection)
    ITkCopyAlgForAmbi = CompFactory.Trk.TrkCollectionAliasAlg (name = name+flags.ITk.Tracking.ActivePass.extension, **kwargs)
    acc.addEventAlgo(ITkCopyAlgForAmbi)
    return acc

# ------------------------------------------------------------
#
# ----------- Setup Si Pattern for New tracking
#
# ------------------------------------------------------------
def ITkTrackingSiPatternCfg(flags, InputCollections = None, ResolvedTrackCollectionKey = None, SiSPSeededTrackCollectionKey = None , ClusterSplitProbContainer=''):
    acc = ComponentAccumulator()
    #
    # --- get list of already associated hits (always do this, even if no other tracking ran before)
    #
    if (len(InputCollections) > 0) and flags.ITk.Tracking.ActivePass.usePrdAssociationTool:
        from InDetConfig.InDetTrackPRD_AssociationConfig import ITkTrackPRD_AssociationCfg
        acc.merge(ITkTrackPRD_AssociationCfg(flags,
                                             name = 'ITkTrackPRD_Association' + flags.ITk.Tracking.ActivePass.extension,
                                             TracksName = list(InputCollections)))

    # ------------------------------------------------------------
    #
    # ----------- SiSPSeededTrackFinder
    #
    # ------------------------------------------------------------

    SiSPSeededTrackFinderCfg = ITkSiSPSeededTrackFinderCfg
    if flags.ITk.Tracking.ActivePass.extension == "ConversionFinding":
        SiSPSeededTrackFinderCfg = ITkSiSPSeededTrackFinderROIConvCfg
    acc.merge(SiSPSeededTrackFinderCfg( flags,
                                        InputCollections = InputCollections,
                                        SiSPSeededTrackCollectionKey = SiSPSeededTrackCollectionKey))
    # ------------------------------------------------------------
    #
    # ---------- Ambiguity solving
    #
    # ------------------------------------------------------------

    if flags.ITk.Tracking.doFastTracking:
        acc.merge(ITkCopyAlgForAmbiCfg( flags,
                                        InputTrackCollection = SiSPSeededTrackCollectionKey,
                                        OutputTrackCollection = ResolvedTrackCollectionKey ))

    else:
        from TrkConfig.TrkAmbiguitySolverConfig import ITkTrkAmbiguityScoreCfg, ITkTrkAmbiguitySolverCfg
        acc.merge(ITkTrkAmbiguityScoreCfg( flags,
                                           SiSPSeededTrackCollectionKey = SiSPSeededTrackCollectionKey,
                                           ClusterSplitProbContainer = ClusterSplitProbContainer))

        acc.merge(ITkTrkAmbiguitySolverCfg(flags,
                                           ResolvedTrackCollectionKey = ResolvedTrackCollectionKey))

    return acc

