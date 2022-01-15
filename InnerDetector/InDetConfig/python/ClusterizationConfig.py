# Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
from AthenaConfiguration.ComponentAccumulator import ComponentAccumulator

from InDetConfig.TrackRecoConfig 	import BCM_ZeroSuppressionCfg, PixelClusterizationCfg, \
                                   PixelClusterizationPUCfg, SCTClusterizationCfg, \
                                   SCTClusterizationPUCfg

#arg_TrackingCuts             = 'TrackingCuts'
#arg_TrackCollectionKeys      = 'tracks'
#arg_TrackCollectionTruthKeys = 'truth'

def InDetClusterizationAlgorithmsCfg(flags, **kwargs) :
    top_acc = ComponentAccumulator()
    ### configure top_acc to be able to read input file
    from AthenaPoolCnvSvc.PoolReadConfig import PoolReadCfg
    top_acc.merge(PoolReadCfg(flags))
   
    ### obtain pixel and SCT geometry
    from PixelGeoModel.PixelGeoModelConfig import PixelReadoutGeometryCfg
    from SCT_GeoModel.SCT_GeoModelConfig   import SCT_ReadoutGeometryCfg
    top_acc.merge( PixelReadoutGeometryCfg(flags) )
    top_acc.merge( SCT_ReadoutGeometryCfg(flags) )


    #redoPatternRecoAndTracking = kwargs.pop('redoPatternRecoAndTracking')
    # @TODO propagate suffix and/or prefix ?
    top_acc.merge( BCM_ZeroSuppressionCfg(flags))
   
    # Pixel clusterization
    ## @TODO is this correct flag to be used here to turn on pixel clusterization
    if flags.Detector.GeometryPixel:
        top_acc.merge( PixelClusterizationCfg(flags, **kwargs) )
        if flags.InDet.doSplitReco :
            top_acc.merge( PixelClusterizationPUCfg(flags, **kwargs) )

    # SCT clusterization
    ## @TODO is this correct flag to be used here to turn on SCT clusterization
    if flags.Detector.GeometrySCT:
        top_acc.merge( SCTClusterizationCfg(flags, **kwargs) )
        if flags.InDet.doSplitReco :
            top_acc.merge( SCTClusterizationPUCfg(flags, **kwargs) )

    # from PixelConditionsTools.PixelConditionsSummaryConfig import PixelConditionsSummaryCfg
    # top_acc.merge( PixelConditionsSummaryCfg(flags))
    # FIXME - the above returns a tool. Need to do something with it!
    
    from SiLorentzAngleTool.SCT_LorentzAngleConfig import SCT_LorentzAngleCfg
    top_acc.popToolsAndMerge(SCT_LorentzAngleCfg(flags))
    return top_acc

if __name__ == "__main__":
    # Run this with python -m InDetConfig.ClusterizationConfig
    from AthenaCommon.Configurable import Configurable
    Configurable.configurableRun3Behavior=1

    from AthenaConfiguration.AllConfigFlags import ConfigFlags

    numThreads=1
    ConfigFlags.Concurrency.NumThreads=numThreads
    ConfigFlags.Concurrency.NumConcurrentEvents=numThreads # Might change this later, but good enough for the moment.

    ConfigFlags.Detector.GeometryPixel   = True 
    ConfigFlags.Detector.GeometrySCT   = True
    ConfigFlags.InDet.Tracking.doPixelClusterSplitting = True

    from AthenaConfiguration.TestDefaults import defaultTestFiles
    ConfigFlags.Input.Files = defaultTestFiles.RDO
    ConfigFlags.lock()
    ConfigFlags.dump()

    from AthenaConfiguration.MainServicesConfig import MainServicesCfg
    top_acc = MainServicesCfg(ConfigFlags)

    from PixelConditionsAlgorithms.PixelConditionsConfig import PixelHitDiscCnfgAlgCfg
    top_acc.merge(PixelHitDiscCnfgAlgCfg(ConfigFlags))

    from PixelRawDataByteStreamCnv.PixelRawDataByteStreamCnvConfig import PixelRawDataProviderAlgCfg
    top_acc.merge(PixelRawDataProviderAlgCfg(ConfigFlags))

    acc = InDetClusterizationAlgorithmsCfg(ConfigFlags)
    top_acc.merge(acc)

    iovsvc = top_acc.getService('IOVDbSvc')
    iovsvc.OutputLevel=5

    top_acc.printConfig()
    top_acc.run(25)
    top_acc.store(open("test_Clusterization.pkl", "wb"))
