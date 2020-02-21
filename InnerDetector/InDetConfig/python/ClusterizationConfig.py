import TrackReco 	   as TrackReco
import PixelGeoModelConfig      as PixelGeoModelConfig
import SCT_GeoModelConfig	as SCT_GeoModelConfig

#arg_TrackingCuts             = 'TrackingCuts'
#arg_TrackCollectionKeys      = 'tracks'
#arg_TrackCollectionTruthKeys = 'truth'

from AthenaConfiguration.ComponentAccumulator import ComponentAccumulator
#from AthenaConfiguration.ComponentFactory import CompFactory

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
    top_acc.merge( PixelGeoModelConfig.PixelGeometryCfg(flags) )
    top_acc.merge( SCT_GeoModelConfig.SCT_GeometryCfg(flags) )


    #redoPatternRecoAndTracking = kwargs.pop('redoPatternRecoAndTracking')
    # @TODO propagate suffix and/or prefix ?
    #result = ComponentAccumulator()
    top_acc.merge(TrackReco.InDetBCM_ZeroSuppressionCfg(flags))
   
    # Pixel clusterization
    ## @TODO is this correct flag to be used here to turn on pixel clusterization
    if flags.DetFlags.pixel_on:
        top_acc.merge(TrackReco.InDetPixelClusterizationCfg(flags, **kwargs) )
        if flags.InDetFlags.doSplitReco :
            top_acc.merge(TrackReco.InDetPixelClusterizationPUCfg(flags, **kwargs) )

    # SCT clusterization
    ## @TODO is this correct flag to be used here to turn on SCT clusterization
    if flags.DetFlags.SCT_on:
        top_acc.merge( TrackReco.InDet_SCTClusterizationCfg(flags, **kwargs) )
        if flags.InDetFlags.doSplitReco :
            top_acc.merge( TrackReco.InDet_SCTClusterizationPUCfg(flags, **kwargs) )

    from PixelConditionsConfig import PixelConditionsSummaryCfg
    top_acc.merge( PixelConditionsSummaryCfg(flags))
    from SiLorentzAngleTool.SCT_LorentzAngleConfig import SCT_LorentzAngleCfg
    top_acc.popToolsAndMerge(SCT_LorentzAngleCfg(flags))
    return top_acc


