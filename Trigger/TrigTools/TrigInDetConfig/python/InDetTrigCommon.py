#  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
#
#     Contains algs/tools used by Inner Detector Trigger
  
#Global keys/names for collections 
from .InDetTrigCollectionKeys import TrigTRTKeys, TrigPixelKeys

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
  from TrigInDetMonitoringTools.TrigInDetTrackMonitoringToolConf import TrigInDetTrackMonitoringTool
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





#-----------------------------------------------------------------------------
#                        Choose track summary helper tool
#
#
#
#  from AthenaCommon.GlobalFlags import globalflags
#  
#  from InDetTrigRecExample.InDetTrigCommonTools import InDetTrigTRTStrawStatusSummaryTool
#  
#  from InDetTrackSummaryHelperTool.InDetTrackSummaryHelperToolConf import InDet__InDetTrackSummaryHelperTool
#  from InDetTrigRecExample.InDetTrigConditionsAccess import TRT_ConditionsSetup
#  InDetTrigTrackSummaryHelperTool = InDet__InDetTrackSummaryHelperTool(name          = "InDetTrigSummaryHelper",
#                                                                       HoleSearch    = InDetTrigHoleSearchTool,
#                                                                       AssoTool      = InDetTrigPrdAssociationTool,
#                                                                       TestBLayerTool = None,
#                                                                       PixelToTPIDTool= InDetTrigPixelToTPIDTool,
#                                                                       DoSharedHits  = False,
#                                                                       TRTStrawSummarySvc=InDetTrigTRTStrawStatusSummaryTool,
#                                                                       usePixel      = DetFlags.haveRIO.pixel_on(),
#                                                                       useSCT        = DetFlags.haveRIO.SCT_on(),
#                                                                       useTRT        = DetFlags.haveRIO.TRT_on())
#    
#  ToolSvc += InDetTrigTrackSummaryHelperTool
#  if (InDetTrigFlags.doPrintConfigurables()):
#    print (     InDetTrigTrackSummaryHelperTool)
#
#   from InDetTrackSummaryHelperTool.InDetTrackSummaryHelperToolConf import InDet__InDetTrackSummaryHelperTool
#    from InDetTrigRecExample.InDetTrigConditionsAccess import TRT_ConditionsSetup
#    InDetTrigTrackSummaryHelperToolSharedHits = InDet__InDetTrackSummaryHelperTool(name         = "InDetTrigSummaryHelperSharedHits",
#                                                                                   AssoTool     = InDetTrigPrdAssociationTool,
#                                                                                   DoSharedHits = InDetTrigFlags.doSharedHits(),
#                                                                                   HoleSearch   = InDetTrigHoleSearchTool,
#                                                                                   TestBLayerTool=InDetTrigTestBLayerTool,
#                                                                                   PixelToTPIDTool=InDetTrigPixelToTPIDTool,
#                                                                                   TRTStrawSummarySvc = InDetTrigTRTStrawStatusSummaryTool)
#
#


#def getInDetTrigTrackSummaryToolBase(slice, name='InDetTrigTrackSummaryTool', **kwargs) :
#  the_name=makeName(name,kwargs)
#
#  from TrkTrackSummaryTool.TrkTrackSummaryToolConf import Trk__TrackSummaryTool
#  return Trk__TrackSummaryTool(the_name,
#                                       **setDefaults(kwargs,
#                                                    InDetSummaryHelperTool = InDetTrigTrackSummaryHelperTool,
#                                                    doSharedHits           = False,
#                                                    doHolesInDet           = True,
#                                                    #this may be temporary #61512 (and used within egamma later)
#                                                    #TRT_ElectronPidTool    = InDetTrigTRT_ElectronPidTool, 
#                                                    TRT_ElectronPidTool    = None, 
#                                                    )
#
#def getInDetTrigTrackSummaryTool(slice, name='InDetTrigTrackSummaryTool', **kwargs) :
#  the_name=makeName(name,kwargs)
#  return getInDetTrigTrackSummaryToolBase(name = the_name,
#                                       **setDefaults(kwargs,
#                                                    InDetSummaryHelperTool = InDetTrigTrackSummaryHelperTool,
#                                                    doSharedHits           = False,
#                                                    doHolesInDet           = True,
#                                                    #this may be temporary #61512 (and used within egamma later)
#                                                    #TRT_ElectronPidTool    = InDetTrigTRT_ElectronPidTool, 
#                                                    TRT_ElectronPidTool    = None, 
#                                                    )
#
#
#
#   
#def trackExtrapolatorTool_builder(signature):
#   #TODO: create a new instance of this tool 
#   #if InDetTrigFlags.loadExtrapolator():
#   from InDetTrigRecExample.InDetTrigConfigRecLoadTools import InDetTrigExtrapolator
#   return InDetTrigExtrapolator
#
#def associationTool_builder(signature):
#   #Shielded by trigger flag? Are there instances where this is not necessary?
#   #if InDetTrigFlags.loadAssoTool():
#  from InDetAssociationTools.InDetAssociationToolsConf import InDet__InDetPRD_AssociationToolGangedPixels
#  return = InDet__InDetPRD_AssociationToolGangedPixels( name                           = "InDetTrigPrdAssociationTool_" + signature,
#                                                        PixelClusterAmbiguitiesMapName = TrigPixelKeys.PixelClusterAmbiguitiesMap )
#
#def holeSearchTool_builder(signature, extrapolator):
#
#  #Retrieve extrapolator
#  #extrapolator = trackExtrapolatorTool_builder(signature)
#  sctCondSummaryTool = sctCondSummaryTool_builder(signature)
#  pixelLayerTool     = pixelLayerTool_builder(signature)
#
#  from InDetTrackHoleSearch.InDetTrackHoleSearchConf import InDet__InDetTrackHoleSearchTool
#  InDetTrigHoleSearchTool = InDet__InDetTrackHoleSearchTool(name = "InDetTrigHoleSearchTool_"+signature ,
#                                                            Extrapolator   = extrapolator,
#                                                            usePixel       = DetFlags.haveRIO.pixel_on(),
#                                                            useSCT         = DetFlags.haveRIO.SCT_on(),
#                                                            SctSummaryTool = sctCondSummaryTool,
#                                                            PixelLayerTool = pixelLayerTool,
#                                                            )
#
#
#   
#def trackSummaryHelperTool_builder(signature):
#
#   if signature == "electron" or "tau" in signature: 
#      doSharedHits = True
#
#   extrapolator    = trackExtrapolatorTool_builder(signature)
#
#   associationTool = associationTool_builder(signature)
#
#   holeSearchTool  = holeSearchTool_builder(signature, extrapolator) 
#
#   bLayerTool      = bLayerTool_builder(signature)
#
#   pixelToTPIDTool  = pixelToTPIDTool_builder(signature)
#
#   trtStrawStatusTool  = trtStrawStatusTool_builder(signature)
#
#
#  from InDetTrackSummaryHelperTool.InDetTrackSummaryHelperToolConf import InDet__InDetTrackSummaryHelperTool
#  #from InDetTrigRecExample.InDetTrigConditionsAccess import TRT_ConditionsSetup
#  return  InDet__InDetTrackSummaryHelperTool(name                 = "InDetTrigSummaryHelper",
#                                             HoleSearch           = holeSearchTool,
#                                             AssoTool             = associationTool,
#                                             TestBLayerTool       = bLayerTool,
#                                             PixelToTPIDTool      = pixelToPIDTool,
#                                             TRTStrawSummarySvc   = trtStrawStatusTool, 
#                                             DoSharedHits         = doSharedHits,
#                                             usePixel             = DetFlags.haveRIO.pixel_on(),
#                                             useSCT               = DetFlags.haveRIO.SCT_on(),
#                                             useTRT               = DetFlags.haveRIO.TRT_on())




   


from TrkTrackSummaryTool.TrkTrackSummaryToolConf import Trk__TrackSummaryTool
class TrackSummaryTool_builder(Trk__TrackSummaryTool):
   """ 
      @TODO add more description
   """
   def __init__(self, name="TrackSummaryTool", signature):

      self.prefix = get_name_prefix( )
      self.suffix = get_name_suffix( signature )
      super(Trk__TrackSummaryTool, self ).__init__( '%s%s%s'%(self.prefix,name,self.suffix) )

      #-----------------------
      #Set alg/tool parameters
      #Atm it seems that the only difference between signatures is really either shared or no shared hits (any others?)
      
      #Option to take into consideration hits that are shared between tracks (Allowing only for ele/tau atm)
      self.doSharedHits = do_shared_hits(signature)

      #This seems to be mostly on for the trigger, are there any instances where it is used? Compare with Run2
      self.doHolesInDet = True

      #-----------------------
      #Set subtools

      #Only subtool being used is the helper which contains all the functionality that is being called by TrackSummryTool (TST)
      load_summary_helper(signature)


   def do_shared_hits(self,signature):
      return ( (signature == "electron") or ("tau" in signature) )
      #Should this be steered through TrigFlags instead?
      #InDetTrigFlags.doSharedHits(),

   #Configure and loads track summary helper tool 
   def load_summary_helper(self,signature):

      #TODO: Go through each tool loaded in here and create getter/ use offline getter
      from InDetTrigRecExample.InDetTrigConfigRecLoadTools import InDetTrigHoleSearchTool, InDetTrigPrdAssociationTool, InDetTrigTRTStrawStatusSummaryTool, InDetTrigTRT_ElectronPidTool, InDetTrigTestBLayerTool, InDetTrigTRT_ToT_dEdx,

      kwargs = setDefaults(kwargs,
                              HoleSearch         = InDetTrigHoleSearchTool,
                              AssoTool           = InDetTrigPrdAssociationTool,
                              TRTStrawSummarySvc = InDetTrigTRTStrawStatusSummaryTool,
                              PixelToTPIDTool    = None, #Normally take from InDetTrigPixelToTPIDTool which is also None atm
                             )

      summaryHelper = None
      #Either calls summary helper with or without shared hit configuration
      if self.doSharedHits:

         #Arg that are being passed to indet getter
         #These tools are necessary for the shared hit version of the helper
         kwargs = setDefaults(kwargs,
               TRT_ElectronPidTool    = InDetTrigTRT_ElectronPidTool,         
               TestBLayerTool         = InDetTrigTestBLayerTool,
               TRT_ToT_dEdxTool       = InDetTrigTRT_ToT_dEdx,  

         from InDetRecExample.TrackingCommon import getInDetSummaryHelperSharedHits
         summaryHelper = getInDetSummaryHelperSharedHits( name, **kwargs )

      else:

         kwargs = setDefaults(kwargs,
               TestBLayerTool         = None
               TRT_ElectronPidTool    = None,         # we don't want to use those tools during pattern
               TRT_ToT_dEdxTool       = None,         # dito

         from InDetRecExample.TrackingCommon import getInDetSummaryHelper
         summaryHelper = getInDetSummaryHelper(name, **kwargs )

      ToolSvc += summaryHelper #Private?
      self.InDetSummaryHelperTool = summaryHelper 
      
      

#--------------------------------------------------------------------------------------
## Scoring tools

def get_ambiScoringToolBase( name='AmbiScoringTool', signature, **kwargs) :
    from InDetRecExample.TrackingCommon import makeName
    #NewTrackingCuts = kwargs.pop("NewTrackingCuts")
    #TODO: add InDetTrigMT to offline common tracking naming functions/make our own
    #the_name=makeName(name,kwargs)
    
    from InDetTrigRecExample.InDetTrigSliceSettings import InDetTrigSliceSettings
    ptintcut = InDetTrigSliceSettings[('pTmin',signature)]
    if signature=='minBias':
      ptintcut = 0.95*InDetTrigSliceSettings[('pTmin',signature)]
    elif signature=='minBias400':
      ptintcut = 0.95*InDetTrigSliceSettings[('pTmin',signature)]

    from InDetTrigRecExample.InDetTrigConfigRecLoadTools import InDetTrigExtrapolator
    from InDetTrigRecExample.InDetTrigConfigRecLoadTools import InDetTrigTRTDriftCircleCut

    #TODO: This seems to be run2 cuts setting, should we not have different for each signature/slice? 
    from InDetTrigRecExample.ConfiguredNewTrackingTrigCuts import EFIDTrackingCuts
    InDetTrigCutValues = EFIDTrackingCuts


    from InDetTrackScoringTools.InDetTrackScoringToolsConf import InDet__InDetAmbiScoringTool
    return InDet__InDetAmbiScoringTool(name,
                                       **setDefaults(kwargs,
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
                                                     )


   
def get_ambiScoringTool(name='AmbiScoringTool', signature, **kwargs) :
    #Retrieve base tool
    ambScoringTool =  get_ambiScoringToolBase(name, signature,
                                       **setDefaults( kwargs,
                                                      useTRT_AmbigFcn= True,
                                                      minTRTonTrk  = 0,    # no TRT here
                                                    )
    #Is this still needed?
    if signature=='beamgas':
        from InDetTrigRecExample.ConfiguredNewTrackingTrigCuts import EFIDTrackingCutsBeamGas
        ambScoringTool.minPt          = EFIDTrackingCutsBeamGas.minPT()
        ambScoringTool.maxRPhiImp     = EFIDTrackingCutsBeamGas.maxPrimaryImpact()
        ambScoringTool.maxZImp        = EFIDTrackingCutsBeamGas.maxZImpact()
        ambScoringTool.minSiClusters  = EFIDTrackingCutsBeamGas.minClusters()
        ambScoringTool.maxSiHoles     = EFIDTrackingCutsBeamGas.maxHoles()
        ambScoringTool.useTRT_AmbigFcn= False
        ambScoringTool.useSigmaChi2   = True

    return ambScoringTool




def get_extScoringTool(name='ExtScoringTool', signature, **kwargs) :
    #This seems to be run2 cuts setting, should we not have different for each signature/slice? 
    from InDetTrigRecExample.ConfiguredNewTrackingTrigCuts import EFIDTrackingCuts
    InDetTrigCutValues = EFIDTrackingCuts

    #This part is in offline!
    #if InDetFlags.trackFitterType() in ['KalmanFitter', 'KalmanDNAFitter', 'ReferenceKalmanFitter']:
    #    kwargs=setDefaults(kwargs, minTRTPrecisionFraction = 0.2)

    #Retrieve base tool
    return get_ambiScoringToolBase(    name, signature,
                                       **setDefaults( kwargs,
                                                      minTRTonTrk  = InDetTrigCutValues.minTRTonTrk(),
                                                      minTRTPrecisionFraction = NewTrackingCuts.minTRTPrecFrac() #Note! Used in offline but not in run2 online
                                                     )

#--------------------------------------------------------------------------------------

def get_name_prefix():
   #Too long? Do we need this to separate from InDet?
   return 'InDetTrigMT_'

def get_name_suffix(signature):
   return '_%s'%signature

from TrkAmbiguityProcessor.TrkAmbiguityProcessorConf import Trk__SimpleAmbiguityProcessorTool 
class AmbiguityProcessor_builder(Trk__SimpleAmbiguityProcessorTool):
   """ 
      @TODO add more description
   """
   def __init__(self, name="AmbiguityProcessor", signature):

      self.prefix = get_name_prefix( )
      self.suffix = get_name_suffix( signature )
      super(Trk__SimpleAmbiguityProcessorTool, self ).__init__( '%s%s%s'%(self.prefix,name,self.suffix) )

      #-----------------------
      #Set alg/tool parameters


      #-----------------------
      #Set subtools
      self.load_track_fitter()

      self.load_association_tool()

      self.load_trk_summary_tool(signature)

      self.load_scoring_tool(signature)

      self.load_trk_selection_tool()
      
 
   def load_track_fitter(self):
      #For now load from RecLoadTools where the config is based on: InDetTrigFlags.trackFitterType()  (gaussian, kalman, globalChi2, ...)
      #There are also variations of cosmic/TRT fitters 
      #Potentially create it is own builder in the future
      from InDetTrigRecExample.InDetTrigConfigRecLoadTools import InDetTrigTrackFitter 
      self.Fitter =  InDetTrigTrackFitter

   def load_trk_summary_tool(self,signature):
      #Is this going to pick the same instance of TST as in scoring tool? Should we pass it as an arg instead?
      trkSummaryTool = TrackSummaryTool_builder( name = '%sTrkSummaryTool%s' %(self.prefix,self.suffix), signature )
      self.TrackSummaryTool = trkSummaryTool
   

   def load_scoring_tool(self,signature):
      trkSummaryTool = TrackSummaryTool_builder( name = '%sTrkSummaryTool%s' %(self.prefix,self.suffix), signature )

      #TODO: Here based on some parameter (for instance name/signature) we should get different type of scoring tool (for cosmic, collision physics)
      self.ScoringTool = get_ambiScoringTool( name = '%sAmbiScoringTool%s'%(self.prefix, self.suffix),
                                              signature = signature,
                                              #Extrapolator = InDetTrigExtrapolator #This is default
                                              SummaryTool  =  trkSummaryTool,
                                             )
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
class AmbiguitySolver_builder(Trk__TrkAmbiguitySolver):
   """ 
      This algorithm resolves any ambiguities between tracks
      Requires ambiguity processor tool
      @TODO add more description
   """
   def __init__(self, name="AmbiguitySolver", signature):

      prefix = get_name_prefix( )
      suffix = get_name_suffix( signature )
      super(Trk__TrkAmbiguitySolver, self ).__init__( '%s%s%s'%(prefix,name,suffix) )

      #-----------------------
      #Set alg/tool parameters

      #Set input/output names
      self.set_track_collections( signature )


      #-----------------------
      #Set subtools

      self.load_ambi_processor(signature)
      

   #------------------------------------------------------------------
   # Track collection naming convention 
   def set_track_collections( self, signature ):
         self.TrackInput   = get_track_input_name(signature)
         self.TrackOutput  = get_track_output_name(signature)

   def get_track_output_name(signature):
      return  "%s%sTrkTrack%s" %('HLT_ID', 'AmbSol', signature)

   def get_track_input_name(signature):
      return  "ScoredMap%s" %(signature)

   #------------------------------------------------------------------
   # Loading subtools
   def load_ambi_processor(self, signature):
      ambiguityProcessor = AmbiguityProcessor_builder( signature ) 
      ToolSvc += ambiguityProcessor #Should this be private ?
      self.AmbiguityProcessor = ambiguityProcessor
      








