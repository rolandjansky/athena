# Copyright (C) 2001-2019 CERN for the benefit of the ATLAS collaboration

__doc__ = "ToolFactory to instantiate  egammaBremCollectionBuilder with default configuration"
__author__ = "Christos"

# default configuration of the EMBremCollectionBuilder
from AthenaCommon.Logging import logging
from AthenaCommon.SystemOfUnits import *
from AthenaCommon.Constants import *

from AthenaCommon.AppMgr import ToolSvc
from AthenaCommon.DetFlags import DetFlags
from AthenaCommon.GlobalFlags  import globalflags
from RecExConfig.RecFlags  import rec
from InDetRecExample.InDetJobProperties import InDetFlags
import traceback
#import base class
from egammaAlgs import egammaAlgsConf
from egammaTools.InDetTools import egammaExtrapolator
from egammaRec.Factories import AlgFactory
from egammaRec import egammaKeys

class egammaBremCollectionBuilder ( egammaAlgsConf.EMBremCollectionBuilder ) :
    __slots__ = ()

    def __init__(self, name="EMBremCollectionBuilder", **kw):
        mlog = logging.getLogger(name+'::__init__')
        mlog.info("entering")

        super(egammaBremCollectionBuilder, self).__init__(name, **kw)

        # do the configuration
        import egammaRec.EMCommonRefitter
        GSFBuildInDetExtrapolator= egammaExtrapolator()

        from AthenaCommon.AppMgr import ToolSvc
        ToolSvc += GSFBuildInDetExtrapolator   # should be temporary

        from egammaTrackTools.egammaTrackToolsConf import egammaTrkRefitterTool
        from TrkExTools.AtlasExtrapolator import AtlasExtrapolator
        GSFRefitterTool = egammaTrkRefitterTool(name = 'GSFRefitterTool',
                                                FitterTool = egammaRec.EMCommonRefitter.getGSFTrackFitter(),
                                                useBeamSpot = False,
                                                Extrapolator = AtlasExtrapolator(),
                                                ReintegrateOutliers=True)
        #
        # Load association tool from Inner Detector to handle pixel ganged ambiguities
        #
        from InDetAssociationTools.InDetAssociationToolsConf import InDet__InDetPRD_AssociationToolGangedPixels
        GSFBuildInDetPrdAssociationTool = InDet__InDetPRD_AssociationToolGangedPixels(name = "GSFBuildInDetPrdAssociationTool",
                                                                                      PixelClusterAmbiguitiesMapName = 'PixelClusterAmbiguitiesMap')
        ToolSvc += GSFBuildInDetPrdAssociationTool        

        if DetFlags.haveRIO.pixel_on():
            from AthenaCommon.AthenaCommonFlags import athenaCommonFlags
            from PixelConditionsTools.PixelConditionsSummaryToolSetup import PixelConditionsSummaryToolSetup
            pixelConditionsSummaryToolSetup = PixelConditionsSummaryToolSetup()
            pixelConditionsSummaryToolSetup.setUseConditions(True)
            pixelConditionsSummaryToolSetup.setUseDCSState((globalflags.DataSource=='data') and InDetFlags.usePixelDCS())
            pixelConditionsSummaryToolSetup.setUseByteStream((globalflags.DataSource=='data'))
            pixelConditionsSummaryToolSetup.setUseTDAQ(False)
            pixelConditionsSummaryToolSetup.setUseDeadMap(True)
            pixelConditionsSummaryToolSetup.setup()

            InDetPixelConditionsSummaryTool = pixelConditionsSummaryToolSetup.getTool()

            if InDetFlags.usePixelDCS():
                InDetPixelConditionsSummaryTool.IsActiveStates = [ 'READY', 'ON', 'UNKNOWN', 'TRANSITION', 'UNDEFINED' ]
                InDetPixelConditionsSummaryTool.IsActiveStatus = [ 'OK', 'WARNING', 'ERROR', 'FATAL' ]

 
        if (DetFlags.haveRIO.SCT_on()):
            from SCT_ConditionsTools.SCT_ConditionsSummaryToolSetup import SCT_ConditionsSummaryToolSetup
            sct_ConditionsSummaryToolSetup = SCT_ConditionsSummaryToolSetup()
            sct_ConditionsSummaryToolSetup.setup()
            InDetSCT_ConditionsSummaryTool = sct_ConditionsSummaryToolSetup.getTool()
        #
        #  Load BLayer tool
        #
        GSFBuildTestBLayerTool = None
        if DetFlags.haveRIO.pixel_on():
            from InDetTestBLayer.InDetTestBLayerConf import InDet__InDetTestBLayerTool

            GSFBuildTestBLayerTool = InDet__InDetTestBLayerTool(name            = "GSFBuildTestBLayerTool",
                                                                PixelSummaryTool = InDetPixelConditionsSummaryTool,
                                                                Extrapolator    = GSFBuildInDetExtrapolator)
            ToolSvc += GSFBuildTestBLayerTool
        #
        #  Configurable version of TRT_ElectronPidTools
        #
        GSFBuildTRT_ElectronPidTool = None
        if DetFlags.haveRIO.TRT_on() and not InDetFlags.doSLHC() and not InDetFlags.doHighPileup() :
         

            # Calibration DB Service
            from TRT_ConditionsServices.TRT_ConditionsServicesConf import TRT_CalDbTool
            InDetTRTCalDbTool = TRT_CalDbTool(name = "TRT_CalDbTool")

            # Straw status DB Tool
            from TRT_ConditionsServices.TRT_ConditionsServicesConf import TRT_StrawStatusSummaryTool
            InDetTRTStrawStatusSummaryTool = TRT_StrawStatusSummaryTool(name = "TRT_StrawStatusSummaryTool",
                                                                    isGEANT4=(globalflags.DataSource == 'geant4'))

   
            from TRT_ElectronPidTools.TRT_ElectronPidToolsConf import InDet__TRT_LocalOccupancy
            GSFBuildTRT_LocalOccupancy = InDet__TRT_LocalOccupancy(name ="GSF_TRT_LocalOccupancy",
                                                                   isTrigger			= False,
                                                                   TRTCalDbTool = InDetTRTCalDbTool,
                                                                   TRTStrawStatusSummaryTool = InDetTRTStrawStatusSummaryTool )

            ToolSvc += GSFBuildTRT_LocalOccupancy
            
            from TRT_ElectronPidTools.TRT_ElectronPidToolsConf import InDet__TRT_ElectronPidToolRun2
            GSFBuildTRT_ElectronPidTool = InDet__TRT_ElectronPidToolRun2(name   = "GSFBuildTRT_ElectronPidTool",
                                                                         TRT_LocalOccupancyTool = GSFBuildTRT_LocalOccupancy,
                                                                         TRTStrawSummaryTool    = InDetTRTStrawStatusSummaryTool,
                                                                         isData = (globalflags.DataSource == 'data') )
            
            ToolSvc += GSFBuildTRT_ElectronPidTool


        #
        #  Configurable version of PixelToTPIDTOol
        #
        GSFBuildPixelToTPIDTool = None
        if DetFlags.haveRIO.pixel_on():                 
            from PixelToTPIDTool.PixelToTPIDToolConf import InDet__PixelToTPIDTool
            GSFBuildPixelToTPIDTool = InDet__PixelToTPIDTool(name = "GSFBuildPixelToTPIDTool")
            ToolSvc += GSFBuildPixelToTPIDTool
        #
        #  Configrable version of loading the InDetTrackSummaryHelperTool
        #
        from InDetTrackSummaryHelperTool.InDetTrackSummaryHelperToolConf import InDet__InDetTrackSummaryHelperTool
        GSFBuildTrackSummaryHelperTool = InDet__InDetTrackSummaryHelperTool(name            = "GSFBuildTrackSummaryHelperTool",
                                                                            AssoTool        = GSFBuildInDetPrdAssociationTool,
                                                                            PixelToTPIDTool = GSFBuildPixelToTPIDTool,
                                                                            TestBLayerTool  = GSFBuildTestBLayerTool,
                                                                            DoSharedHits    = False,
                                                                            usePixel        = DetFlags.haveRIO.pixel_on(),
                                                                            useSCT          = DetFlags.haveRIO.SCT_on(),
                                                                            useTRT          = DetFlags.haveRIO.TRT_on())
        ToolSvc += GSFBuildTrackSummaryHelperTool
        #
        #  Configurable version of TrkTrackSummaryTool: no TRT_PID tool needed here (no shared hits)
        #
        from TrkTrackSummaryTool.TrkTrackSummaryToolConf import Trk__TrackSummaryTool
        GSFBuildInDetTrackSummaryTool = Trk__TrackSummaryTool(name = "GSFBuildInDetTrackSummaryTool",
                                                              InDetSummaryHelperTool = GSFBuildTrackSummaryHelperTool,
                                                              doSharedHits           = False,
                                                              doHolesInDet           = False,
                                                              TRT_ElectronPidTool    = GSFBuildTRT_ElectronPidTool,
                                                              PixelToTPIDTool        = GSFBuildPixelToTPIDTool)
        ToolSvc += GSFBuildInDetTrackSummaryTool
        #
        #  load patricle creator tool
        #
        from TrkParticleCreator.TrkParticleCreatorConf import Trk__TrackParticleCreatorTool
        GSFBuildInDetParticleCreatorTool = Trk__TrackParticleCreatorTool(name                    = "GSFBuildInDetParticleCreatorTool",
                                                                         KeepParameters          = True,
                                                                         Extrapolator            = GSFBuildInDetExtrapolator,
                                                                         TrackSummaryTool        = GSFBuildInDetTrackSummaryTool,
                                                                         UseTrackSummaryTool     = False)
        #
        #  do track slimming
        #
        from TrkTrackSlimmingTool.TrkTrackSlimmingToolConf import Trk__TrackSlimmingTool as ConfigurableTrackSlimmingTool
        GSFBuildInDetTrkSlimmingTool = ConfigurableTrackSlimmingTool(name  = "GSFBuildInDetTrackSlimmingTool",
                                                                     KeepParameters = False,
                                                                     KeepOutliers   = True )
        # 
	#  Default Configuration
        #
        self.TrackRefitTool= GSFRefitterTool
        self.TrackParticleCreatorTool=GSFBuildInDetParticleCreatorTool
        self.TrackSlimmingTool=GSFBuildInDetTrkSlimmingTool
        self.TrackSummaryTool=GSFBuildInDetTrackSummaryTool

from egammaTrackTools.egammaTrackToolsFactories import EMExtrapolationTools
from InDetRecExample.InDetKeys import InDetKeys
EMBremCollectionBuilder = AlgFactory( egammaBremCollectionBuilder,
                                      name = 'EMBremCollectionBuilder',
                                      ExtrapolationTool = EMExtrapolationTools,
                                      TrackParticleContainerName=InDetKeys.xAODTrackParticleContainer(),
                                      OutputTrkPartContainerName=egammaKeys.outputTrackParticleKey(),
                                      OutputTrackContainerName=egammaKeys.outputTrackKey(),  
                                      DoTruth=rec.doTruth(),
                                      usePixel      = DetFlags.haveRIO.pixel_on(),
                                      useSCT        = DetFlags.haveRIO.SCT_on()
                                      )
