# Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
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

def TRT_TrackSegmentsMaker_ATLxkCfg(flags, name = 'InDetTRT_SeedsMaker', extension = '', InputCollections = None, **kwargs):
    acc = ComponentAccumulator()
    #
    # --- decide if use the association tool
    #
    usePrdAssociationTool = InputCollections is not None

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
        MinNumberDCs   = flags.InDet.Tracking.ActivePass.minTRTonly
        pTmin          = flags.InDet.Tracking.ActivePass.minPT
        sharedFrac     = flags.InDet.Tracking.ActivePass.maxTRTonlyShared
    else:
        # TRT-only/back-tracking segment finding
        MinNumberDCs   = flags.InDet.Tracking.ActivePass.minSecondaryTRTonTrk
        pTmin          = flags.InDet.Tracking.ActivePass.minSecondaryPt
        sharedFrac     = flags.InDet.Tracking.ActivePass.maxSecondaryTRTShared
    #
    # --- offline version  of TRT segemnt making
    #
    InDetPatternPropagator = acc.getPrimaryAndMerge(TC.InDetPatternPropagatorCfg())

    InDetTRTExtensionTool = acc.popToolsAndMerge(TC.InDetTRT_ExtensionToolCfg(flags))
    acc.addPublicTool(InDetTRTExtensionTool)

    kwargs.setdefault("TRT_ClustersContainer", 'TRT_DriftCircles') # InDetKeys.TRT_DriftCircles
    kwargs.setdefault("PropagatorTool", InDetPatternPropagator)
    kwargs.setdefault("TrackExtensionTool", InDetTRTExtensionTool)
    kwargs.setdefault("PRDtoTrackMap", prefix+'PRDtoTrackMap'+suffix if usePrdAssociationTool else '')
    kwargs.setdefault("RemoveNoiseDriftCircles", False)
    kwargs.setdefault("MinNumberDriftCircles", MinNumberDCs)
    kwargs.setdefault("NumberMomentumChannel", flags.InDet.Tracking.ActivePass.TRTSegFinderPtBins)
    kwargs.setdefault("pTmin", pTmin)
    kwargs.setdefault("sharedFrac", sharedFrac)

    InDetTRT_TrackSegmentsMaker = CompFactory.InDet.TRT_TrackSegmentsMaker_ATLxk(name = name, **kwargs)
    acc.setPrivateTools(InDetTRT_TrackSegmentsMaker)
    return acc

def TRT_TrackSegmentsMakerCondAlg_ATLxkCfg(flags, name = 'InDetTRT_SeedsMakerCondAlg', extension = '', **kwargs):
    acc = ComponentAccumulator()
    #
    # --- cut values
    #
    if extension == "_TRT":
        # TRT Subdetector segment finding
        pTmin = flags.InDet.Tracking.ActivePass.minPT
    else:
        # TRT-only/back-tracking segment finding
        pTmin = flags.InDet.Tracking.ActivePass.minSecondaryPt

    InDetPatternPropagator = acc.getPrimaryAndMerge(TC.InDetPatternPropagatorCfg())

    kwargs.setdefault("PropagatorTool", InDetPatternPropagator)
    kwargs.setdefault("NumberMomentumChannel", flags.InDet.Tracking.ActivePass.TRTSegFinderPtBins)
    kwargs.setdefault("pTmin", pTmin)

    InDetTRT_TrackSegmentsMakerCondAlg = CompFactory.InDet.TRT_TrackSegmentsMakerCondAlg_ATLxk(name = name, **kwargs)
    acc.addCondAlgo(InDetTRT_TrackSegmentsMakerCondAlg)
    return acc

def TRT_TrackSegmentsFinderCfg(flags, name = 'InDetTRT_TrackSegmentsFinder', extension = '', BarrelSegments = None, InputCollections =None, doPhase = False, **kwargs):
    from MagFieldServices.MagFieldServicesConfig import MagneticFieldSvcCfg
    acc = MagneticFieldSvcCfg(flags)

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

            if flags.Detector.EnableCalo and flags.InDet.Tracking.ActivePass.RoISeededBackTracking:
                from InDetConfig.InDetRecCaloSeededROISelectionConfig import CaloClusterROI_SelectorCfg
                acc.merge(CaloClusterROI_SelectorCfg(flags))
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
                                                                                            InputCollections = InputCollections))
        acc.addPublicTool(InDetTRT_TrackSegmentsMaker)
        kwargs.setdefault("SegmentsMakerTool", InDetTRT_TrackSegmentsMaker)

        acc.merge(TRT_TrackSegmentsMakerCondAlg_ATLxkCfg(flags, 
                                                         name = 'InDetTRT_SeedsMakerCondAlg'+ extension, 
                                                         extension = extension))

    acc.addEventAlgo(CompFactory.InDet.TRT_TrackSegmentsFinder( name = name, **kwargs))
    return acc

def SegmentDriftCircleAssValidationCfg(flags, name="InDetSegmentDriftCircleAssValidation", extension='', BarrelSegments='', **kwargs):
    acc = ComponentAccumulator()
    #
    # --- cut values
    #
    if extension == "_TRT":
        # TRT Subdetector segment finding
        MinNumberDCs = flags.InDet.Tracking.ActivePass.minTRTonly
        pTmin        = flags.InDet.Tracking.ActivePass.minPT
    else:
        # TRT-only/back-tracking segment finding
        MinNumberDCs = flags.InDet.Tracking.ActivePass.minSecondaryTRTonTrk
        pTmin        = flags.InDet.Tracking.ActivePass.minSecondaryPt

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

def TRTSegmentFindingCfg(flags, extension = "", InputCollections = None, BarrelSegments = None, doPhase = False):

    acc = ComponentAccumulator()
    #
    # --- decide if use the association tool
    #
    usePrdAssociationTool = InputCollections is not None

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
                                          name = 'InDetTRT_TrackSegmentsFinder'+extension,
                                          extension =extension,
                                          BarrelSegments=BarrelSegments,
                                          InputCollections = InputCollections,
                                          doPhase = doPhase))
    #
    # --- load TRT validation alg
    #
    
    if flags.InDet.doTruth and not flags.Beam.Type == "cosmics":
        acc.merge(SegmentDriftCircleAssValidationCfg(flags,
                                                    name="InDetSegmentDriftCircleAssValidation"+extension,
                                                    BarrelSegments=BarrelSegments))
    
    return acc


if __name__ == "__main__":
    from AthenaCommon.Configurable import Configurable
    Configurable.configurableRun3Behavior=1

    from AthenaConfiguration.AllConfigFlags import ConfigFlags as flags
    from AthenaConfiguration.TestDefaults import defaultTestFiles
    flags.Input.Files=defaultTestFiles.RDO


    numThreads=1
    flags.Concurrency.NumThreads=numThreads
    flags.Concurrency.NumConcurrentEvents=numThreads # Might change this later, but good enough for the moment.

    flags.lock()
    flags.dump()

    from AthenaConfiguration.MainServicesConfig import MainServicesCfg
    top_acc = MainServicesCfg(flags)
    
    from AthenaPoolCnvSvc.PoolReadConfig import PoolReadCfg
    top_acc.merge(PoolReadCfg(flags))

    # NewTracking collection keys
    InputCombinedInDetTracks = []

    if not flags.InDet.Tracking.doDBMstandalone:
        from InDetConfig.TRTPreProcessing import TRTPreProcessingCfg
        top_acc.merge(TRTPreProcessingCfg(flags,
                                          useTimeInfo = not flags.InDet.Tracking.doTRTPhaseCalculation or flags.Beam.Type =="collisions",
                                          usePhase = False))

    top_acc.merge(TRTSegmentFindingCfg( flags,
                                        extension = "",
                                        InputCollections = InputCombinedInDetTracks,
                                        BarrelSegments = 'TRTSegments'))
    #############################################################################

    iovsvc = top_acc.getService('IOVDbSvc')
    iovsvc.OutputLevel=5

    top_acc.store(open("test_TRTSegmentFinding.pkl", "wb"))

    import sys
    if "--norun" not in sys.argv:
        sc = top_acc.run(5)
        sys.exit(not sc.isSuccess())
