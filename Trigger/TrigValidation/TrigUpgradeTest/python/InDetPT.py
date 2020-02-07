#  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
#
#           Setup of precision tracking

from __future__ import print_function

from AthenaCommon.Include import include
include.block("InDetTrigRecExample/EFInDetConfig.py")
include("InDetTrigRecExample/InDetTrigRec_jobOptions.py") # this is needed to get InDetTrigFlags
from InDetTrigRecExample.InDetTrigFlags import InDetTrigFlags

from AthenaCommon.Logging import logging 
log = logging.getLogger("InDetPT")



#Start using already decided naming conventions
#TODO: remap might not be needed in the end once this is consistent with FTF configuration
def remap_signature( signature ):
   if signature == 'electron':
       return 'Electron'
   else:
        return signature

def makeInDetPrecisionTracking( whichSignature, verifier = False, inputFTFtracks='TrigFastTrackFinder_Tracks', outputTrackPrefixName = "HLT_ID", rois = 'EMViewRoIs' ):
  dbgLevel = 0
  doTRTextension = False 
  ptAlgs = [] #List containing all the precision tracking algorithms hence every new added alg has to be appended to the list


  #-----------------------------------------------------------------------------
  #                        Naming conventions

  algNamePrefix = "InDetTrigMT" 
  #If signature specified add suffix to the algorithms
  signature =  "_" + whichSignature if whichSignature else ''
  
  #Name settings for output Tracks/TrackParticles
  #This first part is for ambiguity solver tracks
  nameAmbiTrackCollection = "%s%sTrkTrack%s"         %(outputTrackPrefixName, 'AmbSol', signature)
  nameAmbiTrackParticles  = "%s%sTrack%s"            %(outputTrackPrefixName, 'AmbSol', signature)
  
  #Tracks from TRT extension
  nameExtTrackCollection = "%s%sTrkTrack%s"         %(outputTrackPrefixName, 'TRText', signature)
  nameExtTrackParticles  = "%s%sTrack%s"            %(outputTrackPrefixName, 'TRText', signature)

  #Note IDTrig suffix as a key to specify that this collection comes from precision tracking rather than fast tracking (FTF)
  outPTTracks             = "%sTrkTrack_%s_%s"         %(outputTrackPrefixName, remap_signature( whichSignature ), 'IDTrig')
  outPTTrackParticles     = "%sTrack_%s_%s"            %(outputTrackPrefixName, remap_signature( whichSignature ), 'IDTrig')

  #Atm there are mainly two output track collections one from ambiguity solver stage and one from trt,
  #we want to have the output name of the track collection the same whether TRT was run or not,
  #e.g InDetTrigPT_Tracks_electron
  if whichSignature == "electron":
      doTRTextension = True
      nameExtTrackCollection = outPTTracks
      nameExtTrackParticles  = outPTTrackParticles
  else:
      nameAmbiTrackCollection = outPTTracks
      nameAmbiTrackParticles  = outPTTrackParticles



  #-----------------------------------------------------------------------------
  #                        Verifying input data for the algorithms
  
  #If run in views need to check data dependancies!
  #NOTE: this seems necessary only when PT is called from a different view than FTF otherwise causes stalls
  if verifier:
         verifier.DataObjects += [  ( 'InDet::PixelGangedClusterAmbiguities' , 'StoreGateSvc+TrigPixelClusterAmbiguitiesMap' ),
                                  ( 'TrackCollection' , 'StoreGateSvc+' + inputFTFtracks ) ] 
      
  
  from AthenaCommon.AppMgr import ToolSvc
  #-----------------------------------------------------------------------------
  #                        Choose track summary tool
  from InDetTrigRecExample.InDetTrigConfigRecLoadTools import InDetTrigTrackSummaryTool
  from TrkTrackSummaryTool.TrkTrackSummaryToolConf import Trk__TrackSummaryTool
  from InDetTrigRecExample.InDetTrigConfigRecLoadTools import InDetTrigTrackSummaryHelperTool,\
    InDetTrigTrackSummaryHelperToolSharedHits,InDetTrigTRT_ElectronPidTool

  InDetTrigTrackSummaryToolSharedHitsWithTRTPid  = Trk__TrackSummaryTool(name = "%sTrackSummaryToolSharedHitsWithTRT%s"%(algNamePrefix, signature),
                          InDetSummaryHelperTool = InDetTrigTrackSummaryHelperToolSharedHits,
                          doSharedHits           = InDetTrigFlags.doSharedHits(),
                          doHolesInDet           = True,
                          OutputLevel            = dbgLevel,
                          TRT_ElectronPidTool    = InDetTrigTRT_ElectronPidTool)


  if whichSignature == "electron":
      Parameter_config = True 
      SummaryTool_config = InDetTrigTrackSummaryToolSharedHitsWithTRTPid
  else:
      SummaryTool_config = InDetTrigTrackSummaryTool
      Parameter_config = False


  ToolSvc += SummaryTool_config


  #-----------------------------------------------------------------------------
  #                        Ambiguity solving stage

  from InDetTrigRecExample.InDetTrigConfigRecLoadTools import InDetTrigExtrapolator
  from InDetTrackScoringTools.InDetTrackScoringToolsConf import InDet__InDetAmbiScoringTool
  InDetTrigAmbiScoringTool =  InDet__InDetAmbiScoringTool( name                        = '%sScoringTool%s'%(algNamePrefix, signature) ,
                                                           Extrapolator                = InDetTrigExtrapolator,
                                                           InputEmClusterContainerName = '', #need to be reset to empty string
                                                           doEmCaloSeed                = False,
                                                           OutputLevel                 = dbgLevel,
                                                           SummaryTool                 = SummaryTool_config)

  ToolSvc += InDetTrigAmbiScoringTool


  #TODO: Need option to change scoring tool based on the slice (beamgas)
  
  from InDetTrigRecExample.InDetTrigConfigRecLoadTools import InDetTrigAmbiTrackSelectionTool

  from InDetRecExample import TrackingCommon as TrackingCommon
  from TrkAmbiguityProcessor.TrkAmbiguityProcessorConf import Trk__DenseEnvironmentsAmbiguityScoreProcessorTool as ScoreProcessorTool
  InDetTrigAmbiguityScoreProcessor = ScoreProcessorTool(     name               = '%sAmbiguityScoreProcessor%s'%(algNamePrefix, signature),
                                                             ScoringTool        = InDetTrigAmbiScoringTool,
                                                             OutputLevel        = dbgLevel,
                                                             AssociationTool    = TrackingCommon.getInDetTrigPRDtoTrackMapToolGangedPixels(),
                                                             SelectionTool      = InDetTrigAmbiTrackSelectionTool)


  from TrkAmbiguitySolver.TrkAmbiguitySolverConf import Trk__TrkAmbiguityScore
  InDetTrigAmbiguityScore = Trk__TrkAmbiguityScore(name                    = '%sAmbiguityScore%s'%(algNamePrefix, signature),
                                                   TrackInput              = [ inputFTFtracks ],
                                                   TrackOutput             = 'ScoredMap'+signature,
                                                   OutputLevel             = dbgLevel,
                                                   AmbiguityScoreProcessor = InDetTrigAmbiguityScoreProcessor 
  ) 
         
  
  
  from InDetTrigRecExample.InDetTrigConfigRecLoadTools import InDetTrigTrackFitter
  from TrkAmbiguityProcessor.TrkAmbiguityProcessorConf import Trk__SimpleAmbiguityProcessorTool as ProcessorTool
  InDetTrigMTAmbiguityProcessor = ProcessorTool(name             = '%sAmbiguityProcessor%s' %(algNamePrefix,signature),
                                                Fitter           = InDetTrigTrackFitter,
                                                ScoringTool      = InDetTrigAmbiScoringTool,
                                                AssociationTool  = TrackingCommon.getInDetTrigPRDtoTrackMapToolGangedPixels(),
                                                TrackSummaryTool = SummaryTool_config,
                                                OutputLevel      = dbgLevel,
                                                SelectionTool    = InDetTrigAmbiTrackSelectionTool)
  
  ToolSvc += InDetTrigMTAmbiguityProcessor
  
  
  from TrkAmbiguitySolver.TrkAmbiguitySolverConf import Trk__TrkAmbiguitySolver
  InDetTrigMTAmbiguitySolver = Trk__TrkAmbiguitySolver(name               = '%sAmbiguitySolver%s' %(algNamePrefix,signature),
                                                       TrackInput         = 'ScoredMap'+signature,
                                                       TrackOutput        = nameAmbiTrackCollection, 
                                                       OutputLevel        = dbgLevel,
                                                       AmbiguityProcessor = InDetTrigMTAmbiguityProcessor)
  

  ptAlgs.extend( [ InDetTrigAmbiguityScore, InDetTrigMTAmbiguitySolver] )
  if doTRTextension:
            #-----------------------------------------------------------------------------
            #                        TRT data preparation
            from AthenaCommon.AppMgr import ServiceMgr
            from AthenaCommon.GlobalFlags import globalflags
            #Only add raw data decoders if we're running over raw data
            if globalflags.InputFormat.is_bytestream():
                from TRT_ConditionsServices.TRT_ConditionsServicesConf import TRT_CalDbSvc
                InDetTRTCalDbSvc = TRT_CalDbSvc()
                ServiceMgr += InDetTRTCalDbSvc
             
                from TRT_ConditionsServices.TRT_ConditionsServicesConf import TRT_StrawStatusSummarySvc
                InDetTRTStrawStatusSummarySvc = TRT_StrawStatusSummarySvc(name = "%sStrawStatusSummarySvc%s"%(algNamePrefix,signature))
                ServiceMgr += InDetTRTStrawStatusSummarySvc
             
                from TRT_RawDataByteStreamCnv.TRT_RawDataByteStreamCnvConf import TRT_RodDecoder
                InDetTRTRodDecoder = TRT_RodDecoder(name = "%sTRTRodDecoder%s" %(algNamePrefix, signature),
                                                    LoadCompressTableDB = True)#(globalflags.DataSource() != 'geant4'))
                ToolSvc += InDetTRTRodDecoder
             
                from TRT_RawDataByteStreamCnv.TRT_RawDataByteStreamCnvConf import TRTRawDataProviderTool
                InDetTRTRawDataProviderTool = TRTRawDataProviderTool(name    = "%sTRTRawDataProviderTool%s"%(algNamePrefix, signature),
                                                                      Decoder = InDetTRTRodDecoder)
                ToolSvc += InDetTRTRawDataProviderTool
             
                # load the TRTRawDataProvider
                from TRT_RawDataByteStreamCnv.TRT_RawDataByteStreamCnvConf import TRTRawDataProvider
                InDetTRTRawDataProvider = TRTRawDataProvider(name         = "%sTRTRawDataProvider%s"%(algNamePrefix, signature),
                                                             RDOKey       = "TRT_RDOs",
                                                             ProviderTool = InDetTRTRawDataProviderTool)
                InDetTRTRawDataProvider.isRoI_Seeded = True
                InDetTRTRawDataProvider.RoIs = rois
             
             

            #-----------------------------------------------------------------------------
            #                        TRT extension
            #Keep track that this needs to have a switch between DAF and XK
            trkExtensionType = 'XK'
            #if InDetTrigFlags.trtExtensionType() is 'DAF' :

            from InDetTrigRecExample.InDetTrigCommonTools import InDetTrigTRT_DriftFunctionTool, InDetTrigTRT_DriftCircleTool
 
            from InDetTrigRecExample.InDetTrigSliceSettings import InDetTrigSliceSettings
            from InDetPrepRawDataFormation.InDetPrepRawDataFormationConf import InDet__TRT_RIO_Maker
            InDetTrigTRTRIOMaker = InDet__TRT_RIO_Maker( name = "%sTRTDriftCircleMaker%s"%(algNamePrefix, signature),
                                                     OutputLevel = dbgLevel,
                                                     #RawDataProvider = InDetTRTRawDataProvider,
                                                     TRTRIOLocation = 'TRT_TrigDriftCircles',
                                                     TRTRDOLocation = "TRT_RDOs",
                                                     #EtaHalfWidth = InDetTrigSliceSettings[('etaHalfWidth',signature)],
                                                     #PhiHalfWidth = InDetTrigSliceSettings[('phiHalfWidth',signature)],
                                                     #doFullScan =   InDetTrigSliceSettings[('doFullScan',signature)],
                                                     TRT_DriftCircleTool = InDetTrigTRT_DriftCircleTool )
 
 
            from InDetTrigRecExample.InDetTrigConfigRecLoadTools import InDetTrigPatternPropagator, InDetTrigPatternUpdator
            from InDetTrigRecExample.ConfiguredNewTrackingTrigCuts import EFIDTrackingCuts
            InDetTrigCutValues = EFIDTrackingCuts
 
            #from InDetTrigRecExample.InDetTrigConfigRecLoadTools import InDetTrigSiDetElementsRoadMaker, InDetTrigSiComTrackFinder


            # Condition algorithm for InDet__TRT_DetElementsRoadMaker_xk
            from AthenaCommon.AlgSequence import AthSequencer
            condSeq = AthSequencer("AthCondSeq")
            #if not hasattr(condSeq, "InDet__TRT_DetElementsRoadCondAlg_xk"):
            from TRT_DetElementsRoadTool_xk.TRT_DetElementsRoadTool_xkConf import InDet__TRT_DetElementsRoadCondAlg_xk
            condSeq += InDet__TRT_DetElementsRoadCondAlg_xk(name = "%sTRT_DetElementsRoadCondAlg_xk%s"%(algNamePrefix, signature) )

 

            from TRT_DetElementsRoadTool_xk.TRT_DetElementsRoadTool_xkConf import InDet__TRT_DetElementsRoadMaker_xk
            InDetTrigTRTDetElementsRoadMaker =  InDet__TRT_DetElementsRoadMaker_xk(name   = '%sTRTRoadMaker%s'%(algNamePrefix, signature),
                                                                                       #DetectorStoreLocation = 'DetectorStore',
                                                                                       #TRTManagerLocation    = 'TRT',
                                                                                       MagneticFieldMode     = 'MapSolenoid',
                                                                                       PropagatorTool        = InDetTrigPatternPropagator)


            ToolSvc += InDetTrigTRTDetElementsRoadMaker


             #TODO implement new configuration of circle cut
            from InDetTrigRecExample.InDetTrigConfigRecLoadTools import InDetTrigTRTDriftCircleCut
            from TRT_TrackExtensionTool_xk.TRT_TrackExtensionTool_xkConf import InDet__TRT_TrackExtensionTool_xk
            InDetTrigTRTExtensionTool = InDet__TRT_TrackExtensionTool_xk ( name = "%sTrackExtensionTool%s"%(algNamePrefix,signature),
                                                                           MagneticFieldMode     = 'MapSolenoid',      # default
                                                                           TRT_ClustersContainer = 'TRT_TrigDriftCircles', # default
                                                                           TrtManagerLocation    = 'TRT',              # default
                                                                           PropagatorTool = InDetTrigPatternPropagator,
                                                                           UpdatorTool    = InDetTrigPatternUpdator,
                                                                           #RIOonTrackToolYesDr = # default for now
                                                                           #RIOonTrackToolNoDr  = # default for now
                                                                           RoadTool            = InDetTrigTRTDetElementsRoadMaker,
                                                                           DriftCircleCutTool = InDetTrigTRTDriftCircleCut,
                                                                           MinNumberDriftCircles = EFIDTrackingCuts.minTRTonTrk(),
                                                                           ScaleHitUncertainty   = 2.,
                                                                           RoadWidth             = 20.,
                                                                           UseParameterization   = EFIDTrackingCuts.useParameterizedTRTCuts()
                                                                 )
 
            ToolSvc += InDetTrigTRTExtensionTool
 
 #          TODO add second option of track extension
 #                   elif InDetTrigFlags.trtExtensionType() is 'DAF' :
 
 
 
            from TRT_TrackExtensionAlg.TRT_TrackExtensionAlgConf import InDet__TRT_TrackExtensionAlg
            InDetTrigTRTextensionAlg = InDet__TRT_TrackExtensionAlg( name = "%sTrackExtensionAlg%s"%(algNamePrefix, signature),
                                                            InputTracksLocation    = nameAmbiTrackCollection,
                                                            OutputLevel            = dbgLevel,
                                                            TrackExtensionTool     = InDetTrigTRTExtensionTool,
                                                            ExtendedTracksLocation = 'ExtendedTrackMap'
                                                             )
            #-----------------------------------------------------------------------------
            #                        TRT processor

            #TODO: do I need a new fitter for this? Or can I use the same one?
            #TODO In Run2 option for cosmic
            #InDetTrigExtensionFitter = InDetTrigTrackFitter
            #from InDetTrackScoringTools.InDetTrackScoringToolsConf import InDet__InDetAmbiScoringTool
            InDetTrigExtScoringTool = InDet__InDetAmbiScoringTool(name               = '%sExtScoringTool%s'%(algNamePrefix, signature),
                                                                  Extrapolator       = InDetTrigExtrapolator,
                                                                  SummaryTool        = SummaryTool_config,
                                                                  useAmbigFcn        = True,     # this is NewTracking  
                                                                  maxRPhiImp         = InDetTrigCutValues.maxPrimaryImpact(),
                                                                  maxZImp            = InDetTrigCutValues.maxZImpact(),
                                                                  maxEta             = InDetTrigCutValues.maxEta(),
                                                                  minSiClusters      = InDetTrigCutValues.minClusters(),
                                                                  maxSiHoles         = InDetTrigCutValues.maxHoles(),
                                                                  maxDoubleHoles     = InDetTrigCutValues.maxDoubleHoles(),
                                                                  usePixel           = InDetTrigCutValues.usePixel(),
                                                                  useSCT             = InDetTrigCutValues.useSCT(),
                                                                  doEmCaloSeed       = False,
                                                                  minTRTonTrk        = InDetTrigCutValues.minTRTonTrk(),
                                                                  #useSigmaChi2   = False # tuning from Thijs
                                                                  DriftCircleCutTool = InDetTrigTRTDriftCircleCut,
                                                                  )

            InDetTrigExtScoringTool.minPt = InDetTrigSliceSettings[('pTmin',whichSignature)]

            ToolSvc += InDetTrigExtScoringTool


            from InDetExtensionProcessor.InDetExtensionProcessorConf import InDet__InDetExtensionProcessor   
            InDetTrigExtensionProcessor = InDet__InDetExtensionProcessor (name               = "%sExtensionProcessor%s"%(algNamePrefix, signature),
                                                                          TrackName          = nameAmbiTrackCollection,
                                                                          #Cosmics           = InDetFlags.doCosmics(),
                                                                          ExtensionMap       = 'ExtendedTrackMap',
                                                                          NewTrackName       = nameExtTrackCollection,
                                                                          OutputLevel        = dbgLevel,
                                                                          TrackFitter        = InDetTrigTrackFitter,
                                                                          TrackSummaryTool   = SummaryTool_config,
                                                                          ScoringTool        = InDetTrigExtScoringTool, #TODO do I provide the same tool as for ambiguity solver?
                                                                          suppressHoleSearch = False )  # does not work properly
                                                                          #Check these option after DAF is implemented
                                                                          #tryBremFit         = InDetFlags.doBremRecovery(),
                                                                          #caloSeededBrem     = InDetFlags.doCaloSeededBrem(),
                                                                          #pTminBrem          = NewTrackingCuts.minPTBrem() )
                                                                          #RefitPrds          = not (InDetFlags.refitROT() or (InDetFlags.trtExtensionType() is 'DAF')))

            #InDetTRTExtension.OutputLevel = VERBOSE
            ptAlgs.extend( [InDetTRTRawDataProvider, InDetTrigTRTRIOMaker, InDetTrigTRTextensionAlg, InDetTrigExtensionProcessor] ) 
  

  #-----------------------------------------------------------------------------
  #                      Track particle conversion algorithm
  #
  #
  """ Use the finalised track collection and convert it to container of xAOD Track Particles  """
  
  
  from TrkParticleCreator.TrkParticleCreatorConf import Trk__TrackParticleCreatorTool
  InDetTrigMTxAODParticleCreatorTool = Trk__TrackParticleCreatorTool(name =  "%sxAODParticleCreatorTool%s" %(algNamePrefix, signature),
                                                                     Extrapolator = InDetTrigExtrapolator,
                                                                     KeepParameters = Parameter_config,
                                                                     #TrackSummaryTool = InDetTrigTrackSummaryToolSharedHits) 
                                                                     TrackSummaryTool = SummaryTool_config)
  
  ToolSvc += InDetTrigMTxAODParticleCreatorTool
  log.info(InDetTrigMTxAODParticleCreatorTool)
  
  
  from xAODTrackingCnv.xAODTrackingCnvConf import xAODMaker__TrackCollectionCnvTool
  InDetTrigMTxAODTrackCollectionCnvTool= xAODMaker__TrackCollectionCnvTool(name = "%sxAODTrackCollectionCnvTool%s" %(algNamePrefix, signature),
                                                                           TrackParticleCreator = InDetTrigMTxAODParticleCreatorTool)
  
  ToolSvc += InDetTrigMTxAODTrackCollectionCnvTool
  log.info(InDetTrigMTxAODTrackCollectionCnvTool)
  
  #This one shouldn't be necessary
  #TODO: obsolete turn off
  from xAODTrackingCnv.xAODTrackingCnvConf import  xAODMaker__RecTrackParticleContainerCnvTool
  InDetTrigMTRecTrackParticleContainerCnvTool=  xAODMaker__RecTrackParticleContainerCnvTool(name = "%sRecTrackContainerCnvTool%s" %(algNamePrefix, signature),
                                                                                            TrackParticleCreator = InDetTrigMTxAODParticleCreatorTool)
  
  ToolSvc += InDetTrigMTRecTrackParticleContainerCnvTool
  #print (InDetTrigMTRecTrackParticleContainerCnvTool)
  
  from xAODTrackingCnv.xAODTrackingCnvConf import xAODMaker__TrackParticleCnvAlg
  InDetTrigMTxAODTrackParticleCnvAlg = xAODMaker__TrackParticleCnvAlg(name = "%sxAODParticleCreatorAlg%s" %( algNamePrefix, signature),
                                                                      # Properties below are used for:  TrackCollection -> xAOD::TrackParticle
                                                                        ConvertTracks = True,  #Turn on  retrieve of TrackCollection, false by default
                                                                        TrackContainerName                        = outPTTracks,
                                                                        xAODTrackParticlesFromTracksContainerName = outPTTrackParticles, 
                                                                        TrackCollectionCnvTool = InDetTrigMTxAODTrackCollectionCnvTool,
                                                                       ## Properties below are used for: Rec:TrackParticle, aod -> xAOD::TrackParticle (Turn off)
                                                                        ConvertTrackParticles = False,  # Retrieve of Rec:TrackParticle, don't need this atm
                                                                        xAODContainerName = '',  
                                                                        RecTrackParticleContainerCnvTool = InDetTrigMTRecTrackParticleContainerCnvTool,
                                                                        #PrintIDSummaryInfo = True, #Just to test and have some output
                                                                        OutputLevel = dbgLevel,
                                                                        TrackParticleCreator = InDetTrigMTxAODParticleCreatorTool
                                                                        )
  
  
  #allViewAlgorithms += InDetTrigMTxAODTrackParticleCnvAlg
  log.info(InDetTrigMTxAODTrackParticleCnvAlg)
  ptAlgs.append( InDetTrigMTxAODTrackParticleCnvAlg)
  
  #ToolSvc.InDetTrigHoleSearchTool.SctSummaryTool.InDetTrigInDetSCT_FlaggedConditionTool.SCT_FlaggedCondData = "SCT_FlaggedCondData_TRIG"
  
  
  
  #Potentialy other algs with more collections? 
  #Might Drop the list in the end and keep just one output key
  nameTrackCollections =[ outPTTracks ]
  nameTrackParticles =  [ outPTTrackParticles ]

  
  #Return list of Track keys, TrackParticle keys, and PT algs
  return  nameTrackCollections, nameTrackParticles, ptAlgs


