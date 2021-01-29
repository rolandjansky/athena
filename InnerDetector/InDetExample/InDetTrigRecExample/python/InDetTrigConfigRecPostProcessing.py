# Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration

from __future__ import print_function

# ------------------------------------------------------------
#
# ----------- now we do post-processing
#
# ------------------------------------------------------------
#
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
      print (     InDetTrkSlimmingTool)

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

      from InDetTrigRecExample.InDetTrigConfigRecLoadTools import ( # noqa: F401
          InDetTrigTrackSummaryHelperTool, InDetTrigTrackSummaryTool, InDetTrigTrackSummaryToolSharedHits,
          InDetTrigExtrapolator, InDetTrigPrdAssociationTool, InDetTrigHoleSearchTool )

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

      from InDetTrigRecExample.InDetTrigConfigRecLoadTools import (  # noqa: F401
          InDetTrigTrackSummaryHelperTool, InDetTrigTrackSummaryTool, InDetTrigTrackSummaryToolSharedHits,
          InDetTrigHoleSearchTool,InDetTrigExtrapolator )
      from TrigInDetConf.TrigInDetRecCommonTools import InDetTrigFastTrackSummaryTool  # noqa: F401


      # load patricle creator tool
      #


      from TrigInDetConf.TrigInDetPostTools import InDetTrigParticleCreatorToolWithSummary, \
          InDetTrigParticleCreatorToolFTF,InDetTrigParticleCreatorToolWithSummaryTRTPid

      creatorTool = InDetTrigParticleCreatorToolWithSummary
      self.doSharedHits=True
      from TrigInDetConf.TrigInDetRecCommonTools import InDetTrigPRDtoTrackMapExchangeTool
      self.PRDToTrackMapExchange = InDetTrigPRDtoTrackMapExchangeTool


      if "_FTF" in name:
        creatorTool = InDetTrigParticleCreatorToolFTF
      elif "_IDTrig" in name and type=="electron":
        creatorTool = InDetTrigParticleCreatorToolWithSummaryTRTPid

      self.ParticleCreatorTool = creatorTool

      if "_IDTrig" in name and (type=="muon" or type=="electron"):
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

      from InDetTrigRecExample.InDetTrigConfigRecLoadTools import (  # noqa: F401
          InDetTrigTrackSummaryHelperTool, InDetTrigTrackSummaryTool, InDetTrigTrackSummaryToolSharedHits,
          InDetTrigHoleSearchTool,InDetTrigExtrapolator )


# --- do truth for particle creation
# -------------------------------------------------------------------------------
from InDetTrigTruthAlgs.InDetTrigTruthAlgsConf import InDet__TrigTrackParticleTruthMaker
class InDetTrigTrackParticleTruthMaker_EF( InDet__TrigTrackParticleTruthMaker ):
   __slots__ = []
   def __init__(self, name="InDetTrigTrackParticleTruthMaker_Electron_EF", type="electron"):
      super( InDet__TrigTrackParticleTruthMaker, self ).__init__( name )

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


