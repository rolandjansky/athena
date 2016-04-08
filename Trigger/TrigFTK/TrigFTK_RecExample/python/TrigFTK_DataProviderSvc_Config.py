# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

from FTK_DataProviderSvc.FTK_DataProviderSvcConf import FTK_DataProviderSvc, FTK_UncertaintyTool


class TrigFTK_DataProviderSvc(FTK_DataProviderSvc) :

    def __init__(self, name = "TrigFTK_DataProviderSvc"):
        print "In FTK_DataProviderSvc_Config.py"  
        FTK_DataProviderSvc.__init__(self, name) 
        # Track Fitter
        from TrkGlobalChi2Fitter.TrkGlobalChi2FitterConf import Trk__GlobalChi2Fitter
        from InDetTrigRecExample.InDetTrigConfigRecLoadTools import InDetTrigExtrapolator, InDetTrigNavigator, InDetTrigPropagator, InDetTrigRefitRotCreator,InDetTrigBroadInDetRotCreator, \
             InDetTrigUpdator, InDetTrigMaterialUpdator, InDetTrigUpdator, InDetTrigMaterialUpdator
        from TrkDetDescrSvc.AtlasTrackingGeometrySvc import AtlasTrackingGeometrySvc

        from TrkDistributedKalmanFilter.DistributedKalmanFilter_Config import ConfiguredDistributedKalmanFilter
        TrigFTK_TrackFitter = ConfiguredDistributedKalmanFilter("TrigFTK_TrackFitter",
            ExtrapTool=InDetTrigExtrapolator,
            ROTCreator=InDetTrigBroadInDetRotCreator,
            )

        #TrigFTK_TrackFitter = Trk__GlobalChi2Fitter(name = "TrigFTK_TrackFitter",
            #                                        ExtrapolationTool     = InDetTrigExtrapolator,
         #                                           NavigatorTool         = InDetTrigNavigator,
          #                                          PropagatorTool        = InDetTrigPropagator,     
           #                                                                         RotCreatorTool        = InDetTrigRefitRotCreator,
             #                                       BroadRotCreatorTool   = InDetTrigBroadInDetRotCreator,
              #                                      MeasurementUpdateTool = InDetTrigUpdator,
               #                                     TrackingGeometrySvc   = AtlasTrackingGeometrySvc,
                #                                    MaterialUpdateTool    = InDetTrigMaterialUpdator,
                 #                                   StraightLine          = False,
                  #                                  OutlierCut            = 4,
                   #                                 SignedDriftRadius     = True,
                    #                                RecalibrateSilicon    = True,
                     #                               RecalibrateTRT        = True,
                      #                              ReintegrateOutliers   = True,
                       #                             TrackChi2PerNDFCut    = 7,
                        #                            TRTExtensionCuts      = False, 
                         ##                           MaxIterations         = 40,
                           #                         Acceleration          = True,
                            #                                    Momentum=1000.,
                             #                       )
        from AthenaCommon.AppMgr import ToolSvc

        print "printing TrigFTK_TrackFitter configured with DKF"
        print TrigFTK_TrackFitter
        ToolSvc += TrigFTK_TrackFitter
        print "added TrigFTK_TrackFitter to ToolSvc"


        from InDetTrigRecExample.InDetTrigConfigRecLoadTools import InDetTrigExtrapolator,InDetTrigTrackSelectorTool,InDetTrigHoleSearchTool
        from InDetTrigRecExample.InDetTrigConfigRecLoadTools import InDetTrigTrackSummaryToolSharedHits, InDetTrigPrdAssociationTool 


        from InDetTrackSummaryHelperTool.InDetTrackSummaryHelperToolConf import InDet__InDetTrackSummaryHelperTool
        TrigFTK_TrackSummaryHelperTool = InDet__InDetTrackSummaryHelperTool(name = "TrigFTK_TrackSummaryHelperTool",
                                                                AssoTool     = InDetTrigPrdAssociationTool,
                                                                DoSharedHits = False,
                                                                HoleSearch   = InDetTrigHoleSearchTool,
                                                                TestBLayerTool = None,
                                                                PixelToTPIDTool = None,
                                                                usePixel=True,
                                                                useSCT=True,
                                                                useTRT=False
                                                                )

        ToolSvc += TrigFTK_TrackSummaryHelperTool
        print "added TrigFTK_TrackSummaryHelperTool to ToolSvc"
        

        
        from TrkTrackSummaryTool.TrkTrackSummaryToolConf import Trk__TrackSummaryTool
        TrigFTK_TrackSummaryTool = Trk__TrackSummaryTool(name = "TrigFTK_TrackSummaryTool",
                                             InDetSummaryHelperTool = TrigFTK_TrackSummaryHelperTool,
                                             InDetHoleSearchTool    = None,
                                             doSharedHits           = False,
                                             TRT_ElectronPidTool    = None
                                             )
        ToolSvc += TrigFTK_TrackSummaryTool
        print "added TrigFTK_TrackSummaryTool to ToolSvc"


        from TrkParticleCreator.TrkParticleCreatorConf import Trk__TrackParticleCreatorTool


        TrigFTK_TrackParticleCreatorTool = Trk__TrackParticleCreatorTool( name = "TrigFTK_ParticleCreatorTool",
                                                         Extrapolator = InDetTrigExtrapolator,
                                                         TrackSummaryTool = TrigFTK_TrackSummaryTool,
                                                         KeepParameters = True,
                                                         ForceTrackSummaryUpdate = False,  #summary update moved (in the slimmer now)
                                                         )
        
        ToolSvc += TrigFTK_TrackParticleCreatorTool
        print TrigFTK_TrackParticleCreatorTool
        print " added TrigFTK_TrackParticleCreatorTool to ToolSvc"

        
        from TrigInDetConf.TrigInDetRecVtxTools import InDetTrigLinFactory, InDetTrigVxEdmCnv
        from TrigInDetConf.TrigInDetRecVtxTools import InDetTrigLinFactory, InDetTrigVxEdmCnv

        from TrkVertexSeedFinderTools.TrkVertexSeedFinderToolsConf import Trk__ZScanSeedFinder
        TrigFTK_VtxSeedFinder = Trk__ZScanSeedFinder(name = "TrigFTK_ZScanSeedFinder"
                                                    #Mode1dFinder = # default, no setting needed
                                                    )
        ToolSvc += TrigFTK_VtxSeedFinder
 
        from TrkVertexFitterUtils.TrkVertexFitterUtilsConf import Trk__ImpactPoint3dEstimator, Trk__DetAnnealingMaker
    
        TrigFTK_ImpactPoint3dEstimator = Trk__ImpactPoint3dEstimator(name         = "TrigFTK_TrkImpactPoint3dEstimator",
                                                         Extrapolator = InDetTrigExtrapolator,
                                                         )
        
        ToolSvc += TrigFTK_ImpactPoint3dEstimator

 
        TrigFTK_AnnealingMaker = Trk__DetAnnealingMaker(name = "TrigFTK_TrkAnnealingMaker")
        TrigFTK_AnnealingMaker.SetOfTemperatures = [64.,16.,4.,2.,1.5,1.] # not default
        ToolSvc += TrigFTK_AnnealingMaker


        from TrkVertexFitters.TrkVertexFittersConf import Trk__SequentialVertexSmoother
        TrigFTK_VertexSmoother =  Trk__SequentialVertexSmoother(name="TrigFTK_SequentialVertexSmoother")
        ToolSvc += TrigFTK_VertexSmoother


        from TrkVertexFitters.TrkVertexFittersConf import Trk__AdaptiveVertexFitter
        TrigFTK_VxFitterTool = Trk__AdaptiveVertexFitter(name                         = "TrigFTK_AdaptiveVxFitterTool",
                                             SeedFinder                   = TrigFTK_VtxSeedFinder,
                                             LinearizedTrackFactory       = InDetTrigLinFactory,
                                             ImpactPoint3dEstimator       = TrigFTK_ImpactPoint3dEstimator,
                                             AnnealingMaker               = TrigFTK_AnnealingMaker,
                                             #TrackCompatibilityEstimator = 
                                             VertexSmoother = TrigFTK_VertexSmoother,
                                             #VertexUpdator = ,  #does not use any tools?
                                             #onlyzseed = ,
                                             )

        TrigFTK_VxFitterTool.XAODConverter = InDetTrigVxEdmCnv
        ToolSvc += TrigFTK_VxFitterTool
        
        from InDetPriVxFinderTool.InDetPriVxFinderToolConf import InDet__InDetIterativePriVxFinderTool
        TrigFTK_PriVxFinderTool = InDet__InDetIterativePriVxFinderTool(name             = "TrigFTK_PriVxFinderTool",
                                                                   VertexFitterTool = TrigFTK_VxFitterTool,
                                                                   TrackSelector    = InDetTrigTrackSelectorTool,
                                                                   SeedFinder = TrigFTK_VtxSeedFinder,
                                                                   ImpactPoint3dEstimator = TrigFTK_ImpactPoint3dEstimator,
                                                                   LinearizedTrackFactory = InDetTrigLinFactory,
                                                                   useBeamConstraint = True,
                                                                   significanceCutSeeding = 12,
                                                                   maximumChi2cutForSeeding = 49,
                                                                   maxVertices = 200,
                                                                   InternalEdmFactory =InDetTrigVxEdmCnv
                                                                   )
        ToolSvc += TrigFTK_PriVxFinderTool


        from FTK_DataProviderSvc.FTK_DataProviderSvcConf import FTK_UncertaintyTool
        theTrigFTK_UncertaintyTool= FTK_UncertaintyTool(name="TrigFTK_UncertaintyTool")
        ToolSvc+=theTrigFTK_UncertaintyTool

        from FTK_RecTools.FTK_RecToolsConf import FTK_VertexFinderTool
        TrigFTK_RawVertexFinderTool=  FTK_VertexFinderTool(name="TrigFTK_RawVertexFinderTool")
        ToolSvc+=TrigFTK_RawVertexFinderTool


        self.TrackCollectionName= "TrigFTK_TrackCollection"
        self.TrackParticleContainerName= "TrigFTK_TrackParticleCollection"
        self.VxContainerName="TrigFTK_VxContainer"
        self.VertexContainerName="TrigFTK_VertexContainer"	  
        self.PixelTruthName="TrigFTK_PRD_MultiTruthPixel"
        self.SctTruthName="TrigFTK_PRD_MultiTruthSCT"
        self.PixelClusterContainerName= "TrigFTK_PixelClusterContainer"
        self.SCT_ClusterContainerName= "TrigFTK_SCT_ClusterContainer"


        self.TrackFitter = TrigFTK_TrackFitter
        self.UncertaintyTool=theTrigFTK_UncertaintyTool
        self.TrackSummaryTool=TrigFTK_TrackSummaryTool
        self.TrackParticleCreatorTool=TrigFTK_TrackParticleCreatorTool
        self.RawVertexFinderTool=TrigFTK_RawVertexFinderTool
        self.VertexFinderTool=TrigFTK_PriVxFinderTool
        from RecExConfig.RecFlags import rec
        self.doTruth= rec.doTruth()
        self.TrainingBeamspotX= -0.0497705
        self.TrainingBeamspotY=1.06299
        self.TrainingBeamspotZ = 0.0
        self.TrainingBeamspotTiltX= -1.51489e-05 
        self.TrainingBeamspotTiltY= -4.83891e-05
        self. PixelBarrelPhiOffsets=[0.,0.,0.,0.]
        self. PixelBarrelEtaOffsets=[0.,0.,0.,0.]
        self. PixelEndCapPhiOffsets=[-0.005,0.,0.]
        self. PixelEndCapEtaOffsets=[0.,0.,0.]

        


