# Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
from AthenaConfiguration.ComponentAccumulator import ComponentAccumulator
from AthenaConfiguration.ComponentFactory import CompFactory
from IOVDbSvc.IOVDbSvcConfig import addFolders, addFoldersSplitOnline
from InDetConfig.InDetRecToolConfig import makeName
import AthenaCommon.SystemOfUnits as Units
#######################################################################

def copyArgs(kwargs, copy_list):
    dict_copy={}
    for elm in copy_list :
        if elm in kwargs :
            dict_copy[elm]=kwargs[elm]
    return dict_copy

def stripArgs(kwargs, copy_list) :
    dict_copy = copyArgs(kwargs,copy_list)
    for an_arg in copy_list :
        kwargs.pop(an_arg,None)
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
    # TODO: not in global tag
    # acc.merge(addFoldersSplitOnline(flags, "PIXEL", "/PIXEL/Onl/PixelClustering/PixelNNCalibJSON", "/PIXEL/PixelClustering/PixelNNCalibJSON", className="CondAttrListCollection"))
    if flags.Input.isMC:
        acc.merge(addFolders(flags, "/PIXEL/PixelClustering/PixelNNCalibJSON", "PIXEL_OFL", className="CondAttrListCollection", tag="PixelNNCalibJSON-SIM-RUN2-000-02", db="OFLP200"))
    else:
        acc.merge(addFolders(flags, "/PIXEL/Onl/PixelClustering/PixelNNCalibJSON", "PIXEL", className="CondAttrListCollection", tag="PixelNNCalibJSON-DATA-RUN2-000-02", db="CONDBR2"))
    # What we'll store it as
    kwargs.setdefault("WriteKey", 'PixelClusterNNJSON')

    # Set up the algorithm
    acc.addCondAlgo(CompFactory.InDet.LWTNNCondAlg(kwargs.pop("name", "LWTNNCondAlg"), **kwargs))
    return acc


def NnClusterizationFactoryCfg(flags, name="NnClusterizationFactory", **kwargs):
    acc = ComponentAccumulator()
    the_name = makeName(name, kwargs)

    if "PixelLorentzAngleTool" not in kwargs:
        from SiLorentzAngleTool.PixelLorentzAngleConfig import PixelLorentzAngleTool
        PixelLorentzAngleTool = PixelLorentzAngleTool(flags, name="PixelLorentzAngleTool", **kwargs)
        kwargs.setdefault("PixelLorentzAngleTool", PixelLorentzAngleTool)

    do_runI = flags.GeoModel.Run == "RUN1"

    if flags.InDet.useNNTTrainedNetworks:
        acc.merge(PixelClusterNnCondAlgCfg(flags, name="PixelClusterNnCondAlg", GetInputsInfo=do_runI))
        acc.merge(PixelClusterNnWithTrackCondAlgCfg(flags, name="PixelClusterNnWithTrackCondAlg", GetInputsInfo=do_runI))
    else:
        acc.merge(LWTNNCondAlgCfg(flags, name="LWTNNCondAlg"))

    kwargs.setdefault("doRunI", do_runI)
    kwargs.setdefault("useToT", False if do_runI else flags.InDet.doNNToTCalibration)
    kwargs.setdefault("useRecenteringNNWithoutTracks", True  if do_runI else False)
    kwargs.setdefault("useRecenteringNNWithTracks", False if do_runI else False)
    kwargs.setdefault("correctLorShiftBarrelWithoutTracks", 0)
    kwargs.setdefault("correctLorShiftBarrelWithTracks", 0.030 if do_runI else 0.000)
    kwargs.setdefault("useTTrainedNetworks", flags.InDet.useNNTTrainedNetworks)
    kwargs.setdefault("NnCollectionReadKey", "PixelClusterNN" if flags.InDet.useNNTTrainedNetworks else "")
    kwargs.setdefault("NnCollectionWithTrackReadKey", "PixelClusterNNWithTrack" if flags.InDet.useNNTTrainedNetworks else "")
    kwargs.setdefault("NnCollectionJSONReadKey", "" if flags.InDet.useNNTTrainedNetworks else "PixelClusterNNJSON")

    acc.setPrivateTools(CompFactory.InDet.NnClusterizationFactory(the_name, **kwargs))
    return acc


def InDetPixelClusterOnTrackToolBaseCfg(flags, name="PixelClusterOnTrackTool", **kwargs):
    acc = ComponentAccumulator()
    the_name = makeName(name, kwargs)

    from PixelConditionsAlgorithms.PixelConditionsConfig import PixelDistortionAlgCfg, PixelOfflineCalibCondAlgCfg
    acc.merge(PixelOfflineCalibCondAlgCfg(flags))
    if not (flags.InDet.doFatras or flags.InDet.doDBMstandalone):
        acc.merge(PixelDistortionAlgCfg(flags))

    split_cluster_map_extension = kwargs.pop('SplitClusterMapExtension','')
    if flags.Beam.Type == "cosmics" or flags.InDet.doDBMstandalone:
        kwargs.setdefault("ErrorStrategy", 0)
        kwargs.setdefault("PositionStrategy", 0)

    kwargs.setdefault("DisableDistortions", flags.InDet.doFatras or flags.InDet.doDBMstandalone)
    kwargs.setdefault("applyNNcorrection", flags.InDet.doPixelClusterSplitting and flags.InDet.pixelClusterSplittingType == "NeuralNet")
    kwargs.setdefault("NNIBLcorrection", flags.InDet.doPixelClusterSplitting and flags.InDet.pixelClusterSplittingType == "NeuralNet")
    kwargs.setdefault("SplitClusterAmbiguityMap", f"SplitClusterAmbiguityMap{split_cluster_map_extension}")
    kwargs.setdefault("RunningTIDE_Ambi", flags.InDet.doTIDE_Ambi)

    acc.setPrivateTools(CompFactory.InDet.PixelClusterOnTrackTool(the_name, **kwargs))
    return acc


def InDetPixelClusterOnTrackToolDigitalCfg(flags, name="InDetPixelClusterOnTrackToolDigital", **kwargs):
    acc = ComponentAccumulator()

    kwargs.setdefault("SplitClusterAmbiguityMap", "")

    if "LorentzAngleTool" not in kwargs:
        from SiLorentzAngleTool.PixelLorentzAngleConfig import PixelLorentzAngleCfg
        PixelLorentzAngleTool = acc.popToolsAndMerge(PixelLorentzAngleCfg(flags))
        kwargs.setdefault("LorentzAngleTool", PixelLorentzAngleTool)

    if flags.InDet.doDigitalROTCreation:
        kwargs.setdefault("applyNNcorrection", False)
        kwargs.setdefault("NNIBLcorrection", False)
        kwargs.setdefault("ErrorStrategy", 2)
        kwargs.setdefault("PositionStrategy", 1)
    else :
        kwargs.setdefault("SplitClusterAmbiguityMap", "")

    acc.setPrivateTools(acc.popToolsAndMerge(InDetPixelClusterOnTrackToolBaseCfg(flags, name, **kwargs)))
    return acc


def InDetPixelClusterOnTrackToolNNSplittingCfg(flags, name="InDetPixelClusterOnTrackToolNNSplitting", **kwargs):
    acc = ComponentAccumulator()

    if flags.InDet.doPixelClusterSplitting and flags.InDet.pixelClusterSplittingType == "NeuralNet":
        if "NnClusterizationFactory" not in kwargs:
            kwargs.setdefault("NnClusterizationFactory", acc.popToolsAndMerge(NnClusterizationFactoryCfg(flags)))

    acc.setPrivateTools(acc.popToolsAndMerge(InDetPixelClusterOnTrackToolBaseCfg(flags, name, **kwargs)))
    return acc


def InDetPixelClusterOnTrackToolCfg(flags, name="InDetPixelClusterOnTrackTool", **kwargs):
    acc = ComponentAccumulator()

    if "LorentzAngleTool" not in kwargs:
        from SiLorentzAngleTool.PixelLorentzAngleConfig import PixelLorentzAngleCfg
        kwargs.setdefault("LorentzAngleTool", acc.popToolsAndMerge(PixelLorentzAngleCfg(flags)))

    if flags.InDet.doDigitalROTCreation:
        PixelClusterOnTrackTool = acc.popToolsAndMerge(InDetPixelClusterOnTrackToolDigitalCfg(flags, name, **kwargs))
    else:
        PixelClusterOnTrackTool = acc.popToolsAndMerge(InDetPixelClusterOnTrackToolNNSplittingCfg(flags, name, **kwargs))

    acc.setPrivateTools(PixelClusterOnTrackTool)
    return acc


def InDetSCT_ClusterOnTrackToolCfg(flags, name='InDetSCT_ClusterOnTrackTool', **kwargs):
    acc = ComponentAccumulator()
    the_name = makeName(name, kwargs)

    if 'LorentzAngleTool' not in kwargs :
        from SiLorentzAngleTool.SCT_LorentzAngleConfig import SCT_LorentzAngleCfg
        SCT_LorentzAngle = acc.popToolsAndMerge(SCT_LorentzAngleCfg(flags))
        kwargs.setdefault("LorentzAngleTool", SCT_LorentzAngle )
        
    kwargs.setdefault("CorrectionStrategy", 0 ) # do correct position bias
    kwargs.setdefault("ErrorStrategy", 2 ) # do use phi dependent errors

    acc.setPrivateTools(CompFactory.InDet.SCT_ClusterOnTrackTool(the_name, **kwargs))
    return acc

def InDetBroadSCT_ClusterOnTrackToolCfg(flags, name='InDetBroadSCT_ClusterOnTrackTool', **kwargs):
    kwargs.setdefault("ErrorStrategy", 0)
    return InDetSCT_ClusterOnTrackToolCfg(flags, name=name, **kwargs)

def InDetBroadPixelClusterOnTrackToolCfg(flags, name='InDetBroadPixelClusterOnTrackTool', **kwargs):
    kwargs.setdefault("ErrorStrategy", 0)
    return InDetPixelClusterOnTrackToolCfg(flags, name=name, **kwargs)

def RIO_OnTrackErrorScalingCondAlgCfg(flags, **kwargs):
    acc = ComponentAccumulator()
    the_name=kwargs.pop("name", None)

    if flags.GeoModel.Run == "RUN1":
        error_scaling_type   = ["PixelRIO_OnTrackErrorScalingRun1"]
        error_scaling_outkey = ["/Indet/TrkErrorScalingPixel"]
    else:  # Run 2 and 3
        error_scaling_type   = ["PixelRIO_OnTrackErrorScaling"]
        error_scaling_outkey = ["/Indet/TrkErrorScalingPixel"]
    # TODO: cover Run 4 and beyond

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

def LumiCondDataKeyForTRTMuScalingCfg(flags, **kwargs) :
    acc = ComponentAccumulator()
    LuminosityOutputKey = ''
    if not flags.Beam.Type == 'cosmics' and False: # InDetFlags.useMuForTRTErrorScaling()  "temporary value"
        from LumiBlockComps.LuminosityCondAlgConfig import LuminosityCondAlgCfg
        LuminosityCondAlg = LuminosityCondAlgCfg (flags)
        acc.merge(LuminosityCondAlg)
        LuminosityOutputKey = LuminosityCondAlg.getCondAlgo ('LuminosityCondAlg').LuminosityOutputKey

    return acc, LuminosityOutputKey

def InDetTRT_DriftCircleOnTrackToolCfg(flags, name='TRT_DriftCircleOnTrackTool', **kwargs):
    acc = ComponentAccumulator()
    the_name = makeName( name, kwargs)
    kwargs.pop('isHLT',None)
    acc.merge(RIO_OnTrackErrorScalingCondAlgCfg(flags, name = 'RIO_OnTrackErrorScalingCondAlg'))
    kwargs.setdefault("TRTErrorScalingKey", '/Indet/TrkErrorScalingTRT')
    tmpAcc, LuminosityOutputKey = LumiCondDataKeyForTRTMuScalingCfg(flags)
    acc.merge(tmpAcc)
    kwargs.setdefault("LumiDataKey", LuminosityOutputKey)
    acc.setPrivateTools(CompFactory.InDet.TRT_DriftCircleOnTrackTool(name = the_name, **kwargs))
    return acc

def InDetBroadTRT_DriftCircleOnTrackToolCfg(name='InDetBroadTRT_DriftCircleOnTrackTool', **kwargs):
    acc = ComponentAccumulator()
    the_name = makeName( name, kwargs)
    acc.setPrivateTools(CompFactory.InDet.TRT_DriftCircleOnTrackNoDriftTimeTool(the_name, **kwargs))
    return acc

def InDetRotCreatorCfg(flags, name='InDetRotCreator', **kwargs):
    if flags.Detector.GeometryITk:
        name = name.replace("InDet", "ITk")
        from InDetConfig.ITkTrackingCommonConfig import ITkRotCreatorCfg
        return ITkRotCreatorCfg(flags, name, **kwargs)

    acc = ComponentAccumulator()
    strip_args=['SplitClusterMapExtension','ClusterSplitProbabilityName','nameSuffix']
    pix_cluster_on_track_args = copyArgs(kwargs,strip_args)
    the_name = makeName(name, kwargs)

    for an_arg in  strip_args:
        kwargs.pop(an_arg, None)

    use_broad_cluster_pix = flags.InDet.useBroadPixClusterErrors and (not flags.InDet.doDBMstandalone)
    use_broad_cluster_sct = flags.InDet.useBroadSCTClusterErrors and (not flags.InDet.doDBMstandalone)

    if 'ToolPixelCluster' not in kwargs :
        if use_broad_cluster_pix :
            ToolPixelCluster= acc.popToolsAndMerge(InDetBroadPixelClusterOnTrackToolCfg(flags, **pix_cluster_on_track_args))
        else:
            ToolPixelCluster= acc.popToolsAndMerge(InDetPixelClusterOnTrackToolCfg(flags, **pix_cluster_on_track_args))
        kwargs.setdefault("ToolPixelCluster", ToolPixelCluster)

    if 'ToolSCT_Cluster' not in kwargs :
        if use_broad_cluster_sct :
            ToolSCT_Cluster = acc.popToolsAndMerge(InDetBroadSCT_ClusterOnTrackToolCfg(flags))
        else :
            ToolSCT_Cluster = acc.popToolsAndMerge(InDetSCT_ClusterOnTrackToolCfg(flags))
        kwargs.setdefault("ToolSCT_Cluster", ToolSCT_Cluster)

    if 'ToolTRT_DriftCircle' not in kwargs :
        kwargs.setdefault("ToolTRT_DriftCircle", acc.popToolsAndMerge(InDetTRT_DriftCircleOnTrackToolCfg(flags)))

    kwargs.setdefault('Mode', 'indet')
    acc.setPrivateTools(CompFactory.Trk.RIO_OnTrackCreator(name=the_name, **kwargs))
    return acc

def InDetTRT_DriftCircleOnTrackUniversalToolCfg(name='InDetTRT_RefitRotCreator', **kwargs):
    acc = ComponentAccumulator()
    the_name = makeName( name, kwargs)
    default_ScaleHitUncertainty = 2.5

    if 'RIOonTrackToolDrift' not in kwargs :
        RIOonTrackToolDrift = acc.popToolsAndMerge(InDetBroadTRT_DriftCircleOnTrackToolCfg())
        kwargs.setdefault("RIOonTrackToolDrift", RIOonTrackToolDrift)
    if 'RIOonTrackToolTube' not in kwargs :
        RIOonTrackToolTube = acc.popToolsAndMerge(InDetBroadTRT_DriftCircleOnTrackToolCfg())
        kwargs.setdefault("RIOonTrackToolTube", RIOonTrackToolTube)
    kwargs.setdefault("ScaleHitUncertainty", default_ScaleHitUncertainty) 
    acc.setPrivateTools(CompFactory.InDet.TRT_DriftCircleOnTrackUniversalTool(name = the_name, **kwargs))
    return acc

def InDetRefitRotCreatorCfg(flags, name='InDetRefitRotCreator', **kwargs):
    acc = ComponentAccumulator()
    default_ScaleHitUncertainty = 2.5
    ScaleHitUncertainty = kwargs.pop('ScaleHitUncertainty', default_ScaleHitUncertainty)
    if flags.Detector.EnableTRT and flags.InDet.redoTRT_LR:
        if 'ToolTRT_DriftCircle' not in kwargs :
            ToolTRT_DriftCircle = acc.popToolsAndMerge(InDetTRT_DriftCircleOnTrackUniversalToolCfg(ScaleHitUncertainty = ScaleHitUncertainty))
            kwargs.setdefault("ToolTRT_DriftCircle", ToolTRT_DriftCircle)
    InDetRotCreator = acc.popToolsAndMerge(InDetRotCreatorCfg(flags, name = name, **kwargs))
    acc.setPrivateTools(InDetRotCreator)
    return acc

def InDetPRDtoTrackMapToolGangedPixelsCfg(flags, name='PRDtoTrackMapToolGangedPixels', **kwargs):
    acc = ComponentAccumulator()
    the_name = makeName( name, kwargs)
    kwargs.setdefault("PixelClusterAmbiguitiesMapName", 'PixelClusterAmbiguitiesMap') # InDetKeys.GangedPixelMap()
    kwargs.setdefault("addTRToutliers", True)
    acc.setPrivateTools(CompFactory.InDet.InDetPRDtoTrackMapToolGangedPixels( name=the_name, **kwargs))
    return acc

def InDetTrackPRD_AssociationCfg(flags, name='InDetTrackPRD_Association', **kwargs):
    acc = ComponentAccumulator()
    from InDetConfig.InDetRecToolConfig import makeNameGetPreAndSuffix
    the_name,prefix,suffix=makeNameGetPreAndSuffix(name,kwargs)

    if kwargs.get('TracksName', None) is None :
        raise Exception('Not TracksName argument provided')

    AssociationTool = acc.popToolsAndMerge(InDetPRDtoTrackMapToolGangedPixelsCfg(flags))
    kwargs.setdefault("AssociationTool", AssociationTool \
                      if 'AssociationTool' not in kwargs else None )
    kwargs.setdefault("AssociationMapName", prefix+'PRDtoTrackMap'+suffix )
    acc.addEventAlgo(CompFactory.InDet.InDetTrackPRD_Association(name = the_name, **kwargs))
    return acc

def InDetTRTDriftCircleCutForPatternRecoCfg(flags, name='InDetTRTDriftCircleCutForPatternReco', **kwargs):
    the_name = makeName( name, kwargs)
    result = ComponentAccumulator()
    kwargs.setdefault("MinOffsetDCs", 5)
    kwargs.setdefault("UseNewParameterization", flags.InDet.Tracking.Pass.useNewParameterizationTRT)
    kwargs.setdefault("UseActiveFractionSvc", flags.Detector.EnableTRT)
    result.setPrivateTools(CompFactory.InDet.InDetTrtDriftCircleCutTool(the_name, **kwargs))
    return result

def InDetSummaryHelperNoHoleSearchCfg(flags, name='InDetSummaryHelperNoHoleSearch', **kwargs):
    if 'HoleSearch' not in kwargs :
        kwargs.setdefault("HoleSearch", None)
    from  InDetConfig.InDetRecToolConfig import InDetTrackSummaryHelperToolCfg
    return InDetTrackSummaryHelperToolCfg(flags, name = name, **kwargs)

def InDetTrackSummaryToolCfg(flags, name='InDetTrackSummaryTool', **kwargs):
    if flags.Detector.GeometryITk:
        name = name.replace("InDet", "ITk")
        from InDetConfig.ITkTrackingCommonConfig import ITkTrackSummaryToolCfg
        return ITkTrackSummaryToolCfg(flags, name, **kwargs)

    acc = ComponentAccumulator()
    # makeName will remove the namePrefix in suffix from kwargs, so copyArgs has to be first
    hlt_args = copyArgs(kwargs,['isHLT','namePrefix'])
    id_helper_args = copyArgs(kwargs,['ClusterSplitProbabilityName','namePrefix','nameSuffix']) if 'ClusterSplitProbabilityName' in kwargs else {}
    kwargs.pop('ClusterSplitProbabilityName',None)
    kwargs.pop('isHLT',None)
    the_name = makeName( name, kwargs)
    do_holes=kwargs.get("doHolesInDet",True)
    if do_holes :
        id_helper_args.update(hlt_args)

    if 'InDetSummaryHelperTool' not in kwargs :
        if do_holes:
            from  InDetConfig.InDetRecToolConfig import InDetTrackSummaryHelperToolCfg
            InDetSummaryHelperTool = acc.popToolsAndMerge(InDetTrackSummaryHelperToolCfg(flags, **id_helper_args))
        else:
            InDetSummaryHelperTool = acc.popToolsAndMerge(InDetSummaryHelperNoHoleSearchCfg(flags, **id_helper_args))
        kwargs.setdefault("InDetSummaryHelperTool", InDetSummaryHelperTool)

    #
    # Configurable version of TrkTrackSummaryTool: no TRT_PID tool needed here (no shared hits)
    #
    kwargs.setdefault("doSharedHits", False)
    kwargs.setdefault("doHolesInDet", do_holes)
    kwargs.setdefault("TRT_ElectronPidTool", None) # we don't want to use those tools during pattern
    kwargs.setdefault("PixelToTPIDTool", None) # we don't want to use those tools during pattern
    acc.addPublicTool(CompFactory.Trk.TrackSummaryTool(name = the_name, **kwargs), primary=True)
    return acc

def PixeldEdxAlg(flags, name = "PixeldEdxAlg", **kwargs):
    acc = ComponentAccumulator()
    acc.merge(addFoldersSplitOnline(flags, "PIXEL", "/PIXEL/PixdEdx", "/PIXEL/PixdEdx", className='AthenaAttributeList'))
    PixeldEdxAlg = CompFactory.PixeldEdxAlg(name=name, ReadFromCOOL = True, **kwargs)
    acc.addCondAlgo(PixeldEdxAlg)
    return acc

def InDetPixelToTPIDToolCfg(flags, name = "InDetPixelToTPIDTool", **kwargs):
    acc = ComponentAccumulator()

    acc.merge(PixeldEdxAlg(flags))

    the_name = makeName( name, kwargs)
    InDetPixelToTPIDTool = CompFactory.InDet.PixelToTPIDTool(name = the_name, **kwargs)
    acc.setPrivateTools(InDetPixelToTPIDTool)
    return acc

def InDetRecTestBLayerToolCfg(flags, name='InDetRecTestBLayerTool', **kwargs):
    acc = ComponentAccumulator()
    the_name = makeName( name, kwargs)

    if not flags.Detector.EnablePixel:
        return None

    if 'Extrapolator' not in kwargs :
        from TrkConfig.AtlasExtrapolatorConfig import InDetExtrapolatorCfg
        kwargs.setdefault("Extrapolator", acc.getPrimaryAndMerge(InDetExtrapolatorCfg(flags)))

    if 'PixelSummaryTool' not in kwargs :
        from PixelConditionsTools.PixelConditionsSummaryConfig import PixelConditionsSummaryCfg
        PixelConditionsSummaryTool = acc.popToolsAndMerge(PixelConditionsSummaryCfg(flags))
        kwargs.setdefault( "PixelSummaryTool", PixelConditionsSummaryTool)

    InDetTestBLayerTool = CompFactory.InDet.InDetTestBLayerTool(name=the_name, **kwargs)
    acc.setPrivateTools(InDetTestBLayerTool)
    return acc

def InDetSummaryHelperSharedHitsCfg(flags, name='InDetSummaryHelperSharedHits', **kwargs):
    acc = ComponentAccumulator()
    if 'PixelToTPIDTool' not in kwargs :
        InDetPixelToTPIDTool = acc.popToolsAndMerge(InDetPixelToTPIDToolCfg(flags))
        kwargs.setdefault("PixelToTPIDTool", InDetPixelToTPIDTool)

    if 'TestBLayerTool' not in kwargs :
        testBLayerToolAcc = InDetRecTestBLayerToolCfg(flags)
        if testBLayerToolAcc is not None:
            InDetRecTestBLayerTool = acc.popToolsAndMerge(testBLayerToolAcc)
        else:
            InDetRecTestBLayerTool = None
        kwargs.setdefault("TestBLayerTool", InDetRecTestBLayerTool)

    kwargs.setdefault("DoSharedHits", flags.InDet.doSharedHits)

    if flags.Detector.EnableTRT:
        kwargs.setdefault("DoSharedHitsTRT", flags.InDet.doSharedHits)

    from  InDetConfig.InDetRecToolConfig import InDetTrackSummaryHelperToolCfg    
    InDetSummaryHelper = acc.popToolsAndMerge(InDetTrackSummaryHelperToolCfg(flags, name = name, **kwargs))

    acc.setPrivateTools(InDetSummaryHelper)
    return acc


def InDetTrackSummaryToolSharedHitsCfg(flags, name='InDetTrackSummaryToolSharedHits',**kwargs):
    acc = ComponentAccumulator()
    if 'InDetSummaryHelperTool' not in kwargs :
        copy_args=['ClusterSplitProbabilityName','namePrefix','nameSuffix']
        do_holes=kwargs.get("doHolesInDet",True)
        if do_holes :
            copy_args += ['isHLT']
        id_helper_args = copyArgs(kwargs,copy_args) if 'ClusterSplitProbabilityName' in kwargs else {}
        kwargs.pop('ClusterSplitProbabilityName',None)

        InDetSummaryHelperSharedHits = acc.popToolsAndMerge(InDetSummaryHelperSharedHitsCfg(flags, **id_helper_args))
        kwargs.setdefault("InDetSummaryHelperTool", InDetSummaryHelperSharedHits)

    if 'TRT_ElectronPidTool' not in kwargs:
        if not flags.Detector.EnableTRT or flags.InDet.doHighPileup \
            or  flags.InDet.useExistingTracksAsInput: # TRT_RDOs (used by the TRT_LocalOccupancy tool) are not present in ESD
            kwargs.setdefault("TRT_ElectronPidTool", None)
        else:
            from InDetConfig.TRT_ElectronPidToolsConfig import TRT_ElectronPidToolCfg
            kwargs.setdefault("TRT_ElectronPidTool", acc.popToolsAndMerge(TRT_ElectronPidToolCfg(flags, name="InDetTRT_ElectronPidTool")))

    if 'PixelToTPIDTool' not in kwargs :
        InDetPixelToTPIDTool = acc.popToolsAndMerge(InDetPixelToTPIDToolCfg(flags))
        kwargs.setdefault( "PixelToTPIDTool", InDetPixelToTPIDTool)

    kwargs.setdefault( "doSharedHits", flags.InDet.doSharedHits)

    InDetTrackSummaryTool = acc.getPrimaryAndMerge(InDetTrackSummaryToolCfg(flags, name, **kwargs))
    acc.addPublicTool(InDetTrackSummaryTool, primary=True)
    return acc

def InDetUpdatorCfg(flags, name = 'InDetUpdator', **kwargs):
    if flags.Detector.GeometryITk:
        name = name.replace("InDet", "ITk")
        from InDetConfig.ITkRecToolConfig import ITkUpdatorCfg
        return ITkUpdatorCfg(flags, name, **kwargs)

    the_name = makeName( name, kwargs )
    acc = ComponentAccumulator()

    tool = None
    if flags.InDet.kalmanUpdator == "fast" :
        tool = CompFactory.Trk.KalmanUpdator_xk(name = the_name, **kwargs)
    elif flags.InDet.kalmanUpdator == "weight" :
        tool = CompFactory.Trk.KalmanWeightUpdator(name = the_name, **kwargs)
    elif flags.InDet.kalmanUpdator == "smatrix" :
        tool = CompFactory.Trk.KalmanUpdatorSMatrix(name = the_name, **kwargs)
    elif flags.InDet.kalmanUpdator == "amg" :
        tool = CompFactory.Trk.KalmanUpdatorAmg(name = the_name, **kwargs)
    else :
        tool = CompFactory.Trk.KalmanUpdator(name = the_name, **kwargs)

    acc.setPrivateTools(tool)
    return acc

def InDetMultipleScatteringUpdatorCfg(name = "InDetMultipleScatteringUpdator", **kwargs):
    acc = ComponentAccumulator()
    the_name = makeName( name, kwargs)

    kwargs.setdefault( "UseTrkUtils", False)
    MultipleScatteringUpdator = CompFactory.Trk.MultipleScatteringUpdator(name = the_name, **kwargs)
    
    acc.setPrivateTools(MultipleScatteringUpdator)
    return acc

def InDetMeasRecalibSTCfg(flags, name='InDetMeasRecalibST', **kwargs) :
    acc = ComponentAccumulator()

    pix_cluster_on_track_args = stripArgs(kwargs,['SplitClusterMapExtension','ClusterSplitProbabilityName','nameSuffix'])

    if 'BroadPixelClusterOnTrackTool' not in kwargs :
        InDetBroadPixelClusterOnTrackTool = acc.popToolsAndMerge(InDetBroadPixelClusterOnTrackToolCfg(flags, **pix_cluster_on_track_args))
        kwargs.setdefault('BroadPixelClusterOnTrackTool', InDetBroadPixelClusterOnTrackTool)
    if 'BroadSCT_ClusterOnTrackTool' not in kwargs :
        InDetBroadSCT_ClusterOnTrackTool = acc.popToolsAndMerge(InDetBroadSCT_ClusterOnTrackToolCfg(flags))
        kwargs.setdefault('BroadSCT_ClusterOnTrackTool', InDetBroadSCT_ClusterOnTrackTool)
    if 'CommonRotCreator' not in kwargs :
        InDetRefitRotCreator = acc.popToolsAndMerge(InDetRefitRotCreatorCfg(flags, **pix_cluster_on_track_args))
        kwargs.setdefault('CommonRotCreator', InDetRefitRotCreator)

    MeasRecalibSteeringTool = CompFactory.Trk.MeasRecalibSteeringTool(name, **kwargs)
    acc.setPrivateTools(MeasRecalibSteeringTool)
    return acc

def InDetKalmanTrackFitterBaseCfg(flags, name='InDetKalmanTrackFitterBase',**kwargs) :
    acc = ComponentAccumulator()
    nameSuffix=kwargs.pop('nameSuffix','')
    pix_cluster_on_track_args = stripArgs(kwargs,['SplitClusterMapExtension','ClusterSplitProbabilityName'])
    if len(pix_cluster_on_track_args)>0 and len(nameSuffix)>0 :
        pix_cluster_on_track_args['nameSuffix']=nameSuffix

    from TrkConfig.AtlasExtrapolatorConfig import InDetExtrapolatorCfg
    kwargs.setdefault("ExtrapolatorHandle", acc.getPrimaryAndMerge(InDetExtrapolatorCfg(flags)))

    if 'RIO_OnTrackCreatorHandle' not in kwargs :
        InDetRefitRotCreator = acc.popToolsAndMerge(InDetRefitRotCreatorCfg(flags, **pix_cluster_on_track_args))
        kwargs.setdefault("RIO_OnTrackCreatorHandle", InDetRefitRotCreator)

    InDetUpdator = acc.popToolsAndMerge(InDetUpdatorCfg(flags))
    kwargs.setdefault('MeasurementUpdatorHandle', InDetUpdator)
    kwargs.setdefault('KalmanSmootherHandle', InDetBKS())
    kwargs.setdefault('KalmanOutlierLogicHandle', InDetKOL())
    kwargs.setdefault('DynamicNoiseAdjustorHandle', None)
    kwargs.setdefault('BrempointAnalyserHandle', None)
    kwargs.setdefault('AlignableSurfaceProviderHandle',None)

    if len(pix_cluster_on_track_args)>0 :
        if 'RecalibratorHandle' not in kwargs :
            the_tool_name = 'InDetMeasRecalibST'
            InDetMeasRecalibST = acc.popToolsAndMerge(InDetMeasRecalibSTCfg(flags, name=the_tool_name+nameSuffix, **pix_cluster_on_track_args))
            kwargs.setdefault('RecalibratorHandle', InDetMeasRecalibST)
    else :
        InDetMeasRecalibST = acc.popToolsAndMerge(InDetMeasRecalibSTCfg(flags))
        kwargs.setdefault('RecalibratorHandle', InDetMeasRecalibST)

    KalmanFitter = CompFactory.Trk.KalmanFitter(name=name, **kwargs)
    acc.setPrivateTools(KalmanFitter)
    return acc

def InDetDNAdjustor(name='InDetDNAdjustor', **kwargs) :
    return CompFactory.Trk.InDetDynamicNoiseAdjustment(name=name,**kwargs)

def InDetDNASeparator(name='InDetDNASeparator',**kwargs) :
    return CompFactory.InDet.InDetDNASeparator(name=name,**kwargs)

def InDetAnnealBKS(name='InDetAnnealBKS',**kwargs) :
    kwargs.setdefault('InitialCovarianceSeedFactor',200.)
    return CompFactory.Trk.KalmanSmoother(name=name,**kwargs)

def KalmanCompetingPixelClustersTool(name='KalmanCompetingPixelClustersTool',**kwargs):
    kwargs.setdefault('WeightCutValueBarrel',5.5)
    kwargs.setdefault('WeightCutValueEndCap',5.5)
    return CompFactory.InDet.CompetingPixelClustersOnTrackTool(name=name, **kwargs)

def KalmanCompetingSCT_ClustersTool(name='KalmanCompetingSCT_ClustersTool',**kwargs) :
    kwargs.setdefault('WeightCutValueBarrel',5.5)
    kwargs.setdefault('WeightCutValueEndCap',5.5)
    return CompFactory.InDet.CompetingSCT_ClustersOnTrackTool(name=name,**kwargs)

def KalmanCompetingRIOsTool(name='KalmanCompetingRIOsTool',**kwargs) :
    kwargs.setdefault('ToolForCompPixelClusters',KalmanCompetingPixelClustersTool())
    kwargs.setdefault('ToolForCompSCT_Clusters',KalmanCompetingSCT_ClustersTool())
    return CompFactory.Trk.CompetingRIOsOnTrackTool(name=name,**kwargs)

def KalmanInternalDAF(name='KalmanInternalDAF',**kwargs) :
    kwargs.setdefault('CompetingRIOsOnTrackCreator',KalmanCompetingRIOsTool())
    kwargs.setdefault('BackwardSmoother',InDetAnnealBKS())
    return CompFactory.Trk.KalmanPiecewiseAnnealingFilter(name=name,**kwargs)

def InDetAnnealFKF(name='InDetAnnealFKF',**kwargs) :
    return CompFactory.Trk.ForwardKalmanFitter(name=name,**kwargs)

def InDetFKFRef(name='InDetFKFRef',**kwargs) :
    kwargs.setdefault('StateChi2PerNDFPreCut',30.0)
    return InDetAnnealFKF(name=name,**kwargs)

def KalmanInternalDAFRef(name='KalmanInternalDAFRef',**kwargs) :
    kwargs.setdefault('ForwardFitter', InDetAnnealFKF())
    kwargs.setdefault('BackwardSmoother', InDetBKS())
    return KalmanInternalDAF(name=name,**kwargs)

def InDetBroadRotCreatorCfg(flags, name='InDetBroadInDetRotCreator', **kwargs) :
    acc = ComponentAccumulator()

    if 'ToolPixelCluster' not in kwargs :
        pix_cluster_on_track_args = copyArgs(kwargs,['SplitClusterMapExtension','ClusterSplitProbabilityName','nameSuffix'])
        InDetBroadPixelClusterOnTrackTool = acc.popToolsAndMerge(InDetBroadPixelClusterOnTrackToolCfg(flags, **pix_cluster_on_track_args))
        kwargs.setdefault('ToolPixelCluster', InDetBroadPixelClusterOnTrackTool)

    if 'ToolSCT_Cluster' not in kwargs :
        InDetBroadSCT_ClusterOnTrackTool = acc.popToolsAndMerge(InDetBroadSCT_ClusterOnTrackToolCfg(flags))
        kwargs.setdefault('ToolSCT_Cluster', InDetBroadSCT_ClusterOnTrackTool)

    if flags.Detector.EnableTRT:
        if 'ToolTRT_DriftCircle' not in kwargs :
            InDetBroadTRT_DriftCircleOnTrackTool = acc.popToolsAndMerge(InDetBroadTRT_DriftCircleOnTrackToolCfg())
            kwargs.setdefault('ToolTRT_DriftCircle', InDetBroadTRT_DriftCircleOnTrackTool)

    InDetRotCreator = acc.popToolsAndMerge(InDetRotCreatorCfg(flags, name = name, **kwargs))
    acc.setPrivateTools(InDetRotCreator)
    return acc

def KalmanFitterCfg(flags, name='KalmanFitter',**kwargs) :
    acc = ComponentAccumulator()

    kwargs.setdefault('ForwardKalmanFitterHandle', InDetFKF())
    if flags.InDet.doBremRecovery:
        kwargs.setdefault('DynamicNoiseAdjustorHandle', InDetDNAdjustor())
        kwargs.setdefault('BrempointAnalyserHandle', InDetDNASeparator())
        kwargs.setdefault('DoDNAForElectronsOnly', True)
    else:
        kwargs.setdefault('DynamicNoiseAdjustorHandle', None)
        kwargs.setdefault('BrempointAnalyserHandle', None)

    kwargs.setdefault('InternalDAFHandle', KalmanInternalDAF())

    InDetKalmanTrackFitterBase = acc.popToolsAndMerge(InDetKalmanTrackFitterBaseCfg(flags, name=name, **kwargs))
    acc.setPrivateTools(InDetKalmanTrackFitterBase)
    return acc

def KalmanDNAFitterCfg(flags, name='KalmanDNAFitter',**kwargs) :
    acc = ComponentAccumulator()

    kwargs.setdefault('ForwardKalmanFitterHandle', InDetFKF())
    kwargs.setdefault('DynamicNoiseAdjustorHandle', InDetDNAdjustor())
    kwargs.setdefault('BrempointAnalyserHandle', InDetDNASeparator())
    kwargs.setdefault('InternalDAFHandle', KalmanInternalDAF())

    InDetKalmanTrackFitterBase = acc.popToolsAndMerge(InDetKalmanTrackFitterBaseCfg(flags, name=name, **kwargs))
    acc.setPrivateTools(InDetKalmanTrackFitterBase)
    return acc

def ReferenceKalmanFitterCfg(flags, name='ReferenceKalmanFitter',**kwargs) :
    acc = ComponentAccumulator()
    
    kwargs.setdefault('ForwardKalmanFitterHandle', InDetFKFRef())
    kwargs.setdefault('InternalDAFHandle', KalmanInternalDAFRef())

    InDetKalmanTrackFitterBase = acc.popToolsAndMerge(InDetKalmanTrackFitterBaseCfg(flags, name=name, **kwargs))
    acc.setPrivateTools(InDetKalmanTrackFitterBase)
    return acc

def DistributedKalmanFilterCfg(flags, name="DistributedKalmanFilter", **kwargs) :
    acc = ComponentAccumulator()

    pix_cluster_on_track_args = stripArgs(kwargs,['SplitClusterMapExtension','ClusterSplitProbabilityName','nameSuffix'])

    if 'ExtrapolatorTool' not in kwargs :
        from TrkConfig.AtlasExtrapolatorConfig import InDetExtrapolatorCfg
        kwargs.setdefault('ExtrapolatorTool', acc.getPrimaryAndMerge(InDetExtrapolatorCfg(flags)))

    if 'ROTcreator' not in kwargs :
        InDetRotCreator = acc.popToolsAndMerge(InDetRotCreatorCfg(flags, **pix_cluster_on_track_args))
        kwargs.setdefault('ROTcreator', InDetRotCreator)

    DistributedKalmanFilter = CompFactory.Trk.DistributedKalmanFilter(name = name, **kwargs)
    acc.setPrivateTools(DistributedKalmanFilter)
    return acc

def InDetGlobalChi2FitterCfg(flags, name='InDetGlobalChi2Fitter', **kwargs) :
    acc = ComponentAccumulator()

    pix_cluster_on_track_args = stripArgs(kwargs,['SplitClusterMapExtension','ClusterSplitProbabilityName','nameSuffix'])
    # PHF cut during fit iterations to save CPU time
    kwargs.setdefault('MinPHFCut', flags.InDet.Tracking.Pass.minTRTPrecFrac)

    if 'RotCreatorTool' not in kwargs :
        InDetRotCreator = acc.popToolsAndMerge(InDetRotCreatorCfg(flags, **pix_cluster_on_track_args))
        kwargs.setdefault('RotCreatorTool', InDetRotCreator)

    if 'BroadRotCreatorTool' not in kwargs and not flags.InDet.Tracking.doRefit:
        InDetBroadRotCreator = acc.popToolsAndMerge(InDetBroadRotCreatorCfg(flags, **pix_cluster_on_track_args))
        kwargs.setdefault('BroadRotCreatorTool', InDetBroadRotCreator)

    if flags.InDet.doDBMstandalone:
        kwargs.setdefault('StraightLine', True)
        kwargs.setdefault('OutlierCut', 5)
        kwargs.setdefault('RecalibrateTRT', False)
        kwargs.setdefault('TRTExtensionCuts', False)
        kwargs.setdefault('TrackChi2PerNDFCut', 20)

    if flags.InDet.useBroadClusterErrors and not flags.InDet.doDBMstandalone:
        kwargs.setdefault('RecalibrateSilicon', False)

    if flags.InDet.Tracking.doRefit:
        kwargs.setdefault('RecalibrateSilicon', False)
        kwargs.setdefault('BroadRotCreatorTool', None)
        kwargs.setdefault('ReintegrateOutliers', False)
        kwargs.setdefault('RecalibrateTRT', False)

    if flags.InDet.doRobustReco:
        kwargs.setdefault('OutlierCut', 10.0)
        kwargs.setdefault('TrackChi2PerNDFCut', 20)

    if flags.InDet.doRobustReco or flags.Beam.Type == 'cosmics':
        kwargs.setdefault('MaxOutliers', 99)

    if flags.Beam.Type == 'cosmics' or flags.InDet.doBeamHalo:
        kwargs.setdefault('Acceleration', False)

    if flags.InDet.materialInteractions and not flags.BField.solenoidOn:
        kwargs.setdefault('Momentum', 1000.*Units.MeV)

    InDetGlobalChi2FitterBase = acc.popToolsAndMerge(InDetGlobalChi2FitterBaseCfg(flags, name=name, **kwargs))
    acc.setPrivateTools(InDetGlobalChi2FitterBase)
    return acc

def GaussianSumFitterCfg(flags, name="GaussianSumFitter", **kwargs):
    acc = ComponentAccumulator()

    pix_cluster_on_track_args = stripArgs(
        kwargs,
        [
            "SplitClusterMapExtension",
            "ClusterSplitProbabilityName",
            "nameSuffix",
        ],
    )

    if "ToolForROTCreation" not in kwargs:
        InDetRotCreator = acc.popToolsAndMerge(
            InDetRotCreatorCfg(flags, **pix_cluster_on_track_args)
        )
        kwargs.setdefault("ToolForROTCreation", InDetRotCreator)

    kwargs.setdefault("MakePerigee", True)
    kwargs.setdefault("RefitOnMeasurementBase", True)
    kwargs.setdefault("DoHitSorting", True)

    from egammaTrackTools.GSFTrackFitterConfig import EMGSFTrackFitterCfg

    GaussianSumFitter = acc.popToolsAndMerge(
        EMGSFTrackFitterCfg(flags, name=name, **kwargs)
    )

    acc.setPrivateTools(GaussianSumFitter)
    return acc

def InDetTrackFitterCfg(flags, name='InDetTrackFitter', **kwargs) :
    return {
            'KalmanFitter'            : KalmanFitterCfg,
            'KalmanDNAFitter'         : KalmanDNAFitterCfg,
            'ReferenceKalmanFitter'   : ReferenceKalmanFitterCfg,
            'DistributedKalmanFilter' : DistributedKalmanFilterCfg,
            'GlobalChi2Fitter'        : InDetGlobalChi2FitterCfg,
            'GaussianSumFilter'       : GaussianSumFitterCfg
    }[flags.InDet.trackFitterType](flags, name=name, **kwargs)


def InDetGlobalChi2FitterBaseCfg(flags, name='GlobalChi2FitterBase', **kwargs):
    acc = ComponentAccumulator()

    if 'TrackingGeometryReadKey' not in kwargs:
        from TrackingGeometryCondAlg.AtlasTrackingGeometryCondAlgConfig import (
            TrackingGeometryCondAlgCfg)
        cond_alg = TrackingGeometryCondAlgCfg(flags)
        geom_cond_key = cond_alg.getPrimary().TrackingGeometryWriteKey
        acc.merge(cond_alg)
        kwargs.setdefault("TrackingGeometryReadKey", geom_cond_key)

    from TrkConfig.AtlasExtrapolatorConfig import InDetExtrapolatorCfg
    from TrkConfig.AtlasExtrapolatorToolsConfig import (
        AtlasNavigatorCfg, AtlasEnergyLossUpdatorCfg, InDetPropagatorCfg, InDetMaterialEffectsUpdatorCfg)

    InDetExtrapolator = acc.getPrimaryAndMerge(InDetExtrapolatorCfg(flags))
    InDetNavigator = acc.popToolsAndMerge(AtlasNavigatorCfg(flags, name="InDetNavigator"))
    ELossUpdator = acc.popToolsAndMerge(AtlasEnergyLossUpdatorCfg(flags))
    InDetPropagator = acc.popToolsAndMerge(InDetPropagatorCfg(flags))
    InDetUpdator = acc.popToolsAndMerge(InDetUpdatorCfg(flags))

    InDetMultipleScatteringUpdator = acc.popToolsAndMerge(
        InDetMultipleScatteringUpdatorCfg())

    InDetMaterialEffectsUpdator = acc.popToolsAndMerge(
        InDetMaterialEffectsUpdatorCfg(flags))

    kwargs.setdefault("ExtrapolationTool", InDetExtrapolator)
    kwargs.setdefault("NavigatorTool", InDetNavigator)
    kwargs.setdefault("PropagatorTool", InDetPropagator)
    kwargs.setdefault("MultipleScatteringTool", InDetMultipleScatteringUpdator)
    kwargs.setdefault("EnergyLossTool", ELossUpdator)
    kwargs.setdefault("MeasurementUpdateTool", InDetUpdator)
    kwargs.setdefault("MaterialUpdateTool", InDetMaterialEffectsUpdator)
    kwargs.setdefault("StraightLine", not flags.BField.solenoidOn)
    kwargs.setdefault("OutlierCut", 4)
    kwargs.setdefault("SignedDriftRadius", True)
    kwargs.setdefault("ReintegrateOutliers", True)
    kwargs.setdefault("RecalibrateSilicon", True)
    kwargs.setdefault("RecalibrateTRT", True)
    kwargs.setdefault("TRTTubeHitCut", 1.75)
    kwargs.setdefault("MaxIterations", 40)
    kwargs.setdefault("Acceleration", True)
    kwargs.setdefault("RecalculateDerivatives",
                      flags.InDet.doMinBias or flags.Beam.Type == 'cosmics' or flags.InDet.doBeamHalo)
    kwargs.setdefault("TRTExtensionCuts", True)
    kwargs.setdefault("TrackChi2PerNDFCut", 7)

    GlobalChi2Fitter = CompFactory.Trk.GlobalChi2Fitter(name=name, **kwargs)
    acc.setPrivateTools(GlobalChi2Fitter)
    return acc

def InDetGlobalChi2FitterTRTCfg(flags, name='InDetGlobalChi2FitterTRT', **kwargs) :
    acc = ComponentAccumulator()
    '''
    Global Chi2 Fitter for TRT segments with different settings
    '''
    pix_cluster_on_track_args = stripArgs(kwargs,['SplitClusterMapExtension','ClusterSplitProbabilityName','nameSuffix'])

    if 'RotCreatorTool' not in kwargs :
        InDetRefitRotCreator = acc.popToolsAndMerge(InDetRefitRotCreatorCfg(flags, **pix_cluster_on_track_args))
        kwargs.setdefault("RotCreatorTool", InDetRefitRotCreator)

    kwargs.setdefault("MaterialUpdateTool", '')
    kwargs.setdefault("SignedDriftRadius", True)
    kwargs.setdefault("RecalibrateSilicon", False)
    kwargs.setdefault("RecalibrateTRT", False)
    kwargs.setdefault("TRTTubeHitCut", 2.5)
    kwargs.setdefault("MaxIterations", 10)
    kwargs.setdefault("Acceleration", False)
    kwargs.setdefault("RecalculateDerivatives", False)
    kwargs.setdefault("TRTExtensionCuts", True)
    kwargs.setdefault("TrackChi2PerNDFCut", 999999)
    kwargs.setdefault("Momentum", 1000.*Units.MeV   if flags.InDet.materialInteractions and not flags.BField.solenoidOn else  0)
    kwargs.setdefault("OutlierCut", 5)
    kwargs.setdefault("MaxOutliers", 99 if flags.InDet.doRobustReco or flags.Beam.Type == 'cosmics' else 10)
    kwargs.setdefault("ReintegrateOutliers", False)

    InDetGlobalChi2FitterBase = acc.popToolsAndMerge(InDetGlobalChi2FitterBaseCfg(flags, name=name, **kwargs))
    acc.setPrivateTools(InDetGlobalChi2FitterBase)
    return acc

def InDetTrackFitterTRTCfg(flags, name='InDetTrackFitterTRT', **kwargs) :
    acc = ComponentAccumulator()

    if flags.InDet.trackFitterType != 'GlobalChi2Fitter' :
        InDetTrackFitter = acc.popToolsAndMerge(InDetTrackFitterCfg(flags, name, **kwargs))
        acc.setPrivateTools(InDetTrackFitter)
    else :
        InDetGlobalChi2FitterTRT = acc.popToolsAndMerge(InDetGlobalChi2FitterTRTCfg(flags, name, **kwargs))
        acc.setPrivateTools(InDetGlobalChi2FitterTRT)

    return acc

def InDetGlobalChi2FitterLowPtCfg(flags, name='InDetGlobalChi2FitterLowPt', **kwargs) :
    acc = ComponentAccumulator()
    # @TODO TrackingGeometrySvc was not set but is set now
    #       RotCreatorTool and BroadRotCreatorTool not set
    pix_cluster_on_track_args = stripArgs(kwargs,['SplitClusterMapExtension','ClusterSplitProbabilityName','nameSuffix'])

    if 'RotCreatorTool' not in kwargs :
        InDetRotCreator = acc.popToolsAndMerge(InDetRotCreatorCfg(flags, **pix_cluster_on_track_args))
        kwargs.setdefault('RotCreatorTool', InDetRotCreator)

    if 'BroadRotCreatorTool' not in kwargs and not flags.InDet.Tracking.doRefit:
        InDetBroadRotCreator = acc.popToolsAndMerge(InDetBroadRotCreatorCfg(flags, **pix_cluster_on_track_args))
        kwargs.setdefault('BroadRotCreatorTool', InDetBroadRotCreator)

    kwargs.setdefault('OutlierCut', 5.0)
    kwargs.setdefault('Acceleration', False)
    kwargs.setdefault('RecalculateDerivatives', True)
    kwargs.setdefault('TrackChi2PerNDFCut', 10)

    InDetGlobalChi2FitterBase = acc.popToolsAndMerge(InDetGlobalChi2FitterBaseCfg(flags, name=name, **kwargs))
    acc.setPrivateTools(InDetGlobalChi2FitterBase)
    return acc

def InDetTrackFitterLowPt(flags, name='InDetTrackFitter', **kwargs) :
    acc = ComponentAccumulator()

    if flags.InDet.trackFitterType != 'GlobalChi2Fitter' :
        InDetTrackFitter = acc.popToolsAndMerge(InDetTrackFitterCfg(flags, name, **kwargs))
        acc.setPrivateTools(InDetTrackFitter)
    else :
        InDetGlobalChi2FitterLowPt = acc.popToolsAndMerge(InDetGlobalChi2FitterLowPtCfg(flags, name, **kwargs))
        acc.setPrivateTools(InDetGlobalChi2FitterLowPt)
    return acc

def InDetGlobalChi2FitterBTCfg(flags, name='InDetGlobalChi2FitterBT', **kwargs):
    acc = ComponentAccumulator()
    '''
    Global Chi2 Fitter for backtracking
    '''
    kwargs.setdefault("MinPHFCut", 0.)

    InDetGlobalChi2Fitter = acc.popToolsAndMerge(InDetGlobalChi2FitterCfg(flags, name=name, **kwargs))
    acc.setPrivateTools(InDetGlobalChi2Fitter)
    return acc

def InDetTrackFitterBTCfg(flags, name='InDetTrackFitterBT', **kwargs) :
    acc = ComponentAccumulator()

    if flags.InDet.trackFitterType != 'GlobalChi2Fitter' :
        InDetTrackFitter = acc.popToolsAndMerge(InDetTrackFitterCfg(flags, name, **kwargs))
        acc.setPrivateTools(InDetTrackFitter)
    else :
        InDetGlobalChi2Fitter = acc.popToolsAndMerge(InDetGlobalChi2FitterBTCfg(flags, name, **kwargs))
        acc.setPrivateTools(InDetGlobalChi2Fitter)
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
#############################################################################################
#TRTSegmentFinder
#############################################################################################
def InDetPatternPropagatorCfg(name='InDetPatternPropagator', **kwargs):
    the_name = makeName( name, kwargs)
    result = ComponentAccumulator()
    tool = CompFactory.Trk.RungeKuttaPropagator(name = the_name, **kwargs)
    result.addPublicTool( tool, primary=True )
    return result

def InDetTRT_DriftCircleOnTrackUniversalToolCosmicsCfg(name='TRT_DriftCircleOnTrackUniversalTool', **kwargs):
    kwargs.setdefault("ScaleHitUncertainty", 2.)
    return InDetTRT_DriftCircleOnTrackUniversalToolCfg(name=name, **kwargs)

def InDetTRT_ExtensionToolCosmicsCfg(flags, name='InDetTRT_ExtensionToolCosmics', **kwargs):
    acc = ComponentAccumulator()
    the_name = makeName( name, kwargs)

    if 'Propagator' not in kwargs :
        from TrkConfig.AtlasExtrapolatorToolsConfig import InDetPropagatorCfg
        InDetPropagator = acc.getPrimaryAndMerge(InDetPropagatorCfg(flags))
        kwargs.setdefault("Propagator", InDetPropagator)

    if 'Extrapolator' not in kwargs :
        from TrkConfig.AtlasExtrapolatorConfig import InDetExtrapolatorCfg
        kwargs.setdefault("Extrapolator", acc.getPrimaryAndMerge(InDetExtrapolatorCfg(flags)))

    if 'RIOonTrackToolYesDr' not in kwargs :
        InDetTRT_DriftCircleOnTrackUniversalToolCosmics = acc.popToolsAndMerge(InDetTRT_DriftCircleOnTrackUniversalToolCosmicsCfg())
        kwargs.setdefault("RIOonTrackToolYesDr", InDetTRT_DriftCircleOnTrackUniversalToolCosmics)

    if 'RIOonTrackToolNoDr' not in kwargs :
        InDetBroadTRT_DriftCircleOnTrackTool = acc.popToolsAndMerge(InDetBroadTRT_DriftCircleOnTrackToolCfg())
        kwargs.setdefault("RIOonTrackToolNoDr", InDetBroadTRT_DriftCircleOnTrackTool)

    kwargs.setdefault("TRT_ClustersContainer", 'TRT_DriftCircles') # InDetKeys.TRT_DriftCircles()
    kwargs.setdefault("SearchNeighbour", False)   # needs debugging!!!
    kwargs.setdefault("RoadWidth", 10.0)
    acc.setPrivateTools(CompFactory.InDet.TRT_TrackExtensionToolCosmics(name = the_name, **kwargs))
    return acc

def InDetPatternUpdatorCfg(name='InDetPatternUpdator', **kwargs):
    the_name = makeName(name, kwargs)
    result = ComponentAccumulator()
    tool = CompFactory.Trk.KalmanUpdator_xk(name = the_name, **kwargs)
    result.addPublicTool( tool, primary=True )
    return result

def InDetTRT_TrackExtensionTool_xkCfg(flags, name='InDetTRT_ExtensionTool', **kwargs):
    from MagFieldServices.MagFieldServicesConfig import MagneticFieldSvcCfg
    acc = MagneticFieldSvcCfg(flags)

    the_name = makeName( name, kwargs)

    if 'PropagatorTool' not in kwargs :
        InDetPatternPropagator = acc.getPrimaryAndMerge(InDetPatternPropagatorCfg())
        kwargs.setdefault("PropagatorTool", InDetPatternPropagator)

    if 'UpdatorTool' not in kwargs :
        InDetPatternUpdator = acc.getPrimaryAndMerge(InDetPatternUpdatorCfg())
        kwargs.setdefault("UpdatorTool", InDetPatternUpdator)

    if 'DriftCircleCutTool' not in kwargs :
        InDetTRTDriftCircleCutForPatternReco = acc.popToolsAndMerge(InDetTRTDriftCircleCutForPatternRecoCfg(flags))
        kwargs.setdefault("DriftCircleCutTool", InDetTRTDriftCircleCutForPatternReco)

    if 'RIOonTrackToolYesDr' not in kwargs :
        kwargs.setdefault("RIOonTrackToolYesDr", acc.popToolsAndMerge(InDetTRT_DriftCircleOnTrackToolCfg(flags)))

    if 'RoadTool' not in kwargs :
        InDetTRT_RoadMaker = acc.popToolsAndMerge(InDetTRT_RoadMakerCfg(flags))
        kwargs.setdefault("RoadTool", InDetTRT_RoadMaker)

    kwargs.setdefault("TRT_ClustersContainer", 'TRT_DriftCircles') # InDetKeys.TRT_DriftCircles()
    kwargs.setdefault("TrtManagerLocation", 'TRT') # InDetKeys.TRT_Manager()
    kwargs.setdefault("UseDriftRadius", not flags.InDet.noTRTTiming)
    kwargs.setdefault("MinNumberDriftCircles", flags.InDet.Tracking.Pass.minTRTonTrk)
    kwargs.setdefault("ScaleHitUncertainty", 2)
    kwargs.setdefault("RoadWidth", 20.)
    kwargs.setdefault("UseParameterization", flags.InDet.Tracking.Pass.useParameterizedTRTCuts)
    kwargs.setdefault("maxImpactParameter", 500 if flags.InDet.doBeamHalo or flags.InDet.doBeamGas else 50 )  # single beam running, open cuts

    if flags.InDet.Tracking.Pass.RoISeededBackTracking:
        kwargs.setdefault("minTRTSegmentpT", flags.InDet.Tracking.Pass.minSecondaryPt)

    acc.merge(TRT_DetElementsRoadCondAlgCfg(flags))
    from TRT_ConditionsAlgs.TRT_ConditionsAlgsConfig import TRTActiveCondAlgCfg
    acc.merge(TRTActiveCondAlgCfg(flags))
    acc.setPrivateTools(CompFactory.InDet.TRT_TrackExtensionTool_xk(the_name, **kwargs))
    return acc

def InDetWeightCalculatorCfg(name='InDetWeightCalculator', **kwargs):
    acc = ComponentAccumulator()
    the_name = makeName( name, kwargs)
    acc.setPrivateTools(CompFactory.Trk.DAF_SimpleWeightCalculator(name = the_name, **kwargs))
    return acc

def InDetCompetingTRT_DC_ToolCfg(flags, name='InDetCompetingTRT_DC_Tool', **kwargs):
    acc = ComponentAccumulator()
    the_name = makeName( name, kwargs)

    if 'Extrapolator' not in kwargs :
        from TrkConfig.AtlasExtrapolatorConfig import InDetExtrapolatorCfg
        kwargs.setdefault("Extrapolator", acc.getPrimaryAndMerge(InDetExtrapolatorCfg(flags)))

    if 'ToolForWeightCalculation' not in kwargs :
        InDetWeightCalculator = acc.popToolsAndMerge(InDetWeightCalculatorCfg())
        kwargs.setdefault("ToolForWeightCalculation", InDetWeightCalculator)

    if 'ToolForTRT_DriftCircleOnTrackCreation' not in kwargs :
        kwargs.setdefault("ToolForTRT_DriftCircleOnTrackCreation", acc.popToolsAndMerge(InDetTRT_DriftCircleOnTrackToolCfg(flags)))

    acc.setPrivateTools(CompFactory.InDet.CompetingTRT_DriftCirclesOnTrackTool( the_name, **kwargs))
    return acc

def InDetTRT_RoadMakerCfg(flags, name='InDetTRT_RoadMaker', **kwargs):
    from MagFieldServices.MagFieldServicesConfig import MagneticFieldSvcCfg
    acc = MagneticFieldSvcCfg(flags)

    the_name = makeName( name, kwargs)
    
    InDetPatternPropagator = acc.getPrimaryAndMerge(InDetPatternPropagatorCfg())
    kwargs.setdefault("RoadWidth", 20.)
    kwargs.setdefault("PropagatorTool", InDetPatternPropagator)
    acc.setPrivateTools(CompFactory.InDet.TRT_DetElementsRoadMaker_xk(the_name, **kwargs))
    return acc

def InDetTRT_TrackExtensionTool_DAFCfg(flags, name='TRT_TrackExtensionTool_DAF',**kwargs):
    acc = ComponentAccumulator()
    the_name = makeName( name, kwargs)

    if 'CompetingDriftCircleTool' not in kwargs :
        InDetCompetingTRT_DC_Tool = acc.popToolsAndMerge(InDetCompetingTRT_DC_ToolCfg(flags))
        kwargs.setdefault("CompetingDriftCircleTool", InDetCompetingTRT_DC_Tool)

    if 'PropagatorTool' not in kwargs :
        InDetPatternPropagator = acc.getPrimaryAndMerge(InDetPatternPropagatorCfg())
        kwargs.setdefault("PropagatorTool", InDetPatternPropagator)

    if 'RoadTool' not in kwargs :
        InDetTRT_RoadMaker = acc.popToolsAndMerge(InDetTRT_RoadMakerCfg(flags)) 
        kwargs.setdefault("RoadTool", InDetTRT_RoadMaker)

    kwargs.setdefault("TRT_DriftCircleContainer", 'TRT_DriftCircles') # InDetKeys.TRT_DriftCircles()

    acc.setPrivateTools(CompFactory.InDet.TRT_TrackExtensionTool_DAF(the_name,**kwargs))
    return acc

def TRT_DetElementsRoadCondAlgCfg(flags, **kwargs):
    acc = ComponentAccumulator()
    the_name=kwargs.pop("name","InDet__TRT_DetElementsRoadCondAlg_xk")
    acc.addCondAlgo(CompFactory.InDet.TRT_DetElementsRoadCondAlg_xk(the_name, **kwargs))
    return acc

def InDetTRT_ExtensionToolCfg(flags, **kwargs):
    # @TODO set all names to InDetTRT_ExtensionTool ?
    if flags.InDet.trtExtensionType == 'xk':
        if flags.Beam.Type == "cosmics":
            return InDetTRT_ExtensionToolCosmicsCfg(flags, **kwargs)
        else:
            return InDetTRT_TrackExtensionTool_xkCfg(flags, **kwargs)
    if flags.InDet.trtExtensionType == 'DAF':
        return InDetTRT_TrackExtensionTool_DAFCfg(flags, name='InDetTRT_ExtensionTool', **kwargs)

#############################################################################################
# BackTracking
#############################################################################################
def InDetRotCreatorDigitalCfg(flags, name='InDetRotCreatorDigital', **kwargs) :
    acc = ComponentAccumulator()
    if 'ToolPixelCluster' not in kwargs :
        pix_cluster_on_track_args = copyArgs(kwargs,['SplitClusterMapExtension','ClusterSplitProbabilityName','nameSuffix'])

        ToolPixelCluster = acc.popToolsAndMerge(InDetPixelClusterOnTrackToolDigitalCfg(flags, **pix_cluster_on_track_args))
        kwargs.setdefault('ToolPixelCluster', ToolPixelCluster)

    acc.setPrivateTools(acc.popToolsAndMerge(InDetRotCreatorCfg(flags, name=name, **kwargs)))
    return acc

def InDetTrackSummaryToolNoHoleSearchCfg(flags, name='InDetTrackSummaryToolNoHoleSearch',**kwargs) :
    acc = ComponentAccumulator()
    kwargs.setdefault('doHolesInDet', False)
    InDetTrackSummaryTool = acc.getPrimaryAndMerge(InDetTrackSummaryToolCfg(flags, name=name, **kwargs))
    acc.setPrivateTools(InDetTrackSummaryTool)
    return acc

def ROIInfoVecAlgCfg(flags, name='InDetROIInfoVecCondAlg', **kwargs) :
    acc = ComponentAccumulator()
    from InDetConfig.InDetRecCaloSeededROISelectionConfig import CaloClusterROI_SelectorCfg
    acc.merge(CaloClusterROI_SelectorCfg(flags))
    kwargs.setdefault("InputEmClusterContainerName", "InDetCaloClusterROIs")
    kwargs.setdefault("WriteKey", kwargs.get("namePrefix","") +"ROIInfoVec"+ kwargs.get("nameSuffix","") )
    kwargs.setdefault("minPtEM", 5000.0) #in MeV
    acc.addEventAlgo(CompFactory.ROIInfoVecAlg(name = name,**kwargs), primary=True)
    return acc

def InDetAmbiScoringToolBaseCfg(flags, name='InDetAmbiScoringTool', **kwargs) :
    acc = ComponentAccumulator()

    from TrkConfig.AtlasExtrapolatorConfig import InDetExtrapolatorCfg
    kwargs.setdefault("Extrapolator", acc.getPrimaryAndMerge(InDetExtrapolatorCfg(flags)))

    InDetTrackSummaryTool = acc.getPrimaryAndMerge(InDetTrackSummaryToolCfg(flags))

    if 'DriftCircleCutTool' not in kwargs :
        InDetTRTDriftCircleCutForPatternReco = acc.popToolsAndMerge(InDetTRTDriftCircleCutForPatternRecoCfg(flags))
        kwargs.setdefault("DriftCircleCutTool", InDetTRTDriftCircleCutForPatternReco )

    have_calo_rois = flags.InDet.doBremRecovery and flags.InDet.doCaloSeededBrem and flags.Detector.EnableCalo
    if have_calo_rois:
        alg = acc.getPrimaryAndMerge(ROIInfoVecAlgCfg(flags))
        kwargs.setdefault("CaloROIInfoName", alg.WriteKey )
    kwargs.setdefault("SummaryTool", InDetTrackSummaryTool )
    kwargs.setdefault("useAmbigFcn", True )
    kwargs.setdefault("useTRT_AmbigFcn", False )
    kwargs.setdefault("maxZImp", flags.InDet.Tracking.Pass.maxZImpact )
    kwargs.setdefault("maxEta", flags.InDet.Tracking.Pass.maxEta )
    kwargs.setdefault("usePixel", flags.InDet.Tracking.Pass.usePixel )
    kwargs.setdefault("useSCT", flags.InDet.Tracking.Pass.useSCT )
    kwargs.setdefault("doEmCaloSeed", have_calo_rois )
    acc.setPrivateTools(CompFactory.InDet.InDetAmbiScoringTool(name, **kwargs))
    return acc

def InDetCosmicsScoringToolBaseCfg(flags, name='InDetCosmicsScoringTool', **kwargs) :
    acc = ComponentAccumulator()
    the_name=makeName(name, kwargs)

    InDetTrackSummaryTool = acc.getPrimaryAndMerge(InDetTrackSummaryToolCfg(flags))

    kwargs.setdefault("nWeightedClustersMin", flags.InDet.Tracking.Pass.nWeightedClustersMin )
    kwargs.setdefault("minTRTHits", 0 )
    kwargs.setdefault("SummaryTool", InDetTrackSummaryTool )

    acc.setPrivateTools(CompFactory.InDet.InDetCosmicScoringTool(name = the_name, **kwargs ))
    return acc

def InDetTRT_ExtensionToolPhaseCfg(flags, name='InDetTRT_ExtensionToolPhase', **kwargs) :
    acc = ComponentAccumulator()
    InDetTRT_DriftCircleOnTrackUniversalTool = acc.popToolsAndMerge(InDetTRT_DriftCircleOnTrackUniversalToolCfg())
    if 'RIOonTrackToolYesDr' not in kwargs :
        kwargs.setdefault("RIOonTrackToolYesDr", InDetTRT_DriftCircleOnTrackUniversalTool)

    kwargs.setdefault("TRT_ClustersContainer", 'TRT_DriftCirclesUncalibrated')
    kwargs.setdefault("RoadWidth", 20.0)
    acc.setPrivateTools(acc.popToolsAndMerge(InDetTRT_ExtensionToolCosmicsCfg(flags, name = name, **kwargs)))
    return acc

def InDetCosmicExtenScoringToolCfg(flags, name='InDetCosmicExtenScoringTool',**kwargs) :
    acc = ComponentAccumulator()
    kwargs.setdefault("nWeightedClustersMin", 0)
    kwargs.setdefault("minTRTHits", flags.InDet.Tracking.Pass.minTRTonTrk )
    acc.setPrivateTools(acc.popToolsAndMerge(InDetCosmicsScoringToolBaseCfg(flags, name = 'InDetCosmicExtenScoringTool', **kwargs)))
    return acc

def SiCombinatorialTrackFinder_xkCfg(flags, name='InDetSiComTrackFinder', **kwargs):
    acc = ComponentAccumulator()

    InDetPatternPropagator = acc.getPrimaryAndMerge(InDetPatternPropagatorCfg())
    InDetPatternUpdator = acc.getPrimaryAndMerge(InDetPatternUpdatorCfg())

    InDetRotCreatorDigital = acc.popToolsAndMerge(InDetRotCreatorDigitalCfg(flags))
    acc.addPublicTool(InDetRotCreatorDigital)

    from  InDetConfig.InDetRecToolConfig import InDetBoundaryCheckToolCfg
    InDetBoundaryCheckTool = acc.popToolsAndMerge(InDetBoundaryCheckToolCfg(flags))

    kwargs.setdefault("PropagatorTool",  InDetPatternPropagator)
    kwargs.setdefault("UpdatorTool", InDetPatternUpdator)
    kwargs.setdefault("BoundaryCheckTool", InDetBoundaryCheckTool)
    kwargs.setdefault("RIOonTrackTool", InDetRotCreatorDigital)
    kwargs.setdefault("usePixel", flags.Detector.EnablePixel) #DetFlags.haveRIO.pixel_on()
    kwargs.setdefault("useSCT", flags.Detector.EnableSCT)
    kwargs.setdefault("PixelClusterContainer", 'PixelClusters') #InDetKeys.PixelClusters()
    kwargs.setdefault("SCT_ClusterContainer", 'SCT_Clusters') # InDetKeys.SCT_Clusters()
    if "PixelSummaryTool" not in kwargs:
        from PixelConditionsTools.PixelConditionsSummaryConfig import PixelConditionsSummaryCfg
        kwargs.setdefault("PixelSummaryTool", acc.popToolsAndMerge(PixelConditionsSummaryCfg(flags)))
    if "SctSummaryTool" not in kwargs:
        from SCT_ConditionsTools.SCT_ConditionsToolsConfig import SCT_ConditionsSummaryToolCfg
        kwargs.setdefault("SctSummaryTool", acc.popToolsAndMerge(SCT_ConditionsSummaryToolCfg(flags)))

    InDetSiComTrackFinder = CompFactory.InDet.SiCombinatorialTrackFinder_xk(name=name, **kwargs)
    acc.setPrivateTools(InDetSiComTrackFinder)
    return acc

def InDetCosmicScoringTool_TRTCfg(flags, name='InDetCosmicExtenScoringTool',**kwargs) :
    acc = ComponentAccumulator()
    InDetTrackSummaryToolNoHoleSearch = acc.popToolsAndMerge(InDetTrackSummaryToolNoHoleSearchCfg(flags))

    kwargs.setdefault("minTRTHits", flags.InDet.Tracking.Pass.minSecondaryTRTonTrk)
    kwargs.setdefault("SummaryTool", InDetTrackSummaryToolNoHoleSearch)

    acc.setPrivateTools(acc.popToolsAndMerge(InDetCosmicExtenScoringToolCfg(flags,
                                                                            name = 'InDetCosmicScoringTool_TRT', **kwargs)))
    return acc

def InDetTRT_SeededScoringToolCfg(flags, name='InDetTRT_SeededScoringTool', **kwargs) :
    acc = ComponentAccumulator()

    kwargs.setdefault("useAmbigFcn", False)
    kwargs.setdefault("useTRT_AmbigFcn", True)
    kwargs.setdefault("minTRTonTrk", flags.InDet.Tracking.Pass.minSecondaryTRTonTrk)
    kwargs.setdefault("minTRTPrecisionFraction", flags.InDet.Tracking.Pass.minSecondaryTRTPrecFrac)
    kwargs.setdefault("minPt", flags.InDet.Tracking.Pass.minSecondaryPt)
    kwargs.setdefault("maxRPhiImp", flags.InDet.Tracking.Pass.maxSecondaryImpact)
    kwargs.setdefault("minSiClusters", flags.InDet.Tracking.Pass.minSecondaryClusters)
    kwargs.setdefault("maxSiHoles", flags.InDet.Tracking.Pass.maxSecondaryHoles)
    kwargs.setdefault("maxPixelHoles", flags.InDet.Tracking.Pass.maxSecondaryPixelHoles)
    kwargs.setdefault("maxSCTHoles", flags.InDet.Tracking.Pass.maxSecondarySCTHoles)
    kwargs.setdefault("maxDoubleHoles", flags.InDet.Tracking.Pass.maxSecondaryDoubleHoles)

    acc.setPrivateTools(acc.popToolsAndMerge(InDetAmbiScoringToolBaseCfg(flags, name=name, **kwargs)))
    return acc

#########################################################################################################
#TRTExtension
#########################################################################################################

def InDetAmbiScoringToolCfg(flags, name='InDetAmbiScoringTool', **kwargs) :
    acc = ComponentAccumulator()
    kwargs.setdefault("useAmbigFcn", True )
    kwargs.setdefault("useTRT_AmbigFcn", False )
    kwargs.setdefault("minTRTonTrk", 0 )
    kwargs.setdefault("minTRTPrecisionFraction", 0 )
    kwargs.setdefault("minPt", flags.InDet.Tracking.Pass.minPT )
    kwargs.setdefault("maxRPhiImp", flags.InDet.Tracking.Pass.maxPrimaryImpact )
    kwargs.setdefault("minSiClusters", flags.InDet.Tracking.Pass.minClusters )
    kwargs.setdefault("minPixel", flags.InDet.Tracking.Pass.minPixel )
    kwargs.setdefault("maxSiHoles", flags.InDet.Tracking.Pass.maxHoles )
    kwargs.setdefault("maxPixelHoles", flags.InDet.Tracking.Pass.maxPixelHoles )
    kwargs.setdefault("maxSCTHoles", flags.InDet.Tracking.Pass.maxSctHoles )
    kwargs.setdefault("maxDoubleHoles", flags.InDet.Tracking.Pass.maxDoubleHoles )
    acc.setPrivateTools(acc.popToolsAndMerge(InDetAmbiScoringToolBaseCfg(flags, name + flags.InDet.Tracking.Pass.extension, **kwargs)))
    return acc

def InDetAmbiScoringToolSiCfg(flags, name='InDetAmbiScoringToolSi', **kwargs) :
    kwargs.setdefault('DriftCircleCutTool','')
    return InDetAmbiScoringToolCfg(flags, name, **kwargs)

def InDetExtenScoringToolCfg(flags, name='InDetExtenScoringTool', **kwargs) :
    acc = ComponentAccumulator()
    if flags.InDet.trackFitterType in ['KalmanFitter', 'KalmanDNAFitter', 'ReferenceKalmanFitter']:
        kwargs.setdefault("minTRTPrecisionFraction", 0.2)
    kwargs.setdefault("minTRTonTrk", flags.InDet.Tracking.Pass.minTRTonTrk)
    kwargs.setdefault("minTRTPrecisionFraction", flags.InDet.Tracking.Pass.minTRTPrecFrac)
    acc.setPrivateTools(acc.popToolsAndMerge(InDetAmbiScoringToolCfg(flags, name = name,  **kwargs)))
    return acc

#############################################################################################
#TrackingSiPatternConfig
#############################################################################################

def InDetPixelClusterOnTrackToolDBMCfg(flags, name='InDetPixelClusterOnTrackToolDBM', **kwargs):
    kwargs.setdefault("DisableDistortions", True )
    kwargs.setdefault("applyNNcorrection", False )
    kwargs.setdefault("NNIBLcorrection", False )
    kwargs.setdefault("RunningTIDE_Ambi", False )
    kwargs.setdefault("ErrorStrategy", 0 )
    kwargs.setdefault("PositionStrategy", 0 )
    return InDetPixelClusterOnTrackToolBaseCfg(flags, name=name, **kwargs)

def InDetRotCreatorDBMCfg(flags, name='InDetRotCreatorDBM', **kwargs) :
    acc = ComponentAccumulator()
    if 'ToolPixelCluster' not in kwargs :
        pix_cluster_on_track_args = copyArgs(kwargs,['SplitClusterMapExtension','ClusterSplitProbabilityName','nameSuffix'])

        if flags.Detector.EnablePixel and flags.InDet.loadRotCreator:
            ToolPixelCluster = InDetPixelClusterOnTrackToolDBMCfg(flags, pix_cluster_on_track_args)
            kwargs.setdefault('ToolPixelCluster', ToolPixelCluster)
        else :
            ToolPixelCluster= InDetPixelClusterOnTrackToolCfg(flags, **pix_cluster_on_track_args)
            kwargs.setdefault('ToolPixelCluster', ToolPixelCluster)

    acc.setPrivateTools(acc.popToolsAndMerge(InDetRotCreatorCfg(flags, name=name, **kwargs)))
    return acc

def PRDtoTrackMapToolCfg(name='PRDtoTrackMapTool',**kwargs) :
    acc = ComponentAccumulator()
    the_name = makeName( name, kwargs)
    acc.setPrivateTools(CompFactory.Trk.PRDtoTrackMapTool( name=the_name, **kwargs))
    return acc

def InDetNNScoringToolBaseCfg(flags, name='InDetNNScoringTool', **kwargs) :
    acc = ComponentAccumulator()
    the_name=makeName(name,kwargs)

    have_calo_rois = flags.InDet.doBremRecovery and flags.InDet.doCaloSeededBrem and flags.Detector.EnableCalo
    if have_calo_rois :
        alg = acc.popToolsAndMerge(ROIInfoVecAlgCfg(flags))
        kwargs.setdefault("CaloROIInfoName", alg.WriteKey )

    from TrkConfig.AtlasExtrapolatorConfig import InDetExtrapolatorCfg
    InDetExtrapolator = acc.getPrimaryAndMerge(InDetExtrapolatorCfg(flags))
    InDetTrackSummaryTool = acc.getPrimaryAndMerge(InDetTrackSummaryToolCfg(flags))

    if 'DriftCircleCutTool' not in kwargs :
        InDetTRTDriftCircleCutForPatternReco = acc.popToolsAndMerge(InDetTRTDriftCircleCutForPatternRecoCfg(flags))
        kwargs.setdefault("DriftCircleCutTool", InDetTRTDriftCircleCutForPatternReco )

    kwargs.setdefault("nnCutConfig", "dev/TrackingCP/LRTAmbiNetwork/20200727_225401/nn-config.json" )
    kwargs.setdefault("nnCutThreshold", flags.InDet.nnCutLargeD0Threshold )
    kwargs.setdefault("Extrapolator", InDetExtrapolator )
    kwargs.setdefault("SummaryTool", InDetTrackSummaryTool )
    kwargs.setdefault("useAmbigFcn", True )
    kwargs.setdefault("useTRT_AmbigFcn", False )
    kwargs.setdefault("maxZImp", flags.InDet.Tracking.Pass.maxZImpact )
    kwargs.setdefault("maxEta", flags.InDet.Tracking.Pass.maxEta )
    kwargs.setdefault("usePixel", flags.InDet.Tracking.Pass.usePixel )
    kwargs.setdefault("useSCT", flags.InDet.Tracking.Pass.useSCT )
    kwargs.setdefault("doEmCaloSeed", have_calo_rois )

    acc.setPrivateTools(CompFactory.InDet.InDetNNScoringTool(name = the_name, **kwargs ))
    return acc

def InDetNNScoringToolCfg(flags, name='InDetNNScoringTool', **kwargs) :
    kwargs.setdefault("useAmbigFcn", True )
    kwargs.setdefault("useTRT_AmbigFcn", False )
    kwargs.setdefault("minTRTonTrk", 0 )
    kwargs.setdefault("minTRTPrecisionFraction", 0 )
    kwargs.setdefault("minPt", flags.InDet.Tracking.Pass.minPT )
    kwargs.setdefault("maxRPhiImp", flags.InDet.Tracking.Pass.maxPrimaryImpact )
    kwargs.setdefault("minSiClusters", flags.InDet.Tracking.Pass.minClusters )
    kwargs.setdefault("minPixel", flags.InDet.Tracking.Pass.minPixel )
    kwargs.setdefault("maxSiHoles", flags.InDet.Tracking.Pass.maxHoles )
    kwargs.setdefault("maxPixelHoles", flags.InDet.Tracking.Pass.maxPixelHoles )
    kwargs.setdefault("maxSCTHoles", flags.InDet.Tracking.Pass.maxSctHoles )
    kwargs.setdefault("maxDoubleHoles", flags.InDet.Tracking.Pass.maxDoubleHoles)

    return InDetNNScoringToolBaseCfg(flags, name=name+flags.InDet.Tracking.Pass.extension, **kwargs )

def InDetNNScoringToolSiCfg(flags, name='InDetNNScoringToolSi', **kwargs) :
    kwargs.setdefault('DriftCircleCutTool','')
    return InDetNNScoringToolCfg(flags,name, **kwargs)

def InDetCosmicsScoringToolCfg(flags, name='InDetCosmicsScoringTool', **kwargs) :
    return InDetCosmicsScoringToolBaseCfg(flags,
                                          name=name+flags.InDet.Tracking.Pass.extension)

def FullLinearizedTrackFactoryCfg(flags, name='TrackToVertexIPEstimator', **kwargs):
    from TrkConfig.AtlasExtrapolatorConfig import InDetExtrapolatorCfg

    acc = InDetExtrapolatorCfg(flags)
    kwargs.setdefault("Extrapolator", acc.getPrimary()) #private
    # FIXME Configure AtlasFieldCacheCondObj
    acc.setPrivateTools(CompFactory.Trk.FullLinearizedTrackFactory( **kwargs))
    return acc

def TrackToVertexIPEstimatorCfg(flags, name='TrackToVertexIPEstimator', **kwargs):
    from TrkConfig.AtlasExtrapolatorConfig import InDetExtrapolatorCfg

    # All sub-tools are currently public
    acc = FullLinearizedTrackFactoryCfg(flags, 'FullLinearizedTrkFactory')
    linearizedTrackFactory = acc.popPrivateTools()
    acc.addPublicTool(linearizedTrackFactory)
    kwargs.setdefault("LinearizedTrackFactory", linearizedTrackFactory )
    if 'Extrapolator' not in kwargs:
        extrapolator = acc.getPrimaryAndMerge( InDetExtrapolatorCfg(flags) )
        kwargs.setdefault("Extrapolator", extrapolator)
    # Think it's okay not to configure VertexUpdator
    acc.setPrivateTools(CompFactory.Trk.TrackToVertexIPEstimator( **kwargs))
    return acc
