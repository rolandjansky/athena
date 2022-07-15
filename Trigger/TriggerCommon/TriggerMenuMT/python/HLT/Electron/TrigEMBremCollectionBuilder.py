# Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration

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


log = logging.getLogger(__name__)

class TrigEgammaBremCollectionBuilder (egammaAlgsConf.EMBremCollectionBuilder):
    __slots__ = ()

    def __init__(self, name="TrigEgammaBremCollectionBuilder", **kw):
        log.debug("entering")

        super(TrigEgammaBremCollectionBuilder, self).__init__(name, **kw)

        import egammaRec.EMCommonRefitter
        # import TriggerMenuMT.HLT.Electron.TrigEMCommonRefitter as TrigEMCommonRefitter
        # Extrapolator to be used for GSF (public)
        GSFBuildInDetExtrapolator = egammaExtrapolator()
        from AthenaCommon.AppMgr import ToolSvc
        ToolSvc += GSFBuildInDetExtrapolator

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
        GSFBuildTestPixelLayerTool = None
        GSFBuildPixelToTPIDTool = None
        if DetFlags.haveRIO.pixel_on():
            GSFPixelConditionsSummaryTool = (
                TrackingCommon.getInDetPixelConditionsSummaryTool())
            if InDetFlags.usePixelDCS():
                GSFPixelConditionsSummaryTool.IsActiveStates = [
                    'READY', 'ON', 'UNKNOWN', 'TRANSITION', 'UNDEFINED']
                GSFPixelConditionsSummaryTool.IsActiveStatus = [
                    'OK', 'WARNING', 'ERROR', 'FATAL']

            GSFBuildTestPixelLayerTool = TrackingCommon.getInDetTrigTestPixelLayerToolInner(
                name="GSFBuildTestPixelLayerTool",
                PixelSummaryTool=GSFPixelConditionsSummaryTool,
                Extrapolator=GSFBuildInDetExtrapolator,
                CheckActiveAreas=False,
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
    
            TRT_LocalOccupancyTool = TrackingCommon.getInDetTRT_LocalOccupancy(
                TRT_DriftCircleCollection = TrigTRTKeys.DriftCircles,
                isTrigger=True)

            TRT_ToT_dEdx_Tool = TrackingCommon.getInDetTRT_dEdxTool(
                    TRT_LocalOccupancyTool=TRT_LocalOccupancyTool,
                    AssociationTool="InDetTrigPrdAssociationTool")

            GSFBuildTRT_ElectronPidTool = TrackingCommon.getInDetTRT_ElectronPidTool(
                name="GSFBuildTRT_ElectronPidTool",
                private=True,
                CalculateNNPid=False,
                MinimumTrackPtForNNPid=0.,
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
                AddExpectedHits=True)
        )
        #
        #  Track Particle Creator tool (private not in ToolSvc)
        #
        from TrkParticleCreator.TrkParticleCreatorConf import (
            Trk__TrackParticleCreatorTool)

        GSFBuildInDetParticleCreatorTool = Trk__TrackParticleCreatorTool(
            name="GSFBuildInDetParticleCreatorTool",
            TRT_ElectronPidTool=GSFBuildTRT_ElectronPidTool,
            PixelToTPIDTool=GSFBuildPixelToTPIDTool,
            TestPixelLayerTool=GSFBuildTestPixelLayerTool,
            ComputeAdditionalInfo=True,
            KeepParameters=True,
            TrackSummaryTool=GSFBuildInDetTrigTrackSummaryTool)
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


""" This is an instance of GSF fitter tool will be used on track particles """

def TrigEMBremCollectionBuilderCfg(variant, TrigEgammaKeys):
    TrigEMBremCollectionBuilder = AlgFactory(TrigEgammaBremCollectionBuilder,
        doAdd = False,
        name='TrigEMBremCollectionBuilder'+variant,
        TrackParticleContainerName          = TrigEgammaKeys.precisionTrackingContainer,
        SelectedTrackParticleContainerName  = TrigEgammaKeys.precisionTrackingContainer,
        OutputTrkPartContainerName          = TrigEgammaKeys.precisionElectronTrackParticleContainerGSF,
        OutputTrackContainerName            = TrigEgammaKeys.precisionElectronTrkCollectionGSF,
        DoTruth=rec.doTruth(),
        usePixel=DetFlags.haveRIO.pixel_on(),
        useSCT=DetFlags.haveRIO.SCT_on()
     )
    
    return TrigEMBremCollectionBuilder()
