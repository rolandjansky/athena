#  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
#
#           Setup of offline pattern recognition tracking for ID Trigger
#Heavily inspired by the offline version:
#https://gitlab.cern.ch/atlas/athena/blob/master/InnerDetector/InDetExample/InDetRecExample/share/ConfiguredNewTrackingSiPattern.py

from AthenaCommon.Include import include
include("InDetTrigRecExample/InDetTrigRec_jobOptions.py") # this is needed to get InDetTrigFlags

from AthenaCommon.Logging import logging 
log = logging.getLogger("EFIDTracking")


#Create a view verifier for necessary data collections
def get_idtrig_view_verifier(name):
   import AthenaCommon.CfgMgr as CfgMgr
   from AthenaCommon.GlobalFlags import globalflags
   from .InDetTrigCollectionKeys import  TrigPixelKeys, TrigSCTKeys
   from InDetRecExample.InDetKeys import InDetKeys
   from TrigInDetConfig.TrigInDetConfig import InDetCacheNames
   viewDataVerifier = CfgMgr.AthViews__ViewDataVerifier( name )
   viewDataVerifier.DataObjects = []

   #Having these (clusters) uncommented breaks cosmic when data preparation is right before offline pattern rec
   #Probably it tries to fetch the data before the actual alg producing them runs?
   #Not case in other signatures where data preparation and offline patern recognition are in different views
   if 'cosmics' not in name:
      viewDataVerifier.DataObjects += [
                                       ( 'SpacePointContainer',           TrigSCTKeys.SpacePoints ),
                                       ( 'SpacePointContainer',           TrigPixelKeys.SpacePoints ),
                                       ( 'SpacePointOverlapCollection',   'StoreGateSvc+OverlapSpacePoints' ),
                                       ( 'InDet::PixelGangedClusterAmbiguities' , 'StoreGateSvc+TrigPixelClusterAmbiguitiesMap' ),
                                       ( 'InDet::SCT_ClusterContainer',   TrigSCTKeys.Clusters ),
                                       ( 'InDet::PixelClusterContainer',  TrigPixelKeys.Clusters ),
                                       ( 'IDCInDetBSErrContainer',        'StoreGateSvc+SCT_FlaggedCondData_TRIG' ),
                                      ]
      if globalflags.InputFormat.is_bytestream():
         viewDataVerifier.DataObjects += [
                                          ( 'IDCInDetBSErrContainer' , 'StoreGateSvc+SCT_ByteStreamErrs' ),
                                          ( 'IDCInDetBSErrContainer' , 'StoreGateSvc+PixelByteStreamErrs' ),
                                         ]
   
   #FIXME:
   #Align with the data preparation, are all of them  really needed in the EFID ?
   viewDataVerifier.DataObjects += [ ( 'InDet::PixelClusterContainerCache' , InDetCacheNames.Pixel_ClusterKey ),
                                     ( 'PixelRDO_Cache' , InDetCacheNames.PixRDOCacheKey ),
                                     ( 'InDet::SCT_ClusterContainerCache' , InDetCacheNames.SCT_ClusterKey ),
                                     ( 'SCT_RDO_Cache' , InDetCacheNames.SCTRDOCacheKey ),
                                     ( 'SpacePointCache' , InDetCacheNames.SpacePointCachePix ),
                                     ( 'SpacePointCache' , InDetCacheNames.SpacePointCacheSCT ),
                                     ( 'IDCInDetBSErrContainer_Cache' , InDetCacheNames.PixBSErrCacheKey ),
                                     ( 'IDCInDetBSErrContainer_Cache' , InDetCacheNames.SCTBSErrCacheKey ),
                                     ( 'IDCInDetBSErrContainer_Cache' , InDetCacheNames.SCTFlaggedCondCacheKey ),
                                     ( 'xAOD::EventInfo' , 'StoreGateSvc+EventInfo' ),
                                     ( 'TagInfo' , 'DetectorStore+ProcessingTags' )]

   
  # Load RDOs if we aren't loading bytestream
   from AthenaCommon.AlgSequence import AlgSequence
   topSequence = AlgSequence()
  
   topSequence.SGInputLoader.Load += [ ( 'TagInfo' , 'DetectorStore+ProcessingTags' ) ]
   
   if not globalflags.InputFormat.is_bytestream():
     viewDataVerifier.DataObjects +=   [( 'PixelRDO_Container' , InDetKeys.PixelRDOs() ),
                                        ( 'SCT_RDO_Container' , InDetKeys.SCT_RDOs() ),
                                        ]
     topSequence.SGInputLoader.Load += [( 'PixelRDO_Container' , InDetKeys.PixelRDOs() ),
                                        ( 'SCT_RDO_Container' , InDetKeys.SCT_RDOs() ),
                                        ]
   
   return viewDataVerifier


#FIXME: temporary before we port offline cuts to config settings
def remapToOffline( name ):
   if name == 'cosmics':
      return 'Cosmics'
   else:
       return name

def makeInDetPatternRecognition( config, verifier = 'IDTrigViewDataVerifier'  ):
      viewAlgs = [] #list of all algs running in this module

      dataVerifier = None
      if verifier:
         dataVerifier = get_idtrig_view_verifier(verifier+config.input_name)
         viewAlgs.append( dataVerifier )

      #FIXME:  eventually adapt the cuts in the configsetting ATR-22755
      mode_name = remapToOffline( config.name )
      if config.name == "cosmics" or config.name == "minBias":
         from InDetTrigRecExample.InDetTrigTrackingCuts import InDetTrigTrackingCuts
         trackingCuts = InDetTrigTrackingCuts( mode_name ) 
      #MinBias cuts need to be revisited: ATR-23077
      else:
         from InDetRecExample.ConfiguredNewTrackingCuts import ConfiguredNewTrackingCuts
         trackingCuts = ConfiguredNewTrackingCuts( mode_name ) 
      #trackingCuts.printInfo() 



      # --- decide if use the association tool
      usePrdAssociationTool = False 
      #FIXME: Do we need this switch? If so, make the same decision as offline (based on the tracking cuts)? ATR-22755
      #Are all of these needed?
      #if (len(InputCollections) > 0) and (trackingCuts.mode() == "LowPt" or trackingCuts.mode() == "VeryLowPt" or trackingCuts.mode() == "LargeD0" or trackingCuts.mode() == "LowPtLargeD0" or trackingCuts.mode() == "BeamGas" or trackingCuts.mode() == "ForwardTracks" or trackingCuts.mode() == "ForwardSLHCTracks"  or trackingCuts.mode() == "Disappearing" or trackingCuts.mode() == "VeryForwardSLHCTracks" or trackingCuts.mode() == "SLHCConversionFinding"):
      #usePrdAssociationTool = True
      if usePrdAssociationTool:
         from .InDetTrigCommon import prdAssociation_builder
         InputCollections = None #Dummy atm
         prdAssociation = prdAssociation_builder( InputCollections )
         viewAlgs.append( prdAssociation )


      #-----------------------------------------------------------------------------
      #                      Track building stage

      # not sure yet about the TRT extension ...   
      #   from InDetTrigRecExample.InDetTrigConfigRecLoadTools import InDetTrigTrackSummaryTool
      #   from TrigInDetConf.TrigInDetRecCommonTools           import InDetTrigTrackSummaryToolSharedHitsWithTRTPid
         
      #   #Load shared hits with Ele PID if TRT specified
      #   if doTRT:
      #      return InDetTrigTrackSummaryToolSharedHitsWithTRTPid
      #   else:
      #      return InDetTrigTrackSummaryTool

      from InDetTrigRecExample.InDetTrigConfigRecLoadTools import InDetTrigTrackSummaryTool
      summaryTool = InDetTrigTrackSummaryTool


      # FIXME Use trigger flags instead of indetflags ATR-22756
      # What are the instances when we don't need this?
      # if InDetFlags.doSiSPSeededTrackFinder():

      doSiSPSeededTrackFinder = True 

      if doSiSPSeededTrackFinder:

         #FIXME: do we need this covered by detflag condition? ATR-22756
         #from AthenaCommon.DetFlags import DetFlags 
         # --- Loading Pixel, SCT conditions
         if True: #DetFlags.haveRIO.pixel_on():
            from AthenaCommon.AlgSequence import AthSequencer
            condSeq = AthSequencer("AthCondSeq")
            if not hasattr(condSeq, "InDetSiDetElementBoundaryLinksPixelCondAlg"):
               from SiCombinatorialTrackFinderTool_xk.SiCombinatorialTrackFinderTool_xkConf import InDet__SiDetElementBoundaryLinksCondAlg_xk
               condSeq += InDet__SiDetElementBoundaryLinksCondAlg_xk(name     = "InDetSiDetElementBoundaryLinksPixelCondAlg",
                                                                     ReadKey  = "PixelDetectorElementCollection",
                                                                     WriteKey = "PixelDetElementBoundaryLinks_xk",)


         if True: #FIXME trackingCuts.useSCT()? ATR-22756
            from AthenaCommon.AlgSequence import AthSequencer
            condSeq = AthSequencer("AthCondSeq")
            if not hasattr(condSeq, "InDet__SiDetElementsRoadCondAlg_xk"):
               from SiDetElementsRoadTool_xk.SiDetElementsRoadTool_xkConf import InDet__SiDetElementsRoadCondAlg_xk
               condSeq += InDet__SiDetElementsRoadCondAlg_xk(name = "InDet__SiDetElementsRoadCondAlg_xk")

            if not hasattr(condSeq, "InDetSiDetElementBoundaryLinksSCTCondAlg"):
               from SiCombinatorialTrackFinderTool_xk.SiCombinatorialTrackFinderTool_xkConf import InDet__SiDetElementBoundaryLinksCondAlg_xk
               condSeq += InDet__SiDetElementBoundaryLinksCondAlg_xk(name     = "InDetSiDetElementBoundaryLinksSCTCondAlg",
                                                                  ReadKey  = "SCT_DetectorElementCollection",
                                                                  WriteKey = "SCT_DetElementBoundaryLinks_xk")
            #-------------------------------------------------------

         from .InDetTrigCommon import siSPSeededTrackFinder_builder, add_prefix
         siSPSeededTrackFinder = siSPSeededTrackFinder_builder( name                  = add_prefix( 'siSPSeededTrackFinder', config.input_name ),
                                                                config                = config,
                                                                outputTracks          = config.trkTracks_IDTrig(), 
                                                                trackingCuts          = trackingCuts,
                                                                usePrdAssociationTool = usePrdAssociationTool,
                                                                nameSuffix            = config.input_name,
                                                                trackSummaryTool      = summaryTool )

         viewAlgs.append( siSPSeededTrackFinder )

      #This code is expected to be used for monitoring purposes and comparison between first and second stage but atm disabled
      #-----------------------------------------------------------------------------
      #                      Track particle conversion algorithm (for pattern rec.)
      #                        atm disabled but might be useful later for debugging
      #
      #from .InDetTrigCommon import trackParticleCnv_builder
      #trackParticleCnvAlg = trackParticleCnv_builder(name                 = add_prefix( 'xAODParticleCreatorAlg',config.name + '_EFID' ), 
      #                                               config               = config,
      #                                               inTrackCollectionKey = config.trkTracks_IDTrig(),
      #                                               outTrackParticlesKey = config.tracks_EFID(),
      #                                               )

      #-----------------------------------------------------------------------------
      #                      Precision algorithms

      #Verifier should not be necessary when both patt. rec. and PT runs in the same view -> None
      #Also provides particle cnv alg inside
      precisionAlgs = ambiguitySolverForIDPatternRecognition(config      = config,
                                                             inputTracks = config.trkTracks_IDTrig(), 
                                                             verifier    = None, 
                                                             summaryTool = summaryTool )


      viewAlgs += precisionAlgs


      return  viewAlgs, dataVerifier


# This could potentially be unified with makeInDetTrigPrecisionTracking in the InDetTrigPrecisionTracking.py?
def ambiguitySolverForIDPatternRecognition( config, summaryTool, inputTracks,verifier=None ):
   ptAlgs = [] #List containing all the precision tracking algorithms hence every new added alg has to be appended to the list
   
   #-----------------------------------------------------------------------------
   #                        Verifying input data for the algorithms
   if verifier:
     verifier.DataObjects += [ #( 'InDet::PixelGangedClusterAmbiguities' , 'StoreGateSvc+' + TrigPixelKeys.PixelClusterAmbiguitiesMap ),
                               ( 'TrackCollection' , 'StoreGateSvc+' + inputTracks )]
   
   
   #-----------------------------------------------------------------------------
   #                        Ambiguity solving stage
   from .InDetTrigCommon import ambiguityScoreAlg_builder, ambiguitySolverAlg_builder, add_prefix

   scoreMapName = "ScoreMap"+config.input_name
   
   ambiguityScoreAlg = ambiguityScoreAlg_builder( name                  = add_prefix( core='TrkAmbiguityScore', suffix=config.input_name ),
                                                  config                = config,
                                                  inputTrackCollection  = inputTracks,
                                                  outputTrackScoreMap   = scoreMapName )
                                                 
   ptAlgs.append( ambiguityScoreAlg )

   #FIXME: these alg internally don't expect EFID setting (but FTF), have to take into consideration

   ambiguitySolverAlg = ambiguitySolverAlg_builder( name                  = add_prefix( core='TrkAmbiguitySolver', suffix=config.input_name ),
                                                    config                = config,
                                                    summaryTool           = summaryTool,
                                                    inputTrackScoreMap    = scoreMapName, #Map of tracks and their scores, 
                                                    outputTrackCollection = config.trkTracks_IDTrig()+"_Amb" )  #FIXME: for now keep PT but if TRT added this will ahve to become intermediate collection

   ptAlgs.append( ambiguitySolverAlg )
   
   #-----------------------------------------------------------------------------
   #                      Track particle conversion algorithm
   from .InDetTrigCommon import trackParticleCnv_builder
   from InDetTrigRecExample.InDetTrigConfigRecLoadToolsPost import InDetTrigParticleCreatorToolWithSummary
   creatorTool = InDetTrigParticleCreatorToolWithSummary
   
   trackParticleCnvAlg = trackParticleCnv_builder(name                 = add_prefix( 'xAODParticleCreatorAlg', config.name + '_IDTrig' ), 
                                                  config               = config,
                                                  inTrackCollectionKey = config.trkTracks_IDTrig()+"_Amb",
                                                  outTrackParticlesKey = config.tracks_IDTrig(),
                                                  trackParticleCreatorTool     =  creatorTool )
   
   ptAlgs.append( trackParticleCnvAlg )

   return ptAlgs
