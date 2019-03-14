# Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration

from TrigFTKTrackConverter.TrigFTKTrackConverterConf import  TrigFTKTrackConverter
from InDetTrigRecExample.InDetTrigConditionsAccess import PixelConditionsSetup

class ConfiguredTrigFTKTrackConverter(TrigFTKTrackConverter) :
    def __init__(self, name = "ConfiguredTrigFTKTrackConverter", DoTrackFit = False, DoTruth = False, TrackFitter = None, UsePixelCalibSvc = True):
        TrigFTKTrackConverter.__init__(self, name) 
        from AthenaCommon.AppMgr import ToolSvc 
        if TrackFitter is None : 
            from TrkDistributedKalmanFilter.DistributedKalmanFilter_Config import ConfiguredDistributedKalmanFilter
            myTrackFitter = ConfiguredDistributedKalmanFilter()
            ToolSvc += myTrackFitter
            self.DoFit = DoTrackFit
            self.DoTruth = DoTruth
            self.TrackFitter = myTrackFitter
            
        self.UsePixelCalibSvc = UsePixelCalibSvc

class ConfiguredTrigFTKTrackConverterWithGX2(TrigFTKTrackConverter) :
    def __init__(self, name = "ConfiguredTrigFTKTrackConverter", DoTrackFit = False, DoTruth = False, TrackFitter = None, UsePixelCalibSvc = True):
        TrigFTKTrackConverter.__init__(self, name) 
        from AthenaCommon.AppMgr import ToolSvc
        from TrkGlobalChi2Fitter.TrkGlobalChi2FitterConf import Trk__GlobalChi2Fitter
        from InDetTrigRecExample.InDetTrigConfigRecLoadTools import InDetTrigExtrapolator, \
            InDetTrigNavigator, InDetTrigPropagator, InDetTrigRefitRotCreator, InDetTrigBroadInDetRotCreator, \
            InDetTrigUpdator, AtlasTrackingGeometrySvc, InDetTrigMaterialUpdator
        TrigFTKTrackFitter = Trk__GlobalChi2Fitter(name                  = 'TrigFTKTrackFitter',
                                                   ExtrapolationTool     = InDetTrigExtrapolator,
                                                   NavigatorTool         = InDetTrigNavigator,
                                                   PropagatorTool        = InDetTrigPropagator,		
                                                   RotCreatorTool        = InDetTrigRefitRotCreator,
                                                   BroadRotCreatorTool   = InDetTrigBroadInDetRotCreator,
                                                   MeasurementUpdateTool = InDetTrigUpdator,
                                                   TrackingGeometrySvc   = AtlasTrackingGeometrySvc,
                                                   #MaterialUpdateTool    = InDetTrigMaterialUpdator,
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
        if TrackFitter is None : 
            self.DoFit = DoTrackFit
            self.DoTruth = False
            self.TrackFitter = TrigFTKTrackFitter
            
        self.UsePixelCalibSvc = UsePixelCalibSvc
            
from TrigFTKTrackConverter.TrigFTKTrackConverterConf import  TrigFTKUncertiantyTool

class ConfiguredTrigFTKUncertiantyTool(TrigFTKUncertiantyTool) :
    def __init__(self, name = "ConfiguredTrigFTKUncertiantyTool"):
        TrigFTKUncertiantyTool.__init__(self, name) 
        

from TrigFTKTrackConverter.TrigFTKTrackConverterConf import  TrigFTKClusterConverterTool

class ConfiguredTrigFTKTool(TrigFTKClusterConverterTool):
    def __init__(self, name = "ConfiguredTrigFTKClusterConverterTool"):
        TrigFTKClusterConverterTool.__init__(self, name) 
        from AthenaCommon.AppMgr import ToolSvc
        from SiLorentzAngleTool.SCTLorentzAngleToolSetup import SCTLorentzAngleToolSetup
        sctLorentzAngleToolSetup = SCTLorentzAngleToolSetup()
        TrigFTKClusterConverterTool.PixelLorentzAngleTool = ToolSvc.InDetTrigPixelLorentzAngleTool
        TrigFTKClusterConverterTool.SCTLorentzAngleTool = sctLorentzAngleToolSetup.SCTLorentzAngleTool
                                                            


