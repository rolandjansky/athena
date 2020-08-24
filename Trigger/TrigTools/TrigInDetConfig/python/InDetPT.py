#  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
#
#           Setup of precision tracking

from __future__ import print_function

from AthenaCommon.Include import include
include.block("InDetTrigRecExample/EFInDetConfig.py")
include("InDetTrigRecExample/InDetTrigRec_jobOptions.py") # this is needed to get InDetTrigFlags
from InDetTrigRecExample.InDetTrigFlags import InDetTrigFlags

from AthenaCommon.Logging import logging 
log = logging.getLogger("InDetPT")


#Global keys/names for collections 
from .InDetTrigCollectionKeys import TrigTRTKeys, TrigPixelKeys




# Start using already decided naming conventions
# NB: this is only needed at the moment since the signature menu code is 
#     inconcistent in what flags they pass the ID Trigger configuration 
#     for the FTF And precision tracking. Once that is tidied up, this
#     should be removed
 
def remapSuffix( signature ):
   suffix_map = { 
      'electron':  'Electron', 
      'muon':      'Muon',
      'muonFS':    'MuonFS',
      'muonLate':  'MuonLate',
      'muonIso':   'MuonIso',
      'bjet':      'Bjet',
      'tau':       'Tau',
      'tauId':     'Tau',
      'tauEF':     'Tau',
      'tauTrk':    'Tau',
      'tauTrkTwo': 'Tau'
   } 


   
   if signature in suffix_map :
      return suffix_map[ signature ]

   return signature
   

def makeInDetPrecisionTracking( whichSignature, 
                                verifier = False, 
                                inputFTFtracks='TrigFastTrackFinder_Tracks', 
                                outputTrackPrefixName = "HLT_ID", 
                                rois = 'EMViewRoIs', 
                                doTRTextension = True ) :

  ptAlgs = [] #List containing all the precision tracking algorithms hence every new added alg has to be appended to the list

  #-----------------------------------------------------------------------------
  #                        Naming conventions

  algNamePrefix = "InDetTrigMT" 
  #If signature specified add suffix to the algorithms
  signature =  "_" + whichSignature if whichSignature else ''
  
  #Name settings for output Tracks/TrackParticles
  #This first part is for ambiguity solver tracks
  nameAmbiTrackCollection = "%s%sTrkTrack%s"         %(outputTrackPrefixName, 'AmbSol', signature)
  
  #Tracks from TRT extension
  nameExtTrackCollection = "%s%sTrkTrack%s"         %(outputTrackPrefixName, 'TRText', signature)

  #Note IDTrig suffix as a key to specify that this collection comes from precision tracking rather than fast tracking (FTF)
  outPTTracks             = "%sTrkTrack_%s_%s"         %(outputTrackPrefixName, remapSuffix( whichSignature ), 'IDTrig')
  outPTTrackParticles     = "%sTrack_%s_%s"            %(outputTrackPrefixName, remapSuffix( whichSignature ), 'IDTrig')

  #Load signature configuration (containing cut values, names of collections, etc)
  from .InDetTrigConfigSettings import getInDetTrigConfig     

  #Remap only temporary until signatures fix their naming
  configSetting = getInDetTrigConfig( whichSignature, doRemap = False )

  #Atm there are mainly two output track collections one from ambiguity solver stage and one from trt,
  #we want to have the output name of the track collection the same whether TRT was run or not,
  #e.g InDetTrigPT_Tracks_electron
  if configSetting.doTRT:
     nameExtTrackCollection = outPTTracks
  else:
     nameAmbiTrackCollection = outPTTracks



  #-----------------------------------------------------------------------------
  #                        Verifying input data for the algorithms
  
  #If run in views need to check data dependancies!
  #NOTE: this seems necessary only when PT is called from a different view than FTF otherwise causes stalls
  if verifier:
    verifier.DataObjects += [( 'InDet::PixelGangedClusterAmbiguities' , 'StoreGateSvc+' + TrigPixelKeys.PixelClusterAmbiguitiesMap ),
                             ( 'TrackCollection' , 'StoreGateSvc+' + inputFTFtracks )]
  
  from AthenaCommon.AppMgr import ToolSvc

  #TODO: this part will not be needed once builders and getters are implemented also for TRT
  #-----------------------------------------------------------------------------
  #                        Choose track summary tool
  from InDetTrigRecExample.InDetTrigConfigRecLoadTools import InDetTrigTrackSummaryTool
  from TrkTrackSummaryTool.TrkTrackSummaryToolConf import Trk__TrackSummaryTool
  from InDetTrigRecExample.InDetTrigConfigRecLoadTools import  InDetTrigTrackSummaryHelperToolSharedHits,InDetTrigTRT_ElectronPidTool

  trigTrackSummaryTool  = Trk__TrackSummaryTool(name = "%sTrackSummaryToolSharedHitsWithTRT%s"%(algNamePrefix, signature),
                                                InDetSummaryHelperTool = InDetTrigTrackSummaryHelperToolSharedHits,
                                                doSharedHits           = InDetTrigFlags.doSharedHits(),
                                                doHolesInDet           = True )
  
  #OBSOLETE but keep Parameter_config
  if configSetting.doTRT:
      if "electron" in whichSignature  or "tau" in whichSignature :
         trigTrackSummaryTool.TRT_ElectronPidTool = InDetTrigTRT_ElectronPidTool

      Parameter_config = True 
      SummaryTool_config = trigTrackSummaryTool
  else:
      SummaryTool_config = InDetTrigTrackSummaryTool
      Parameter_config = False


  ToolSvc += SummaryTool_config


  #-----------------------------------------------------------------------------
  #                        Ambiguity solving stage
  from .InDetTrigCommon import ambiguityScoreAlg_builder, ambiguitySolverAlg_builder, get_full_name
  ambSolvingStageAlgs = [
                           ambiguityScoreAlg_builder( name   = get_full_name(  core = 'TrkAmbiguityScore', suffix  = configSetting.name() ),
                                                   config = configSetting ),

                           ambiguitySolverAlg_builder( name   = get_full_name( core = 'TrkAmbiguitySolver', suffix = configSetting.name() ),
                                                    config = configSetting )
                        ]

  #Loading the alg to the sequence
  ptAlgs.extend( ambSolvingStageAlgs )

  print('MAT ALGS')
  for alg in ptAlgs:
      print(alg)

  #TODO:implement builders and getters for TRT
  if configSetting.doTRT:

            proxySignature = whichSignature
            if "tau" in whichSignature :
               proxySignature = "tau"
     

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
                InDetTRTRawDataProvider.isRoI_Seeded = True
                InDetTRTRawDataProvider.RoIs = rois

                ptAlgs.append( InDetTRTRawDataProvider )
             
             

            #-----------------------------------------------------------------------------
            #                        TRT extension
            # Keep track that this needs to have a switch between DAF and XK
            # trkExtensionType = 'XK'
            # if InDetTrigFlags.trtExtensionType() is 'DAF' :

            from InDetTrigRecExample.InDetTrigCommonTools import  InDetTrigTRT_DriftCircleTool
 
            from InDetTrigRecExample.InDetTrigSliceSettings import InDetTrigSliceSettings
            from InDetPrepRawDataFormation.InDetPrepRawDataFormationConf import InDet__TRT_RIO_Maker
            InDetTrigTRTRIOMaker = InDet__TRT_RIO_Maker( name = "%sTRTDriftCircleMaker%s"%(algNamePrefix, signature),
                                                     TRTRIOLocation = TrigTRTKeys.DriftCircles,
                                                     TRTRDOLocation = TRT_RDO_Key,
                                                     #EtaHalfWidth = InDetTrigSliceSettings[('etaHalfWidth',signature)],
                                                     #PhiHalfWidth = InDetTrigSliceSettings[('phiHalfWidth',signature)],
                                                     #doFullScan =   InDetTrigSliceSettings[('doFullScan',signature)],
                                                     TRT_DriftCircleTool = InDetTrigTRT_DriftCircleTool )
            InDetTrigTRTRIOMaker.isRoI_Seeded = True
            InDetTrigTRTRIOMaker.RoIs = rois
 
 
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
            from InDetTrigRecExample.InDetTrigConfigRecLoadTools import  InDetTrigExtrapolator
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

            InDetTrigExtScoringTool.minPt = InDetTrigSliceSettings[('pTmin', proxySignature)]

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
  """ Use the finalised track collection and convert it to container of xAOD Track Particles  """
  
  
  from TrkParticleCreator.TrkParticleCreatorConf import Trk__TrackParticleCreatorTool
  InDetTrigMTxAODParticleCreatorTool = Trk__TrackParticleCreatorTool(name =  "%sxAODParticleCreatorTool%s" %(algNamePrefix, signature),
                                                                     Extrapolator = InDetTrigExtrapolator,
                                                                     KeepParameters = Parameter_config,
                                                                     #TrackSummaryTool = InDetTrigTrackSummaryToolSharedHits) 
                                                                     TrackSummaryTool = SummaryTool_config)
  
  ToolSvc += InDetTrigMTxAODParticleCreatorTool
  log.debug(InDetTrigMTxAODParticleCreatorTool)
  
  
  from xAODTrackingCnv.xAODTrackingCnvConf import xAODMaker__TrackCollectionCnvTool
  InDetTrigMTxAODTrackCollectionCnvTool= xAODMaker__TrackCollectionCnvTool(
     name = "%sxAODTrackCollectionCnvTool%s" %(algNamePrefix, signature),
     TrackParticleCreator = InDetTrigMTxAODParticleCreatorTool)
  

  ToolSvc += InDetTrigMTxAODTrackCollectionCnvTool
  log.debug(InDetTrigMTxAODTrackCollectionCnvTool)
  
  #This one shouldn't be necessary
  #TODO: obsolete turn off
  from xAODTrackingCnv.xAODTrackingCnvConf import  xAODMaker__RecTrackParticleContainerCnvTool
  InDetTrigMTRecTrackParticleContainerCnvTool=  xAODMaker__RecTrackParticleContainerCnvTool(
     name = "%sRecTrackContainerCnvTool%s" %(algNamePrefix, signature),
     TrackParticleCreator = InDetTrigMTxAODParticleCreatorTool )
  
  ToolSvc += InDetTrigMTRecTrackParticleContainerCnvTool
  #print (InDetTrigMTRecTrackParticleContainerCnvTool)
  
  from TrigEDMConfig.TriggerEDMRun3 import recordable
  from xAODTrackingCnv.xAODTrackingCnvConf import xAODMaker__TrackParticleCnvAlg
  InDetTrigMTxAODTrackParticleCnvAlg = xAODMaker__TrackParticleCnvAlg( name = "%sxAODParticleCreatorAlg%s" %( algNamePrefix, signature),
                                                                      # Properties below are used for:  TrackCollection -> xAOD::TrackParticle
                                                                        ConvertTracks = True,  #Turn on  retrieve of TrackCollection, false by default
                                                                        TrackContainerName                        = outPTTracks,
                                                                        xAODTrackParticlesFromTracksContainerName = recordable( outPTTrackParticles ), 
                                                                        TrackCollectionCnvTool = InDetTrigMTxAODTrackCollectionCnvTool,
                                                                       ## Properties below are used for: Rec:TrackParticle, aod -> xAOD::TrackParticle (Turn off)
                                                                        ConvertTrackParticles = False,  # Retrieve of Rec:TrackParticle, don't need this atm
                                                                        xAODContainerName = '',  
                                                                        RecTrackParticleContainerCnvTool = InDetTrigMTRecTrackParticleContainerCnvTool,
                                                                        #PrintIDSummaryInfo = True, #Just to test and have some output
                                                                        TrackParticleCreator = InDetTrigMTxAODParticleCreatorTool
                                                                        )
  
  
  #allViewAlgorithms += InDetTrigMTxAODTrackParticleCnvAlg
  log.debug(InDetTrigMTxAODTrackParticleCnvAlg)
  ptAlgs.append( InDetTrigMTxAODTrackParticleCnvAlg)
  
  #ToolSvc.InDetTrigHoleSearchTool.SctSummaryTool.InDetTrigInDetSCT_FlaggedConditionTool.SCT_FlaggedCondData = "SCT_FlaggedCondData_TRIG"
  
  
  
  #Potentialy other algs with more collections? 
  #Might Drop the list in the end and keep just one output key
  nameTrackCollections =[ outPTTracks ]
  nameTrackParticles =  [ outPTTrackParticles ]

  
  #Return list of Track keys, TrackParticle keys, and PT algs
  return  nameTrackCollections, nameTrackParticles, ptAlgs


