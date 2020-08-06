#  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
#
#     Contains algs/tools used by Inner Detector Trigger
  
#Global keys/names for collections 
#from .InDetTrigCollectionKeys import TrigTRTKeys, TrigPixelKeys

from InDetRecExample.TrackingCommon import setDefaults

def get_name_prefix():
   #Too long? Do we need this to separate from InDet?
   return 'InDetTrigMT_'

def get_name_suffix(signature):
   return '_%s'%signature

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

   
from TrkTrackSummaryTool.TrkTrackSummaryToolConf import Trk__TrackSummaryTool
class TrackSummaryTool_builder(Trk__TrackSummaryTool):
   """ 
      @TODO add more description
   """
   def __init__(self, name="TrackSummaryTool", signature=''):
      self.signature = signature #str Parameter (muon, bjet, etc) that helps to determine specific settings of the tool

      #Building name of the class from prefix + name + suffix
      self.prefix = get_name_prefix( )
      self.suffix = get_name_suffix( signature )
      super(Trk__TrackSummaryTool, self ).__init__( '%s%s%s'%(self.prefix,name,self.suffix) )

      #-----------------------
      #Set alg/tool parameters
      #Atm it seems that the only difference between signatures is really either shared or no shared hits (any others?)
      
      #Option to take into consideration hits that are shared between tracks (Allowing only for ele/tau atm)
      self.doSharedHits = self.do_shared_hits()

      #This seems to be mostly on for the trigger, are there any instances where it is used? Compare with Run2
      self.doHolesInDet = True

      #-----------------------
      #Set subtools

      #Only subtool being used is the helper which contains all the functionality that is being called by TrackSummryTool (TST)
      self.load_summary_tool_helper()

      self.load_dedx_tool()

      self.load_trtElectronPIDTool_tool()

      self.load_pixelToTPID_tool()


   def do_shared_hits(self):
      return ( (self.signature == "electron") or ("tau" in self.signature) )
      #Should this be steered through TrigFlags instead?
      #InDetTrigFlags.doSharedHits(),

   #Configure and loads track summary helper tool 
   def load_summary_tool_helper(self, **kwargs):

      #from AthenaCommon.AppMgr import ToolSvc
      #TODO: Go through each tool loaded in here and create getter/ use offline getter
      from InDetTrigRecExample.InDetTrigConfigRecLoadTools import InDetTrigHoleSearchTool, InDetTrigPrdAssociationTool, InDetTrigTRTStrawStatusSummaryTool,  InDetTrigTestBLayerTool 

      kwargs =  setDefaults(  kwargs, 
                              HoleSearch         = InDetTrigHoleSearchTool,
                              AssoTool           = InDetTrigPrdAssociationTool,
                              TRTStrawSummarySvc = InDetTrigTRTStrawStatusSummaryTool,
                              #PixelToTPIDTool    = None #Normally take from InDetTrigPixelToTPIDTool which is also None atm
                           )            

      summaryHelper = None
      #Either calls summary helper with or without shared hit configuration
      if self.doSharedHits:

         #Arg that are being passed to indet getter
         #These tools are necessary for the shared hit version of the helper
         kwargs = setDefaults( kwargs,
                               TestBLayerTool         = InDetTrigTestBLayerTool,
               )  

         from InDetRecExample.TrackingCommon import getInDetSummaryHelperSharedHits
         summaryHelper = getInDetSummaryHelperSharedHits( name = "InDetTrigSummaryHelperSharedHits" + self.signature, **kwargs )
      else:
         kwargs = setDefaults(kwargs,
                              TestBLayerTool         = None,
                             )
         from InDetRecExample.TrackingCommon import getInDetSummaryHelper
         summaryHelper = getInDetSummaryHelper( name ="InDetTrigSummaryHelper" + self.signature, **kwargs )

      #ToolSvc += summaryHelper #Private? Already loaded in the TrackingCommon?
      self.InDetSummaryHelperTool = summaryHelper 
     
      

   def load_trtElectronPIDTool_tool(self):
      trtElectronPIDtool       = None   #Default
      if self.doSharedHits:
               from InDetTrigRecExample.InDetTrigConfigRecLoadTools import  InDetTrigTRT_ElectronPidTool
               trtElectronPIDtool       = InDetTrigTRT_ElectronPidTool

      self.TRT_ElectronPidTool = trtElectronPIDtool
      
   def load_dedx_tool(self):

      dxdeTool       = None   #Default
      if self.doSharedHits:
               from InDetTrigRecExample.InDetTrigConfigRecLoadTools import  InDetTrigTRT_ToT_dEdx
               dxdeTool       = InDetTrigTRT_ToT_dEdx

      self.TRT_ToT_dEdxTool = dxdeTool

   def load_pixelToTPID_tool(self):
      #Should this not have some setting for shared version? Offline does
      self.PixelToTPIDTool    = None #Normally take from InDetTrigPixelToTPIDTool which seems to be  just None atm

#--------------------------------------------------------------------------------------
## Scoring tools
def get_ambiguityScoringTool(config):

    from InDetTrigRecExample.InDetTrigSliceSettings import InDetTrigSliceSettings

    #TODO adapt cuts below based on the configuration settings
    #TODO put into signature settings!
    ptintcut = InDetTrigSliceSettings[('pTmin',signature)]
    if signature=='minBias':
      ptintcut = 0.95*InDetTrigSliceSettings[('pTmin',signature)]
    elif signature=='minBias400':
      ptintcut = 0.95*InDetTrigSliceSettings[('pTmin',signature)]

    #TODO retrieve new extrapolator? Use offline version?
    from InDetTrigRecExample.InDetTrigConfigRecLoadTools import InDetTrigExtrapolator
    from InDetTrigRecExample.InDetTrigConfigRecLoadTools import InDetTrigTRTDriftCircleCut

    #TODO: This seems to be run2 cuts setting, should we not have different for each signature/slice? 
    from InDetTrigRecExample.ConfiguredNewTrackingTrigCuts import EFIDTrackingCuts
    InDetTrigCutValues = EFIDTrackingCuts


    from InDetTrackScoringTools.InDetTrackScoringToolsConf import InDet__InDetAmbiScoringTool
    return InDet__InDetAmbiScoringTool(name,
                                       Extrapolator        = InDetTrigExtrapolator, #TODO: getInDetExtrapolator(),
                                       DriftCircleCutTool  = InDetTrigTRTDriftCircleCut, #TODO: getInDetTRTDriftCircleCutForPatternReco(),
                                       #to have a steeper turn-n curve
                                       minPt               = ptintcut,
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
                                       doEmCaloSeed        = False
                                       SummaryTool         = get_trk_summary_tool(config),
                                       ###
                                       useTRT_AmbigFcn= True,
                                       minTRTonTrk  = 0    # no TRT here
                                      )


def get_extScoringTool(config, **kwargs) :
    #This seems to be run2 cuts setting, should we not have different for each signature/slice? 
    from InDetTrigRecExample.ConfiguredNewTrackingTrigCuts import EFIDTrackingCuts
    #Shouldn't this be based on the signature? 
    InDetTrigCutValues = EFIDTrackingCuts

    #This part is in offline!
    #if InDetFlags.trackFitterType() in ['KalmanFitter', 'KalmanDNAFitter', 'ReferenceKalmanFitter']:
    #    kwargs=setDefaults(kwargs, minTRTPrecisionFraction = 0.2)

    #Retrieve base tool
    return get_ambiScoringToolBase(  config,
                                       **setDefaults( kwargs,
                                                      minTRTonTrk  = InDetTrigCutValues.minTRTonTrk(),
                                                      minTRTPrecisionFraction = InDetTrigCutValues.minTRTPrecFrac() #Note! Used in offline but not in run2 online
                                                     )
                                   )



#Temporary function before trk sum tool builder is ready
def get_trk_summary_tool(config):
   from InDetTrigRecExample.InDetTrigConfigRecLoadTools import InDetTrigTrackSummaryTool  
   from TrigInDetConf.TrigInDetRecCommonTools           import InDetTrigTrackSummaryToolSharedHitsWithTRTPid  

   #Load shared hits with Ele PID if TRT specified
   if config.doTRT:
      return InDetTrigTrackSummaryToolSharedHitsWithTRTPid
   else:
      return InDetTrigTrackSummaryTool

#--------------------------------------------------------------------------
#                    Track Ambiguity algs/tools

from TrkAmbiguityProcessor.TrkAmbiguityProcessorConf import Trk__SimpleAmbiguityProcessorTool 
class TrkAmbiguityProcessor_builder(Trk__SimpleAmbiguityProcessorTool):
   """ 
      @TODO add more description
   """
   def __init__(self, config):

      name = '%s%s%s' %( get_name_prefix(), 'TrkAmbiguityProcessor', get_name_suffix( config.name ) )
      super(Trk__SimpleAmbiguityProcessorTool, self ).__init__( name )

      #-----------------------
      #Set alg/tool parameters


      #-----------------------
      #Set subtools
      self.load_track_fitter()

      self.load_association_tool()

      self.load_trk_summary_tool(config)

      self.load_scoring_tool(config)

      self.load_trk_selection_tool()
      
 
   def load_track_fitter(self):
      #For now load from RecLoadTools where the config is based on: InDetTrigFlags.trackFitterType()  (gaussian, kalman, globalChi2, ...)
      #There are also variations of cosmic/TRT fitters 
      #Potentially create it is own builder in the future
      from InDetTrigRecExample.InDetTrigConfigRecLoadTools import InDetTrigTrackFitter 
      self.Fitter =  InDetTrigTrackFitter

   def load_trk_summary_tool(self,config):
      #Is this going to pick the same instance of TST as in scoring tool? Should we pass it as an arg instead?
      #trkSummaryTool = TrackSummaryTool_builder( name = '%sTrkSummaryTool%s' %(self.prefix,self.suffix), signature = signature )
      self.TrackSummaryTool = get_trk_summary_tool(config)
   

   def load_scoring_tool(self,config):

      from AthenaCommon.AppMgr import ToolSvc
      #trkSummaryTool =  #TrackSummaryTool_builder( name = '%sTrkSummaryTool%s' %(self.prefix,self.suffix), signature = signature )

      #TODO: Here based on some parameter (for instance name/signature) we should get different type of scoring tool (for cosmic, collision physics)
      scoreTool = get_ambiguityScoringTool(config)

      ToolSvc  += scoreTool #Should turn in the private?
      self.ScoringTool = scoreTool

   def load_association_tool(self):
      from InDetTrigRecExample.InDetTrigConfigRecLoadTools import  InDetTrigPrdAssociationTool
      self.AssociationTool =   InDetTrigPrdAssociationTool
      #Potential to use offline getter
      #AssociationTool  = TrackingCommon.getInDetTrigPRDtoTrackMapToolGangedPixels(),  


   def load_trk_selection_tool(self):
      from InDetTrigRecExample.InDetTrigConfigRecLoadTools import InDetTrigAmbiTrackSelectionTool
      #TODO this might need to be revisited!
      self.SelectionTool = InDetTrigAmbiTrackSelectionTool



#Need to differentiate between "standard" precision tracking code seeded by FTF and EFID-like/offline pattern recognition seeding configured here:
#https://gitlab.cern.ch/atlas/athena/blob/master/InnerDetector/InDetExample/InDetRecExample/share/ConfiguredNewTrackingSiPattern.py#L499


from TrkAmbiguitySolver.TrkAmbiguitySolverConf import Trk__TrkAmbiguitySolver
class TrkAmbiguitySolver_builder(Trk__TrkAmbiguitySolver):
   """ 
      This algorithm resolves any ambiguities between tracks
      Requires ambiguity processor tool
      @TODO add more description
   """
   def __init__(self, config):

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
      from AthenaCommon.AppMgr import ToolSvc
      #ToolSvc += ambiguityProcessor #Should this be private ?
      self.AmbiguityProcessor = TrkAmbiguityProcessor_builder( config = config ) 
      ToolSvc += self.AmbiguityProcessor
      


#--------------------------------------------------------------------------
#                    Track Ambiguity Score algs/tools


from TrkAmbiguityProcessor.TrkAmbiguityProcessorConf import Trk__DenseEnvironmentsAmbiguityScoreProcessorTool
class TrkAmbiguityScoreProcessor_builder(Trk__DenseEnvironmentsAmbiguityScoreProcessorTool):
   """ 
      @TODO add more description
   """
   def __init__(self, config):
      self.signature = config.name #str Parameter (muon, bjet, etc) that helps to determine specific settings of the tool

      #Building name of the class from prefix + name + suffix
      name = '%s%s%s' %( get_name_prefix(), 'AmbiguityScoreProcessor', get_name_suffix( config.name ) )
      super(Trk__DenseEnvironmentsAmbiguityScoreProcessorTool, self ).__init__( name )

      #print 'Signature in processor: %s' %signature

      #-----------------------
      #Set alg/tool parameters

      #-----------------------
      #Set subtools
      self.load_association_tool()

      self.load_scoring_tool(config)

      self.load_trk_selection_tool()
      

   def load_scoring_tool(self, config):
      #trkSummaryTool = TrackSummaryTool_builder( name = '%sTrkSummaryTool%s' %(self.prefix,self.suffix), signature= self.signature )

      from AthenaCommon.AppMgr import ToolSvc
      #TODO: Here based on some parameter (for instance name/signature) we should get different type of scoring tool (for cosmic, collision physics)
      scoreTool = get_ambiguityScoringTool( config ) 

      ToolSvc  += scoreTool #Should turn in the private?
      self.ScoringTool = scoreTool

   def load_association_tool(self):
      from InDetTrigRecExample.InDetTrigConfigRecLoadTools import  InDetTrigPrdAssociationTool
      self.AssociationTool =   InDetTrigPrdAssociationTool
      #Potential to use offline getter
      #AssociationTool  = TrackingCommon.getInDetTrigPRDtoTrackMapToolGangedPixels(),  


   def load_trk_selection_tool(self):
      from InDetTrigRecExample.InDetTrigConfigRecLoadTools import InDetTrigAmbiTrackSelectionTool
      #TODO this might need to be revisited!
      self.SelectionTool = InDetTrigAmbiTrackSelectionTool



from TrkAmbiguitySolver.TrkAmbiguitySolverConf import Trk__TrkAmbiguityScore
class TrkAmbiguityScore_builder(Trk__TrkAmbiguityScore):
   """ 
      @TODO add more description
   """
   def __init__(self, config):
      self.signature = config.name #str Parameter (muon, bjet, etc) that helps to determine specific settings of the tool

      #Building name of the class from prefix + name + suffix
      name = '%s%s%s' %( get_name_prefix(), 'TrkAmbiguityScore', get_name_suffix( config.name ) )
      super(Trk__TrkAmbiguityScore, self ).__init__( name )

      #print 'Signature in builder: %s' %signature

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
      return  "%s%sTrkTrack%s" %('HLT_ID', 'AmbSol', self.signature)

   def get_track_input_name(self):
      return  "ScoredMap%s" %(self.signature)

   #------------------------------------------------------------------
   # Loading subtools
   def load_ambi_score_processor(self,config):
      from AthenaCommon.AppMgr import ToolSvc
      #ambiguityScoreProcessor = TrkAmbiguityScoreProcessor_builder( name = "AmbiguityScoreProcessor", config = config )
      #ToolSvc += ambiguityScoreProcessor 
      self.AmbiguityScoreProcessor = TrkAmbiguityScoreProcessor_builder( config = config )
      ToolSvc +=  self.AmbiguityScoreProcessor #Make private?

