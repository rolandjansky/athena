# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

# ------------------------------------------------------------
#
# ----------- now we do post-processing
#
# ------------------------------------------------------------
#
from AthenaCommon.AppMgr import ToolSvc
from AthenaCommon.Logging import logging 
log = logging.getLogger("EFID")


## track slimming
from InDetTrigTrackSlimmer.InDetTrigTrackSlimmerConf import InDet__TrigTrackSlimmer
class InDetTrigTrackSlimmer_EF( InDet__TrigTrackSlimmer ):
  __slots__ = []
  def __init__(self, name="InDetTrigTrackSlimmer_EF", type="electron"):
    super( InDet__TrigTrackSlimmer, self ).__init__( name )
    
    from AthenaCommon.AppMgr import ToolSvc
    from InDetTrigRecExample.InDetTrigFlags import InDetTrigFlags
    from InDetTrigRecExample.InDetTrigConfigRecLoadTools import InDetTrigTrackSummaryToolSharedHits

    TrkSlimmingName = "InDetTrigTrackSlimmingTool"
    keepPars=False
    keepOut=False
    if type=="photon" or type=="electron":
       TrkSlimmingName = "InDetTrigTrackSlimmingToolParams"
       keepPars = True
       keepOut=True
       
    #slimming tool
    from TrkTrackSlimmingTool.TrkTrackSlimmingToolConf import Trk__TrackSlimmingTool as ConfigurableTrackSlimmingTool
    InDetTrkSlimmingTool = \
        ConfigurableTrackSlimmingTool(name = TrkSlimmingName,
                                      KeepParameters = keepPars,
                                      KeepOutliers = keepOut )
    ToolSvc += InDetTrkSlimmingTool
    self.TrackSlimmingTool = InDetTrkSlimmingTool
    if (InDetTrigFlags.doPrintConfigurables()):
      print      InDetTrkSlimmingTool

    #optional update of the track summary
    self.SummaryTool = InDetTrigTrackSummaryToolSharedHits


#
# ------------ Vertex finding
#
 
from InDetTrigPriVxFinder.InDetTrigPriVxFinderConf import InDet__TrigVxPrimary
class TrigVxPrimary_EF( InDet__TrigVxPrimary ):
  __slots__ = []
  def __init__(self, name="TrigVxPrimary_Electron_EF", type="electron"):
    super( InDet__TrigVxPrimary, self ).__init__( name )

    from AthenaCommon.AppMgr import ToolSvc
    from AthenaCommon.GlobalFlags import GlobalFlags
    from InDetTrigRecExample.InDetTrigFlags import InDetTrigFlags

      
    from InDetTrigRecExample.InDetTrigConfigRecLoadToolsPost import InDetTrigPriVxFinderTool
    
    self.VertexFinderTool = InDetTrigPriVxFinderTool
    self.RunWithoutField = False
    
    #monitoring
    from InDetTrigPriVxFinder.InDetTrigPriVxFinderMonitoring import InDetTrigPriVxFinderValidationMonitor
    from InDetTrigPriVxFinder.InDetTrigPriVxFinderMonitoring import InDetTrigPriVxFinderOnlineMonitor
    from TrigTimeMonitor.TrigTimeHistToolConfig import TrigTimeHistToolConfig
    vxtime = TrigTimeHistToolConfig("VxTime")
    vxtime.TimerHistLimits = [0,100]
    from InDetTrigRecExample.InDetTrigSliceSettings import InDetTrigSliceSettings
    if InDetTrigSliceSettings[('doFullScan',type)]:
      vxtime.TimerHistLimits = [0,400]
    self.AthenaMonTools = [InDetTrigPriVxFinderValidationMonitor(),
                           InDetTrigPriVxFinderOnlineMonitor(),
                           vxtime]
      

#
# ------------ Vertex finding AllTE
#
 
from InDetTrigPriVxFinder.InDetTrigPriVxFinderConf import InDet__TrigVxPrimaryAllTE
class TrigVxPrimaryAllTE_EF( InDet__TrigVxPrimaryAllTE ):
  __slots__ = []
  def __init__(self, name="TrigVxPrimaryAllTE_Electron_EF", type="electron"):
    super( InDet__TrigVxPrimaryAllTE, self ).__init__( name )

    from AthenaCommon.AppMgr import ToolSvc
    from AthenaCommon.GlobalFlags import GlobalFlags
    from InDetTrigRecExample.InDetTrigFlags import InDetTrigFlags
    
    
    from InDetTrigRecExample.InDetTrigConfigRecLoadToolsPost import InDetTrigPriVxFinderTool
    
    self.VertexFinderTool = InDetTrigPriVxFinderTool
    self.RunWithoutField = False
    
      #monitoring
    from InDetTrigPriVxFinder.InDetTrigPriVxFinderMonitoring import InDetTrigPriVxFinderValidationMonitor
    from InDetTrigPriVxFinder.InDetTrigPriVxFinderMonitoring import InDetTrigPriVxFinderOnlineMonitor
    from TrigTimeMonitor.TrigTimeHistToolConfig import TrigTimeHistToolConfig
    vxtime = TrigTimeHistToolConfig("VxTime")
    vxtime.TimerHistLimits = [0,100]
    from InDetTrigRecExample.InDetTrigSliceSettings import InDetTrigSliceSettings
    if InDetTrigSliceSettings[('doFullScan',type)]:
      vxtime.TimerHistLimits = [0,400]
    self.AthenaMonTools = [InDetTrigPriVxFinderValidationMonitor(name="InDetTrigPriVxFinderAllTEOnlineMonitor"),
                           InDetTrigPriVxFinderOnlineMonitor(name="InDetTrigPriVxFinderAllTEValidationMonitor"),
                           vxtime]
      

from InDetTrigPriVxFinder.InDetTrigPriVxFinderConf import InDet__TrigVxPrimaryAllTE
class TrigVxPrimaryAllTESG_EF( InDet__TrigVxPrimaryAllTE ):
  __slots__ = []
  def __init__(self, name="TrigVxPrimaryAllTESG_Electron_EF", type="electron"):
    super( InDet__TrigVxPrimaryAllTE, self ).__init__( name )

    from AthenaCommon.AppMgr import ToolSvc
    from AthenaCommon.GlobalFlags import GlobalFlags
    from InDetTrigRecExample.InDetTrigFlags import InDetTrigFlags
    import AthenaCommon.SystemOfUnits as Units

    from InDetTrigRecExample.InDetTrigConfigRecLoadTools import InDetTrigExtrapolator,InDetTrigTrackSelectorTool

    from InDetTrigRecExample.InDetTrigConfigRecLoadToolsPost import InDetTrigLinFactory  
    from InDetMultipleVertexSeedFinderUtils.InDetMultipleVertexSeedFinderUtilsConf import InDet__InDetTrackZ0SortingTool
    InDetTrackZ0SortingTool =  InDet__InDetTrackZ0SortingTool(name = "InDetTrigTrackZ0SortingTool_SG")
    ToolSvc += InDetTrackZ0SortingTool

    from TrkVertexSeedFinderUtils.TrkVertexSeedFinderUtilsConf import Trk__Trk2dDistanceSeeder
    Trk2dDistanceSeeder = Trk__Trk2dDistanceSeeder(name                 = "TrigTrk2dDistanceSeederFor2D_SG",
                                                   SolveAmbiguityUsingZ = False)
    ToolSvc+=Trk2dDistanceSeeder

    from TrkVertexSeedFinderUtils.TrkVertexSeedFinderUtilsConf import Trk__Trk2DDistanceFinder
    Trk2DDistanceFinder = Trk__Trk2DDistanceFinder(name                = "TrigTrk2DDistanceFinder_SG",
                                                   Trk2dDistanceSeeder = Trk2dDistanceSeeder)
    
    ToolSvc+=Trk2DDistanceFinder

    from TrkVertexSeedFinderTools.TrkVertexSeedFinderToolsConf import Trk__CrossDistancesSeedFinder
    InDet2DVtxSeedFinder = Trk__CrossDistancesSeedFinder(name                = "InDetTrig2DCrossDistancesSeedFinder_SG",
                                                         TrkDistanceFinder   = Trk2DDistanceFinder,
                                                         trackdistcutoff     = 1.,
                                                         trackdistexppower   = 2,
                                                         useweights          = True
                                                         #Mode1dFinder = # default, no setting needed
                                                         )
    ToolSvc+=InDet2DVtxSeedFinder

    from TrkVertexBilloirTools.TrkVertexBilloirToolsConf import Trk__FastVertexFitter
    InDetVxFitterTool = Trk__FastVertexFitter(name                   = "InDetTrigFastVertexFitterTool_SG",
                                              LinearizedTrackFactory = InDetTrigLinFactory,
                                              Extrapolator           = InDetTrigExtrapolator)
    ToolSvc += InDetVxFitterTool


    from InDetMultipleVertexSeedFinder.InDetMultipleVertexSeedFinderConf import InDet__SlidingWindowMultiSeedFinder
    InDetMultiSeedFinder = InDet__SlidingWindowMultiSeedFinder(name          = "InDetTrigSlidingWindowMultiSeedFinder_SG",
                                                              clusterLength = 5.*Units.mm,
                                                              TrackSelector = InDetTrigTrackSelectorTool,
                                                              Extrapolator  = InDetTrigExtrapolator,
                                                              SortingTool   = InDetTrackZ0SortingTool,
                                                              #UseMaxInCluster = True
                                                              )
    ToolSvc += InDetMultiSeedFinder

    from InDetPriVxFinderTool.InDetPriVxFinderToolConf import InDet__InDetPriVxFinderTool
    InDetTrigPriVxFinderTool = InDet__InDetPriVxFinderTool(name              = "InDetTrigPriVxFinderTool_SG",
                                                       PriVxSeedFinder   = InDetMultiSeedFinder,
                                                       TrackSelector     = InDetTrigTrackSelectorTool,
                                                       VertexFitterTool  = InDetVxFitterTool,
                                                       #maxChi2PerTrack   = InDetPrimaryVertexingCuts.MaxChi2PerTrack(),
                                                       #chi2CutMethod     = InDetPrimaryVertexingCuts.chi2CutMethod(),
                                                       enableMultipleVertices = True,
                                                       useBeamConstraint = True)

    ToolSvc += InDetTrigPriVxFinderTool
  
    self.VertexFinderTool = InDetTrigPriVxFinderTool
    self.RunWithoutField = False
    self.RetrieveTracksFromSG = True
    self.TrackCollectionFromSG = "FTKConv_Trk_Tracks";
  
      #monitoring
    from InDetTrigPriVxFinder.InDetTrigPriVxFinderMonitoring import InDetTrigPriVxFinderValidationMonitor
    from InDetTrigPriVxFinder.InDetTrigPriVxFinderMonitoring import InDetTrigPriVxFinderOnlineMonitor
    from TrigTimeMonitor.TrigTimeHistToolConfig import TrigTimeHistToolConfig
    vxtime = TrigTimeHistToolConfig("VxTime")
    vxtime.TimerHistLimits = [0,100]
    from InDetTrigRecExample.InDetTrigSliceSettings import InDetTrigSliceSettings
    if InDetTrigSliceSettings[('doFullScan',type)]:
      vxtime.TimerHistLimits = [0,400]
    self.AthenaMonTools = [InDetTrigPriVxFinderValidationMonitor(name="InDetTrigPriVxFinderAllTEOnlineMonitor"),
                           InDetTrigPriVxFinderOnlineMonitor(name="InDetTrigPriVxFinderAllTEValidationMonitor"),
                           vxtime]
      
      
   
# ------------ Particle creation
#
from InDetTrigParticleCreation.InDetTrigParticleCreationConf import InDet__TrigParticleCreator
class InDetTrigParticleCreation_EF( InDet__TrigParticleCreator ):
   __slots__ = []
   def __init__(self, name="InDetTrigParticleCreation_Electron_EF", type="electron"):
      super( InDet__TrigParticleCreator, self ).__init__( name )

      from AthenaCommon.AppMgr import ToolSvc
      from InDetTrigRecExample.InDetTrigFlags import InDetTrigFlags
      from InDetTrigRecExample.InDetTrigConfigRecLoadTools import \
          InDetTrigTrackSummaryHelperTool, InDetTrigTrackSummaryTool, InDetTrigTrackSummaryToolSharedHits, \
          InDetTrigExtrapolator, InDetTrigPrdAssociationTool, InDetTrigHoleSearchTool
      
      InDetTrigPartCreaPrdAssociationTool     = None
      InDetTrigPartCreaTrackSummaryHelperTool = InDetTrigTrackSummaryHelperTool
      

      from TrigInDetConf.TrigInDetPostTools import InDetTrigParticleCreatorTool,InDetTrigParticleCreatorToolParams

      creatorTool = InDetTrigParticleCreatorTool
      if type=="photon":
        creatorTool = InDetTrigParticleCreatorToolParams

      self.ParticleCreatorTool = creatorTool
      self.PRDAssociationTool = InDetTrigPrdAssociationTool #for b-tagging

      #monitoring
      self.MonSliceSpecific = True
      from InDetTrigRecExample.InDetTrigSliceSettings import InDetTrigSliceSettings
      self.MonPrescale    = InDetTrigSliceSettings[('monPS',type)]
      self.MonPtmin       = InDetTrigSliceSettings[('monPtMin',type)]
      from InDetTrigParticleCreation.InDetTrigParticleCreationMonitoring import \
          InDetTrigParticleCreationOnlineMonitor, InDetTrigParticleCreationValidationMonitor
      from TrigTimeMonitor.TrigTimeHistToolConfig import TrigTimeHistToolConfig
      pctime = TrigTimeHistToolConfig("PcTime")
      pctime.TimerHistLimits = [0,40]
      self.AthenaMonTools = [InDetTrigParticleCreationValidationMonitor(type=type),
                             InDetTrigParticleCreationOnlineMonitor(type=type),
                             pctime]


# ------------ xAOD TrackParticle creation
#
from InDetTrigParticleCreation.InDetTrigParticleCreationConf import InDet__TrigTrackingxAODCnv
class InDetTrigTrackingxAODCnv_EF( InDet__TrigTrackingxAODCnv ):
   __slots__ = []
   def __init__(self, name="InDetTrigTrackingxAODCnv_Electron_EF", type="electron"):
      super( InDet__TrigTrackingxAODCnv, self ).__init__( name )

      from AthenaCommon.AppMgr import ToolSvc
      from InDetTrigRecExample.InDetTrigFlags import InDetTrigFlags
      from InDetTrigRecExample.InDetTrigConfigRecLoadTools import \
          InDetTrigTrackSummaryHelperTool, InDetTrigTrackSummaryTool, InDetTrigTrackSummaryToolSharedHits, \
          InDetTrigHoleSearchTool,InDetTrigExtrapolator
      from TrigInDetConf.TrigInDetRecCommonTools import InDetTrigFastTrackSummaryTool

         
      # load patricle creator tool
      #


      from TrigInDetConf.TrigInDetPostTools import InDetTrigParticleCreatorToolWithSummary, \
          InDetTrigParticleCreatorToolFTF,InDetTrigParticleCreatorToolWithSummaryTRTPid

      creatorTool = InDetTrigParticleCreatorToolWithSummary

      if "_FTF" in name or "_FTK" in name:
        creatorTool = InDetTrigParticleCreatorToolFTF
      elif "_IDTrig" in name and type=="electron":
        creatorTool = InDetTrigParticleCreatorToolWithSummaryTRTPid
        
      self.ParticleCreatorTool = creatorTool

      if "_IDTrig" in name and (type=="muon" or type=="electron"):
        self.doIBLresidual = True

      #for FTK commissioning
      if "_FTK" in name:
        self.doIBLresidual = True

      #monitoring
      self.MonSliceSpecific = True
      from InDetTrigRecExample.InDetTrigSliceSettings import InDetTrigSliceSettings
      self.MonPrescale    = InDetTrigSliceSettings[('monPS',type)]
      self.MonPtmin       = InDetTrigSliceSettings[('monPtMin',type)]
      from InDetTrigParticleCreation.InDetTrigTrackingxAODCnvMonitoring import \
          InDetTrigTrackingxAODCnvOnlineMonitor, InDetTrigTrackingxAODCnvValidationMonitor
      from TrigTimeMonitor.TrigTimeHistToolConfig import TrigTimeHistToolConfig
      pctime = TrigTimeHistToolConfig("PcTime")
      pctime.TimerHistLimits = [0,40]
      self.AthenaMonTools = [InDetTrigTrackingxAODCnvValidationMonitor(type=type),
                             InDetTrigTrackingxAODCnvOnlineMonitor(type=type),
                             pctime]

# ------------- Vertex AOD to xAOD conversion
#
from InDetTrigParticleCreation.InDetTrigParticleCreationConf import InDet__TrigVertexxAODCnv
class InDetTrigVertexxAODCnv_EF( InDet__TrigVertexxAODCnv ):
   __slots__ = []
   def __init__(self, name="InDetTrigVertexxAODCnv_Electron_EF", type="electron"):
      super( InDet__TrigVertexxAODCnv, self ).__init__( name )

      from AthenaCommon.AppMgr import ToolSvc
      from InDetTrigRecExample.InDetTrigFlags import InDetTrigFlags
      from InDetTrigRecExample.InDetTrigConfigRecLoadTools import \
          InDetTrigTrackSummaryHelperTool, InDetTrigTrackSummaryTool, InDetTrigTrackSummaryToolSharedHits, \
          InDetTrigHoleSearchTool,InDetTrigExtrapolator
      
         
# --- do truth for particle creation
# ------------------------------------------------------------------------------- 
from InDetTrigTruthAlgs.InDetTrigTruthAlgsConf import InDet__TrigTrackParticleTruthMaker
class InDetTrigTrackParticleTruthMaker_EF( InDet__TrigTrackParticleTruthMaker ):
   __slots__ = []
   def __init__(self, name="InDetTrigTrackParticleTruthMaker_Electron_EF", type="electron"):
      super( InDet__TrigTrackParticleTruthMaker, self ).__init__( name )

      from AthenaCommon.AppMgr import ToolSvc
      from InDetTrigRecExample.InDetTrigFlags import InDetTrigFlags

      self.doTruthAss = InDetTrigFlags.doTruth()

      #monitoring
      #from InDetTrigTruthAlgs.TrigTrackParticleTruthMonitoring import \
      #    TrigTrackParticleTruthValidationMonitor, TrigTrackParticleTruthOnlineMonitor
      #from TrigTimeMonitor.TrigTimeHistToolConfig import TrigTimeHistToolConfig
      #trutime = TrigTimeHistToolConfig("TruTime")
      #trutime.TimerHistLimits = [0,200]
      #self.AthenaMonTools = [TrigTrackParticleTruthValidationMonitor(),
      #                       trutime]
         

# ------------ Conversion Finder
#
from InDetTrigConversionFinder.InDetTrigConversionFinderConf import InDet__TrigConversionFinder
class InDetTrigConversionFinder_EF( InDet__TrigConversionFinder ):
   __slots__ = []
   def __init__(self, name="InDetTrigConversionFinder_Electron_EFID", type="electron"):
      super( InDet__TrigConversionFinder, self ).__init__( name )

      from AthenaCommon.AppMgr import ToolSvc
      from InDetTrigRecExample.InDetTrigFlags import InDetTrigFlags
      from InDetTrigRecExample.InDetTrigSliceSettings import InDetTrigSliceSettings
      from InDetTrigRecExample.InDetTrigConfigRecLoadTools import \
          InDetTrigExtrapolator, InDetTrigTrackSummaryTool

      #
      from TrkVKalVrtFitter.TrkVKalVrtFitterConf import Trk__TrkVKalVrtFitter
      InDetTrigConversionVxFitterTool = Trk__TrkVKalVrtFitter(name                = "InDetTrigConversionVxFitter",
                                                              Extrapolator        = InDetTrigExtrapolator,
                                                              IterationNumber     = 30,
                                                              MakeExtendedVertex  = True,
                                                              FirstMeasuredPoint  = True,
#                                                              MagFieldSvc         = InDetTrigMagField,
                                                              Robustness          = 6,
                                                              usePhiCnst          = True,
	                                                      useThetaCnst        = True,
                                                              InputParticleMasses = [0.511,0.511],
                                                              VertexForConstraint = [0.,0.,0.],
                                                              CovVrtForConstraint = [0.015*0.015,0.,0.015*0.015,0.,0.,10000.*10000.])
      ToolSvc += InDetTrigConversionVxFitterTool 
      if (InDetTrigFlags.doPrintConfigurables()):
         print      InDetTrigConversionVxFitterTool

      # Distance of minimum approach utility
      #
      from TrkVertexSeedFinderUtils.TrkVertexSeedFinderUtilsConf import Trk__SeedNewtonTrkDistanceFinder
      InDetTrigConversionTrkDistanceFinder = Trk__SeedNewtonTrkDistanceFinder(name = 'InDetTrigConversionTrkDistanceFinder')

      ToolSvc += InDetTrigConversionTrkDistanceFinder 
      if (InDetTrigFlags.doPrintConfigurables()):
         print      InDetTrigConversionTrkDistanceFinder

      # Helper Tool
      #
      from InDetConversionFinderTools.InDetConversionFinderToolsConf import InDet__ConversionFinderUtils
      InDetTrigConversionHelper = InDet__ConversionFinderUtils(name = "InDetTrigConversionFinderUtils",
                                                               #OutputLevel                = 1
                                                               )
      
      ToolSvc += InDetTrigConversionHelper
      if (InDetTrigFlags.doPrintConfigurables()):
         print      InDetTrigConversionHelper

      # Track selector tool
      #
      from InDetTrackSelectorTool.InDetTrackSelectorToolConf import InDet__InDetConversionTrackSelectorTool
      InDetTrigConversionTrackSelector = InDet__InDetConversionTrackSelectorTool(name   = "InDetTrigConversionTrackSelector",
                                                                                 TrackSummaryTool = InDetTrigTrackSummaryTool,
                                                                                 Extrapolator     = InDetTrigExtrapolator, 
                                                                                 maxSiD0          = 10000.,  #50.0,
	                                                                         maxTrtD0         = 10000.,  #100.,
	                                                                         maxSiZ0          = 10000.,  #350.0,
	                                                                         maxTrtZ0         = 10000.,  #1400.,
	                                                                         minPt            = InDetTrigSliceSettings[('pTmin',type)],
	                                                                         RatioCut1        = 0.0,     #0.5,
	                                                                         RatioCut2        = 0.1,
	                                                                         RatioCut3        = 0.1)
      
      ToolSvc += InDetTrigConversionTrackSelector
      if (InDetTrigFlags.doPrintConfigurables()):
         print      InDetTrigConversionTrackSelector

      # Track pairs selector
      #
      from InDetConversionFinderTools.InDetConversionFinderToolsConf import InDet__TrackPairsSelector
      InDetTrigConversionTrackPairsSelector = InDet__TrackPairsSelector(name   = "InDetTrigConversionTrackPairsSelector",
                                                                        ConversionFinderHelperTool = InDetTrigConversionHelper,
                                                                        DistanceTool               = InDetTrigConversionTrkDistanceFinder,
                                                                        MaxFirstHitRadius          = 500.,
                                                                        #OutputLevel                = 1,
									MaxDistBetweenTracks       = [10.,50.,50.],      #[6.,80.,30.],
									MaxEta                     = [0.3,0.5,0.5]       #[0.5,1.0,0.5]
                                                                        )
      ToolSvc += InDetTrigConversionTrackPairsSelector
      if (InDetTrigFlags.doPrintConfigurables()):
         print      InDetTrigConversionTrackPairsSelector
         
      # Vertex point estimator
      #
      from InDetConversionFinderTools.InDetConversionFinderToolsConf import InDet__VertexPointEstimator
      InDetTrigConversionVtxPointEstimator = InDet__VertexPointEstimator(name    = "InDetTrigConversionVtxPointEstimator",
                                                                         MinDeltaR              = [-5.,-25.,-50.],
                                                                         MaxDeltaR              = [5.,10.,10.],
                                                                         MaxPhi                 = [0.05, 0.5, 0.5]) #[0.05, 0.1, 0.1])


      ToolSvc += InDetTrigConversionVtxPointEstimator
      if (InDetTrigFlags.doPrintConfigurables()):
         print      InDetTrigConversionVtxPointEstimator
         
      # Conversion post selector
      #
      from InDetConversionFinderTools.InDetConversionFinderToolsConf import InDet__ConversionPostSelector
      InDetTrigConversionPostSelector = InDet__ConversionPostSelector(name = "InDetTrigConversionPostSelector",
                                                                      MaxChi2Vtx       = [50.,50.,50.],              #[40.,100.,80.],
	                                                              MaxInvariantMass = [10000.,10000.,10000.],     #[60.,60.,30.],
	                                                              MinFitMomentum   = [0.,0.,0.],                 #[2000.,2000.,2000.],
	                                                              MinRadius        = [-10000.,-10000.,-10000.],  #[30.,35.,250.],
	                                                              MinPt            = 0.,
	                                                              MaxdR            = -10000.,                    #-250.,
	                                                              MaxPhiVtxTrk     = 10000.)  #0.2)

      ToolSvc += InDetTrigConversionPostSelector
      if (InDetTrigFlags.doPrintConfigurables()):
         print      InDetTrigConversionPostSelector

      # Single track conversion tool
      #
      from InDetConversionFinderTools.InDetConversionFinderToolsConf import InDet__SingleTrackConversionTool
      InDetTrigSingleTrackConversion = InDet__SingleTrackConversionTool(name       = "InDetTrigSingleTrackConversionTool",
                                                                        ConversionFinderHelperTool = InDetTrigConversionHelper,
                                                                        TrackSummaryTool           = InDetTrigTrackSummaryTool,
									Extrapolator               = InDetTrigExtrapolator,                                              
                          					        MinInitialHitRadius        = 70.,
                                                                        MinRatioOfHLhits           = 0.95)
      ToolSvc += InDetTrigSingleTrackConversion
      if (InDetTrigFlags.doPrintConfigurables()):
         print      InDetTrigSingleTrackConversion
         
      from InDetConversionFinderTools.InDetConversionFinderToolsConf import InDet__InDetConversionFinderTools
      InDetTrigConversionFinderTools = InDet__InDetConversionFinderTools(name        = "InDetTrigConversionFinderTools",
                                                                         VertexFitterTool           = InDetTrigConversionVxFitterTool,
                                                                         TrackSelectorTool          = InDetTrigConversionTrackSelector,
                                                                         TrackPairsSelector         = InDetTrigConversionTrackPairsSelector,
                                                                         ConversionFinderHelperTool = InDetTrigConversionHelper,
                                                                         VertexPointEstimator       = InDetTrigConversionVtxPointEstimator,
                                                                         PostSelector               = InDetTrigConversionPostSelector,
                                                                         SingleTrackConversionTool  = InDetTrigSingleTrackConversion,
                                                                         Extrapolator               = InDetTrigExtrapolator,
                                                                         RemoveTrtTracks            = False,
                                                                         #OutputLevel                = 1,
                                                                         IsConversion               = True)
      ToolSvc += InDetTrigConversionFinderTools
      if (InDetTrigFlags.doPrintConfigurables()):
         print      InDetTrigConversionFinderTools

      self.VertexFinderTool          = InDetTrigConversionFinderTools

      #monitoring
      from InDetTrigConversionFinder.InDetTrigConversionFinderMonitoring import InDetTrigConversionFinderValidationMonitor
      from InDetTrigConversionFinder.InDetTrigConversionFinderMonitoring import InDetTrigConversionFinderOnlineMonitor
      from TrigTimeMonitor.TrigTimeHistToolConfig import TrigTimeHistToolConfig
      convtime = TrigTimeHistToolConfig("ConvVxTime")
      convtime.TimerHistLimits = [0,200]
      self.AthenaMonTools = [InDetTrigConversionFinderValidationMonitor(),
                             InDetTrigConversionFinderOnlineMonitor(),
                             convtime]


