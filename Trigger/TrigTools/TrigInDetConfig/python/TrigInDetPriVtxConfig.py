#  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
#
#           Vertexing 


from AthenaCommon.Logging import logging
log = logging.getLogger("InDetVtx")


def makeVertices( whichSignature, inputTrackCollection, outputVtxCollection ):
   from AthenaCommon.AppMgr import ToolSvc
   from TrigEDMConfig.TriggerEDMRun3 import recordable
   #vtxCollectionName = "HLT_xPrimVx"

   #If signature specified add suffix to the algorithms
   signature =  "_" + whichSignature if whichSignature else ''
   
   
   from TrkExTools.TrkExToolsConf import Trk__Extrapolator
   InDetTrigExtrapolator = Trk__Extrapolator(name                    = 'InDetTrigExtrapolator' + signature,
   #                                        Propagators             = [ InDetPropagator ], # [ InDetPropagator, InDetStepPropagator ],
   #                                        MaterialEffectsUpdators = [ InDetMaterialUpdator ],
   #                                        Navigator               = InDetNavigator,
   #                                        SubPropagators          = InDetSubPropagators,
   #                                        SubMEUpdators           = InDetSubUpdators)
                                           )
   ToolSvc += InDetTrigExtrapolator
   
   from TrkVertexFitterUtils.TrkVertexFitterUtilsConf import Trk__FullLinearizedTrackFactory
   InDetTrigLinFactory = Trk__FullLinearizedTrackFactory(name              = "InDetTrigFullLinearizedTrackFactory" + signature,
                                                     Extrapolator      = InDetTrigExtrapolator )
   ToolSvc += InDetTrigLinFactory
   
   from TrkVertexBilloirTools.TrkVertexBilloirToolsConf import Trk__FastVertexFitter
   InDetTrigVxFitterTool = Trk__FastVertexFitter(name                   = "InDetTrigFastVertexFitterTool" + signature,
                                             LinearizedTrackFactory = InDetTrigLinFactory,
                                             Extrapolator           = InDetTrigExtrapolator) # commented out due to the InDetPriVxFinderToolNoBeamConstraint testing 
   ToolSvc += InDetTrigVxFitterTool
   
   
   from InDetTrackSelectionTool.InDetTrackSelectionToolConf import InDet__InDetTrackSelectionTool
   InDetTrigTrackSelectorTool = InDet__InDetTrackSelectionTool(name = "InDetTrigDetailedTrackSelectionTool" + signature,
   #                                                          CutLevel = InDetPrimaryVertexingCuts.TrackCutLevel(),
   #                                                          minPt = InDetPrimaryVertexingCuts.minPT(),
   #                                                          maxD0 = InDetPrimaryVertexingCuts.IPd0Max(),
   #                                                          maxZ0 = InDetPrimaryVertexingCuts.z0Max(),
   #                                                          maxZ0SinTheta = InDetPrimaryVertexingCuts.IPz0Max(),
   #                                                          maxSigmaD0 = InDetPrimaryVertexingCuts.sigIPd0Max(),
   #                                                          maxSigmaZ0SinTheta = InDetPrimaryVertexingCuts.sigIPz0Max(),
                                                             # maxChiSqperNdf = InDetPrimaryVertexingCuts.fitChi2OnNdfMax(), # Seems not to be implemented?
   #                                                          maxAbsEta = InDetPrimaryVertexingCuts.etaMax(),
   #                                                          minNInnermostLayerHits = InDetPrimaryVertexingCuts.nHitInnermostLayer(),
   #                                                          minNPixelHits = InDetPrimaryVertexingCuts.nHitPix(),
   #                                                          maxNPixelHoles = InDetPrimaryVertexingCuts.nHolesPix(),
   #                                                          minNSctHits = InDetPrimaryVertexingCuts.nHitSct(),
   #                                                          minNTrtHits = InDetPrimaryVertexingCuts.nHitTrt(),
   #                                                          minNSiHits = InDetPrimaryVertexingCuts.nHitSi(),
   #                                                          TrackSummaryTool = InDetTrackSummaryTool,
                                                             Extrapolator = InDetTrigExtrapolator)
   ToolSvc += InDetTrigTrackSelectorTool
   
   from TrkVertexSeedFinderUtils.TrkVertexSeedFinderUtilsConf import Trk__GaussianTrackDensity
   TrigGaussDensityEstimator = Trk__GaussianTrackDensity( name = "TrigGaussianDensity" + signature )
   
   ToolSvc += TrigGaussDensityEstimator
   
   from TrkVertexSeedFinderTools.TrkVertexSeedFinderToolsConf import Trk__TrackDensitySeedFinder
   InDetTrigVtxSeedFinder = Trk__TrackDensitySeedFinder( name = "TrigGaussianDensitySeedFinder" + signature,
                                                     DensityEstimator = TrigGaussDensityEstimator )
   
   ToolSvc += InDetTrigVtxSeedFinder
   
   from TrkVertexFitterUtils.TrkVertexFitterUtilsConf import Trk__ImpactPoint3dEstimator
   InDetTrigImpactPoint3dEstimator = Trk__ImpactPoint3dEstimator(name              = "InDetTrigImpactPoint3dEstimator" + signature,
                                                             Extrapolator      = InDetTrigExtrapolator)
   
   ToolSvc += InDetTrigImpactPoint3dEstimator
   
   from InDetPriVxFinderTool.InDetPriVxFinderToolConf import InDet__InDetIterativePriVxFinderTool
   InDetTrigPriVxFinderToolNoBeamConstraint = InDet__InDetIterativePriVxFinderTool(name                 = "InDetTrigPriVxFinderToolNoBeamConstraint" + signature,
                                                                               VertexFitterTool         = InDetTrigVxFitterTool,
                                                                               TrackSelector            = InDetTrigTrackSelectorTool,
                                                                               SeedFinder               = InDetTrigVtxSeedFinder,
                                                                               ImpactPoint3dEstimator   = InDetTrigImpactPoint3dEstimator,
                                                                               LinearizedTrackFactory   = InDetTrigLinFactory,
                                                                               useBeamConstraint        = False,
                                                                               significanceCutSeeding   = 12,
                                                                               maximumChi2cutForSeeding = 49,
                                                                               maxVertices              = 200,
                                                                               createSplitVertices      = False,
                                                                               #doMaxTracksCut           = InDetPrimaryVertexingCuts.doMaxTracksCut(),
                                                                               #MaxTracks                = InDetPrimaryVertexingCuts.MaxTracks()  )
                                                                              ) 
   ToolSvc += InDetTrigPriVxFinderToolNoBeamConstraint
   
   #  if InDetFlags.primaryVertexSortingSetup() == 'SumPt2Sorting':
   
   from TrkVertexWeightCalculators.TrkVertexWeightCalculatorsConf import Trk__SumPtVertexWeightCalculator
   TrigVertexWeightCalculator = Trk__SumPtVertexWeightCalculator(name              = "InDetSumPtVertexWeightCalculator" + signature,
                                                             DoSumPt2Selection = True)
   
   #    elif InDetFlags.primaryVertexSortingSetup() == 'VxProbSorting':
   
   #  from TrkVertexWeightCalculators.TrkVertexWeightCalculatorsConf import Trk__VxProbVertexWeightCalculator
   #  VertexWeightCalculator = Trk__VxProbVertexWeightCalculator(name          = "InDetVxProbVertexWeightCalculator",
   #                                                             HistogramPath = "/VxProbHisto/h_sTrkPdfminBias")
   
   #    elif InDetFlags.primaryVertexSortingSetup() == 'NNSorting':
         
   #  from TrkVertexWeightCalculators.TrkVertexWeightCalculatorsConf import Trk__NNVertexWeightCalculator
   #  VertexWeightCalculator = Trk__NNVertexWeightCalculator(name = "InDetNNVertexWeightCalculator",
   #                                                             HistoFilePath ="/NNHisto/")
       
   ToolSvc += TrigVertexWeightCalculator
   
   
   from TrkVertexTools.TrkVertexToolsConf import Trk__VertexCollectionSortingTool
   TrigVertexCollectionSortingTool = Trk__VertexCollectionSortingTool(name                   = "InDetVertexCollectionSortingTool" + signature,
                                                                  VertexWeightCalculator = TrigVertexWeightCalculator)
   ToolSvc += TrigVertexCollectionSortingTool
   
   from InDetPriVxFinder.InDetPriVxFinderMonitoring import InDetPriVxFinderMonitoringTool
   
   
   from InDetPriVxFinder.InDetPriVxFinderConf import InDet__InDetPriVxFinder
   InDetTrigPriVxFinder = InDet__InDetPriVxFinder(name                        = "InDetTrigPriVxFinder" + signature,
                          VertexFinderTool            = InDetTrigPriVxFinderToolNoBeamConstraint,
                          TracksName                  = recordable(inputTrackCollection), #recordable("HLT_xAODTracks"),          #InDetKeys.xAODTrackParticleContainer(),
                          VxCandidatesOutputName      = outputVtxCollection, #"HLT_xPrimVx",  #InDetKeys.PrimaryVerticesWithoutBeamConstraint(),
                          VertexCollectionSortingTool = TrigVertexCollectionSortingTool,
                          doVertexSorting             = True,
                          #OutputLevel = 2,
                          PriVxMonTool = InDetPriVxFinderMonitoringTool(),
                          )
   
   #Its not a tool but algorithm!
   #ToolSvc += InDetTrigPriVxFinder
   log.debug(InDetTrigPriVxFinder)

   
   
   return  [ InDetTrigPriVxFinder  ]
   
