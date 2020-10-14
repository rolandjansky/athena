#  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
#
#           Setup of precision tracking

from __future__ import print_function

from AthenaCommon.Include import include
include.block("InDetTrigRecExample/EFInDetConfig.py")

from AthenaCommon.Logging import logging 
log = logging.getLogger("InDetPT")


#Global keys/names for collections 
from .InDetTrigCollectionKeys import TrigTRTKeys, TrigPixelKeys


def makeInDetPrecisionTracking( config = None,
                                verifier = False, 
                                rois = 'EMViewRoIs',              #FIXME: eventually should go into slice settings
                                                         ) :
  ptAlgs = [] #List containing all the precision tracking algorithms hence every new added alg has to be appended to the list

  #Expects configuration  
  if config is None:
    raise ValueError('PrecisionTracking No configuration provided!')

  #-----------------------------------------------------------------------------
  #                        Naming conventions

  algNamePrefix = "InDetTrigMT" 
  #Add suffix to the algorithms
  signature =  "_{}".format( config.name )
  
  #Name settings for output Tracks/TrackParticles
  #This first part is for ambiguity solver tracks
  nameAmbiTrackCollection = config.PT.trkTracksAS() 
  
  #Tracks from TRT extension
  nameExtTrackCollection = config.PT.trkTracksTE() 

  outPTTracks             = config.PT.trkTracksPT()
  outPTTrackParticles     = config.PT.tracksPT( doRecord = config.isRecordable )

  #Atm there are mainly two output track collections one from ambiguity solver stage and one from trt,
  #we want to have the output name of the track collection the same whether TRT was run or not,
  #Therefore, we have to adapt output names of the algorithm which produces last collection
  #However, this condition should be handled internally in configuration of the algs once TRT is configured with builders as well
  if config.PT.setting.doTRT:
     nameExtTrackCollection = outPTTracks
  else:
     nameAmbiTrackCollection = outPTTracks

  #-----------------------------------------------------------------------------
  #                        Verifying input data for the algorithms
  
  #If run in views need to check data dependancies!
  #NOTE: this seems necessary only when PT is called from a different view than FTF otherwise causes stalls
  if verifier:
    verifier.DataObjects += [( 'InDet::PixelGangedClusterAmbiguities' , 'StoreGateSvc+' + TrigPixelKeys.PixelClusterAmbiguitiesMap ),
                             ( 'TrackCollection' , 'StoreGateSvc+' + config.FT.trkTracksFTF() )]
  
  from AthenaCommon.AppMgr import ToolSvc

  #TODO: this part will not be needed once builders and getters are implemented also for TRT
  #-----------------------------------------------------------------------------
  #                        Choose track summary tool
  from InDetTrigRecExample.InDetTrigConfigRecLoadTools import InDetTrigTrackSummaryTool
  from TrkTrackSummaryTool.TrkTrackSummaryToolConf import Trk__TrackSummaryTool
  from InDetTrigRecExample.InDetTrigConfigRecLoadTools import  InDetTrigTrackSummaryHelperToolSharedHits,InDetTrigTRT_ElectronPidTool

  trigTrackSummaryTool  = Trk__TrackSummaryTool(name = "%sTrackSummaryToolSharedHitsWithTRT%s"%(algNamePrefix, signature),
                                                InDetSummaryHelperTool = InDetTrigTrackSummaryHelperToolSharedHits,
                                                doSharedHits           = True,
                                                doHolesInDet           = True )

  
  #Obsolete, will be eventually replaced
  #Note: keep Parameter_config!
  if config.PT.setting.doTRT:
      if "electron" in config.name  or "tau" in config.name:
         trigTrackSummaryTool.TRT_ElectronPidTool = InDetTrigTRT_ElectronPidTool

      #Parameter_config = True 
      SummaryTool_config = trigTrackSummaryTool
      ToolSvc += SummaryTool_config
  else:
      SummaryTool_config = InDetTrigTrackSummaryTool
      #Parameter_config = False


  #-----------------------------------------------------------------------------
  #                        Ambiguity solving stage
  from .InDetTrigCommon import ambiguityScoreAlg_builder, ambiguitySolverAlg_builder, get_full_name
  ambSolvingStageAlgs = [
                           ambiguityScoreAlg_builder( name   = get_full_name(  core = 'TrkAmbiguityScore', suffix  = config.name ),
                                                      config = config ),

                           ambiguitySolverAlg_builder( name   = get_full_name( core = 'TrkAmbiguitySolver', suffix = config.name ),
                                                       config = config )
                        ]

  #Loading the alg to the sequence
  ptAlgs.extend( ambSolvingStageAlgs )

  from InDetTrigRecExample.InDetTrigConfigRecLoadTools import  InDetTrigExtrapolator
  #TODO:implement builders and getters for TRT (WIP)
  if config.PT.setting.doTRT:

            #-----------------------------------------------------------------------------
            #                        TRT data preparation
            from AthenaCommon.GlobalFlags import globalflags
            #Only add raw data decoders if we're running over raw data
            TRT_RDO_Key = "TRT_RDOs"
            if globalflags.InputFormat.is_bytestream():
                TRT_RDO_Key = TrigTRTKeys.RDOs
                from TRT_RawDataByteStreamCnv.TRT_RawDataByteStreamCnvConf import TRT_RodDecoder
                InDetTRTRodDecoder = TRT_RodDecoder( name = "%sTRTRodDecoder%s" %(algNamePrefix, signature),
                                                     LoadCompressTableDB = True )#(globalflags.DataSource() != 'geant4'))
                ToolSvc += InDetTRTRodDecoder
             
                from TRT_RawDataByteStreamCnv.TRT_RawDataByteStreamCnvConf import TRTRawDataProviderTool
                InDetTRTRawDataProviderTool = TRTRawDataProviderTool( name    = "%sTRTRawDataProviderTool%s"%(algNamePrefix, signature),
                                                                      Decoder = InDetTRTRodDecoder )

                ToolSvc += InDetTRTRawDataProviderTool
             



                # load the TRTRawDataProvider
                from TRT_RawDataByteStreamCnv.TRT_RawDataByteStreamCnvConf import TRTRawDataProvider
                InDetTRTRawDataProvider = TRTRawDataProvider(name         = "%sTRTRawDataProvider%s"%(algNamePrefix, signature),
                                                             RDOKey       = TrigTRTKeys.RDOs,
                                                             ProviderTool = InDetTRTRawDataProviderTool)

                from RegionSelector.RegSelToolConfig import makeRegSelTool_TRT
                InDetTRTRawDataProvider.RegSelTool = makeRegSelTool_TRT()

                InDetTRTRawDataProvider.isRoI_Seeded = True
                InDetTRTRawDataProvider.RoIs = rois

                ptAlgs.append( InDetTRTRawDataProvider )
             
             

            #-----------------------------------------------------------------------------
            #                        TRT extension
            # Keep track that this needs to have a switch between DAF and XK
            # trkExtensionType = 'XK'
            # if InDetTrigFlags.trtExtensionType() is 'DAF' :

            from InDetTrigRecExample.InDetTrigCommonTools import  InDetTrigTRT_DriftCircleTool
 
            #from InDetTrigRecExample.InDetTrigSliceSettings import InDetTrigSliceSettings
            from InDetPrepRawDataFormation.InDetPrepRawDataFormationConf import InDet__TRT_RIO_Maker
            InDetTrigTRTRIOMaker = InDet__TRT_RIO_Maker( name = "%sTRTDriftCircleMaker%s"%(algNamePrefix, signature),
                                                     TRTRIOLocation = TrigTRTKeys.DriftCircles,
                                                     TRTRDOLocation = TRT_RDO_Key,
                                                     #FIXME:
                                                     #EtaHalfWidth = InDetTrigSliceSettings[('etaHalfWidth',signature)],
                                                     #PhiHalfWidth = InDetTrigSliceSettings[('phiHalfWidth',signature)],
                                                     #doFullScan =   InDetTrigSliceSettings[('doFullScan',signature)],
                                                     TRT_DriftCircleTool = InDetTrigTRT_DriftCircleTool )
            InDetTrigTRTRIOMaker.isRoI_Seeded = True
            InDetTrigTRTRIOMaker.RoIs = rois

            from RegionSelector.RegSelToolConfig import makeRegSelTool_TRT
            InDetTrigTRTRIOMaker.RegSelTool = makeRegSelTool_TRT()
 
 
            from InDetTrigRecExample.InDetTrigConfigRecLoadTools import InDetTrigPatternPropagator, InDetTrigPatternUpdator
            from InDetTrigRecExample.ConfiguredNewTrackingTrigCuts import EFIDTrackingCuts
            InDetTrigCutValues = EFIDTrackingCuts
 
            #from InDetTrigRecExample.InDetTrigConfigRecLoadTools import InDetTrigSiDetElementsRoadMaker, InDetTrigSiComTrackFinder


            # Condition algorithm for InDet__TRT_DetElementsRoadMaker_xk
            from AthenaCommon.AlgSequence import AthSequencer
            condSeq = AthSequencer("AthCondSeq")
            

            InDetTRTRoadAlgName = "%sTRT_DetElementsRoadCondAlg_xk"%(algNamePrefix)

            if not hasattr( condSeq, InDetTRTRoadAlgName ):
               from TRT_DetElementsRoadTool_xk.TRT_DetElementsRoadTool_xkConf import InDet__TRT_DetElementsRoadCondAlg_xk

               # condSeq += InDet__TRT_DetElementsRoadCondAlg_xk(name = "%sTRT_DetElementsRoadCondAlg_xk%s"%(algNamePrefix, signature) )
               condSeq += InDet__TRT_DetElementsRoadCondAlg_xk( name = InDetTRTRoadAlgName )


            from TRT_DetElementsRoadTool_xk.TRT_DetElementsRoadTool_xkConf import InDet__TRT_DetElementsRoadMaker_xk
            InDetTrigTRTDetElementsRoadMaker =  InDet__TRT_DetElementsRoadMaker_xk( name   = '%sTRTRoadMaker%s'%(algNamePrefix, signature),
                                                                                    #DetectorStoreLocation = 'DetectorStore',
                                                                                    #TRTManagerLocation    = 'TRT',
                                                                                    MagneticFieldMode     = 'MapSolenoid',
                                                                                    PropagatorTool        = InDetTrigPatternPropagator )
            

            ToolSvc += InDetTrigTRTDetElementsRoadMaker

             #TODO implement new configuration of circle cut
            from InDetTrigRecExample.InDetTrigConfigRecLoadTools import InDetTrigTRTDriftCircleCut
            from TRT_TrackExtensionTool_xk.TRT_TrackExtensionTool_xkConf import InDet__TRT_TrackExtensionTool_xk
            InDetTrigTRTExtensionTool = InDet__TRT_TrackExtensionTool_xk ( name = "%sTrackExtensionTool%s"%(algNamePrefix,signature),
                                                                           MagneticFieldMode     = 'MapSolenoid',      # default
                                                                           TRT_ClustersContainer = TrigTRTKeys.DriftCircles, # default
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
                                                            TrackExtensionTool     = InDetTrigTRTExtensionTool,
                                                            ExtendedTracksLocation = 'ExtendedTrackMap'
                                                             )
            #-----------------------------------------------------------------------------
            #                        TRT processor

            #TODO: do I need a new fitter for this? Or can I use the same one?
            #TODO In Run2 option for cosmic
            #InDetTrigExtensionFitter = InDetTrigTrackFitter
            from InDetTrackScoringTools.InDetTrackScoringToolsConf import InDet__InDetAmbiScoringTool
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

            InDetTrigExtScoringTool.minPt = config.PT.setting.pTmin 

            ToolSvc += InDetTrigExtScoringTool


            from InDetTrigRecExample.InDetTrigConfigRecLoadTools import InDetTrigTrackFitter
            from InDetExtensionProcessor.InDetExtensionProcessorConf import InDet__InDetExtensionProcessor   
            InDetTrigExtensionProcessor = InDet__InDetExtensionProcessor (name               = "%sExtensionProcessor%s"%(algNamePrefix, signature),
                                                                          TrackName          = nameAmbiTrackCollection,
                                                                          #Cosmics           = InDetFlags.doCosmics(),
                                                                          ExtensionMap       = 'ExtendedTrackMap',
                                                                          NewTrackName       = nameExtTrackCollection,
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
            ptAlgs.extend( [ InDetTrigTRTRIOMaker, InDetTrigTRTextensionAlg, InDetTrigExtensionProcessor] ) 
  

  #-----------------------------------------------------------------------------
  #                      Track particle conversion algorithm
  #
  #
  from .InDetTrigCommon import trackParticleCnv_builder
  trackParticleCnvAlg = trackParticleCnv_builder(name                 = get_full_name( 'xAODParticleCreatorAlg',config.name + '_IDTrig' ), #IDTrig suffix signifies that this is for precision tracking
                                                 config               = config,
                                                 inTrackCollectionKey = outPTTracks,
                                                 outTrackParticlesKey = outPTTrackParticles,
                                                 )
  log.debug(trackParticleCnvAlg)
  ptAlgs.append(trackParticleCnvAlg)

  #ToolSvc.InDetTrigHoleSearchTool.SctSummaryTool.InDetTrigInDetSCT_FlaggedConditionTool.SCT_FlaggedCondData = "SCT_FlaggedCondData_TRIG"
  
  
  
  #Potentialy other algs with more collections? 
  #Might Drop the list in the end and keep just one output key
  nameTrackCollections =[ outPTTracks ]
  nameTrackParticles =  [ outPTTrackParticles ]

  
  #Return list of Track keys, TrackParticle keys, and PT algs
  return  nameTrackCollections, nameTrackParticles, ptAlgs


