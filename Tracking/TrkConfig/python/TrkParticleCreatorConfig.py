# Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
# Configuration of TrkParticleCreator package
from AthenaConfiguration.ComponentAccumulator import ComponentAccumulator
from AthenaConfiguration.ComponentFactory import CompFactory
from AthenaConfiguration.Enums import BeamType, LHCPeriod

####################################
#####        InDet/ITk         #####
####################################

def TrackParticleCreatorToolCfg(flags, name="InDetxAODParticleCreatorTool", **kwargs):

    if flags.Detector.GeometryITk:
        name = name.replace("InDet", "ITk")
        return ITkTrackParticleCreatorToolCfg(flags, name, **kwargs)

    result = ComponentAccumulator()
    if "TrackToVertex" not in kwargs:
        from InDetConfig.TrackRecoConfig import TrackToVertexCfg
        kwargs.setdefault("TrackToVertex", result.popToolsAndMerge(TrackToVertexCfg(flags)))
    if "TrackSummaryTool" not in kwargs:
        from TrkConfig.TrkTrackSummaryToolConfig import InDetTrackSummaryToolSharedHitsCfg
        TrackSummaryTool = result.popToolsAndMerge(InDetTrackSummaryToolSharedHitsCfg(flags))
        result.addPublicTool(TrackSummaryTool)
        kwargs.setdefault("TrackSummaryTool", TrackSummaryTool)

    if "TRT_ElectronPidTool" not in kwargs :
        from InDetConfig.TRT_ElectronPidToolsConfig import TRT_ElectronPidToolCfg
        kwargs.setdefault("TRT_ElectronPidTool", result.popToolsAndMerge(TRT_ElectronPidToolCfg(flags, name="InDetTRT_ElectronPidTool")))

    if 'PixelToTPIDTool' not in kwargs :
        from InDetConfig.PixelToTPIDToolConfig import PixelToTPIDToolCfg
        kwargs.setdefault("PixelToTPIDTool", result.popToolsAndMerge(PixelToTPIDToolCfg(flags)))

    if 'TestPixelLayerTool' not in kwargs and flags.Detector.EnablePixel:
        from InDetConfig.InDetTestPixelLayerConfig import InDetTestPixelLayerToolInnerCfg
        InDetTestPixelLayerTool = result.popToolsAndMerge(InDetTestPixelLayerToolInnerCfg(flags))
        kwargs.setdefault("TestPixelLayerTool", InDetTestPixelLayerTool)

    kwargs.setdefault("ComputeAdditionalInfo", True)
    kwargs.setdefault("BadClusterID", 3) # Select the mode to identify suspicous pixel cluster
    kwargs.setdefault("KeepParameters", True)
    kwargs.setdefault("KeepFirstParameters", False)
    # need to treat Vertex specifically because at the time of
    # the track particle creation the primary vertex does not yet exist.
    # The problem is solved by first creating track particles wrt. the beam line
    # and correcting the parameters after the vertex finding.
    kwargs.setdefault("PerigeeExpression", "BeamLine" if flags.InDet.Tracking.perigeeExpression=="Vertex"
                      else flags.InDet.Tracking.perigeeExpression)
    result.setPrivateTools(CompFactory.Trk.TrackParticleCreatorTool(name, **kwargs))
    return result

def TrackParticleCreatorToolPIDCheckCfg(flags, name="InDetxAODParticleCreatorTool", **kwargs):
    # Used only through tracking passes, where ActivePass flags are defined
    if not flags.InDet.Tracking.ActivePass.RunTRTPID:
        kwargs.setdefault("TRT_ElectronPidTool", None)
    if not flags.InDet.Tracking.ActivePass.RunPixelPID:
        kwargs.setdefault("PixelToTPIDTool", None)
        kwargs.setdefault("TestPixelLayerTool", None)

    # have to create special public instance depending on PID tool configuration
    if name=="InDetxAODParticleCreatorTool" :
        pixel_pid = flags.InDet.Tracking.ActivePass.RunPixelPID
        trt_pid = flags.InDet.Tracking.ActivePass.RunTRTPID
        if not trt_pid and not pixel_pid :
            name  += "NoPID"
        elif not trt_pid :
            name += "NoTRTPID"
        elif not pixel_pid :
            name  += "NoPixPID"

    return TrackParticleCreatorToolCfg(flags, name, **kwargs)

def TrackParticleCreatorToolNoPIDCfg(flags, name="InDetxAODParticleCreatorToolNoPID", **kwargs):
    kwargs.setdefault("TRT_ElectronPidTool", None)
    kwargs.setdefault("PixelToTPIDTool", None)
    kwargs.setdefault("TestPixelLayerTool", None)
    return TrackParticleCreatorToolCfg(flags, name, **kwargs)

def InDetTrigParticleCreatorToolFTFCfg(flags, name="InDetTrigParticleCreatorToolFTF", **kwargs):
    result = ComponentAccumulator()

    if "TrackToVertex" not in kwargs:
        from TrigInDetConfig.TrigInDetConfig import TrigTrackToVertexCfg
        kwargs.setdefault("TrackToVertex", result.popToolsAndMerge(TrigTrackToVertexCfg(flags)))

    if "TrackSummaryTool" not in kwargs:
        from TrkConfig.TrkTrackSummaryToolConfig import InDetTrigTrackSummaryToolCfg
        TrackSummaryTool = result.popToolsAndMerge( InDetTrigTrackSummaryToolCfg(flags, name="InDetTrigFastTrackSummaryTool") )
        result.addPublicTool(TrackSummaryTool)
        kwargs.setdefault("TrackSummaryTool", TrackSummaryTool)

    kwargs.setdefault("TestPixelLayerTool", None)
    kwargs.setdefault("KeepParameters", True)
    kwargs.setdefault("ComputeAdditionalInfo", True)

    result.setPrivateTools(CompFactory.Trk.TrackParticleCreatorTool(name, **kwargs))
    return result

def ITkTrackParticleCreatorToolCfg(flags, name="ITkTrackParticleCreatorTool", **kwargs):
    result = ComponentAccumulator()

    if "TrackToVertex" not in kwargs:
        from InDetConfig.TrackRecoConfig import TrackToVertexCfg
        kwargs.setdefault("TrackToVertex", result.popToolsAndMerge(TrackToVertexCfg(flags)))
    if "TrackSummaryTool" not in kwargs:
        from TrkConfig.TrkTrackSummaryToolConfig import ITkTrackSummaryToolSharedHitsCfg
        TrackSummaryTool = result.popToolsAndMerge(ITkTrackSummaryToolSharedHitsCfg(flags))
        result.addPublicTool(TrackSummaryTool)
        kwargs.setdefault("TrackSummaryTool", TrackSummaryTool)

    if "TestPixelLayerTool" not in kwargs and flags.Detector.EnableITkPixel:
        from InDetConfig.InDetTestPixelLayerConfig import ITkTestPixelLayerToolInnerCfg
        ITkTestPixelLayerTool = result.popToolsAndMerge(ITkTestPixelLayerToolInnerCfg(flags))
        kwargs.setdefault("TestPixelLayerTool", ITkTestPixelLayerTool)

    kwargs.setdefault("ComputeAdditionalInfo", True)
    kwargs.setdefault("BadClusterID", 3) # Select the mode to identify suspicous pixel cluster
    kwargs.setdefault("KeepParameters", True)
    kwargs.setdefault("KeepFirstParameters", False)
    # need to treat Vertex specifically because at the time of
    # the track particle creation the primary vertex does not yet exist.
    # The problem is solved by first creating track particles wrt. the beam line
    # and correcting the parameters after the vertex finding.
    kwargs.setdefault("PerigeeExpression", "BeamLine" if flags.ITk.Tracking.perigeeExpression=="Vertex"
                      else flags.ITk.Tracking.perigeeExpression)
    kwargs.setdefault("IBLParameterSvc", "")
    kwargs.setdefault("DoITk", True)

    result.setPrivateTools(CompFactory.Trk.TrackParticleCreatorTool(name, **kwargs))
    return result


####################################
#####         egamma           #####
####################################

def GSFBuildInDetParticleCreatorToolCfg(flags, name="GSFBuildInDetParticleCreatorTool", **kwargs):
    result = ComponentAccumulator()

    if "TrackToVertex" not in kwargs:
        from InDetConfig.TrackRecoConfig import TrackToVertexCfg
        kwargs.setdefault("TrackToVertex", result.popToolsAndMerge(TrackToVertexCfg(flags)))

    if "TrackSummaryTool" not in kwargs:
        from TrkConfig.TrkTrackSummaryToolConfig import GSFTrackSummaryToolCfg
        TrackSummaryTool = result.popToolsAndMerge(GSFTrackSummaryToolCfg(flags))
        result.addPublicTool(TrackSummaryTool)
        kwargs.setdefault("TrackSummaryTool", TrackSummaryTool)

    if flags.GeoModel.Run < LHCPeriod.Run4 and "PixelToTPIDTool" not in kwargs:
        kwargs.setdefault("PixelToTPIDTool", CompFactory.InDet.PixelToTPIDTool(name="GSFBuildPixelToTPIDTool"))

    if flags.Detector.EnableTRT and "TRT_ElectronPidTool" not in kwargs :
        from InDetConfig.TRT_ElectronPidToolsConfig import GSFBuildTRT_ElectronPidToolCfg
        kwargs.setdefault("TRT_ElectronPidTool", result.popToolsAndMerge(GSFBuildTRT_ElectronPidToolCfg(flags)))

    if (flags.Detector.EnablePixel or flags.Detector.EnableITkPixel) and "TestPixelLayerTool" not in kwargs:
        from InDetConfig.InDetTestPixelLayerConfig import InDetTestPixelLayerToolInnerCfg
        kwargs.setdefault("TestPixelLayerTool", result.popToolsAndMerge(InDetTestPixelLayerToolInnerCfg(flags)))

    kwargs.setdefault("ComputeAdditionalInfo", True)
    kwargs.setdefault("KeepParameters", True)
    kwargs.setdefault("BadClusterID", 0)
    kwargs.setdefault("IBLParameterSvc",
                      "IBLParameterSvc" if flags.Detector.GeometryID else "")

    result.setPrivateTools(CompFactory.Trk.TrackParticleCreatorTool(name, **kwargs))
    return result


####################################
#####          Muons           #####
####################################

def MuonParticleCreatorToolCfg(flags, name="MuonParticleCreatorTool", **kwargs):
    result = ComponentAccumulator()

    if "TrackToVertex" not in kwargs:
        from InDetConfig.TrackRecoConfig import TrackToVertexCfg
        kwargs.setdefault("TrackToVertex", result.popToolsAndMerge(
            TrackToVertexCfg(flags)))

    if "TrackSummaryTool" not in kwargs:
        from TrkConfig.TrkTrackSummaryToolConfig import MuonTrackSummaryToolCfg
        kwargs.setdefault("TrackSummaryTool", result.popToolsAndMerge(
            MuonTrackSummaryToolCfg(flags)))

    if "MuonSummaryTool" not in kwargs:
        from MuonConfig.MuonRecToolsConfig import MuonHitSummaryToolCfg
        kwargs.setdefault("MuonSummaryTool", result.popToolsAndMerge(
            MuonHitSummaryToolCfg(flags)))

    kwargs.setdefault("KeepAllPerigee", True)
    kwargs.setdefault("PerigeeExpression", "Origin")
    kwargs.setdefault("IBLParameterSvc",
                      "IBLParameterSvc" if flags.Detector.GeometryID else "")

    result.setPrivateTools(CompFactory.Trk.TrackParticleCreatorTool(name, **kwargs))
    return result

def MuonCombinedParticleCreatorCfg(flags, name="MuonCombinedParticleCreator", **kwargs):
    result = ComponentAccumulator()

    if "TrackSummaryTool" not in kwargs:
        if flags.Muon.SAMuonTrigger:
            from TrkConfig.TrkTrackSummaryToolConfig import MuonTrackSummaryToolCfg
            kwargs.setdefault("TrackSummaryTool", result.popToolsAndMerge(MuonTrackSummaryToolCfg(flags)))
        else:
            from TrkConfig.TrkTrackSummaryToolConfig import MuonCombinedTrackSummaryToolCfg
            kwargs.setdefault("TrackSummaryTool", result.popToolsAndMerge(MuonCombinedTrackSummaryToolCfg(flags)))
            from InDetConfig.InDetTestPixelLayerConfig import InDetTestPixelLayerToolInnerCfg
            kwargs.setdefault("TestPixelLayerTool", result.popToolsAndMerge(InDetTestPixelLayerToolInnerCfg(flags)))

    if "TrackToVertex" not in kwargs:
        from MuonCombinedConfig.MuonCombinedRecToolsConfig import MuonTrackToVertexCfg
        kwargs.setdefault('TrackToVertex', result.popToolsAndMerge(
            MuonTrackToVertexCfg(flags)))

    if "MuonSummaryTool" not in kwargs:
        from MuonConfig.MuonRecToolsConfig import MuonHitSummaryToolCfg
        kwargs.setdefault("MuonSummaryTool", result.popToolsAndMerge(
            MuonHitSummaryToolCfg(flags)))

    # if "PixelToTPIDTool" not in kwargs and not flags.Muon.MuonTrigger and flags.GeoModel.Run < LHCPeriod.Run4:
    #    from InDetConfig.TrackingCommonConfig import InDetPixelToTPIDToolCfg
    #    kwargs.setdefault("PixelToTPIDTool", result.popToolsAndMerge(
    #        InDetPixelToTPIDToolCfg(flags)))

    kwargs.setdefault("ComputeAdditionalInfo", True)
    kwargs.setdefault("KeepAllPerigee", True)
    if flags.Beam.Type is BeamType.Cosmics:
        kwargs.setdefault("PerigeeExpression", "Origin")
    kwargs.setdefault("IBLParameterSvc",
                      "IBLParameterSvc" if flags.Detector.GeometryID else "")

    result.setPrivateTools(CompFactory.Trk.TrackParticleCreatorTool(name, **kwargs))
    return result

def MuonCaloParticleCreatorCfg(flags, name="MuonCaloParticleCreator", **kwargs):
    result = ComponentAccumulator()

    if "TrackToVertex" not in kwargs:
        from InDetConfig.TrackRecoConfig import TrackToVertexCfg
        kwargs.setdefault("TrackToVertex", result.popToolsAndMerge(
            TrackToVertexCfg(flags, name='TrackToVertex')))

    if "TrackSummaryTool" not in kwargs:
        if flags.Muon.SAMuonTrigger:
            from TrkConfig.TrkTrackSummaryToolConfig import MuonTrackSummaryToolCfg
            kwargs.setdefault("TrackSummaryTool", result.popToolsAndMerge(
                MuonTrackSummaryToolCfg(flags)))
        else:
            from TrkConfig.TrkTrackSummaryToolConfig import MuonCombinedTrackSummaryToolCfg
            kwargs.setdefault("TrackSummaryTool", result.popToolsAndMerge(
                MuonCombinedTrackSummaryToolCfg(flags)))

    kwargs.setdefault("KeepAllPerigee", True)
    kwargs.setdefault("PerigeeExpression", "Origin")
    kwargs.setdefault("IBLParameterSvc",
                      "IBLParameterSvc" if flags.Detector.GeometryID else "")

    result.setPrivateTools(CompFactory.Trk.TrackParticleCreatorTool(name, **kwargs))
    return result
