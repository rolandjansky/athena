# Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
from AthenaConfiguration.ComponentAccumulator import ComponentAccumulator
from AthenaConfiguration.ComponentFactory import CompFactory
from AthenaConfiguration.Enums import BeamType
from IOVDbSvc.IOVDbSvcConfig import addFoldersSplitOnline
import AthenaCommon.SystemOfUnits as Units


def ITkEtaDependentCutsSvcCfg(flags, name = 'ITkEtaDependentCutsSvc', **kwargs):
    acc = ComponentAccumulator()

    cuts = flags.ITk.Tracking.ActivePass

    kwargs.setdefault("etaBins",              cuts.etaBins)
    kwargs.setdefault("etaWidthBrem",         cuts.etaWidthBrem)
    kwargs.setdefault("maxdImpactSSSSeeds",   cuts.maxdImpactSSSSeeds)
    kwargs.setdefault("maxDoubleHoles",       cuts.maxDoubleHoles)
    kwargs.setdefault("maxHoles",             cuts.maxHoles)
    kwargs.setdefault("maxPixelHoles",        cuts.maxPixelHoles)
    kwargs.setdefault("maxPrimaryImpact",     cuts.maxPrimaryImpact)
    kwargs.setdefault("maxSctHoles",          cuts.maxSctHoles)
    kwargs.setdefault("maxShared",            cuts.maxShared)
    kwargs.setdefault("maxZImpact",           cuts.maxZImpact)
    kwargs.setdefault("minClusters",          cuts.minClusters)
    kwargs.setdefault("minPixelHits",         cuts.minPixel)
    kwargs.setdefault("minPT",                cuts.minPT)
    kwargs.setdefault("minPTBrem",            cuts.minPTBrem)
    kwargs.setdefault("minSiNotShared",       cuts.minSiNotShared)
    kwargs.setdefault("maxHolesGapPattern",   cuts.nHolesGapMax)
    kwargs.setdefault("maxHolesPattern",      cuts.nHolesMax)
    kwargs.setdefault("nWeightedClustersMin", cuts.nWeightedClustersMin)
    kwargs.setdefault("phiWidthBrem",         cuts.phiWidthBrem)
    kwargs.setdefault("Xi2max",               cuts.Xi2max)
    kwargs.setdefault("Xi2maxNoAdd",          cuts.Xi2maxNoAdd)

    ITkEtaDependentCutsSvc = CompFactory.InDet.InDetEtaDependentCutsSvc( name = name + flags.ITk.Tracking.ActivePass.extension, **kwargs )
    acc.addService(ITkEtaDependentCutsSvc)
    return acc

def ITkTruthClusterizationFactoryCfg(flags, name = 'ITkTruthClusterizationFactory', **kwargs):
    acc = ComponentAccumulator()

    kwargs.setdefault("InputSDOMap", 'ITkPixelSDO_Map')
    ITkTruthClusterizationFactory = CompFactory.InDet.TruthClusterizationFactory( name, **kwargs )
    acc.setPrivateTools(ITkTruthClusterizationFactory)
    return acc

def ITkPixelClusterOnTrackToolBaseCfg(flags, name="ITkPixelClusterOnTrackTool", **kwargs):
    acc = ComponentAccumulator()

    from PixelConditionsAlgorithms.ITkPixelConditionsConfig import ITkPixelOfflineCalibCondAlgCfg
    acc.merge(ITkPixelOfflineCalibCondAlgCfg(flags))

    if flags.Beam.Type is BeamType.Cosmics:
        kwargs.setdefault("ErrorStrategy", 0)
        kwargs.setdefault("PositionStrategy", 0)

    kwargs.setdefault("applyNNcorrection", False )
    split_cluster_map_extension = flags.ITk.Tracking.ActivePass.extension
    kwargs.setdefault("SplitClusterAmbiguityMap", f"SplitClusterAmbiguityMap{split_cluster_map_extension}")
    kwargs.setdefault("RunningTIDE_Ambi", True )

    kwargs.setdefault("PixelErrorScalingKey", "")

    acc.setPrivateTools(CompFactory.ITk.PixelClusterOnTrackTool(name, **kwargs))
    return acc

def ITkPixelClusterOnTrackToolDigitalCfg(flags, name='ITkPixelClusterOnTrackToolDigital', **kwargs):
    acc = ComponentAccumulator()

    if 'LorentzAngleTool' not in kwargs :
        from SiLorentzAngleTool.ITkPixelLorentzAngleConfig import ITkPixelLorentzAngleCfg
        ITkPixelLorentzAngleTool = acc.popToolsAndMerge(ITkPixelLorentzAngleCfg(flags))
        kwargs.setdefault("LorentzAngleTool", ITkPixelLorentzAngleTool )

    if flags.ITk.Tracking.doDigitalROTCreation:
        kwargs.setdefault("applyNNcorrection", False )
        kwargs.setdefault("NNIBLcorrection", False )
        kwargs.setdefault("ErrorStrategy", 2 )
        kwargs.setdefault("PositionStrategy", 1 )

    kwargs.setdefault("SplitClusterAmbiguityMap", "" )

    ITkPixelClusterOnTrackTool = acc.popToolsAndMerge(ITkPixelClusterOnTrackToolBaseCfg(flags, name=name, **kwargs))
    acc.setPrivateTools(ITkPixelClusterOnTrackTool)
    return acc

def ITkPixelClusterOnTrackToolNNSplittingCfg(flags, name='ITkPixelClusterOnTrackToolNNSplitting', **kwargs):
    acc = ComponentAccumulator()

    if flags.ITk.Tracking.doPixelClusterSplitting and flags.ITk.Tracking.pixelClusterSplittingType == "Truth":
        if 'NnClusterizationFactory' not in kwargs :
            ITkTruthClusterizationFactory = acc.popToolsAndMerge(ITkTruthClusterizationFactoryCfg(flags))
            kwargs.setdefault("NnClusterizationFactory", ITkTruthClusterizationFactory) #Truth-based for ITk for now

    ITkPixelClusterOnTrackTool = acc.popToolsAndMerge(ITkPixelClusterOnTrackToolBaseCfg(flags, name=name, **kwargs))
    acc.setPrivateTools(ITkPixelClusterOnTrackTool)
    return acc

def ITkPixelClusterOnTrackToolCfg(flags, name='ITkPixelClusterOnTrackTool', **kwargs):
    acc = ComponentAccumulator()

    if 'LorentzAngleTool' not in kwargs :
        from SiLorentzAngleTool.ITkPixelLorentzAngleConfig import ITkPixelLorentzAngleCfg
        ITkPixelLorentzAngleTool = acc.popToolsAndMerge(ITkPixelLorentzAngleCfg(flags))
        kwargs.setdefault("LorentzAngleTool", ITkPixelLorentzAngleTool )

    ITkPixelClusterOnTrackTool = None
    if flags.ITk.Tracking.doDigitalROTCreation:
        ITkPixelClusterOnTrackTool = acc.popToolsAndMerge(ITkPixelClusterOnTrackToolDigitalCfg(flags, name, **kwargs))
    else:
        ITkPixelClusterOnTrackTool = acc.popToolsAndMerge(ITkPixelClusterOnTrackToolNNSplittingCfg(flags, name, **kwargs))

    acc.setPrivateTools(ITkPixelClusterOnTrackTool)
    return acc

def ITkStripClusterOnTrackToolCfg(flags, name='ITkStrip_ClusterOnTrackTool', **kwargs):
    acc = ComponentAccumulator()

    if 'LorentzAngleTool' not in kwargs :
        from SiLorentzAngleTool.ITkStripLorentzAngleConfig import ITkStripLorentzAngleCfg
        ITkStripLorentzAngleTool = acc.popToolsAndMerge( ITkStripLorentzAngleCfg(flags) )
        kwargs.setdefault("LorentzAngleTool", ITkStripLorentzAngleTool )

    kwargs.setdefault("CorrectionStrategy", 0 ) # do correct position bias
    kwargs.setdefault("ErrorStrategy", 0 ) # use width / sqrt(12)

    kwargs.setdefault("ErrorScalingKey", "")

    acc.setPrivateTools(CompFactory.ITk.StripClusterOnTrackTool(name, **kwargs))
    return acc

def ITkBroadStripClusterOnTrackToolCfg(flags, name='ITkBroadStripClusterOnTrackTool', **kwargs):
    kwargs.setdefault("ErrorStrategy", 0)
    return ITkStripClusterOnTrackToolCfg(flags, name=name, **kwargs)

def ITkBroadPixelClusterOnTrackToolCfg(flags, name='ITkBroadPixelClusterOnTrackTool', **kwargs):
    kwargs.setdefault("ErrorStrategy", 0)
    return ITkPixelClusterOnTrackToolCfg(flags, name, **kwargs)

def ITk_RIO_OnTrackErrorScalingCondAlgCfg(flags, name='ITk_RIO_OnTrackErrorScalingCondAlg', **kwargs):
    acc = ComponentAccumulator()

    # TODO: confirm for Run 4
    error_scaling_type   = ["PixelRIO_OnTrackErrorScaling"]
    error_scaling_outkey = ["/Indet/TrkErrorScalingPixel"]

    error_scaling_type      += ["SCTRIO_OnTrackErrorScaling"]
    error_scaling_outkey    += ["/Indet/TrkErrorScalingSCT"]

    acc.merge(addFoldersSplitOnline(flags, 'INDET','/Indet/Onl/TrkErrorScaling','/Indet/TrkErrorScaling', className="CondAttrListCollection"))

    kwargs.setdefault("ReadKey", "/Indet/TrkErrorScaling")
    kwargs.setdefault("ErrorScalingType", error_scaling_type)
    kwargs.setdefault("OutKeys", error_scaling_outkey)

    acc.addCondAlgo(CompFactory.RIO_OnTrackErrorScalingCondAlg(name, **kwargs))
    return acc

def ITkRotCreatorCfg(flags, name='ITkRotCreator', **kwargs):
    acc = ComponentAccumulator()

    if 'ToolPixelCluster' not in kwargs :
        ToolPixelCluster = acc.popToolsAndMerge(ITkPixelClusterOnTrackToolCfg(flags))
        kwargs.setdefault("ToolPixelCluster", ToolPixelCluster)

    if 'ToolSCT_Cluster' not in kwargs :
        ToolStripCluster = acc.popToolsAndMerge(ITkStripClusterOnTrackToolCfg(flags))
        kwargs.setdefault("ToolSCT_Cluster", ToolStripCluster)

    kwargs.setdefault("ToolTRT_DriftCircle", None)
    kwargs.setdefault('Mode', 'indet')

    acc.setPrivateTools(CompFactory.Trk.RIO_OnTrackCreator(name, **kwargs))
    return acc

def ITkPRDtoTrackMapToolGangedPixelsCfg(flags, name='ITkPRDtoTrackMapToolGangedPixels', **kwargs):
    acc = ComponentAccumulator()
    kwargs.setdefault("PixelClusterAmbiguitiesMapName", 'ITkPixelClusterAmbiguitiesMap')
    kwargs.setdefault("addTRToutliers", False)
    acc.setPrivateTools(CompFactory.InDet.InDetPRDtoTrackMapToolGangedPixels( name, **kwargs))
    return acc

def ITkTrackPRD_AssociationCfg(flags, name='ITkTrackPRD_Association', **kwargs):
    acc = ComponentAccumulator()

    if kwargs.get('TracksName', None) is None :
        raise Exception('Not TracksName argument provided')

    if 'AssociationTool' not in kwargs:
        kwargs.setdefault("AssociationTool", acc.popToolsAndMerge(ITkPRDtoTrackMapToolGangedPixelsCfg(flags)))

    kwargs.setdefault("AssociationMapName", 'ITkPRDtoTrackMap' + flags.ITk.Tracking.ActivePass.extension)
    acc.addEventAlgo(CompFactory.InDet.InDetTrackPRD_Association(name, **kwargs))
    return acc

def ITkSummaryHelperNoHoleSearchCfg(flags, name='ITkSummaryHelperNoHoleSearch', **kwargs):
    if 'HoleSearch' not in kwargs :
        kwargs.setdefault("HoleSearch", None)
    from  InDetConfig.ITkRecToolConfig import ITkTrackSummaryHelperToolCfg
    return ITkTrackSummaryHelperToolCfg(flags, name = name, **kwargs)

def ITkTrackSummaryToolCfg(flags, name='ITkTrackSummaryTool', **kwargs):
    acc = ComponentAccumulator()
    do_holes=kwargs.get("doHolesInDet",True)

    if 'InDetSummaryHelperTool' not in kwargs :
        if do_holes:
            from  InDetConfig.ITkRecToolConfig import ITkTrackSummaryHelperToolCfg
            ITkSummaryHelperTool = acc.popToolsAndMerge(ITkTrackSummaryHelperToolCfg(flags))
        else:
            ITkSummaryHelperTool = acc.popToolsAndMerge(ITkSummaryHelperNoHoleSearchCfg(flags))
        kwargs.setdefault("InDetSummaryHelperTool", ITkSummaryHelperTool)

    #
    # Configurable version of TrkTrackSummaryTool: no TRT_PID tool needed here (no shared hits)
    #
    kwargs.setdefault("doSharedHits", False)
    kwargs.setdefault("doHolesInDet", do_holes)
    kwargs.setdefault("TRT_ElectronPidTool", None) # we don't want to use those tools during pattern
    kwargs.setdefault("PixelToTPIDTool", None) # we don't want to use those tools during pattern
    acc.addPublicTool(CompFactory.Trk.TrackSummaryTool(name, **kwargs), primary=True)
    return acc

def ITkTrackSummaryToolAmbiCfg(flags, name='ITkTrackSummaryToolAmbi', **kwargs):
    acc = ComponentAccumulator()

    if 'InDetSummaryHelperTool' not in kwargs :
        from InDetConfig.ITkRecToolConfig import ITkTrackSummaryHelperToolCfg
        ITkSummaryHelperTool = acc.popToolsAndMerge(ITkTrackSummaryHelperToolCfg(flags,
                                                                                 ClusterSplitProbabilityName = "ITkAmbiguityProcessorSplitProb" + flags.ITk.Tracking.ActivePass.extension))
        kwargs.setdefault("InDetSummaryHelperTool", ITkSummaryHelperTool)

    ITkTrackSummaryTool = acc.getPrimaryAndMerge(ITkTrackSummaryToolCfg(flags, name, **kwargs))
    acc.addPublicTool(ITkTrackSummaryTool, primary=True)
    return acc

def ITkSummaryHelperSharedHitsCfg(flags, name='ITkSummaryHelperSharedHits', **kwargs):
    kwargs.setdefault("PixelToTPIDTool", None)
    kwargs.setdefault("TestBLayerTool", None)
    kwargs.setdefault("DoSharedHits", flags.ITk.Tracking.doSharedHits)

    from  InDetConfig.ITkRecToolConfig import ITkTrackSummaryHelperToolCfg
    return ITkTrackSummaryHelperToolCfg(flags, name = name, **kwargs)

def ITkTrackSummaryToolSharedHitsCfg(flags, name='ITkTrackSummaryToolSharedHits',**kwargs):
    acc = ComponentAccumulator()
    if 'InDetSummaryHelperTool' not in kwargs :
        ITkSummaryHelperSharedHits = acc.popToolsAndMerge(ITkSummaryHelperSharedHitsCfg(flags))
        kwargs.setdefault("InDetSummaryHelperTool", ITkSummaryHelperSharedHits)

    kwargs.setdefault( "PixelToTPIDTool", None)
    kwargs.setdefault( "doSharedHits", flags.ITk.Tracking.doSharedHits)

    ITkTrackSummaryTool = acc.getPrimaryAndMerge(ITkTrackSummaryToolCfg(flags, name, **kwargs))
    acc.addPublicTool(ITkTrackSummaryTool, primary=True)
    return acc

def ITkMultipleScatteringUpdatorCfg(flags, name = "ITkMultipleScatteringUpdator", **kwargs):
    acc = ComponentAccumulator()

    kwargs.setdefault( "UseTrkUtils", False)
    MultipleScatteringUpdator = CompFactory.Trk.MultipleScatteringUpdator(name, **kwargs)
    acc.setPrivateTools(MultipleScatteringUpdator)
    return acc

def ITkMeasRecalibSTCfg(flags, name='ITkMeasRecalibST', **kwargs) :
    acc = ComponentAccumulator()

    if 'BroadPixelClusterOnTrackTool' not in kwargs :
        ITkBroadPixelClusterOnTrackTool = acc.popToolsAndMerge(ITkBroadPixelClusterOnTrackToolCfg(flags))
        kwargs.setdefault('BroadPixelClusterOnTrackTool', ITkBroadPixelClusterOnTrackTool)
    if 'BroadSCT_ClusterOnTrackTool' not in kwargs :
        ITkBroadStripClusterOnTrackTool = acc.popToolsAndMerge(ITkBroadStripClusterOnTrackToolCfg(flags))
        kwargs.setdefault('BroadSCT_ClusterOnTrackTool', ITkBroadStripClusterOnTrackTool)
    if 'CommonRotCreator' not in kwargs :
        ITkRefitRotCreator = acc.popToolsAndMerge(ITkRotCreatorCfg(flags, 
                                                                   name = 'ITkRefitRotCreator'))
        kwargs.setdefault('CommonRotCreator', ITkRefitRotCreator)

    MeasRecalibSteeringTool = CompFactory.Trk.MeasRecalibSteeringTool(name, **kwargs)
    acc.setPrivateTools(MeasRecalibSteeringTool)
    return acc

def ITkKalmanTrackFitterBaseCfg(flags, name='ITkKalmanTrackFitterBase', **kwargs) :
    acc = ComponentAccumulator()

    from TrkConfig.AtlasExtrapolatorConfig import AtlasExtrapolatorCfg
    extrapolator = acc.popToolsAndMerge(AtlasExtrapolatorCfg(flags))
    acc.addPublicTool(extrapolator)  # TODO: migrate to private?
    kwargs.setdefault("ExtrapolatorHandle", extrapolator)

    if 'RIO_OnTrackCreatorHandle' not in kwargs :
        ITkRefitRotCreator = acc.popToolsAndMerge(ITkRotCreatorCfg(flags, 
                                                                   name = 'ITkRefitRotCreator'))
        kwargs.setdefault("RIO_OnTrackCreatorHandle", ITkRefitRotCreator)

    from  InDetConfig.ITkRecToolConfig import ITkUpdatorCfg
    ITkUpdator = acc.popToolsAndMerge(ITkUpdatorCfg(flags))
    ITkBKS = acc.popToolsAndMerge(ITkBKSCfg(flags))
    ITkKOL = acc.popToolsAndMerge(ITkKOLCfg(flags))

    kwargs.setdefault('MeasurementUpdatorHandle', ITkUpdator)
    kwargs.setdefault('KalmanSmootherHandle', ITkBKS)
    kwargs.setdefault('KalmanOutlierLogicHandle', ITkKOL)
    kwargs.setdefault('DynamicNoiseAdjustorHandle', None)
    kwargs.setdefault('BrempointAnalyserHandle', None)
    kwargs.setdefault('AlignableSurfaceProviderHandle',None)

    if 'RecalibratorHandle' not in kwargs :
        ITkMeasRecalibST = acc.popToolsAndMerge(ITkMeasRecalibSTCfg(flags))
        kwargs.setdefault('RecalibratorHandle', ITkMeasRecalibST)

    KalmanFitter = CompFactory.Trk.KalmanFitter(name=name, **kwargs)
    acc.setPrivateTools(KalmanFitter)
    return acc

def ITkDNAdjustorCfg(flags, name='ITkDNAdjustor', **kwargs) :
    acc = ComponentAccumulator()
    acc.setPrivateTools(CompFactory.Trk.InDetDynamicNoiseAdjustment(name=name,**kwargs))
    return acc

def ITkDNASeparatorCfg(flags, name='ITkDNASeparator',**kwargs) :
    acc = ComponentAccumulator()
    acc.setPrivateTools(CompFactory.InDet.InDetDNASeparator(name=name,**kwargs))
    return acc

def ITkAnnealBKSCfg(flags, name='ITkAnnealBKS',**kwargs) :
    acc = ComponentAccumulator()
    kwargs.setdefault('InitialCovarianceSeedFactor',200.)
    acc.setPrivateTools(CompFactory.Trk.KalmanSmoother(name=name,**kwargs))
    return acc

def ITkKalmanCompetingPixelClustersToolCfg(flags, name='ITkKalmanCompetingPixelClustersTool',**kwargs):
    acc = ComponentAccumulator()
    kwargs.setdefault('WeightCutValueBarrel',5.5)
    kwargs.setdefault('WeightCutValueEndCap',5.5)

    if 'Extrapolator' not in kwargs:
        from TrkConfig.AtlasExtrapolatorConfig import AtlasExtrapolatorCfg
        extrapolator = acc.popToolsAndMerge(AtlasExtrapolatorCfg(flags))
        acc.addPublicTool(extrapolator)  # TODO: migrate to private?
        kwargs.setdefault("Extrapolator", extrapolator)

    acc.setPrivateTools(CompFactory.InDet.CompetingPixelClustersOnTrackTool(name=name, **kwargs))
    return acc

def ITkKalmanCompetingStripClustersToolCfg(flags, name='ITkKalmanCompetingStripClustersTool',**kwargs) :
    acc = ComponentAccumulator()
    kwargs.setdefault('WeightCutValueBarrel',5.5)
    kwargs.setdefault('WeightCutValueEndCap',5.5)

    if 'Extrapolator' not in kwargs:
        from TrkConfig.AtlasExtrapolatorConfig import AtlasExtrapolatorCfg
        extrapolator = acc.popToolsAndMerge(AtlasExtrapolatorCfg(flags))
        acc.addPublicTool(extrapolator)  # TODO: migrate to private?
        kwargs.setdefault("Extrapolator", extrapolator)

    acc.setPrivateTools(CompFactory.InDet.CompetingSCT_ClustersOnTrackTool(name=name,**kwargs))
    return acc

def ITkKalmanCompetingRIOsToolCfg(flags, name='ITkKalmanCompetingRIOsTool',**kwargs) :
    acc = ComponentAccumulator()

    ITkKalmanCompetingPixelClustersTool = acc.popToolsAndMerge(ITkKalmanCompetingPixelClustersToolCfg(flags))
    ITkKalmanCompetingStripClustersTool = acc.popToolsAndMerge(ITkKalmanCompetingStripClustersToolCfg(flags))    
    kwargs.setdefault('ToolForCompPixelClusters', ITkKalmanCompetingPixelClustersTool)
    kwargs.setdefault('ToolForCompSCT_Clusters', ITkKalmanCompetingStripClustersTool)
    acc.setPrivateTools(CompFactory.Trk.CompetingRIOsOnTrackTool(name=name,**kwargs))
    return acc

def ITkKalmanInternalDAFCfg(flags, name='ITkKalmanInternalDAF',**kwargs) :
    acc = ComponentAccumulator()

    ITkKalmanCompetingRIOsTool = acc.popToolsAndMerge(ITkKalmanCompetingRIOsToolCfg(flags))
    kwargs.setdefault('CompetingRIOsOnTrackCreator',ITkKalmanCompetingRIOsTool)

    if 'BackwardSmoother' not in kwargs:
        ITkAnnealBKS = acc.popToolsAndMerge(ITkAnnealBKSCfg(flags))
        kwargs.setdefault('BackwardSmoother',ITkAnnealBKS)

    acc.setPrivateTools(CompFactory.Trk.KalmanPiecewiseAnnealingFilter(name=name,**kwargs))
    return acc

def ITkAnnealFKFCfg(flags, name='ITkAnnealFKF',**kwargs) :
    acc = ComponentAccumulator()
    acc.setPrivateTools(CompFactory.Trk.ForwardKalmanFitter(name=name,**kwargs))
    return acc

def ITkFKFRefCfg(flags, name='ITkFKFRef',**kwargs) :
    kwargs.setdefault('StateChi2PerNDFPreCut',30.0)
    return ITkAnnealFKFCfg(name=name,**kwargs)

def ITkKalmanInternalDAFRefCfg(flags, name='ITkKalmanInternalDAFRef',**kwargs) :
    acc = ComponentAccumulator()
    
    ITkAnnealFKF = acc.popToolsAndMerge(ITkAnnealFKFCfg(flags))
    ITkBKS = acc.popToolsAndMerge(ITkBKSCfg(flags))
    kwargs.setdefault('ForwardFitter', ITkAnnealFKF)
    kwargs.setdefault('BackwardSmoother', ITkBKS)

    ITkKalmanInternalDAF = acc.popToolsAndMerge(ITkKalmanInternalDAFCfg(flags, name=name, **kwargs))
    acc.setPrivateTools(ITkKalmanInternalDAF)
    return acc

def ITkBroadRotCreatorCfg(flags, name='ITkBroadRotCreator', **kwargs) :
    acc = ComponentAccumulator()

    if 'ToolPixelCluster' not in kwargs :
        ITkBroadPixelClusterOnTrackTool = acc.popToolsAndMerge(ITkBroadPixelClusterOnTrackToolCfg(flags))
        kwargs.setdefault('ToolPixelCluster', ITkBroadPixelClusterOnTrackTool)

    if 'ToolSCT_Cluster' not in kwargs :
        ITkBroadStripClusterOnTrackTool = acc.popToolsAndMerge(ITkBroadStripClusterOnTrackToolCfg(flags))
        kwargs.setdefault('ToolSCT_Cluster', ITkBroadStripClusterOnTrackTool)

    ITkRotCreator = acc.popToolsAndMerge(ITkRotCreatorCfg(flags, name = name, **kwargs))
    acc.setPrivateTools(ITkRotCreator)
    return acc

def ITkKalmanFitterCfg(flags, name='ITkKalmanFitter', **kwargs) :
    acc = ComponentAccumulator()

    ITkFKF = acc.popToolsAndMerge(ITkFKFCfg(flags))
    kwargs.setdefault('ForwardKalmanFitterHandle', ITkFKF)

    if flags.ITk.Tracking.doBremRecovery:
        
        ITkDNAdjustor = acc.popToolsAndMerge(ITkDNAdjustorCfg(flags))
        ITkDNASeparator = acc.popToolsAndMerge(ITkDNASeparatorCfg(flags))

        kwargs.setdefault('DynamicNoiseAdjustorHandle', ITkDNAdjustor)
        kwargs.setdefault('BrempointAnalyserHandle', ITkDNASeparator)
        kwargs.setdefault('DoDNAForElectronsOnly', True)
    else:
        kwargs.setdefault('DynamicNoiseAdjustorHandle', None)
        kwargs.setdefault('BrempointAnalyserHandle', None)

    ITkKalmanInternalDAF = acc.popToolsAndMerge(ITkKalmanInternalDAFCfg(flags))
    kwargs.setdefault('InternalDAFHandle', ITkKalmanInternalDAF)

    ITkKalmanTrackFitter = acc.popToolsAndMerge(ITkKalmanTrackFitterBaseCfg(flags, name, **kwargs))
    acc.setPrivateTools(ITkKalmanTrackFitter)
    return acc

def ITkKalmanDNAFitterCfg(flags, name='ITkKalmanDNAFitter', **kwargs) :
    acc = ComponentAccumulator()

    ITkFKF = acc.popToolsAndMerge(ITkFKFCfg(flags))
    ITkDNAdjustor = acc.popToolsAndMerge(ITkDNAdjustorCfg(flags))
    ITkDNASeparator = acc.popToolsAndMerge(ITkDNASeparatorCfg(flags))
    ITkKalmanInternalDAF = acc.popToolsAndMerge(ITkKalmanInternalDAFCfg(flags))

    kwargs.setdefault('ForwardKalmanFitterHandle', ITkFKF)
    kwargs.setdefault('DynamicNoiseAdjustorHandle', ITkDNAdjustor)
    kwargs.setdefault('BrempointAnalyserHandle', ITkDNASeparator)
    kwargs.setdefault('InternalDAFHandle', ITkKalmanInternalDAF)

    ITkKalmanTrackFitter = acc.popToolsAndMerge(ITkKalmanTrackFitterBaseCfg(flags, name, **kwargs))
    acc.setPrivateTools(ITkKalmanTrackFitter)
    return acc

def ITkReferenceKalmanFitterCfg(flags, name='ITkReferenceKalmanFitter', **kwargs) :
    acc = ComponentAccumulator()

    ITkFKFRef = acc.popToolsAndMerge(ITkFKFRefCfg(flags))
    ITkKalmanInternalDAFRef = acc.popToolsAndMerge(ITkKalmanInternalDAFRefCfg(flags))

    kwargs.setdefault('ForwardKalmanFitterHandle', ITkFKFRef)
    kwargs.setdefault('InternalDAFHandle', ITkKalmanInternalDAFRef)

    ITkKalmanTrackFitter = acc.popToolsAndMerge(ITkKalmanTrackFitterBaseCfg(flags, name, **kwargs))
    acc.setPrivateTools(ITkKalmanTrackFitter)
    return acc

def ITkDistributedKalmanFilterCfg(flags, name="ITkDistributedKalmanFilter", **kwargs) :
    acc = ComponentAccumulator()

    if 'ExtrapolatorTool' not in kwargs:
        from TrkConfig.AtlasExtrapolatorConfig import AtlasExtrapolatorCfg
        kwargs.setdefault('ExtrapolatorTool', acc.popToolsAndMerge(AtlasExtrapolatorCfg(flags)))

    if 'ROTcreator' not in kwargs:
        ITkRotCreator = acc.popToolsAndMerge(ITkRotCreatorCfg(flags))
        kwargs.setdefault('ROTcreator', ITkRotCreator)

    DistributedKalmanFilter = CompFactory.Trk.DistributedKalmanFilter(name = name, **kwargs)
    acc.setPrivateTools(DistributedKalmanFilter)
    return acc

def ITkGlobalChi2FitterCfg(flags, name='ITkGlobalChi2Fitter', **kwargs) :
    acc = ComponentAccumulator()

    if 'RotCreatorTool' not in kwargs :
        ITkRotCreator = acc.popToolsAndMerge(ITkRotCreatorCfg(flags))
        kwargs.setdefault('RotCreatorTool', ITkRotCreator)

    if 'BroadRotCreatorTool' not in kwargs:
        ITkBroadRotCreator = acc.popToolsAndMerge(ITkBroadRotCreatorCfg(flags))
        kwargs.setdefault('BroadRotCreatorTool', ITkBroadRotCreator)

    if flags.Beam.Type is BeamType.Cosmics:
        kwargs.setdefault('MaxOutliers', 99)
        kwargs.setdefault('Acceleration', False)

    if flags.ITk.Tracking.materialInteractions and not flags.BField.solenoidOn:
        kwargs.setdefault('Momentum', 1000.*Units.MeV)

    ITkGlobalChi2Fitter = acc.popToolsAndMerge(ITkGlobalChi2FitterBaseCfg(flags, name=name, **kwargs))
    acc.setPrivateTools(ITkGlobalChi2Fitter)
    return acc

def ITkGaussianSumFitterCfg(flags, name="ITkGaussianSumFitter", **kwargs):
    acc = ComponentAccumulator()

    if "ToolForROTCreation" not in kwargs:
        ITkRotCreator = acc.popToolsAndMerge(ITkRotCreatorCfg(flags))
        kwargs.setdefault("ToolForROTCreation", ITkRotCreator )

    kwargs.setdefault("MakePerigee", True)
    kwargs.setdefault("RefitOnMeasurementBase", True)
    kwargs.setdefault("DoHitSorting", True)

    from egammaTrackTools.GSFTrackFitterConfig import EMGSFTrackFitterCfg

    GaussianSumFitter = acc.popToolsAndMerge(
        EMGSFTrackFitterCfg(flags, name=name, **kwargs)
    )

    acc.setPrivateTools(GaussianSumFitter)
    return acc

def ITkTrackFitterCfg(flags, name='ITkTrackFitter', **kwargs) :
    return {
            'KalmanFitter'            : ITkKalmanFitterCfg,
            'KalmanDNAFitter'         : ITkKalmanDNAFitterCfg,
            'ReferenceKalmanFitter'   : ITkReferenceKalmanFitterCfg,
            'DistributedKalmanFilter' : ITkDistributedKalmanFilterCfg,
            'GlobalChi2Fitter'        : ITkGlobalChi2FitterCfg,
            'GaussianSumFilter'       : ITkGaussianSumFitterCfg
    }[flags.ITk.Tracking.trackFitterType](flags, name, **kwargs)

def ITkTrackFitterAmbiCfg(flags, name='ITkTrackFitterAmbi', **kwargs) :
    acc = ComponentAccumulator()

    ITkTrackFitter = acc.popToolsAndMerge(ITkTrackFitterCfg(flags, name, **kwargs))
    ClusterSplitProbabilityName = "ITkAmbiguityProcessorSplitProb" + flags.ITk.Tracking.ActivePass.extension

    if flags.ITk.Tracking.trackFitterType in ['KalmanFitter', 'KalmanDNAFitter', 'ReferenceKalmanFitter', 'DistributedKalmanFilter']:
        ITkTrackFitter.RecalibratorHandle.BroadPixelClusterOnTrackTool.ClusterSplitProbabilityName = ClusterSplitProbabilityName

    elif flags.ITk.Tracking.trackFitterType=='GlobalChi2Fitter':
        ITkTrackFitter.RotCreatorTool.ToolPixelCluster.ClusterSplitProbabilityName = ClusterSplitProbabilityName
        ITkTrackFitter.BroadRotCreatorTool.ToolPixelCluster.ClusterSplitProbabilityName = ClusterSplitProbabilityName

    elif flags.InDet.Tracking.trackFitterType=='GaussianSumFilter':
        ITkTrackFitter.ToolForROTCreation.ToolPixelCluster.ClusterSplitProbabilityName = ClusterSplitProbabilityName

    acc.setPrivateTools(ITkTrackFitter)
    return acc

def ITkGlobalChi2FitterBaseCfg(flags, name='ITkGlobalChi2FitterBase', **kwargs) :
    acc = ComponentAccumulator()

    if 'TrackingGeometryReadKey' not in kwargs :
        from TrackingGeometryCondAlg.AtlasTrackingGeometryCondAlgConfig import (
                TrackingGeometryCondAlgCfg)
        geom_cond = TrackingGeometryCondAlgCfg(flags)
        geom_cond_key = geom_cond.getPrimary().TrackingGeometryWriteKey
        acc.merge(acc)
        kwargs.setdefault("TrackingGeometryReadKey", geom_cond_key)

    from TrkConfig.AtlasExtrapolatorConfig import AtlasExtrapolatorCfg
    from TrkConfig.AtlasExtrapolatorToolsConfig import AtlasNavigatorCfg, AtlasEnergyLossUpdatorCfg, ITkPropagatorCfg, ITkMaterialEffectsUpdatorCfg
    from InDetConfig.ITkRecToolConfig import ITkUpdatorCfg

    Extrapolator = acc.popToolsAndMerge(AtlasExtrapolatorCfg(flags))
    Navigator = acc.popToolsAndMerge(AtlasNavigatorCfg(flags))
    ELossUpdator = acc.popToolsAndMerge(AtlasEnergyLossUpdatorCfg(flags))
    ITkPropagator = acc.popToolsAndMerge(ITkPropagatorCfg(flags))
    ITkUpdator = acc.popToolsAndMerge(ITkUpdatorCfg(flags))
    ITkMultipleScatteringUpdator = acc.popToolsAndMerge(ITkMultipleScatteringUpdatorCfg(flags))
    ITkMaterialEffectsUpdator = acc.popToolsAndMerge(ITkMaterialEffectsUpdatorCfg(flags))

    kwargs.setdefault("ExtrapolationTool", Extrapolator)
    kwargs.setdefault("NavigatorTool", Navigator)
    kwargs.setdefault("PropagatorTool", ITkPropagator)
    kwargs.setdefault("MultipleScatteringTool", ITkMultipleScatteringUpdator)
    kwargs.setdefault("EnergyLossTool", ELossUpdator)
    kwargs.setdefault("MeasurementUpdateTool", ITkUpdator)
    kwargs.setdefault("MaterialUpdateTool", ITkMaterialEffectsUpdator)
    kwargs.setdefault("StraightLine", not flags.BField.solenoidOn)
    kwargs.setdefault("OutlierCut", 4)
    kwargs.setdefault("SignedDriftRadius", True)
    kwargs.setdefault("ReintegrateOutliers", True)
    kwargs.setdefault("RecalibrateSilicon", True)
    kwargs.setdefault("MaxIterations", 40)
    kwargs.setdefault("Acceleration", True)
    kwargs.setdefault("RecalculateDerivatives", flags.Beam.Type is BeamType.Cosmics)
    kwargs.setdefault("TrackChi2PerNDFCut", 7)

    GlobalChi2Fitter = CompFactory.Trk.GlobalChi2Fitter(name=name, **kwargs)
    acc.setPrivateTools(GlobalChi2Fitter)
    return acc

def ITkFKFCfg(flags, name='ITkFKF', **kwargs):
    acc = ComponentAccumulator()
    kwargs.setdefault("StateChi2PerNDFPreCut", 30.0)
    PublicFKF = CompFactory.Trk.ForwardKalmanFitter(name = name, **kwargs)
    acc.setPrivateTools(PublicFKF)
    return acc

def ITkBKSCfg(flags, name='ITkBKS', **kwargs):
    acc = ComponentAccumulator()
    kwargs.setdefault("InitialCovarianceSeedFactor", 200.)
    PublicBKS = CompFactory.Trk.KalmanSmoother(name = name, **kwargs)
    acc.setPrivateTools(PublicBKS)
    return acc

def ITkKOLCfg(flags, name = 'ITkKOL', **kwargs):
    acc = ComponentAccumulator()
    kwargs.setdefault("TrackChi2PerNDFCut", 17.0)
    kwargs.setdefault("StateChi2PerNDFCut", 12.5)
    PublicKOL = CompFactory.Trk.KalmanOutlierLogic(name = name, **kwargs)
    acc.setPrivateTools(PublicKOL)
    return acc

#############################################################################################
# BackTracking
#############################################################################################
def ITkRotCreatorDigitalCfg(flags, name='ITkRotCreatorDigital', **kwargs) :
    acc = ComponentAccumulator()
    if 'ToolPixelCluster' not in kwargs :
        ToolPixelCluster = acc.popToolsAndMerge(ITkPixelClusterOnTrackToolDigitalCfg(flags))
        kwargs.setdefault('ToolPixelCluster', ToolPixelCluster)

    ITkRotCreator = acc.popToolsAndMerge(ITkRotCreatorCfg(flags, name=name, **kwargs))
    acc.addPublicTool(ITkRotCreator, primary=True)
    return acc

def ITkTrackSummaryToolNoHoleSearchCfg(flags, name='ITkTrackSummaryToolNoHoleSearch',**kwargs) :
    kwargs.setdefault('doHolesInDet', False)
    return ITkTrackSummaryToolCfg(flags, name=name, **kwargs)

def ITkROIInfoVecCondAlgCfg(flags, name='ITkROIInfoVecCondAlg', **kwargs) :
    acc = ComponentAccumulator()
    from InDetConfig.ITkRecCaloSeededROISelectionConfig import ITkCaloClusterROI_SelectorCfg
    acc.merge(ITkCaloClusterROI_SelectorCfg(flags))
    kwargs.setdefault("InputEmClusterContainerName", "ITkCaloClusterROIs")
    kwargs.setdefault("WriteKey", kwargs.get("namePrefix","") +"ROIInfoVec"+ kwargs.get("nameSuffix","") )
    kwargs.setdefault("minPtEM", 5000.0) #in MeV
    acc.addEventAlgo(CompFactory.ROIInfoVecAlg(name = name,**kwargs), primary=True)
    return acc

def ITkAmbiScoringToolBaseCfg(flags, name='ITkAmbiScoringTool', **kwargs) :
    acc = ComponentAccumulator()

    from TrkConfig.AtlasExtrapolatorConfig import AtlasExtrapolatorCfg
    kwargs.setdefault("Extrapolator", acc.popToolsAndMerge(AtlasExtrapolatorCfg(flags)))

    ITkTrackSummaryTool = acc.getPrimaryAndMerge(ITkTrackSummaryToolCfg(flags))

    have_calo_rois = flags.ITk.Tracking.doBremRecovery and flags.ITk.Tracking.doCaloSeededBrem and flags.Detector.EnableCalo
    if have_calo_rois:
        alg = acc.getPrimaryAndMerge(ITkROIInfoVecCondAlgCfg(flags))
        kwargs.setdefault("CaloROIInfoName", alg.WriteKey )
    kwargs.setdefault("SummaryTool", ITkTrackSummaryTool )
    kwargs.setdefault("DriftCircleCutTool", None )
    kwargs.setdefault("useAmbigFcn", True )
    kwargs.setdefault("useTRT_AmbigFcn", False )
    kwargs.setdefault("maxEta", flags.ITk.Tracking.ActivePass.maxEta )
    kwargs.setdefault("usePixel", flags.ITk.Tracking.ActivePass.useITkPixel )
    kwargs.setdefault("useSCT", flags.ITk.Tracking.ActivePass.useITkStrip )
    kwargs.setdefault("doEmCaloSeed", have_calo_rois )
    kwargs.setdefault("useITkAmbigFcn", True )
    kwargs.setdefault("minTRTonTrk", 0 )
    kwargs.setdefault("minTRTPrecisionFraction", 0 )

    if 'InDetEtaDependentCutsSvc' not in kwargs :
        acc.merge(ITkEtaDependentCutsSvcCfg(flags))
        kwargs.setdefault("InDetEtaDependentCutsSvc", acc.getService("ITkEtaDependentCutsSvc"+flags.ITk.Tracking.ActivePass.extension))

    the_name = name + flags.ITk.Tracking.ActivePass.extension
    acc.setPrivateTools(CompFactory.InDet.InDetAmbiScoringTool(name = the_name, **kwargs))
    return acc

def ITkCosmicsScoringToolBaseCfg(flags, name='ITkCosmicsScoringTool', **kwargs) :
    acc = ComponentAccumulator()

    ITkTrackSummaryTool = acc.getPrimaryAndMerge(ITkTrackSummaryToolCfg(flags))

    kwargs.setdefault("nWeightedClustersMin", flags.ITk.Tracking.ActivePass.nWeightedClustersMin )
    kwargs.setdefault("minTRTHits", 0 )
    kwargs.setdefault("SummaryTool", ITkTrackSummaryTool )

    acc.setPrivateTools( CompFactory.InDet.InDetCosmicScoringTool(name, **kwargs ) )
    return acc

def ITkCosmicExtenScoringToolCfg(flags, name='ITkCosmicExtenScoringTool',**kwargs) :
    kwargs.setdefault("nWeightedClustersMin", 0)
    kwargs.setdefault("minTRTHits", 0 )
    return ITkCosmicsScoringToolBaseCfg(flags, name = 'ITkCosmicExtenScoringTool', **kwargs)

def ITkAmbiScoringToolCfg(flags, name='ITkAmbiScoringTool', **kwargs) :
    return ITkAmbiScoringToolBaseCfg(flags, name = name + flags.ITk.Tracking.ActivePass.extension, **kwargs)


#############################################################################################
#TrackingSiPatternConfig
#############################################################################################

def ITkPRDtoTrackMapToolCfg(flags, name='ITkPRDtoTrackMapTool',**kwargs) :
    acc = ComponentAccumulator()
    acc.setPrivateTools(CompFactory.Trk.PRDtoTrackMapTool(name, **kwargs))
    return acc

def ITkCosmicsScoringToolCfg(flags, name='ITkCosmicsScoringTool', **kwargs) :
    return ITkCosmicsScoringToolBaseCfg(flags,
                                        name=name+flags.ITk.Tracking.ActivePass.extension)
