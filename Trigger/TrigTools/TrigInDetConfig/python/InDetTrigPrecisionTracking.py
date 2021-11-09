#  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
#
#           Setup of precision tracking

from __future__ import print_function

from AthenaCommon.Logging import logging 
log = logging.getLogger("InDetTrigPrecisionTracking")




def makeInDetTrigPrecisionTracking( config=None, verifier=False, rois='EMViewRoIs', prefix="InDetTrigMT" ) :      
    
    log.info( "makeInDetTrigPrecisionTracking:: {} {} doTRT: {} ".format(  config.input_name, config.name, config.doTRT ) )
    
    ptAlgs = [] # List containing all the precision tracking algorithms hence every new added alg has to be appended to the list
    
    # Expects configuration  
    if config is None:
        raise ValueError('PrecisionTracking No configuration provided!')
        
    doTRT = config.doTRT

    # Add suffix to the algorithms
    signature =  "_{}".format( config.input_name )
    
    # Name settings for output Tracks/TrackParticles
    outTrkTracks        = config.trkTracks_IDTrig() # Final output Track collection
    outTrackParticles   = config.tracks_IDTrig() # Final output xAOD::TrackParticle collection
    ambiTrackCollection = config.trkTracks_IDTrig()+"_Amb"  # Ambiguity solver tracks
    
    #  Verifying input data for the algorithms
  
    # If run in views need to check data dependancies!
    # NOTE: this seems necessary only when PT is called from a different view than FTF otherwise causes stalls
    if verifier:
        from .InDetTrigCollectionKeys import TrigPixelKeys
        verifier.DataObjects += [( 'InDet::PixelGangedClusterAmbiguities' , 'StoreGateSvc+' + TrigPixelKeys.PixelClusterAmbiguitiesMap ),
                                 ( 'TrackCollection' , 'StoreGateSvc+' + config.trkTracks_FTF() )]

    
    from InDetTrigRecExample.InDetTrigConfigRecLoadTools import InDetTrigTrackSummaryTool
    summaryTool = InDetTrigTrackSummaryTool
    
    if config.newConfig:
        log.info( "ID Trigger: NEW precision tracking configuration {} {}".format(config.input_name, signature) )
        ambiSolvingAlgs = ambiguitySolver_builder( signature, config, summaryTool, outputTrackName=ambiTrackCollection, prefix=prefix+"Trk" )
    else:
        log.info( "ID Trigger: OLD precision tracking configuration {} {}".format(config.input_name, signature) )
        ambiSolvingAlgs = ambiguitySolverOld_builder( signature, config, summaryTool, outputTrackName=ambiTrackCollection, prefix=prefix )

    #Loading the alg to the sequence
    ptAlgs.extend( ambiSolvingAlgs )

    finalTrackCollection = ambiTrackCollection
    if doTRT:
        # do the TRT extension if requested
        finalTrackCollection = outTrkTracks
        trtAlgs = trtExtension_builder( signature, config, rois, summaryTool, inputTracks=ambiTrackCollection, outputTracks=outTrkTracks, prefix=prefix ) 
        ptAlgs.extend( trtAlgs )
  

    #
    #  Track particle conversion algorithm
    #
    from .InDetTrigCommon import trackParticleCnv_builder
    from InDetTrigRecExample.InDetTrigConfigRecLoadToolsPost import InDetTrigParticleCreatorToolWithSummary, \
        InDetTrigParticleCreatorToolWithSummaryTRTPid

    creatorTool = InDetTrigParticleCreatorToolWithSummary
    if config.electronPID:
      creatorTool = InDetTrigParticleCreatorToolWithSummaryTRTPid
    
    trackParticleCnvAlg = trackParticleCnv_builder( name                 = prefix+'xAODParticleCreatorAlg'+config.input_name+'_IDTrig',
                                                    config               = config,
                                                    inTrackCollectionKey = finalTrackCollection,
                                                    outTrackParticlesKey = outTrackParticles,
                                                    trackParticleCreatorTool     =  creatorTool)
    
    log.debug(trackParticleCnvAlg)
    ptAlgs.append(trackParticleCnvAlg)
    
    # ToolSvc.InDetTrigHoleSearchTool.SctSummaryTool.InDetTrigInDetSCT_FlaggedConditionTool.SCT_FlaggedCondData = "SCT_FlaggedCondData_TRIG"
    
    # Potentialy other algs with more collections? 
    # Might Drop the list in the end and keep just one output key
    nameTrackCollections =[ outTrkTracks ]
    nameTrackParticles =  [ outTrackParticles ]
    
    # Return list of Track keys, TrackParticle keys, and PT algs
    return  nameTrackCollections, nameTrackParticles, ptAlgs




# top level alg

def ambiguitySolver_builder( signature, config, summaryTool, outputTrackName=None, prefix="InDetTrigMT" ) :

    log.info( "Precision tracking using new configuration: {} {} {} {}".format(  signature, config.input_name, config.name, prefix ) )

    scoreMap        = 'ScoreMap'+config.input_name
    if config.usePixelNN:
        ambiguityScore  = ambiguityScoreNN_builder( signature, config, scoreMap, prefix )
    else:
        ambiguityScore  = ambiguityScore_builder( signature, config, scoreMap, prefix )
    ambiguitySolver = ambiguitySolverInternal_builder( signature, config, summaryTool, scoreMap, outputTrackName, prefix )

    return [ ambiguityScore, ambiguitySolver ]



# next level alg

def ambiguityScore_builder( signature, config, scoreMap, prefix=None ):
    
    from TrkAmbiguitySolver.TrkAmbiguitySolverConf import Trk__TrkAmbiguityScore
    ambiguityScore = Trk__TrkAmbiguityScore( name                    = '%sAmbiguityScore_%s'%(prefix, config.input_name),
                                             TrackInput              = [ config.trkTracks_FTF() ],
                                             TrackOutput             = scoreMap,
                                             AmbiguityScoreProcessor = None ) 
         
    log.info(ambiguityScore)
    
    return ambiguityScore 



def ambiguityScoreNN_builder( signature, config, scoreMap, prefix=None ):
    MultiplicityContent = [1 , 1 , 1]
    from AthenaCommon.CfgGetter import getPublicTool
    from InDetRecExample import TrackingCommon as TrackingCommon
    from InDetTrigRecExample.InDetTrigFlags import InDetTrigFlags
    from AthenaCommon.AppMgr import ToolSvc
    from InDetRecExample.TrackingCommon import createAndAddCondAlg,getPixelClusterNnCondAlg,getPixelClusterNnWithTrackCondAlg
    createAndAddCondAlg( getPixelClusterNnCondAlg,         'PixelClusterNnCondAlg',          GetInputsInfo = False)
    createAndAddCondAlg( getPixelClusterNnWithTrackCondAlg,'PixelClusterNnWithTrackCondAlg', GetInputsInfo = False)
    TrigPixelLorentzAngleTool = getPublicTool("PixelLorentzAngleTool")
    TrigNnClusterizationFactory = TrackingCommon.getNnClusterizationFactory( name                  = "%sTrigNnClusterizationFactory_%s"%(prefix, config.input_name),
                                                                      PixelLorentzAngleTool        = TrigPixelLorentzAngleTool,
                                                                      useToT                       = InDetTrigFlags.doNNToTCalibration(),
                                                                      NnCollectionReadKey          = 'PixelClusterNN',
                                                                      NnCollectionWithTrackReadKey = 'PixelClusterNNWithTrack',
                                                                      useTTrainedNetworks          = True)
    from SiClusterizationTool.SiClusterizationToolConf import InDet__NnPixelClusterSplitProbTool as PixelClusterSplitProbTool
    TrigNnPixelClusterSplitProbTool=PixelClusterSplitProbTool(name       = "%sTrigNnPixelClusterSplitProbTool_%s"%(prefix, config.input_name),
                                                                PriorMultiplicityContent = MultiplicityContent,
                                                                NnClusterizationFactory  = TrigNnClusterizationFactory,
                                                                useBeamSpotInfo          = True)
    ToolSvc += TrigNnPixelClusterSplitProbTool
    from TrkAmbiguityProcessor.TrkAmbiguityProcessorConf import Trk__DenseEnvironmentsAmbiguityScoreProcessorTool
    trackMapTool = TrackingCommon.getInDetTrigPRDtoTrackMapToolGangedPixels()
    scoringTool = scoringTool_builder( signature, config, summaryTool=ToolSvc.InDetTrigTrackSummaryTool, prefix=None )
    ambiguityScoreProcessor = Trk__DenseEnvironmentsAmbiguityScoreProcessorTool( name               = "%sInDetTrigMT_AmbiguityScoreProcessorTool_%s"%(prefix, config.input_name),
                                                                 ScoringTool        = scoringTool,
                                                                 AssociationTool    = trackMapTool,
                                                                 SplitProbTool      = TrigNnPixelClusterSplitProbTool
                                                                )
    from TrkAmbiguitySolver.TrkAmbiguitySolverConf import Trk__TrkAmbiguityScore
    ambiguityScore = Trk__TrkAmbiguityScore( name                    = '%sAmbiguityScore_%s'%(prefix, config.input_name),
                                             TrackInput              = [ config.trkTracks_FTF() ],
                                             TrackOutput             = scoreMap,
                                             AmbiguityScoreProcessor = ambiguityScoreProcessor )
    log.info(ambiguityScore)
    return ambiguityScore
# next level alg

def ambiguitySolverInternal_builder( signature, config, summaryTool, scoreMap, outputTrackName=None, prefix=None ):
    if config.usePixelNN: 
        ambiguityProcessorTool = ambiguityProcessorToolNN_builder( signature, config, summaryTool, prefix )
    else:
        ambiguityProcessorTool = ambiguityProcessorTool_builder( signature, config, summaryTool, prefix )
    
    from TrkAmbiguitySolver.TrkAmbiguitySolverConf import Trk__TrkAmbiguitySolver
    ambiguitySolver = Trk__TrkAmbiguitySolver( name               = '%sAmbiguitySolver_%s'%(prefix,config.input_name),
                                               TrackInput         = scoreMap,
                                               TrackOutput        = outputTrackName, 
                                               AmbiguityProcessor = ambiguityProcessorTool )
    
    
    return ambiguitySolver




def ambiguityProcessorTool_builder( signature, config, summaryTool ,prefix=None ) : 

    from InDetTrigRecExample.InDetTrigConfigRecLoadTools import InDetTrigTrackFitter
    from InDetTrigRecExample.InDetTrigConfigRecLoadTools import InDetTrigAmbiTrackSelectionTool

    from InDetRecExample import TrackingCommon as TrackingCommon
    trackMapTool = TrackingCommon.getInDetTrigPRDtoTrackMapToolGangedPixels()

    scoringTool = scoringTool_builder( signature, config, summaryTool, prefix )
    
    from TrkAmbiguityProcessor.TrkAmbiguityProcessorConf import Trk__SimpleAmbiguityProcessorTool 
    ambiguityProcessorTool = Trk__SimpleAmbiguityProcessorTool( name             = '%sAmbiguityProcessor_%s'%(prefix,config.input_name),
                                                                Fitter           = InDetTrigTrackFitter,
                                                                ScoringTool      = scoringTool,
                                                                AssociationTool  = trackMapTool,
                                                                TrackSummaryTool = summaryTool,
                                                                SelectionTool    = InDetTrigAmbiTrackSelectionTool )
    
    from AthenaCommon.AppMgr import ToolSvc
    ToolSvc += ambiguityProcessorTool
    
    return ambiguityProcessorTool

def ambiguityProcessorToolNN_builder( signature, config, summaryTool ,prefix=None ) : 

    from InDetTrigRecExample.InDetTrigConfigRecLoadTools import InDetTrigTrackFitter
    from InDetTrigRecExample.InDetTrigConfigRecLoadTools import InDetTrigAmbiTrackSelectionTool

    from InDetRecExample import TrackingCommon as TrackingCommon
    trackMapTool = TrackingCommon.getInDetTrigPRDtoTrackMapToolGangedPixels()

    scoringTool = scoringTool_builder( signature, config, summaryTool, prefix )
   
    InDetTrigAmbiTrackSelectionTool.doPixelSplitting=True 
    from TrkAmbiguityProcessor.TrkAmbiguityProcessorConf import Trk__DenseEnvironmentsAmbiguityProcessorTool as ProcessorTool
    ambiguityProcessorTool = ProcessorTool( name             = '%sDenseEnvironmentsAmbiguityProcessor_%s'%(prefix,config.input_name),
                                                                Fitter           = [InDetTrigTrackFitter],
                                                                ScoringTool      = scoringTool,
                                                                AssociationTool  = trackMapTool,
                                                                TrackSummaryTool = summaryTool,
                                                                SelectionTool    = InDetTrigAmbiTrackSelectionTool,
                                                                SuppressHoleSearch = False,
                                                                tryBremFit         = False,
                                                                caloSeededBrem     = False,
                                                                RefitPrds          = True,
                                                                pTminBrem          = 1000.0 )
    
    from AthenaCommon.AppMgr import ToolSvc
    ToolSvc += ambiguityProcessorTool
    
    return ambiguityProcessorTool



def scoringTool_builder( signature, config, summaryTool, prefix=None ):

  from InDetTrigRecExample.InDetTrigConfigRecLoadTools import InDetTrigExtrapolator
  
  from InDetTrigRecExample.InDetTrigConfigRecLoadTools import InDetTrigTRTDriftCircleCut

  # NB: This DriftCircleCutTool should not be used here, we want to use the AmbiScoringTool 
  #     without using the DriftCircleCutTool at all, but unfortunatly, the AmbiScoringTool
  #     needs a DriftCircleCut tool - either the one we pass in, or the default offline 
  #     tool. 
  #     In both of these cases, using the tool might be problematic, so we hope to 
  #     be able to disable it in the tool completely at some point in the near future 
  from InDetTrackScoringTools.InDetTrackScoringToolsConf import InDet__InDetAmbiScoringTool
  from InDetRecExample.TrackingCommon import setDefaults
  kwargs = {}

  kwargs = setDefaults(kwargs, name = '%sScoringTool_%s'%( prefix, config.input_name),
                                              Extrapolator = InDetTrigExtrapolator,
                                              minPt        = config.pTmin, 
                                              doEmCaloSeed = False,
                                              SummaryTool  = summaryTool,
                                              minTRTonTrk        = 0,
                                              DriftCircleCutTool = InDetTrigTRTDriftCircleCut)

  if(config.maxRPhiImpact is not None):
    kwargs = setDefaults(kwargs, maxRPhiImp = config.maxRPhiImpact)

  if(config.maxZImpact is not None):
    kwargs = setDefaults(kwargs, maxZImp = config.maxZImpact)
    
  if(config.maxEta is not None):
    kwargs = setDefaults(kwargs, maxEta = config.maxEta)

  if(config.minSiClusters is not None):
    kwargs = setDefaults(kwargs, minSiClusters = config.minSiClusters)

  if(config.maxSiHoles is not None):
    kwargs = setDefaults(kwargs, maxSiHoles = config.maxSiHoles)

  if(config.maxPixelHoles is not None):
    kwargs = setDefaults(kwargs, maxPixelHoles = config.maxPixelHoles)

  if(config.maxSCTHoles is not None):
    kwargs = setDefaults(kwargs, maxSCTHoles = config.maxSCTHoles)

  if(config.maxDoubleHoles is not None):
    kwargs = setDefaults(kwargs, maxDoubleHoles = config.maxDoubleHoles)

  if(config.usePixel is not None):
    kwargs = setDefaults(kwargs, usePixel = config.usePixel)

  if(config.useSCT is not None):
    kwargs = setDefaults(kwargs, useSCT = config.useSCT)

  if(config.doEmCaloSeed is not None):
    kwargs = setDefaults(kwargs, doEmCaloSeed = config.doEmCaloSeed)

  if(config.minTRTonTrk is not None):
    kwargs = setDefaults(kwargs, minTRTonTrk = config.minTRTonTrk)

  scoringTool =  InDet__InDetAmbiScoringTool(**kwargs)
                                                                                          
  log.info( scoringTool )

  from AthenaCommon.AppMgr import ToolSvc
  ToolSvc += scoringTool

  return scoringTool




def ambiguitySolverOld_builder( signature, config, summaryTool, outputTrackName=None, prefix="InDetTrigMT" ) :
    
    #-----------------------------------------------------------------------------
    #                        Ambiguity solving stage
    from .InDetTrigCommon import ambiguityScoreAlg_builder
    from .InDetTrigCommon import ambiguitySolverAlg_builder
    
    # Map of tracks and their scores
    scoreAlg = ambiguityScoreAlg_builder( name                  = prefix+'TrkAmbiguityScore_'+config.input_name,
                                          config                = config,
                                          inputTrackCollection  = config.trkTracks_FTF(),
                                          outputTrackScoreMap   = 'ScoreMap'+config.input_name ) 
    
  
    solverAlg = ambiguitySolverAlg_builder( name                  = prefix+'TrkAmbiguitySolver_'+config.input_name,
                                            config                = config,
                                            summaryTool           = summaryTool,
                                            inputTrackScoreMap    = 'ScoreMap'+config.input_name,
                                            outputTrackCollection = outputTrackName )
    
    return [ scoreAlg, solverAlg ] 




def trtExtension_builder( signature, config, rois, summaryTool, inputTracks, outputTracks, prefix="InDetTrigMT" ): 

    trtRIOMaker           = trtRIOMaker_builder( signature, config, rois, prefix  )
    trtExtensionAlg       = trtExtensionAlg_builder( signature, config, inputTracks, prefix )
    trtExtensionProcessor = trtExtensionProcessor_builder( signature, config, summaryTool, inputTracks, outputTracks, prefix )

    return [ trtRIOMaker, trtExtensionAlg, trtExtensionProcessor]
    




def trtRIOMaker_builder( signature, config, rois, prefix="InDetTrigMT" ): 
    
    log.info( "trtRIOMaker_builder: {} {}".format( signature, prefix ) )

    algs = []

    TRT_RDO_Key = "TRT_RDOs"

    from .InDetTrigCollectionKeys import TrigTRTKeys
    from AthenaCommon.GlobalFlags import globalflags
    
    # Only add raw data decoders if we're running over raw data
    if globalflags.InputFormat.is_bytestream():
        #Global keys/names for collections 
        TRT_RDO_Key = TrigTRTKeys.RDOs
        trtDataProvider = trtDataProvider_builder( signature, config, TRT_RDO_Key, rois )
        algs.append( trtDataProvider )
        
        
    #-----------------------------------------------------------------------------
    #                        TRT extension
    # Keep track that this needs to have a switch between DAF and XK
    # trkExtensionType = 'XK'
    # if InDetTrigFlags.trtExtensionType() is 'DAF' :
    
    from InDetTrigRecExample.InDetTrigConfigRecLoadTools import  InDetTrigTRT_DriftCircleTool
    
    from InDetPrepRawDataFormation.InDetPrepRawDataFormationConf import InDet__TRT_RIO_Maker
    trtRIOMaker = InDet__TRT_RIO_Maker( name = "%sTRTDriftCircleMaker%s"%(prefix, signature),
                                        TRTRIOLocation = TrigTRTKeys.DriftCircles,
                                        TRTRDOLocation = TRT_RDO_Key,
                                        # FIXME:
                                        # EtaHalfWidth = config.etaHalfWidth
                                        # PhiHalfWidth = config.phiHalfWidth
                                        # doFullScan   = config.doFullScan
                                        TRT_DriftCircleTool = InDetTrigTRT_DriftCircleTool )
    
    trtRIOMaker.isRoI_Seeded = True
    trtRIOMaker.RoIs = rois

    from RegionSelector.RegSelToolConfig import makeRegSelTool_TRT
    trtRIOMaker.RegSelTool = makeRegSelTool_TRT()

    algs.append( trtRIOMaker )

    return algs
  
 




def trtDataProvider_builder( signature, config, TRT_RDO_Key, rois, prefix="InDetTrigMT" ) :
  
    from AthenaCommon.AppMgr import ToolSvc

    from TRT_RawDataByteStreamCnv.TRT_RawDataByteStreamCnvConf import TRT_RodDecoder
    trtRodDecoder = TRT_RodDecoder( name="%sTRTRodDecoder%s"%(prefix, signature),
                                       LoadCompressTableDB=True )
    ToolSvc += trtRodDecoder
  
    from TRT_RawDataByteStreamCnv.TRT_RawDataByteStreamCnvConf import TRTRawDataProviderTool
    trtRawDataProviderTool = TRTRawDataProviderTool( name="%sTRTRawDataProviderTool%s"%(prefix, signature),
                                                     Decoder=trtRodDecoder )
    
    ToolSvc += trtRawDataProviderTool
    
    # load the TRTRawDataProvider
    from TRT_RawDataByteStreamCnv.TRT_RawDataByteStreamCnvConf import TRTRawDataProvider
    trtRawDataProvider = TRTRawDataProvider(name         = "%sTRTRawDataProvider%s"%(prefix, signature),
                                            RDOKey       = TRT_RDO_Key,
                                            ProviderTool = trtRawDataProviderTool)
    
    from RegionSelector.RegSelToolConfig import makeRegSelTool_TRT
    
    trtRawDataProvider.RegSelTool = makeRegSelTool_TRT()
    trtRawDataProvider.isRoI_Seeded = True
    trtRawDataProvider.RoIs = rois
    
    return  trtRawDataProvider 








def trtExtensionTool_builder( signature, config, prefix="InDetTrigMT" ): 

    from AthenaCommon.AppMgr import ToolSvc

    # Condition algorithm for InDet__TRT_DetElementsRoadMaker_xk
    from AthenaCommon.AlgSequence import AthSequencer
    condSeq = AthSequencer("AthCondSeq")
    
    trtRoadAlgName = "%sTRT_DetElementsRoadCondAlg_xk"%(prefix)
    
    if not hasattr( condSeq, trtRoadAlgName ):
        from TRT_DetElementsRoadTool_xk.TRT_DetElementsRoadTool_xkConf import InDet__TRT_DetElementsRoadCondAlg_xk
    
        # condSeq += InDet__TRT_DetElementsRoadCondAlg_xk(name = "%sTRT_DetElementsRoadCondAlg_xk%s"%(prefix, signature) )
        condSeq += InDet__TRT_DetElementsRoadCondAlg_xk( name = trtRoadAlgName )


    from InDetTrigRecExample.InDetTrigConfigRecLoadTools import InDetTrigPatternPropagator
    
    from TRT_DetElementsRoadTool_xk.TRT_DetElementsRoadTool_xkConf import InDet__TRT_DetElementsRoadMaker_xk
    
    trtRoadMaker =  InDet__TRT_DetElementsRoadMaker_xk( name   = '%sTRTRoadMaker%s'%(prefix, signature),
                                                        #DetectorStoreLocation = 'DetectorStore',
                                                        #TRTManagerLocation    = 'TRT',
                                                        MagneticFieldMode     = 'MapSolenoid',
                                                        PropagatorTool        = InDetTrigPatternPropagator )

    ToolSvc += trtRoadMaker



    # TODO implement new configuration of circle cut

    from InDetTrigRecExample.ConfiguredNewTrackingTrigCuts import EFIDTrackingCuts

    cutValues = EFIDTrackingCuts

    if(config.isLRT):
        from InDetTrigRecExample.ConfiguredNewTrackingTrigCuts import EFIDTrackingCutLRT
        cutValues = EFIDTrackingCutLRT
        
    from .InDetTrigCollectionKeys import TrigTRTKeys

    from InDetTrigRecExample.InDetTrigConfigRecLoadTools import InDetTrigPatternUpdator 
    from InDetTrigRecExample.InDetTrigConfigRecLoadTools import InDetTrigTRTDriftCircleCut
    
    from TRT_TrackExtensionTool_xk.TRT_TrackExtensionTool_xkConf import InDet__TRT_TrackExtensionTool_xk
    
    trtExtensionTool = InDet__TRT_TrackExtensionTool_xk ( name = "%sTrackExtensionTool%s"%(prefix,signature),
                                                          MagneticFieldMode     = 'MapSolenoid',      # default
                                                          TRT_ClustersContainer = TrigTRTKeys.DriftCircles, # default
                                                          TrtManagerLocation    = 'TRT',              # default
                                                          PropagatorTool = InDetTrigPatternPropagator,
                                                          UpdatorTool    = InDetTrigPatternUpdator,
                                                          # RIOonTrackToolYesDr = # default for now
                                                          # RIOonTrackToolNoDr  = # default for now
                                                          RoadTool            = trtRoadMaker,
                                                          DriftCircleCutTool = InDetTrigTRTDriftCircleCut,
                                                          MinNumberDriftCircles = cutValues.minTRTonTrk(),
                                                          ScaleHitUncertainty   = 2.,
                                                          RoadWidth             = 20.,
                                                          UseParameterization   = cutValues.useParameterizedTRTCuts() )
               
    ToolSvc += trtExtensionTool

    return trtExtensionTool


 

def trtExtensionAlg_builder( signature, config, inputTracks, prefix="InDetTrigMT" ): 
 
    extensionTool = trtExtensionTool_builder( signature, config ) 
 
    from TRT_TrackExtensionAlg.TRT_TrackExtensionAlgConf import InDet__TRT_TrackExtensionAlg
    extensionAlg = InDet__TRT_TrackExtensionAlg( name = "%sTrackExtensionAlg%s"%(prefix, signature),
                                                    InputTracksLocation    = inputTracks,
                                                    TrackExtensionTool     = extensionTool,
                                                    ExtendedTracksLocation = 'ExtendedTrackMap' )

    return extensionAlg




def trtExtensionProcessor_builder( signature, config, summaryTool, inputTracks, outputTracks, prefix="InDetTrigMT" ):   

    from AthenaCommon.AppMgr import ToolSvc

    #-----------------------------------------------------------------------------
    #                        TRT processor
    
    # TODO: do I need a new fitter for this? Or can I use the same one?
    # TODO In Run2 option for cosmic
    # InDetTrigExtensionFitter = InDetTrigTrackFitter
    from InDetTrigRecExample.ConfiguredNewTrackingTrigCuts import EFIDTrackingCuts

    cutValues = EFIDTrackingCuts

    if(config.isLRT):
        from InDetTrigRecExample.ConfiguredNewTrackingTrigCuts import EFIDTrackingCutLRT
        cutValues = EFIDTrackingCutLRT
    
    from InDetTrigRecExample.InDetTrigConfigRecLoadTools import  InDetTrigExtrapolator
    
    from InDetTrackScoringTools.InDetTrackScoringToolsConf import InDet__InDetAmbiScoringTool
    from InDetTrigRecExample.InDetTrigConfigRecLoadTools import InDetTrigTRTDriftCircleCut

    scoringTool = InDet__InDetAmbiScoringTool(name               = '%sExtScoringTool%s'%(prefix, signature),
                                              Extrapolator       = InDetTrigExtrapolator,
                                              SummaryTool        = summaryTool,
                                              useAmbigFcn        = True,     # this is NewTracking  
                                              maxRPhiImp         = cutValues.maxPrimaryImpact(),
                                              maxZImp            = cutValues.maxZImpact(),
                                              maxEta             = cutValues.maxEta(),
                                              minSiClusters      = cutValues.minClusters(),
                                              maxSiHoles         = cutValues.maxHoles(),
                                              maxDoubleHoles     = cutValues.maxDoubleHoles(),
                                              usePixel           = cutValues.usePixel(),
                                              useSCT             = cutValues.useSCT(),
                                              doEmCaloSeed       = False,
                                              minTRTonTrk        = cutValues.minTRTonTrk(),
                                              minTRTPrecisionFraction = cutValues.minTRTPrecFrac(),
                                              #useSigmaChi2   = False # tuning from Thijs
                                              DriftCircleCutTool = InDetTrigTRTDriftCircleCut,
                                              minPt              = config.pTmin )
    ToolSvc += scoringTool


    from InDetTrigRecExample.InDetTrigConfigRecLoadTools import InDetTrigTrackFitter
    from InDetTrigRecExample.InDetTrigFlags import InDetTrigFlags
    from InDetExtensionProcessor.InDetExtensionProcessorConf import InDet__InDetExtensionProcessor   
    trtExtensionProcessor = InDet__InDetExtensionProcessor (name               = "%sExtensionProcessor%s"%(prefix, signature),
                                                            TrackName          = inputTracks,
                                                            #Cosmics           = InDetFlags.doCosmics(),
                                                            ExtensionMap       = 'ExtendedTrackMap',
                                                            NewTrackName       = outputTracks,
                                                            TrackFitter        = InDetTrigTrackFitter,
                                                            TrackSummaryTool   = summaryTool,
                                                            ScoringTool        = scoringTool, 
                                                            suppressHoleSearch = False,
                                                            RefitPrds = not (InDetTrigFlags.refitROT() or (InDetTrigFlags.trtExtensionType() == 'DAF')))
                                                            # Check these option after DAF is implemented
                                                            # tryBremFit         = InDetFlags.doBremRecovery(),
                                                            # caloSeededBrem     = InDetFlags.doCaloSeededBrem(),
                                                            # pTminBrem          = NewTrackingCuts.minPTBrem() )
    return trtExtensionProcessor





