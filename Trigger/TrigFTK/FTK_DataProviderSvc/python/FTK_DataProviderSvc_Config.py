# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

from FTK_DataProviderSvc.FTK_DataProviderSvcConf import FTK_DataProviderSvc, FTK_UncertaintyTool

from AthenaCommon.Constants import *   # for "INFO"

#class ConfiguredFTK_DataProviderSvc(FTK_DataProviderSvc) :
#    def __init__(self, name = "ConfiguredFTK_DataProviderSvc", DoTrackFit = False, TrackFitter = None):
#        FTK_DataProviderSvc.__init__(self, name)
#        from AthenaCommon.AppMgr import ToolSvc
#        if TrackFitter is None:
#            from TrkDistributedKalmanFilter.DistributedKalmanFilter_Config import ConfiguredDistributedKalmanFilter
#            myTrackFitter = ConfiguredDistributedKalmanFilter()
#            ToolSvc += myTrackFitter
#            self.DoFit = DoTrackFit
#            self.TrackFitter = myTrackFitter
class ConfiguredFTK_UncertaintyTool(FTK_UncertaintyTool) :
    def __init__(self, name = "ConfiguredFTK_UncertaintyTool"):
        FTK_UncertaintyTool.__init__(self, name) 

class ConfiguredFTK_DataProviderSvc(FTK_DataProviderSvc) :
    def __init__(self, name = "ConfiguredFTK_DataProviderSvc", TrackFitter="GX2", OutputLevel=INFO):
        print "In FTK_DataProviderSvc_Config.py"  
        FTK_DataProviderSvc.__init__(self, name)
        from AthenaCommon.AppMgr import ToolSvc
        print "configuring DataProviderSvc to use GX2 fitter"  
        
        from TrkGlobalChi2Fitter.TrkGlobalChi2FitterConf import Trk__GlobalChi2Fitter
        from InDetTrigRecExample.InDetTrigConfigRecLoadTools import InDetTrigExtrapolator, InDetTrigNavigator, InDetTrigPropagator, InDetTrigRefitRotCreator,InDetTrigBroadInDetRotCreator, \
             InDetTrigUpdator, InDetTrigMaterialUpdator, InDetTrigUpdator, InDetTrigMaterialUpdator
        from TrkDetDescrSvc.AtlasTrackingGeometrySvc import AtlasTrackingGeometrySvc
        TrigFTKTrackFitter = Trk__GlobalChi2Fitter(name                  = 'TrigFTKTrackFitter',
                                                   ExtrapolationTool     = InDetTrigExtrapolator,
                                                   NavigatorTool         = InDetTrigNavigator,
                                                   PropagatorTool        = InDetTrigPropagator,     
                                                   RotCreatorTool        = InDetTrigRefitRotCreator,
                                                   BroadRotCreatorTool   = InDetTrigBroadInDetRotCreator,
                                                MeasurementUpdateTool = InDetTrigUpdator,
                                                   TrackingGeometrySvc   = AtlasTrackingGeometrySvc,
                                                   MaterialUpdateTool    = InDetTrigMaterialUpdator,
                                                   StraightLine          = False,
                                                   OutlierCut            = 4,
                                                   SignedDriftRadius     = True,
                                                   RecalibrateSilicon    = True,
                                                   RecalibrateTRT        = True,
                                                   ReintegrateOutliers   = True,
                                                   TrackChi2PerNDFCut    = 7,
                                                TRTExtensionCuts      = False, 
                                                   MaxIterations         = 40,
                                                Acceleration          = True,
                                                   Momentum=1000.,
                                                   )
        ToolSvc += TrigFTKTrackFitter
        self.TrackFitter = TrigFTKTrackFitter
        print "added TrigFTKTrackFitter to ToolSvc"
        print "ToolSvc=",ToolSvc 

        self.OutputLevel=OutputLevel

        from FTK_DataProviderSvc.FTK_DataProviderSvc_Config import ConfiguredFTK_UncertaintyTool
        ConfiguredFTK_UncertaintyTool= ConfiguredFTK_UncertaintyTool(name="FTK_UncertaintyTool")
        ToolSvc+=ConfiguredFTK_UncertaintyTool
        self.UncertaintyTool=ConfiguredFTK_UncertaintyTool
        



        from InDetTrigRecExample.InDetTrigConfigRecLoadTools import InDetTrigExtrapolator,InDetTrigTrackSelectorTool,InDetTrigHoleSearchTool
        from InDetTrigRecExample.InDetTrigConfigRecLoadTools import InDetTrigTrackSummaryToolSharedHits, InDetTrigPrdAssociationTool 


        from InDetTrackSummaryHelperTool.InDetTrackSummaryHelperToolConf import InDet__InDetTrackSummaryHelperTool
        FTK_TrackSummaryHelperTool = InDet__InDetTrackSummaryHelperTool(name = "FTK_TrackSummaryHelperTool",
                                                                        AssoTool     = InDetTrigPrdAssociationTool,
                                                                        DoSharedHits = False,
                                                                        HoleSearch   = InDetTrigHoleSearchTool,
                                                                        TestBLayerTool = None,
                                                                        PixelToTPIDTool = None,
                                                                        usePixel=False,
                                                                        useSCT=False,
                                                                        useTRT=False
                                                                        )

        ToolSvc += FTK_TrackSummaryHelperTool



        from TrkTrackSummaryTool.TrkTrackSummaryToolConf import Trk__TrackSummaryTool
        FTK_TrackSummaryTool = Trk__TrackSummaryTool(name = "FTK_TrackSummaryTool",
                                                     InDetSummaryHelperTool = FTK_TrackSummaryHelperTool,
                                                     InDetHoleSearchTool    = None,
                                                     doSharedHits           = False,
                                                     TRT_ElectronPidTool    = None
                                                     )
        ToolSvc += FTK_TrackSummaryTool
        self.TrackSummaryTool = FTK_TrackSummaryTool
        print "added FTK_TrackSummaryTool to ToolSvc"


        from TrkParticleCreator.TrkParticleCreatorConf import Trk__TrackParticleCreatorTool
        

        FTK_ParticleCreatorTool = Trk__TrackParticleCreatorTool( name = " FTK_ParticleCreatorTool",
                                                                      Extrapolator = InDetTrigExtrapolator,
                                                                      TrackSummaryTool = FTK_TrackSummaryHelperTool,
                                                                      KeepParameters = True,
                                                                      ForceTrackSummaryUpdate = False,  #summary update moved (in the slimmer now)
                                                                      )
        
        ToolSvc += FTK_ParticleCreatorTool
        print FTK_ParticleCreatorTool

        self.TrackParticleCreatorTool = FTK_ParticleCreatorTool


        print "ToolSvc=",ToolSvc
#        from TrigInDetConf.TrigInDetRecCommonTools import InDetTrigFastTrackSummaryTool
#        self.TrackSummaryTool = InDetTrigFastTrackSummaryTool
#        print self
        
#        from InDetTrigRecExample.InDetTrigConfigRecLoadTools import InDetTrigHoleSearchTool,InDetTrigPrdAssociationTool,InDetTrigPixelToTPIDTool

#        from InDetTrackSummaryHelperTool.InDetTrackSummaryHelperToolConf import InDet__InDetTrackSummaryHelperTool
#        FTK_TrackSummaryHelperTool = InDet__InDetTrackSummaryHelperTool(name          = "FTK_TrackSummaryHelperTool",
                                                                        
                                                                        
 #                                                                      HoleSearch    = InDetTrigHoleSearchTool,
  #                                                                     AssoTool      = InDetTrigPrdAssociationTool,
   #                                                                     TestBLayerTool = None,
    #                                                                    PixelToTPIDTool= InDetTrigPixelToTPIDTool,
     #                                                                   DoSharedHits  = False,
      #                                                                  usePixel      = True,
       #                                                                 useSCT        = True, 
        #                                                                useTRT        = False)
     #
      #  ToolSvc +=  FTK_TrackSummaryHelperTool
#
