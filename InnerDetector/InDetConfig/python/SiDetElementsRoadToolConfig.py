# Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
# Configuration of SiDetElementsRoadTool_xk package

from AthenaConfiguration.ComponentAccumulator import ComponentAccumulator
from AthenaConfiguration.ComponentFactory import CompFactory
from AthenaConfiguration.Enums import BeamType

def SiDetElementsRoadMaker_xkCfg(flags, name="InDetSiRoadMaker", **kwargs) :
    acc = ComponentAccumulator()
    #
    # --- SCT and Pixel detector elements road builder
    #

    # Create ReadCondHandle SiDetElementsLayerVectors_xk
    acc.addCondAlgo(CompFactory.InDet.SiDetElementsRoadCondAlg_xk(name = "InDet__SiDetElementsRoadCondAlg_xk"))

    from InDetConfig.TrackingCommonConfig import InDetPatternPropagatorCfg
    InDetPatternPropagator = acc.getPrimaryAndMerge(InDetPatternPropagatorCfg())

    kwargs.setdefault("PropagatorTool", InDetPatternPropagator)
    kwargs.setdefault("usePixel", flags.InDet.Tracking.ActivePass.usePixel )
    kwargs.setdefault("PixManagerLocation", 'Pixel')
    kwargs.setdefault("useSCT", flags.InDet.Tracking.ActivePass.useSCT)
    kwargs.setdefault("SCTManagerLocation", 'SCT')
    kwargs.setdefault("RoadWidth", flags.InDet.Tracking.ActivePass.roadWidth)

    InDetSiDetElementsRoadMaker = CompFactory.InDet.SiDetElementsRoadMaker_xk(name = name+flags.InDet.Tracking.ActivePass.extension, **kwargs)
    acc.setPrivateTools(InDetSiDetElementsRoadMaker)
    return acc

def SiDetElementsRoadMaker_xk_TRT_Cfg(flags, name = 'InDetTRT_SeededSiRoad', **kwargs):
    #
    # Silicon det elements road maker tool
    #
    kwargs.setdefault("RoadWidth", 50. if flags.Beam.Type is BeamType.Cosmics else 35.)
    kwargs.setdefault("MaxStep", 20.)

    return SiDetElementsRoadMaker_xkCfg(flags, name, **kwargs)

def SiDetElementsRoadMaker_xk_Trig_Cfg( flags, name="InDetTrigSiDetElementsRoadMaker", **kwargs ):
    acc = ComponentAccumulator()

    from TrigInDetConfig.TrigInDetConfig import RungeKuttaPropagatorCfg
    RungeKuttaPropagator = acc.getPrimaryAndMerge(RungeKuttaPropagatorCfg(flags))
    kwargs.setdefault("PropagatorTool", RungeKuttaPropagator)

    return SiDetElementsRoadMaker_xkCfg(flags, name, **kwargs)

def ITkSiDetElementsRoadMaker_xkCfg(flags, name="ITkSiRoadMaker", **kwargs) :
    acc = ComponentAccumulator()
    #
    # --- ITk Strip and Pixel detector elements road builder
    #

    # Create ReadCondHandle SiDetElementsLayerVectors_xk
    acc.addCondAlgo(CompFactory.InDet.SiDetElementsRoadCondAlg_xk(name = "InDet__SiDetElementsRoadCondAlg_xk",
                                                                  PixelDetEleCollKey = "ITkPixelDetectorElementCollection",
                                                                  SCTDetEleCollKey = "ITkStripDetectorElementCollection"))

    from InDetConfig.ITkRecToolConfig import ITkPatternPropagatorCfg
    ITkPatternPropagator = acc.getPrimaryAndMerge(ITkPatternPropagatorCfg(flags))

    kwargs.setdefault("PropagatorTool", ITkPatternPropagator)
    kwargs.setdefault("usePixel", flags.ITk.Tracking.ActivePass.useITkPixel )
    kwargs.setdefault("PixManagerLocation", 'ITkPixel')
    kwargs.setdefault("useSCT", flags.ITk.Tracking.ActivePass.useITkStrip)
    kwargs.setdefault("SCTManagerLocation", 'ITkStrip')
    kwargs.setdefault("RoadWidth", flags.ITk.Tracking.ActivePass.roadWidth)

    ITkSiDetElementsRoadMaker = CompFactory.InDet.SiDetElementsRoadMaker_xk(name = name+flags.ITk.Tracking.ActivePass.extension, **kwargs)
    acc.setPrivateTools(ITkSiDetElementsRoadMaker)
    return acc

