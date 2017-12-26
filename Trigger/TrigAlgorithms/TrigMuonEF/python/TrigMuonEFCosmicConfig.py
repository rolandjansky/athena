# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

#
# TrigMuonEF configurables
#
from TrigMuonEF.TrigMuonEFConf import *
from TriggerJobOpts.TriggerFlags  import TriggerFlags
from TrigMuonEF.TrigMuonEFMonitoring import *

# configure calorimeter for track extrapolation and matching
from AthenaCommon.AppMgr import ToolSvc
from AthenaCommon.AppMgr import ServiceMgr
from AthenaCommon.Constants import VERBOSE,DEBUG,ERROR
from AthenaCommon import CfgMgr, CfgGetter

from MuonRecExample import MuonRecTools
from MuonRecExample.MuonRecFlags import muonRecFlags
#from MuonRecExample.MuonRecUtils import AlgToolFactory,ServiceFactory,ConfiguredBase,getProperty

from TrigTimeMonitor.TrigTimeHistToolConfig import TrigTimeHistToolConfig

doT0Fit = True

# (private) storage of tools
#_toolFactory = AlgToolFactory("Tools for Muon Reconstruction",
#                              tryDefaultPublicTool=True,
#                              tryDefaultPrivateTool=True)
#getPublicTool  = _toolFactory.getPublicTool
#addPublicTool  = _toolFactory.addPublicTool

#from TrkDetDescrSvc.TrkDetDescrSvcConf import TrackingVolumesSvc
#ServiceMgr += TrackingVolumesSvc(BuildVolumesFromTagInfo = False)#
#
#from MuidCaloEnergyTools.MuidCaloEnergyToolsConf import Rec__MuidCaloEnergyMeas
#TMEFCaloEnergyMeas = Rec__MuidCaloEnergyMeas(name = 'TMEFCaloEnergyMeas', UseCaloNoiseTool = False)
#ToolSvc += TMEFCaloEnergyMeas###

#from MuidCaloEnergyTools.MuidCaloEnergyToolsConf import Rec__MuidCaloEnergyTool
#TMEFCaloEnergyTool = Rec__MuidCaloEnergyTool(name = 'TMEFCaloEnergyTool', CaloMeasTool = TMEFCaloEnergyMeas,
#                                             EnergyLossMeasurement = False, MopParametrization = True)
#ToolSvc += TMEFCaloEnergyTool#
#
#from MuidCaloScatteringTools.MuidCaloScatteringToolsConf import Rec__MuidCaloTrackStateOnSurface
#TMEFCaloTrackStateOnSurface = Rec__MuidCaloTrackStateOnSurface(name = 'TMEFCaloTrackStateOnSurface',
#                                                               CaloEnergyDeposit = TMEFCaloEnergyTool)
#ToolSvc += TMEFCaloTrackStateOnSurface#
#
#from MuidTrackBuilder.MuidTrackBuilderConf import Rec__CombinedMuonTrackBuilder
#TMEFCombinedTrackBuilderCosmic = Rec__CombinedMuonTrackBuilder(name = 'TMEFCombinedTrackBuilderCosmic',
#                                                               CaloTSOS = TMEFCaloTrackStateOnSurface,
#                                                               CaloEnergyParam = TMEFCaloEnergyTool,
#                                                               ReallocateMaterial = False
#                                                               )
#ToolSvc += TMEFCombinedTrackBuilderCosmic#
#


from MdtDriftCircleOnTrackCreator.MdtDriftCircleOnTrackCreatorConf import Muon__MdtDriftCircleOnTrackCreator
TMEF_MdtDriftCircleOnTrackCreatorCosmic = Muon__MdtDriftCircleOnTrackCreator("TMEF_MdtDriftCircleOnTrackCreatorCosmic",
                                                                             TimingMode = 1,
                                                                             DoFixedError = True,
                                                                             FixedError = 2.0,
                                                                             DoTofCorrection = False
                                                                             )
ToolSvc += TMEF_MdtDriftCircleOnTrackCreatorCosmic

TMEF_AdjustableT0Tool = CfgMgr.AdjT0__AdjustableT0Tool("AdjustableT0ToolTMEF", DoTof = 0 )
ToolSvc += TMEF_AdjustableT0Tool
TMEF_MdtDriftCircleOnTrackCreatorAdjustableT0Cosmic = Muon__MdtDriftCircleOnTrackCreator("TMEF_MdtDriftCircleOnTrackCreatorAdjustableT0Cosmic",
                                                                                         TimingMode = 3,
                                                                                         DoFixedError = True,
                                                                                         FixedError = 2.0,
                                                                                         DoTofCorrection = True,
                                                                                         MuonTofTool = TMEF_AdjustableT0Tool,
                                                                                         )
ToolSvc += TMEF_MdtDriftCircleOnTrackCreatorAdjustableT0Cosmic

from TrkRIO_OnTrackCreator.TrkRIO_OnTrackCreatorConf import Trk__RIO_OnTrackCreator
TMEF_MuonRotCreatorCosmic =  Trk__RIO_OnTrackCreator("TMEF_MuonRotCreatorCosmic",
                                                     ToolMuonDriftCircle = TMEF_MdtDriftCircleOnTrackCreatorCosmic,
                                                     ToolMuonCluster     = "MuonClusterOnTrackCreator"    ,
                                                     Mode                = 'muon'
                                                     )

ToolSvc +=TMEF_MuonRotCreatorCosmic

from MuonCombinePatternTools.MuonCombinePatternToolsConf import MuonCombinePatternTool
TMEF_MuonCombinePatternToolCosmic = MuonCombinePatternTool("TMEF_MuonCombinePatternToolCosmic",
                                                           UseCosmics = True
                                                           )
ToolSvc += TMEF_MuonCombinePatternToolCosmic

from MuonHoughPatternTools.MuonHoughPatternToolsConf import MuonHoughPatternTool
TMEF_MuonHoughPatternToolCosmic = MuonHoughPatternTool("TMEF_MuonHoughPatternToolCosmic",
                                                       UseCosmics = True,
                                                       NumberOfMaximaPerIterations = 1
                                                       )

ToolSvc += TMEF_MuonHoughPatternToolCosmic

from DCMathSegmentMaker.DCMathSegmentMakerConf import Muon__MdtMathSegmentFinder
TMEF_MdtMathSegmentFinderCosmic = Muon__MdtMathSegmentFinder("TMEF_MdtMathSegmentFinderCosmic",
                                                             AssociationRoadWidth = 15,
                                                             SortSegmentWithAllHits = True)
if doT0Fit:
    if muonRecFlags.doSegmentT0Fit():
        TMEF_MdtSegmentT0Fitter = MuonRecTools.getPublicTool( "MdtSegmentT0Fitter","TMEF_MdtSegmentT0Fitter" )
        TMEF_MdtMathSegmentFinderCosmic.DCFitProvider = TMEF_MdtSegmentT0Fitter

ToolSvc += TMEF_MdtMathSegmentFinderCosmic

from DCMathSegmentMaker.DCMathSegmentMakerConf import Muon__DCMathSegmentMaker
TMEF_DCMathSegmentMakerCosmic = Muon__DCMathSegmentMaker("TMEF_DCMathSegmentMakerCosmic",
                                                         SinAngleCut = 0.9,
                                                         AddUnassociatedPhiHits = True,
                                                         #SortSegmentWithAllHits = True,
                                                         CurvedErrorScaling = False,
                                                         MdtCreator = TMEF_MdtDriftCircleOnTrackCreatorCosmic,
                                                         MdtSegmentFinder = TMEF_MdtMathSegmentFinderCosmic
                                                         )

if doT0Fit:
    if muonRecFlags.doSegmentT0Fit():
        TMEF_DCMathSegmentMakerCosmic.MdtCreator = TMEF_MdtDriftCircleOnTrackCreatorAdjustableT0Cosmic
        TMEF_DCMathSegmentMakerCosmic.TofTool = TMEF_AdjustableT0Tool
        
ToolSvc += TMEF_DCMathSegmentMakerCosmic

from MuonPatternSegmentMaker.MuonPatternSegmentMakerConf import Muon__MuonPatternSegmentMaker
TMEF_MuonPatternSegmentMakerCosmic = Muon__MuonPatternSegmentMaker("TMEF_MuonPatternSegmentMakerCosmic",
                                                                   DropDistance = 100000000.,
                                                                   SegmentMaker = TMEF_DCMathSegmentMakerCosmic,
                                                                   MdtCreator = TMEF_MdtDriftCircleOnTrackCreatorCosmic
                                                                   )

ToolSvc += TMEF_MuonPatternSegmentMakerCosmic
#?
from DCMathSegmentMaker.DCMathSegmentMakerConf import Muon__DCMathSegmentMaker
TMEF_MCTBDCMathSegmentMakerCosmic = Muon__DCMathSegmentMaker("TMEF_MCTBDCMathSegmentMakerCosmic",
                                                             SinAngleCut = 0.9,
                                                             AddUnassociatedPhiHits = True,
                                                             DoGeometry = False,
                                                             MdtCreator = TMEF_MdtDriftCircleOnTrackCreatorCosmic,
                                                             MdtSegmentFinder = TMEF_MdtMathSegmentFinderCosmic
                                                             )
ToolSvc += TMEF_MCTBDCMathSegmentMakerCosmic


#from MuonCombiTrackMaker.MuonCombiTrackMakerConf import Muon__MuonSeededSegmentFinder
TMEF_MuonSeededSegmentFinderCosmic = CfgMgr.Muon__MuonSeededSegmentFinder( "TMEF_MuonSeededSegmentFinderCosmic",
                                                                    SegmentMaker = TMEF_MCTBDCMathSegmentMakerCosmic,
                                                                    SegmentMakerNoHoles = TMEF_DCMathSegmentMakerCosmic
                                                                    )
ToolSvc += TMEF_MuonSeededSegmentFinderCosmic



#from MuonSegmentCleaner.MuonSegmentCleanerConf import MuonPhiHitSelector
#TMEF_MuonPhiHitSelectorCosmic = MuonPhiHitSelector("TMEF_MuonPhiHitSelectorCosmic",
#                                                   DoCosmics = True,
#                                                   MakeClusters = True,
#                                                   CompetingRios = True
#                                                   )
#ToolSvc += TMEF_MuonPhiHitSelectorCosmic

#from MuonSegmentMomentum.MuonSegmentMomentumConf import MuonSegmentMomentum
#TMEF_MuonSegmentMomentumCosmic = MuonSegmentMomentum("TMEF_MuonSegmentMomentumCosmic",
#                                                     DoCosmics = True)
#ToolSvc += TMEF_MuonSegmentMomentumCosmic

#from MuonCurvedSegmentCombiner.MuonCurvedSegmentCombinerConf import Muon__MuonSegmentCombinationCleanerTool
#TMEF_MuonSegmentCombinationCleanerToolCosmic = Muon__MuonSegmentCombinationCleanerTool("TMEF_MuonSegmentCombinationCleanerToolCosmic",
#                                                                                       UpdatePhi = False,
#                                                                                       MuonHitSelector = TMEF_MuonPhiHitSelectorCosmic,
#                                                                                       MuonSegmentMomentum = TMEF_MuonSegmentMomentumCosmic,
#                                                                                       NoInner = True,
#                                                                                       DoMoore = False,
#                                                                                       DoCleaning = False)
#ToolSvc += TMEF_MuonSegmentCombinationCleanerToolCosmic
#MuonRecTools.addToolClone("MCTBExtrapolator","TMEF_MCTBExtrapolator",ResolveMuonStation=False)
MCTBExtrapolator = MuonRecTools.getPublicTool("MCTBExtrapolator")

from MuonRecExample.MuonRecTools import MuonChi2TrackFitter
TMEF_MCTBSLFitterCosmic = MuonChi2TrackFitter('TMEF_MCTBSLFitterCosmic',
                                              StraightLine      = True,
                                              Momentum          = 5000.,
                                              OutputLevel         = ERROR,
                                              RotCreatorTool = TMEF_MuonRotCreatorCosmic,
                                              ExtrapolationTool = MCTBExtrapolator,
                                              ExtrapolatorMaterial = True 
                                              )

ToolSvc += TMEF_MCTBSLFitterCosmic

TMEF_MCTBFitterCosmic = MuonChi2TrackFitter('TMEF_MCTBFitterCosmic',
                                            #StraightLine          = True,#toroinds off
                                            #OutlierCut            = 4.0,
                                            Momentum          = 0.,
                                            OutputLevel           = ERROR,
                                            ExtrapolationTool = MCTBExtrapolator,
                                            RotCreatorTool = TMEF_MuonRotCreatorCosmic,
                                            OutlierCut            = 3.0   ,
                                            NumericalDerivs       = False ,
                                            GetMaterialFromTrack  = True,
                                            ExtrapolatorMaterial = True 
                                            )

ToolSvc += TMEF_MCTBFitterCosmic

from TrkExSTEP_Propagator.TrkExSTEP_PropagatorConf import Trk__STEP_Propagator
TMEF_MuonPropagatorCosmic = Trk__STEP_Propagator("TMEF_MuonPropagatorCosmic",
                                                 MaterialEffects = True,
                                                 Tolerance  = 0.0001,
                                                 IncludeBgradients = False  
                                                 )
ToolSvc += TMEF_MuonPropagatorCosmic

#from MuonCombiTrackMaker.MuonCombiTrackMakerConf import Muon__MooTrackFitter
TMEF_MooTrackFitterCosmic = CfgMgr.Muon__MooTrackFitter("TMEF_MooTrackFitterCosmic",
                                                   SeedWithAvePhi = True,
                                                   SeedWithSegmentTheta = False,
                                                   Propagator=TMEF_MuonPropagatorCosmic,
                                                   SLFit = False,
                                                   ReducedChi2Cut = 20.,
                                                   Cosmics = True,   
                                                   Fitter = TMEF_MCTBFitterCosmic,
                                                   FitterPreFit = TMEF_MCTBFitterCosmic,
                                                   CleanPhiHits = False,
                                                   FitEtaStrips = True
                                                   )
ToolSvc += TMEF_MooTrackFitterCosmic

#from MuonCombiTrackMaker.MuonCombiTrackMakerConf import Muon__MooTrackFitter
TMEF_MooSLTrackFitterCosmic =  CfgMgr.Muon__MooTrackFitter("TMEF_MooSLTrackFitterCosmic",
                                                      Fitter = TMEF_MCTBSLFitterCosmic,
                                                      Cosmics = True,
                                                      FitterPreFit=TMEF_MCTBSLFitterCosmic,
                                                      Propagator="MuonStraightLinePropagator",
                                                      ReducedChi2Cut = 10.0,
                                                      SLFit=True,
                                                      CleanPhiHits = False,
                                                      SeedWithAvePhi = True,
                                                      SeedWithSegmentTheta = False
                                                      )

ToolSvc += TMEF_MooSLTrackFitterCosmic


#from MuonCombiTrackMaker.MuonCombiTrackMakerConf import Muon__MuonSegmentTrackBuilder
TMEF_MooTrackBuilderCosmic = CfgMgr.Muon__MooTrackBuilder("TMEF_MooTrackBuilderCosmic",
                                                                   SeededSegmentFinder = TMEF_MuonSeededSegmentFinderCosmic,
                                                                   SLFitter =  TMEF_MooSLTrackFitterCosmic,
                                                                   Fitter = TMEF_MooTrackFitterCosmic,
                                                                   MdtRotCreator = TMEF_MdtDriftCircleOnTrackCreatorCosmic
                                                                   )
ToolSvc += TMEF_MooTrackBuilderCosmic

#from MuonCombiTrackMaker.MuonCombiTrackMakerConf import Muon__MuonChamberHoleRecoveryTool
TMEF_MuonChamberHoleRecoveryToolCosmic = CfgMgr.Muon__MuonChamberHoleRecoveryTool("TMEF_MuonChamberHoleRecoveryToolCosmic",
                                                                           CscRotCreator = None,
                                                                           MdtRotCreator = TMEF_MdtDriftCircleOnTrackCreatorCosmic
                                                                           )
ToolSvc += TMEF_MuonChamberHoleRecoveryToolCosmic

#from MuonCurvedSegmentCombiner.MuonCurvedSegmentCombinerConf import Muon__MuonCurvedSegmentCombiner
TMEF_MuonCurvedSegmentCombinerCosmic =  CfgMgr.Muon__MuonCurvedSegmentCombiner("TMEF_MuonCurvedSegmentCombinerCosmic",
                                                                        MissedHitsCut = 100,
                                                                        AddAll2DCscs = False,
                                                                        UseCscSegments = False,
                                                                        DoCosmics = True
                                                                        )
ToolSvc += TMEF_MuonCurvedSegmentCombinerCosmic

TMEF_MCTBFitterTGMatCosmic = MuonChi2TrackFitter('TMEF_MCTBFitterTGMatCosmic',
                                                 #StraightLine          = True,#toroinds off
                                                 #OutlierCut            = 4.0,
                                                 Momentum              = 0.,
                                                 OutputLevel           = ERROR,
                                                 ExtrapolationTool = MCTBExtrapolator,
                                                 RotCreatorTool = TMEF_MuonRotCreatorCosmic,
                                                 GetMaterialFromTrack  = True,
                                                 ExtrapolatorMaterial = True,
                                                 OutlierCut            = 3.0,
                                                 NumericalDerivs       = False
                                                 )

# for backwards compat.
# In newer verions of Trk::GlobalChi2Fitter the MboyMat property has been removed
try:
    TMEF_MCTBFitterTGMatCosmic.MboyMat = False
except AttributeError:
    pass

ToolSvc += TMEF_MCTBFitterTGMatCosmic


#from MuonCombiTrackMaker.MuonCombiTrackMakerConf import Muon__MuonTrackScoringTool
TMEF_MuonTrackScoringToolCosmic = CfgMgr.Muon__MuonTrackScoringTool ("TMEF_MuonTrackScoringToolCosmic",
                                                              SumHelpTool = "MuonTrackSummaryTool")
ToolSvc += TMEF_MuonTrackScoringToolCosmic


#from MuonAmbiTrackSelectionTool.MuonAmbiTrackSelectionToolConf import  Muon__MuonAmbiTrackSelectionTool
TMEF_MuonAmbiTrackSelectionToolCosmic = CfgMgr.Muon__MuonAmbiTrackSelectionTool("TMEF_MuonAmbiTrackSelectionToolCosmic")
ToolSvc += TMEF_MuonAmbiTrackSelectionToolCosmic

#from TrkAmbiguityProcessor.TrkAmbiguityProcessorConf import Trk__SimpleAmbiguityProcessorTool
TMEF_MuonAmbiProcessorCosmic = CfgMgr.Trk__SimpleAmbiguityProcessorTool("TMEF_MuonAmbiProcessorCosmic",
                                                                 Fitter             = TMEF_MCTBFitterTGMatCosmic,
                                                                 ScoringTool        = TMEF_MuonTrackScoringToolCosmic,
                                                                 SuppressHoleSearch = True  ,
                                                                 SuppressTrackFit   = True  ,
                                                                 DropDouble         = False ,
                                                                 ForceRefit         = False ,
                                                                 SelectionTool      = TMEF_MuonAmbiTrackSelectionToolCosmic
                                                                 )
ToolSvc += TMEF_MuonAmbiProcessorCosmic
#from MuonCombiTrackMaker.MuonCombiTrackMakerConf import Muon__MCTBHitHandler
#TMEF_MCTBHitHandlerCosmic = Muon__MCTBHitHandler( "TMEF_MCTBHitHandlerCosmic",
#                                                  CscRotCreator = None,
#                                                  MdtRotCreator = "MdtTubeHitOnTrackCreator",
#                                                  OutputLevel = ERROR )
#ToolSvc +=TrigMuonEF_MCTBHitHandler


# START SUMMARY TOOL
# particle creation, Analysis::Muon building and CBNT filling need summary helpers
# indet and muon TrackSummaryHelper's: take existing public instances when available
from TrkTrackSummaryTool.TrkTrackSummaryToolConf import Trk__TrackSummaryTool
MuonTrackSummaryHelperTool    = MuonRecTools.getPublicTool("MuonTrackSummaryHelper")


from AthenaCommon.DetFlags import DetFlags

if not DetFlags.detdescr.ID_on():
    MuidTrackSummaryTool = Trk__TrackSummaryTool(
         name = "MuidTrackSummaryTool",
         MuonSummaryHelperTool  = MuonTrackSummaryHelperTool,
         doSharedHits           = False)    
else:
    from InDetTrigRecExample.InDetTrigConfigRecLoadTools import InDetTrigTrackSummaryHelperTool, InDetTrigHoleSearchTool
    from TrkTrackSummaryTool.TrkTrackSummaryToolConf import Trk__TrackSummaryTool
    MuidTrackSummaryTool = Trk__TrackSummaryTool(name ="MuidTrackSummaryTool",
                                                 InDetSummaryHelperTool = InDetTrigTrackSummaryHelperTool,
                                                 InDetHoleSearchTool = InDetTrigHoleSearchTool,
                                                 MuonSummaryHelperTool = MuonTrackSummaryHelperTool,
                                                 doSharedHits           = False)     

ToolSvc += MuidTrackSummaryTool
#END SUMMARY TOOL

# TrigMuonEF classes
class TrigMuonEFSegmentFinderCosmicConfig (TrigMuonEFSegmentFinder):
    __slots__ = ()

    def __init__( self, name="TrigMuonEFSegmentFinderCosmic" ):
        super( TrigMuonEFSegmentFinderCosmicConfig, self ).__init__( name )

        self.CscClusterProvider = CfgGetter.getPublicTool("CscThresholdClusterBuilderTool")

        from MuonRecExample.MooreTools import MooSegmentCombinationFinder        
        self.SegmentsFinderTool = MooSegmentCombinationFinder("SegmentsFinderToolCosmic")
        
        # use seeded decoding
        if (TriggerFlags.MuonSlice.doEFRoIDrivenAccess()):
            self.useMdtSeededDecoding = True
            self.useRpcSeededDecoding = True
            self.useTgcSeededDecoding = True
            self.useCscSeededDecoding = True
            
            # use ROB based seeded decoding instead of PRD based
            self.useMdtRobDecoding = True
            self.useRpcRobDecoding = True
            self.useTgcRobDecoding = False # neither available nor needed
            self.useCscRobDecoding = False # neither available nor needed
            

        self.SegmentsFinderTool.HoughPatternFinder.RecordAll = False
        #self.SegmentsFinderTool.HoughPatternFinder.OutputLevel = DEBUG
        self.SegmentsFinderTool.HoughPatternFinder.muonHoughPatternTool = TMEF_MuonHoughPatternToolCosmic
        self.SegmentsFinderTool.HoughPatternFinder.muonCombinePatternTool = TMEF_MuonCombinePatternToolCosmic
        self.SegmentsFinderTool.MdtSegmentMaker = TMEF_MuonPatternSegmentMakerCosmic
        self.SegmentsFinderTool.SegmentCombiner = TMEF_MuonCurvedSegmentCombinerCosmic
        #self.SegmentsFinderTool.SegmentCombinationCleaner = TMEF_MuonSegmentCombinationCleanerToolCosmic
        
        #self.SegmentsFinderTool.DoSummary = False
        #self.SegmentsFinderTool.SegmentCombiner.DoSummary = True
        #self.print_level = 5
        self.histoPathBase = ""	
        self.useRpcData=muonRecFlags.doRPCs()
        self.useTgcData=muonRecFlags.doTGCs()
        self.useCscData=muonRecFlags.doCSCs()
        # to select barrel(useMdtData=2), endcap(useMdtData=3)
        if muonRecFlags.doMDTs():
            self.useMdtData=1
        else:
            self.useMdtData=0

        self.deltaEtaRoI = 1.
        self.deltaPhiRoI = 1.
        self.useTGCInPriorNextBC = True
        
        validation_ms = TrigMuonEFSegmentFinderValidationMonitoring()
        online_ms     = TrigMuonEFSegmentFinderOnlineMonitoring()
        cosmic_ms     = TrigMuonEFSegmentFinderCosmicMonitoring()
        self.AthenaMonTools = [ validation_ms, online_ms, cosmic_ms]
        self.doMyTiming = False        
        time = TrigTimeHistToolConfig("Time")
        time.TimerHistLimits = [0, 10000]
        time.NumberOfHistBins = 1000
        self.AthenaMonTools += [time]
        self.doTimeOutChecks = False
        self.doTimeOutGuard  = False
        self.maxTgcHits      = 0
        self.maxCscHits      = 0
        self.maxRpcHits      = 0
        self.maxMdtHits      = 0
        self.doCache = True

class TrigMuonEFTrackBuilderCosmicConfig ():
    __slots__ = ()

    def __init__( self, name="TrigMuonEFTrackBuilderCosmic" ):

        raise RuntimeError("TrigMuonEFTrackBuilder no longer supported in release 19 (or later) due to removal of MooTrackSteering from the release.")

#class TrigMuonEFExtrapolatorCosmicConfig (TrigMuonEFExtrapolator):
#    __slots__ = ()
#
#    def __init__( self, name="TrigMuonEFExtrapolatorCosmic" ):
#        super( TrigMuonEFExtrapolatorConfig, self ).__init__( name )#
#
#        from MuidCombinedTools.MuidCombinedToolsConf import Rec__MuidBackTracker
#        TMEFBackTracker = Rec__MuidBackTracker(name = 'TMEFBackTracker',
#                                               TrackBuilder = TMEFCombinedTrackBuilderCosmic)
#        self.BackTrackerTool  = TMEFBackTracker
#        self.histoPathBase = ""	
#        validation_sa = TrigMuonEFExtrapolatorValidationMonitoring()
#        online_sa     = TrigMuonEFExtrapolatorOnlineMonitoring()

#        self.AthenaMonTools = [ validation_sa, online_sa ]

#class TrigMuonEFCombinerCosmicConfig (TrigMuonEFCombiner):
#    __slots__ = ()#
#
#    def __init__( self, name="TrigMuonEFCombinerCosmic" ):
#        super( TrigMuonEFCombinerConfig, self ).__init__( name )##
#
#        from MuidCombinedTools.MuidCombinedToolsConf import Rec__MuidMatchMaker
#        TMEFMatchMaker = Rec__MuidMatchMaker(name = 'TMEFMatchMaker',
#                                             CaloTSOS = TMEFCaloTrackStateOnSurface,
#                                             TrackBuilder = TMEFCombinedTrackBuilderCosmic)
#        self.MatchMakerTool  = TMEFMatchMaker
#        self.IdTracks = "InDetTrigTrackSlimmer_Muon_EFID"
#        self.histoPathBase = ""
#        validation_cb = TrigMuonEFCombinerValidationMonitoring()
#        online_cb     = TrigMuonEFCombinerOnlineMonitoring()#
#
#        self.AthenaMonTools = [ validation_cb, online_cb ]





