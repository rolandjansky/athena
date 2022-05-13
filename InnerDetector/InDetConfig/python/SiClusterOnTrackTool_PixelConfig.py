# Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
# Configuration of pixel tools of SiClusterOnTrackTool package
from AthenaConfiguration.ComponentAccumulator import ComponentAccumulator
from AthenaConfiguration.ComponentFactory import CompFactory
from AthenaConfiguration.Enums import BeamType

#############################################
### InDet PixelClusterOnTrackTool offline ###
#############################################

def InDetPixelClusterOnTrackToolBaseCfg(flags, name="PixelClusterOnTrackTool", **kwargs):
    acc = ComponentAccumulator()

    from PixelConditionsAlgorithms.PixelConditionsConfig import PixelDistortionAlgCfg, PixelOfflineCalibCondAlgCfg
    acc.merge(PixelOfflineCalibCondAlgCfg(flags))
    if not flags.InDet.Tracking.doDBMstandalone:
        acc.merge(PixelDistortionAlgCfg(flags))

    if 'LorentzAngleTool' not in kwargs:
        from SiLorentzAngleTool.PixelLorentzAngleConfig import PixelLorentzAngleToolCfg
        PixelLorentzAngleTool = acc.popToolsAndMerge(PixelLorentzAngleToolCfg(flags))
        kwargs.setdefault("LorentzAngleTool", PixelLorentzAngleTool )

    if flags.Beam.Type is BeamType.Cosmics or flags.InDet.Tracking.doDBMstandalone:
        kwargs.setdefault("ErrorStrategy", 0)
        kwargs.setdefault("PositionStrategy", 0)

    kwargs.setdefault("DisableDistortions", flags.InDet.Tracking.doDBMstandalone)
    kwargs.setdefault("applyNNcorrection", flags.InDet.Tracking.doPixelClusterSplitting and flags.InDet.Tracking.pixelClusterSplittingType == "NeuralNet")
    kwargs.setdefault("NNIBLcorrection", flags.InDet.Tracking.doPixelClusterSplitting and flags.InDet.Tracking.pixelClusterSplittingType == "NeuralNet")
    split_cluster_map_extension = flags.InDet.Tracking.ActivePass.extension if flags.InDet.Tracking.ActivePass.useTIDE_Ambi else ""
    kwargs.setdefault("SplitClusterAmbiguityMap", f"SplitClusterAmbiguityMap{split_cluster_map_extension}")
    kwargs.setdefault("RunningTIDE_Ambi", flags.InDet.Tracking.doTIDE_Ambi)

    acc.setPrivateTools(CompFactory.InDet.PixelClusterOnTrackTool(name, **kwargs))
    return acc

def InDetPixelClusterOnTrackToolDigitalCfg(flags, name="InDetPixelClusterOnTrackToolDigital", **kwargs):
    acc = ComponentAccumulator()

    kwargs.setdefault("SplitClusterAmbiguityMap", "")

    if "LorentzAngleTool" not in kwargs:
        from SiLorentzAngleTool.PixelLorentzAngleConfig import PixelLorentzAngleToolCfg
        PixelLorentzAngleTool = acc.popToolsAndMerge(PixelLorentzAngleToolCfg(flags))
        kwargs.setdefault("LorentzAngleTool", PixelLorentzAngleTool)

    if flags.InDet.Tracking.doDigitalROTCreation:
        kwargs.setdefault("applyNNcorrection", False)
        kwargs.setdefault("NNIBLcorrection", False)
        kwargs.setdefault("ErrorStrategy", 2)
        kwargs.setdefault("PositionStrategy", 1)

    acc.setPrivateTools(acc.popToolsAndMerge(InDetPixelClusterOnTrackToolBaseCfg(flags, name, **kwargs)))
    return acc

def InDetPixelClusterOnTrackToolNNSplittingCfg(flags, name="InDetPixelClusterOnTrackToolNNSplitting", **kwargs):
    acc = ComponentAccumulator()

    if flags.InDet.Tracking.doPixelClusterSplitting and flags.InDet.Tracking.pixelClusterSplittingType == "NeuralNet" and "NnClusterizationFactory" not in kwargs:
        from InDetConfig.SiClusterizationToolConfig import NnClusterizationFactoryCfg
        kwargs.setdefault("NnClusterizationFactory", acc.popToolsAndMerge(NnClusterizationFactoryCfg(flags)))

    acc.setPrivateTools(acc.popToolsAndMerge(InDetPixelClusterOnTrackToolBaseCfg(flags, name, **kwargs)))
    return acc

def InDetPixelClusterOnTrackToolCfg(flags, name="InDetPixelClusterOnTrackTool", **kwargs):
    acc = ComponentAccumulator()

    if "LorentzAngleTool" not in kwargs:
        from SiLorentzAngleTool.PixelLorentzAngleConfig import PixelLorentzAngleToolCfg
        kwargs.setdefault("LorentzAngleTool", acc.popToolsAndMerge(PixelLorentzAngleToolCfg(flags)))

    if flags.InDet.Tracking.doDigitalROTCreation:
        PixelClusterOnTrackTool = acc.popToolsAndMerge(InDetPixelClusterOnTrackToolDigitalCfg(flags, name, **kwargs))
    else:
        PixelClusterOnTrackTool = acc.popToolsAndMerge(InDetPixelClusterOnTrackToolNNSplittingCfg(flags, name, **kwargs))

    acc.setPrivateTools(PixelClusterOnTrackTool)
    return acc

def InDetBroadPixelClusterOnTrackToolCfg(flags, name='InDetBroadPixelClusterOnTrackTool', **kwargs):
    kwargs.setdefault("ErrorStrategy", 0)
    return InDetPixelClusterOnTrackToolCfg(flags, name, **kwargs)

def InDetPixelClusterOnTrackToolDBMCfg(flags, name='InDetPixelClusterOnTrackToolDBM', **kwargs):
    kwargs.setdefault("DisableDistortions", True )
    kwargs.setdefault("applyNNcorrection", False )
    kwargs.setdefault("NNIBLcorrection", False )
    kwargs.setdefault("RunningTIDE_Ambi", False )
    kwargs.setdefault("ErrorStrategy", 0 )
    kwargs.setdefault("PositionStrategy", 0 )
    return InDetPixelClusterOnTrackToolBaseCfg(flags, name, **kwargs)

#############################################
### InDet PixelClusterOnTrackTool trigger ###
#############################################

def TrigPixelClusterOnTrackToolBaseCfg(flags, name="InDetTrigPixelClusterOnTrackTool", **kwargs):
    acc = ComponentAccumulator()

    from PixelConditionsAlgorithms.PixelConditionsConfig import PixelDistortionAlgCfg, PixelOfflineCalibCondAlgCfg
    acc.merge(PixelOfflineCalibCondAlgCfg(flags))
    acc.merge(PixelDistortionAlgCfg(flags))

    if 'LorentzAngleTool' not in kwargs:
        from SiLorentzAngleTool.PixelLorentzAngleConfig import PixelLorentzAngleToolCfg
        PixelLorentzAngleTool = acc.popToolsAndMerge(PixelLorentzAngleToolCfg(flags))
        kwargs.setdefault("LorentzAngleTool", PixelLorentzAngleTool )

    if 'NnClusterizationFactory' not in kwargs:
        from InDetConfig.SiClusterizationToolConfig import TrigNnClusterizationFactoryCfg
        nnTool = acc.popToolsAndMerge(TrigNnClusterizationFactoryCfg(flags))
        kwargs.setdefault("NnClusterizationFactory", nnTool)

    kwargs.setdefault("ErrorStrategy", 2)
    kwargs.setdefault("SplitClusterAmbiguityMap", "TrigPixelClusterAmbiguitiesMap")

    acc.setPrivateTools(CompFactory.InDet.PixelClusterOnTrackTool(name, **kwargs))
    return acc

###########################################
### ITk PixelClusterOnTrackTool offline ###
###########################################

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

def ITkPixelClusterOnTrackToolTruthSplittingCfg(flags, name='ITkPixelClusterOnTrackToolTruthSplitting', **kwargs):
    acc = ComponentAccumulator()

    if flags.ITk.Tracking.doPixelClusterSplitting and flags.ITk.Tracking.pixelClusterSplittingType == "Truth":
        if 'NnClusterizationFactory' not in kwargs:
            from InDetConfig.SiClusterizationToolConfig import ITkTruthClusterizationFactoryCfg
            ITkTruthClusterizationFactory = acc.popToolsAndMerge(ITkTruthClusterizationFactoryCfg(flags))
            kwargs.setdefault("NnClusterizationFactory", ITkTruthClusterizationFactory) #Truth-based for ITk for now

    ITkPixelClusterOnTrackTool = acc.popToolsAndMerge(ITkPixelClusterOnTrackToolBaseCfg(flags, name, **kwargs))
    acc.setPrivateTools(ITkPixelClusterOnTrackTool)
    return acc

def ITkPixelClusterOnTrackToolCfg(flags, name='ITkPixelClusterOnTrackTool', **kwargs):
    acc = ComponentAccumulator()

    if 'LorentzAngleTool' not in kwargs:
        from SiLorentzAngleTool.ITkPixelLorentzAngleConfig import ITkPixelLorentzAngleToolCfg
        ITkPixelLorentzAngleTool = acc.popToolsAndMerge(ITkPixelLorentzAngleToolCfg(flags))
        kwargs.setdefault("LorentzAngleTool", ITkPixelLorentzAngleTool )

    if flags.ITk.Tracking.doDigitalClustering:
        kwargs.setdefault("PositionStrategy", 0)
        kwargs.setdefault("ErrorStrategy", 1)

    ITkPixelClusterOnTrackTool = acc.popToolsAndMerge(ITkPixelClusterOnTrackToolTruthSplittingCfg(flags, name, **kwargs))
    acc.setPrivateTools(ITkPixelClusterOnTrackTool)
    return acc

def ITkBroadPixelClusterOnTrackToolCfg(flags, name='ITkBroadPixelClusterOnTrackTool', **kwargs):
    kwargs.setdefault("ErrorStrategy", 0)
    return ITkPixelClusterOnTrackToolCfg(flags, name, **kwargs)
