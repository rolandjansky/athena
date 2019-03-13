#  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
#
#           Setup of precision tracking

def makeInDetPrecisionTracking( whichSignature, verifier = False, inputFTFtracks='TrigFastTrackFinder_Tracks', outputTrackPrefixName = "InDetTrigPT" ):
#def makeInDetPrecisionTracking( whichSignature, inputFTFtracks='TrigFastTrackFinder_Tracks', outputTrackPrefixName = "InDetTrigPT" ):
  from AthenaCommon.AppMgr import ToolSvc

#def makeInDetPrecisionTracking( whichSignature, inputFTFtracks, outputTrackPrefixName ):
  #If signature specified add suffix to the algorithms
  signature =  "_" + whichSignature if whichSignature else ''
  
  #Name settings
  
  #This first part is for ambiguity solver tracks
  nameAmbiTrackCollection = outputTrackPrefixName + "Tracks" + signature
  nameAmbiTrackParticles  = outputTrackPrefixName + "TrackParticles" + signature 
  
  
  
  #Potentialy other algs with more collections? Make a list just in case
  nameTrackCollections = [ nameAmbiTrackCollection ]
  nameTrackParticles = [ nameAmbiTrackParticles ]
  
  

  #If run in views need to check data dependancies!
  #if runInViews: 
  #FIXME: check whether the passed argument is a verifier?
  #NOTE: this seems necessary only when PT is called from a different view than FTF otherwise causes stalls
  if verifier:
         verifier.DataObjects += [  ( 'InDet::PixelGangedClusterAmbiguities' , 'StoreGateSvc+TrigPixelClusterAmbiguitiesMap' ),
                                  ( 'TrackCollection' , 'StoreGateSvc+' + inputFTFtracks ) ] 
      
   

  
  #
  # --- Ambiguity solver algorithm
  #
  #TODO:Check whether InDet alg have to be replaced by new ones
  from InDetTrigRecExample.InDetTrigConfigRecLoadTools import InDetTrigTrackSummaryTool
  from InDetTrigRecExample.InDetTrigConfigRecLoadTools import InDetTrigExtrapolator
  from InDetTrackScoringTools.InDetTrackScoringToolsConf import InDet__InDetAmbiScoringTool
  InDetTrigMTAmbiScoringTool =  InDet__InDetAmbiScoringTool( name                        = 'InDetTrigMTScoringTool' + signature ,
                                                             Extrapolator                = InDetTrigExtrapolator,
                                                             InputEmClusterContainerName = '', #need to be reset to empty string
                                                             doEmCaloSeed                = False,
                                                             SummaryTool                 = InDetTrigTrackSummaryTool)
  
  
  
  ToolSvc += InDetTrigMTAmbiScoringTool
  
  from InDetTrigRecExample.InDetTrigConfigRecLoadTools import InDetTrigAmbiTrackSelectionTool
  from InDetTrigRecExample.InDetTrigConfigRecLoadTools import InDetTrigTrackFitter
  from TrkAmbiguityProcessor.TrkAmbiguityProcessorConf import Trk__SimpleAmbiguityProcessorTool as ProcessorTool
  InDetTrigMTAmbiguityProcessor = ProcessorTool(name          = 'InDetTrigMTAmbiguityProcessor' + signature,
                                                Fitter        = InDetTrigTrackFitter,
                                                ScoringTool   = InDetTrigMTAmbiScoringTool,
                                                SelectionTool = InDetTrigAmbiTrackSelectionTool)
  
  
  ToolSvc += InDetTrigMTAmbiguityProcessor
  
  
  from TrkAmbiguitySolver.TrkAmbiguitySolverConf import Trk__TrkAmbiguitySolver
  InDetTrigMTAmbiguitySolver = Trk__TrkAmbiguitySolver(name         = 'InDetTrigMTAmbiguitySolver' + signature,
                                                       TrackInput         =[ inputFTFtracks ],
                                                       TrackOutput        = nameAmbiTrackCollection, 
                                                       AmbiguityProcessor = InDetTrigMTAmbiguityProcessor)
  
  #allViewAlgorithms += InDetTrigMTAmbiguitySolver
  
  
  #
  # --- Track particle conversion algorithm
  #
  
  
  from TrkParticleCreator.TrkParticleCreatorConf import Trk__TrackParticleCreatorTool
  InDetTrigMTxAODParticleCreatorTool = Trk__TrackParticleCreatorTool(name =  "InDetTrigMTxAODParticleCreatorTool" + signature,
                                                                     Extrapolator = InDetTrigExtrapolator,
                                                                     #ForceTrackSummaryUpdate = False,
                                                                     #TrackSummaryTool = InDetTrigTrackSummaryToolSharedHits) 
                                                                     TrackSummaryTool = InDetTrigTrackSummaryTool)
  
  ToolSvc += InDetTrigMTxAODParticleCreatorTool
  print InDetTrigMTxAODParticleCreatorTool
  
  
  from xAODTrackingCnv.xAODTrackingCnvConf import xAODMaker__TrackCollectionCnvTool
  InDetTrigMTxAODTrackCollectionCnvTool= xAODMaker__TrackCollectionCnvTool(name = "InDetTrigMTxAODTrackCollectionCnvTool" + signature,
                                                                           TrackParticleCreator = InDetTrigMTxAODParticleCreatorTool)
  
  ToolSvc += InDetTrigMTxAODTrackCollectionCnvTool
  print InDetTrigMTxAODTrackCollectionCnvTool
  
  #This one shouldn't be necessary
  #TODO: obsolete turn off
  from xAODTrackingCnv.xAODTrackingCnvConf import  xAODMaker__RecTrackParticleContainerCnvTool
  InDetTrigMTRecTrackParticleContainerCnvTool=  xAODMaker__RecTrackParticleContainerCnvTool(name = "InDetTrigMTRecTrackContainerCnvTool" + signature,
                                                                                            TrackParticleCreator = InDetTrigMTxAODParticleCreatorTool)
  
  ToolSvc += InDetTrigMTRecTrackParticleContainerCnvTool
  print InDetTrigMTRecTrackParticleContainerCnvTool
  
  from xAODTrackingCnv.xAODTrackingCnvConf import xAODMaker__TrackParticleCnvAlg
  InDetTrigMTxAODTrackParticleCnvAlg = xAODMaker__TrackParticleCnvAlg(name = "InDetTrigMTxAODParticleCreatorAlg" + signature,
                                                                      # Properties below are used for:  TrackCollection -> xAOD::TrackParticle
                                                                        ConvertTracks = True,  #Turn on  retrieve of TrackCollection, false by default
                                                                        TrackContainerName = nameAmbiTrackCollection,
                                                                        xAODTrackParticlesFromTracksContainerName = nameAmbiTrackParticles, 
                                                                        TrackCollectionCnvTool = InDetTrigMTxAODTrackCollectionCnvTool,
                                                                       ## Properties below are used for: Rec:TrackParticle, aod -> xAOD::TrackParticle (Turn off)
                                                                        ConvertTrackParticles = False,  # Retrieve of Rec:TrackParticle, don't need this atm
                                                                        xAODContainerName = '',  
                                                                        RecTrackParticleContainerCnvTool = InDetTrigMTRecTrackParticleContainerCnvTool,
                                                                        #PrintIDSummaryInfo = True, #Just to test and have some output
                                                                        TrackParticleCreator = InDetTrigMTxAODParticleCreatorTool
                                                                        )
  
  
  #allViewAlgorithms += InDetTrigMTxAODTrackParticleCnvAlg
  print  InDetTrigMTxAODTrackParticleCnvAlg
  
  #ToolSvc.InDetTrigHoleSearchTool.SctSummaryTool.InDetTrigInDetSCT_FlaggedConditionTool.SCT_FlaggedCondData = "SCT_FlaggedCondData_TRIG"

  
  #Return list of Track keys, TrackParticle keys, and PT algs
  return  nameTrackCollections, nameTrackParticles, [InDetTrigMTAmbiguitySolver, InDetTrigMTxAODTrackParticleCnvAlg]
  
