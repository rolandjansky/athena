# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

"""
#
# the configuration of tools for processing of FTK tracks
#
"""

___author___ = "Jiri Masik"
___version___ = "$Id: TrigInDetRecToolsFTK.py 786281 2016-11-24 12:48:17Z baines $"


from AthenaCommon.AppMgr import ToolSvc
from InDetTrigRecExample.InDetTrigFlags import InDetTrigFlags

from InDetTrigRecExample.ConfiguredNewTrackingTrigCuts import FTKTrackingCuts

from InDetAmbiTrackSelectionTool.InDetAmbiTrackSelectionToolConf import InDet__InDetAmbiTrackSelectionTool
InDetTrigAmbiTrackSelectionToolFTK = \
                                   InDet__InDetAmbiTrackSelectionTool(name               = 'InDetTrigAmbiTrackSelectionToolFTK',
                                                                      #AssociationTool    = None,
                                                                      #DriftCircleCutTool = None,
                                                                      minHits         = FTKTrackingCuts.minClusters(),
                                                                      minNotShared    = FTKTrackingCuts.minSiNotShared(),
                                                                      maxShared       = FTKTrackingCuts.maxShared(),
                                                                      minTRTHits      = 0,  # used for Si only tracking !!!
                                                                      Cosmics         = False,  #there is a different instance
                                                                      UseParameterization = False,
                                                                      )
    
   
ToolSvc += InDetTrigAmbiTrackSelectionToolFTK

if (InDetTrigFlags.doPrintConfigurables()):
  print InDetTrigAmbiTrackSelectionToolFTK


from TrkGlobalChi2Fitter.TrkGlobalChi2FitterConf import Trk__GlobalChi2Fitter
from InDetTrigRecExample.InDetTrigConfigRecLoadTools import InDetTrigExtrapolator, InDetTrigNavigator, InDetTrigPropagator, \
     InDetTrigRefitRotCreator,InDetTrigUpdator, InDetTrigMaterialUpdator
from TrkDetDescrSvc.AtlasTrackingGeometrySvc import AtlasTrackingGeometrySvc

from InDetTrigRecExample.InDetTrigConditionsAccess import PixelConditionsSetup

from FTK_RecTools.FTK_RecToolsConf import FTK_PixelClusterOnTrackTool, FTK_SCTClusterOnTrackTool


InDetTrigBroadPixelClusterOnTrackToolFTK = \
                                         FTK_PixelClusterOnTrackTool("InDetTrigBroadPixelClusterOnTrackToolFTK",
                                                                     PixelOfflineCalibSvc=PixelConditionsSetup.instanceName('PixelOfflineCalibSvc'),
                                                                     ErrorStrategy = 0) # use broad errors
ToolSvc += InDetTrigBroadPixelClusterOnTrackToolFTK
if (InDetTrigFlags.doPrintConfigurables()):
  print InDetTrigBroadPixelClusterOnTrackToolFTK
  

InDetTrigBroadSCT_ClusterOnTrackToolFTK = FTK_SCTClusterOnTrackTool("InDetTrigBroadSCT_ClusterOnTrackToolFTK",
                                                                    CorrectionStrategy = 0,  # do correct position bias
                                                                    ErrorStrategy      = 0)  # do use broad errors
ToolSvc += InDetTrigBroadSCT_ClusterOnTrackToolFTK
if (InDetTrigFlags.doPrintConfigurables()):
  print InDetTrigBroadSCT_ClusterOnTrackToolFTK
  
  
from TrkRIO_OnTrackCreator.TrkRIO_OnTrackCreatorConf import Trk__RIO_OnTrackCreator
InDetTrigRotCreatorFTK = Trk__RIO_OnTrackCreator(name            = 'InDetTrigRotCreatorFTK',
                                                 ToolPixelCluster= InDetTrigBroadPixelClusterOnTrackToolFTK,
                                                 ToolSCT_Cluster = InDetTrigBroadSCT_ClusterOnTrackToolFTK,
                                                 Mode            = 'indet')
ToolSvc += InDetTrigRotCreatorFTK
if (InDetTrigFlags.doPrintConfigurables()):
  print InDetTrigRotCreatorFTK
  

InDetTrigTrackFitterFTK = Trk__GlobalChi2Fitter(name = "InDetTrigTrackFitterFTK",
                                            ExtrapolationTool     = InDetTrigExtrapolator,
                                            NavigatorTool         = InDetTrigNavigator,
                                            PropagatorTool        = InDetTrigPropagator,     
                                            RotCreatorTool        = InDetTrigRotCreatorFTK,
                                            BroadRotCreatorTool   = None,
                                            MeasurementUpdateTool = InDetTrigUpdator,
                                            TrackingGeometrySvc   = AtlasTrackingGeometrySvc,
                                            MaterialUpdateTool    = InDetTrigMaterialUpdator,
                                            StraightLine          = not InDetTrigFlags.solenoidOn(),
                                            OutlierCut            = 4,
                                            SignedDriftRadius     = True,
                                            RecalibrateSilicon    = False,
                                            RecalibrateTRT        = False,
                                            ReintegrateOutliers   = False,
                                            TrackChi2PerNDFCut    = 9,
                                            TRTExtensionCuts      = True, 
                                            MaxIterations         = 40,
                                            Acceleration          = True,
                                            Momentum=1000.,
                                            )

ToolSvc += InDetTrigTrackFitterFTK
if (InDetTrigFlags.doPrintConfigurables()):
  printInDetTrigTrackFitterFTK
  
