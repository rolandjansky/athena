# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

"""
#
# the configuration of tools for processing of FTK tracks
#
"""

___author___ = "Jiri Masik"
___version___ = "$Id: TrigInDetRecToolsFTK.py 788416 2016-12-07 14:18:16Z baines $"


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
     InDetTrigRefitRotCreator, InDetTrigUpdator, InDetTrigMaterialUpdator
from TrkDetDescrSvc.AtlasTrackingGeometrySvc import AtlasTrackingGeometrySvc

from FTK_RecTools.FTK_RecToolsConf import FTK_PixelClusterOnTrackTool, FTK_SCTClusterOnTrackTool

from InDetTrigRecExample.InDetTrigConditionsAccess import PixelConditionsSetup
 
InDetTrigBroadPixelClusterOnTrackToolFTK = \
                                         FTK_PixelClusterOnTrackTool("InDetTrigBroadPixelClusterOnTrackToolFTK",
                                                                     ErrorStrategy = 0) # use broad errors
 
  
ToolSvc += InDetTrigBroadPixelClusterOnTrackToolFTK
if (InDetTrigFlags.doPrintConfigurables()):
  print InDetTrigBroadPixelClusterOnTrackToolFTK
  
# SiLorentzAngleTool for SCT
from SiLorentzAngleSvc.SCTLorentzAngleToolSetup import SCTLorentzAngleToolSetup
sctLorentzAngleToolSetup = SCTLorentzAngleToolSetup()

InDetTrigBroadSCT_ClusterOnTrackToolFTK = FTK_SCTClusterOnTrackTool("InDetTrigBroadSCT_ClusterOnTrackToolFTK",
                                                                    CorrectionStrategy = 0,  # do correct position bias
                                                                    ErrorStrategy      = 0,  # do use broad errors
                                                                    LorentzAngleTool = sctLorentzAngleToolSetup.SCTLorentzAngleTool)
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
  print InDetTrigTrackFitterFTK


from InDetTrigRecExample.InDetTrigConfigRecLoadTools import InDetTrigExtrapolator,InDetTrigHoleSearchTool,InDetTrigPrdAssociationTool 


from InDetTrackSummaryHelperTool.InDetTrackSummaryHelperToolConf import InDet__InDetTrackSummaryHelperTool
InDetTrigTrackSummaryHelperToolFTK = InDet__InDetTrackSummaryHelperTool(name = "InDetTrigTrackSummaryHelperToolFTK",
                                                                        AssoTool     = InDetTrigPrdAssociationTool,
                                                                        DoSharedHits = False,
                                                                        HoleSearch   = InDetTrigHoleSearchTool,
                                                                        TestBLayerTool = None,
                                                                        PixelToTPIDTool = None,
                                                                        usePixel=True,
                                                                        useSCT=True,
                                                                        useTRT=False
                                                                        )

ToolSvc += InDetTrigTrackSummaryHelperToolFTK
if (InDetTrigFlags.doPrintConfigurables()):
  print InDetTrigTrackSummaryHelperToolFTK
        

        
from TrkTrackSummaryTool.TrkTrackSummaryToolConf import Trk__TrackSummaryTool
InDetTrigTrackSummaryToolFTK = Trk__TrackSummaryTool(name = "InDetTrigTrackSummaryToolFTK",
                                                 InDetSummaryHelperTool = InDetTrigTrackSummaryHelperToolFTK,
                                                 InDetHoleSearchTool    = None,
                                                 doSharedHits           = False,
                                                 TRT_ElectronPidTool    = None
                                                 )
ToolSvc += InDetTrigTrackSummaryToolFTK
if (InDetTrigFlags.doPrintConfigurables()):
  print InDetTrigTrackSummaryToolFTK


from TrkParticleCreator.TrkParticleCreatorConf import Trk__TrackParticleCreatorTool


InDetTrigTrackParticleCreatorToolFTK = Trk__TrackParticleCreatorTool( name = "InDetTrigParticleCreatorToolFTK",
                                                                      Extrapolator = InDetTrigExtrapolator,
                                                                      TrackSummaryTool = InDetTrigTrackSummaryToolFTK,
                                                                      KeepParameters = True,
                                                                      ForceTrackSummaryUpdate = False,  #summary update moved (in the slimmer now)
                                                                      )
        
ToolSvc += InDetTrigTrackParticleCreatorToolFTK
if (InDetTrigFlags.doPrintConfigurables()):
  print InDetTrigTrackParticleCreatorToolFTK


from FTK_DataProviderSvc.FTK_DataProviderSvcConf import FTK_UncertaintyTool
TrigFTK_UncertaintyTool= FTK_UncertaintyTool(name="FTK_UncertaintyTool")
ToolSvc+=TrigFTK_UncertaintyTool
if (InDetTrigFlags.doPrintConfigurables()):
  print TrigFTK_UncertaintyTool

from TrigInDetConf.TrigInDetRecVtxTools import InDetTrigVxEdmCnv  
from FTK_RecTools.FTK_RecToolsConf import FTK_VertexFinderTool
TrigFTK_RawVertexFinderTool=  FTK_VertexFinderTool(name="FTK_RawVertexFinderTool",
                                                   VertexInternalEdmFactory=InDetTrigVxEdmCnv)
ToolSvc+=TrigFTK_RawVertexFinderTool
if (InDetTrigFlags.doPrintConfigurables()):
  print TrigFTK_RawVertexFinderTool

from TrkVertexWeightCalculators.TrkVertexWeightCalculatorsConf import Trk__SumPtVertexWeightCalculator
TrigFTK_VertexWeightCalculator = Trk__SumPtVertexWeightCalculator(name = "TrigFTK_SumPtVertexWeightCalculator",
                                                                  DoSumPt2Selection = True)
if (InDetTrigFlags.doPrintConfigurables()):
  print TrigFTK_VertexWeightCalculator 
 

ToolSvc += TrigFTK_VertexWeightCalculator 

from TrkVertexTools.TrkVertexToolsConf import Trk__VertexCollectionSortingTool
TrigFTK_VertexCollectionSortingTool = Trk__VertexCollectionSortingTool(name                   = "TrigFTK_VertexCollectionSortingTool",
                                                                       VertexWeightCalculator = TrigFTK_VertexWeightCalculator)
ToolSvc += TrigFTK_VertexCollectionSortingTool
if (InDetTrigFlags.doPrintConfigurables()):
  print TrigFTK_VertexCollectionSortingTool

from FTK_RecTools.FTK_RecToolsConf import FTK_HashIDTool
TrigFTK_HashIDTool = FTK_HashIDTool("TrigFTK_HashIDTool")
ToolSvc+=TrigFTK_HashIDTool

