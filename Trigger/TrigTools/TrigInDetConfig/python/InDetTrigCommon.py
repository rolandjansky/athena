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

def get_full_name( core, suffix ):
   return  '{}{}{}'.format(  get_name_prefix(), core, get_name_suffix(suffix) ),

#Temporary function before trk sum tool builder is ready
def get_track_summary_tool( doTRT ):
   from InDetTrigRecExample.InDetTrigConfigRecLoadTools import InDetTrigTrackSummaryTool  
   from TrigInDetConf.TrigInDetRecCommonTools           import InDetTrigTrackSummaryToolSharedHitsWithTRTPid  

   #Load shared hits with Ele PID if TRT specified
   if doTRT:
      return InDetTrigTrackSummaryToolSharedHitsWithTRTPid
   else:
      return InDetTrigTrackSummaryTool

#TODO 
#need to implement this in both FTF and PT
#Better retrieval of summary tool (rather than through a parameter?)? Use the correct one!
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
                                                                        #PrintIDSummaryInfo = True, #Just to test and have some output
                                                                        TrackParticleCreator = InDetTrigMTxAODParticleCreatorTool,
                                                                        #Add online track monitoring
                                                                        DoMonitoring = True,
                                                                        TrackMonTool  = monTrackCnv
                                                                      )

  return InDetTrigMTxAODTrackParticleCnvAlg

   

#--------------------------------------------------------------------------------------
## Scoring tools
@makePublicTool
def get_ambiguity_scoring_tool(name, config):

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
    from InDetTrigRecExample.ConfiguredNewTrackingTrigCuts import EFIDTrackingCuts
    InDetTrigCutValues = EFIDTrackingCuts
    #https://gitlab.cern.ch/atlas/athena/-/blob/master/InnerDetector/InDetExample/InDetRecExample/python/ConfiguredNewTrackingCuts.py
    #1] Is this really what we want here? 
    #2] Somehow merge/adapt the settings in InDetTrigConfigSettings.py?

    doTRT = config.doTRT()
    
    if config.name() == 'cosmicsN':
        #Can potentially recreate the isntance of the tool here and in the if config just have a list of parameters needed to be changed for the tool
        from InDetTrigRecExample.InDetTrigConfigRecLoadToolsCosmics import InDetTrigScoringToolCosmics_SiPattern 
        return InDetTrigScoringToolCosmics_SiPattern
    else:
        from InDetTrackScoringTools.InDetTrackScoringToolsConf import InDet__InDetAmbiScoringTool
        return InDet__InDetAmbiScoringTool(name                = name,
                                       Extrapolator        = InDetTrigExtrapolator, 
                                       DriftCircleCutTool  = InDetTrigTRTDriftCircleCut,
                                       #to have a steeper turn-n curve
                                       minPt               = InDetTrigCutValues.minPT(), #config.pTmin(),
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
                                       useTRT_AmbigFcn     = True,
                                       minTRTonTrk         = 0,
                                       SummaryTool         = get_track_summary_tool( doTRT ),
                                      )






#--------------------------------------------------------------------------
#                    Track Ambiguity algs/tools

def get_association_tool():
      return TrackingCommon.getInDetTrigPRDtoTrackMapToolGangedPixels(),  
      #Which one to use?
      from InDetTrigRecExample.InDetTrigConfigRecLoadTools import  InDetTrigPrdAssociationTool
      return  InDetTrigPrdAssociationTool

def get_track_fitter(config):
      #For now load from RecLoadTools where the config is based on: InDetTrigFlags.trackFitterType()  (gaussian, kalman, globalChi2, ...)
      #There are also variations of cosmic/TRT fitters -> Decision which fitter to return  has to be adapted based on the signature as well

      if config.name() == 'cosmicsN':
         from InDetTrigRecExample.InDetTrigConfigRecLoadTools import InDetTrigTrackFitterCosmics
         return InDetTrigTrackFitterCosmics
      else:
         from InDetTrigRecExample.InDetTrigConfigRecLoadTools import InDetTrigTrackFitter 
         return  InDetTrigTrackFitter

def get_track_selection_tool(config):
      #TODO this might need to be revisited!

      if config.name() == 'beamgas':
        from InDetTrigRecExample.InDetTrigConfigRecLoadToolsBeamGas import InDetTrigAmbiTrackSelectionToolBeamGas
        return InDetTrigAmbiTrackSelectionToolBeamGas

      elif config.name() == 'cosmicsN':
        from InDetTrigRecExample.InDetTrigConfigRecLoadToolsCosmics import  InDetTrigAmbiTrackSelectionToolCosmicsN
        return InDetTrigAmbiTrackSelectionToolCosmicsN

      else:
        from InDetTrigRecExample.InDetTrigConfigRecLoadTools import InDetTrigAmbiTrackSelectionTool
        return InDetTrigAmbiTrackSelectionTool





@makePublicTool
def get_TrkAmbiguityProcessorTool( name, config):

   #Configuration of parameters based on the signature and Flags (following Run2 settings)
   kwargs = {}

   #Add parameters to empty kwargs
   if config.name() == 'cosmicsN':
     kwargs = setDefaults( kwargs,
                           SuppressHoleSearch = False,
                           RefitPrds =  False)
   elif config.name() =='electron' and InDetTrigFlags.doBremRecovery():
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


   #Return configured tool
   from TrkAmbiguityProcessor.TrkAmbiguityProcessorConf import Trk__SimpleAmbiguityProcessorTool
   return  Trk__SimpleAmbiguityProcessorTool(    name             = name , 
                                                 Fitter           = get_track_fitter(config),
                                                 ScoringTool      = get_ambiguity_scoring_tool( get_full_name( 'AmbiguityScoringTool',config.name()), config),
                                                 AssociationTool  = get_association_tool(),
                                                 TrackSummaryTool = get_track_summary_tool( config.doTRT() ),
                                                 SelectionTool    = get_track_selection_tool(config),
                                                 **kwargs
                                             )

   




#Need to differentiate between "standard" precision tracking code seeded by FTF and EFID-like/offline pattern recognition seeding configured here:
#https://gitlab.cern.ch/atlas/athena/blob/master/InnerDetector/InDetExample/InDetRecExample/share/ConfiguredNewTrackingSiPattern.py#L499


from TrkAmbiguitySolver.TrkAmbiguitySolverConf import Trk__TrkAmbiguitySolver
class TrkAmbiguitySolver_builder(Trk__TrkAmbiguitySolver):
   """ 
      This algorithm resolves any ambiguities between tracks
      Requires ambiguity processor tool
   """
   def __init__(self, name, config):

      name = '%s%s%s' %( get_name_prefix(), 'TrkAmbiguitySolver', get_name_suffix( config.name ) )
      super(Trk__TrkAmbiguitySolver, self ).__init__( name ) 

      #-----------------------
      #Set alg/tool parameters

      #Set input/output names
      self.set_track_collections( config.name )

      #-----------------------
      #Set subtools

      self.load_ambi_processor(config.name)
      

   #------------------------------------------------------------------
   # Track collection naming convention 
   def set_track_collections( self, signature ):
         self.TrackInput   = self.get_track_input_name(signature)
         self.TrackOutput  = self.get_track_output_name(signature)

   def get_track_output_name(self,signature):
      return  "%s%sTrkTrack%s" %('HLT_ID', 'AmbSol', signature)

   def get_track_input_name(self,signature):
      return  "ScoredMap%s" %(signature)

   #------------------------------------------------------------------
   # Loading subtools
   def load_ambi_processor(self, config):
      self.AmbiguityProcessor = get_TrkAmbiguityProcessorTool( get_full_name( 'AmbiguityProcessor', config.name()), config = config ) 
      


#--------------------------------------------------------------------------
#                    Track Ambiguity Score algs/tools
@makePublicTool
def get_TrkAmbiguityScoreProcessorTool( name, config):
   #   Tool contains backend functions for calculating score of a provided track 
   #   Score of each track is based on track parameters such as hits in the ID, higher score -> more likely to survive ambiguity resolving between tracks
      
      from AthenaCommon.AppMgr import ToolSvc
      from InDetTrigRecExample.InDetTrigConfigRecLoadTools import  InDetTrigPrdAssociationTool, InDetTrigAmbiTrackSelectionTool

      from TrkAmbiguityProcessor.TrkAmbiguityProcessorConf import Trk__DenseEnvironmentsAmbiguityScoreProcessorTool
      return  Trk__DenseEnvironmentsAmbiguityScoreProcessorTool(     name               = name,
                                                                     ScoringTool        = get_ambiguity_scoring_tool( get_full_name( 'AmbiguityScoringTool',config.name() ), config = config),
                                                                     AssociationTool    = InDetTrigPrdAssociationTool,#TrackingCommon.getInDetTrigPRDtoTrackMapToolGangedPixels(),
                                                                     SelectionTool      = InDetTrigAmbiTrackSelectionTool ) # #TODO this might need to be revisited!




from TrkAmbiguitySolver.TrkAmbiguitySolverConf import Trk__TrkAmbiguityScore
class TrkAmbiguityScore_builder(Trk__TrkAmbiguityScore):
   def __init__(self, name, config):
      """ 
         Alg loops over provided tracks and calls subtool(TrkAmbigutyScoreProcessor) to assign scores for each one of them
      """
      super().__init__( name  )

      self.signatureSuffix = get_name_suffix(config.name() )


      #-----------------------
      #Set alg/tool parameters

      #Set input/output names
      self.set_track_collections( )

      #-----------------------
      #Set subtools
      self.load_ambi_score_processor( config )
      

   #------------------------------------------------------------------
   # Track collection naming convention 
   def set_track_collections( self ):
         self.TrackInput   = [ self.get_track_input_name()  ]
         self.TrackOutput  =   self.get_track_output_name()

   def get_track_output_name(self):
      return  "%s%sTrkTrack%s" %('HLT_ID', 'AmbSol', self.signatureSuffix)

   def get_track_input_name(self):
      return  "ScoredMap%s" %(self.signatureSuffix)

   #------------------------------------------------------------------
   # Loading subtools
   def load_ambi_score_processor(self, signatureConfig):
      from AthenaCommon.AppMgr import ToolSvc
      self.AmbiguityScoreProcessor = get_TrkAmbiguityScoreProcessorTool( name = get_full_name( 'AmbiguityScoreProcessorTool', signatureConfig.name()), config = signatureConfig )
