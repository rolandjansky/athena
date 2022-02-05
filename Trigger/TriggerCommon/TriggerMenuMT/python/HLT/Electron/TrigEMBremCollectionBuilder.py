# Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration

__doc__ = """ToolFactory to instantiate  TrigEMBremCollectionBuilder
with default configuration"""

import InDetRecExample.TrackingCommon as TrackingCommon
from AthenaCommon.DetFlags import DetFlags
from AthenaCommon.Logging import logging
# import base class
from egammaAlgs import egammaAlgsConf
from egammaRec.Factories import AlgFactory
from egammaTools.egammaExtrapolators import egammaExtrapolator

# default configuration of the EMBremCollectionBuilder
from InDetRecExample.InDetJobProperties import InDetFlags
from RecExConfig.RecFlags import rec

from TriggerMenuMT.HLT.Egamma.TrigEgammaKeys import getTrigEgammaKeys
TrigEgammaKeys = getTrigEgammaKeys('_GSF')



log = logging.getLogger(__name__)

class TrigEgammaBremCollectionBuilder (egammaAlgsConf.EMBremCollectionBuilder):
    __slots__ = ()

    def __init__(self, name="TrigEgammaBremCollectionBuilder", **kw):
        log.debug("entering")

        super(TrigEgammaBremCollectionBuilder, self).__init__(name, **kw)

        import egammaRec.EMCommonRefitter
        # import TriggerMenuMT.HLT.Electron.TrigEMCommonRefitter as TrigEMCommonRefitter
        # Extrapolator to be used for GSF (private)
        GSFBuildInDetExtrapolator = egammaExtrapolator()

        # GsfReffiter (private not in ToolSvc)
        from egammaTrackTools.egammaTrackToolsConf import egammaTrkRefitterTool
        GSFRefitterTool = egammaTrkRefitterTool(
            name='GSFRefitterTool',
            FitterTool=egammaRec.EMCommonRefitter.getGSFTrackFitter(),
            #FitterTool=TrigEMCommonRefitter.getGSFTrackFitter(),
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
        if DetFlags.haveRIO.TRT_on() and not InDetFlags.doHighPileup():

            from TrigInDetConfig.InDetTrigCollectionKeys import TrigTRTKeys
            from AthenaCommon.GlobalFlags import globalflags
            TrigTRTRDOs = "TRT_RDOs"
            if globalflags.DataSource() == "data":
                TrigTRTRDOs = TrigTRTKeys.RDOs

            TRT_LocalOccupancyTool = TrackingCommon.getInDetTRT_LocalOccupancy(
                TRT_RDOContainerName=TrigTRTRDOs,
                TRT_DriftCircleCollection="",
                isTrigger=True)

            TRT_ToT_dEdx_Tool = TrackingCommon.getInDetTRT_dEdxTool(
                    TRT_LocalOccupancyTool=TRT_LocalOccupancyTool,
                    AssociationTool="InDetTrigPrdAssociationTool")

            GSFBuildTRT_ElectronPidTool = TrackingCommon.getInDetTRT_ElectronPidTool(
                    name="GSFBuildTRT_ElectronPidTool",
                    private=True,
                    TRT_LocalOccupancyTool=TRT_LocalOccupancyTool,
                    TRT_ToT_dEdx_Tool=TRT_ToT_dEdx_Tool)

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
            isHLT = True,
            DoSharedHits=False,
            private=True)

        #
        #  TrkTrackSummaryTool: no shared hits  no hole search
        #
        GSFBuildInDetTrigTrackSummaryTool = (
            TrackingCommon.getInDetTrackSummaryTool(
                name="GSFBuildInDetTrigTrackSummaryTool",
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
            TrackSummaryTool="")
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
        self.TrackSummaryTool = GSFBuildInDetTrigTrackSummaryTool



""" This is an instance of GSF fitter tool will be used on track particles """
TrigEMBremCollectionBuilder = AlgFactory(TrigEgammaBremCollectionBuilder,
    doAdd = False,
    name='TrigEMBremCollectionBuilder',
    TrackParticleContainerName          = TrigEgammaKeys.precisionTrackingContainer,
    SelectedTrackParticleContainerName  = TrigEgammaKeys.precisionTrackingContainer,
    OutputTrkPartContainerName          = TrigEgammaKeys.precisionElectronTrackParticleContainerGSF,
    OutputTrackContainerName            = TrigEgammaKeys.precisionElectronTrkCollectionGSF,
    DoTruth=rec.doTruth(),
    usePixel=DetFlags.haveRIO.pixel_on(),
    useSCT=DetFlags.haveRIO.SCT_on()
    )
