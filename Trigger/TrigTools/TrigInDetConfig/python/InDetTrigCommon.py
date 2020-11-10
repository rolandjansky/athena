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
def get_scoredmap( suffix ):
   return "ScoredMap{}".format(suffix) 


def trackSummaryTool_getter( doTRT ):
   from InDetTrigRecExample.InDetTrigConfigRecLoadTools import InDetTrigTrackSummaryTool  
   from TrigInDetConf.TrigInDetRecCommonTools           import InDetTrigTrackSummaryToolSharedHitsWithTRTPid  

   #Load shared hits with Ele PID if TRT specified
   if doTRT:
      return InDetTrigTrackSummaryToolSharedHitsWithTRTPid
   else:
      return InDetTrigTrackSummaryTool

#--------------------------------------------------------------------------------------


@makePublicTool
def trackParticleCreatorTool_builder(name, config):
  """Tool with functionality to convert Trk:Tracks into xAOD::TrackParticles"""
  from InDetTrigRecExample.InDetTrigConfigRecLoadTools import  InDetTrigExtrapolator
  from TrkParticleCreator.TrkParticleCreatorConf import Trk__TrackParticleCreatorTool
  return Trk__TrackParticleCreatorTool(name             = name,
                                       Extrapolator     = InDetTrigExtrapolator,
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
def ambiguitySolverAlg_builder(name, config):

      #Get correct name for the ouput TrkTrack collection
      def getTrackOutput():
         #If we are also applying TRT then this collection is just intermediate
         if config.PT.setting.doTRT:
            return  config.PT.trkTracksAS() #"%s%sTrkTrack%s" %('HLT_ID', 'AmbSol', get_name_suffix( config.name() ))
         #Otherwise use final collection name
         else:
            return  config.PT.trkTracksPT()

      #-----------------------
      #Set/Get subtools
      ambiguityProcessor = ambiguityProcessorTool_builder( name   = get_full_name( 'AmbiguityProcessor', config.name),
                                                           config = config ) 

      return Trk__TrkAmbiguitySolver( name               = name,
                                      TrackInput         = get_scoredmap( get_name_suffix(config.name )),
                                      TrackOutput        = getTrackOutput(),
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
def ambiguityScoreAlg_builder(name, config):
      #Alg loops over provided tracks and calls subtool(TrkAmbigutyScoreProcessor) to assign scores for each one of them

      #-----------------------
      #Set/Get subtools
      #ambiguityScoreProcessor = ambiguityScoreProcessorTool_builder( name   = get_full_name( 'AmbiguityScoreProcessorTool', config.name()),
      #                                                               config = config )

      return Trk__TrkAmbiguityScore(   name                    = name,
                                       TrackInput              = [ config.FT.trkTracksFTF()  ], 
                                       TrackOutput             = get_scoredmap( get_name_suffix( config.name ) ),
                                       #Disable processor, see: https://gitlab.cern.ch/atlas/athena/-/merge_requests/36431
                                       AmbiguityScoreProcessor = None, #ambiguityScoreProcessor,  
                                    )
      

