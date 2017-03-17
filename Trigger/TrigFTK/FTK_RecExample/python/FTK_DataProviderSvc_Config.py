# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

from FTK_DataProviderSvc.FTK_DataProviderSvcConf import FTK_DataProviderSvc, FTK_UncertaintyTool

from AthenaCommon.Constants import *   # for "INFO"


class ConfiguredFTK_DataProviderSvc(FTK_DataProviderSvc) :

    def __init__(self, name = "ConfiguredFTK_DataProviderSvc"):
        print "In FTK_DataProviderSvc_Config.py"  
        FTK_DataProviderSvc.__init__(self, name) 
        # Track Fitter
        from TrkGlobalChi2Fitter.TrkGlobalChi2FitterConf import Trk__GlobalChi2Fitter
        from InDetTrigRecExample.InDetTrigConfigRecLoadTools import InDetTrigExtrapolator, InDetTrigNavigator, InDetTrigPropagator, InDetTrigRefitRotCreator,InDetTrigBroadInDetRotCreator, \
             InDetTrigUpdator, InDetTrigMaterialUpdator, InDetTrigUpdator, InDetTrigMaterialUpdator
        from TrkDetDescrSvc.AtlasTrackingGeometrySvc import AtlasTrackingGeometrySvc

        from TrkDistributedKalmanFilter.DistributedKalmanFilter_Config import ConfiguredDistributedKalmanFilter

        FTK_TrackFitter = Trk__GlobalChi2Fitter(name = "FTK_TrackFitter",
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
        from AthenaCommon.AppMgr import ToolSvc

        print "printing FTK_TrackFitter configured with DKF"
        print FTK_TrackFitter
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
                                                                usePixel=True,
                                                                useSCT=True,
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


        FTK_TrackParticleCreatorTool = Trk__TrackParticleCreatorTool( name = "FTK_ParticleCreatorTool",
                                                         Extrapolator = InDetTrigExtrapolator,
                                                         TrackSummaryTool = FTK_TrackSummaryTool,
                                                         KeepParameters = True,
                                                         ForceTrackSummaryUpdate = False,  #summary update moved (in the slimmer now)
                                                         )
        
        ToolSvc += FTK_TrackParticleCreatorTool
        print FTK_TrackParticleCreatorTool
        print " added FTK_TrackParticleCreatorTool to ToolSvc"

        
        from TrigInDetConf.TrigInDetRecVtxTools import InDetTrigLinFactory, InDetTrigVxEdmCnv
        from TrigInDetConf.TrigInDetRecVtxTools import InDetTrigLinFactory, InDetTrigVxEdmCnv

        from TrkVertexSeedFinderTools.TrkVertexSeedFinderToolsConf import Trk__ZScanSeedFinder
        FTK_VtxSeedFinder = Trk__ZScanSeedFinder(name = "FTK_ZScanSeedFinder"
                                                    #Mode1dFinder = # default, no setting needed
                                                    )
        ToolSvc += FTK_VtxSeedFinder
 
        from TrkVertexFitterUtils.TrkVertexFitterUtilsConf import Trk__ImpactPoint3dEstimator, Trk__DetAnnealingMaker
    
        FTK_ImpactPoint3dEstimator = Trk__ImpactPoint3dEstimator(name         = "FTK_TrkImpactPoint3dEstimator",
                                                         Extrapolator = InDetTrigExtrapolator,
                                                         )
        
        ToolSvc += FTK_ImpactPoint3dEstimator

 
        FTK_AnnealingMaker = Trk__DetAnnealingMaker(name = "FTK_TrkAnnealingMaker")
        FTK_AnnealingMaker.SetOfTemperatures = [64.,16.,4.,2.,1.5,1.] # not default
        ToolSvc += FTK_AnnealingMaker


        from TrkVertexFitters.TrkVertexFittersConf import Trk__SequentialVertexSmoother
        FTK_VertexSmoother =  Trk__SequentialVertexSmoother(name="FTK_SequentialVertexSmoother")
        ToolSvc += FTK_VertexSmoother


        from TrkVertexFitters.TrkVertexFittersConf import Trk__AdaptiveVertexFitter
        FTK_VxFitterTool = Trk__AdaptiveVertexFitter(name                         = "FTK_AdaptiveVxFitterTool",
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


        from FTK_DataProviderSvc.FTK_DataProviderSvcConf import FTK_UncertaintyTool
        theFTK_UncertaintyTool= FTK_UncertaintyTool(name="FTK_UncertaintyTool")
        ToolSvc+=theFTK_UncertaintyTool

        from FTK_RecTools.FTK_RecToolsConf import FTK_VertexFinderTool
        FTK_RawVertexFinderTool=  FTK_VertexFinderTool(name="FTK_RawVertexFinderTool",
                                                           VertexInternalEdmFactory=InDetTrigVxEdmCnv)
        ToolSvc+=FTK_RawVertexFinderTool

        self.TrackCollectionName= "FTK_TrackCollection"
        self.TrackParticleContainerName= "FTK_TrackParticleContainer"
        self.VxContainerName="FTK_VxContainer"
        self.VertexContainerName="FTK_VertexContainer"	  
        self.PixelTruthName="FTK_PRD_MultiTruthPixel"
        self.SctTruthName="FTK_PRD_MultiTruthSCT"
        self.PixelClusterContainerName= "FTK_PixelClusterContainer"
        self.SCT_ClusterContainerName= "FTK_SCT_ClusterContainer"

        self.TrackFitter = FTK_TrackFitter
        self.UncertaintyTool=theFTK_UncertaintyTool
        self.TrackSummaryTool=FTK_TrackSummaryTool
        self.TrackParticleCreatorTool=FTK_TrackParticleCreatorTool
        self.RawVertexFinderTool=FTK_RawVertexFinderTool
        self.VertexFinderTool=FTK_PriVxFinderTool
        from RecExConfig.RecFlags import rec

        self.doTruth=rec.doTruth()
        self.TrainingBeamspotX= 0.0
        self.TrainingBeamspotY= 0.0
        self.TrainingBeamspotZ = 0.0
        self.TrainingBeamspotTiltX= 0.0
        self.TrainingBeamspotTiltY= 0.0
        self. PixelBarrelPhiOffsets=[0.0,0.0,0.0,0.0]
        self. PixelBarrelEtaOffsets=[0.0,0.0,0.0,0.0]
        self. PixelEndCapPhiOffsets=[0.0,0.0,0.0]
        self. PixelEndCapEtaOffsets=[0.0,0.0,0.0]



