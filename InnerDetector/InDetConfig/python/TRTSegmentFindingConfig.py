# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
from AthenaConfiguration.ComponentAccumulator import ComponentAccumulator
from AthenaConfiguration.ComponentFactory import CompFactory
import InDetConfig.TrackingCommonConfig as TC

def TRT_TrackSegmentsMaker_BarrelCosmicsCfg(flags, name='InDetTRTSegmentsMaker', **kwargs) :
    acc = ComponentAccumulator()

    kwargs.setdefault("TrtManagerLocation", 'TRT') # InDetKeys.TRT_Manager
    kwargs.setdefault("TRT_ClustersContainer", 'TRT_DriftCirclesUncalibrated') # InDetKeys.TRT_DriftCirclesUncalibrated
    kwargs.setdefault("IsMagneticFieldOn", flags.BField.solenoidOn)

    acc.setPrivateTools(CompFactory.InDet.TRT_TrackSegmentsMaker_BarrelCosmics(name = name, **kwargs))
    return acc

def TRT_TrackSegmentsMaker_ATLxkCfg(flags, name = 'InDetTRT_SeedsMaker', extension = '', TrackingCuts = None, InputCollections = None, **kwargs):
    acc = ComponentAccumulator()
    #
    # --- decide if use the association tool
    #
    usePrdAssociationTool = len(InputCollections) > 0

    #
    # --- get list of already associated hits (always do this, even if no other tracking ran before)
    #
    prefix = 'InDetSegment'
    suffix = extension
    #
    # --- cut values
    #
    if extension == "_TRT":
        # TRT Subdetector segment finding
        MinNumberDCs   = TrackingCuts.minTRTonly
        pTmin          = TrackingCuts.minPT
        sharedFrac     = TrackingCuts.maxTRTonlyShared
    else:
        # TRT-only/back-tracking segment finding
        MinNumberDCs   = TrackingCuts.minSecondaryTRTonTrk
        pTmin          = TrackingCuts.minSecondaryPt
        sharedFrac     = TrackingCuts.maxSecondaryTRTShared
    #
    # --- offline version  of TRT segemnt making
    #
    InDetPatternPropagator = TC.InDetPatternPropagatorCfg()
    acc.addPublicTool(InDetPatternPropagator)

    InDetTRTExtensionTool = acc.popToolsAndMerge(TC.InDetTRT_ExtensionToolCfg(flags, TrackingCuts = TrackingCuts))
    acc.addPublicTool(InDetTRTExtensionTool)

    kwargs.setdefault("TRT_ClustersContainer", 'TRT_DriftCircles') # InDetKeys.TRT_DriftCircles
    kwargs.setdefault("PropagatorTool", InDetPatternPropagator)
    kwargs.setdefault("TrackExtensionTool", InDetTRTExtensionTool)
    kwargs.setdefault("PRDtoTrackMap", prefix+'PRDtoTrackMap'+suffix if usePrdAssociationTool else '')
    kwargs.setdefault("RemoveNoiseDriftCircles", flags.InDet.removeTRTNoise)
    kwargs.setdefault("MinNumberDriftCircles", MinNumberDCs)
    kwargs.setdefault("NumberMomentumChannel", TrackingCuts.TRTSegFinderPtBins)
    kwargs.setdefault("pTmin", pTmin)
    kwargs.setdefault("sharedFrac", sharedFrac)

    InDetTRT_TrackSegmentsMaker = CompFactory.InDet.TRT_TrackSegmentsMaker_ATLxk(name = name, **kwargs)
    acc.setPrivateTools(InDetTRT_TrackSegmentsMaker)
    return acc

def TRT_TrackSegmentsMakerCondAlg_ATLxkCfg(name = 'InDetTRT_SeedsMakerCondAlg', extension = '', TrackingCuts = None, **kwargs):
    acc = ComponentAccumulator()
    #
    # --- cut values
    #
    if extension == "_TRT":
        # TRT Subdetector segment finding
        pTmin = TrackingCuts.minPT
    else:
        # TRT-only/back-tracking segment finding
        pTmin = TrackingCuts.minSecondaryPt

    InDetPatternPropagator = TC.InDetPatternPropagatorCfg()
    acc.addPublicTool(InDetPatternPropagator)

    kwargs.setdefault("PropagatorTool", InDetPatternPropagator)
    kwargs.setdefault("NumberMomentumChannel", TrackingCuts.TRTSegFinderPtBins)
    kwargs.setdefault("pTmin", pTmin)

    InDetTRT_TrackSegmentsMakerCondAlg = CompFactory.InDet.TRT_TrackSegmentsMakerCondAlg_ATLxk(name = name, **kwargs)
    acc.addCondAlgo(InDetTRT_TrackSegmentsMakerCondAlg)
    return acc

def TRT_TrackSegmentsFinderCfg(flags, name = 'InDetTRT_TrackSegmentsFinderPhase', extension = '', TrackingCuts = None, BarrelSegments = None, InputCollections =None, doPhase = False, **kwargs):
    acc = ComponentAccumulator()

    # ---------------------------------------------------------------
    #
    # --- now the main steering of the TRT segments finding
    #
    # ---------------------------------------------------------------
    if flags.Beam.Type == "cosmics":
        #
        # --- cosmics barrel segments (use TRT track segements even for NewT) 
        #
        if doPhase:
            InDetTRT_TrackSegmentsMakerPhase = acc.popToolsAndMerge(TRT_TrackSegmentsMaker_BarrelCosmicsCfg(flags, name='InDetTRTSegmentsMakerPhase'+extension))
            acc.addPublicTool(InDetTRT_TrackSegmentsMakerPhase)

            kwargs.setdefault("SegmentsMakerTool", InDetTRT_TrackSegmentsMakerPhase)
            kwargs.setdefault("SegmentsLocation", BarrelSegments)
        else:
            InDetTRT_TrackSegmentsMaker = acc.popToolsAndMerge(TRT_TrackSegmentsMaker_BarrelCosmicsCfg(flags, name='InDetTRTSegmentsMaker'+extension,
                                                                                                              TRT_ClustersContainer = 'TRT_DriftCircles')) # InDetKeys.TRT_DriftCircles
            acc.addPublicTool(InDetTRT_TrackSegmentsMaker)

            if flags.InDet.doCaloSeededTRTSegments or TrackingCuts.RoISeededBackTracking:
                kwargs.setdefault("SegmentsMakerTool", InDetTRT_TrackSegmentsMaker)
                kwargs.setdefault("SegmentsLocation", BarrelSegments)
                kwargs.setdefault("useCaloSeeds", True)
                kwargs.setdefault("InputClusterContainerName", 'InDetCaloClusterROIs') # InDetKeys.CaloClusterROIContainer
            else:
                kwargs.setdefault("SegmentsMakerTool", InDetTRT_TrackSegmentsMaker)
                kwargs.setdefault("SegmentsLocation", BarrelSegments)
                kwargs.setdefault("InputClusterContainerName", "")
    else:
        #
        # --- offline version  of TRT segemnt making
        #
        InDetTRT_TrackSegmentsMaker = acc.popToolsAndMerge(TRT_TrackSegmentsMaker_ATLxkCfg( flags, 
                                                                                            name = 'InDetTRT_SeedsMaker'+extension, 
                                                                                            extension = extension,
                                                                                            TrackingCuts = TrackingCuts,
                                                                                            InputCollections = InputCollections))
        acc.addPublicTool(InDetTRT_TrackSegmentsMaker)
        kwargs.setdefault("SegmentsMakerTool", InDetTRT_TrackSegmentsMaker)

        acc.merge(TRT_TrackSegmentsMakerCondAlg_ATLxkCfg(name = 'InDetTRT_SeedsMakerCondAlg'+ extension, 
                                                         extension = extension,
                                                         TrackingCuts=TrackingCuts))

    acc.addEventAlgo(CompFactory.InDet.TRT_TrackSegmentsFinder( name = name, **kwargs))
    return acc

def SegmentDriftCircleAssValidationCfg(flags, name="InDetSegmentDriftCircleAssValidation", extension='', TrackingCuts=None, BarrelSegments='', **kwargs):
    acc = ComponentAccumulator()
    #
    # --- cut values
    #
    if extension == "_TRT":
        # TRT Subdetector segment finding
        MinNumberDCs = TrackingCuts.minTRTonly
        pTmin        = TrackingCuts.minPT
    else:
        # TRT-only/back-tracking segment finding
        MinNumberDCs = TrackingCuts.minSecondaryTRTonTrk
        pTmin        = TrackingCuts.minSecondaryPt

    #kwargs.setdefault("OrigTracksLocation", BarrelSegments)
    kwargs.setdefault("TRT_DriftCirclesName", 'TRT_DriftCircles') # InDetKeys.TRT_DriftCircles
    kwargs.setdefault("pTmin", pTmin)
    kwargs.setdefault("Pseudorapidity", 2.1) # end of TRT
    kwargs.setdefault("RadiusMin", 0.)
    kwargs.setdefault("RadiusMax", 600.)
    kwargs.setdefault("MinNumberDCs", MinNumberDCs)

    InDetSegmentDriftCircleAssValidation = CompFactory.InDet.SegmentDriftCircleAssValidation(name = name, **kwargs)
    acc.addEventAlgo(InDetSegmentDriftCircleAssValidation)
    return acc

def TRTActiveCondAlgCfg(flags, name="TRTActiveCondAlg", **kwargs):
    acc = ComponentAccumulator()

    InDetTRTStrawStatusSummaryTool = acc.popToolsAndMerge(TC.InDetTRTStrawStatusSummaryToolCfg(flags))
    acc.addPublicTool(InDetTRTStrawStatusSummaryTool)

    kwargs.setdefault("TRTStrawStatusSummaryTool", InDetTRTStrawStatusSummaryTool)

    TRTActiveCondAlg = CompFactory.TRTActiveCondAlg(name = name, **kwargs)
    acc.addCondAlgo(TRTActiveCondAlg)
    return acc

def TRTSegmentFindingCfg(flags, extension = "", InputCollections = None, NewTrackingCuts = None, BarrelSegments = None, doPhase = False):
    acc = ComponentAccumulator()
    #
    # --- decide if use the association tool
    #
    usePrdAssociationTool = len(InputCollections) > 0

    #
    # --- get list of already associated hits (always do this, even if no other tracking ran before)
    #
    prefix = 'InDetSegment'
    suffix = extension
    if usePrdAssociationTool:
        acc.merge(TC.InDetTrackPRD_AssociationCfg(flags, namePrefix = prefix,
                                                         nameSuffix = suffix,
                                                         TracksName = list(InputCollections)))
    #
    # --- TRT track reconstruction
    #
    acc.merge(TRT_TrackSegmentsFinderCfg( flags,
                                          name = 'InDetTRT_TrackSegmentsFinderPhase'+extension,
                                          extension =extension,
                                          TrackingCuts = NewTrackingCuts,
                                          BarrelSegments=BarrelSegments,
                                          InputCollections = InputCollections,
                                          doPhase = doPhase))
    #
    # --- load TRT validation alg
    #
    
    if flags.InDet.doTruth and not flags.Beam.Type == "cosmics":
        acc.merge(SegmentDriftCircleAssValidationCfg(flags,
                                                    name="InDetSegmentDriftCircleAssValidation"+extension,
                                                    TrackingCuts = NewTrackingCuts,
                                                    BarrelSegments=BarrelSegments))
    
    return acc


if __name__ == "__main__":
    from AthenaCommon.Configurable import Configurable
    Configurable.configurableRun3Behavior=1

    from AthenaConfiguration.AllConfigFlags import ConfigFlags
    from AthenaConfiguration.TestDefaults import defaultTestFiles
    ConfigFlags.Input.Files=defaultTestFiles.RDO

    ConfigFlags.Detector.RecoTRT = True
    ConfigFlags.Detector.RecoIBL = True

    numThreads=1
    ConfigFlags.Concurrency.NumThreads=numThreads
    ConfigFlags.Concurrency.NumConcurrentEvents=numThreads # Might change this later, but good enough for the moment.

    ConfigFlags.lock()
    ConfigFlags.dump()

    from AthenaConfiguration.MainServicesConfig import MainServicesCfg
    top_acc = MainServicesCfg(ConfigFlags)

    msgService = top_acc.getService('MessageSvc')
    msgService.Format = "S:%s E:%e % F%138W%S%7W%R%T  %0W%M"
    
    from AthenaPoolCnvSvc.PoolReadConfig import PoolReadCfg
    top_acc.merge(PoolReadCfg(ConfigFlags))

    from MagFieldServices.MagFieldServicesConfig import MagneticFieldSvcCfg
    top_acc.merge(MagneticFieldSvcCfg(ConfigFlags))

    from TRT_GeoModel.TRT_GeoModelConfig import TRT_GeometryCfg
    top_acc.merge(TRT_GeometryCfg( ConfigFlags ))

    from PixelGeoModel.PixelGeoModelConfig import PixelGeometryCfg
    from SCT_GeoModel.SCT_GeoModelConfig import SCT_GeometryCfg
    top_acc.merge( PixelGeometryCfg(ConfigFlags) )
    top_acc.merge( SCT_GeometryCfg(ConfigFlags) )

    # NewTracking collection keys
    InputCombinedInDetTracks = []

    InDetNewTrackingCuts = ConfigFlags.InDet.Tracking
    #############################################################################
    top_acc.merge(TRTActiveCondAlgCfg(ConfigFlags))
    top_acc.merge(TC.TRT_DetElementsRoadCondAlgCfg())

    from InDetConfig.TRTPreProcessing import TRTPreProcessingCfg
    if not ConfigFlags.InDet.doDBMstandalone:
        top_acc.merge(TRTPreProcessingCfg(ConfigFlags,(not ConfigFlags.InDet.doTRTPhaseCalculation or ConfigFlags.Beam.Type =="collisions"),False))

    top_acc.merge(TRTSegmentFindingCfg( ConfigFlags,
                                        "",
                                        InputCombinedInDetTracks,
                                        InDetNewTrackingCuts,
                                        'TRTSegments')) # InDetKeys.TRT_Segments
    #############################################################################

    iovsvc = top_acc.getService('IOVDbSvc')
    iovsvc.OutputLevel=5
    top_acc.getService('StoreGateSvc').Dump = True
    top_acc.printConfig(withDetails = True, summariseProps = True)
    top_acc.run(25)
    top_acc.store(open("test_TRTSegmentFinding.pkl", "wb"))