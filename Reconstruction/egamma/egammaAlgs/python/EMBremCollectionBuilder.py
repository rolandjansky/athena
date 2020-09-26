# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

__doc__ = """ToolFactory to instantiate  egammaBremCollectionBuilder
with default configuration"""
__author__ = "Christos"

import InDetRecExample.TrackingCommon as TrackingCommon
from AthenaCommon.DetFlags import DetFlags
from AthenaCommon.Logging import logging
# import base class
from egammaAlgs import egammaAlgsConf
from egammaRec import egammaKeys
from egammaRec.Factories import AlgFactory
from egammaTools.egammaExtrapolators import (AtlasPublicExtrapolator,
                                             egammaExtrapolator)
# default configuration of the EMBremCollectionBuilder
from InDetRecExample.InDetJobProperties import InDetFlags
from InDetRecExample.InDetKeys import InDetKeys
from RecExConfig.RecFlags import rec


class egammaBremCollectionBuilder (egammaAlgsConf.EMBremCollectionBuilder):
    __slots__ = ()

    def __init__(self, name="EMBremCollectionBuilder", **kw):
        mlog = logging.getLogger(name + '::__init__')
        mlog.info("entering")

        super(egammaBremCollectionBuilder, self).__init__(name, **kw)

        import egammaRec.EMCommonRefitter

        # Extrapolator to be used for GSF (private)
        GSFBuildInDetExtrapolator = egammaExtrapolator()

        # GsfReffiter (private not in ToolSvc)
        from egammaTrackTools.egammaTrackToolsConf import egammaTrkRefitterTool
        GSFRefitterTool = egammaTrkRefitterTool(
            name='GSFRefitterTool',
            FitterTool=egammaRec.EMCommonRefitter.getGSFTrackFitter(),
            useBeamSpot=False,
            Extrapolator=GSFBuildInDetExtrapolator,
            ReintegrateOutliers=True)

        #
        #  BLayer and Pixel Related Tools (private = True)
        #
        GSFBuildTestBLayerTool = None
        GSFBuildPixelToTPIDTool = None
        if DetFlags.haveRIO.pixel_on():
            GSFPixelConditionsSummaryTool = (
                TrackingCommon.getInDetPixelConditionsSummaryTool())
            if InDetFlags.usePixelDCS():
                GSFPixelConditionsSummaryTool.IsActiveStates = [
                    'READY', 'ON', 'UNKNOWN', 'TRANSITION', 'UNDEFINED']
                GSFPixelConditionsSummaryTool.IsActiveStatus = [
                    'OK', 'WARNING', 'ERROR', 'FATAL']

            GSFBuildTestBLayerTool = TrackingCommon.getInDetRecTestBLayerTool(
                name="GSFBuildTestBLayerTool",
                PixelSummaryTool=GSFPixelConditionsSummaryTool,
                Extrapolator=GSFBuildInDetExtrapolator,
                private=True)

            GSFBuildPixelToTPIDTool = TrackingCommon.getInDetPixelToTPIDTool(
                name="GSFBuildPixelToTPIDTool",
                private=True)
        #
        #  TRT_ElectronPidTool (private =True)
        #
        GSFBuildTRT_ElectronPidTool = None
        if DetFlags.haveRIO.TRT_on() and not InDetFlags.doSLHC(
        ) and not InDetFlags.doHighPileup():
            GSFBuildTRT_ElectronPidTool = (
                TrackingCommon.getInDetTRT_ElectronPidTool(
                    name="GSFBuildTRT_ElectronPidTool",
                    private=True))

        #
        #  InDet Track Summary Helper, no Association and no hole
        #  as we do not redo them (private = true)
        #
        GSFBuildTrackSummaryHelperTool = TrackingCommon.getInDetSummaryHelper(
            name="GSFBuildTrackSummaryHelperTool",
            AssoTool=None,
            HoleSearch=None,
            PixelToTPIDTool=GSFBuildPixelToTPIDTool,
            TestBLayerTool=GSFBuildTestBLayerTool,
            ClusterSplitProbabilityName=TrackingCommon.combinedClusterSplitProbName(),
            DoSharedHits=False,
            private=True)

        #
        #  TrkTrackSummaryTool: no shared hits  no hole search
        #
        GSFBuildInDetTrackSummaryTool = (
            TrackingCommon.getInDetTrackSummaryTool(
                name="GSFBuildInDetTrackSummaryTool",
                InDetSummaryHelperTool=GSFBuildTrackSummaryHelperTool,
                doSharedHits=False,
                doHolesInDet=False,
                TRT_ElectronPidTool=GSFBuildTRT_ElectronPidTool,
                PixelToTPIDTool=GSFBuildPixelToTPIDTool,
                private=True)
        )

        #
        #  Track Particle Creator tool (private not in ToolSvc)
        #  But needs a public extrapolator
        #
        from TrkParticleCreator.TrkParticleCreatorConf import (
            Trk__TrackParticleCreatorTool)

        GSFBuildInDetParticleCreatorTool = Trk__TrackParticleCreatorTool(
            name="GSFBuildInDetParticleCreatorTool",
            KeepParameters=True,
            Extrapolator=AtlasPublicExtrapolator(),
            UseTrackSummaryTool=False)
        #
        #  Track slimming (private not in ToolSvc)
        #
        from TrkTrackSlimmingTool.TrkTrackSlimmingToolConf import (
            Trk__TrackSlimmingTool as ConfigurableTrackSlimmingTool)
        GSFBuildInDetTrkSlimmingTool = ConfigurableTrackSlimmingTool(
            name="GSFBuildInDetTrackSlimmingTool",
            KeepParameters=False,
            KeepOutliers=True)
        #
        #  Default Configuration
        #
        self.TrackRefitTool = GSFRefitterTool
        self.TrackParticleCreatorTool = GSFBuildInDetParticleCreatorTool
        self.TrackSlimmingTool = GSFBuildInDetTrkSlimmingTool
        self.TrackSummaryTool = GSFBuildInDetTrackSummaryTool


EMBremCollectionBuilder = AlgFactory(
    egammaBremCollectionBuilder,
    name='EMBremCollectionBuilder',
    TrackParticleContainerName=InDetKeys.xAODTrackParticleContainer(),
    OutputTrkPartContainerName=egammaKeys.outputTrackParticleKey(),
    OutputTrackContainerName=egammaKeys.outputTrackKey(),
    DoTruth=rec.doTruth(),
    usePixel=DetFlags.haveRIO.pixel_on(),
    useSCT=DetFlags.haveRIO.SCT_on()
)
