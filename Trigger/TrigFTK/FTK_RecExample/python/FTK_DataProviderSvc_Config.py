# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

from FTK_DataProviderSvc.FTK_DataProviderSvcConf import FTK_DataProviderSvc, FTK_UncertaintyTool

from AthenaCommon.Constants import *   # for "INFO"


class ConfiguredFTK_DataProviderSvc(FTK_DataProviderSvc) :
    def __init__(self, name = "ConfiguredFTK_DataProviderSvc", OutputLevel=INFO):
        print "In FTK_DataProviderSvc_Config.py"  
        FTK_DataProviderSvc.__init__(self, name) 
        # Track Fitter

        from TrkGlobalChi2Fitter.TrkGlobalChi2FitterConf import Trk__GlobalChi2Fitter


        # set up geometry
        from AthenaCommon.Include import include
        from TrkDetDescrSvc.AtlasTrackingGeometrySvc import AtlasTrackingGeometrySvc
        
        #
        # get propagator
        #
        from TrkExSTEP_Propagator.TrkExSTEP_PropagatorConf import Trk__STEP_Propagator
        FTK_StepPropagator = Trk__STEP_Propagator(name = 'FTK_StepPropagator')

        from AthenaCommon.AppMgr import ToolSvc

        ToolSvc += FTK_StepPropagator
  


        from TrkExRungeKuttaPropagator.TrkExRungeKuttaPropagatorConf import Trk__RungeKuttaPropagator
        FTK_RKPropagator = Trk__RungeKuttaPropagator(name = 'FTK_RKPropagator')
        ToolSvc += FTK_RKPropagator

        FTK_Propagator = FTK_RKPropagator

        #
        # Setup the Navigator (default, could be removed)
        #
        from AthenaCommon.AppMgr import ServiceMgr as svcMgr
        AtlasTrackingGeometrySvc  = svcMgr.AtlasTrackingGeometrySvc 

        from TrkExTools.TrkExToolsConf import Trk__Navigator
        FTK_Navigator = Trk__Navigator(name = 'FTK_Navigator',
                                            TrackingGeometrySvc = AtlasTrackingGeometrySvc)
        
        ToolSvc += FTK_Navigator
  
        #
        # Setup the MaterialEffectsUpdator
        #
        from TrkExTools.TrkExToolsConf import Trk__MaterialEffectsUpdator
        FTK_MaterialUpdator = Trk__MaterialEffectsUpdator(name = "FTK_MaterialEffectsUpdator")

        ToolSvc += FTK_MaterialUpdator


        #
        # Set up extrapolator
        #
        
        FTK_SubPropagators = []
        FTK_SubUpdators = []
  
        # -------------------- set it depending on the geometry --------------
        # default for ID is (Rk,Mat)
        FTK_SubPropagators += [ FTK_Propagator.name() ]
        FTK_SubUpdators    += [ FTK_MaterialUpdator.name() ]
        
        # default for Calo is (Rk,MatLandau)
        FTK_SubPropagators += [  FTK_Propagator.name() ]
        FTK_SubUpdators    += [  FTK_MaterialUpdator.name() ]
        
        # default for MS is (STEP,Mat)
        FTK_SubPropagators += [  FTK_StepPropagator.name() ]
        FTK_SubUpdators    += [  FTK_MaterialUpdator.name() ]
        
        from TrkExTools.TrkExToolsConf import Trk__Extrapolator
        FTK_Extrapolator = Trk__Extrapolator(name        = 'FTK_Extrapolator',
                                            Propagators             = [ FTK_RKPropagator, FTK_StepPropagator],
                                            MaterialEffectsUpdators = [ FTK_MaterialUpdator ],
                                            Navigator               = FTK_Navigator,
                                            SubPropagators          = FTK_SubPropagators,
                                            SubMEUpdators           = FTK_SubUpdators,
                                            #DoCaloDynamic          = False
                                            )

        ToolSvc += FTK_Extrapolator

        


        # clusterOnTrack Tools
        #
        from SiClusterOnTrackTool.SiClusterOnTrackToolConf import InDet__SCT_ClusterOnTrackTool
        SCT_ClusterOnTrackTool = InDet__SCT_ClusterOnTrackTool ("SCT_ClusterOnTrackTool",
                                                                CorrectionStrategy = 0,  # do correct position bias
                                                                ErrorStrategy      = 2)  # do use phi dependent errors
        ToolSvc += SCT_ClusterOnTrackTool

        # tool to always make conservative pixel cluster errors
        from SiClusterOnTrackTool.SiClusterOnTrackToolConf import InDet__PixelClusterOnTrackTool

        FTK_PixelClusterOnTrackTool = \
                               InDet__PixelClusterOnTrackTool("FTK_PixelClusterOnTrackTool",
                                                              ErrorStrategy = 2)
        ToolSvc += FTK_PixelClusterOnTrackTool


        # tool to always make conservative sct cluster errors
        from SiClusterOnTrackTool.SiClusterOnTrackToolConf import InDet__SCT_ClusterOnTrackTool
        FTK_BroadSCT_ClusterOnTrackTool = InDet__SCT_ClusterOnTrackTool ("FTK_BroadSCT_ClusterOnTrackTool",
                                                                         CorrectionStrategy = 0,  # do correct position bias
                                                                         ErrorStrategy      = 0)  # do use broad errors
        ToolSvc += FTK_BroadSCT_ClusterOnTrackTool

        #--

        FTK_BroadPixelClusterOnTrackTool = InDet__PixelClusterOnTrackTool("FTK_BroadPixelClusterOnTrackTool",
                                                                          ErrorStrategy = 0)
        ToolSvc += FTK_BroadPixelClusterOnTrackTool


        # load RIO_OnTrackCreator for Inner Detector
        #

        from TrkRIO_OnTrackCreator.TrkRIO_OnTrackCreatorConf import Trk__RIO_OnTrackCreator
        FTK_RotCreator = Trk__RIO_OnTrackCreator(name = 'FTK_RotCreator',
                                                ToolPixelCluster= FTK_PixelClusterOnTrackTool,
                                                ToolSCT_Cluster = SCT_ClusterOnTrackTool,
                                                Mode = 'indet')
        ToolSvc += FTK_RotCreator

        FTK_RotCreator.ToolPixelCluster = FTK_BroadPixelClusterOnTrackTool
        FTK_RotCreator.ToolSCT_Cluster  = FTK_BroadSCT_ClusterOnTrackTool


        #--
        from TrkRIO_OnTrackCreator.TrkRIO_OnTrackCreatorConf import Trk__RIO_OnTrackCreator
        FTK_BroadInDetRotCreator = Trk__RIO_OnTrackCreator(name            = 'FTK_BroadInDetRotCreator',
                                                           ToolPixelCluster= FTK_BroadPixelClusterOnTrackTool,
                                                           ToolSCT_Cluster = FTK_BroadSCT_ClusterOnTrackTool,
                                                           Mode            = 'indet')
        ToolSvc += FTK_BroadInDetRotCreator


        from TrkDistributedKalmanFilter.DistributedKalmanFilter_Config import ConfiguredDistributedKalmanFilter
        FTK_TrackFitter = ConfiguredDistributedKalmanFilter("FTK_TrackFitter",
            ExtrapTool=FTK_Extrapolator,
            ROTCreator=FTK_BroadInDetRotCreator,
            )

                
        from AthenaCommon.AppMgr import ToolSvc
    
        ToolSvc += FTK_TrackFitter
        print "added FTK_TrackFitter to ToolSvc"

            

        from InDetAssociationTools.InDetAssociationToolsConf import InDet__InDetPRD_AssociationToolGangedPixels
        FTK_PrdAssociationTool = InDet__InDetPRD_AssociationToolGangedPixels(name = "FTK_PrdAssociationTool",
                                                                             PixelClusterAmbiguitiesMapName = "FTK_ClusterAmbiguitiesMap")
        
        ToolSvc += FTK_PrdAssociationTool


        # Loading Configurable HoleSearchTool
        #
        from InDetTrackHoleSearch.InDetTrackHoleSearchConf import InDet__InDetTrackHoleSearchTool
        FTK_HoleSearchTool = InDet__InDetTrackHoleSearchTool(name = "FTK_HoleSearchTool",
                                                              Extrapolator = FTK_Extrapolator,
                                                              usePixel      = True,
                                                              useSCT        = True,
                                                              CountDeadModulesAfterLastHit = True)

#        from AthenaCommon.AppMgr import ServiceMgr as svcMgr
#        FTK_HoleSearchTool.SctSummarySvc = svcMgr.InDetSCT_ConditionsSummarySvc 
        ToolSvc +=  FTK_HoleSearchTool 
       
        

        from InDetTrackSummaryHelperTool.InDetTrackSummaryHelperToolConf import InDet__InDetTrackSummaryHelperTool
        FTK_TrackSummaryHelperTool = InDet__InDetTrackSummaryHelperTool(name = "FTK_TrackSummaryHelperTool",
                                                                AssoTool     = FTK_PrdAssociationTool,
                                                                DoSharedHits = False,
                                                                HoleSearch   = FTK_HoleSearchTool,
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
                                                         Extrapolator = FTK_Extrapolator,
                                                         TrackSummaryTool = FTK_TrackSummaryTool,
                                                         KeepParameters = True,
                                                         ForceTrackSummaryUpdate = False,  #summary update moved (in the slimmer now)
                                                         )
        
        ToolSvc += FTK_TrackParticleCreatorTool
        print FTK_TrackParticleCreatorTool
        print " added FTK_TrackParticleCreatorTool to ToolSvc"

        

        from TrkVertexSeedFinderTools.TrkVertexSeedFinderToolsConf import Trk__ZScanSeedFinder
        FTK_VtxSeedFinder = Trk__ZScanSeedFinder(name = "FTK_ZScanSeedFinder"
                                                    #Mode1dFinder = # default, no setting needed
                                                    )


        ToolSvc += FTK_VtxSeedFinder
 
        from TrkVertexFitterUtils.TrkVertexFitterUtilsConf import Trk__ImpactPoint3dEstimator, Trk__DetAnnealingMaker
    
        FTK_ImpactPoint3dEstimator = Trk__ImpactPoint3dEstimator(name         = "FTK_TrkImpactPoint3dEstimator",
                                                         Extrapolator = FTK_Extrapolator,
                                                         )
        
        ToolSvc += FTK_ImpactPoint3dEstimator

 
        FTK_AnnealingMaker = Trk__DetAnnealingMaker(name = "FTK_TrkAnnealingMaker")
        FTK_AnnealingMaker.SetOfTemperatures = [64.,16.,4.,2.,1.5,1.] # not default
        ToolSvc += FTK_AnnealingMaker


        from TrkVertexFitters.TrkVertexFittersConf import Trk__SequentialVertexSmoother
        FTK_VertexSmoother =  Trk__SequentialVertexSmoother(name="FTK_SequentialVertexSmoother")
        ToolSvc += FTK_VertexSmoother


        from TrkVertexFitterUtils.TrkVertexFitterUtilsConf import Trk__FullLinearizedTrackFactory

        FTK_LinFactory = Trk__FullLinearizedTrackFactory(name              = "FTK_FullLinearizedTrackFactory",
                                                         Extrapolator      = FTK_Extrapolator,
                                                         )
        ToolSvc += FTK_LinFactory

        from TrkVxEdmCnv.TrkVxEdmCnvConf import Trk__VxCandidateXAODVertex
        FTK_VxEdmCnv = Trk__VxCandidateXAODVertex(name="FTK_VertexInternalEdmFactory")
        FTK_VxEdmCnv.LinearizedTrackFactory=FTK_LinFactory
        ToolSvc += FTK_VxEdmCnv

        from TrkVertexFitters.TrkVertexFittersConf import Trk__AdaptiveVertexFitter
        FTK_VxFitterTool = Trk__AdaptiveVertexFitter(name                         = "FTK_AdaptiveVxFitterTool",
                                             SeedFinder                   = FTK_VtxSeedFinder,
                                             LinearizedTrackFactory       = FTK_LinFactory,
                                             ImpactPoint3dEstimator       = FTK_ImpactPoint3dEstimator,
                                             AnnealingMaker               = FTK_AnnealingMaker,
                                             #TrackCompatibilityEstimator = 
                                             VertexSmoother = FTK_VertexSmoother,
                                             #VertexUpdator = ,  #does not use any tools?
                                             #onlyzseed = ,
                                             )

        FTK_VxFitterTool.XAODConverter = FTK_VxEdmCnv
        ToolSvc += FTK_VxFitterTool
        


        from InDetRecExample.ConfiguredVertexingCuts import ConfiguredVertexingCuts

        FTK_VertexingCuts = ConfiguredVertexingCuts("PileUp")

        from InDetTrackSelectionTool.InDetTrackSelectionToolConf import InDet__InDetTrackSelectionTool
        FTK_TrackSelectorTool = InDet__InDetTrackSelectionTool(name = "FTK_DetailedTrackSelectorTool",
                                                                    CutLevel               = FTK_VertexingCuts.TrackCutLevel(),
                                                                    minPt                  = FTK_VertexingCuts.minPT(),
                                                                    maxD0                  = FTK_VertexingCuts.IPd0Max(),
                                                                    maxZ0                  = FTK_VertexingCuts.z0Max(),
                                                                    maxZ0SinTheta          = FTK_VertexingCuts.IPz0Max(),
                                                                    maxSigmaD0             = FTK_VertexingCuts.sigIPd0Max(),
                                                                    maxSigmaZ0SinTheta     = FTK_VertexingCuts.sigIPz0Max(),
                                                                    # maxChiSqperNdf       = FTK_VertexingCuts.fitChi2OnNdfMax(), # Seems not to be implemented?
                                                                    maxAbsEta              = FTK_VertexingCuts.etaMax(),
                                                                    minNInnermostLayerHits = FTK_VertexingCuts.nHitInnermostLayer(),
                                                                    minNPixelHits          = FTK_VertexingCuts.nHitPix(),
                                                                    maxNPixelHoles         = FTK_VertexingCuts.nHolesPix(),
                                                                    minNSctHits            = FTK_VertexingCuts.nHitSct(),
                                                                    minNTrtHits            = FTK_VertexingCuts.nHitTrt(),
                                                                    minNSiHits             = FTK_VertexingCuts.nHitSi(),
                                                                    TrackSummaryTool       = FTK_TrackSummaryTool,
                                                                    Extrapolator           = FTK_Extrapolator,
                                                                  )
  
  
  
        ToolSvc += FTK_TrackSelectorTool


        from InDetPriVxFinderTool.InDetPriVxFinderToolConf import InDet__InDetIterativePriVxFinderTool
        FTK_PriVxFinderTool = InDet__InDetIterativePriVxFinderTool(name             = "FTK_PriVxFinderTool",
                                                                   VertexFitterTool = FTK_VxFitterTool,
                                                                   TrackSelector    = FTK_TrackSelectorTool,
                                                                   SeedFinder = FTK_VtxSeedFinder,
                                                                   ImpactPoint3dEstimator = FTK_ImpactPoint3dEstimator,
                                                                   LinearizedTrackFactory = FTK_LinFactory,
                                                                   useBeamConstraint = True,
                                                                   significanceCutSeeding = 12,
                                                                   maximumChi2cutForSeeding = 49,
                                                                   maxVertices = 200,
                                                                   InternalEdmFactory = FTK_VxEdmCnv
                                                                   )
        ToolSvc += FTK_PriVxFinderTool


        from FTK_DataProviderSvc.FTK_DataProviderSvcConf import FTK_UncertaintyTool
        theFTK_UncertaintyTool= FTK_UncertaintyTool(name="FTK_UncertaintyTool")
        ToolSvc+=theFTK_UncertaintyTool

        from FTK_RecTools.FTK_RecToolsConf import FTK_VertexFinderTool
        FTK_RawVertexFinderTool=  FTK_VertexFinderTool(name="FTK_RawVertexFinderTool")
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
        self.TrainingBeamspotX= -0.0497705
        self.TrainingBeamspotY=1.06299
        self.TrainingBeamspotZ = 0.0
        self.TrainingBeamspotTiltX= -1.51489e-05 
        self.TrainingBeamspotTiltY= -4.83891e-05
        self. PixelBarrelPhiOffsets=[0.,0.,0.,0.]
        self. PixelBarrelEtaOffsets=[0.,0.,0.,0.]
        self. PixelEndCapPhiOffsets=[-0.005,0.,0.]
        self. PixelEndCapEtaOffsets=[0.,0.,0.]



