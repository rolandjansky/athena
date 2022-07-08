# Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
# Configuration of SiClusterizationTool package

from AthenaConfiguration.ComponentAccumulator import ComponentAccumulator
from AthenaConfiguration.ComponentFactory import CompFactory
from AthenaConfiguration.Enums import BeamType, LHCPeriod
from IOVDbSvc.IOVDbSvcConfig import addFoldersSplitOnline

def ClusterMakerToolCfg(flags, name="InDetClusterMakerTool", **kwargs) :
    acc = ComponentAccumulator()

    # This directly needs the following Conditions data:
    # PixelChargeCalibCondData & PixelOfflineCalibData
    from PixelConditionsAlgorithms.PixelConditionsConfig import PixelChargeLUTCalibCondAlgCfg, PixelChargeCalibCondAlgCfg, PixelOfflineCalibCondAlgCfg
    if flags.GeoModel.Run is LHCPeriod.Run3:
        acc.merge(PixelChargeLUTCalibCondAlgCfg(flags))
    else:
        acc.merge(PixelChargeCalibCondAlgCfg(flags))
    acc.merge(PixelOfflineCalibCondAlgCfg(flags))

    from PixelReadoutGeometry.PixelReadoutGeometryConfig import PixelReadoutManagerCfg
    acc.merge(PixelReadoutManagerCfg(flags))

    from SiLorentzAngleTool.PixelLorentzAngleConfig import PixelLorentzAngleToolCfg
    PixelLorentzAngleTool = acc.popToolsAndMerge(PixelLorentzAngleToolCfg(flags))
    from SiLorentzAngleTool.SCT_LorentzAngleConfig import SCT_LorentzAngleToolCfg
    SCTLorentzAngleTool = acc.popToolsAndMerge( SCT_LorentzAngleToolCfg(flags) )

    kwargs.setdefault("PixelLorentzAngleTool", PixelLorentzAngleTool)
    kwargs.setdefault("SCTLorentzAngleTool", SCTLorentzAngleTool)

    InDetClusterMakerTool = CompFactory.InDet.ClusterMakerTool(name, **kwargs)
    acc.setPrivateTools(InDetClusterMakerTool)
    return acc

def ITkClusterMakerToolCfg(flags, name="ITkClusterMakerTool", **kwargs) :
    acc = ComponentAccumulator()

    from PixelConditionsAlgorithms.ITkPixelConditionsConfig import ITkPixelChargeCalibCondAlgCfg
    from PixelReadoutGeometry.PixelReadoutGeometryConfig import ITkPixelReadoutManagerCfg

    # This directly needs the following Conditions data:
    # PixelModuleData & PixelChargeCalibCondData
    acc.merge(ITkPixelChargeCalibCondAlgCfg(flags))
    acc.merge(ITkPixelReadoutManagerCfg(flags))

    from SiLorentzAngleTool.ITkPixelLorentzAngleConfig import ITkPixelLorentzAngleToolCfg
    ITkPixelLorentzAngleTool = acc.popToolsAndMerge(ITkPixelLorentzAngleToolCfg(flags))
    from SiLorentzAngleTool.ITkStripLorentzAngleConfig import ITkStripLorentzAngleToolCfg
    ITkStripLorentzAngleTool = acc.popToolsAndMerge(ITkStripLorentzAngleToolCfg(flags))

    kwargs.setdefault("PixelChargeCalibCondData", "ITkPixelChargeCalibCondData")
    kwargs.setdefault("PixelReadoutManager",acc.getService("ITkPixelReadoutManager"))
    kwargs.setdefault("PixelLorentzAngleTool", ITkPixelLorentzAngleTool)
    kwargs.setdefault("SCTLorentzAngleTool", ITkStripLorentzAngleTool)
    kwargs.setdefault("PixelOfflineCalibData", "")

    ITkClusterMakerTool = CompFactory.InDet.ClusterMakerTool(name, **kwargs)
    acc.setPrivateTools(ITkClusterMakerTool)
    return acc

def MergedPixelsToolCfg(flags, name="InDetMergedPixelsTool", **kwargs) :
    acc = ComponentAccumulator()
    # --- now load the framework for the clustering
    kwargs.setdefault("globalPosAlg", acc.popToolsAndMerge(ClusterMakerToolCfg(flags)) )

    # PixelClusteringToolBase uses PixelConditionsSummaryTool
    from PixelConditionsTools.PixelConditionsSummaryConfig import PixelConditionsSummaryCfg
    kwargs.setdefault("PixelConditionsSummaryTool", acc.popToolsAndMerge(PixelConditionsSummaryCfg(flags)) )

    if "PixelDetElStatus" not in kwargs :
         from PixelConditionsAlgorithms.PixelConditionsConfig import PixelDetectorElementStatusAlgCfg
         acc.merge( PixelDetectorElementStatusAlgCfg(flags) )
         kwargs.setdefault("PixelDetElStatus", "PixelDetectorElementStatus")

    # Enable duplcated RDO check for data15 because duplication mechanism was used.
    if len(flags.Input.ProjectName)>=6 and flags.Input.ProjectName[:6]=="data15":
        kwargs.setdefault("CheckDuplicatedRDO", True )

    InDetMergedPixelsTool = CompFactory.InDet.MergedPixelsTool(name, **kwargs)
    acc.setPrivateTools(InDetMergedPixelsTool)
    return acc

def TrigMergedPixelsToolCfg(flags, name="InDetMergedPixelsTool", **kwargs) :
    # @TODO if the configuration is different maybe it should also get a new name ?
    #       for the time being to not introduce naming changes, the name of this private
    #       tool is unchanged.
    kwargs.setdefault("PixelDetElStatus","")
    return MergedPixelsToolCfg(flags, name=name, **kwargs)

def ITkMergedPixelsToolCfg(flags, name="ITkMergedPixelsTool", **kwargs) :
    acc = ComponentAccumulator()
    # --- now load the framework for the clustering
    kwargs.setdefault("globalPosAlg", acc.popToolsAndMerge(ITkClusterMakerToolCfg(flags)))

    # PixelClusteringToolBase uses PixelConditionsSummaryTool
    from PixelConditionsTools.ITkPixelConditionsSummaryConfig import ITkPixelConditionsSummaryCfg
    ITkPixelConditionsSummary = acc.popToolsAndMerge(ITkPixelConditionsSummaryCfg(flags))
    kwargs.setdefault("PixelConditionsSummaryTool", ITkPixelConditionsSummary)
    kwargs.setdefault("PixelDetEleCollKey","ITkPixelDetectorElementCollection")

    ITkMergedPixelsTool = CompFactory.InDet.MergedPixelsTool(name, **kwargs)
    acc.setPrivateTools(ITkMergedPixelsTool)
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

    if 'NetworkToHistoTool' not in kwargs:
        from TrkConfig.TrkNeuralNetworkUtilsConfig import NeuralNetworkToHistoToolCfg
        NeuralNetworkToHistoTool = acc.popToolsAndMerge(NeuralNetworkToHistoToolCfg(flags))
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
    acc.merge(addFoldersSplitOnline(flags, "PIXEL", "/PIXEL/Onl/PixelClustering/PixelNNCalibJSON", "/PIXEL/PixelClustering/PixelNNCalibJSON", className="CondAttrListCollection"))
    # What we'll store it as
    kwargs.setdefault("WriteKey", 'PixelClusterNNJSON')

    # Set up the algorithm
    acc.addCondAlgo(CompFactory.InDet.LWTNNCondAlg(kwargs.pop("name", "LWTNNCondAlg"), **kwargs))
    return acc

def NnClusterizationFactoryCfg(flags, name="NnClusterizationFactory", **kwargs):
    acc = ComponentAccumulator()

    from PixelConditionsAlgorithms.PixelConditionsConfig import PixelChargeLUTCalibCondAlgCfg, PixelChargeCalibCondAlgCfg
    if flags.GeoModel.Run is LHCPeriod.Run3:
        acc.merge(PixelChargeLUTCalibCondAlgCfg(flags))
    else:
        acc.merge(PixelChargeCalibCondAlgCfg(flags))

    if "PixelLorentzAngleTool" not in kwargs:
        from SiLorentzAngleTool.PixelLorentzAngleConfig import PixelLorentzAngleToolCfg
        PixelLorentzAngleTool = acc.popToolsAndMerge(PixelLorentzAngleToolCfg(flags))
        kwargs.setdefault("PixelLorentzAngleTool", PixelLorentzAngleTool)

    if flags.GeoModel.Run is LHCPeriod.Run1:
        acc.merge(PixelClusterNnCondAlgCfg(flags, name="PixelClusterNnCondAlg", GetInputsInfo=True))
        acc.merge(PixelClusterNnWithTrackCondAlgCfg(flags, name="PixelClusterNnWithTrackCondAlg", GetInputsInfo=True))
    else:
        acc.merge(LWTNNCondAlgCfg(flags, name="LWTNNCondAlg"))

    kwargs.setdefault("doRunI", flags.GeoModel.Run is LHCPeriod.Run1)
    kwargs.setdefault("useToT", False)
    kwargs.setdefault("useRecenteringNNWithoutTracks", flags.GeoModel.Run is LHCPeriod.Run1)
    kwargs.setdefault("useRecenteringNNWithTracks", False)
    kwargs.setdefault("correctLorShiftBarrelWithoutTracks", 0)
    kwargs.setdefault("correctLorShiftBarrelWithTracks", 0.030 if flags.GeoModel.Run is LHCPeriod.Run1 else 0.000)
    kwargs.setdefault("useTTrainedNetworks", flags.GeoModel.Run is LHCPeriod.Run1)
    kwargs.setdefault("NnCollectionReadKey", "PixelClusterNN" if flags.GeoModel.Run is LHCPeriod.Run1 else "")
    kwargs.setdefault("NnCollectionWithTrackReadKey", "PixelClusterNNWithTrack" if flags.GeoModel.Run is LHCPeriod.Run1 else "")
    kwargs.setdefault("NnCollectionJSONReadKey", "" if flags.GeoModel.Run is LHCPeriod.Run1 else "PixelClusterNNJSON")

    acc.setPrivateTools(CompFactory.InDet.NnClusterizationFactory(name, **kwargs))
    return acc

def TrigNnClusterizationFactoryCfg(flags, name="TrigNnClusterizationFactory", **kwargs):
    acc = ComponentAccumulator()

    from PixelConditionsAlgorithms.PixelConditionsConfig import PixelChargeLUTCalibCondAlgCfg, PixelChargeCalibCondAlgCfg
    if flags.GeoModel.Run is LHCPeriod.Run3:
        acc.merge(PixelChargeLUTCalibCondAlgCfg(flags))
    else:
        acc.merge(PixelChargeCalibCondAlgCfg(flags))

    acc.merge(PixelClusterNnCondAlgCfg(flags, name="PixelClusterNnCondAlg", GetInputsInfo=True))
    acc.merge(PixelClusterNnWithTrackCondAlgCfg(flags, name="PixelClusterNnWithTrackCondAlg", GetInputsInfo=True))

    if "PixelLorentzAngleTool" not in kwargs:
        from SiLorentzAngleTool.PixelLorentzAngleConfig import PixelLorentzAngleToolCfg
        PixelLorentzAngleTool = acc.popToolsAndMerge(PixelLorentzAngleToolCfg(flags))
        kwargs.setdefault("PixelLorentzAngleTool", PixelLorentzAngleTool)

    kwargs.setdefault("useToT", False)
    kwargs.setdefault("NnCollectionReadKey", "PixelClusterNN")
    kwargs.setdefault("NnCollectionWithTrackReadKey", "PixelClusterNNWithTrack")

    acc.setPrivateTools(CompFactory.InDet.NnClusterizationFactory(name, **kwargs))
    return acc

def NnPixelClusterSplitProbToolCfg(flags, name="NnPixelClusterSplitProbTool", **kwargs):
    acc = ComponentAccumulator()

    # --- new NN prob tool
    MultiplicityContent = [1 , 1 , 1]
    NnClusterizationFactory = acc.popToolsAndMerge(NnClusterizationFactoryCfg(flags))
    kwargs.setdefault("PriorMultiplicityContent", MultiplicityContent)
    kwargs.setdefault("NnClusterizationFactory", NnClusterizationFactory)
    kwargs.setdefault("useBeamSpotInfo", flags.InDet.Tracking.useBeamSpotInfoNN)

    NnPixelClusterSplitProbTool = CompFactory.InDet.NnPixelClusterSplitProbTool(name,**kwargs)

    acc.setPrivateTools(NnPixelClusterSplitProbTool)
    return acc

def PixelGangedAmbiguitiesFinderCfg(flags, name="InDetPixelGangedAmbiguitiesFinder", **kwargs) :
    from PixelGeoModel.PixelGeoModelConfig import PixelReadoutGeometryCfg
    acc = PixelReadoutGeometryCfg(flags)
    InDetPixelGangedAmbiguitiesFinder = CompFactory.InDet.PixelGangedAmbiguitiesFinder(name, **kwargs)
    acc.setPrivateTools( InDetPixelGangedAmbiguitiesFinder )
    return acc

def ITkPixelGangedAmbiguitiesFinderCfg(flags, name="ITkPixelGangedAmbiguitiesFinder", **kwargs) :
    from PixelGeoModelXml.ITkPixelGeoModelConfig import ITkPixelReadoutGeometryCfg
    acc = ITkPixelReadoutGeometryCfg(flags)
    kwargs.setdefault("PixelDetEleCollKey", "ITkPixelDetectorElementCollection")
    ITkPixelGangedAmbiguitiesFinder = CompFactory.InDet.PixelGangedAmbiguitiesFinder(name, **kwargs)
    acc.setPrivateTools( ITkPixelGangedAmbiguitiesFinder )
    return acc

def SCT_ClusteringToolCfg(flags, name="InDetSCT_ClusteringTool", **kwargs):
    from SCT_GeoModel.SCT_GeoModelConfig import SCT_ReadoutGeometryCfg
    acc = SCT_ReadoutGeometryCfg(flags) # To produce SCT_DetectorElementCollection
    from SCT_ConditionsTools.SCT_ConditionsToolsConfig import SCT_ConditionsSummaryToolCfg
    InDetSCT_ConditionsSummaryToolWithoutFlagged = acc.popToolsAndMerge(SCT_ConditionsSummaryToolCfg(flags, withFlaggedCondTool=False))
    InDetClusterMakerTool = acc.popToolsAndMerge(ClusterMakerToolCfg(flags))
    from SiLorentzAngleTool.SCT_LorentzAngleConfig import SCT_LorentzAngleToolCfg
    SCTLorentzAngleTool = acc.popToolsAndMerge(SCT_LorentzAngleToolCfg(flags))

    kwargs.setdefault("conditionsTool", InDetSCT_ConditionsSummaryToolWithoutFlagged)
    kwargs.setdefault("SCTDetElStatus","SCTDetectorElementStatusWithoutFlagged")
    kwargs.setdefault("globalPosAlg", InDetClusterMakerTool)
    kwargs.setdefault("LorentzAngleTool", SCTLorentzAngleTool)
    if flags.InDet.selectSCTIntimeHits :
        coll_25ns = flags.Beam.BunchSpacing<=25 and flags.Beam.Type is BeamType.Collisions
        kwargs.setdefault("timeBins", "01X" if coll_25ns else "X1X")

    InDetSCT_ClusteringTool = CompFactory.InDet.SCT_ClusteringTool(name, **kwargs)
    acc.setPrivateTools(InDetSCT_ClusteringTool)
    return acc

def Trig_SCT_ClusteringToolCfg(flags, name="Trig_SCT_ClusteringTool", **kwargs):
    from SCT_GeoModel.SCT_GeoModelConfig import SCT_ReadoutGeometryCfg
    acc = SCT_ReadoutGeometryCfg(flags) # To produce SCT_DetectorElementCollection
    from SCT_ConditionsTools.SCT_ConditionsToolsConfig import SCT_ConditionsSummaryToolCfg
    InDetSCT_ConditionsSummaryToolWithoutFlagged = acc.popToolsAndMerge(SCT_ConditionsSummaryToolCfg(flags, withFlaggedCondTool=False, withTdaqTool = False))
    InDetClusterMakerTool = acc.popToolsAndMerge(ClusterMakerToolCfg(flags))
    from SiLorentzAngleTool.SCT_LorentzAngleConfig import SCT_LorentzAngleToolCfg
    SCTLorentzAngleTool = acc.popToolsAndMerge(SCT_LorentzAngleToolCfg(flags))

    kwargs.setdefault("conditionsTool", InDetSCT_ConditionsSummaryToolWithoutFlagged)
    kwargs.setdefault("globalPosAlg", InDetClusterMakerTool)
    kwargs.setdefault("LorentzAngleTool", SCTLorentzAngleTool)

    InDetSCT_ClusteringTool = CompFactory.InDet.SCT_ClusteringTool(name, **kwargs)
    acc.setPrivateTools(InDetSCT_ClusteringTool)
    return acc

def ITKStrip_SCT_ClusteringToolCfg(flags, name="ITkStripClusteringTool", **kwargs):
    from StripGeoModelXml.ITkStripGeoModelConfig import ITkStripReadoutGeometryCfg
    acc = ITkStripReadoutGeometryCfg(flags) # To produce ITkStripDetectorElementCollection
    kwargs.setdefault("SCTDetEleCollKey", "ITkStripDetectorElementCollection")

    from SCT_ConditionsTools.ITkStripConditionsToolsConfig import ITkStripConditionsSummaryToolCfg
    ITkStripConditionsSummaryTool = acc.popToolsAndMerge(ITkStripConditionsSummaryToolCfg(flags))
    ITkClusterMakerTool = acc.popToolsAndMerge(ITkClusterMakerToolCfg(flags))
    from SiLorentzAngleTool.ITkStripLorentzAngleConfig import ITkStripLorentzAngleToolCfg
    ITkStripLorentzAngleTool = acc.popToolsAndMerge( ITkStripLorentzAngleToolCfg(flags) )

    kwargs.setdefault("conditionsTool", ITkStripConditionsSummaryTool)
    kwargs.setdefault("globalPosAlg", ITkClusterMakerTool)
    kwargs.setdefault("LorentzAngleTool", ITkStripLorentzAngleTool)
    kwargs.setdefault("useRowInformation", True) # ITk-specific clustering
    if flags.ITk.selectStripIntimeHits :
        coll_25ns = flags.Beam.BunchSpacing<=25 and flags.Beam.Type is BeamType.Collisions
        kwargs.setdefault("timeBins", "01X" if coll_25ns else "X1X")

    InDetSCT_ClusteringTool = CompFactory.InDet.SCT_ClusteringTool(name, **kwargs)
    acc.setPrivateTools(InDetSCT_ClusteringTool)
    return acc

def ITkTruthClusterizationFactoryCfg(flags, name = 'ITkTruthClusterizationFactory', **kwargs):
    acc = ComponentAccumulator()
    kwargs.setdefault("InputSDOMap", 'ITkPixelSDO_Map')
    ITkTruthClusterizationFactory = CompFactory.InDet.TruthClusterizationFactory( name, **kwargs )
    acc.setPrivateTools(ITkTruthClusterizationFactory)
    return acc

def ITkTruthPixelClusterSplitProbToolCfg(flags, name="ITkTruthPixelClusterSplitProbTool", **kwargs):
    acc = ComponentAccumulator()

    # --- new NN prob tool
    MultiplicityContent = [1 , 1 , 1]
    ITkTruthClusterizationFactory = acc.popToolsAndMerge(ITkTruthClusterizationFactoryCfg(flags))

    kwargs.setdefault("PriorMultiplicityContent", MultiplicityContent)
    kwargs.setdefault("NnClusterizationFactory", ITkTruthClusterizationFactory) #Truth-based for ITk for now

    ITkTruthPixelClusterSplitProbTool = CompFactory.InDet.TruthPixelClusterSplitProbTool(name,**kwargs) #Truth-based for ITk for now

    acc.setPrivateTools(ITkTruthPixelClusterSplitProbTool)
    return acc

