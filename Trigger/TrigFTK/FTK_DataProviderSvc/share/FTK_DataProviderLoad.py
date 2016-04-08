from FTK_DataProviderSvc.FTK_DataProviderSvcConf import FTK_DataProviderSvc, FTK_UncertaintyTool

from AthenaCommon.Constants import *   # for "INFO"

# Track Fitter

from TrkGlobalChi2Fitter.TrkGlobalChi2FitterConf import Trk__GlobalChi2Fitter
from InDetTrigRecExample.InDetTrigConfigRecLoadTools import InDetTrigExtrapolator, InDetTrigNavigator, InDetTrigPropagator, InDetTrigRefitRotCreator,InDetTrigBroadInDetRotCreator, \
     InDetTrigUpdator, InDetTrigMaterialUpdator, InDetTrigUpdator, InDetTrigMaterialUpdator
from TrkDetDescrSvc.AtlasTrackingGeometrySvc import AtlasTrackingGeometrySvc

FTK_TrackFitter = Trk__GlobalChi2Fitter(name                  = 'FTK_TrackFitter',
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
ToolSvc += FTK_TrackFitter
print "added FTK_TrackFitter to ToolSvc"


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
print "added FTK_TrackSummaryHelperTool to ToolSvc"



from TrkTrackSummaryTool.TrkTrackSummaryToolConf import Trk__TrackSummaryTool
FTK_TrackSummaryTool = Trk__TrackSummaryTool(name = "FTK_TrackSummaryTool",
                                             InDetSummaryHelperTool = FTK_TrackSummaryHelperTool,
                                             InDetHoleSearchTool    = None,
                                             doSharedHits           = False,
                                             TRT_ElectronPidTool    = None
                                             )
ToolSvc += FTK_TrackSummaryTool
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
print " added FTK_ParticleCreatorTool to ToolSvc"


from TrigInDetConf.TrigInDetRecVtxTools import InDetTrigLinFactory, InDetTrigVxEdmCnv
from TrigInDetConf.TrigInDetRecVtxTools import InDetTrigLinFactory, InDetTrigVxEdmCnv

from TrkVertexSeedFinderTools.TrkVertexSeedFinderToolsConf import Trk__CrossDistancesSeedFinder
FTK_VtxSeedFinder = Trk__CrossDistancesSeedFinder(name = "InDetTrigCrossDistancesSeedFinder",
                                                  trackdistcutoff = 1.,
                                                  trackdistexppower = 2)
ToolSvc += FTK_TrigVtxSeedFinder
 
from TrkVertexFitterUtils.TrkVertexFitterUtilsConf import \
     Trk__ImpactPoint3dEstimator, Trk__DetAnnealingMaker
    
FTK_ImpactPoint3dEstimator = Trk__ImpactPoint3dEstimator(name         = "InDetTrigTrkImpactPoint3dEstimator",
                                                         Extrapolator = InDetTrigExtrapolator,
                                                         )
        
ToolSvc += FTK_ImpactPoint3dEstimator

 
FTK_AnnealingMaker = Trk__DetAnnealingMaker(name = "InDetTrigTrkAnnealingMaker")
FTK_AnnealingMaker.SetOfTemperatures = [64.,16.,4.,2.,1.5,1.] # not default
ToolSvc += FTK_AnnealingMaker


from TrkVertexFitters.TrkVertexFittersConf import Trk__SequentialVertexSmoother
FTK_VertexSmoother =  Trk__SequentialVertexSmoother(name="InDetTrigSequentialVertexSmoother")
ToolSvc += FTK_VertexSmoother


from TrkVertexFitters.TrkVertexFittersConf import Trk__AdaptiveVertexFitter
FTK_VxFitterTool = Trk__AdaptiveVertexFitter(name                         = "InDetTrigAdaptiveVxFitterTool",
                                             SeedFinder                   = FTK_VtxSeedFinder,
                                             LinearizedTrackFactory       = InDetTrigLinFactory,
                                             ImpactPoint3dEstimator       = FTK_ImpactPoint3dEstimator,
                                             AnnealingMaker               = FTK_AnnealingMaker,
                                             #TrackCompatibilityEstimator = 
                                             VertexSmoother = FTK_VertexSmoother,
                                             #VertexUpdator = ,  #does not use any tools?
                                             #onlyzseed = ,
                                             )

FTK_VxFitterTool.XAODConverter = InDetTrigVxEdmCnv
ToolSvc += FTK_VxFitterTool

from InDetPriVxFinderTool.InDetPriVxFinderToolConf import InDet__InDetIterativePriVxFinderTool
FTK_PriVxFinderTool = InDet__InDetIterativePriVxFinderTool(name             = "FTK_PriVxFinderTool",
                                                           VertexFitterTool = FTK_VxFitterTool,
                                                           TrackSelector    = InDetTrigTrackSelectorTool,
                                                           SeedFinder = FTK_VtxSeedFinder,
                                                           ImpactPoint3dEstimator = FTK_ImpactPoint3dEstimator,
                                                           LinearizedTrackFactory = InDetTrigLinFactory,
                                                           useBeamConstraint = True,
                                                           significanceCutSeeding = 12,
                                                           maximumChi2cutForSeeding = 49,
                                                           maxVertices = 200,
                                                           InternalEdmFactory =InDetTrigVxEdmCnv
                                                           )
ToolSvc += FTK_PriVxFinderTool

from FTK_DataProviderSvc.FTK_DataProviderSvc_Config import ConfiguredFTK_UncertaintyTool
FTK_UncertaintyTool= ConfiguredFTK_UncertaintyTool(name="FTK_UncertaintyTool")
ToolSvc+=FTK_UncertaintyTool


   

print "FTK_DataProviderLoad.py loaded all FTK tools: ToolSvc=",ToolSvc



from AthenaCommon.AppMgr import ServiceMgr as svcMgr
from FTK_DataProviderSvc.FTK_DataProviderSvcConf import FTK_DataProviderSvc
#if not hasattr(ServiceMgr,"FTK_DataProviderSvc"):
FTK_DataProviderSvc = FTK_DataProviderSvc("ConfiguredFTK_DataProviderSvc",
                                          TrackFitter = FTK_TrackFitter,
                                          UncertaintyTool=FTK_UncertaintyTool,
                                          TrackSummaryTool = FTK_TrackSummaryTool,
                                          TrackParticleCreatorTool = FTK_ParticleCreatorTool,
                                          VertexFinderTool =  FTK_PriVxFinderTool,
                                          OutputLevel=VERBOSE);
FTK_DataProviderSvc.OutputLevel=VERBOSE
ServiceMgr += FTK_DataProviderSvc
ConfiguredFTK_DataProviderSvc.OutputLevel=VERBOSE
ServiceMgr.ConfiguredFTK_DataProviderSvc.OutputLevel=VERBOSE
print FTK_DataProviderSvc
