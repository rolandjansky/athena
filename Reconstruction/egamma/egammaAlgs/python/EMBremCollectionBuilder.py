# Copyright (C) 2001-2019 CERN for the benefit of the ATLAS collaboration

__doc__ = "ToolFactory to instantiate  egammaBremCollectionBuilder with default configuration"
__author__ = "Christos"

# default configuration of the EMBremCollectionBuilder
from egammaTrackTools.egammaTrackToolsFactories import EMExtrapolationTools
from InDetRecExample.InDetKeys import InDetKeys
from AthenaCommon.Logging import logging
from AthenaCommon.DetFlags import DetFlags
from RecExConfig.RecFlags import rec
from InDetRecExample.InDetJobProperties import InDetFlags
# import base class
from egammaAlgs import egammaAlgsConf
from egammaTools.InDetTools import egammaExtrapolator
from egammaRec.Factories import AlgFactory
from egammaRec import egammaKeys

import InDetRecExample.TrackingCommon as TrackingCommon


class egammaBremCollectionBuilder (egammaAlgsConf.EMBremCollectionBuilder):
    __slots__ = ()

    def __init__(self, name="EMBremCollectionBuilder", **kw):
        mlog = logging.getLogger(name + '::__init__')
        mlog.info("entering")

        super(egammaBremCollectionBuilder, self).__init__(name, **kw)

        # Extrapolator to be used for GSF
        import egammaRec.EMCommonRefitter
        GSFBuildInDetExtrapolator = egammaExtrapolator()

        # Some of the InDet tool need the extrapolator to be passed
        # to be public. We can remove this part if this becomes
        # not needed
        from AthenaCommon.AppMgr import ToolSvc
        ToolSvc += GSFBuildInDetExtrapolator

        from egammaTrackTools.egammaTrackToolsConf import egammaTrkRefitterTool
        GSFRefitterTool = egammaTrkRefitterTool(name='GSFRefitterTool',
                                                FitterTool=egammaRec.EMCommonRefitter.getGSFTrackFitter(),
                                                useBeamSpot=False,
                                                Extrapolator=GSFBuildInDetExtrapolator,
                                                ReintegrateOutliers=True)
        #
        # Association tool from Inner Detector to handle pixel ganged ambiguities
        #
        GSFBuildInDetPrdAssociationTool = TrackingCommon.getInDetPrdAssociationTool(
            name="GSFBuildInDetPrdAssociationTool")

        #
        #  BLayer and Pixel Related Tools
        #
        GSFBuildTestBLayerTool = None
        GSFBuildPixelToTPIDTool = None
        if DetFlags.haveRIO.pixel_on():

            GSFPixelConditionsSummaryTool = TrackingCommon.getInDetPixelConditionsSummaryTool()
            if InDetFlags.usePixelDCS():
                GSFPixelConditionsSummaryTool.IsActiveStates = [
                    'READY', 'ON', 'UNKNOWN', 'TRANSITION', 'UNDEFINED']
                GSFPixelConditionsSummaryTool.IsActiveStatus = [
                    'OK', 'WARNING', 'ERROR', 'FATAL']

            GSFBuildTestBLayerTool = TrackingCommon.getInDetRecTestBLayerTool(name="GSFBuildTestBLayerTool",
                                                                              PixelSummaryTool=GSFPixelConditionsSummaryTool,
                                                                              Extrapolator=GSFBuildInDetExtrapolator)

            GSFBuildPixelToTPIDTool = TrackingCommon.getInDetPixelToTPIDTool(
                name="GSFBuildPixelToTPIDTool")

        #
        #  TRT_ElectronPidTool
        #
        GSFBuildTRT_ElectronPidTool = None
        if DetFlags.haveRIO.TRT_on() and not InDetFlags.doSLHC() and not InDetFlags.doHighPileup():
            GSFBuildTRT_ElectronPidTool = TrackingCommon.getInDetTRT_ElectronPidTool(
                name="GSFBuildTRT_ElectronPidTool")

        #
        #  InDet Track Summary Helper
        #
        from InDetTrackSummaryHelperTool.InDetTrackSummaryHelperToolConf import InDet__InDetTrackSummaryHelperTool
        GSFBuildTrackSummaryHelperTool = InDet__InDetTrackSummaryHelperTool(name="GSFBuildTrackSummaryHelperTool",
                                                                            AssoTool=GSFBuildInDetPrdAssociationTool,
                                                                            PixelToTPIDTool=GSFBuildPixelToTPIDTool,
                                                                            TestBLayerTool=GSFBuildTestBLayerTool,
                                                                            DoSharedHits=False,
                                                                            usePixel=DetFlags.haveRIO.pixel_on(),
                                                                            useSCT=DetFlags.haveRIO.SCT_on(),
                                                                            useTRT=DetFlags.haveRIO.TRT_on())
        #
        #  TrkTrackSummaryTool: no shared hits and avoid repeat of hole search
        #
        from TrkTrackSummaryTool.TrkTrackSummaryToolConf import Trk__TrackSummaryTool
        GSFBuildInDetTrackSummaryTool = Trk__TrackSummaryTool(name="GSFBuildInDetTrackSummaryTool",
                                                              InDetSummaryHelperTool=GSFBuildTrackSummaryHelperTool,
                                                              doSharedHits=False,
                                                              doHolesInDet=False,
                                                              TRT_ElectronPidTool=GSFBuildTRT_ElectronPidTool,
                                                              PixelToTPIDTool=GSFBuildPixelToTPIDTool)

        ToolSvc += GSFBuildInDetTrackSummaryTool
        #
        #  Track Particle Creator tool
        #
        from TrkParticleCreator.TrkParticleCreatorConf import Trk__TrackParticleCreatorTool
        GSFBuildInDetParticleCreatorTool = Trk__TrackParticleCreatorTool(name="GSFBuildInDetParticleCreatorTool",
                                                                         KeepParameters=True,
                                                                         Extrapolator=GSFBuildInDetExtrapolator,
                                                                         TrackSummaryTool=GSFBuildInDetTrackSummaryTool,
                                                                         UseTrackSummaryTool=False)
        #
        #  do track slimming
        #
        from TrkTrackSlimmingTool.TrkTrackSlimmingToolConf import Trk__TrackSlimmingTool as ConfigurableTrackSlimmingTool
        GSFBuildInDetTrkSlimmingTool = ConfigurableTrackSlimmingTool(name="GSFBuildInDetTrackSlimmingTool",
                                                                     KeepParameters=False,
                                                                     KeepOutliers=True)
        #
        #  Default Configuration
        #
        self.TrackRefitTool = GSFRefitterTool
        self.TrackParticleCreatorTool = GSFBuildInDetParticleCreatorTool
        self.TrackSlimmingTool = GSFBuildInDetTrkSlimmingTool
        self.TrackSummaryTool = GSFBuildInDetTrackSummaryTool


EMBremCollectionBuilder = AlgFactory(egammaBremCollectionBuilder,
                                     name='EMBremCollectionBuilder',
                                     ExtrapolationTool=EMExtrapolationTools,
                                     TrackParticleContainerName=InDetKeys.xAODTrackParticleContainer(),
                                     OutputTrkPartContainerName=egammaKeys.outputTrackParticleKey(),
                                     OutputTrackContainerName=egammaKeys.outputTrackKey(),
                                     DoTruth=rec.doTruth(),
                                     usePixel=DetFlags.haveRIO.pixel_on(),
                                     useSCT=DetFlags.haveRIO.SCT_on()
                                     )
