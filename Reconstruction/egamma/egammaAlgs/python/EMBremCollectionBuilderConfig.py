# Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration

from AthenaConfiguration.ComponentAccumulator import ComponentAccumulator
from AthenaConfiguration.ComponentFactory import CompFactory
from AthenaCommon.Logging import logging


def EMBremCollectionBuilderCfg(flags,
                               name="EMBremCollectionBuilder",
                               **kwargs):
    acc = ComponentAccumulator()

    # FIXME runtime testing required this, but since this is not a
    # direct dependency it should be added elsewhere,
    # but I do not know where yet
    if not flags.Input.isMC:
        from LumiBlockComps.LumiBlockMuWriterConfig import LumiBlockMuWriterCfg
        acc.merge(LumiBlockMuWriterCfg(flags))

    if "TrackRefitTool" not in kwargs:
        from egammaTrackTools.egammaTrackToolsConfig import (
            egammaTrkRefitterToolCfg)
        kwargs["TrackRefitTool"] = acc.popToolsAndMerge(
            egammaTrkRefitterToolCfg(flags))

    if "TrackParticleCreatorTool" not in kwargs:
        from InDetConfig.TrackRecoConfig import TrackParticleCreatorToolCfg
        kwargs["TrackParticleCreatorTool"] = acc.getPrimaryAndMerge(
            TrackParticleCreatorToolCfg(
                flags,
                name="GSFBuildInDetParticleCreatorTool",
                BadClusterID=3))

    if "TrackSlimmingTool" not in kwargs:
        slimmingTool = CompFactory.Trk.TrackSlimmingTool(
            name="GSFBuildInDetTrackSlimmingTool", KeepOutliers=True)
        kwargs["TrackSlimmingTool"] = slimmingTool

    if "TrackSummaryTool" not in kwargs:
        from egammaTrackTools.egammaTrackToolsConfig import (
            GSFTrackSummaryToolCfg)
        kwargs["TrackSummaryTool"] = acc.popToolsAndMerge(
            GSFTrackSummaryToolCfg(flags))

    # TODO configure according to some doPixel, presumably
    # flags.Detector.EnablePixel, same for src
    kwargs.setdefault("usePixel", True)
    kwargs.setdefault("useSCT", True)
    alg = CompFactory.EMBremCollectionBuilder(name, **kwargs)
    acc.addEventAlgo(alg)
    return acc


if __name__ == "__main__":
    from AthenaCommon.Configurable import Configurable
    Configurable.configurableRun3Behavior = True
    from AthenaConfiguration.AllConfigFlags import ConfigFlags as flags
    from AthenaConfiguration.TestDefaults import defaultTestFiles
    from AthenaConfiguration.ComponentAccumulator import printProperties
    from AthenaConfiguration.MainServicesConfig import MainServicesCfg
    flags.Input.Files = defaultTestFiles.RDO

    acc = MainServicesCfg(flags)
    acc.merge(EMBremCollectionBuilderCfg(flags))
    mlog = logging.getLogger("EMBremCollectionBuilderConfigTest")
    mlog.info("Configuring  EMBremCollectionBuilder: ")
    printProperties(mlog,
                    acc.getEventAlgo("EMBremCollectionBuilder"),
                    nestLevel=1,
                    printDefaults=True)
    with open("embremcollectionbuilder.pkl", "wb") as f:
        acc.store(f)
