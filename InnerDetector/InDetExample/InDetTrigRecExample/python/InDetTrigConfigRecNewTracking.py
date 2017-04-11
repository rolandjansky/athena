# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

# ------------------------------------------------------------
#
# ----------- now we do the RTF inside-out tracking
#
# ------------------------------------------------------------
# common things
from AthenaCommon.AppMgr import ToolSvc
from InDetTrigRecExample.InDetTrigFlags import InDetTrigFlags
from InDetTrigRecExample.ConfiguredNewTrackingTrigCuts import \
    EFIDTrackingCuts,EFIDTrackingCutsCosmics,EFIDTrackingCutsBeamGas, \
    EFIDTrackingCutsHeavyIon, FTKTrackingCuts
InDetTrigCutValues = EFIDTrackingCuts

from AthenaCommon.Logging import logging 
log = logging.getLogger("EFID")

# ----------- SiSPSeededTrackFinder
#
# Track finder using space points seeds
#---------------------------------------
# (it uses the tools which are setup above because it knows their instance names internally.
#  the tools jobOs need to be changed to configurables)

from SiTrigSPSeededTrackFinder.SiTrigSPSeededTrackFinderConf import InDet__SiTrigSPSeededTrackFinder
class SiTrigTrackFinder_EF( InDet__SiTrigSPSeededTrackFinder ):
  __slots__ = []
  def __init__(self, name="SiTrigTrackFinder_Electron_EF", type="electron", lowPt=False):
    super( InDet__SiTrigSPSeededTrackFinder, self ).__init__( name )
    
    from InDetTrigRecExample.InDetTrigConfigRecLoadTools import InDetTrigPrdAssociationTool, \
        InDetTrigPatternPropagator, InDetTrigPatternUpdator, \
        InDetTrigRotCreator
    from AthenaCommon.AppMgr import ToolSvc
    
    # configure tools used
    
    # Space points seeds maker
    #
    from InDetTrigRecExample.InDetTrigSliceSettings import InDetTrigSliceSettings
    from AthenaCommon.DetFlags    import DetFlags
    
    if type=="beamgas":
      from InDetTrigRecExample.InDetTrigConfigRecLoadToolsBeamGas import InDetTrigSiSpacePointsSeedMakerBeamGas
      InDetTrigSiSpacePointsSeedMaker = InDetTrigSiSpacePointsSeedMakerBeamGas
      
    elif type=="cosmicsN":
      from SiSpacePointsSeedTool_xk.SiSpacePointsSeedTool_xkConf import InDet__SiSpacePointsSeedMaker_Cosmic

      InDetTrigSiSpacePointsSeedMaker = \
          InDet__SiSpacePointsSeedMaker_Cosmic(name                   = 'InDetTrigSiSpacePointsSeedMaker_'+type,
                                               pTmin                  = InDetTrigSliceSettings[('pTmin',type)],
                                               maxdImpact             = EFIDTrackingCutsCosmics.maxPrimaryImpact(),
                                               maxZ                   = EFIDTrackingCutsCosmics.maxZImpact(),
                                               minZ                   = -EFIDTrackingCutsCosmics.maxZImpact(),
                                               SpacePointsPixelName   = 'SCT_CosmicsTrigSpacePoints',
                                               SpacePointsSCTName     = 'PixelCosmicsTrigSpacePoints',
                                               #SpacePointsOverlapName = InDetKeys.OverlapSpacePoints(),
                                               UseAssociationTool     = False,
                                               AssociationTool        =  InDetTrigPrdAssociationTool)
    elif lowPt:
      from InDetTrigRecExample.InDetTrigConfigRecLoadToolsLowPt import InDetTrigSiSpacePointsSeedMakerLowPt
      InDetTrigSiSpacePointsSeedMaker = InDetTrigSiSpacePointsSeedMakerLowPt

    elif type=="heavyIon" or type=="heavyIonFS":
      from SiSpacePointsSeedTool_xk.SiSpacePointsSeedTool_xkConf import InDet__SiSpacePointsSeedMaker_HeavyIon
      InDetTrigSiSpacePointsSeedMaker = \
          InDet__SiSpacePointsSeedMaker_HeavyIon(name=                  'InDetTrigSiSpacePointsSeedMaker_'+type,
                                               pTmin = InDetTrigSliceSettings[('pTmin',type)],
                                               maxdImpact = EFIDTrackingCutsHeavyIon.maxPrimaryImpact(),
                                               maxZ = EFIDTrackingCutsHeavyIon.maxZImpact(),
                                               minZ = -EFIDTrackingCutsHeavyIon.maxZImpact(),
                                               #useOverlapSpCollection   = False, #overlapCollections not used in EFID
                                               useOverlapSpCollection   = InDetTrigFlags.doOverlapSP(), #testing
                                               usePixel = DetFlags.haveRIO.pixel_on(),
                                               useSCT   = DetFlags.haveRIO.SCT_on(),
                                               SpacePointsSCTName = 'SCT_TrigSpacePoints',
                                               SpacePointsPixelName = 'PixelTrigSpacePoints',
                                               SpacePointsOverlapName = 'SPTrigOverlap',
                                               radMax = EFIDTrackingCutsHeavyIon.radMax())
                                                 
    else:
      from SiSpacePointsSeedTool_xk.SiSpacePointsSeedTool_xkConf import InDet__SiSpacePointsSeedMaker_ATLxk
      InDetTrigSiSpacePointsSeedMaker = \
          InDet__SiSpacePointsSeedMaker_ATLxk( name = 'InDetTrigSiSpacePointsSeedMaker_'+type,
                                               pTmin = InDetTrigSliceSettings[('pTmin',type)],
                                               maxdImpact = InDetTrigCutValues.maxPrimaryImpact(),
                                               maxZ = InDetTrigCutValues.maxZImpact(),
                                               minZ = -InDetTrigCutValues.maxZImpact(),
                                               #useOverlapSpCollection   = False, #overlapCollections not used in EFID
                                               useOverlapSpCollection   = InDetTrigFlags.doOverlapSP(), #testing
                                               usePixel = DetFlags.haveRIO.pixel_on(),
                                               useSCT   = DetFlags.haveRIO.SCT_on(),
                                               SpacePointsSCTName = 'SCT_TrigSpacePoints',
                                               SpacePointsPixelName = 'PixelTrigSpacePoints',
                                               SpacePointsOverlapName = 'SPTrigOverlap',
                                               radMax = InDetTrigCutValues.radMax(),
                                               maxdImpactPPS = InDetTrigCutValues.maxdImpactPPSSeeds(),
                                               maxdImpactSSS = InDetTrigCutValues.maxdImpactSSSSeeds(),
                                               )

    if type!= "cosmicsN":
      InDetTrigSiSpacePointsSeedMaker.maxRadius1 =  0.75*InDetTrigCutValues.radMax()
      InDetTrigSiSpacePointsSeedMaker.maxRadius2 = InDetTrigCutValues.radMax()
      InDetTrigSiSpacePointsSeedMaker.maxRadius3 = InDetTrigCutValues.radMax()
      
    # if type=="minBias":
    #   InDetTrigSiSpacePointsSeedMaker.maxdImpact=5.

    ToolSvc += InDetTrigSiSpacePointsSeedMaker
    if (InDetTrigFlags.doPrintConfigurables()):
      print InDetTrigSiSpacePointsSeedMaker

    #InDetTrigSiSpacePointsSeedMaker.OutputLevel = 1
    
    # Z-coordinates primary vertices finder
    #

    from SiZvertexTool_xk.SiZvertexTool_xkConf import InDet__SiZvertexMaker_xk
    InDetTrigZvertexMaker = InDet__SiZvertexMaker_xk( name = 'InDetTrigZvertexMaker_'+type,
                                                        Zmax = InDetTrigCutValues.maxZImpact(),
                                                        Zmin = -InDetTrigCutValues.maxZImpact(),
                                                        minRatio = 0.17 # not default
                                                      )
    if type=="heavyIon" or type=="heavyIonFS":
      InDetTrigZvertexMaker.HistSize=2000
      InDetTrigZvertexMaker.minContent = 30
      

    #InDetTrigZvertexMaker.SeedMakerTool = InDet__SiSpacePointsSeedMaker_ATLxk('InDetTrigSiSpacePointsSeedMaker_'+type)#to make it tool neutral
    InDetTrigZvertexMaker.SeedMakerTool = ToolSvc.allConfigurables['InDetTrigSiSpacePointsSeedMaker_'+type]
    ToolSvc += InDetTrigZvertexMaker

    #InDetTrigZvertexMaker.OutputLevel=1
    
    # SCT and Pixel detector elements road builder
    #
    from InDetTrigRecExample.InDetTrigConfigRecLoadTools import InDetTrigSiDetElementsRoadMaker, InDetTrigSiComTrackFinder
    
    # Local track finding using space point seed
    #
    from SiTrackMakerTool_xk.SiTrackMakerTool_xkConf import InDet__SiTrackMaker_xk
    InDetTrigSiTrackMaker = InDet__SiTrackMaker_xk( name = 'InDetTrigSiTrackMaker_'+type,
                                                    RoadTool       = InDetTrigSiDetElementsRoadMaker,
                                                    CombinatorialTrackFinder = InDetTrigSiComTrackFinder,
                                                    pTmin          = InDetTrigSliceSettings[('pTmin',type)],
                                                    nClustersMin   = InDetTrigCutValues.minClusters(),
                                                    nHolesMax      = InDetTrigCutValues.nHolesMax(),
                                                    nHolesGapMax   = InDetTrigCutValues.nHolesGapMax(),
                                                    SeedsFilterLevel = InDetTrigCutValues.seedFilterLevel(),
                                                    Xi2max         = InDetTrigCutValues.Xi2max(),
                                                    Xi2maxNoAdd    = InDetTrigCutValues.Xi2maxNoAdd(),
                                                    nWeightedClustersMin= InDetTrigCutValues.nWeightedClustersMin(),
                                                    #CosmicTrack              = InDetFlags.doCosmics(),
                                                    Xi2maxMultiTracks         = InDetTrigCutValues.Xi2max(),
                                                    UseAssociationTool       = False
                                                    )

    if type=="beamgas":
      InDetTrigSiTrackMaker.pTmin = EFIDTrackingCutsBeamGas.minPT()
      InDetTrigSiTrackMaker.nClustersMin = EFIDTrackingCutsBeamGas.minClusters()
      InDetTrigSiTrackMaker.nHolesMax = EFIDTrackingCutsBeamGas.nHolesMax()
      InDetTrigSiTrackMaker.UseAssociationTool = True       #for BG and LowPt
    elif type=="cosmicsN":   
      #create an additional for cosmics
      from InDetTrigRecExample.InDetTrigConfigRecLoadToolsCosmics import InDetTrigSiDetElementsRoadMakerCosmics
      InDetTrigSiTrackMaker.RoadTool = InDetTrigSiDetElementsRoadMakerCosmics
      InDetTrigSiTrackMaker.nClustersMin = EFIDTrackingCutsCosmics.minClusters()
      InDetTrigSiTrackMaker.nHolesMax = EFIDTrackingCutsCosmics.nHolesMax()  #3 vs 7
      InDetTrigSiTrackMaker.nHolesGapMax = EFIDTrackingCutsCosmics.nHolesGapMax() #3
      InDetTrigSiTrackMaker.CosmicTrack=True
      #InDetTrigSiTrackMaker.GoodSeedClusterCount = 3
    elif type=='electron' and InDetTrigFlags.doBremRecovery():
      InDetTrigSiTrackMaker.useBremModel=True

    ToolSvc += InDetTrigSiTrackMaker
    
    if (InDetTrigFlags.doPrintConfigurables()):
      print InDetTrigSiTrackMaker

    self.SeedsTool = InDetTrigSiSpacePointsSeedMaker
    self.ZvertexTool = InDetTrigZvertexMaker
    self.useZvertexTool = InDetTrigFlags.useZvertexTool()
    if type=="beamgas" or type=="cosmicsN":
      self.ZvertexTool = None
      self.useZvertexTool = False
    elif type=="heavyIon" or type=="heavyIonFS":
      self.useZvertexTool = True
      
    if lowPt:
      from InDetTrigRecExample.InDetTrigConfigRecLoadToolsLowPt \
          import InDetTrigSiSpacePointsSeedMakerLowPt,InDetTrigSiTrackMakerLowPt, InDetTrigZvertexMakerLowPt
      self.TrackTool =  InDetTrigSiTrackMakerLowPt
      self.SeedsTool =  InDetTrigSiSpacePointsSeedMakerLowPt
      self.useZvertexTool = InDetTrigFlags.useZvertexTool()
      self.ZvertexTool = InDetTrigZvertexMakerLowPt
      
    else:
      #self.TrackTool = InDetTrigSiTrackMaker
      self.TrackTool =  InDet__SiTrackMaker_xk('InDetTrigSiTrackMaker_'+type)
       

    self.EtaHalfWidth = InDetTrigSliceSettings[('etaHalfWidth',type)]
    self.PhiHalfWidth = InDetTrigSliceSettings[('phiHalfWidth',type)]
    self.doFullScan =   InDetTrigSliceSettings[('doFullScan',type)]
    
    #monitoring
    from SiTrigSPSeededTrackFinder.SiTrigSPSeededTrackFinderMonitoring import \
        SiTrigSPSeededTrackFinderValidationMonitor, \
        SiTrigSPSeededTrackFinderOnlineMonitor
    
    from TrigTimeMonitor.TrigTimeHistToolConfig import TrigTimeHistToolConfig
    
    sitrktime = TrigTimeHistToolConfig("SiTRKTime")
    sitrktime.TimerHistLimits = [0,200]
    self.AthenaMonTools = [SiTrigSPSeededTrackFinderValidationMonitor(type=type),
                           SiTrigSPSeededTrackFinderOnlineMonitor(type=type),
                           sitrktime]
    
# ----------- Refitting of tracks before further processing
#
if InDetTrigFlags.doRefit():
   log.info("InDetTrigFlags - refit set but not used in the Trigger! ")   


# -------------------- 
#
class SiTrigTrackSeededFinder_EF( SiTrigTrackFinder_EF ):
  __slots__ = []
  def __init__(self, name="SiTrigTrackSeededFinder_Electron_EF", type="electron", lowPt=False):
    SiTrigTrackFinder_EF.__init__( self, name, type, lowPt )
    
    from SiTrackMakerTool_xk.SiTrackMakerTool_xkConf import InDet__SiTrackMaker_xk
    tracktool = InDet__SiTrackMaker_xk(self.TrackTool.getName()).clone('InDetTrigSiTrackMaker_merge_'+type) 
    from AthenaCommon.AppMgr import ToolSvc
    ToolSvc += tracktool
    self.TrackTool = tracktool
    self.useSeedMaker = False

# -------------------- 
#
class SiTrigSimpleTrackFinder_EF( SiTrigTrackFinder_EF ):
  __slots__ = []
  def __init__(self, name="SiTrigSimpleTrackFinder_Electron_EF", type="electron", lowPt=False):
    SiTrigTrackFinder_EF.__init__( self, name, type, lowPt )
    
    self.FastTracking = True
    #self.FastTracking = False
 

    

# ---------- Ambiguity solving
#
from InDetTrigAmbiguitySolver.InDetTrigAmbiguitySolverConf import InDet__InDetTrigAmbiguitySolver
class TrigAmbiguitySolver_EF( InDet__InDetTrigAmbiguitySolver ):
   __slots__ = []
   def __init__(self, name="TrigAmbiguitySolver_Electron_EF", type="electron", lowPt=False):
      super(InDet__InDetTrigAmbiguitySolver , self ).__init__( name )

      slice = type
      if name.find('FTK')>-1:
        slice = 'FTK'

      from AthenaCommon.AppMgr import ToolSvc
      from InDetTrigRecExample.InDetTrigConfigRecLoadTools import InDetTrigPrdAssociationTool, \
          InDetTrigTrackFitter, InDetTrigExtrapolator, InDetTrigTrackSummaryTool, \
          InDetTrigTRTDriftCircleCut

      # load InnerDetector TrackSelectionTool
      #

      from InDetTrigRecExample.InDetTrigSliceSettings import InDetTrigSliceSettings

      from InDetTrackScoringTools.InDetTrackScoringToolsConf import InDet__InDetAmbiScoringTool
      #ptint
      ptintcut = InDetTrigSliceSettings[('pTmin',slice)]
      if slice=='minBias':
        ptintcut = 0.95*InDetTrigSliceSettings[('pTmin',slice)]
      elif slice=='minBias400':
        ptintcut = 0.95*InDetTrigSliceSettings[('pTmin',slice)]

      InDetTrigScoringTool = InDet__InDetAmbiScoringTool(name         = 'InDetTrigScoringTool_'+slice,
                                                         Extrapolator = InDetTrigExtrapolator,
                                                         SummaryTool  = InDetTrigTrackSummaryTool,
                                                         useAmbigFcn  = True,   # this is NewTracking
                                                         useTRT_AmbigFcn= False,
                                                         #to have a steeper turn-n curve
                                                         minPt        = ptintcut,
                                                         maxRPhiImp   = InDetTrigCutValues.maxPrimaryImpact(),
                                                         maxZImp      = InDetTrigCutValues.maxZImpact(),
                                                         maxEta       = InDetTrigCutValues.maxEta(),
                                                         minSiClusters= InDetTrigCutValues.minClusters(),
                                                         maxSiHoles   = InDetTrigCutValues.maxHoles(),
                                                         maxPixelHoles= InDetTrigCutValues.maxPixelHoles(),
                                                         maxSCTHoles  = InDetTrigCutValues.maxSCTHoles(),
                                                         maxDoubleHoles=InDetTrigCutValues.maxDoubleHoles(),
                                                         usePixel      = InDetTrigCutValues.usePixel(),
                                                         useSCT        = InDetTrigCutValues.useSCT(),
                                                         minTRTonTrk  = 0,    # no TRT here
                                                         DriftCircleCutTool = InDetTrigTRTDriftCircleCut,
                                                         doEmCaloSeed = False,
                                                         #BeamPositionSvc = default instance ,
                                                         )

      if slice=='beamgas':
        InDetTrigScoringTool.minPt          = EFIDTrackingCutsBeamGas.minPT()
        InDetTrigScoringTool.maxRPhiImp     = EFIDTrackingCutsBeamGas.maxPrimaryImpact()
        InDetTrigScoringTool.maxZImp        = EFIDTrackingCutsBeamGas.maxZImpact()
        InDetTrigScoringTool.minSiClusters  = EFIDTrackingCutsBeamGas.minClusters()
        InDetTrigScoringTool.maxSiHoles     = EFIDTrackingCutsBeamGas.maxHoles()
        InDetTrigScoringTool.useTRT_AmbigFcn= False
        InDetTrigScoringTool.useSigmaChi2   = True

      if slice=='FTK' or slice=='FTKRefit' or slice=='FTKMon':
        InDetTrigScoringTool.minSiClusters  = FTKTrackingCuts.minClusters()
        InDetTrigScoringTool.maxSiHoles     = FTKTrackingCuts.maxHoles()
        InDetTrigScoringTool.maxPixelHoles  = FTKTrackingCuts.maxPixelHoles()
        InDetTrigScoringTool.maxSCTHoles    = FTKTrackingCuts.maxSCTHoles()
        InDetTrigScoringTool.maxDoubleHoles = FTKTrackingCuts.maxDoubleHoles()
         
      #
      ToolSvc += InDetTrigScoringTool
      if (InDetTrigFlags.doPrintConfigurables()):
        print      InDetTrigScoringTool

      # load Ambiguity Processor
      from InDetTrigRecExample.InDetTrigConfigRecLoadTools import \
          InDetTrigAmbiTrackSelectionTool
      
      from TrkAmbiguityProcessor.TrkAmbiguityProcessorConf import Trk__SimpleAmbiguityProcessorTool

      InDetTrigAmbiguityProcessor = \
          Trk__SimpleAmbiguityProcessorTool(name = 'InDetTrigAmbiguityProcessor_'+slice,
                                            #AssoTool    = InDetTrigPrdAssociationTool,
                                            Fitter      = InDetTrigTrackFitter,
                                            SelectionTool = InDetTrigAmbiTrackSelectionTool,
                                            RefitPrds   = not InDetTrigFlags.refitROT()
                                            )

      InDetTrigAmbiguityProcessor.ScoringTool = InDet__InDetAmbiScoringTool('InDetTrigScoringTool_'+slice)

      if slice=='beamgas':
        from InDetTrigRecExample.InDetTrigConfigRecLoadToolsBeamGas import \
            InDetTrigAmbiTrackSelectionToolBeamGas
        InDetTrigAmbiguityProcessor.SelectionTool = InDetTrigAmbiTrackSelectionToolBeamGas

      elif slice=='cosmicsN':
        from InDetTrigRecExample.InDetTrigConfigRecLoadTools import InDetTrigTrackFitterCosmics
        from InDetTrigRecExample.InDetTrigConfigRecLoadToolsCosmics \
            import InDetTrigScoringToolCosmics_SiPattern, InDetTrigAmbiTrackSelectionToolCosmicsN

        InDetTrigAmbiguityProcessor.Fitter = InDetTrigTrackFitterCosmics
        InDetTrigAmbiguityProcessor.SuppressHoleSearch = False
        InDetTrigAmbiguityProcessor.SelectionTool = InDetTrigAmbiTrackSelectionToolCosmicsN
        InDetTrigAmbiguityProcessor.ScoringTool = InDetTrigScoringToolCosmics_SiPattern
        #InDetTrigAmbiguityProcessor.SuppressTrackFit = True
        #InDetTrigAmbiguityProcessor.ForceRefit = False
        InDetTrigAmbiguityProcessor.RefitPrds =  False
      elif slice=='electron' and InDetTrigFlags.doBremRecovery():
        InDetTrigAmbiguityProcessor.tryBremFit  = True
        import AthenaCommon.SystemOfUnits as Units
        InDetTrigAmbiguityProcessor.pTminBrem   = 5 * Units.GeV
      elif slice=='FTK' or slice=='FTKRefit'  or slice=='FTKMon':
        from TrigInDetConf.TrigInDetRecToolsFTK import InDetTrigAmbiTrackSelectionToolFTK,InDetTrigTrackFitterFTK
        InDetTrigAmbiguityProcessor.SelectionTool = InDetTrigAmbiTrackSelectionToolFTK
        InDetTrigAmbiguityProcessor.Fitter=InDetTrigTrackFitterFTK
        #InDetTrigAmbiguityProcessor.RefitPrds = 

      if InDetTrigFlags.materialInteractions() and InDetTrigFlags.solenoidOn():
         InDetTrigAmbiguityProcessor.MatEffects = 3
      else:
         InDetTrigAmbiguityProcessor.MatEffects = 0

      ToolSvc += InDetTrigAmbiguityProcessor
      if (InDetTrigFlags.doPrintConfigurables()):
         print InDetTrigAmbiguityProcessor

      self.AmbiguityProcessor = InDetTrigAmbiguityProcessor
      if lowPt:
        from InDetTrigRecExample.InDetTrigConfigRecLoadToolsLowPt import InDetTrigAmbiguityProcessorLowPt
        self.AmbiguityProcessor = InDetTrigAmbiguityProcessorLowPt
        self.OutputTracksLabel = "AmbigSolvLowPt"

      #use either SPSeeded or FTF tracks
      self.InputTracksLabel = ""

      #FIX
      #InDetTrigExtrapolator.ApplyMaterialEffects = False

      #monitoring
      from InDetTrigAmbiguitySolver.InDetTrigAmbiguitySolverMonitoring import InDetTrigAmbiguitySolverValidationMonitor
      from InDetTrigAmbiguitySolver.InDetTrigAmbiguitySolverMonitoring import InDetTrigAmbiguitySolverOnlineMonitor
      from TrigTimeMonitor.TrigTimeHistToolConfig import TrigTimeHistToolConfig
      ambtime = TrigTimeHistToolConfig("AmbTime")
      ambtime.TimerHistLimits = [0,100]
      if InDetTrigSliceSettings[('doFullScan',slice)]:
        ambtime.TimerHistLimits = [0,500]
      self.AthenaMonTools = [InDetTrigAmbiguitySolverValidationMonitor(),
                             InDetTrigAmbiguitySolverOnlineMonitor(),
                             ambtime]

      #self.ResolveTracks=False

# Track extension to TRT algorithm
#--------------------------------------------------------------------------
from TRT_TrigTrackExtensionAlg.TRT_TrigTrackExtensionAlgConf import InDet__TRT_TrigTrackExtensionAlg
class TRTTrackExtAlg_EF( InDet__TRT_TrigTrackExtensionAlg ):
   __slots__ = []
   def __init__(self, name="TRTTrackExtAlg_Electron_EF", type="electron"):
      super( InDet__TRT_TrigTrackExtensionAlg, self ).__init__( name )
      
      from AthenaCommon.AppMgr import ToolSvc
      from InDetTrigRecExample.InDetTrigConfigRecLoadTools import InDetTrigPatternPropagator, InDetTrigPatternUpdator
      
      # TRT detector elements road builder
      #
      from TRT_DetElementsRoadTool_xk.TRT_DetElementsRoadTool_xkConf import InDet__TRT_DetElementsRoadMaker_xk
      InDetTrigTRTDetElementsRoadMaker =  InDet__TRT_DetElementsRoadMaker_xk(name   = 'InDetTrigTRTRoadMaker',
                                                                             #DetectorStoreLocation = 'DetectorStore',
                                                                             TRTManagerLocation    = 'TRT',
                                                                             MagneticFieldMode     = 'MapSolenoid',
                                                                             PropagatorTool        = InDetTrigPatternPropagator)

      ToolSvc += InDetTrigTRTDetElementsRoadMaker
      if (InDetTrigFlags.doPrintConfigurables()):
         print      InDetTrigTRTDetElementsRoadMaker

      # Track extension to TRT tool
      #
      from AthenaCommon.DetFlags    import DetFlags
      if (DetFlags.haveRIO.TRT_on()):
         if InDetTrigFlags.trtExtensionType() is 'xk' :
         
            from TRT_TrackExtensionTool_xk.TRT_TrackExtensionTool_xkConf import InDet__TRT_TrackExtensionTool_xk
            from InDetTrigRecExample.InDetTrigConfigRecLoadTools import InDetTrigTRTDriftCircleCut
            InDetTrigTRTExtensionTool =  \
                InDet__TRT_TrackExtensionTool_xk(name   = 'InDetTrigTRTExtensionTool',
                                                 MagneticFieldMode     = 'MapSolenoid',      # default
                                                 TRT_ClustersContainer = 'TRT_TrigDriftCircles', # default
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
         elif InDetTrigFlags.trtExtensionType() is 'DAF' :
            # load TRT Competing ROT tool
            from TrkDeterministicAnnealingFilter.TrkDeterministicAnnealingFilterConf import Trk__DAF_SimpleWeightCalculator
            InDetTrigWeightCalculator =  Trk__DAF_SimpleWeightCalculator( name = 'InDetTrigWeightCalculator')
            
            ToolSvc += InDetTrigWeightCalculator
            if (InDetTrigFlags.doPrintConfigurables()):
               print      InDetTrigWeightCalculator
      
            from InDetCompetingRIOsOnTrackTool.InDetCompetingRIOsOnTrackToolConf import InDet__CompetingTRT_DriftCirclesOnTrackTool
            InDetTrigCompetingTRT_DC_Tool =  \
                InDet__CompetingTRT_DriftCirclesOnTrackTool( name     = 'InDetTrigCompetingTRT_DC_Tool',
                                                             MagneticFieldMode   = 'MapSolenoid',      # default
                                                             PropagatorTool          = InDetTrigPatternPropagator,
                                                             ToolForWeightCalculation= InDetTrigWeightCalculator,
                                                             ToolForTRT_DriftCircleOnTrackCreation = InDetTrigRotCreator.ToolTRT_DriftCircle)
            ToolSvc += InDetTrigCompetingTRT_DC_Tool
            if (InDetTrigFlags.doPrintConfigurables()):
               print      InDetTrigCompetingTRT_DC_Tool
      
            from TRT_TrackExtensionTool_DAF.TRT_TrackExtensionTool_DAFConf import InDet__TRT_TrackExtensionTool_DAF
            InDetTrigTRTExtensionTool =  \
                InDet__TRT_TrackExtensionTool_DAF(name  = 'InDetTrigTRTExtensionTool',
                                                  MagneticFieldMode        = 'MapSolenoid',      # default
                                                  TRT_DriftCircleContainer = 'TRT_TrigDriftCircles', # default
                                                  CompetingDriftCircleTool    = InDetTrigCompetingTRT_DC_Tool,
                                                  PropagatorTool           = InDetTrigPatternPropagator,
                                                  RoadTool                 = InDetTrigTRTDetElementsRoadMaker)
         #
         ToolSvc += InDetTrigTRTExtensionTool
         if (InDetTrigFlags.doPrintConfigurables()):
            print InDetTrigTRTExtensionTool

         self.TrackExtensionTool     = InDetTrigTRTExtensionTool

         #monitoring
         from TRT_TrigTrackExtensionAlg.TRT_TrigTrackExtensionAlgMonitoring import TRT_TrigTrackExtensionAlgValidationMonitor
         from TRT_TrigTrackExtensionAlg.TRT_TrigTrackExtensionAlgMonitoring import TRT_TrigTrackExtensionAlgOnlineMonitor
         from TrigTimeMonitor.TrigTimeHistToolConfig import TrigTimeHistToolConfig
         exttime = TrigTimeHistToolConfig("ExtTime")
         exttime.TimerHistLimits = [0,40]
         self.AthenaMonTools = [TRT_TrigTrackExtensionAlgValidationMonitor(),
                                TRT_TrigTrackExtensionAlgOnlineMonitor(),
                                exttime]
    

# ------------ Track Extension Processor
#
from InDetTrigExtensProcessor.InDetTrigExtensProcessorConf import InDet__InDetTrigExtensProcessor
class TrigExtProcessor_EF( InDet__InDetTrigExtensProcessor ):
  __slots__ = []
  def __init__(self, name="TrigExtProcessor_Electron_EF", type="electron"):
    super( InDet__InDetTrigExtensProcessor, self ).__init__( name )
      
    from AthenaCommon.AppMgr import ToolSvc
    from InDetTrigRecExample.InDetTrigConfigRecLoadTools import InDetTrigTrackFitter, InDetTrigPatternUpdator, InDetTrigExtrapolator, InDetTrigTrackSummaryTool

    if InDetTrigFlags.trtExtensionType() is 'DAF' :

      # DAF Fitter setup
      #
      from TrkCompetingRIOsOnTrackTool.TrkCompetingRIOsOnTrackToolConf import Trk__CompetingRIOsOnTrackTool
      InDetTrigCompetingRotCreator =  \
          Trk__CompetingRIOsOnTrackTool( name       = 'InDetTrigCompetingRotCreator',
                                         ToolForCompPixelClusters    = None,      # default
                                         ToolForCompSCT_Clusters     = None,      # default
                                         ToolForCompTRT_DriftCircles = InDetTrigCompetingTRT_DC_Tool )
      ToolSvc += InDetTrigCompetingRotCreator
      if (InDetTrigFlags.doPrintConfigurables()):
        print      InDetTrigCompetingRotCreator
      
      from TrkDeterministicAnnealingFilter.TrkDeterministicAnnealingFilterConf import Trk__DeterministicAnnealingFilter
      InDetTrigExtensionFitter = \
          Trk__DeterministicAnnealingFilter (name                 = 'InDetTrigDAF',
                                             ToolForExtrapolation = InDetTrigExtrapolator,
                                             ToolForCompetingROTsCreation   = InDetTrigCompetingRotCreator,
                                             ToolForUpdating                = InDetTrigPatternUpdator,
                                             AnnealingScheme                = [200., 81., 9., 4., 1., 1., 1.],
                                             DropOutlierCutValue            = 1.E-7,
                                             OutlierCutValue                = 0.01 )
                                                                             
      ToolSvc += InDetTrigExtensionFitter
      if (InDetTrigFlags.doPrintConfigurables()):
        print      InDetTrigExtensionFitter

    else:
      if type=="cosmicsN":
        from InDetTrigRecExample.InDetTrigConfigRecLoadTools import InDetTrigTrackFitterCosmics
        InDetTrigExtensionFitter = InDetTrigTrackFitterCosmics
      else:
        from InDetTrigRecExample.InDetTrigConfigRecLoadTools import InDetTrigTrackFitter
        InDetTrigExtensionFitter = InDetTrigTrackFitter

      
      ToolSvc += InDetTrigExtensionFitter
      if (InDetTrigFlags.doPrintConfigurables()):
        print      InDetTrigExtensionFitter

    # get configured track extension processor
    #-----------------------------------------------------------------------
    from InDetTrigRecExample.InDetTrigSliceSettings import InDetTrigSliceSettings
    from InDetTrigRecExample.InDetTrigConfigRecLoadTools import \
         InDetTrigTRTDriftCircleCut

    from InDetTrackScoringTools.InDetTrackScoringToolsConf import InDet__InDetAmbiScoringTool
    InDetTrigExtScoringTool = InDet__InDetAmbiScoringTool(name         = 'InDetTrigExtScoringTool_'+type,
                                                          Extrapolator = InDetTrigExtrapolator,
                                                          SummaryTool  = InDetTrigTrackSummaryTool,
                                                          useAmbigFcn  = True,     # this is NewTracking  
                                                          #minPt        = InDetTrigCutValues.minPT(), #see below
                                                          maxRPhiImp   = InDetTrigCutValues.maxPrimaryImpact(),
                                                          maxZImp      = InDetTrigCutValues.maxZImpact(),
                                                          maxEta       = InDetTrigCutValues.maxEta(),
                                                          minSiClusters= InDetTrigCutValues.minClusters(),
                                                          maxSiHoles   = InDetTrigCutValues.maxHoles(),
                                                          maxDoubleHoles=InDetTrigCutValues.maxDoubleHoles(),
                                                          usePixel      = InDetTrigCutValues.usePixel(),
                                                          useSCT        = InDetTrigCutValues.useSCT(),
                                                          doEmCaloSeed = False,
                                                          minTRTonTrk  = InDetTrigCutValues.minTRTonTrk(),
                                                          #useSigmaChi2   = False # tuning from Thijs
                                                          DriftCircleCutTool = InDetTrigTRTDriftCircleCut,
                                                          )

    InDetTrigExtScoringTool.minPt = InDetTrigSliceSettings[('pTmin',type)]

    #
    ToolSvc += InDetTrigExtScoringTool
    if (InDetTrigFlags.doPrintConfigurables()):
      print      InDetTrigExtScoringTool
      
    if type=="cosmicsN":
      from InDetTrigRecExample.InDetTrigConfigRecLoadToolsCosmics import InDetTrigTrackSummaryToolCosmics
      from InDetTrackScoringTools.InDetTrackScoringToolsConf import InDet__InDetCosmicScoringTool
      InDetTrigScoringToolCosmics_TRT= \
          InDet__InDetCosmicScoringTool(name = 'InDetTrigCosmicScoringTool_TRT',
                                        nWeightedClustersMin = 0,
                                        minTRTHits = EFIDTrackingCutsCosmics.minTRTonTrk(),
                                        SummaryTool= InDetTrigTrackSummaryToolCosmics
                                        )
      ToolSvc += InDetTrigScoringToolCosmics_TRT
      if (InDetTrigFlags.doPrintConfigurables()):
        print InDetTrigScoringToolCosmics_TRT

      
    self.TrackFitter  = InDetTrigExtensionFitter
    self.ScoringTool  = InDet__InDetAmbiScoringTool('InDetTrigExtScoringTool_'+type)
                                                          
    self.RefitPrds    = not (InDetTrigFlags.refitROT() or (InDetTrigFlags.trtExtensionType() is 'DAF'))
    self.suppressHoleSearch = False    # does not work properly      
    if type=="cosmicsN":
      self.ScoringTool = InDetTrigScoringToolCosmics_TRT
      self.Cosmics  = True

    #Material effects
    if InDetTrigFlags.materialInteractions():
      self.matEffects           = 3 # default in code is 4!!
    else:
      self.matEffects           = 0
    if type=="cosmicsN" and not InDetTrigFlags.solenoidOn():
      self.matEffects           = 0

    #monitoring
    from InDetTrigExtensProcessor.InDetTrigExtensProcessorMonitoring import InDetTrigExtensProcessorValidationMonitor
    from InDetTrigExtensProcessor.InDetTrigExtensProcessorMonitoring import InDetTrigExtensProcessorOnlineMonitor
    from TrigTimeMonitor.TrigTimeHistToolConfig import TrigTimeHistToolConfig
    idexttime = TrigTimeHistToolConfig("IdExtTime")
    idexttime.TimerHistLimits = [0,200]
    self.AthenaMonTools = [InDetTrigExtensProcessorValidationMonitor(),
                           InDetTrigExtensProcessorOnlineMonitor(),
                           idexttime]

         
# ------------ Track truth.
#
from InDetTrigTruthAlgs.InDetTrigTruthAlgsConf import InDet__InDetTrigDetailedTrackTruthMaker
class InDetTrigDetailedTrackTruthMaker_EF( InDet__InDetTrigDetailedTrackTruthMaker ):
  __slots__ = []
  def __init__(self, name="InDetTrigDetailedTrackTruthMaker_Electron_EF", type="electron"):
    super( InDet__InDetTrigDetailedTrackTruthMaker, self ).__init__( name )
    
    from InDetTrigRecExample.InDetTrigFlags import InDetTrigFlags
    self.doTruthAss = InDetTrigFlags.doTruth()
    
      #monitoring
    from InDetTrigTruthAlgs.InDetTrigDetailedTruthMonitoring import InDetTrigDetailedTruthValidationMonitor
    from InDetTrigTruthAlgs.InDetTrigDetailedTruthMonitoring import InDetTrigDetailedTruthOnlineMonitor
    from TrigTimeMonitor.TrigTimeHistToolConfig import TrigTimeHistToolConfig
    truthtime = TrigTimeHistToolConfig("TruthTime")
    truthtime.TimerHistLimits = [0,200]
    self.AthenaMonTools = [InDetTrigDetailedTruthValidationMonitor(),
                           truthtime]

# ------------ CollectionMerging (for 2-pass lowPt tracking)
#

from InDetTrigTrackCollectionMerger.InDetTrigTrackCollectionMergerConf import Trk__SimpleTrigTrackCollMerger
class SimpleTrigTrackCollMerger_EF( Trk__SimpleTrigTrackCollMerger ):
  __slots__ = []
  def __init__(self, name="SimpleTrigTrackCollMerger_MinBias_EF", type="minBias"):
    super( Trk__SimpleTrigTrackCollMerger, self ).__init__( name )
    
    from InDetTrigRecExample.InDetTrigConfigRecLoadTools import InDetTrigPrdAssociationTool,InDetTrigTrackSummaryToolSharedHits
    
    self.AssociationTool    = InDetTrigPrdAssociationTool
    self.SummaryTool        = InDetTrigTrackSummaryToolSharedHits
    self.InputCollections   = ["AmbigSolv", "AmbigSolvLowPt"]
    self.OutputCollection  = "CombinedLowPtTracksEFID"
  
