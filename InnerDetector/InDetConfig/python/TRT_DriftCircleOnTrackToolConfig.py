# Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
# Configuration of TrkNeuralNetworkUtils package
from AthenaConfiguration.ComponentAccumulator import ComponentAccumulator
from AthenaConfiguration.ComponentFactory import CompFactory
from AthenaConfiguration.Enums import BeamType

def TRT_DriftCircleOnTrackNoDriftTimeToolCfg(flags, name='TRT_DriftCircleOnTrackNoDriftTimeTool', **kwargs):
    acc = ComponentAccumulator()
    acc.setPrivateTools(CompFactory.InDet.TRT_DriftCircleOnTrackNoDriftTimeTool(name, **kwargs))
    return acc

def TRT_DriftCircleOnTrackToolCfg(flags, name='TRT_DriftCircleOnTrackTool', **kwargs):
    acc = ComponentAccumulator()
    from TrkConfig.TrkRIO_OnTrackCreatorConfig import RIO_OnTrackErrorScalingCondAlgCfg
    acc.merge(RIO_OnTrackErrorScalingCondAlgCfg(flags))
    kwargs.setdefault("TRTErrorScalingKey", '/Indet/TrkErrorScalingTRT')
    
    LuminosityOutputKey = ''
    if flags.Beam.Type is not BeamType.Cosmics:
        from LumiBlockComps.LuminosityCondAlgConfig import LuminosityCondAlgCfg
        LuminosityCondAlg = LuminosityCondAlgCfg(flags)
        acc.merge(LuminosityCondAlg)
        LuminosityOutputKey = LuminosityCondAlg.getCondAlgo('LuminosityCondAlg').LuminosityOutputKey

    kwargs.setdefault("LumiDataKey", LuminosityOutputKey)
    acc.setPrivateTools(CompFactory.InDet.TRT_DriftCircleOnTrackTool(name, **kwargs))
    return acc

def TRT_DriftCircleOnTrackUniversalToolCfg(flags, name='InDetTRT_RefitRotCreator', **kwargs):
    acc = ComponentAccumulator()

    if 'RIOonTrackToolDrift' not in kwargs:
        kwargs.setdefault("RIOonTrackToolDrift", acc.popToolsAndMerge(TRT_DriftCircleOnTrackNoDriftTimeToolCfg(flags)))
    if 'RIOonTrackToolTube' not in kwargs:
        kwargs.setdefault("RIOonTrackToolTube", acc.popToolsAndMerge(TRT_DriftCircleOnTrackNoDriftTimeToolCfg(flags)))
    kwargs.setdefault("ScaleHitUncertainty", 2.5) 

    acc.setPrivateTools(CompFactory.InDet.TRT_DriftCircleOnTrackUniversalTool(name, **kwargs))
    return acc

def TRT_DriftCircleOnTrackUniversalToolCosmicsCfg(flags, name='TRT_DriftCircleOnTrackUniversalTool', **kwargs):
    kwargs.setdefault("ScaleHitUncertainty", 2.)
    return TRT_DriftCircleOnTrackUniversalToolCfg(flags, name, **kwargs)
