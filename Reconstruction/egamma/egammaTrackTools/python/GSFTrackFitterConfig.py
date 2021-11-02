# Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration

__doc__ = "Configure GSF track fitter building"

from AthenaCommon.Logging import logging
from AthenaConfiguration.ComponentFactory import CompFactory
from AthenaConfiguration.ComponentAccumulator import ComponentAccumulator
from TrkConfig.AtlasExtrapolatorToolsConfig import AtlasNavigatorCfg


def EMGSFExtrapolatorToolCfg(flags, **kwargs):

    mlog = logging.getLogger('EMGSFExtrapolatorTool')
    mlog.debug('Start configuration')

    acc = ComponentAccumulator()

    if "Propagators" not in kwargs:
        egTrkPropagator = CompFactory.Trk.RungeKuttaPropagator(
            name='egTrkPropagator',
            AccuracyParameter=0.0001)
        kwargs["Propagators"] = [egTrkPropagator]

    if "Navigator" not in kwargs:
        egTrkNavigator = AtlasNavigatorCfg(
            flags,
            name='egTrkNavigator')
        kwargs["Navigator"] = acc.getPrimaryAndMerge(egTrkNavigator)

    if "GsfMaterialConvolution" not in kwargs:
        GsfMaterialUpdator = CompFactory.Trk.GsfMaterialMixtureConvolution(
            name='GsfMaterialUpdator',
            MaximumNumberOfComponents=12)
        kwargs["GsfMaterialConvolution"] = GsfMaterialUpdator

    kwargs.setdefault("SearchLevelClosestParameters", 10)
    kwargs.setdefault("StickyConfiguration",          True)
    kwargs.setdefault("SurfaceBasedMaterialEffects",  False)

    gsfextrapolatorTool = CompFactory.Trk.GsfExtrapolator(
        name='GsfExtrapolator', **kwargs)
    acc.setPrivateTools(gsfextrapolatorTool)
    return acc


def EMGSFTrackFitterCfg(flags, name='EMGSFTrackFitter', **kwargs):

    mlog = logging.getLogger(name)
    mlog.debug('Start configuration')

    acc = ComponentAccumulator()

    if "RefitOnMeasurementBase" not in kwargs:
        kwargs["RefitOnMeasurementBase"] = True

    if not kwargs["RefitOnMeasurementBase"]:
        from InDetConfig.TrackingCommonConfig import InDetRotCreatorCfg
        egRotCreator = InDetRotCreatorCfg(
            flags,
            name='egRotCreator')
        kwargs["ToolForROTCreation"] = acc.popToolsAndMerge(egRotCreator)

        # TODO
        # TrackingCommon.createAndAddCondAlg(
        #    TrackingCommon.getRIO_OnTrackErrorScalingCondAlg,
        #    'RIO_OnTrackErrorScalingCondAlg')

    if "ToolForExtrapolation" not in kwargs:
        gsfextrap = EMGSFExtrapolatorToolCfg(flags)
        kwargs["ToolForExtrapolation"] = acc.popToolsAndMerge(gsfextrap)

    kwargs.setdefault("ReintegrateOutliers", True)
    kwargs.setdefault("MakePerigee",         True)
    kwargs.setdefault("DoHitSorting",        True)

    GSFTrackFitter = CompFactory.Trk.GaussianSumFitter(
        name='GSFTrackFitter', **kwargs)
    acc.setPrivateTools(GSFTrackFitter)
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
    mlog = logging.getLogger("GSFTrackFitterConfigTest")
    mlog.info("Configuring EMGSFExtrapolatorTool :")
    printProperties(mlog, cfg.popToolsAndMerge(
        EMGSFExtrapolatorToolCfg(ConfigFlags)),
        nestLevel=1,
        printDefaults=True)
    mlog.info("Configuring EMGSFTrackFitter :")
    printProperties(mlog, cfg.popToolsAndMerge(
        EMGSFTrackFitterCfg(ConfigFlags)),
        nestLevel=1,
        printDefaults=True)

    f = open("gsftrackfitter.pkl", "wb")
    cfg.store(f)
    f.close()
