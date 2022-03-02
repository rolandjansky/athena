# Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration

__doc__ = """Tool configuration to instantiate all
 egammaCaloTools with default configuration"""

from AthenaCommon.Logging import logging
from AthenaConfiguration.ComponentFactory import CompFactory
from AthenaConfiguration.ComponentAccumulator import ComponentAccumulator
from TrkConfig.AtlasExtrapolatorConfig import (
    AtlasExtrapolatorCfg, egammaCaloExtrapolatorCfg)
from TrackToCalo.TrackToCaloConfig import ParticleCaloExtensionToolCfg


def egCaloDepthCfg(flags, **kwargs):
    acc = ComponentAccumulator()
    kwargs.setdefault("name", "egCaloDepthToolmiddle")
    kwargs.setdefault("DepthChoice", "middle")
    acc.setPrivateTools(
        CompFactory.CaloDepthTool(**kwargs))
    return acc


def egCaloSurfaceBuilderCfg(flags, **kwargs):
    acc = ComponentAccumulator()
    kwargs.setdefault(
        "CaloDepthTool",
        acc.popToolsAndMerge(egCaloDepthCfg(flags)))

    acc.setPrivateTools(
        CompFactory.CaloSurfaceBuilder(**kwargs)
    )
    return acc


def EMParticleCaloExtensionToolCfg(flags, **kwargs):
    acc = ComponentAccumulator()
    kwargs.setdefault("name", "EMParticleCaloExtensionTool")
    kwargs.setdefault("ParticleType", "electron")
    kwargs.setdefault("StartFromPerigee", True)

    if "CaloSurfaceBuilder" not in kwargs:
        kwargs["CaloSurfaceBuilder"] = acc.popToolsAndMerge(
            egCaloSurfaceBuilderCfg(flags))

    if "Extrapolator" not in kwargs:
        extrapAcc = egammaCaloExtrapolatorCfg(flags)
        kwargs["Extrapolator"] = acc.popToolsAndMerge(extrapAcc)
    return ParticleCaloExtensionToolCfg(flags, **kwargs)


def EMExtrapolationToolsCfg(flags, **kwargs):

    mlog = logging.getLogger('EMExtrapolationTools')
    mlog.debug('Start configuration')

    acc = ComponentAccumulator()

    if "Extrapolator" not in kwargs:
        extrapAcc = AtlasExtrapolatorCfg(flags)
        kwargs["Extrapolator"] = acc.popToolsAndMerge(extrapAcc)

    if "CaloExtensionTool" not in kwargs:
        kwargs["CaloExtensionTool"] = acc.popToolsAndMerge(
            EMParticleCaloExtensionToolCfg(flags))

    kwargs["EnableTRT"] = flags.Detector.GeometryTRT

    emExtrapolationTools = CompFactory.EMExtrapolationTools(**kwargs)
    acc.setPrivateTools(emExtrapolationTools)
    return acc


def egammaTrkRefitterToolCfg(flags,
                             name='GSFRefitterTool',
                             **kwargs):
    acc = ComponentAccumulator()
    if "FitterTool" not in kwargs:
        from egammaTrackTools.GSFTrackFitterConfig import EMGSFTrackFitterCfg
        kwargs["FitterTool"] = acc.popToolsAndMerge(
            EMGSFTrackFitterCfg(flags, name="GSFTrackFitter"), **kwargs)
    kwargs.setdefault("useBeamSpot", False)
    kwargs.setdefault("ReintegrateOutliers", True)
    if "Extrapolator" not in kwargs:
        kwargs["Extrapolator"] = acc.getPrimaryAndMerge(
            AtlasExtrapolatorCfg(flags))
    tool = CompFactory.egammaTrkRefitterTool(name, **kwargs)
    acc.setPrivateTools(tool)
    return acc


if __name__ == "__main__":

    from AthenaConfiguration.AllConfigFlags import ConfigFlags
    from AthenaConfiguration.ComponentAccumulator import printProperties
    from AthenaCommon.Configurable import Configurable
    from AthenaConfiguration.TestDefaults import defaultTestFiles
    Configurable.configurableRun3Behavior = True

    ConfigFlags.Input.Files = defaultTestFiles.RDO_RUN2
    ConfigFlags.fillFromArgs()
    ConfigFlags.lock()
    ConfigFlags.dump()

    cfg = ComponentAccumulator()
    mlog = logging.getLogger("egammaTrackToolsConfigTest")
    mlog.info("Configuring EMExtrapolationTools : ")
    printProperties(mlog, cfg.popToolsAndMerge(
        EMExtrapolationToolsCfg(ConfigFlags)),
        nestLevel=1,
        printDefaults=True)
    mlog.info("Configuring egammaTrkRefitterToolCfg :")
    printProperties(mlog, cfg.popToolsAndMerge(
        egammaTrkRefitterToolCfg(ConfigFlags)),
        nestLevel=1,
        printDefaults=True)

    f = open("egtracktools.pkl", "wb")
    cfg.store(f)
    f.close()
