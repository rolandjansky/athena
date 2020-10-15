# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

from AthenaConfiguration.ComponentAccumulator import ComponentAccumulator
from AthenaConfiguration.ComponentFactory import CompFactory

def NnPixelClusterSplitProbToolCfg(flags, name="NnPixelClusterSplitProbTool", **kwargs):
    acc = ComponentAccumulator()

    # --- new NN prob tool
    MultiplicityContent = [1 , 1 , 1]

    from InDetConfig.TrackingCommonConfig import NnClusterizationFactoryCfg
    NnClusterizationFactory = NnClusterizationFactoryCfg(flags)
    acc.addPublicTool(NnClusterizationFactory)

    useBeamConstraint = flags.InDet.useBeamConstraint

    kwargs.setdefault("PriorMultiplicityContent", MultiplicityContent)
    kwargs.setdefault("NnClusterizationFactory", NnClusterizationFactory)
    kwargs.setdefault("useBeamSpotInfo", useBeamConstraint)

    if flags.InDet.doSLHC:
        NnPixelClusterSplitProbTool = CompFactory.InDet.TruthPixelClusterSplitProbTool(name=name,**kwargs)
    else:
        NnPixelClusterSplitProbTool = CompFactory.InDet.NnPixelClusterSplitProbTool(name=name,**kwargs)

    acc.setPrivateTools(NnPixelClusterSplitProbTool)
    return acc

def NnPixelClusterSplitterCfg(flags, name="NnPixelClusterSplitter", **kwargs):
    acc = ComponentAccumulator()

    from InDetConfig.TrackingCommonConfig import NnClusterizationFactoryCfg
    NnClusterizationFactory = NnClusterizationFactoryCfg(flags)
    acc.addPublicTool(NnClusterizationFactory)

    useBeamConstraint = flags.InDet.useBeamConstraint

    kwargs.setdefault("NnClusterizationFactory", NnClusterizationFactory)
    kwargs.setdefault("ThresholdSplittingIntoTwoClusters", 0.5)
    kwargs.setdefault("ThresholdSplittingIntoThreeClusters", 0.25)
    kwargs.setdefault("SplitOnlyOnBLayer", False)
    kwargs.setdefault("useBeamSpotInfo", useBeamConstraint)
    # --- new NN splitter
    if flags.InDet.doSLHC:
        NnPixelClusterSplitter = CompFactory.InDet.TruthPixelClusterSplitter(name=name,**kwargs)
    else:
        NnPixelClusterSplitter = CompFactory.InDet.NnPixelClusterSplitter(name=name,**kwargs)

    acc.setPrivateTools(NnPixelClusterSplitter)
    return acc

def InDetRecPreProcessingSiliconCfg(flags, **kwargs):
    acc = ComponentAccumulator()
    # ------------------------------------------------------------
    #
    # ----------- Data-Preparation stage
    #
    # ------------------------------------------------------------
    #
    # ----------- PrepRawData creation from Raw Data Objects
    #
    redoPatternRecoAndTracking = False

    if flags.InDet.doPRDFormation:
        #
        # --- Slim BCM RDOs by zero-suppressing
        #   
        if flags.Detector.RecoBCM:
            from InDetConfig.TrackRecoConfig import BCM_ZeroSuppressionCfg
            acc.merge(BCM_ZeroSuppressionCfg(flags))
        
        if flags.Detector.RecoPixel or flags.Detector.RecoSCT:
            #
            # --- SiLorentzAngleTool
            #
            from SiLorentzAngleTool.PixelLorentzAngleConfig import PixelLorentzAngleTool, PixelLorentzAngleCfg
            PixelLorentzAngleTool = PixelLorentzAngleTool(flags)
            acc.addPublicTool(PixelLorentzAngleTool)
            acc.addPublicTool(acc.popToolsAndMerge(PixelLorentzAngleCfg(flags)))

            from SiLorentzAngleTool.SCT_LorentzAngleConfig import SCT_LorentzAngleCfg
            SCTLorentzAngleTool = acc.popToolsAndMerge( SCT_LorentzAngleCfg(flags) )    
            acc.addPublicTool(SCTLorentzAngleTool)
            #
            # --- ClusterMakerTool (public), needed by Pixel and SCT Clusterization
            #
            from InDetConfig.TrackRecoConfig import ClusterMakerToolCfg
            acc.merge(ClusterMakerToolCfg(flags, PixelLorentzAngleTool=PixelLorentzAngleTool, SCTLorentzAngleTool=SCTLorentzAngleTool))
        #
        # -- Pixel Clusterization
        #
        if (flags.Detector.RecoPixel and flags.InDet.doPixelPRDFormation) or redoPatternRecoAndTracking:
            #
            # --- do we use new splittig or not ?
            #
            if flags.InDet.doPixelClusterSplitting:
                #
                # --- Neutral Network version ?
                #
                if flags.InDet.pixelClusterSplittingType == 'NeuralNet':
                    NnPixelClusterSplitProbTool = acc.popToolsAndMerge(NnPixelClusterSplitProbToolCfg(flags))
                    acc.addPublicTool(NnPixelClusterSplitProbTool)
                
                    NnPixelClusterSplitter = acc.popToolsAndMerge(NnPixelClusterSplitterCfg(flags))
                    acc.addPublicTool(NnPixelClusterSplitter)
                #
                # --- Neutral Network version ?
                #
                elif flags.InDet.pixelClusterSplittingType == 'AnalogClus':      
                    # new splitter tool
                    TotPixelClusterSplitter=CompFactory.InDet.TotPixelClusterSplitter (name="TotPixelClusterSplitter")
                    acc.addPublicTool(TotPixelClusterSplitter)
            #
            # --- PixelClusterization algorithm
            #
            from InDetConfig.TrackRecoConfig import PixelClusterizationCfg
            acc.merge(PixelClusterizationCfg(flags))
            if flags.InDet.doSplitReco :
                from InDetConfig.TrackRecoConfig import PixelClusterizationPUCfg
                acc.merge(PixelClusterizationPUCfg(flags))
        #
        # --- SCT Clusterization
        #
        if flags.Detector.RecoSCT and flags.InDet.doSCT_PRDFormation:
            #
            # --- SCT_Clusterization algorithm
            #
            from InDetConfig.TrackRecoConfig import SCTClusterizationCfg
            acc.merge(SCTClusterizationCfg(flags))
            if flags.InDet.doSplitReco :
                from InDetConfig.TrackRecoConfig import SCTClusterizationPUCfg
                acc.merge(SCTClusterizationPUCfg(flags))
    #
    # ----------- form SpacePoints from clusters in SCT and Pixels
    #
    #
    if flags.InDet.doSpacePointFormation:
        from InDetConfig.SpacePointFinder import InDetSiTrackerSpacePointFinderCfg, InDetSiElementPropertiesTableCondAlgCfg
        acc.merge(InDetSiElementPropertiesTableCondAlgCfg(flags))
        acc.merge(InDetSiTrackerSpacePointFinderCfg(flags))

    # this truth must only be done if you do PRD and SpacePointformation
    # If you only do the latter (== running on ESD) then the needed input (simdata)
    # is not in ESD but the resulting truth (clustertruth) is already there ...
    if flags.InDet.doPRDFormation and flags.InDet.doSpacePointFormation:
        if flags.InDet.doTruth:
            from InDetConfig.SpacePointFinder import InDetPRD_MultiTruthMakerSiCfg
            acc.merge(InDetPRD_MultiTruthMakerSiCfg(flags))
            if flags.InDet.doSplitReco:
                from InDetConfig.SpacePointFinder import InDetPRD_MultiTruthMakerSiPUCfg
                acc.merge(InDetPRD_MultiTruthMakerSiPUCfg(flags))

    return acc


if __name__ == "__main__":
    from AthenaCommon.Configurable import Configurable
    Configurable.configurableRun3Behavior=1

    from AthenaConfiguration.AllConfigFlags import ConfigFlags
    from AthenaConfiguration.TestDefaults import defaultTestFiles
    ConfigFlags.Input.Files=defaultTestFiles.RDO

    ConfigFlags.Detector.GeometryPixel   = True 
    ConfigFlags.Detector.GeometrySCT   = True
    ConfigFlags.InDet.doPixelClusterSplitting = True

    ConfigFlags.Detector.RecoIBL = True
    ConfigFlags.Detector.RecoPixel = True
    ConfigFlags.Detector.RecoSCT = True
    ConfigFlags.Detector.RecoTRT = True
    
    numThreads=1
    ConfigFlags.Concurrency.NumThreads=numThreads
    ConfigFlags.Concurrency.NumConcurrentEvents=numThreads

    ConfigFlags.loadAllDynamicFlags()

    ConfigFlags.lock()
    ConfigFlags.dump()
    
    from AthenaConfiguration.MainServicesConfig import MainServicesCfg
    top_acc = MainServicesCfg(ConfigFlags)

    from AthenaPoolCnvSvc.PoolReadConfig import PoolReadCfg
    top_acc.merge(PoolReadCfg(ConfigFlags))

    from MagFieldServices.MagFieldServicesConfig import MagneticFieldSvcCfg
    top_acc.merge(MagneticFieldSvcCfg(ConfigFlags))

    from BeamSpotConditions.BeamSpotConditionsConfig import BeamSpotCondAlgCfg
    top_acc.merge(BeamSpotCondAlgCfg(ConfigFlags))

    from PixelGeoModel.PixelGeoModelConfig import PixelGeometryCfg
    top_acc.merge( PixelGeometryCfg(ConfigFlags) )

    from SCT_GeoModel.SCT_GeoModelConfig import SCT_GeometryCfg
    top_acc.merge(SCT_GeometryCfg(ConfigFlags))

    from PixelConditionsAlgorithms.PixelConditionsConfig import PixelDetectorElementCondAlgCfg, PixelAlignCondAlgCfg
    top_acc.merge(PixelAlignCondAlgCfg(ConfigFlags))
    top_acc.merge(PixelDetectorElementCondAlgCfg(ConfigFlags))

    top_acc.merge(InDetRecPreProcessingSiliconCfg(ConfigFlags))

    iovsvc = top_acc.getService('IOVDbSvc')
    iovsvc.OutputLevel=5

    top_acc.getService('StoreGateSvc').Dump = True
    top_acc.printConfig(withDetails = True, summariseProps = True)
    top_acc.run(25)
