#
#  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
#

from AthenaCommon.Include import include

from AthenaCommon.Logging import logging
log = logging.getLogger("InDetTrigFastTracking")

from AthenaCommon.AthenaCommonFlags import athenaCommonFlags # noqa: F401

include("InDetTrigRecExample/InDetTrigRec_jobOptions.py")

def makeInDetTrigFastTrackingNoView( config = None, rois = 'EMViewRoIs', doFTF = True, secondStageConfig = None ):

  viewAlgs, viewVerify = makeInDetTrigFastTracking( config, rois, doFTF, None, secondStageConfig)
  return viewAlgs

def makeInDetTrigFastTracking( config = None, rois = 'EMViewRoIs', doFTF = True, viewVerifier='IDViewDataVerifier', secondStageConfig = None):

  if config is None :
    raise ValueError('makeInDetTrigFastTracking() No config provided!')
  #Add suffix to the algorithms
  signature =  '_{}'.format( config.input_name )

  log.info( "Fast tracking using new configuration: {} {}".format( config.input_name, config.name ) )

  #Global keys/names for Trigger collections
  from .InDetTrigCollectionKeys import  TrigPixelKeys, TrigSCTKeys
  from InDetRecExample.InDetKeys import InDetKeys
  from TrigInDetConfig.TrigInDetConfig import InDetCacheNames
  from AthenaCommon.GlobalFlags import globalflags

  viewAlgs = []

  ViewDataVerifier = None
  if viewVerifier:
    import AthenaCommon.CfgMgr as CfgMgr
    ViewDataVerifier = CfgMgr.AthViews__ViewDataVerifier( viewVerifier + signature )
    ViewDataVerifier.DataObjects = [( 'InDet::PixelClusterContainerCache' , InDetCacheNames.Pixel_ClusterKey ),
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
    if doFTF and config.name == 'fullScanUTT' :
      ViewDataVerifier.DataObjects += [ ( 'DataVector< LVL1::RecJetRoI >' , 'StoreGateSvc+HLT_RecJETRoIs' ) ]



    viewAlgs.append( ViewDataVerifier )

    # Load RDOs if we aren't loading bytestream
    from AthenaCommon.AlgSequence import AlgSequence
    topSequence = AlgSequence()

    topSequence.SGInputLoader.Load += [ ( 'TagInfo' , 'DetectorStore+ProcessingTags' ) ]



    if not globalflags.InputFormat.is_bytestream():
      ViewDataVerifier.DataObjects +=   [( 'PixelRDO_Container' , InDetKeys.PixelRDOs() ),
                                         ( 'SCT_RDO_Container' , InDetKeys.SCT_RDOs() )]
      topSequence.SGInputLoader.Load += [( 'PixelRDO_Container' , InDetKeys.PixelRDOs() ),
                                         ( 'SCT_RDO_Container' , InDetKeys.SCT_RDOs() )]

  from InDetTrigRecExample.InDetTrigFlags import InDetTrigFlags
  from AthenaCommon.AppMgr import ToolSvc

  #Only add raw data decoders if we're running over raw data
  if globalflags.InputFormat.is_bytestream():
    #Pixel

    from PixelRawDataByteStreamCnv.PixelRawDataByteStreamCnvConf import PixelRodDecoder
    InDetPixelRodDecoder = PixelRodDecoder(name = "InDetPixelRodDecoder_" + signature)
    # Disable duplcated pixel check for data15 because duplication mechanism was used.
    from RecExConfig.RecFlags import rec
    if len(rec.projectName())>=6 and rec.projectName()[:6]=="data15":
       InDetPixelRodDecoder.CheckDuplicatedPixel=False
    ToolSvc += InDetPixelRodDecoder

    from PixelRawDataByteStreamCnv.PixelRawDataByteStreamCnvConf import PixelRawDataProviderTool
    InDetPixelRawDataProviderTool = PixelRawDataProviderTool(name    = "InDetPixelRawDataProviderTool_" + signature,
                                                             Decoder = InDetPixelRodDecoder)
    ToolSvc += InDetPixelRawDataProviderTool

    if (InDetTrigFlags.doPrintConfigurables()):
      print(InDetPixelRawDataProviderTool) # noqa: ATL901

    # load the PixelRawDataProvider
    from PixelRawDataByteStreamCnv.PixelRawDataByteStreamCnvConf import PixelRawDataProvider
    InDetPixelRawDataProvider = PixelRawDataProvider(name         = "InDetPixelRawDataProvider_"+ signature,
                                                     RDOKey       = InDetKeys.PixelRDOs(),
                                                     ProviderTool = InDetPixelRawDataProviderTool,)
    InDetPixelRawDataProvider.isRoI_Seeded = True
    InDetPixelRawDataProvider.RoIs = rois
    InDetPixelRawDataProvider.RDOCacheKey = InDetCacheNames.PixRDOCacheKey
    InDetPixelRawDataProvider.BSErrorsCacheKey = InDetCacheNames.PixBSErrCacheKey

    from RegionSelector.RegSelToolConfig import makeRegSelTool_Pixel

    InDetPixelRawDataProvider.RegSelTool = makeRegSelTool_Pixel()

    viewAlgs.append(InDetPixelRawDataProvider)


    if (InDetTrigFlags.doPrintConfigurables()):
      print(InDetPixelRawDataProvider) # noqa: ATL901


    #SCT
    from SCT_RawDataByteStreamCnv.SCT_RawDataByteStreamCnvConf import SCT_RodDecoder
    InDetSCTRodDecoder = SCT_RodDecoder(name          = "InDetSCTRodDecoder_" + signature)
    ToolSvc += InDetSCTRodDecoder

    from SCT_RawDataByteStreamCnv.SCT_RawDataByteStreamCnvConf import SCTRawDataProviderTool
    InDetSCTRawDataProviderTool = SCTRawDataProviderTool(name    = "InDetSCTRawDataProviderTool_" + signature,
                                                         Decoder = InDetSCTRodDecoder)
    ToolSvc += InDetSCTRawDataProviderTool
    if (InDetTrigFlags.doPrintConfigurables()):
      print(InDetSCTRawDataProviderTool) # noqa: ATL901


    # load the SCTRawDataProvider
    from SCT_RawDataByteStreamCnv.SCT_RawDataByteStreamCnvConf import SCTRawDataProvider
    InDetSCTRawDataProvider = SCTRawDataProvider(name         = "InDetSCTRawDataProvider_" + signature,
                                                 RDOKey       = InDetKeys.SCT_RDOs(),
                                                 ProviderTool = InDetSCTRawDataProviderTool)
    InDetSCTRawDataProvider.isRoI_Seeded = True
    InDetSCTRawDataProvider.RoIs = rois
    InDetSCTRawDataProvider.RDOCacheKey = InDetCacheNames.SCTRDOCacheKey
    InDetSCTRawDataProvider.BSErrCacheKey = InDetCacheNames.SCTBSErrCacheKey

    from RegionSelector.RegSelToolConfig import makeRegSelTool_SCT
    InDetSCTRawDataProvider.RegSelTool = makeRegSelTool_SCT()

    viewAlgs.append(InDetSCTRawDataProvider)

    # load the SCTEventFlagWriter
    from SCT_RawDataByteStreamCnv.SCT_RawDataByteStreamCnvConf import SCTEventFlagWriter
    InDetSCTEventFlagWriter = SCTEventFlagWriter(name = "InDetSCTEventFlagWriter_"+ signature)

    viewAlgs.append(InDetSCTEventFlagWriter)

  #Pixel clusterisation
  from InDetTrigRecExample.InDetTrigConfigRecLoadTools import TrigPixelLorentzAngleTool, TrigSCTLorentzAngleTool

  from SiClusterizationTool.SiClusterizationToolConf import InDet__ClusterMakerTool
  InDetClusterMakerTool = InDet__ClusterMakerTool(name                 = "InDetClusterMakerTool_" + signature,
                                                  SCTLorentzAngleTool = TrigSCTLorentzAngleTool,
                                                  PixelLorentzAngleTool = TrigPixelLorentzAngleTool)

  ToolSvc += InDetClusterMakerTool

  from PixelConditionsTools.PixelConditionsToolsConf import PixelConditionsSummaryTool
  idPixelSummary = PixelConditionsSummaryTool("PixelConditionsSummaryTool",
                                              UseByteStreamFEI4=globalflags.InputFormat.is_bytestream(),
                                              UseByteStreamFEI3=globalflags.InputFormat.is_bytestream())

  from SiClusterizationTool.SiClusterizationToolConf import InDet__MergedPixelsTool
  InDetMergedPixelsTool = InDet__MergedPixelsTool(name                       = "InDetMergedPixelsTool_" + signature,
                                                  PixelConditionsSummaryTool = idPixelSummary,
                                                  globalPosAlg               = InDetClusterMakerTool)

  # Enable duplcated RDO check for data15 because duplication mechanism was used.
  from RecExConfig.RecFlags import rec
  if len(rec.projectName())>=6 and rec.projectName()[:6]=="data15":
     InDetMergedPixelsTool.CheckDuplicatedRDO = True
  ToolSvc += InDetMergedPixelsTool

  from SiClusterizationTool.SiClusterizationToolConf import InDet__PixelGangedAmbiguitiesFinder
  InDetPixelGangedAmbiguitiesFinder = InDet__PixelGangedAmbiguitiesFinder(name = "InDetPixelGangedAmbiguitiesFinder_" + signature)
  ToolSvc += InDetPixelGangedAmbiguitiesFinder

  from InDetPrepRawDataFormation.InDetPrepRawDataFormationConf import InDet__PixelClusterization
  InDetPixelClusterization = InDet__PixelClusterization(name                    = "InDetPixelClusterization_" + signature,
                                                        clusteringTool          = InDetMergedPixelsTool,
                                                        gangedAmbiguitiesFinder = InDetPixelGangedAmbiguitiesFinder,
                                                        DataObjectName          = InDetKeys.PixelRDOs(),
                                                        AmbiguitiesMap          = TrigPixelKeys.PixelClusterAmbiguitiesMap,
                                                        ClustersName            = TrigPixelKeys.Clusters)

  InDetPixelClusterization.isRoI_Seeded = True
  InDetPixelClusterization.RoIs = rois
  InDetPixelClusterization.ClusterContainerCacheKey = InDetCacheNames.Pixel_ClusterKey

  from RegionSelector.RegSelToolConfig import makeRegSelTool_Pixel
  InDetPixelClusterization.RegSelTool = makeRegSelTool_Pixel()

  from InDetPrepRawDataFormation.MonitoringTool import PixelClusterization_MonitoringTool
  InDetPixelClusterization.MonTool = PixelClusterization_MonitoringTool()

  viewAlgs.append(InDetPixelClusterization)

  # Create SCT_ConditionsSummaryTool
  from SCT_ConditionsTools.SCT_ConditionsSummaryToolSetup import SCT_ConditionsSummaryToolSetup
  sct_ConditionsSummaryToolSetup = SCT_ConditionsSummaryToolSetup("InDetSCT_ConditionsSummaryTool_" + signature)
  sct_ConditionsSummaryToolSetup.setup()
  InDetSCT_ConditionsSummaryTool = sct_ConditionsSummaryToolSetup.getTool() # noqa: F841
  sct_ConditionsSummaryToolSetupWithoutFlagged = SCT_ConditionsSummaryToolSetup("InDetSCT_ConditionsSummaryToolWithoutFlagged_" + signature)
  sct_ConditionsSummaryToolSetupWithoutFlagged.setup()
  InDetSCT_ConditionsSummaryToolWithoutFlagged = sct_ConditionsSummaryToolSetupWithoutFlagged.getTool()

  # Add conditions tools to SCT_ConditionsSummaryTool
  from SCT_ConditionsTools.SCT_ConfigurationConditionsToolSetup import SCT_ConfigurationConditionsToolSetup
  sct_ConfigurationConditionsToolSetup = SCT_ConfigurationConditionsToolSetup()
  sct_ConfigurationConditionsToolSetup.setToolName("InDetSCT_ConfigurationConditionsTool_" + signature)
  from IOVDbSvc.CondDB import conddb
  if (globalflags.DataSource() == "geant4") or (conddb.dbdata == "CONDBR2"):
     sct_ConfigurationConditionsToolSetup.setChannelFolder("/SCT/DAQ/Config/ChipSlim") # For MC (OFLP200) or Run 2, 3 data (CONDBR2)
  sct_ConfigurationConditionsToolSetup.setup()
  InDetSCT_ConditionsSummaryToolWithoutFlagged.ConditionsTools.append(sct_ConfigurationConditionsToolSetup.getTool().getFullName())

  from SCT_ConditionsTools.SCT_ReadCalibDataToolSetup import SCT_ReadCalibDataToolSetup
  sct_ReadCalibDataToolSetup = SCT_ReadCalibDataToolSetup()
  sct_ReadCalibDataToolSetup.setToolName("InDetSCT_ReadCalibDataTool_" + signature)
  sct_ReadCalibDataToolSetup.setup()
  InDetSCT_ConditionsSummaryToolWithoutFlagged.ConditionsTools.append(sct_ReadCalibDataToolSetup.getTool().getFullName())

  if (globalflags.InputFormat.is_bytestream()):
     from SCT_ConditionsTools.SCT_ByteStreamErrorsToolSetup import SCT_ByteStreamErrorsToolSetup
     sct_ByteStreamErrorsToolSetup = SCT_ByteStreamErrorsToolSetup()
     sct_ByteStreamErrorsToolSetup.setToolName("InDetSCT_BSErrorTool_" + signature)
     sct_ByteStreamErrorsToolSetup.setConfigTool(sct_ConfigurationConditionsToolSetup.getTool())
     sct_ByteStreamErrorsToolSetup.setup()
     InDetSCT_ConditionsSummaryToolWithoutFlagged.ConditionsTools.append(sct_ByteStreamErrorsToolSetup.getTool().getFullName())

  if (InDetTrigFlags.doPrintConfigurables()):
     print (InDetSCT_ConditionsSummaryToolWithoutFlagged)  # noqa: ATL901

  #
  # --- SCT_ClusteringTool
  #
  from SiClusterizationTool.SiClusterizationToolConf import InDet__SCT_ClusteringTool
  InDetSCT_ClusteringTool = InDet__SCT_ClusteringTool(name              = "InDetSCT_ClusteringTool_" + signature,
                                                      globalPosAlg      = InDetClusterMakerTool,
                                                      conditionsTool    = InDetSCT_ConditionsSummaryToolWithoutFlagged)
  if InDetTrigFlags.doSCTIntimeHits():
     if InDetTrigFlags.InDet25nsec():
        InDetSCT_ClusteringTool.timeBins = "01X"
     else:
        InDetSCT_ClusteringTool.timeBins = "X1X"

  #
  # --- SCT_Clusterization algorithm
  #

  from InDetPrepRawDataFormation.InDetPrepRawDataFormationConf import InDet__SCT_Clusterization
  InDetSCT_Clusterization = InDet__SCT_Clusterization(name                    = "InDetSCT_Clusterization_" + signature,
                                                      clusteringTool          = InDetSCT_ClusteringTool,
                                                      # ChannelStatus         = InDetSCT_ChannelStatusAlg,
                                                      DataObjectName          = InDetKeys.SCT_RDOs(),
                                                      ClustersName            = TrigSCTKeys.Clusters,
                                                      #Adding the suffix to flagged conditions
                                                      SCT_FlaggedCondData     = "SCT_FlaggedCondData_TRIG",
                                                      conditionsTool          = InDetSCT_ConditionsSummaryToolWithoutFlagged)
  InDetSCT_Clusterization.isRoI_Seeded = True
  InDetSCT_Clusterization.RoIs = rois
  InDetSCT_Clusterization.ClusterContainerCacheKey = InDetCacheNames.SCT_ClusterKey
  InDetSCT_Clusterization.FlaggedCondCacheKey = InDetCacheNames.SCTFlaggedCondCacheKey

  from RegionSelector.RegSelToolConfig import makeRegSelTool_SCT
  InDetSCT_Clusterization.RegSelTool = makeRegSelTool_SCT()
  
  from InDetPrepRawDataFormation.MonitoringTool import SCT_Clusterization_MonitoringTool
  InDetSCT_Clusterization.MonTool = SCT_Clusterization_MonitoringTool()
  

  viewAlgs.append(InDetSCT_Clusterization)


  #Space points and FTF

  from SiSpacePointTool.SiSpacePointToolConf import InDet__SiSpacePointMakerTool
  InDetSiSpacePointMakerTool = InDet__SiSpacePointMakerTool(name = "InDetSiSpacePointMakerTool_" + signature)

  from AthenaCommon.DetFlags import DetFlags
  from SiSpacePointFormation.SiSpacePointFormationConf import InDet__SiTrackerSpacePointFinder
  from SiSpacePointFormation.InDetOnlineMonitor import InDetMonitoringTool
  InDetSiTrackerSpacePointFinder = InDet__SiTrackerSpacePointFinder(name                   = "InDetSiTrackerSpacePointFinder_" + signature,
                                                                    SiSpacePointMakerTool  = InDetSiSpacePointMakerTool,
                                                                    PixelsClustersName     = TrigPixelKeys.Clusters,
                                                                    SpacePointsPixelName   = TrigPixelKeys.SpacePoints,
                                                                    SCT_ClustersName	    = TrigSCTKeys.Clusters,
                                                                    SpacePointsSCTName     = TrigSCTKeys.SpacePoints,
                                                                    SpacePointsOverlapName = InDetKeys.OverlapSpacePoints(),
                                                                    ProcessPixels          = DetFlags.haveRIO.pixel_on(),
                                                                    ProcessSCTs            = DetFlags.haveRIO.SCT_on(),
                                                                    ProcessOverlaps        = DetFlags.haveRIO.SCT_on(),
                                                                    SpacePointCacheSCT     = InDetCacheNames.SpacePointCacheSCT,
                                                                    SpacePointCachePix     = InDetCacheNames.SpacePointCachePix,
                                                                    monTool                = InDetMonitoringTool())

  viewAlgs.append(InDetSiTrackerSpacePointFinder)

  # Condition algorithm for SiTrackerSpacePointFinder
  if InDetSiTrackerSpacePointFinder.ProcessSCTs:
    from AthenaCommon.AlgSequence import AthSequencer
    condSeq = AthSequencer("AthCondSeq")
    if not hasattr(condSeq, "InDetSiElementPropertiesTableCondAlg"):
      # Setup alignment folders and conditions algorithms
      from SiSpacePointFormation.SiSpacePointFormationConf import InDet__SiElementPropertiesTableCondAlg
      condSeq += InDet__SiElementPropertiesTableCondAlg(name = "InDetSiElementPropertiesTableCondAlg")

  #FIXME have a flag for now set for True( as most cases call FTF) but potentially separate
  #do not add if the config is LRT
  if doFTF:

      if config is None:
            raise ValueError('makeInDetTrigFastTracking() No signature config specified')

      if config.useSiSPSeededTrackFinder and "LRT" in config.name:
        # use SiSPSeededTrackFinder for fast tracking
        from InDetRecExample.ConfiguredNewTrackingCuts import ConfiguredNewTrackingCuts
        trackingCuts = ConfiguredNewTrackingCuts( "R3LargeD0" )
        from InDetTrigRecExample.InDetTrigConfigRecLoadTools import InDetTrigTrackSummaryTool
        summaryTool = InDetTrigTrackSummaryTool
        # --- Loading Pixel, SCT conditions
        from AthenaCommon.AlgSequence import AthSequencer
        condSeq = AthSequencer("AthCondSeq")
        if not hasattr(condSeq, "InDetSiDetElementBoundaryLinksPixelCondAlg"):
          from SiCombinatorialTrackFinderTool_xk.SiCombinatorialTrackFinderTool_xkConf import InDet__SiDetElementBoundaryLinksCondAlg_xk
          condSeq += InDet__SiDetElementBoundaryLinksCondAlg_xk(name     = "InDetSiDetElementBoundaryLinksPixelCondAlg",
                                                                ReadKey  = "PixelDetectorElementCollection",
                                                                WriteKey = "PixelDetElementBoundaryLinks_xk",)

        if not hasattr(condSeq, "InDet__SiDetElementsRoadCondAlg_xk"):
          from SiDetElementsRoadTool_xk.SiDetElementsRoadTool_xkConf import InDet__SiDetElementsRoadCondAlg_xk
          condSeq += InDet__SiDetElementsRoadCondAlg_xk(name = "InDet__SiDetElementsRoadCondAlg_xk")

        if not hasattr(condSeq, "InDetSiDetElementBoundaryLinksSCTCondAlg"):
          from SiCombinatorialTrackFinderTool_xk.SiCombinatorialTrackFinderTool_xkConf import InDet__SiDetElementBoundaryLinksCondAlg_xk
          condSeq += InDet__SiDetElementBoundaryLinksCondAlg_xk(name     = "InDetSiDetElementBoundaryLinksSCTCondAlg",
                                                                ReadKey  = "SCT_DetectorElementCollection",
                                                                WriteKey = "SCT_DetElementBoundaryLinks_xk")

        from .InDetTrigCommon import siSPSeededTrackFinder_builder, add_prefix
        siSPSeededTrackFinder = siSPSeededTrackFinder_builder( name                  = add_prefix( 'siSPSeededTrackFinder', config.input_name ),
                                                               config                = config,
                                                               outputTracks          = config.trkTracks_FTF(), 
                                                               trackingCuts          = trackingCuts,
                                                               usePrdAssociationTool = False,
                                                               nameSuffix            = config.input_name,
                                                               trackSummaryTool      = summaryTool )


        viewAlgs.append( siSPSeededTrackFinder )

      else:

        from TrigFastTrackFinder.TrigFastTrackFinder_Config import TrigFastTrackFinderBase
        #TODO: eventually adapt IDTrigConfig also in FTF configuration (pass as additional param)
        theFTF = TrigFastTrackFinderBase("TrigFastTrackFinder_" + signature, config.input_name,
                                        conditionsTool = InDetSCT_ConditionsSummaryToolWithoutFlagged )
        theFTF.RoIs           = rois

        viewAlgs.append(theFTF)

      if not config.doZFinderOnly: 

        from InDetTrigRecExample.InDetTrigConfigRecLoadToolsPost import  InDetTrigParticleCreatorToolFTF
        from InDetTrigParticleCreation.InDetTrigParticleCreationConf import InDet__TrigTrackingxAODCnvMT

        theTrackParticleCreatorAlg = InDet__TrigTrackingxAODCnvMT(name = "InDetTrigTrackParticleCreatorAlg" + signature,
                                                                  TrackName = config.trkTracks_FTF(),
                                                                  ParticleCreatorTool = InDetTrigParticleCreatorToolFTF)

        #In general all FTF trackParticle collections are recordable except beamspot to save space
        theTrackParticleCreatorAlg.TrackParticlesName = config.tracks_FTF()

        viewAlgs.append(theTrackParticleCreatorAlg)

        if secondStageConfig is not None:
          #have been supplied with a second stage config, create another instance of FTF
          theFTF2 = TrigFastTrackFinderBase("TrigFastTrackFinder_" + secondStageConfig.input_name, secondStageConfig.input_name,
                                            conditionsTool = InDetSCT_ConditionsSummaryToolWithoutFlagged )
          theFTF2.RoIs           = rois
          theFTF2.inputTracksName = config.trkTracks_FTF()
        
          
          viewAlgs.append(theFTF2)

          
          from InDetTrigRecExample.InDetTrigConfigRecLoadToolsPost import  InDetTrigParticleCreatorToolFTF
          from InDetTrigParticleCreation.InDetTrigParticleCreationConf import InDet__TrigTrackingxAODCnvMT
          
          
          
          theTrackParticleCreatorAlg2 = InDet__TrigTrackingxAODCnvMT(name = "InDetTrigTrackParticleCreatorAlg_" + secondStageConfig.input_name,
                                                                  TrackName = secondStageConfig.trkTracks_FTF(),
                                                                     ParticleCreatorTool = InDetTrigParticleCreatorToolFTF)
          
          
          #In general all FTF trackParticle collections are recordable except beamspot to save space
          theTrackParticleCreatorAlg2.TrackParticlesName = secondStageConfig.tracks_FTF()
          
          viewAlgs.append(theTrackParticleCreatorAlg2)


      if (InDetTrigFlags.doTruth()):   

        from InDetTruthAlgs.InDetTruthAlgsConf import InDet__PRD_MultiTruthMaker
        InDetTrigPRD_MultiTruthMakerSi = InDet__PRD_MultiTruthMaker (name                    = 'InDetTrigPRD_MultiTruthMakerSi',
                                                                 PixelClusterContainerName   = 'PixelTrigClusters',
                                                                 SCTClusterContainerName     = 'SCT_TrigClusters',
                                                                 TRTDriftCircleContainerName = '',
                                                                 SimDataMapNamePixel         = 'PixelSDO_Map',
                                                                 SimDataMapNameSCT           = 'SCT_SDO_Map',
                                                                 SimDataMapNameTRT           = '',
                                                                 TruthNamePixel              = 'PRD_MultiTruthPixel',
                                                                 TruthNameSCT                = 'PRD_MultiTruthSCT',
                                                                 TruthNameTRT                = '')
        
        viewAlgs.append(InDetTrigPRD_MultiTruthMakerSi)
        MyTrackCollections = ["HLT_IDTrkTrack_FS_FTF"]
        import AthenaCommon.SystemOfUnits as Units
        from InDetTrackClusterAssValidation.InDetTrackClusterAssValidationConf import InDet__TrackClusterAssValidation
        InDetTrigTrackClusterAssValidation = InDet__TrackClusterAssValidation(name              = "InDetTrigTrackClusterAssValidation",
                                                                          TracksLocation         = MyTrackCollections             ,
                                                                          SpacePointsPixelName   = "PixelTrigSpacePoints"    ,
                                                                          SpacePointsSCTName     = "SCT_TrigSpacePoints"    ,
                                                                          SpacePointsOverlapName = "OverlapSpacePoints",
                                                                          PixelClustesContainer  = 'PixelTrigClusters',
                                                                          SCT_ClustesContainer   = 'SCT_TrigClusters',
                                                                          MomentumCut            = 1.5 * Units.GeV,
                                                                          RapidityCut            = 2.7     ,
                                                                          RadiusMin              = 0.0     ,
                                                                          RadiusMax              = 20.0 * Units.mm    ,
                                                                          MinNumberClusters      = 7       ,
                                                                          MinNumberClustersTRT   = 0       ,
                                                                          MinNumberSpacePoints   = 3       ,
                                                                          usePixel               = True     ,
                                                                          useSCT                 = True     ,
                                                                          useTRT                 = False     )
        viewAlgs.append(InDetTrigTrackClusterAssValidation)
 
  return viewAlgs, ViewDataVerifier
