# Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration

__doc__ = """Tool configuration to instantiate all
 egammaCaloTools with default configuration"""

from AthenaCommon.Logging import logging
from AthenaConfiguration.ComponentFactory import CompFactory
from AthenaConfiguration.ComponentAccumulator import ComponentAccumulator
from TrkConfig.AtlasExtrapolatorConfig import (
    egammaCaloExtrapolatorCfg, InDetExtrapolatorCfg)
from TrackToCalo.TrackToCaloConfig import ParticleCaloExtensionToolCfg


def EMExtrapolationToolsCfg(flags, **kwargs):

    mlog = logging.getLogger('EMExtrapolationTools')
    mlog.debug('Start configuration')

    acc = ComponentAccumulator()
    EMExtrapolationTools = CompFactory.EMExtrapolationTools

    if "Extrapolator" not in kwargs:
        extrapAcc = egammaCaloExtrapolatorCfg(flags)
        kwargs["Extrapolator"] = acc.popToolsAndMerge(extrapAcc)

    if "PerigeeCaloExtensionTool" not in kwargs:
        perigeeCaloExtrapAcc = ParticleCaloExtensionToolCfg(
            flags,
            name="EMParticleCaloExtensionTool",
            Extrapolator=kwargs["Extrapolator"],
            ParticleType="electron",
            StartFromPerigee=True)
        kwargs["PerigeeCaloExtensionTool"] = acc.popToolsAndMerge(
            perigeeCaloExtrapAcc)

    if "LastCaloExtensionTool" not in kwargs:
        lastCaloExtrapAcc = ParticleCaloExtensionToolCfg(
            flags,
            name="EMLastCaloExtensionTool",
            ParticleType="electron",
            Extrapolator=kwargs["Extrapolator"])
        kwargs["LastCaloExtensionTool"] = acc.popToolsAndMerge(
            lastCaloExtrapAcc)

    emExtrapolationTools = EMExtrapolationTools(**kwargs)
    acc.setPrivateTools(emExtrapolationTools)
    return acc


def EMExtrapolationToolsCacheCfg(flags, **kwargs):
    kwargs.setdefault("name", "EMExtrapolationToolsCache")
    kwargs.setdefault("PerigeeCache", "GSFPerigeeCaloExtension")
    kwargs.setdefault("LastCache", "GSFLastCaloExtension")
    kwargs.setdefault("useCaching", True)
    kwargs.setdefault("useLastCaching", True)
    return EMExtrapolationToolsCfg(flags, **kwargs)


def EMExtrapolationToolsCommonCacheCfg(flags, **kwargs):
    kwargs.setdefault("name", "EMExtrapolationToolsCommonCache")
    kwargs.setdefault("LastCache", "ParticleCaloExtension")
    kwargs.setdefault("useCaching", False)
    kwargs.setdefault("useLastCaching", True)
    return EMExtrapolationToolsCfg(flags, **kwargs)


def GSFTrackSummaryToolCfg(flags, name="GSFBuildInDetTrackSummaryTool", **kwargs):
    acc = ComponentAccumulator()

    if "InDetSummaryHelperTool" not in kwargs:
        from InDetConfig.InDetRecToolConfig import (
            InDetTrackSummaryHelperToolCfg)
        kwargs["InDetSummaryHelperTool"] = acc.getPrimaryAndMerge(
            InDetTrackSummaryHelperToolCfg(
                flags,
                name="GSFBuildTrackSummaryHelperTool"))

    if "PixelToTPIDTool" not in kwargs:
        from InDetConfig.TrackingCommonConfig import InDetPixelToTPIDToolCfg
        kwargs["PixelToTPIDTool"] = acc.popToolsAndMerge(
            InDetPixelToTPIDToolCfg(
                flags,
                name="GSFBuildPixelToTPIDTool"))

    if "TRT_ElectronPidTool" not in kwargs:
        from InDetConfig.TRT_ElectronPidToolsConfig import TRT_ElectronPidToolCfg
        kwargs["TRT_ElectronPidTool"] = acc.popToolsAndMerge(
            TRT_ElectronPidToolCfg(flags, name="GSFBuildTRT_ElectronPidTool"))

    summaryTool = CompFactory.Trk.TrackSummaryTool(name, **kwargs)
    acc.setPrivateTools(summaryTool)
    return acc


def egammaTrkRefitterToolCfg(flags, name='GSFRefitterTool', **kwargs):
    acc = ComponentAccumulator()
    kwargs.setdefault("useBeamSpot", False)
    kwargs.setdefault("ReintegrateOutliers", True)
    if "Extrapolator" not in kwargs:
        kwargs["Extrapolator"] = acc.getPrimaryAndMerge(
            InDetExtrapolatorCfg(flags, name="egammaExtrapolator"))
    if "FitterTool" not in kwargs:
        from InDetConfig.TrackingCommonConfig import GaussianSumFitterCfg
        kwargs["FitterTool"] = acc.popToolsAndMerge(
            GaussianSumFitterCfg(flags, name="GSFTrackFitter"))
    tool = CompFactory.egammaTrkRefitterTool(name, **kwargs)
    acc.setPrivateTools(tool)
    return acc


if __name__ == "__main__":

    from AthenaConfiguration.AllConfigFlags import ConfigFlags
    from AthenaConfiguration.ComponentAccumulator import printProperties
    from AthenaCommon.Configurable import Configurable
    from AthenaConfiguration.TestDefaults import defaultTestFiles
    Configurable.configurableRun3Behavior = True

    ConfigFlags.Input.Files = defaultTestFiles.RDO
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
    mlog.info("Configuring EMExtrapolationTools with cache : ")
    printProperties(mlog, cfg.popToolsAndMerge(
        EMExtrapolationToolsCacheCfg(ConfigFlags)),
        nestLevel=1,
        printDefaults=True)

    f = open("egtracktools.pkl", "wb")
    cfg.store(f)
    f.close()
