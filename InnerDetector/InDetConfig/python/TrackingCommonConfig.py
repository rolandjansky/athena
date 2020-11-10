# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
from AthenaConfiguration.ComponentAccumulator import ComponentAccumulator
from AthenaConfiguration.ComponentFactory     import CompFactory
from IOVDbSvc.IOVDbSvcConfig                  import addFoldersSplitOnline
from InDetConfig.InDetRecToolConfig           import makeName
#######################################################################

def copyArgs(kwargs, copy_list):
    dict_copy={}
    for elm in copy_list :
        if elm in kwargs :
            dict_copy[elm]=kwargs[elm]
    return dict_copy

def NeuralNetworkToHistoToolCfg(**kwargs):
    acc = ComponentAccumulator()
    name = kwargs.pop('name',"NeuralNetworkToHistoTool")

    NeuralNetworkToHistoTool=CompFactory.Trk.NeuralNetworkToHistoTool(name, **kwargs)
    acc.setPrivateTools(NeuralNetworkToHistoTool)
    return acc

def PixelClusterNnCondAlgCfg(flags, **kwargs):
    acc = ComponentAccumulator()
    track_nn = kwargs.pop('TrackNetwork',False)
    nn_names = [
          "NumberParticles_NoTrack/",
          "ImpactPoints1P_NoTrack/",
          "ImpactPoints2P_NoTrack/",
          "ImpactPoints3P_NoTrack/",
          "ImpactPointErrorsX1_NoTrack/",
          "ImpactPointErrorsX2_NoTrack/",
          "ImpactPointErrorsX3_NoTrack/",
          "ImpactPointErrorsY1_NoTrack/",
          "ImpactPointErrorsY2_NoTrack/",
          "ImpactPointErrorsY3_NoTrack/" ]

    if track_nn :
        nn_names = [ elm.replace('_NoTrack', '')  for elm in nn_names ]

    acc.merge(addFoldersSplitOnline(flags, "PIXEL", "/PIXEL/Onl/PixelClustering/PixelClusNNCalib", "/PIXEL/PixelClustering/PixelClusNNCalib", className='CondAttrListCollection'))
    kwargs.setdefault("NetworkNames", nn_names)
    kwargs.setdefault("WriteKey", 'PixelClusterNN' if not track_nn else 'PixelClusterNNWithTrack')

    if 'NetworkToHistoTool' not in kwargs :
        NeuralNetworkToHistoTool = acc.popToolsAndMerge(NeuralNetworkToHistoToolCfg(name = "NeuralNetworkToHistoTool"))
        kwargs.setdefault("NetworkToHistoTool", NeuralNetworkToHistoTool)
        acc.addPublicTool(NeuralNetworkToHistoTool)

    acc.addCondAlgo(CompFactory.InDet.TTrainedNetworkCondAlg(kwargs.pop("name", 'PixelClusterNnCondAlg'), **kwargs))
    return acc

def PixelClusterNnWithTrackCondAlgCfg(flags, **kwargs):
    kwargs.setdefault("TrackNetwork", True)
    kwargs.setdefault("name", 'PixelClusterNnWithTrackCondAlg')

    acc = PixelClusterNnCondAlgCfg(flags, **kwargs)
    return acc

def LWTNNCondAlgCfg(flags, **kwargs):
    acc = ComponentAccumulator()
    # Check for the folder
    acc.merge(addFoldersSplitOnline(flags, "PIXEL", "/PIXEL/Onl/PixelClustering/PixelNNCalibJSON", "/PIXEL/PixelClustering/PixelNNCalibJSON", className='CondAttrListCollection'))
    # What we'll store it as
    kwargs.setdefault("WriteKey", 'PixelClusterNNJSON')

    # Set up the algorithm
    acc.addCondAlgo(CompFactory.InDet.LWTNNCondAlg(kwargs.pop("name", "LWTNNCondAlg"), **kwargs))
    return acc

def NnClusterizationFactoryCfg(flags, name = 'NnClusterizationFactory', **kwargs):
    acc = ComponentAccumulator()
    the_name = makeName(name, kwargs)

    if 'PixelLorentzAngleTool' not in kwargs :
        from SiLorentzAngleTool.PixelLorentzAngleConfig import PixelLorentzAngleTool
        PixelLorentzAngleTool = PixelLorentzAngleTool(flags, name="PixelLorentzAngleTool", **kwargs)
        acc.addPublicTool(PixelLorentzAngleTool)
        kwargs.setdefault("PixelLorentzAngleTool", PixelLorentzAngleTool)

    useTTrainedNetworks = flags.InDet.useNNTTrainedNetworks
    do_runI = flags.GeoModel.Run not in ["RUN2", "RUN3"]

    if useTTrainedNetworks :
        acc.merge(PixelClusterNnCondAlgCfg(flags, name='PixelClusterNnCondAlg', GetInputsInfo = do_runI))
        acc.merge(PixelClusterNnWithTrackCondAlgCfg(flags, name = 'PixelClusterNnWithTrackCondAlg', GetInputsInfo = do_runI))
    else:
        acc.merge(addFoldersSplitOnline(flags, "PIXEL", "/PIXEL/Onl/PixelClustering/PixelClusNNCalib", "/PIXEL/PixelClustering/PixelClusNNCalib", className='CondAttrListCollection'))
        acc.merge(LWTNNCondAlgCfg(flags, name='LWTNNCondAlg'))

    kwargs.setdefault("doRunI", True)
    kwargs.setdefault("useToT", False if do_runI else flags.InDet.doNNToTCalibration)
    kwargs.setdefault("useRecenteringNNWithoutTracks", True  if do_runI else False)
    kwargs.setdefault("useRecenteringNNWithTracks", False if do_runI else False)
    kwargs.setdefault("correctLorShiftBarrelWithoutTracks", 0)
    kwargs.setdefault("correctLorShiftBarrelWithTracks", 0.030 if do_runI else 0.000)
    kwargs.setdefault("useTTrainedNetworks", useTTrainedNetworks)
    kwargs.setdefault("NnCollectionReadKey", 'PixelClusterNN')
    kwargs.setdefault("NnCollectionWithTrackReadKey", 'PixelClusterNNWithTrack')
    kwargs.setdefault("NnCollectionJSONReadKey", '' if useTTrainedNetworks else 'PixelClusterNNJSON')

    NnClusterizationFactory = CompFactory.InDet.NnClusterizationFactory( name = the_name, **kwargs )

    return NnClusterizationFactory

def InDetPixelClusterOnTrackToolBaseCfg(flags, name="PixelClusterOnTrackTool", **kwargs):
    the_name = makeName(name, kwargs)
    split_cluster_map_extension = kwargs.pop('SplitClusterMapExtension','')
    if (flags.Beam.Type == "cosmics") or flags.InDet.doDBMstandalone:
        kwargs.setdefault("ErrorStrategy", 0)
        kwargs.setdefault("PositionStrategy", 0)

    kwargs.setdefault("DisableDistortions", flags.InDet.doFatras or flags.InDet.doDBMstandalone )
    kwargs.setdefault("applyNNcorrection", flags.InDet.doPixelClusterSplitting and flags.InDet.pixelClusterSplittingType == 'NeuralNet' and not flags.InDet.doSLHC )
    kwargs.setdefault("NNIBLcorrection", flags.InDet.doPixelClusterSplitting and flags.InDet.pixelClusterSplittingType == 'NeuralNet' and not flags.InDet.doSLHC )
    kwargs.setdefault("SplitClusterAmbiguityMap", 'SplitClusterAmbiguityMap' + split_cluster_map_extension )
    kwargs.setdefault("RunningTIDE_Ambi", flags.InDet.doTIDE_Ambi )

    return CompFactory.InDet.PixelClusterOnTrackTool(the_name, **kwargs)

def InDetPixelClusterOnTrackToolDigitalCfg(flags, name='InDetPixelClusterOnTrackToolDigital', **kwargs):
    if 'LorentzAngleTool' not in kwargs :
        from SiLorentzAngleTool.PixelLorentzAngleConfig import PixelLorentzAngleTool
        kwargs.setdefault("LorentzAngleTool", PixelLorentzAngleTool(flags, name="PixelLorentzAngleTool") )

    if flags.InDet.doDigitalROTCreation:
        kwargs.setdefault("applyNNcorrection", False )
        kwargs.setdefault("NNIBLcorrection", False )
        kwargs.setdefault("ErrorStrategy", 2 )
        kwargs.setdefault("PositionStrategy", 1 )
        kwargs.setdefault("SplitClusterAmbiguityMap", "" )
    else :
        kwargs.setdefault("SplitClusterAmbiguityMap", "" )
    return InDetPixelClusterOnTrackToolBaseCfg(flags, name=name, **kwargs)

def InDetPixelClusterOnTrackToolNNSplittingCfg(flags, name='InDetPixelClusterOnTrackToolNNSplitting', **kwargs):
    if flags.InDet.doPixelClusterSplitting and flags.InDet.pixelClusterSplittingType == 'NeuralNet':
        if 'NnClusterizationFactory' not in kwargs :
            NnClusterizationFactory = NnClusterizationFactoryCfg(flags)
            kwargs.setdefault("NnClusterizationFactory", NnClusterizationFactory)

    return InDetPixelClusterOnTrackToolBaseCfg(flags, name=name, **kwargs)

def InDetPixelClusterOnTrackToolCfg(flags, name='InDetPixelClusterOnTrackTool', **kwargs):
    if 'LorentzAngleTool' not in kwargs :
        from SiLorentzAngleTool.PixelLorentzAngleConfig import PixelLorentzAngleTool
        kwargs.setdefault("LorentzAngleTool", PixelLorentzAngleTool(flags, name="PixelLorentzAngleTool") )

    if flags.InDet.doDigitalROTCreation:
        return InDetPixelClusterOnTrackToolDigitalCfg(flags, name=name, **kwargs)
    else:
        return InDetPixelClusterOnTrackToolNNSplittingCfg(flags, name=name, **kwargs)

def InDetSCT_ClusterOnTrackToolCfg(flags, name='InDetSCT_ClusterOnTrackTool', **kwargs):
    the_name = makeName(name, kwargs)

    if 'LorentzAngleTool' not in kwargs :
        from SiLorentzAngleTool.PixelLorentzAngleConfig import PixelLorentzAngleTool
        kwargs.setdefault("LorentzAngleTool", PixelLorentzAngleTool(flags, name="PixelLorentzAngleTool") )

    kwargs.setdefault("CorrectionStrategy", 0 ) # do correct position bias
    kwargs.setdefault("ErrorStrategy", 2 ) # do use phi dependent errors
    return CompFactory.InDet.SCT_ClusterOnTrackTool(the_name, **kwargs)

def InDetBroadSCT_ClusterOnTrackToolCfg(flags, name='InDetBroadSCT_ClusterOnTrackTool', **kwargs):
    kwargs.setdefault("ErrorStrategy", 0)
    return InDetSCT_ClusterOnTrackToolCfg(flags, name=name, **kwargs)

def InDetBroadPixelClusterOnTrackToolCfg(flags, name='InDetBroadPixelClusterOnTrackTool', **kwargs):
    kwargs.setdefault("ErrorStrategy", 0)
    return InDetPixelClusterOnTrackToolCfg(flags, name=name, **kwargs)

def RIO_OnTrackErrorScalingCondAlgCfg(flags, **kwargs):
    acc = ComponentAccumulator()
    the_name=kwargs.pop("name", None)

    if flags.Detector.RecoIBL:
        error_scaling_type   = ["PixelRIO_OnTrackErrorScaling"]
        error_scaling_outkey = ["/Indet/TrkErrorScalingPixel"]
    else:
        error_scaling_type   = ["PixelRIO_OnTrackErrorScalingRun1"]
        error_scaling_outkey = ["/Indet/TrkErrorScalingPixel"]

    error_scaling_type      += ["SCTRIO_OnTrackErrorScaling"]
    error_scaling_outkey    += ["/Indet/TrkErrorScalingSCT"]

    error_scaling_type      += ["TRTRIO_OnTrackErrorScaling"]
    error_scaling_outkey    += ["/Indet/TrkErrorScalingTRT"]

    acc.merge(addFoldersSplitOnline(flags, 'INDET','/Indet/Onl/TrkErrorScaling','/Indet/TrkErrorScaling', className="CondAttrListCollection"))

    kwargs.setdefault("ReadKey", "/Indet/TrkErrorScaling")
    kwargs.setdefault("ErrorScalingType", error_scaling_type)
    kwargs.setdefault("OutKeys", error_scaling_outkey)

    if the_name is not None:
        kwargs.setdefault("name", the_name)
    acc.addCondAlgo(CompFactory.RIO_OnTrackErrorScalingCondAlg(**kwargs))
    return acc

def InDetTRT_DriftCircleOnTrackToolCfg(flags, name='TRT_DriftCircleOnTrackTool', **kwargs):
    acc = ComponentAccumulator()
    the_name = makeName( name, kwargs)
    kwargs.pop('isHLT',None)
    acc.merge(RIO_OnTrackErrorScalingCondAlgCfg(flags, name = 'RIO_OnTrackErrorScalingCondAlg'))
    kwargs.setdefault("TRTErrorScalingKey", '/Indet/TrkErrorScalingTRT')
    kwargs.setdefault("LumiDataKey", '') # need to check
    acc.addPublicTool(CompFactory.InDet.TRT_DriftCircleOnTrackTool(name = the_name, **kwargs), primary = True)
    return acc

def InDetBroadTRT_DriftCircleOnTrackToolCfg(name='InDetBroadTRT_DriftCircleOnTrackTool', **kwargs):
    the_name = makeName( name, kwargs)
    return CompFactory.InDet.TRT_DriftCircleOnTrackNoDriftTimeTool(the_name, **kwargs)

def InDetRotCreatorCfg(flags, name='InDetRotCreator', **kwargs):
    acc = ComponentAccumulator()
    strip_args=['SplitClusterMapExtension','ClusterSplitProbabilityName','RenounceInputHandles','nameSuffix']
    pix_cluster_on_track_args = copyArgs(kwargs,strip_args)
    the_name = makeName(name, kwargs)

    for an_arg in  strip_args:
        kwargs.pop(an_arg, None)

    use_broad_cluster_pix = flags.InDet.useBroadPixClusterErrors and (not flags.InDet.doDBMstandalone)
    use_broad_cluster_sct = flags.InDet.useBroadSCTClusterErrors and (not flags.InDet.doDBMstandalone)

    if 'ToolPixelCluster' not in kwargs :
        if use_broad_cluster_pix :
            ToolPixelCluster= InDetBroadPixelClusterOnTrackToolCfg(flags, **pix_cluster_on_track_args) 
        else:
            ToolPixelCluster= InDetPixelClusterOnTrackToolCfg(flags, **pix_cluster_on_track_args)
        acc.addPublicTool(ToolPixelCluster)
        kwargs.setdefault("ToolPixelCluster", ToolPixelCluster)

    if 'ToolSCT_Cluster' not in kwargs :
        if use_broad_cluster_sct :
            ToolSCT_Cluster = InDetBroadSCT_ClusterOnTrackToolCfg(flags)
        else :
            ToolSCT_Cluster = InDetSCT_ClusterOnTrackToolCfg(flags)
        kwargs.setdefault("ToolSCT_Cluster", ToolSCT_Cluster)
        acc.addPublicTool(ToolSCT_Cluster)

    if 'ToolTRT_DriftCircle' not in kwargs :
        acc_trt = InDetTRT_DriftCircleOnTrackToolCfg(flags)
        TRT_DriftCircleOnTrackTool = acc_trt.getPrimary()
        acc.merge(acc_trt)
        kwargs.setdefault("ToolTRT_DriftCircle", TRT_DriftCircleOnTrackTool)

    kwargs.setdefault('Mode', 'indet')
    acc.setPrivateTools(CompFactory.Trk.RIO_OnTrackCreator(name=the_name, **kwargs))
    return acc

def InDetTRT_DriftCircleOnTrackUniversalToolCfg(name='InDetTRT_RefitRotCreator', **kwargs):
    acc = ComponentAccumulator()
    the_name = makeName( name, kwargs)
    default_ScaleHitUncertainty = 2.5

    if 'RIOonTrackToolDrift' not in kwargs :
        RIOonTrackToolDrift = InDetBroadTRT_DriftCircleOnTrackToolCfg()
        kwargs.setdefault("RIOonTrackToolDrift", RIOonTrackToolDrift)
        acc.addPublicTool(RIOonTrackToolDrift)
    if 'RIOonTrackToolTube' not in kwargs :
        RIOonTrackToolTube = InDetBroadTRT_DriftCircleOnTrackToolCfg()
        kwargs.setdefault("RIOonTrackToolTube", RIOonTrackToolTube)
        acc.addPublicTool(RIOonTrackToolTube)
    kwargs.setdefault("ScaleHitUncertainty", default_ScaleHitUncertainty) 
    acc.setPrivateTools(CompFactory.InDet.TRT_DriftCircleOnTrackUniversalTool(name = the_name, **kwargs))
    return acc

def InDetRefitRotCreatorCfg(flags, name='InDetRefitRotCreator', **kwargs):
    acc = ComponentAccumulator()
    default_ScaleHitUncertainty = 2.5
    ScaleHitUncertainty = kwargs.pop('ScaleHitUncertainty', default_ScaleHitUncertainty)
    if flags.InDet.redoTRT_LR:
        if flags.Detector.RecoTRT:
            if 'ToolTRT_DriftCircle' not in kwargs :
                ToolTRT_DriftCircle = acc.popToolsAndMerge(InDetTRT_DriftCircleOnTrackUniversalToolCfg(ScaleHitUncertainty = ScaleHitUncertainty))
                kwargs.setdefault("ToolTRT_DriftCircle", ToolTRT_DriftCircle)
                acc.addPublicTool(ToolTRT_DriftCircle)
    InDetRotCreator = acc.popToolsAndMerge(InDetRotCreatorCfg(flags, name = name, **kwargs))
    acc.setPrivateTools(InDetRotCreator)
    return acc

def InDetPRDtoTrackMapToolGangedPixelsCfg(flags, name='PRDtoTrackMapToolGangedPixels', **kwargs):
    the_name = makeName( name, kwargs)
    kwargs.setdefault("PixelClusterAmbiguitiesMapName", 'PixelClusterAmbiguitiesMap') # InDetKeys.GangedPixelMap()
    kwargs.setdefault("addTRToutliers", True)
    return CompFactory.InDet.InDetPRDtoTrackMapToolGangedPixels( name=the_name, **kwargs)

def InDetTrackPRD_AssociationCfg(flags, name='InDetTrackPRD_Association', **kwargs):
    acc = ComponentAccumulator()
    from InDetConfig.InDetRecToolConfig import makeNameGetPreAndSuffix
    the_name,prefix,suffix=makeNameGetPreAndSuffix(name,kwargs)

    if kwargs.get('TracksName', None) is None :
        raise Exception('Not TracksName argument provided')
    kwargs.setdefault("AssociationTool", InDetPRDtoTrackMapToolGangedPixelsCfg(flags) \
                                                    if 'AssociationTool' not in kwargs else None )
    kwargs.setdefault("AssociationMapName", prefix+'PRDtoTrackMap'+suffix )
    acc.addEventAlgo(CompFactory.InDet.InDetTrackPRD_Association(name = the_name, **kwargs))
    return acc

def InDetTRTDriftCircleCutForPatternRecoCfg(flags, name='InDetTRTDriftCircleCutForPatternReco', TrackingCuts=None, **kwargs):
    the_name = makeName( name, kwargs)

    kwargs.setdefault("MinOffsetDCs", 5)
    kwargs.setdefault("UseNewParameterization", TrackingCuts.useNewParameterizationTRT)
    kwargs.setdefault("UseActiveFractionSvc", flags.Detector.RecoTRT)
    return CompFactory.InDet.InDetTrtDriftCircleCutTool(the_name, **kwargs)

def InDetSummaryHelperNoHoleSearchCfg(flags, name='InDetSummaryHelperNoHoleSearch', **kwargs):
    acc = ComponentAccumulator()
    if 'HoleSearch' not in kwargs :
        kwargs.setdefault("HoleSearch", None)
    from  InDetConfig.InDetRecToolConfig import InDetTrackSummaryHelperToolCfg
    tmpAcc =  InDetTrackSummaryHelperToolCfg(flags, name = name, **kwargs)
    InDetSummaryHelper = tmpAcc.getPrimary()
    acc.merge(tmpAcc)
    acc.setPrivateTools(InDetSummaryHelper)
    return acc

def InDetTrackSummaryToolCfg(flags, name='InDetTrackSummaryTool', **kwargs):
    acc = ComponentAccumulator()
    # makeName will remove the namePrefix in suffix from kwargs, so copyArgs has to be first
    hlt_args = copyArgs(kwargs,['isHLT','namePrefix'])
    id_helper_args = copyArgs(kwargs,['ClusterSplitProbabilityName','RenounceInputHandles','namePrefix','nameSuffix']) if 'ClusterSplitProbabilityName' in kwargs else {}
    kwargs.pop('ClusterSplitProbabilityName',None)
    kwargs.pop('RenounceInputHandles',None)
    kwargs.pop('isHLT',None)
    the_name = makeName( name, kwargs)
    do_holes=kwargs.get("doHolesInDet",True)
    if do_holes :
        id_helper_args.update(hlt_args)

    if 'InDetSummaryHelperTool' not in kwargs :
        from  InDetConfig.InDetRecToolConfig import InDetTrackSummaryHelperToolCfg
        tmpAcc =  InDetTrackSummaryHelperToolCfg(flags, **id_helper_args)
        InDetSummaryHelper = tmpAcc.getPrimary()
        acc.merge(tmpAcc)
        InDetSummaryHelperNoHoleSearch = acc.popToolsAndMerge(InDetSummaryHelperNoHoleSearchCfg(flags, **id_helper_args))
        acc.addPublicTool(InDetSummaryHelperNoHoleSearch)
        kwargs.setdefault("InDetSummaryHelperTool", InDetSummaryHelper if do_holes else InDetSummaryHelperNoHoleSearch)

    #
    # Configurable version of TrkTrackSummaryTool: no TRT_PID tool needed here (no shared hits)
    #
    kwargs.setdefault("doSharedHits", False)
    kwargs.setdefault("doHolesInDet", do_holes)
    kwargs.setdefault("TRT_ElectronPidTool", None) # we don't want to use those tools during pattern
    kwargs.setdefault("TRT_ToT_dEdxTool", None) # dito
    kwargs.setdefault("PixelToTPIDTool", None) # we don't want to use those tools during pattern
    acc.setPrivateTools(CompFactory.Trk.TrackSummaryTool(name = the_name, **kwargs))
    return acc

def InDetPixelToTPIDToolCfg(name = "InDetPixelToTPIDTool", **kwargs):
    acc = ComponentAccumulator()
    the_name = makeName( name, kwargs)
    InDetPixelToTPIDTool = CompFactory.InDet.PixelToTPIDTool(name = the_name, **kwargs)
    acc.setPrivateTools(InDetPixelToTPIDTool)
    return acc

def InDetRecTestBLayerToolCfg(flags, name='InDetRecTestBLayerTool', **kwargs):
    acc = ComponentAccumulator()
    the_name = makeName( name, kwargs)

    if not flags.Detector.RecoPixel:
        return None

    if 'Extrapolator' not in kwargs :
        from  InDetConfig.InDetRecToolConfig import InDetExtrapolatorCfg
        tmpAcc =  InDetExtrapolatorCfg(flags)
        kwargs.setdefault("Extrapolator", tmpAcc.getPrimary())
        acc.merge(tmpAcc)

    if 'PixelSummaryTool' not in kwargs :
        from  InDetConfig.InDetRecToolConfig import PixelConditionsSummaryToolCfg
        PixelConditionsSummaryTool = acc.popToolsAndMerge(PixelConditionsSummaryToolCfg(flags))
        kwargs.setdefault( "PixelSummaryTool", PixelConditionsSummaryTool)
        acc.merge(tmpAcc)

    InDetTestBLayerTool = CompFactory.InDet.InDetTestBLayerTool(name=the_name, **kwargs)
    acc.setPrivateTools(InDetTestBLayerTool)
    return acc

def InDetSummaryHelperSharedHitsCfg(flags, name='InDetSummaryHelperSharedHits', **kwargs):
    acc = ComponentAccumulator()
    if 'PixelToTPIDTool' not in kwargs :
        InDetPixelToTPIDTool = acc.popToolsAndMerge(InDetPixelToTPIDToolCfg())
        acc.addPublicTool(InDetPixelToTPIDTool)
        kwargs.setdefault("PixelToTPIDTool", InDetPixelToTPIDTool)

    if 'TestBLayerTool' not in kwargs :
        if InDetRecTestBLayerToolCfg(flags) is not None:
            InDetRecTestBLayerTool = acc.popToolsAndMerge(InDetRecTestBLayerToolCfg(flags))
            acc.addPublicTool(InDetRecTestBLayerTool)
        else:
            InDetRecTestBLayerTool = None
        kwargs.setdefault("TestBLayerTool", InDetRecTestBLayerTool)

    kwargs.setdefault("DoSharedHits", flags.InDet.doSharedHits)

    if flags.Detector.RecoTRT:
        kwargs.setdefault("DoSharedHitsTRT", flags.InDet.doSharedHits)

    from  InDetConfig.InDetRecToolConfig import InDetTrackSummaryHelperToolCfg
    tmpAcc =  InDetTrackSummaryHelperToolCfg(flags, name = name, **kwargs)
    InDetSummaryHelper = tmpAcc.getPrimary()
    acc.merge(tmpAcc)
    acc.setPrivateTools(InDetSummaryHelper)
    return acc

def InDetTRTStrawStatusSummaryToolCfg(flags, name = "InDetTRT_StrawStatusSummaryTool", **kwargs):
    acc = ComponentAccumulator()
    the_name = makeName( name, kwargs)
    kwargs.setdefault("isGEANT4", flags.Input.isMC)
    InDetTRTStrawStatusSummaryTool = CompFactory.TRT_StrawStatusSummaryTool(name = the_name, **kwargs )
    acc.setPrivateTools(InDetTRTStrawStatusSummaryTool)
    return acc

def InDetTRTCalDbToolCfg(name = "InDetTRT_CalDbTool", **kwargs):
    acc = ComponentAccumulator()
    the_name = makeName( name, kwargs)
    InDetTRTCalDbTool =  CompFactory.TRT_CalDbTool(name = the_name, **kwargs)
    acc.setPrivateTools(InDetTRTCalDbTool)
    return acc

def InDetTRT_LocalOccupancyCfg(flags, name ="InDet_TRT_LocalOccupancy", **kwargs):
    acc = ComponentAccumulator()
    the_name = makeName( name, kwargs)
    if 'TRTCalDbTool' not in kwargs :
        InDetTRTCalDbTool = acc.popToolsAndMerge(InDetTRTCalDbToolCfg())
        acc.addPublicTool(InDetTRTCalDbTool)
        kwargs.setdefault( "TRTCalDbTool", InDetTRTCalDbTool )

    if 'TRTStrawStatusSummaryTool' not in kwargs :
        InDetTRTStrawStatusSummaryTool = acc.popToolsAndMerge(InDetTRTStrawStatusSummaryToolCfg(flags))
        acc.addPublicTool(InDetTRTStrawStatusSummaryTool)
        kwargs.setdefault( "TRTStrawStatusSummaryTool", InDetTRTStrawStatusSummaryTool )

    kwargs.setdefault("isTrigger", False)
    InDetTRT_LocalOccupancy = CompFactory.InDet.TRT_LocalOccupancy(name=the_name, **kwargs )
    acc.setPrivateTools(InDetTRT_LocalOccupancy)
    return acc

def InDetTRT_dEdxToolCfg(flags, name = "InDetTRT_dEdxTool", **kwargs):
    acc = ComponentAccumulator()
    the_name = makeName( name, kwargs)

    if not flags.Detector.RecoTRT or flags.InDet.doSLHC or flags.InDet.doHighPileup \
            or  flags.InDet.useExistingTracksAsInput: # TRT_RDOs (used by the TRT_LocalOccupancy tool) are not present in ESD
        return None

    kwargs.setdefault("TRT_dEdx_isData", not flags.Input.isMC)

    if 'TRT_LocalOccupancyTool' not in kwargs :
        InDetTRT_LocalOccupancy = acc.popToolsAndMerge(InDetTRT_LocalOccupancyCfg(flags))
        kwargs.setdefault( "TRT_LocalOccupancyTool", InDetTRT_LocalOccupancy)

    InDetTRT_dEdxTool = CompFactory.TRT_ToT_dEdx(name = the_name, **kwargs)
    return InDetTRT_dEdxTool

def InDetTRT_ElectronPidToolCfg(flags, name = "InDetTRT_ElectronPidTool", **kwargs):
    acc = ComponentAccumulator()
    the_name = makeName( name, kwargs)

    if not flags.Detector.RecoTRT or  flags.InDet.doSLHC or  flags.InDet.doHighPileup \
            or  flags.InDet.useExistingTracksAsInput: # TRT_RDOs (used by the TRT_LocalOccupancy tool) are not present in ESD
        return None

    if 'TRTStrawSummaryTool' not in kwargs :
        InDetTRTStrawStatusSummaryTool = acc.popToolsAndMerge(InDetTRTStrawStatusSummaryToolCfg(flags))
        acc.addPublicTool(InDetTRTStrawStatusSummaryTool)
        kwargs.setdefault( "TRTStrawSummaryTool", InDetTRTStrawStatusSummaryTool)

    if 'TRT_LocalOccupancyTool' not in kwargs :
        InDetTRT_LocalOccupancy = acc.popToolsAndMerge(InDetTRT_LocalOccupancyCfg(flags))
        acc.addPublicTool(InDetTRT_LocalOccupancy)
        kwargs.setdefault( "TRT_LocalOccupancyTool", InDetTRT_LocalOccupancy)

    if 'TRT_ToT_dEdx_Tool' not in kwargs :
        if InDetTRT_dEdxToolCfg(flags) is not None:
            InDetTRT_dEdxTool = InDetTRT_dEdxToolCfg(flags)
            acc.addPublicTool(InDetTRT_dEdxTool)
        else:
            InDetTRT_dEdxTool = None
        kwargs.setdefault( "TRT_ToT_dEdx_Tool", InDetTRT_dEdxTool)

    kwargs.setdefault( "isData", not flags.Input.isMC)

    InDetTRT_ElectronPidTool = CompFactory.InDet.TRT_ElectronPidToolRun2(name = the_name, **kwargs)
    return InDetTRT_ElectronPidTool

def InDetTrackSummaryToolSharedHitsCfg(flags, name='InDetTrackSummaryToolSharedHits',**kwargs):
    acc = ComponentAccumulator()
    if 'InDetSummaryHelperTool' not in kwargs :
        copy_args=['ClusterSplitProbabilityName','RenounceInputHandles','namePrefix','nameSuffix']
        do_holes=kwargs.get("doHolesInDet",True)
        if do_holes :
            copy_args += ['isHLT']
        id_helper_args = copyArgs(kwargs,copy_args) if 'ClusterSplitProbabilityName' in kwargs else {}
        kwargs.pop('ClusterSplitProbabilityName',None)
        kwargs.pop('RenounceInputHandles',None)

        InDetSummaryHelperSharedHits = acc.popToolsAndMerge(InDetSummaryHelperSharedHitsCfg(flags, **id_helper_args))
        acc.addPublicTool(InDetSummaryHelperSharedHits)
        kwargs.setdefault("InDetSummaryHelperTool", InDetSummaryHelperSharedHits)

    if 'TRT_ElectronPidTool' not in kwargs :
        if InDetTRT_ElectronPidToolCfg(flags) is not None:
            InDetTRT_ElectronPidTool = InDetTRT_ElectronPidToolCfg(flags)
            acc.addPublicTool(InDetTRT_ElectronPidTool)
        else:
            InDetTRT_ElectronPidTool = None
        kwargs.setdefault("TRT_ElectronPidTool", InDetTRT_ElectronPidTool)

    if 'TRT_ToT_dEdxTool' not in kwargs :
        if InDetTRT_dEdxToolCfg(flags) is not None:
            InDetTRT_dEdxTool = InDetTRT_dEdxToolCfg(flags)
            acc.addPublicTool(InDetTRT_dEdxTool)
        else:
            InDetTRT_dEdxTool = None
        kwargs.setdefault("TRT_ToT_dEdxTool", InDetTRT_dEdxTool)

    if 'PixelToTPIDTool' not in kwargs :
        InDetPixelToTPIDTool = acc.popToolsAndMerge(InDetPixelToTPIDToolCfg())
        acc.addPublicTool(InDetPixelToTPIDTool)
        kwargs.setdefault( "PixelToTPIDTool", InDetPixelToTPIDTool)

    kwargs.setdefault( "doSharedHits", flags.InDet.doSharedHits)
    kwargs.setdefault( "minTRThitsForTRTdEdx", 1)

    InDetTrackSummaryTool = acc.popToolsAndMerge(InDetTrackSummaryToolCfg(flags, name, **kwargs))
    acc.setPrivateTools(InDetTrackSummaryTool)
    return acc

def InDetUpdatorCfg(flags, name = 'InDetUpdator', **kwargs):
    the_name = makeName( name, kwargs )
    if flags.InDet.kalmanUpdator == "fast" :
        return CompFactory.Trk.KalmanUpdator_xk(name = the_name, **kwargs)
    elif flags.InDet.kalmanUpdator == "weight" :
        return CompFactory.Trk.KalmanWeightUpdator(name = the_name, **kwargs)
    elif flags.InDet.kalmanUpdator == "smatrix" :
        return CompFactory.Trk.KalmanUpdatorSMatrix(name = the_name, **kwargs)
    elif flags.InDet.kalmanUpdator == "amg" :
        return CompFactory.Trk.KalmanUpdatorAmg(name = the_name, **kwargs)
    else :
        return CompFactory.Trk.KalmanUpdator(name = the_name, **kwargs)

def InDetMultipleScatteringUpdatorCfg(name = "InDetMultipleScatteringUpdator", **kwargs):
    acc = ComponentAccumulator()
    the_name = makeName( name, kwargs)

    kwargs.setdefault( "UseTrkUtils", False)
    MultipleScatteringUpdator = CompFactory.Trk.MultipleScatteringUpdator(name = the_name, **kwargs)
    
    acc.setPrivateTools(MultipleScatteringUpdator)
    return acc

def GlobalChi2FitterCfg(flags, name ='InDetTrackFitterTRT', **kwargs):
    from InDetConfig.InDetRecToolConfig  import InDetNavigatorCfg, InDetPropagatorCfg, InDetExtrapolatorCfg
    acc = ComponentAccumulator()

    tmpAcc =  InDetExtrapolatorCfg(flags)
    InDetExtrapolator = tmpAcc.getPrimary()
    acc.merge(tmpAcc)

    InDetNavigator = acc.popToolsAndMerge(InDetNavigatorCfg(flags))
    acc.addPublicTool(InDetNavigator)

    InDetPropagator = acc.popToolsAndMerge(InDetPropagatorCfg(flags))
    acc.addPublicTool(InDetPropagator)

    InDetRefitRotCreator = acc.popToolsAndMerge(InDetRefitRotCreatorCfg(flags))
    acc.addPublicTool(InDetRefitRotCreator)

    InDetUpdator = InDetUpdatorCfg(flags)
    acc.addPublicTool(InDetUpdator)

    InDetMultipleScatteringUpdator = acc.popToolsAndMerge(InDetMultipleScatteringUpdatorCfg())
    acc.addPublicTool(InDetMultipleScatteringUpdator)

    if flags.InDet.doRobustReco or flags.Beam.Type == "cosmics":
        kwargs.setdefault("MaxOutliers", 99)

    if flags.InDet.materialInteractions and not flags.BField.solenoidOn:
        from AthenaCommon.SystemOfUnits import MeV
        kwargs.setdefault("Momentum", 1000.*MeV)

    kwargs.setdefault("ExtrapolationTool", InDetExtrapolator)
    kwargs.setdefault("NavigatorTool", InDetNavigator)
    kwargs.setdefault("PropagatorTool", InDetPropagator)
    kwargs.setdefault("RotCreatorTool", InDetRefitRotCreator)
    kwargs.setdefault("MeasurementUpdateTool", InDetUpdator)
    kwargs.setdefault("MultipleScatteringTool", InDetMultipleScatteringUpdator)
    kwargs.setdefault("StraightLine", not flags.BField.solenoidOn)
    kwargs.setdefault("ReintegrateOutliers", False)
    kwargs.setdefault("MaxIterations", 10)
    kwargs.setdefault("RecalculateDerivatives", False)
    kwargs.setdefault("TrackChi2PerNDFCut", 999999)

    acc.setPrivateTools(CompFactory.Trk.GlobalChi2Fitter(name = name, **kwargs))
    return acc

def InDetTrackSummaryToolTRTTracksCfg(flags, name='InDetTrackSummaryToolTRTTracks',**kwargs):
    kwargs.setdefault("doSharedHits", True)
    return InDetTrackSummaryToolSharedHitsCfg(flags, name = name, **kwargs)

def PublicFKF(name = 'PublicFKF', **kwargs):
    PublicFKF = CompFactory.Trk.ForwardKalmanFitter
    return PublicFKF(name = name, **kwargs)

def InDetFKF(name='InDetFKF', **kwargs):
    kwargs.setdefault("StateChi2PerNDFPreCut", 30.0)
    return PublicFKF(name=name, **kwargs)

def InDetBKS(name='InDetBKS', **kwargs):
    kwargs.setdefault("InitialCovarianceSeedFactor", 200.)
    PublicBKS = CompFactory.Trk.KalmanSmoother
    return PublicBKS(name = name, **kwargs)

def InDetKOL(name = 'InDetKOL', **kwargs):
    kwargs.setdefault("TrackChi2PerNDFCut", 17.0)
    kwargs.setdefault("StateChi2PerNDFCut", 12.5)
    PublicKOL = CompFactory.Trk.KalmanOutlierLogic
    return PublicKOL(name = name, **kwargs)

def InDetKalmanFitterCfg(flags, name ='InDetKalmanFitter', **kwargs):
    acc = ComponentAccumulator()
    from InDetConfig.InDetRecToolConfig import InDetExtrapolatorCfg

    tmpAcc =  InDetExtrapolatorCfg(flags)
    InDetExtrapolator = tmpAcc.getPrimary()
    acc.merge(tmpAcc)

    InDetUpdator = InDetUpdatorCfg(flags)
    acc.addPublicTool(InDetUpdator)

    InDetRotCreator = acc.popToolsAndMerge(InDetRotCreatorCfg(flags))
    acc.addPublicTool(InDetRotCreator)

    kwargs.setdefault("ExtrapolatorHandle", InDetExtrapolator)
    kwargs.setdefault("RIO_OnTrackCreatorHandle", InDetRotCreator)
    kwargs.setdefault("MeasurementUpdatorHandle", InDetUpdator)
    kwargs.setdefault("ForwardKalmanFitterHandle", InDetFKF())
    kwargs.setdefault("KalmanSmootherHandle", InDetBKS())
    kwargs.setdefault("KalmanOutlierLogicHandle", InDetKOL())
    kwargs.setdefault("DynamicNoiseAdjustorHandle", None)
    kwargs.setdefault("BrempointAnalyserHandle", None)
    kwargs.setdefault("AlignableSurfaceProviderHandle", None)
    kwargs.setdefault("RecalibratorHandle", None)
    kwargs.setdefault("InternalDAFHandle", None)

    acc.setPrivateTools(CompFactory.Trk.KalmanFitter(name = name, **kwargs))
    return acc
#############################################################################################
#TRTSegmentFinder
#############################################################################################
def InDetPatternPropagatorCfg(name='InDetPatternPropagator', **kwargs):
    the_name = makeName( name, kwargs)
    return CompFactory.Trk.RungeKuttaPropagator(name = the_name, **kwargs)

def InDetTRT_DriftCircleOnTrackUniversalToolCosmicsCfg(name='TRT_DriftCircleOnTrackUniversalTool', **kwargs):
    kwargs.setdefault("ScaleHitUncertainty", 2.)
    return InDetTRT_DriftCircleOnTrackUniversalToolCfg(name=name, **kwargs)

def InDetTRT_DriftCircleOnTrackNoDriftTimeToolCfg(**kwargs):
    return InDetBroadTRT_DriftCircleOnTrackToolCfg(**kwargs)

def InDetTRT_ExtensionToolCosmicsCfg(flags, name='InDetTRT_ExtensionToolCosmics', **kwargs):
    acc = ComponentAccumulator()
    the_name = makeName( name, kwargs)

    if 'Propagator' not in kwargs :
        from  InDetConfig.InDetRecToolConfig import InDetPropagatorCfg
        InDetPropagator = acc.popToolsAndMerge(InDetPropagatorCfg(flags))
        acc.addPublicTool(InDetPropagator)
        kwargs.setdefault("Propagator", InDetPropagator)

    if 'Extrapolator' not in kwargs :
        from  InDetConfig.InDetRecToolConfig import InDetExtrapolatorCfg
        tmpAcc =  InDetExtrapolatorCfg(flags)
        InDetExtrapolator = tmpAcc.getPrimary()
        acc.merge(tmpAcc)
        kwargs.setdefault("Extrapolator", InDetExtrapolator)

    if 'RIOonTrackToolYesDr' not in kwargs :
        InDetTRT_DriftCircleOnTrackUniversalToolCosmics = acc.popToolsAndMerge(InDetTRT_DriftCircleOnTrackUniversalToolCosmicsCfg())
        acc.addPublicTool(InDetTRT_DriftCircleOnTrackUniversalToolCosmics)
        kwargs.setdefault("RIOonTrackToolYesDr", InDetTRT_DriftCircleOnTrackUniversalToolCosmics)

    if 'RIOonTrackToolNoDr' not in kwargs :
        InDetBroadTRT_DriftCircleOnTrackTool = InDetTRT_DriftCircleOnTrackNoDriftTimeToolCfg()
        acc.addPublicTool(InDetBroadTRT_DriftCircleOnTrackTool)
        kwargs.setdefault("RIOonTrackToolNoDr", InDetBroadTRT_DriftCircleOnTrackTool)

    kwargs.setdefault("TRT_ClustersContainer", 'TRT_DriftCircles') # InDetKeys.TRT_DriftCircles()
    kwargs.setdefault("SearchNeighbour", False)   # needs debugging!!!
    kwargs.setdefault("RoadWidth", 10.0)
    acc.setPrivateTools(CompFactory.InDet.TRT_TrackExtensionToolCosmics(name = the_name, **kwargs))
    return acc

def InDetPatternUpdatorCfg(name='InDetPatternUpdator', **kwargs):
    the_name = makeName(name, kwargs)
    return CompFactory.Trk.KalmanUpdator_xk(name = the_name, **kwargs)

def InDetTRT_TrackExtensionTool_xkCfg(flags, name='InDetTRT_ExtensionTool', TrackingCuts=None, **kwargs):
    acc = ComponentAccumulator()
    the_name = makeName( name, kwargs)

    if 'PropagatorTool' not in kwargs :
        InDetPatternPropagator = InDetPatternPropagatorCfg()
        acc.addPublicTool(InDetPatternPropagator)
        kwargs.setdefault("PropagatorTool", InDetPatternPropagator)

    if 'UpdatorTool' not in kwargs :
        InDetPatternUpdator = InDetPatternUpdatorCfg()
        acc.addPublicTool(InDetPatternUpdator)
        kwargs.setdefault("UpdatorTool", InDetPatternUpdator)

    if 'DriftCircleCutTool' not in kwargs :
        InDetTRTDriftCircleCutForPatternReco = InDetTRTDriftCircleCutForPatternRecoCfg(flags, TrackingCuts=TrackingCuts)
        acc.addPublicTool(InDetTRTDriftCircleCutForPatternReco)
        kwargs.setdefault("DriftCircleCutTool", InDetTRTDriftCircleCutForPatternReco)

    if 'RIOonTrackToolYesDr' not in kwargs :
        acc_tmp = InDetTRT_DriftCircleOnTrackToolCfg(flags)
        InDetTRT_DriftCircleOnTrackTool = acc_tmp.getPrimary()
        acc.merge(acc_tmp)
        kwargs.setdefault("RIOonTrackToolYesDr", InDetTRT_DriftCircleOnTrackTool)

    if 'RoadTool' not in kwargs :
        InDetTRT_RoadMaker = acc.popToolsAndMerge(InDetTRT_RoadMakerCfg(flags))
        acc.addPublicTool(InDetTRT_RoadMaker)
        kwargs.setdefault("RoadTool", InDetTRT_RoadMaker)

    kwargs.setdefault("TRT_ClustersContainer", 'TRT_DriftCircles') # InDetKeys.TRT_DriftCircles()
    kwargs.setdefault("TrtManagerLocation", 'TRT') # InDetKeys.TRT_Manager()
    kwargs.setdefault("UseDriftRadius", not flags.InDet.noTRTTiming)
    kwargs.setdefault("MinNumberDriftCircles", TrackingCuts.minTRTonTrk)
    kwargs.setdefault("ScaleHitUncertainty", 2)
    kwargs.setdefault("RoadWidth", 20.)
    kwargs.setdefault("UseParameterization", TrackingCuts.useParameterizedTRTCuts)
    kwargs.setdefault("maxImpactParameter", 500 if flags.InDet.doBeamHalo or flags.InDet.doBeamGas else 50 )  # single beam running, open cuts

    if TrackingCuts.RoISeededBackTracking:
        kwargs.setdefault("minTRTSegmentpT", TrackingCuts.minSecondaryPt)

    acc.setPrivateTools(CompFactory.InDet.TRT_TrackExtensionTool_xk(the_name, **kwargs))
    return acc

def InDetWeightCalculatorCfg(name='InDetWeightCalculator', **kwargs):
    the_name = makeName( name, kwargs)
    return CompFactory.Trk.DAF_SimpleWeightCalculator(name = the_name, **kwargs)

def InDetCompetingTRT_DC_ToolCfg(flags, name='InDetCompetingTRT_DC_Tool', **kwargs):
    acc = ComponentAccumulator()
    the_name = makeName( name, kwargs)

    if 'Extrapolator' not in kwargs :
        from  InDetConfig.InDetRecToolConfig import InDetExtrapolatorCfg
        tmpAcc =  InDetExtrapolatorCfg(flags)
        InDetExtrapolator = tmpAcc.getPrimary()
        acc.merge(tmpAcc)
        kwargs.setdefault("Extrapolator", InDetExtrapolator)

    if 'ToolForWeightCalculation' not in kwargs :
        InDetWeightCalculator = InDetWeightCalculatorCfg()
        acc.addPublicTool(InDetWeightCalculator)
        kwargs.setdefault("ToolForWeightCalculation", InDetWeightCalculator)

    if 'ToolForTRT_DriftCircleOnTrackCreation' not in kwargs :
        acc_tmp = InDetTRT_DriftCircleOnTrackToolCfg(flags)
        InDetTRT_DriftCircleOnTrackTool = acc_tmp.getPrimary()
        acc.merge(acc_tmp)
        kwargs.setdefault("ToolForTRT_DriftCircleOnTrackCreation", InDetTRT_DriftCircleOnTrackTool)

    acc.setPrivateTools(CompFactory.InDet.CompetingTRT_DriftCirclesOnTrackTool( the_name, **kwargs))
    return acc

def InDetTRT_RoadMakerCfg(flags, name='InDetTRT_RoadMaker', **kwargs):
    acc = ComponentAccumulator()
    the_name = makeName( name, kwargs)
    
    InDetPatternPropagator = InDetPatternPropagatorCfg()
    acc.addPublicTool(InDetPatternPropagator)
    kwargs.setdefault("RoadWidth", 20.)
    kwargs.setdefault("PropagatorTool", InDetPatternPropagator)
    acc.setPrivateTools(CompFactory.InDet.TRT_DetElementsRoadMaker_xk(the_name, **kwargs))
    return acc

def InDetTRT_TrackExtensionTool_DAFCfg(flags, name='TRT_TrackExtensionTool_DAF',**kwargs):
    acc = ComponentAccumulator()
    the_name = makeName( name, kwargs)

    if 'CompetingDriftCircleTool' not in kwargs :
        InDetCompetingTRT_DC_Tool = acc.popToolsAndMerge(InDetCompetingTRT_DC_ToolCfg(flags))
        acc.addPublicTool(InDetCompetingTRT_DC_Tool)
        kwargs.setdefault("CompetingDriftCircleTool", InDetCompetingTRT_DC_Tool)

    if 'PropagatorTool' not in kwargs :
        InDetPatternPropagator = InDetPatternPropagatorCfg()
        acc.addPublicTool(InDetPatternPropagator)
        kwargs.setdefault("PropagatorTool", InDetPatternPropagator)

    if 'RoadTool' not in kwargs :
        InDetTRT_RoadMaker = acc.popToolsAndMerge(InDetTRT_RoadMakerCfg(flags)) 
        acc.addPublicTool(InDetTRT_RoadMaker)
        kwargs.setdefault("RoadTool", InDetTRT_RoadMaker)

    kwargs.setdefault("TRT_DriftCircleContainer", 'TRT_DriftCircles') # InDetKeys.TRT_DriftCircles()

    acc.setPrivateTools(CompFactory.InDet.TRT_TrackExtensionTool_DAF(the_name,**kwargs))
    return acc

def InDetTRT_ExtensionToolCfg(flags, TrackingCuts=None, **kwargs):
    # @TODO set all names to InDetTRT_ExtensionTool ?
    if (flags.InDet.trtExtensionType == 'xk') or (not flags.InDet.doNewTracking) :
        if (flags.Beam.Type == "cosmics"):
            return InDetTRT_ExtensionToolCosmicsCfg(flags, **kwargs)
        else:
            return InDetTRT_TrackExtensionTool_xkCfg(flags, TrackingCuts=TrackingCuts, **kwargs)
    elif flags.InDet.trtExtensionType == 'DAF' :
        return InDetTRT_TrackExtensionTool_DAFCfg(flags, name = 'InDetTRT_ExtensionTool',**kwargs)

def TRT_DetElementsRoadCondAlgCfg(**kwargs):
    acc = ComponentAccumulator()
    the_name=kwargs.pop("name","InDet__TRT_DetElementsRoadCondAlg_xk")
    acc.addCondAlgo(CompFactory.InDet.TRT_DetElementsRoadCondAlg_xk(the_name, **kwargs))
    return acc
#############################################################################################
# BackTracking
#############################################################################################
def InDetRotCreatorDigitalCfg(flags, name='InDetRotCreatorDigital', **kwargs) :
    acc = ComponentAccumulator()
    if 'ToolPixelCluster' not in kwargs :
        pix_cluster_on_track_args = copyArgs(kwargs,['SplitClusterMapExtension','ClusterSplitProbabilityName','RenounceInputHandles','nameSuffix'])

        ToolPixelCluster = InDetPixelClusterOnTrackToolDigitalCfg(flags, **pix_cluster_on_track_args)
        kwargs.setdefault('ToolPixelCluster', ToolPixelCluster)
        acc.addPublicTool(ToolPixelCluster)

    acc.setPrivateTools(acc.popToolsAndMerge(InDetRotCreatorCfg(flags, name=name, **kwargs)))
    return acc

def InDetTrackSummaryToolNoHoleSearchCfg(flags, name='InDetTrackSummaryToolNoHoleSearch',**kwargs) :
    acc = ComponentAccumulator()
    kwargs.setdefault('doHolesInDet', False)
    InDetTrackSummaryTool = acc.popToolsAndMerge(InDetTrackSummaryToolCfg(flags, name=name, **kwargs))
    acc.setPrivateTools(InDetTrackSummaryTool)
    return acc

def InDetROIInfoVecCondAlgCfg(name='InDetROIInfoVecCondAlg', **kwargs) :
    kwargs.setdefault("InputEmClusterContainerName", 'InDetCaloClusterROIs')
    kwargs.setdefault("WriteKey", kwargs.get("namePrefix","") +"ROIInfoVec"+ kwargs.get("nameSuffix","") )
    kwargs.setdefault("minPtEM", 5000.0) #in MeV
    return CompFactory.ROIInfoVecAlg(name = name,**kwargs)

def InDetAmbiScoringToolBaseCfg(flags, name='InDetAmbiScoringTool', **kwargs) :
    acc = ComponentAccumulator()
    NewTrackingCuts = kwargs.pop("NewTrackingCuts")

    from  InDetConfig.InDetRecToolConfig import InDetExtrapolatorCfg
    tmpAcc =  InDetExtrapolatorCfg(flags)
    InDetExtrapolator = tmpAcc.getPrimary()
    acc.merge(tmpAcc)
    kwargs.setdefault("Extrapolator", InDetExtrapolator)

    InDetTrackSummaryTool = acc.popToolsAndMerge(InDetTrackSummaryToolCfg(flags))
    acc.addPublicTool(InDetTrackSummaryTool)

    InDetTRTDriftCircleCutForPatternReco = InDetTRTDriftCircleCutForPatternRecoCfg(flags, TrackingCuts=NewTrackingCuts)
    acc.addPublicTool(InDetTRTDriftCircleCutForPatternReco)

    from AthenaCommon.DetFlags  import DetFlags
    have_calo_rois = flags.InDet.doBremRecovery and flags.InDet.doCaloSeededBrem and DetFlags.detdescr.Calo_allOn()
    if have_calo_rois :
        alg = InDetROIInfoVecCondAlgCfg()
        kwargs.setdefault("CaloROIInfoName", alg.WriteKey )
    kwargs.setdefault("SummaryTool", InDetTrackSummaryTool )
    kwargs.setdefault("DriftCircleCutTool", InDetTRTDriftCircleCutForPatternReco )
    kwargs.setdefault("useAmbigFcn", True )
    kwargs.setdefault("useTRT_AmbigFcn", False )
    kwargs.setdefault("maxZImp", NewTrackingCuts.maxZImpact )
    kwargs.setdefault("maxEta", NewTrackingCuts.maxEta )
    kwargs.setdefault("usePixel", NewTrackingCuts.usePixel )
    kwargs.setdefault("useSCT", NewTrackingCuts.useSCT )
    kwargs.setdefault("doEmCaloSeed", have_calo_rois )
    acc.setPrivateTools(CompFactory.InDet.InDetAmbiScoringTool(name = name, **kwargs))
    return acc

def InDetCosmicsScoringToolBaseCfg(flags, name='InDetCosmicsScoringTool', **kwargs) :
    acc = ComponentAccumulator()
    NewTrackingCuts = kwargs.pop("NewTrackingCuts")
    the_name=makeName(name, kwargs)

    InDetTrackSummaryTool = acc.popToolsAndMerge(InDetTrackSummaryToolCfg(flags))
    acc.setPrivateTools(InDetTrackSummaryTool)

    kwargs.setdefault("nWeightedClustersMin", NewTrackingCuts.nWeightedClustersMin )
    kwargs.setdefault("minTRTHits", 0 )
    kwargs.setdefault("SummaryTool", InDetTrackSummaryTool )

    acc.setPrivateTools(CompFactory.InDet.InDetCosmicScoringTool(name = the_name, **kwargs ))
    return acc

def InDetTRT_ExtensionToolPhaseCfg(flags, name='InDetTRT_ExtensionToolPhase', **kwargs) :
    acc = ComponentAccumulator()
    InDetTRT_DriftCircleOnTrackUniversalTool = acc.popToolsAndMerge(InDetTRT_DriftCircleOnTrackUniversalToolCfg())
    acc.addPublicTool(InDetTRT_DriftCircleOnTrackUniversalTool)
    if 'RIOonTrackToolYesDr' not in kwargs :
        kwargs.setdefault("RIOonTrackToolYesDr", InDetTRT_DriftCircleOnTrackUniversalTool)

    kwargs.setdefault("TRT_ClustersContainer", 'TRT_DriftCirclesUncalibrated')
    kwargs.setdefault("RoadWidth", 20.0)
    acc.setPrivateTools(acc.popToolsAndMerge(InDetTRT_ExtensionToolCosmicsCfg(flags, name = name, **kwargs)))
    return acc

def InDetCosmicExtenScoringToolCfg(flags, NewTrackingCuts, name='InDetCosmicExtenScoringTool',**kwargs) :
    acc = ComponentAccumulator()
    kwargs.setdefault("NewTrackingCuts", NewTrackingCuts)
    kwargs.setdefault("nWeightedClustersMin", 0)
    kwargs.setdefault("minTRTHits", NewTrackingCuts.minTRTonTrk )
    acc.setPrivateTools(acc.popToolsAndMerge(InDetCosmicsScoringToolBaseCfg(flags, name = 'InDetCosmicExtenScoringTool', **kwargs)))
    return acc

def SiCombinatorialTrackFinder_xkCfg(flags, name='InDetSiComTrackFinder', **kwargs):
    acc = ComponentAccumulator()

    InDetPatternPropagator = InDetPatternPropagatorCfg()
    acc.addPublicTool(InDetPatternPropagator)

    InDetPatternUpdator = InDetPatternUpdatorCfg()
    acc.addPublicTool(InDetPatternUpdator)

    InDetRotCreatorDigital = acc.popToolsAndMerge(InDetRotCreatorDigitalCfg(flags))
    acc.addPublicTool(InDetRotCreatorDigital)

    kwargs.setdefault("PropagatorTool",  InDetPatternPropagator)
    kwargs.setdefault("UpdatorTool", InDetPatternUpdator)
    kwargs.setdefault("RIOonTrackTool", InDetRotCreatorDigital)
    kwargs.setdefault("usePixel", flags.Detector.RecoPixel) #DetFlags.haveRIO.pixel_on()
    kwargs.setdefault("useSCT", flags.Detector.RecoSCT) #DetFlags.haveRIO.SCT_on()
    kwargs.setdefault("PixelClusterContainer", 'PixelClusters') #InDetKeys.PixelClusters()
    kwargs.setdefault("SCT_ClusterContainer", 'SCT_Clusters') # InDetKeys.SCT_Clusters()

    InDetSiComTrackFinder = CompFactory.InDet.SiCombinatorialTrackFinder_xk(name=name, **kwargs)
    acc.setPrivateTools(InDetSiComTrackFinder)
    return acc

def InDetCosmicScoringTool_TRTCfg(flags, NewTrackingCuts, name='InDetCosmicExtenScoringTool',**kwargs) :
    acc = ComponentAccumulator()
    InDetTrackSummaryToolNoHoleSearch = acc.popToolsAndMerge(InDetTrackSummaryToolNoHoleSearchCfg(flags))
    acc.addPublicTool(InDetTrackSummaryToolNoHoleSearch)

    kwargs.setdefault("minTRTHits", NewTrackingCuts.minSecondaryTRTonTrk)
    kwargs.setdefault("SummaryTool", InDetTrackSummaryToolNoHoleSearch)

    acc.setPrivateTools(acc.popToolsAndMerge(InDetCosmicExtenScoringToolCfg(flags,
                                                                            NewTrackingCuts=NewTrackingCuts,
                                                                            name = 'InDetCosmicScoringTool_TRT', **kwargs)))
    return acc

def InDetTRT_SeededScoringToolCfg(flags, name='InDetTRT_SeededScoringTool', NewTrackingCuts=None, **kwargs) :
    acc = ComponentAccumulator()

    kwargs.setdefault("NewTrackingCuts",  NewTrackingCuts)
    kwargs.setdefault("useAmbigFcn",  not flags.InDet.doNewTracking) # full search => use NewT
    kwargs.setdefault("useTRT_AmbigFcn",  flags.InDet.doNewTracking) # full search => use NewT
    kwargs.setdefault("minTRTonTrk",  NewTrackingCuts.minSecondaryTRTonTrk)
    kwargs.setdefault("minTRTPrecisionFraction",  NewTrackingCuts.minSecondaryTRTPrecFrac)
    kwargs.setdefault("minPt",  NewTrackingCuts.minSecondaryPt)
    kwargs.setdefault("maxRPhiImp",  NewTrackingCuts.maxSecondaryImpact)
    kwargs.setdefault("minSiClusters",  NewTrackingCuts.minSecondaryClusters)
    kwargs.setdefault("maxSiHoles",  NewTrackingCuts.maxSecondaryHoles)
    kwargs.setdefault("maxPixelHoles",  NewTrackingCuts.maxSecondaryPixelHoles)
    kwargs.setdefault("maxSCTHoles",  NewTrackingCuts.maxSecondarySCTHoles)
    kwargs.setdefault("maxDoubleHoles",  NewTrackingCuts.maxSecondaryDoubleHoles)

    acc.setPrivateTools(acc.popToolsAndMerge(InDetAmbiScoringToolBaseCfg(flags, name=name, **kwargs)))
    return acc