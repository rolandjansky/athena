# Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration

from AthenaConfiguration.ComponentAccumulator import ComponentAccumulator
from AthenaConfiguration.ComponentFactory import CompFactory
from AthenaCommon.Logging import logging


def GSFTrackSummaryToolCfg(flags,
                           name="GSFTrackSummaryTool",
                           **kwargs):
    """ The Track Summary for the GSF refitted Tracks/TrackParticles"""

    acc = ComponentAccumulator()

    if "PixelToTPIDTool" not in kwargs:
        if flags.Detector.EnablePixel:
            kwargs["PixelToTPIDTool"] = CompFactory.InDet.PixelToTPIDTool(
                name="GSFBuildPixelToTPIDTool")
        else:
            kwargs["PixelToTPIDTool"] = None

    # TODO what happens to
    # ClusterSplitProbabilityName=
    # TrackingCommon.combinedClusterSplitProbName() ?
    # It is "InDetTRT_SeededAmbiguityProcessorSplitProb" in run-2 config
    #         (because backTrk and TRTSA are run)
    # It might be "AmbiguityProcessorSplitProb" in run-3 config
    # (only one existing till now)
    if "InDetSummaryHelperTool" not in kwargs:
        testBLTool = None
        if flags.Detector.EnablePixel:
            from InDetConfig.TrackingCommonConfig import (
                InDetRecTestBLayerToolCfg)
            testBLTool = acc.popToolsAndMerge(
                InDetRecTestBLayerToolCfg(
                    flags,
                    name="GSFBuildTestBLayerTool"))

        from InDetConfig.InDetRecToolConfig import (
            InDetTrackSummaryHelperToolCfg)
        kwargs["InDetSummaryHelperTool"] = acc.getPrimaryAndMerge(
            InDetTrackSummaryHelperToolCfg(
                flags,
                name="GSFBuildTrackSummaryHelperTool",
                HoleSearch=None,
                AssoTool=None,
                PixelToTPIDTool=kwargs["PixelToTPIDTool"],
                TestBLayerTool=testBLTool
            ))

    if "TRT_ElectronPidTool" not in kwargs:
        if flags.Detector.EnableTRT:
            from InDetConfig.TRT_ElectronPidToolsConfig import (
                TRT_ElectronPidToolCfg)
            kwargs["TRT_ElectronPidTool"] = acc.popToolsAndMerge(
                TRT_ElectronPidToolCfg(
                    flags,
                    name="GSFBuildTRT_ElectronPidTool",
                    CalculateNNPid=False,
                    MinimumTrackPtForNNPid=0.))
        else:
            kwargs["TRT_ElectronPidTool"] = None

    kwargs.setdefault("doSharedHits", False)
    kwargs.setdefault("doHolesInDet", False)

    summaryTool = CompFactory.Trk.TrackSummaryTool(name, **kwargs)
    acc.setPrivateTools(summaryTool)
    return acc


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
        gsfTrackParticleCreatorTool = CompFactory.Trk.TrackParticleCreatorTool(
            name="GSFBuildInDetParticleCreatorTool",
            KeepParameters=True,
            TrackToVertex=acc.popToolsAndMerge(TrackToVertexCfg(flags)),
            TrackSummaryTool="",
            BadClusterID=0,
            IBLParameterSvc="IBLParameterSvc" if flags.Detector.GeometryID else "")
        kwargs["TrackParticleCreatorTool"] = gsfTrackParticleCreatorTool

    if "TrackSlimmingTool" not in kwargs:
        slimmingTool = CompFactory.Trk.TrackSlimmingTool(
            name="GSFBuildInDetTrackSlimmingTool",
            KeepParameters=False,
            KeepOutliers=True)
        kwargs["TrackSlimmingTool"] = slimmingTool

    if "TrackSummaryTool" not in kwargs:
        kwargs["TrackSummaryTool"] = acc.popToolsAndMerge(
            GSFTrackSummaryToolCfg(flags))

    kwargs.setdefault("usePixel", flags.Detector.EnablePixel or flags.Detector.EnableITkPixel)
    kwargs.setdefault("useSCT", flags.Detector.EnableSCT or flags.Detector.EnableITkStrip)
    kwargs.setdefault("DoTruth", flags.Input.isMC)

    # P->T conversion extra dependencies
    if flags.Detector.GeometryITk:
        kwargs.setdefault("ExtraInputs", [
            ("InDetDD::SiDetectorElementCollection", "ConditionStore+ITkPixelDetectorElementCollection"),
            ("InDetDD::SiDetectorElementCollection", "ConditionStore+ITkStripDetectorElementCollection"),
        ])
    else:
        kwargs.setdefault("ExtraInputs", [
            ("InDetDD::SiDetectorElementCollection", "ConditionStore+PixelDetectorElementCollection"),
            ("InDetDD::SiDetectorElementCollection", "ConditionStore+SCT_DetectorElementCollection"),
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
    flags.Input.Files = defaultTestFiles.RDO
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
