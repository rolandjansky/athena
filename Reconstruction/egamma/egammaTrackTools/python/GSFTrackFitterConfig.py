# Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration

__doc__ = "Configure GSF track fitter building"

from AthenaCommon.Logging import logging
from AthenaConfiguration.ComponentFactory import CompFactory
from AthenaConfiguration.ComponentAccumulator import ComponentAccumulator


def EMGSFExtrapolatorToolCfg(flags, **kwargs):

    acc = ComponentAccumulator()

    if "Propagators" not in kwargs:
        egTrkPropagator = CompFactory.Trk.RungeKuttaPropagator(
            name="egTrkPropagator", AccuracyParameter=0.0001
        )
        kwargs["Propagator"] = egTrkPropagator

    if "Navigator" not in kwargs:
        from TrkConfig.AtlasExtrapolatorToolsConfig import AtlasNavigatorCfg

        egTrkNavigator = AtlasNavigatorCfg(flags, name="egTrkNavigator")
        kwargs["Navigator"] = acc.popToolsAndMerge(egTrkNavigator)

    if "EnergyLossUpdator" not in kwargs:
        from TrkConfig.AtlasExtrapolatorToolsConfig import AtlasEnergyLossUpdatorCfg
        ELossUpdator = AtlasEnergyLossUpdatorCfg(flags)
        kwargs["EnergyLossUpdator"] = acc.popToolsAndMerge(ELossUpdator)

    if "GsfMaterialConvolution" not in kwargs:
        GsfMaterialUpdator = CompFactory.Trk.GsfMaterialMixtureConvolution(
            name="GsfMaterialUpdator", MaximumNumberOfComponents=12
        )
        kwargs["GsfMaterialConvolution"] = GsfMaterialUpdator

    kwargs.setdefault("SurfaceBasedMaterialEffects", False)

    acc.setPrivateTools(CompFactory.Trk.GsfExtrapolator(**kwargs))
    return acc


def EMGSFTrackFitterCfg(flags, name="EMGSFTrackFitter", **kwargs):

    acc = ComponentAccumulator()

    if "RefitOnMeasurementBase" not in kwargs:
        kwargs["RefitOnMeasurementBase"] = True

    kwargs["ToolForROTCreation"] = None
    if not kwargs["RefitOnMeasurementBase"]:
        from InDetConfig.TrackingCommonConfig import InDetRotCreatorCfg

        kwargs["ToolForROTCreation"] = acc.popToolsAndMerge(
            InDetRotCreatorCfg(flags)
        )

    if "ToolForExtrapolation" not in kwargs:
        gsfextrap = EMGSFExtrapolatorToolCfg(flags)
        kwargs["ToolForExtrapolation"] = acc.popToolsAndMerge(gsfextrap)

    kwargs.setdefault("ReintegrateOutliers", True)
    kwargs.setdefault("MakePerigee", True)
    kwargs.setdefault("DoHitSorting", True)

    acc.setPrivateTools(CompFactory.Trk.GaussianSumFitter(**kwargs))

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
    printProperties(
        mlog,
        cfg.popToolsAndMerge(EMGSFExtrapolatorToolCfg(ConfigFlags)),
        nestLevel=1,
        printDefaults=True,
    )
    mlog.info("Configuring EMGSFTrackFitter :")
    printProperties(
        mlog,
        cfg.popToolsAndMerge(EMGSFTrackFitterCfg(ConfigFlags)),
        nestLevel=1,
        printDefaults=True,
    )

    f = open("gsftrackfitter.pkl", "wb")
    cfg.store(f)
    f.close()
