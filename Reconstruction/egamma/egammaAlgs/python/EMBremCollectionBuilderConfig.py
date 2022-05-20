# Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration

from AthenaConfiguration.ComponentAccumulator import ComponentAccumulator
from AthenaConfiguration.ComponentFactory import CompFactory
from AthenaConfiguration.Enums import LHCPeriod
from AthenaCommon.Logging import logging


def EMBremCollectionBuilderCfg(flags,
                               name="EMBremCollectionBuilder",
                               **kwargs):
    """ Algorithm to create the GSF collection
        We need to refit with GSF
        Create a Track Summary
        Create the relevant TrackParticles """

    acc = ComponentAccumulator()

    if "TrackRefitTool" not in kwargs:
        from egammaTrackTools.egammaTrackToolsConfig import (
            egammaTrkRefitterToolCfg)
        kwargs["TrackRefitTool"] = acc.popToolsAndMerge(
            egammaTrkRefitterToolCfg(flags))

    if "TrackParticleCreatorTool" not in kwargs:
        from InDetConfig.TrackRecoConfig import TrackToVertexCfg

        from InDetConfig.TRT_ElectronPidToolsConfig import (
            TRT_ElectronPidToolCfg)
        from TrkConfig.TrkTrackSummaryToolConfig import GSFTrackSummaryToolCfg
        TrackSummaryTool = acc.popToolsAndMerge(GSFTrackSummaryToolCfg(flags))
        acc.addPublicTool(TrackSummaryTool)

        gsfTrackParticleCreatorTool = CompFactory.Trk.TrackParticleCreatorTool(
            name="GSFBuildInDetParticleCreatorTool",
            KeepParameters=True,
            TrackToVertex=acc.popToolsAndMerge(TrackToVertexCfg(flags)),
            TrackSummaryTool=TrackSummaryTool,
            PixelToTPIDTool=(CompFactory.InDet.PixelToTPIDTool(
                name="GSFBuildPixelToTPIDTool")
                if flags.GeoModel.Run < LHCPeriod.Run4 else None),
            TRT_ElectronPidTool=(acc.popToolsAndMerge(
                TRT_ElectronPidToolCfg(
                    flags,
                    name="GSFBuildTRT_ElectronPidTool",
                    CalculateNNPid=(flags.GeoModel.Run is LHCPeriod.Run3),
                    MinimumTrackPtForNNPid=0.))
                if flags.Detector.EnableTRT else None),
            BadClusterID=0,
            IBLParameterSvc=(
                "IBLParameterSvc" if flags.Detector.GeometryID else "")
        )
        kwargs["TrackParticleCreatorTool"] = gsfTrackParticleCreatorTool

    if "TrackSlimmingTool" not in kwargs:
        slimmingTool = CompFactory.Trk.TrackSlimmingTool(
            name="GSFBuildInDetTrackSlimmingTool",
            KeepParameters=False,
            KeepOutliers=True)
        kwargs["TrackSlimmingTool"] = slimmingTool

    kwargs.setdefault(
        "usePixel",
        flags.Detector.EnablePixel or flags.Detector.EnableITkPixel)
    kwargs.setdefault(
        "useSCT",
        flags.Detector.EnableSCT or flags.Detector.EnableITkStrip)
    kwargs.setdefault("useTRT", flags.Detector.EnableTRT)
    kwargs.setdefault("DoTruth", flags.Input.isMC)
    kwargs.setdefault("slimTrkTracks", flags.Egamma.slimGSFTrkTracks)

    # P->T conversion extra dependencies
    if flags.Detector.GeometryITk:
        kwargs.setdefault("ExtraInputs", [
            ("InDetDD::SiDetectorElementCollection",
             "ConditionStore+ITkPixelDetectorElementCollection"),
            ("InDetDD::SiDetectorElementCollection",
             "ConditionStore+ITkStripDetectorElementCollection"),
        ])
    else:
        kwargs.setdefault("ExtraInputs", [
            ("InDetDD::SiDetectorElementCollection",
             "ConditionStore+PixelDetectorElementCollection"),
            ("InDetDD::SiDetectorElementCollection",
             "ConditionStore+SCT_DetectorElementCollection"),
        ])

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
    flags.Input.Files = defaultTestFiles.RDO_RUN2
    flags.lock()
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
