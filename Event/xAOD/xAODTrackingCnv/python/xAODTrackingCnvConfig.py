"""Define methods to construct configured Tracking conversion algorithms

Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
"""
from AthenaConfiguration.ComponentAccumulator import ComponentAccumulator
from AthenaConfiguration.ComponentFactory import CompFactory

######################################
###   RecTrackParticleContainerCnvTool
######################################

def RecTrackParticleContainerCnvToolCfg(flags, name="RecTrackParticleContainerCnvTool", **kwargs):
    result = ComponentAccumulator()

    if "TrackParticleCreator" not in kwargs:
        from TrkConfig.TrkParticleCreatorConfig import TrackParticleCreatorToolCfg
        TrackParticleCreator = result.popToolsAndMerge(TrackParticleCreatorToolCfg(flags))
        result.addPublicTool(TrackParticleCreator)
        kwargs.setdefault("TrackParticleCreator", TrackParticleCreator)

    result.setPrivateTools(CompFactory.xAODMaker.RecTrackParticleContainerCnvTool(name, **kwargs))
    return result

def MuonRecTrackParticleContainerCnvToolCfg(flags, name = "MuonRecTrackParticleContainerCnvTool", **kwargs):
    result = ComponentAccumulator()

    if "TrackParticleCreator" not in kwargs:
        from TrkConfig.TrkParticleCreatorConfig import MuonCombinedParticleCreatorCfg
        TrackParticleCreator = result.popToolsAndMerge(MuonCombinedParticleCreatorCfg(flags))
        result.addPublicTool(TrackParticleCreator)
        kwargs.setdefault("TrackParticleCreator", TrackParticleCreator)

    result.setPrivateTools(CompFactory.xAODMaker.RecTrackParticleContainerCnvTool(name, **kwargs))
    return result

######################################
###   TrackCollectionCnvTool
######################################

def TrackCollectionCnvToolCfg(flags, name="TrackCollectionCnvTool", **kwargs):
    if flags.Detector.GeometryITk:
        name = name.replace("InDet", "ITk")
        from InDetConfig.ITkTrackRecoConfig import ITkTrackCollectionCnvToolCfg
        return ITkTrackCollectionCnvToolCfg(flags, name, **kwargs)

    result = ComponentAccumulator()

    if "TrackParticleCreator" not in kwargs:
        from TrkConfig.TrkParticleCreatorConfig import TrackParticleCreatorToolCfg
        TrackParticleCreator = result.popToolsAndMerge(TrackParticleCreatorToolCfg(flags))
        result.addPublicTool(TrackParticleCreator)
        kwargs.setdefault("TrackParticleCreator", TrackParticleCreator)

    result.setPrivateTools(CompFactory.xAODMaker.TrackCollectionCnvTool(name, **kwargs))
    return result

def ITkTrackCollectionCnvToolCfg(flags, name="ITkTrackCollectionCnvTool", **kwargs):
    result = ComponentAccumulator()

    if "TrackParticleCreator" not in kwargs:
        from TrkConfig.TrkParticleCreatorConfig import ITkTrackParticleCreatorToolCfg
        TrackParticleCreator = result.popToolsAndMerge(ITkTrackParticleCreatorToolCfg(flags))
        result.addPublicTool(TrackParticleCreator)
        kwargs.setdefault("TrackParticleCreator", TrackParticleCreator)

    result.setPrivateTools(CompFactory.xAODMaker.TrackCollectionCnvTool(name, **kwargs))
    return result

def MuonTrackCollectionCnvToolCfg(flags, name = "MuonTrackCollectionCnvTool", **kwargs):
    result = ComponentAccumulator()

    if "TrackParticleCreator" not in kwargs:
        from TrkConfig.TrkParticleCreatorConfig import MuonCombinedParticleCreatorCfg
        TrackParticleCreator = result.popToolsAndMerge(MuonCombinedParticleCreatorCfg(flags))
        result.addPublicTool(TrackParticleCreator)
        kwargs.setdefault("TrackParticleCreator", TrackParticleCreator)

    result.setPrivateTools(CompFactory.xAODMaker.TrackCollectionCnvTool(name, **kwargs))
    return result

######################################
###   TrackParticleCnvAlg
######################################

def TrackParticleCnvAlgCfg(flags, name="TrackParticleCnvAlg", **kwargs):
    if flags.Detector.GeometryITk:
        name = name.replace("InDet", "ITk")
        from InDetConfig.ITkTrackRecoConfig import ITkTrackParticleCnvAlgCfg
        return ITkTrackParticleCnvAlgCfg(flags, name, **kwargs)

    result = ComponentAccumulator()

    kwargs.setdefault("ConvertTracks", True)
    kwargs.setdefault("ConvertTrackParticles", False)
    kwargs.setdefault("TrackContainerName", "CombinedInDetTracks")
    kwargs.setdefault("xAODTrackParticlesFromTracksContainerName", "InDetTrackParticles")

    if "TrackParticleCreator" not in kwargs:
        from TrkConfig.TrkParticleCreatorConfig import TrackParticleCreatorToolCfg
        kwargs.setdefault("TrackParticleCreator", result.popToolsAndMerge(TrackParticleCreatorToolCfg(flags)))

    if "TrackCollectionCnvTool" not in kwargs:
        kwargs.setdefault("TrackCollectionCnvTool", result.popToolsAndMerge(TrackCollectionCnvToolCfg(flags)))

    if flags.InDet.doTruth:
        kwargs.setdefault("TrackTruthContainerName", kwargs["TrackContainerName"]+"TruthCollection")
        kwargs.setdefault("AddTruthLink", True)
        if "MCTruthClassifier" not in kwargs:
            from MCTruthClassifier.MCTruthClassifierConfig import MCTruthClassifierCfg
            kwargs.setdefault("MCTruthClassifier", result.popToolsAndMerge(MCTruthClassifierCfg(flags)))
    else:
        kwargs.setdefault("AddTruthLink", False)

    result.addEventAlgo(CompFactory.xAODMaker.TrackParticleCnvAlg(name, **kwargs))
    return result

def TrackParticleCnvAlgPIDCheckCfg(flags, name, **kwargs):
    result = ComponentAccumulator()

    if "TrackParticleCreator" not in kwargs:
        from TrkConfig.TrkParticleCreatorConfig import TrackParticleCreatorToolPIDCheckCfg
        kwargs.setdefault("TrackParticleCreator", result.popToolsAndMerge(TrackParticleCreatorToolPIDCheckCfg(flags)))

    result.merge(TrackParticleCnvAlgCfg(flags, name, **kwargs))
    return result

def TrackParticleCnvAlgNoPIDCfg(flags, name, **kwargs):
    result = ComponentAccumulator()

    if "TrackParticleCreator" not in kwargs:
        from TrkConfig.TrkParticleCreatorConfig import TrackParticleCreatorToolNoPIDCfg
        kwargs.setdefault("TrackParticleCreator", result.popToolsAndMerge(TrackParticleCreatorToolNoPIDCfg(flags)))

    result.merge(TrackParticleCnvAlgCfg(flags, name, **kwargs))
    return result

def ITkTrackParticleCnvAlgCfg(flags, name="ITkTrackParticleCnvAlg", TrackContainerName="CombinedITkTracks", OutputTrackParticleContainer="InDetTrackParticles", **kwargs):
    result = ComponentAccumulator()

    kwargs.setdefault("ConvertTracks", True)
    kwargs.setdefault("ConvertTrackParticles", False)
    kwargs.setdefault("TrackContainerName", "CombinedITkTracks")
    kwargs.setdefault("xAODTrackParticlesFromTracksContainerName", "InDetTrackParticles")

    if "TrackParticleCreator" not in kwargs:
        from TrkConfig.TrkParticleCreatorConfig import ITkTrackParticleCreatorToolCfg
        kwargs.setdefault("TrackParticleCreator", result.popToolsAndMerge(ITkTrackParticleCreatorToolCfg(flags)))

    if "TrackCollectionCnvTool" not in kwargs:
        kwargs.setdefault("TrackCollectionCnvTool", result.popToolsAndMerge(ITkTrackCollectionCnvToolCfg(flags)))

    if flags.ITk.Tracking.doTruth:
        kwargs.setdefault("TrackTruthContainerName", kwargs["TrackContainerName"]+"TruthCollection")
        kwargs.setdefault("AddTruthLink", True)
        if "MCTruthClassifier" not in kwargs:
            from MCTruthClassifier.MCTruthClassifierConfig import MCTruthClassifierCfg
            kwargs.setdefault("MCTruthClassifier", result.popToolsAndMerge(MCTruthClassifierCfg(flags)))
    else:
        kwargs.setdefault("AddTruthLink", False)

    result.addEventAlgo(CompFactory.xAODMaker.TrackParticleCnvAlg(name, **kwargs))
    return result

def MuonTrackParticleCnvCfg(flags, name = "MuonTrackParticleCnvAlg",**kwargs):
    result=ComponentAccumulator()

    if "TrackParticleCreator" not in kwargs:
        from TrkConfig.TrkParticleCreatorConfig import MuonCombinedParticleCreatorCfg
        kwargs.setdefault("TrackParticleCreator", result.popToolsAndMerge(MuonCombinedParticleCreatorCfg(flags)))

    if "TrackCollectionCnvTool" not in kwargs:
        from xAODTrackingCnv.xAODTrackingCnvConfig import MuonTrackCollectionCnvToolCfg
        kwargs.setdefault("TrackCollectionCnvTool", result.popToolsAndMerge(MuonTrackCollectionCnvToolCfg(flags)))

    kwargs.setdefault("TrackContainerName", "MuonSpectrometerTracks")
    kwargs.setdefault("xAODTrackParticlesFromTracksContainerName", "MuonSpectrometerTrackParticles")
    kwargs.setdefault("ConvertTrackParticles", False)
    kwargs.setdefault("ConvertTracks", True)
    kwargs.setdefault("AddTruthLink", False)

    result.addEventAlgo(CompFactory.xAODMaker.TrackParticleCnvAlg(name, **kwargs))
    return result

def MuonStandaloneTrackParticleCnvAlgCfg(flags, name = "MuonStandaloneTrackParticleCnvAlg", **kwargs):
    result=ComponentAccumulator()

    if "TrackParticleCreator" not in kwargs:
        from TrkConfig.TrkParticleCreatorConfig import MuonParticleCreatorToolCfg
        kwargs.setdefault("TrackParticleCreator", result.popToolsAndMerge(MuonParticleCreatorToolCfg(flags)))

    result.merge(MuonTrackParticleCnvCfg(flags, name, **kwargs))
    return result
