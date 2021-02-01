#  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
#
#     Contains algs/tools used by Inner Detector Trigger

#Global keys/names for collections
#from .InDetTrigCollectionKeys import TrigTRTKeys, TrigPixelKeys

from InDetRecExample.TrackingCommon import makePublicTool, setDefaults
from InDetTrigRecExample.InDetTrigFlags import InDetTrigFlags



#-------------------------------
#TODO:

#Make loader for extrapolator

#-------------------------------

def get_name_prefix():
   #Too long? Do we need this to separate from InDet?
   return 'InDetTrigMT_'

def get_name_suffix(signature):
   return '_%s'%signature

#Retrieve full name of the algorithm/tool which consist of
#1] Predefined PREFIX describing the scope where this tool is being used (Inner Detector Trigger)
#2] CORE name containing the actual name of the tool
#3] SUFFIX which is derived from signature name: _electron, _muonLate, _FS etc
def get_full_name( core, suffix ):
   return  '{}{}{}'.format(  get_name_prefix(), core, get_name_suffix(suffix) )


#Retrieve name of the score map
#Map of Tracks and floats (representing score of a given track)
def get_scoremap_name( suffix ):
   return "ScoredMap{}".format(suffix)


def trackSummaryTool_getter( doTRT ):
   from InDetTrigRecExample.InDetTrigConfigRecLoadTools import InDetTrigTrackSummaryTool
   from TrigInDetConf.TrigInDetRecCommonTools           import InDetTrigTrackSummaryToolSharedHitsWithTRTPid

   #Load shared hits with Ele PID if TRT specified
   if doTRT:
      return InDetTrigTrackSummaryToolSharedHitsWithTRTPid
   else:
      return InDetTrigTrackSummaryTool

def trigPropagator_getter():
   from InDetTrigRecExample.InDetTrigConfigRecLoadTools import InDetTrigPropagator
   return InDetTrigPropagator

#--------------------------------------------------------------------------------------


@makePublicTool
def trackParticleCreatorTool_builder(name, config):
  """Tool with functionality to convert Trk:Tracks into xAOD::TrackParticles"""
  from TrkParticleCreator.TrkParticleCreatorConf import Trk__TrackParticleCreatorTool
  return Trk__TrackParticleCreatorTool(name             = name,
                                       KeepParameters   = config.PT.setting.keepTrackParameters,
                                       TrackSummaryTool = trackSummaryTool_getter( config.PT.setting.doTRT ) )


@makePublicTool
def trackCollectionCnvTool_builder(name, trackParticleCreatorTool, config):
  """A wrapper tool around trackParticleCreatorTool that enables to run on the whole TrackCollections"""
  from xAODTrackingCnv.xAODTrackingCnvConf import xAODMaker__TrackCollectionCnvTool
  return xAODMaker__TrackCollectionCnvTool( name                 = name,
                                            TrackParticleCreator = trackParticleCreatorTool )



def trackMonitoringTool_builder(suffix):
  #First load the generic monitoring tool with set of histograms for Particle Cnv
  from TrigInDetMonitoringTools.TrigInDetTrackingMonitoring import  TrigInDetTrackCnvMonitoring
  genericMonTool = TrigInDetTrackCnvMonitoring( name = 'GenericMonitoring_{}'.format(suffix))



  #Now pass this tool to the Track Monitoring tool
  from TrigInDetMonitoringTools.TrigInDetMonitoringToolsConf import TrigInDetTrackMonitoringTool
  return TrigInDetTrackMonitoringTool( name           = 'xAODParticleCreatorAlg_{}'.format(suffix),
                                       MonitoringTool = genericMonTool)



#Returns suffix of tracking type from a given alg name
def getTrackingSuffix( name ):
   if 'IDTrig' in name:
         return '_IDTrig'
   elif 'FTF' in name:
         return '_FTF'
   elif 'EFID' in name:
         return '_EFID'
   else:
      return ''


def trackParticleCnv_builder(name, config, inTrackCollectionKey, outTrackParticlesKey ):
  """Alg that stages conversion of Trk::TrackCollection into xAOD::TrackParticle container"""

  trackParticleCreatorTool =  trackParticleCreatorTool_builder( name   = get_full_name( 'TrackParticleCreatorTool',config.name),
                                                                config = config )

  trackCollectionCnvTool   =  trackCollectionCnvTool_builder( name                     = get_full_name( 'xAODTrackCollectionCnvTool',config.name),
                                                              trackParticleCreatorTool = trackParticleCreatorTool,
                                                              config                   = config )

  from xAODTrackingCnv.xAODTrackingCnvConf import xAODMaker__TrackParticleCnvAlg
  return  xAODMaker__TrackParticleCnvAlg(  name                                      = name,
                                           # Properties below are used for:  TrackCollection -> xAOD::TrackParticle
                                           ConvertTracks                             = True,  #Turn on  retrieve of TrackCollection, false by default
                                           TrackContainerName                        = inTrackCollectionKey,
                                           xAODTrackParticlesFromTracksContainerName = outTrackParticlesKey,
                                           TrackCollectionCnvTool                    = trackCollectionCnvTool,
                                           TrackParticleCreator                      = trackParticleCreatorTool,
                                           #Add track monitoring
                                           DoMonitoring                              = True,
                                           TrackMonTool                              = trackMonitoringTool_builder( config.name + getTrackingSuffix(name) ),
                                           # Properties below are used for obsolete: Rec:TrackParticle, aod -> xAOD::TrackParticle (Turn off)
                                           ConvertTrackParticles = False,  # Retrieve of Rec:TrackParticle, don't need this atm
                                           xAODContainerName                         = '',
                                           #---------------------------------------------------------------------------------
                                         )


#--------------------------------------------------------------------------------------
## Scoring tools
@makePublicTool
def ambiguityScoringTool_builder(name, config):

    #NOTE extra scaling for MB on top of standard cuts (taken from Run2) -> Can we not just put it in the setting with 0.95 factor?
    #from InDetTrigRecExample.InDetTrigSliceSettings import InDetTrigSliceSettings
    #ptintcut = InDetTrigSliceSettings[('pTmin',signature)]
    #if signature=='minBias':
    #  ptintcut = 0.95*InDetTrigSliceSettings[('pTmin',signature)]
    #elif signature=='minBias400':
    #  ptintcut = 0.95*InDetTrigSliceSettings[('pTmin',signature)]

    #NOTE retrieve new extrapolator? Use offline version?
    from InDetTrigRecExample.InDetTrigConfigRecLoadTools import InDetTrigExtrapolator
    from InDetTrigRecExample.InDetTrigConfigRecLoadTools import InDetTrigTRTDriftCircleCut

    #NOTE: Run2 config seems to be using offline version of
    #https://gitlab.cern.ch/atlas/athena/-/blob/master/InnerDetector/InDetExample/InDetRecExample/python/ConfiguredNewTrackingCuts.py
    #1] Is this really what we want here?
    #2] TODO: Somehow merge/adapt the settings in InDetTrigConfigSettings.py?

    if config.PT.isSignature('cosmicsN'):
        #Can potentially recreate the isntance of the tool here and in the if config just have a list of parameters needed to be changed for the tool
        from InDetTrigRecExample.InDetTrigConfigRecLoadToolsCosmics import InDetTrigScoringToolCosmics_SiPattern
        return InDetTrigScoringToolCosmics_SiPattern
    else:

        from InDetTrigRecExample.ConfiguredNewTrackingTrigCuts import EFIDTrackingCuts
        InDetTrigCutValues = EFIDTrackingCuts
        #Configuration of parameters based on the signature and Flags (following Run2 settings)
        kwargs = {}
        #Prepare default parameter settings for the tool
        kwargs = setDefaults( kwargs,
                              Extrapolator        = InDetTrigExtrapolator,
                              DriftCircleCutTool  = InDetTrigTRTDriftCircleCut,
                              SummaryTool         = trackSummaryTool_getter( config.PT.setting.doTRT ),
                              #to have a steeper turn-n curve
                              minPt               = config.PT.setting.pTmin, #TODO: double check values, implement 0.95 for MinBias?  #InDetTrigCutValues.minPT(), #config.pTmin(),
                              maxRPhiImp          = InDetTrigCutValues.maxPrimaryImpact(),
                              maxZImp             = InDetTrigCutValues.maxZImpact(),
                              maxEta              = InDetTrigCutValues.maxEta(),
                              minSiClusters       = InDetTrigCutValues.minClusters(),
                              maxSiHoles          = InDetTrigCutValues.maxHoles(),
                              maxPixelHoles       = InDetTrigCutValues.maxPixelHoles(),
                              maxSCTHoles         = InDetTrigCutValues.maxSCTHoles(),
                              maxDoubleHoles      = InDetTrigCutValues.maxDoubleHoles(),
                              usePixel            = InDetTrigCutValues.usePixel(),
                              useSCT              = InDetTrigCutValues.useSCT(),
                              doEmCaloSeed        = False,
                              useAmbigFcn         = True,
                              useTRT_AmbigFcn     = False,
                              minTRTonTrk         = 0,
                             )

        #Change some of the parameters in case of beamgas signature
        if config.PT.isSignature('beamgas'):
            from InDetTrigRecExample.ConfiguredNewTrackingTrigCuts import EFIDTrackingCutsBeamGas
            kwargs = setDefaults( kwargs,
                                  minPt          = EFIDTrackingCutsBeamGas.minPT(),
                                  maxRPhiImp     = EFIDTrackingCutsBeamGas.maxPrimaryImpact(),
                                  maxZImp        = EFIDTrackingCutsBeamGas.maxZImpact(),
                                  minSiClusters  = EFIDTrackingCutsBeamGas.minClusters(),
                                  maxSiHoles     = EFIDTrackingCutsBeamGas.maxHoles(),
                                  useSigmaChi2   = True
                                )


        from InDetTrackScoringTools.InDetTrackScoringToolsConf import InDet__InDetAmbiScoringTool
        return InDet__InDetAmbiScoringTool(name             = name,
                                           **kwargs
                                           )





#--------------------------------------------------------------------------
#                    Track Ambiguity algs/tools
def associationTool_getter():
      #TODO double check this!
      from InDetRecExample.TrackingCommon import getInDetTrigPRDtoTrackMapToolGangedPixels
      return getInDetTrigPRDtoTrackMapToolGangedPixels()

def trackFitterTool_getter(config):
      #For now load from RecLoadTools where the config is based on: InDetTrigFlags.trackFitterType()  (gaussian, kalman, globalChi2, ...)
      #There are also variations of cosmic/TRT fitters -> Decision which fitter to return  has to be adapted based on the signature as well
      if config.PT.isSignature('cosmicsN'):
         from InDetTrigRecExample.InDetTrigConfigRecLoadTools import InDetTrigTrackFitterCosmics
         return InDetTrigTrackFitterCosmics
      else:
         from InDetTrigRecExample.InDetTrigConfigRecLoadTools import InDetTrigTrackFitter
         return  InDetTrigTrackFitter

def trackSelectionTool_getter(config):
      #TODO this might need to be revisited!

      if config.PT.isSignature('beamgas'):
        from InDetTrigRecExample.InDetTrigConfigRecLoadToolsBeamGas import InDetTrigAmbiTrackSelectionToolBeamGas
        return InDetTrigAmbiTrackSelectionToolBeamGas

      elif config.PT.isSignature('cosmicsN'):
        from InDetTrigRecExample.InDetTrigConfigRecLoadToolsCosmics import  InDetTrigAmbiTrackSelectionToolCosmicsN
        return InDetTrigAmbiTrackSelectionToolCosmicsN

      else:
        from InDetTrigRecExample.InDetTrigConfigRecLoadTools import InDetTrigAmbiTrackSelectionTool
        return InDetTrigAmbiTrackSelectionTool




#--------------------------------------------------------------------------
#                    Track Ambiguity Solver algs/tools

@makePublicTool
def ambiguityProcessorTool_builder( name, config):

   #Configuration of parameters based on the signature and Flags (following Run2 settings)
   kwargs = {}

   #Add parameters to empty kwargs
   if config.PT.isSignature('cosmicsN'):
     kwargs = setDefaults( kwargs,
                           SuppressHoleSearch = False,
                           RefitPrds =  False)
   elif config.PT.isSignature('electron') and InDetTrigFlags.doBremRecovery():
     import AthenaCommon.SystemOfUnits as Units
     kwargs = setDefaults( kwargs,
                           tryBremFit  = True,
                           pTminBrem   = 5 * Units.GeV)

   if InDetTrigFlags.materialInteractions() and InDetTrigFlags.solenoidOn():
     kwargs = setDefaults( kwargs,
                           MatEffects = 3   )
   else:
     kwargs = setDefaults( kwargs,
                           MatEffects = 0  )

   #-----------------------
   #Set/Get subtools
   #trackFitterTool    = trackFitterTool_getter(config),

   scoringTool        = ambiguityScoringTool_builder( name   = get_full_name( 'AmbiguityScoringTool',config.name),
                                                      config = config)

   associationTool    = associationTool_getter()

   trackSummaryTool   = trackSummaryTool_getter( config.PT.setting.doTRT )

   trackSelectionTool = trackSelectionTool_getter(config)


   kwargs = setDefaults(kwargs,
                                                 Fitter           = trackFitterTool_getter(config), # trackFitterTool,
                                                 ScoringTool      = scoringTool,
                                                 AssociationTool  = associationTool,
                                                 TrackSummaryTool = trackSummaryTool,
                                                 SelectionTool    = trackSelectionTool
                                                 )

   #Return configured tool
   from TrkAmbiguityProcessor.TrkAmbiguityProcessorConf import Trk__SimpleAmbiguityProcessorTool
   return  Trk__SimpleAmbiguityProcessorTool(    name             = name,
                                                 **kwargs
                                   #**setDefaults(kwargs,
                                   #              Fitter           = trackFitterTool,
                                   #              ScoringTool      = scoringTool,
                                   #              AssociationTool  = associationTool,
                                   #              TrackSummaryTool = trackSummaryTool,
                                   #              SelectionTool    = trackSelectionTool
                                   #              )
                                             )




#Need to differentiate between "standard" precision tracking code seeded by FTF and EFID-like/offline pattern recognition seeding configured here:
#https://gitlab.cern.ch/atlas/athena/blob/master/InnerDetector/InDetExample/InDetRecExample/share/ConfiguredNewTrackingSiPattern.py#L499


from TrkAmbiguitySolver.TrkAmbiguitySolverConf import Trk__TrkAmbiguitySolver
def ambiguitySolverAlg_builder(name, config, inputTrackScoreMap, outputTrackCollection):

      #Set/Get subtools
      ambiguityProcessor = ambiguityProcessorTool_builder( name   = get_full_name( 'AmbiguityProcessor', config.name),
                                                           config = config )

      return Trk__TrkAmbiguitySolver( name               = name,
                                      TrackInput         = inputTrackScoreMap,
                                      TrackOutput        = outputTrackCollection,
                                      AmbiguityProcessor = ambiguityProcessor
                                    )


#--------------------------------------------------------------------------
#                    Track Ambiguity Score algs/tools
@makePublicTool
def ambiguityScoreProcessorTool_builder( name, config):
   #   Tool contains backend functions for calculating score of a provided track
   #   Score of each track is based on track parameters such as hits in the ID, higher score -> more likely to survive ambiguity resolving between tracks

      #-----------------------
      #Set/Get subtools
      scoringTool = ambiguityScoringTool_builder( name   = get_full_name( 'AmbiguityScoringTool',config.name ),
                                                  config = config)

      associationTool    = associationTool_getter()

      trackSelectionTool = trackSelectionTool_getter(config)

      from TrkAmbiguityProcessor.TrkAmbiguityProcessorConf import Trk__DenseEnvironmentsAmbiguityScoreProcessorTool
      return  Trk__DenseEnvironmentsAmbiguityScoreProcessorTool( name               = name,
                                                                 ScoringTool        = scoringTool,
                                                                 AssociationTool    = associationTool,
                                                                 SelectionTool      = trackSelectionTool
                                                                )



from TrkAmbiguitySolver.TrkAmbiguitySolverConf import Trk__TrkAmbiguityScore
def ambiguityScoreAlg_builder(name, config, inputTrackCollection, outputTrackScoreMap ):
      #Alg loops over provided tracks and calls subtool(TrkAmbigutyScoreProcessor) to assign scores for each one of them

      #-----------------------
      #Set/Get subtools
      #ambiguityScoreProcessor = ambiguityScoreProcessorTool_builder( name   = get_full_name( 'AmbiguityScoreProcessorTool', config.name()),
      #                                                               config = config )

      return Trk__TrkAmbiguityScore(   name                    = name,
                                       TrackInput              = [ inputTrackCollection ], #[ config.FT.trkTracksFTF()  ], 
                                       TrackOutput             = outputTrackScoreMap,  #get_scoredmap( get_name_suffix( config.name ) ),
                                       #Disable processor, see: https://gitlab.cern.ch/atlas/athena/-/merge_requests/36431
                                       AmbiguityScoreProcessor = None, #ambiguityScoreProcessor,
                                   )

#-------------------------------------------------------------------------------------------------
#                       Alg/Tools for offline pattern recognition tracking

@makePublicTool
def siSpacePointsSeedMakerTool_builder(name, trackingCuts, usePrdAssociationTool ):
   from InDetRecExample.InDetKeys  import  InDetKeys
   from .InDetTrigCollectionKeys   import  TrigPixelKeys, TrigSCTKeys
   from InDetRecExample.InDetJobProperties import InDetFlags

   kwargs = {}
   kwargs = setDefaults( kwargs,
                         pTmin                  = trackingCuts.minPT(),
                         maxdImpact             = trackingCuts.maxPrimaryImpact(),
                         maxZ                   = trackingCuts.maxZImpact(),
                         minZ                   = -trackingCuts.maxZImpact(),
                         usePixel               = trackingCuts.usePixel(),
                         SpacePointsPixelName   = TrigPixelKeys.SpacePoints,
                         useSCT                 = (trackingCuts.useSCT() and trackingCuts.useSCTSeeding()),
                         SpacePointsSCTName     = TrigSCTKeys.SpacePoints,
                         useOverlapSpCollection = (trackingCuts.useSCT() and trackingCuts.useSCTSeeding()),
                         SpacePointsOverlapName = InDetKeys.OverlapSpacePoints(), #Switch to trigger flags?
                         radMax                 = trackingCuts.radMax(),
                         RapidityCut            = trackingCuts.maxEta())
                       
   
   #Change/add tracking  parameters based on the different tracking mode
   if trackingCuts.mode() == "Offline" or InDetFlags.doHeavyIon() or  trackingCuts.mode() == "ForwardTracks":
      kwargs = setDefaults( kwargs,
                            maxdImpactPPS = trackingCuts.maxdImpactPPSSeeds(),
                            maxdImpactSSS = trackingCuts.maxdImpactSSSSeeds())
   
   if usePrdAssociationTool:
   # not all classes have that property !!!
      kwargs = setDefaults( kwargs,
                            PRDtoTrackMap      = TrigPixelKeys.PRDtoTrackMap)

   #FIXME: switch to TrigFlags?
   if not InDetFlags.doCosmics():
      kwargs = setDefaults( kwargs,
                            maxRadius1     = 0.75*trackingCuts.radMax(),
                            maxRadius2     = trackingCuts.radMax(),
                            maxRadius3     = trackingCuts.radMax())

   #FIXME add later if needed
   #if trackingCuts.mode() == "LowPt" or trackingCuts.mode() == "VeryLowPt" or (trackingCuts.mode() == "Pixel" and InDetFlags.doMinBias()):
   #   try :
   #      InDetSiSpacePointsSeedMaker.pTmax              = trackingCuts.maxPT()
   #   except:
   #      pass 
   #   InDetSiSpacePointsSeedMaker.mindRadius         = 4.0
   #
   #if trackingCuts.mode() == "SLHC" or trackingCuts.mode() == "SLHCConversionFinding":
   #   InDetSiSpacePointsSeedMaker.minRadius1         = 0
   #   InDetSiSpacePointsSeedMaker.minRadius2         = 0
   #   InDetSiSpacePointsSeedMaker.minRadius3         = 0
   #   InDetSiSpacePointsSeedMaker.maxRadius1         =1000.*Units.mm
   #   InDetSiSpacePointsSeedMaker.maxRadius2         =1000.*Units.mm
   #   InDetSiSpacePointsSeedMaker.maxRadius3         =1000.*Units.mm
   #
   #if trackingCuts.mode() == "ForwardTracks" or trackingCuts.mode() == "ForwardSLHCTracks" or trackingCuts.mode() == "VeryForwardSLHCTracks":
   #   InDetSiSpacePointsSeedMaker.checkEta           = True
   #   InDetSiSpacePointsSeedMaker.etaMin             = trackingCuts.minEta()
   #   InDetSiSpacePointsSeedMaker.etaMax             = trackingCuts.maxEta()
   #   InDetSiSpacePointsSeedMaker.RapidityCut        = trackingCuts.maxEta()
   #
   #if trackingCuts.mode() == "DBM":
   #   InDetSiSpacePointsSeedMaker.etaMin             = trackingCuts.minEta()
   #   InDetSiSpacePointsSeedMaker.etaMax             = trackingCuts.maxEta()
   #   InDetSiSpacePointsSeedMaker.useDBM = True
   
   
   from SiSpacePointsSeedTool_xk.SiSpacePointsSeedTool_xkConf import InDet__SiSpacePointsSeedMaker_ATLxk as SiSpacePointsSeedMaker
   return SiSpacePointsSeedMaker ( name    =  name,
                                   **kwargs)




@makePublicTool
def zVertexMakerTool_builder(name, trackingCuts, seedMakerTool ):
   from InDetRecExample.InDetJobProperties import InDetFlags
   
   kwargs = {}
   
   #Prepare default parameter settings for the tool
   kwargs = setDefaults( kwargs,
                         Zmax          = trackingCuts.maxZImpact(),
                         Zmin          = -trackingCuts.maxZImpact(),
                         minRatio      = 0.17,
                         SeedMakerTool = seedMakerTool )
   
   
   if InDetFlags.doHeavyIon():
      kwargs = setDefaults( kwargs,
                            HistSize   = 2000,
                            minContent = 30)
   
   from SiZvertexTool_xk.SiZvertexTool_xkConf import InDet__SiZvertexMaker_xk
   return InDet__SiZvertexMaker_xk(name  = name,
                                   **kwargs)

   


def prdAssociation_builder( InputCollections ):
   import InDetRecExample.TrackingCommon as TrackingCommon
   #FIXME: If so:
   # 1] Get a new association tool
   #associationTool = TrackingCommon.getInDetTrigPRDtoTrackMapToolGangedPixels(),
   
   # 2] Use the ganged pixel from here?
   #from InDetTrigRecExample.InDetTrigConfigRecLoadTools import InDetTrigPrdAssociationTool
   
   # 3] Create the new one as in offline tracking?:
   prefix     = 'TrigInDet'
   suffix     = ''#NewTrackingCuts.extension()
   return TrackingCommon.getInDetTrackPRD_Association(namePrefix   = prefix,
                                                      nameSuffix   = suffix,
                                                      TracksName   = list(InputCollections))#This is readHandle #What input collection Thought there are no tracks at this point??!     
   # 4] if so do I use normal or ganged?
   #from InDetAssociationTools.InDetAssociationToolsConf import InDet__InDetPRD_AssociationToolGangedPixels
   #InDetTrigPrdAssociationl = InDet__InDetPRD_AssociationToolGangedPixels(name = "%sPrdAssociationTool%s"%(prefix,suffix),
   #                                                                          PixelClusterAmbiguitiesMapName = TrigPixelKeys.PRDtoTrackMap )

@makePublicTool
def siDetectorElementRoadMakerTool_builder( name, trackingCuts ):
   from InDetRecExample.InDetKeys  import  InDetKeys 
   
   #Are we happy with this propagator?
   from SiDetElementsRoadTool_xk.SiDetElementsRoadTool_xkConf import InDet__SiDetElementsRoadMaker_xk
   return  InDet__SiDetElementsRoadMaker_xk(name               = name,
                                            PropagatorTool     = trigPropagator_getter(),
                                            usePixel           = trackingCuts.usePixel(),
                                            PixManagerLocation = InDetKeys.PixelManager(), #FIXME: InDetTrigKeys?
                                            useSCT             = trackingCuts.useSCT(), 
                                            SCTManagerLocation = InDetKeys.SCT_Manager(),  #FIXME InDetTrigKeys?
                                            RoadWidth          = trackingCuts.RoadWidth())



@makePublicTool
def siCombinatorialTrackFinderTool_builder( name, trackingCuts ):
   from .InDetTrigCollectionKeys   import TrigPixelKeys, TrigSCTKeys
   from AthenaCommon.DetFlags      import DetFlags
   from InDetRecExample.InDetJobProperties import InDetFlags
   import InDetRecExample.TrackingCommon as TrackingCommon

   #FIXME: quick hack to try running ID, remove later
   DetFlags.ID_setOn()
   
   #Are we happy with these settings?
   from InDetTrigRecExample.InDetTrigConfigRecLoadTools import InDetTrigSCTConditionsSummaryTool, InDetTrigPatternUpdator
   # @TODO ensure that PRD association map is used if usePrdAssociationTool is set
   
   kwargs = {}
   #Prepare default parameter settings for the tool
   kwargs = setDefaults( kwargs,
                         PropagatorTool        = trigPropagator_getter(),
                         UpdatorTool           = InDetTrigPatternUpdator,
                         SctSummaryTool        = InDetTrigSCTConditionsSummaryTool, #Any reason for this to be turned off? None,
                         RIOonTrackTool        = TrackingCommon.getInDetRotCreatorDigital(),
                         usePixel              = DetFlags.haveRIO.pixel_on(),
                         useSCT                = DetFlags.haveRIO.SCT_on(),
                         PixelClusterContainer = TrigPixelKeys.Clusters,
                         SCT_ClusterContainer  = TrigSCTKeys.Clusters)
   
   #FIXME: Use TriggerFlags instead?
   if InDetFlags.doDBMstandalone() or trackingCuts.extension() =='DBM':
      kwargs = setDefaults( kwargs,
                            MagneticFieldMode     = "NoField",
                            TrackQualityCut       = 9.3,
                            useSCT                =  False,
                            RIOonTrackTool        = TrackingCommon.getInDetRotCreatorDBM(),
                            )
   
   
   #Add SCT condition summary if specified
   #FIXME: Use TriggerFlags instead?
   #if (DetFlags.haveRIO.SCT_on()):
   #   kwargs = setDefaults( kwargs,
   #                         SctSummaryTool = InDetTrigSCTConditionsSummaryTool )
   
   from SiCombinatorialTrackFinderTool_xk.SiCombinatorialTrackFinderTool_xkConf import InDet__SiCombinatorialTrackFinder_xk
   return InDet__SiCombinatorialTrackFinder_xk(name  = name,
                                               **kwargs)
   

@makePublicTool
def siTrackMakerTool_builder( name, siDetElementsRoadMakerTool, trackFinderTool, trackingCuts, usePrdAssociationTool ):
   from InDetRecExample.InDetJobProperties import InDetFlags
   from InDetRecExample.InDetKeys          import InDetKeys 
   
   trackPatternRecoInfo = 'SiSPSeededFinder'
   if InDetFlags.doCosmics():
      trackPatternRecoInfo = 'SiSpacePointsSeedMaker_Cosmic'
   
   elif InDetFlags.doHeavyIon():
     trackPatternRecoInfo = 'SiSpacePointsSeedMaker_HeavyIon'
   
   elif trackingCuts.mode() == "LowPt":
     trackPatternRecoInfo = 'SiSpacePointsSeedMaker_LowMomentum'
   
   elif trackingCuts.mode() == "VeryLowPt" or (trackingCuts.mode() == "Pixel" and InDetFlags.doMinBias()):
     trackPatternRecoInfo = 'SiSpacePointsSeedMaker_VeryLowMomentum'           
   
   elif trackingCuts.mode() == "BeamGas":
     trackPatternRecoInfo = 'SiSpacePointsSeedMaker_BeamGas'
   
   elif trackingCuts.mode() == "ForwardTracks":
     trackPatternRecoInfo = 'SiSpacePointsSeedMaker_ForwardTracks'
   
   elif trackingCuts.mode() == "ForwardSLHCTracks":
     trackPatternRecoInfo = 'SiSpacePointsSeedMaker_ForwardSLHCTracks'
   
   elif trackingCuts.mode() == "VeryForwardSLHCTracks": 
     trackPatternRecoInfo = 'SiSpacePointsSeedMaker_VeryForwardSLHCTracks' 
   
   elif trackingCuts.mode() == "SLHCConversionFinding":
     trackPatternRecoInfo = 'SiSpacePointsSeedMaker_SLHCConversionTracks'
   
   elif trackingCuts.mode() == "LargeD0" or trackingCuts.mode() == "LowPtLargeD0":
     trackPatternRecoInfo = 'SiSpacePointsSeedMaker_LargeD0'
   
   useBremMode = trackingCuts.mode() == "Offline" or trackingCuts.mode() == "SLHC" or trackingCuts.mode() == "DBM"
   
   kwargs = {}
   #Prepare default parameter settings for the tool
   kwargs = setDefaults( kwargs,
                         useSCT                        = trackingCuts.useSCT(),
                         usePixel                      = trackingCuts.usePixel(),
                         RoadTool                      = siDetElementsRoadMakerTool,
                         CombinatorialTrackFinder      = trackFinderTool,
                         pTmin                         = trackingCuts.minPT(),
                         pTminBrem                     = trackingCuts.minPTBrem(),
                         pTminSSS                      = InDetFlags.pT_SSScut(),
                         nClustersMin                  = trackingCuts.minClusters(),
                         nHolesMax                     = trackingCuts.nHolesMax(),
                         nHolesGapMax                  = trackingCuts.nHolesGapMax(),
                         SeedsFilterLevel              = trackingCuts.seedFilterLevel(),
                         Xi2max                        = trackingCuts.Xi2max(),
                         Xi2maxNoAdd                   = trackingCuts.Xi2maxNoAdd(),
                         nWeightedClustersMin          = trackingCuts.nWeightedClustersMin(),
                         CosmicTrack                   = InDetFlags.doCosmics(),
                         Xi2maxMultiTracks             = trackingCuts.Xi2max(), # was 3.
                         useSSSseedsFilter             = InDetFlags.doSSSfilter(),
                         doMultiTracksProd             = True,
                         useBremModel                  = InDetFlags.doBremRecovery() and useBremMode, # only for NewTracking the brem is debugged !!!
                         doCaloSeededBrem              = InDetFlags.doCaloSeededBrem(),
                         doHadCaloSeedSSS              = InDetFlags.doHadCaloSeededSSS(),
                         phiWidth                      = trackingCuts.phiWidthBrem(),
                         etaWidth                      = trackingCuts.etaWidthBrem(),
                         InputClusterContainerName     = InDetKeys.CaloClusterROIContainer(), 
                         InputHadClusterContainerName  = InDetKeys.HadCaloClusterROIContainer(), 
                         TrackPatternRecoInfo          = trackPatternRecoInfo,
                         UseAssociationTool            = usePrdAssociationTool)
   
   
   #Change the parameters based on the tracking cuts
   if trackingCuts.mode() == "SLHC" or trackingCuts.mode() == "ForwardSLHCTracks" or trackingCuts.mode() == "VeryForwardSLHCTracks" :
      kwargs = setDefaults( kwargs,
                            ITKGeometry = True )
   
   if trackingCuts.mode() == "DBM":
      kwargs = setDefaults( kwargs,
                            MagneticFieldMode = "NoField",
                            useBremModel = False,
                            doMultiTracksProd = False,
                            pTminSSS = -1,
                            CosmicTrack = False,
                            useSSSseedsFilter = False,
                            doCaloSeededBrem = False,
                            doHadCaloSeedSSS = False)
   
   if InDetFlags.doStoreTrackSeeds():
      from SeedToTrackConversionTool.SeedToTrackConversionToolConf import InDet__SeedToTrackConversionTool
      InDet_SeedToTrackConversion = InDet__SeedToTrackConversionTool(name       = "InDet_SeedToTrackConversion"+trackingCuts.extension(),
                                                                     OutputName = InDetKeys.SiSPSeedSegments()+trackingCuts.extension())
      kwargs = setDefaults( kwargs,
                            SeedToTrackConversion = InDet_SeedToTrackConversion,
                            SeedSegmentsWrite = True )
   
   
   
   from SiTrackMakerTool_xk.SiTrackMakerTool_xkConf import InDet__SiTrackMaker_xk
   return  InDet__SiTrackMaker_xk(name = name,
                                  **kwargs)



def siSPSeededTrackFinder_builder( name, outputTracks, trackingCuts, usePrdAssociationTool, nameSuffix ):

   #FIXME:
   # 1] Currently some flags are copy paste from offline configuration, might need to switch those to trigger flags
   # 2] trackingCuts are adapted from offline version as well, ideally we would want to have these from ConfigSettings.py in the end
   # 3] This code requires proper validation (currently ongoing using minBias and cosmic signatures)


   from .InDetTrigCollectionKeys           import TrigPixelKeys, TrigSCTKeys
   from InDetRecExample.InDetJobProperties import InDetFlags


   
   #Load subtools of the TrackFinder
   siSpacePointsSeedMakerTool = siSpacePointsSeedMakerTool_builder(name                  = get_full_name( 'siSPSeedMaker', nameSuffix),
                                                                   trackingCuts          = trackingCuts,
                                                                   usePrdAssociationTool = usePrdAssociationTool )
   
   # --- Z-coordinates primary vertices finder (only for collisions)
   zVertexMakerTool = None
   #FIXME:Switch to trig flags?
   if InDetFlags.useZvertexTool() and trackingCuts.mode() != "DBM":
      print('Running z-vertex maker')
      zVertexMakerTool =  zVertexMakerTool_builder(name, trackingCuts, siSpacePointsSeedMakerTool )
   
   # --- SCT and Pixel detector elements road builder
   siDetectorElementRoadMaker = siDetectorElementRoadMakerTool_builder( name         = get_full_name( 'SiDetectorElementRoadMaker', nameSuffix),
                                                                        trackingCuts = trackingCuts )
   
   # --- Local track finding using sdCaloSeededSSSpace point seed
   siCombinatorialTrackFinderTool = siCombinatorialTrackFinderTool_builder( name         = get_full_name( 'SiCombinatorialTrackFinder', nameSuffix),
                                                                            trackingCuts = trackingCuts)


   siTrackMakerTool =  siTrackMakerTool_builder( name                       = get_full_name( 'siTrackMaker', nameSuffix),
                                                 siDetElementsRoadMakerTool = siDetectorElementRoadMaker,
                                                 trackFinderTool            = siCombinatorialTrackFinderTool,
                                                 trackingCuts               = trackingCuts,
                                                 usePrdAssociationTool      = usePrdAssociationTool)

   #-----------------------------------------------------
   #  Configure parameters

   kwargs = {}
   #Prepare default parameter settings for the tool
   kwargs = setDefaults( kwargs,
                         TrackTool           = siTrackMakerTool,
                         PRDtoTrackMap       = TrigPixelKeys.PRDtoTrackMap if usePrdAssociationTool else '', #TODO: if prd is enabled this needs to be tested
                         SpacePointsPixelName= TrigPixelKeys.SpacePoints,
                         SpacePointsSCTName  = TrigSCTKeys.SpacePoints,
                         TrackSummaryTool    = trackSummaryTool_getter(doTRT=False), #TODO: x-check whether we need different config
                         TracksLocation      = outputTracks, #outEFIDTracks,
                         SeedsTool           = siSpacePointsSeedMakerTool,
                         ZvertexTool         = zVertexMakerTool, 
                         useZvertexTool      = InDetFlags.useZvertexTool() and trackingCuts.mode() != "DBM",
                         useNewStrategy      = InDetFlags.useNewSiSPSeededTF() and trackingCuts.mode() != "DBM",
                         useMBTSTimeDiff     = InDetFlags.useMBTSTimeDiff(),
                         useZBoundFinding    = trackingCuts.doZBoundary() and trackingCuts.mode() != "DBM" )
   
   
   #Specific tracking settings 
   if trackingCuts.mode() == "ForwardSLHCTracks" or trackingCuts.mode() == "ForwardTracks":
      kwargs = setDefaults( kwargs,
                            useNewStrategy     = False,
                            useMBTSTimeDiff    = InDetFlags.useMBTSTimeDiff(),
                            useZBoundFinding   = False,
                            useZvertexTool     = InDetFlags.useZvertexTool() )
   
      if InDetFlags.doHeavyIon():
            kwargs = setDefaults( kwargs, FreeClustersCut = 2) #Heavy Ion optimization from Igor

   #-----------------------------------------------------


   from SiSPSeededTrackFinder.SiSPSeededTrackFinderConf import InDet__SiSPSeededTrackFinder
   return InDet__SiSPSeededTrackFinder(name = name,
                                    **kwargs )

