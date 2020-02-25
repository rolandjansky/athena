# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

from InDetConfig.TrackReco 	import InDetBCM_ZeroSuppressionCfg, InDetPixelClusterizationCfg, \
                                   InDetPixelClusterizationPUCfg, InDet_SCTClusterizationCfg, \
                                   InDet_SCTClusterizationPUCfg

#arg_TrackingCuts             = 'TrackingCuts'
#arg_TrackCollectionKeys      = 'tracks'
#arg_TrackCollectionTruthKeys = 'truth'

def InDetClusterizationAlgorithmsCfg(flags, **kwargs) :

    from AthenaCommon.Configurable import Configurable
    Configurable.configurableRun3Behavior = 1

    from AthenaConfiguration.MainServicesConfig import MainServicesThreadedCfg
    #top_acc=MainServicesMiniCfg(flags)
    top_acc = MainServicesThreadedCfg(flags)
    #top_acc=MainServicesSerialCfg()
    ##top_acc = ComponentAccumulator()
    ### configure top_acc to be able to read input file
    from AthenaPoolCnvSvc.PoolReadConfig import PoolReadCfg
    top_acc.merge(PoolReadCfg(flags))
   
    ### obtain pixel and SCT geometry
    from PixelGeoModel.PixelGeoModelConfig import PixelGeometryCfg
    from SCT_GeoModel.SCT_GeoModelConfig	import SCT_GeometryCfg
    top_acc.merge( PixelGeometryCfg(flags) )
    top_acc.merge( SCT_GeometryCfg(flags) )


    #redoPatternRecoAndTracking = kwargs.pop('redoPatternRecoAndTracking')
    # @TODO propagate suffix and/or prefix ?
    top_acc.merge(InDetBCM_ZeroSuppressionCfg(flags))
   
    # Pixel clusterization
    ## @TODO is this correct flag to be used here to turn on pixel clusterization
    if flags.Detector.GeometryPixel:
        top_acc.merge(InDetPixelClusterizationCfg(flags, **kwargs) )
        if flags.InDet.doSplitReco :
            top_acc.merge(InDetPixelClusterizationPUCfg(flags, **kwargs) )

    # SCT clusterization
    ## @TODO is this correct flag to be used here to turn on SCT clusterization
    if flags.Detector.GeometrySCT:
        top_acc.merge( InDet_SCTClusterizationCfg(flags, **kwargs) )
        if flags.InDet.doSplitReco :
            top_acc.merge( InDet_SCTClusterizationPUCfg(flags, **kwargs) )

    # from PixelConditionsTools.PixelConditionsSummaryConfig import PixelConditionsSummaryCfg
    # top_acc.merge( PixelConditionsSummaryCfg(flags))
    # FIXME - the above returns a tool. Need to do something with it!
    
    from SiLorentzAngleTool.SCT_LorentzAngleConfig import SCT_LorentzAngleCfg
    top_acc.popToolsAndMerge(SCT_LorentzAngleCfg(flags))
    return top_acc

if __name__ == "__main__":
    # Run this with python -m InDetConfig.PixelClusterizationConfig
    from AthenaCommon.Configurable import Configurable
    Configurable.configurableRun3Behavior=1

    from AthenaConfiguration.AllConfigFlags import ConfigFlags
    flags = ConfigFlags

    ConfigFlags.Detector.GeometryPixel   = True 
    ConfigFlags.Detector.GeometrySCT   = True

    ConfigFlags.IOVDb.GlobalTag = "OFLCOND-MC16-SDR-16"
    ConfigFlags.Input.isMC = True
    ConfigFlags.GeoModel.Align.Dynamic    = False
    ConfigFlags.GeoModel.AtlasVersion = 'ATLAS-R2-2016-01-00-01'
    ConfigFlags.InDet.doPixelClusterSplitting = True

    from AthenaConfiguration.TestDefaults import defaultTestFiles
    ConfigFlags.Input.Files = defaultTestFiles.RDO
    ConfigFlags.lock()
    ConfigFlags.dump()

    acc = InDetClusterizationAlgorithmsCfg(ConfigFlags)

    ##acc.setAppProperty("EvtMax",25)
    ##acc.store(open("test_SiClusterization.pkl", "w"))
    acc.run(25)
    #with open('test4.pkl', mode="wb") as f:
    #   dill.dump(acc, f)
    acc.store(open("test00.pkl", "w"))