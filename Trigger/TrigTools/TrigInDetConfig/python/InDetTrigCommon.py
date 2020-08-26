#  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
#
#     Contains algs/tools used by Inner Detector Trigger
  
#Global keys/names for collections 
#from .InDetTrigCollectionKeys import TrigTRTKeys, TrigPixelKeys

from InDetRecExample.TrackingCommon import makePublicTool, setDefaults
from InDetTrigRecExample.InDetTrigFlags import InDetTrigFlags



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
#TODO:
#1] need to implement this in both FTF and PT
#2] Convert this tool into builders and getters
def getTrackParticleCnv( prefix, suffix, outPTTracks, outPTTrackParticles ):
  """ Use the  track collection and convert it to container of xAOD Track Particles  """
  #suffix = '_%s'suff if suff else '' #Suffix should be based on the signature and type of tracking, e.g for cosmics and PT  -> Cosmics_PT ( for FTF or EFID, Cosmics_FTF, Cosmics_EFID )
  from AthenaCommon.AppMgr import ToolSvc
  keepParameter = False 
  if 'Electron' in suffix:
      keepParameter = True
      

  from InDetTrigRecExample.InDetTrigConfigRecLoadTools import InDetTrigTrackSummaryTool, InDetTrigExtrapolator
  from TrkParticleCreator.TrkParticleCreatorConf import Trk__TrackParticleCreatorTool
  InDetTrigMTxAODParticleCreatorTool = Trk__TrackParticleCreatorTool(name =  "%sxAODParticleCreatorTool%s" %(prefix, suffix),
                                                                     Extrapolator = InDetTrigExtrapolator,
                                                                     KeepParameters = keepParameter,
                                                                     #TrackSummaryTool = InDetTrigTrackSummaryToolSharedHits) 
                                                                     TrackSummaryTool = InDetTrigTrackSummaryTool )
  
  ToolSvc += InDetTrigMTxAODParticleCreatorTool
  #log.info(InDetTrigMTxAODParticleCreatorTool)
  
  
  from xAODTrackingCnv.xAODTrackingCnvConf import xAODMaker__TrackCollectionCnvTool
  InDetTrigMTxAODTrackCollectionCnvTool= xAODMaker__TrackCollectionCnvTool(name = "%sxAODTrackCollectionCnvTool%s" %(prefix, suffix),
                                                                           TrackParticleCreator = InDetTrigMTxAODParticleCreatorTool)
  
  ToolSvc += InDetTrigMTxAODTrackCollectionCnvTool
  #log.info(InDetTrigMTxAODTrackCollectionCnvTool)
  
  #This one shouldn't be necessary
  #TODO: obsolete, try to turn off
  from xAODTrackingCnv.xAODTrackingCnvConf import  xAODMaker__RecTrackParticleContainerCnvTool
  InDetTrigMTRecTrackParticleContainerCnvTool=  xAODMaker__RecTrackParticleContainerCnvTool(name = "%sRecTrackContainerCnvTool%s" %(prefix, suffix),
                                                                                            TrackParticleCreator = InDetTrigMTxAODParticleCreatorTool)
  
  ToolSvc += InDetTrigMTRecTrackParticleContainerCnvTool



  #Adding new monitoring tool
  #First load the generic monitoring tool with set of histograms for Particle Cnv
  from TrigInDetMonitoringTools.TrigInDetTrackingMonitoring import  TrigInDetTrackCnvMonitoring
  #TODO need to add extra property for prefix and suffix of the track collections to be monitored
  #Or maybe no need the name itself should be obvious from the particle cnv alg
  monTool = TrigInDetTrackCnvMonitoring( name = '%sParticleCreatorCnv%s'%( prefix, suffix ) )

  #Now pass this tool to the Track Monitoring tool
  from TrigInDetMonitoringTools.TrigInDetMonitoringToolsConf import TrigInDetTrackMonitoringTool
  monTrackCnv = TrigInDetTrackMonitoringTool( name = '%sParticleCreatorCnv%s'%( prefix, suffix ),
                                             MonitoringTool = monTool
                                            )

  
   
  ToolSvc += monTrackCnv

  from xAODTrackingCnv.xAODTrackingCnvConf import xAODMaker__TrackParticleCnvAlg
  InDetTrigMTxAODTrackParticleCnvAlg = xAODMaker__TrackParticleCnvAlg(  name = "%sxAODParticleCreatorAlg%s" %( prefix, suffix),
                                                                      # Properties below are used for:  TrackCollection -> xAOD::TrackParticle
                                                                        ConvertTracks = True,  #Turn on  retrieve of TrackCollection, false by default
                                                                        TrackContainerName                        = outPTTracks,
                                                                        xAODTrackParticlesFromTracksContainerName = outPTTrackParticles, 
                                                                        TrackCollectionCnvTool = InDetTrigMTxAODTrackCollectionCnvTool,
                                                                       ## Properties below are used for: Rec:TrackParticle, aod -> xAOD::TrackParticle (Turn off)
                                                                        ConvertTrackParticles = False,  # Retrieve of Rec:TrackParticle, don't need this atm
                                                                        xAODContainerName = '',  
                                                                        RecTrackParticleContainerCnvTool = InDetTrigMTRecTrackParticleContainerCnvTool,
                                                                        TrackParticleCreator = InDetTrigMTxAODParticleCreatorTool,
                                                                        #Add online track monitoring
                                                                        DoMonitoring = True,
                                                                        TrackMonTool  = monTrackCnv
                                                                      )

  return InDetTrigMTxAODTrackParticleCnvAlg

   

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

    if config.isSignature('cosmicsN'):
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
                              SummaryTool         = trackSummaryTool_getter( config.doTRT() ),
                              #to have a steeper turn-n curve
                              minPt               = config.pTmin(), #TODO: double check values, implement 0.95 for MinBias?  #InDetTrigCutValues.minPT(), #config.pTmin(),
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
        if config.isSignature('beamgas'):
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
      if config.isSignature('cosmicsN'):
         from InDetTrigRecExample.InDetTrigConfigRecLoadTools import InDetTrigTrackFitterCosmics
         return InDetTrigTrackFitterCosmics
      else:
         from InDetTrigRecExample.InDetTrigConfigRecLoadTools import InDetTrigTrackFitter 
         return  InDetTrigTrackFitter

def trackSelectionTool_getter(config):
      #TODO this might need to be revisited!

      if config.isSignature('beamgas'):
        from InDetTrigRecExample.InDetTrigConfigRecLoadToolsBeamGas import InDetTrigAmbiTrackSelectionToolBeamGas
        return InDetTrigAmbiTrackSelectionToolBeamGas

      elif config.isSignature('cosmicsN'):
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
   if config.isSignature('cosmicsN'):
     kwargs = setDefaults( kwargs,
                           SuppressHoleSearch = False,
                           RefitPrds =  False)
   elif config.isSignature('electron') and InDetTrigFlags.doBremRecovery():
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

   scoringTool        = ambiguityScoringTool_builder( name   = get_full_name( 'AmbiguityScoringTool',config.name()),
                                                      config = config)

   associationTool    = associationTool_getter()

   trackSummaryTool   = trackSummaryTool_getter( config.doTRT() )

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
         if config.doTRT():
            return  config.trkTracksAS() #"%s%sTrkTrack%s" %('HLT_ID', 'AmbSol', get_name_suffix( config.name() ))
         #Otherwise use final collection name
         else:
            return  config.trkTracksPT()


      #-----------------------
      #Set/Get subtools
      ambiguityProcessor = ambiguityProcessorTool_builder( name   = get_full_name( 'AmbiguityProcessor', config.name()),
                                                           config = config ) 

      return Trk__TrkAmbiguitySolver( name               = name,
                                      TrackInput         = get_scoredmap( get_name_suffix(config.name() )),
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
      scoringTool = ambiguityScoringTool_builder( name   = get_full_name( 'AmbiguityScoringTool',config.name() ),
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
      ambiguityScoreProcessor = ambiguityScoreProcessorTool_builder( name   = get_full_name( 'AmbiguityScoreProcessorTool', config.name()),
                                                                     config = config )

      return Trk__TrkAmbiguityScore(   name                    = name,
                                       TrackInput              = [ config.trkTracksFTF()  ],
                                       TrackOutput             = get_scoredmap( get_name_suffix( config.name() ) ),
                                       AmbiguityScoreProcessor = ambiguityScoreProcessor,
                                    )
      

