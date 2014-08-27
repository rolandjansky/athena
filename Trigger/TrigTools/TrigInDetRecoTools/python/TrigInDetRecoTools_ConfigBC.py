# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

from TrigInDetRecoTools.TrigInDetRecoToolsConf import TrigL2PattRecoStrategyBC
from TrigInDetRecoTools.ConfiguredSettings import SettingsForStrategyB
from TrigInDetRecoTools.ConfiguredSettings import SettingsForStrategyBC
from AthenaCommon.SystemOfUnits import *

from AthenaCommon.Logging import logging  
log = logging.getLogger("FactoryForStrategyBC")

class FactoryForStrategyBC() :
    __slots__=[]
    def __init__(self, name = 'FactoryForStrategyBC') :
        self.strategy = None
        self.settings = SettingsForStrategyB()
        self.settingsBC = SettingsForStrategyBC()
    
    def createStrategy(self, instName) :
        if instName in self.settings.allowedInstanceNames :
            self.strategy = TrigL2PattRecoStrategyBC("StrategyBC_"+instName)
            self.strategy.nUsedLayers = self.settingsBC[('nUsedLayers',instName)]

            ## Layer properties ##
            self.strategy.Map_LayerOneMultiplicity        = self.settings[('Map_LayerOneMultiplicity',instName)]
            self.strategy.Map_LayerTwoMultiplicity        = self.settings[('Map_LayerTwoMultiplicity',instName)]
            self.strategy.Map_MaxLayerFailureFraction     = 0.001
            self.strategy.Map_MaxTotalFailureFraction     = 0.00001
            
            ## New cut properties ##
            self.strategy.Reco_NewCuts                    = False
            self.strategy.Reco_NewCutsRZ                  = False
            self.strategy.Reco_PtMinSeedDiff              = 0.0
            self.strategy.Reco_D0MaxSeedDiff              = 2.0
            self.strategy.Reco_PtMinTripletDiff           = 0.0
            self.strategy.Reco_D0MaxTripletDiff           = 1.0
            self.strategy.Reco_PtMin                      = self.settings[('pTmin',instName)]
            self.strategy.Reco_D0Max                      = self.settings[('Reco_D0Max',instName)]
            self.strategy.Reco_TimingFactor               = 3.0
            self.strategy.Reco_AlignmentScaling           = 1.0
            self.strategy.Reco_OutlierRemoval             = False
            self.strategy.Reco_OutlierRemovalCut          = 0.9
            self.strategy.Reco_OutlierRemovalLayers       = 4
            self.strategy.Reco_OutlierRemovalD0           = 0.005
            self.strategy.Reco_OutlierRemovalPt           = 40
            self.strategy.Reco_MaxSeeds                   = 30000
            self.strategy.Reco_MaxTracks                  = 3000
            self.strategy.Reco_Z0Bin                      = 20
            self.strategy.Reco_EtaBin                     = 0.1

            self.strategy.maxFoundTracks                  = self.settingsBC[('maxFoundTracks',instName)]
            
            ## Preliminary vertexing properties ##
            self.strategy.PreVertexing_Enable             = False
            self.strategy.PreVertexing_VtxName            = "TrueTrigVertexColl"
            self.strategy.PreVertexing_ZCut               = 5
            
            ## Seeding properties ##             
            self.strategy.Seeding_PreCutRZ                = 10000
            self.strategy.Seeding_PreCutPhi               = 3.15
            self.strategy.Seeding_PreCutRPhi              = 0.001
            self.strategy.Seeding_CutRZ                   = 1.5
            
            ## Vertexing properties ##           
            self.strategy.Vertexing_Enable                = self.settings[('Vertexing_Enable',instName)]
            self.strategy.Vertexing_CutRZ                 = self.settings[('Vertexing_CutRZ',instName)]
            self.strategy.Vertexing_CutRPhi               = self.settings[('Vertexing_CutRPhi',instName)]
            self.strategy.Vertexing_MaxVertices           = 3
            self.strategy.Vertexing_WindowSize            = self.settings[('Vertexing_WindowSize',instName)]
            self.strategy.Vertexing_WindowSegments        = 4
            
            ## Extension properties ##
            self.strategy.Extension_PreCutRZ              = 10000
            self.strategy.Extension_PreCutPhi             = 3.15
            self.strategy.Extension_PreCutRPhi            = 0.0006
            self.strategy.Extension_PreCutRPhiDiff        = 0.0006
            self.strategy.Extension_CutRZPixel            = 0.8
            self.strategy.Extension_CutRZ                 = 4.5
            self.strategy.Extension_CutRPhi               = 0.0035
            self.strategy.Extension_CutSpacePoints        = 4
            self.strategy.Extension_CutSpacePointsMiss1   = 4
            self.strategy.Extension_CutSpacePointsMiss2   = 4
            self.strategy.Extension_SolveTriplets         = True
            self.strategy.Extension_SolveTripletsDRCut    = 9.0
            self.strategy.Extension_SolveTripletsPhiScale = 0.0006
            self.strategy.Extension_SolveTripletsEtaScale = 0.0022
            self.strategy.Extension_SolveTracks           = True
            self.strategy.Extension_SolveV0Tracks         = True
            self.strategy.Extension_SolveTracksDRCut      = 9.0
            self.strategy.Extension_SolveTracksPhiScale   = 0.0006
            self.strategy.Extension_SolveTracksEtaScale   = 0.0022
            self.strategy.Extension_SolveShared           = True
            self.strategy.Extension_MaxMissingLayers      = 20
            self.strategy.Extension_Refit                 = True
            self.strategy.Extension_SpacePoints           = self.settings[('Extension_SpacePoints',instName)]
            self.strategy.Extension_SpacePointsMiss1      = self.settings[('Extension_SpacePointsMiss1',instName)]
            self.strategy.Extension_SpacePointsMiss2      = self.settings[('Extension_SpacePointsMiss2',instName)]
            self.strategy.Extension_TRTExtension          = self.settings[('Extension_TRTExtension',instName)]
            
            ## Debug properties ##
            self.strategy.Debug_Tuning                    = False
            self.strategy.Debug_TuningGoodSeeds           = False
            self.strategy.Debug_TuningBadSeeds            = False
            self.strategy.Debug_TuningGoodTriplets        = False
            self.strategy.Debug_TuningBadTriplets         = False

            from AthenaCommon.DetFlags    import DetFlags
            from AthenaCommon.AppMgr import ToolSvc
            from InDetTrigRecExample.InDetTrigFlags import InDetTrigFlags
            from InDetTrigRecExample.ConfiguredNewTrackingTrigCuts import EFIDTrackingCuts
            InDetTrigCutValues = EFIDTrackingCuts

            # SCT and Pixel detector elements road builder
            from InDetTrigRecExample.InDetTrigConfigRecLoadTools import InDetTrigPixelConditionsSummarySvc, InDetTrigSCTConditionsSummarySvc
            from InDetTrigRecExample.InDetTrigConfigRecLoadTools import InDetTrigSiDetElementsRoadMaker
            InDetTrigSiDetElementsRoadMaker.RoadWidth = self.settingsBC[('RoadWidth',instName)]
            if DetFlags.haveRIO.pixel_on():
                from PixelConditionsServices.PixelConditionsServicesConf import PixelConditionsSummarySvc
                from InDetTrigRecExample.InDetTrigConditionsAccess import PixelConditionsSetup
                InDetTrigPixelConditionsSummarySvc = PixelConditionsSummarySvc(PixelConditionsSetup.instanceName('PixelConditionsSummarySvc'))
            else:
                InDetTrigPixelConditionsSummarySvc = None
            
            if DetFlags.haveRIO.SCT_on():
                from SCT_ConditionsServices.SCT_ConditionsServicesConf import SCT_ConditionsSummarySvc
                from InDetTrigRecExample.InDetTrigConditionsAccess import SCT_ConditionsSetup
                InDetTrigSCTConditionsSummarySvc = SCT_ConditionsSummarySvc(SCT_ConditionsSetup.instanceName('InDetSCT_ConditionsSummarySvc'))
            else:
                InDetTrigSCTConditionsSummarySvc = None

            from SiCombinatorialTrackFinderTool_xk.SiCombinatorialTrackFinderTool_xkConf import InDet__SiCombinatorialTrackFinder_xk
            from InDetTrigRecExample.InDetTrigConfigRecLoadTools import InDetTrigPrdAssociationTool, InDetTrigPatternPropagator
            from InDetTrigRecExample.InDetTrigConfigRecLoadTools import InDetTrigPatternUpdator, InDetTrigRotCreator

            TrackFinder_BC = InDet__SiCombinatorialTrackFinder_xk(name = 'InDetTrigSiComTrackFinder_BC_'+instName,
                                                                  PropagatorTool = InDetTrigPatternPropagator,
                                                                  UpdatorTool    = InDetTrigPatternUpdator,
                                                                  RIOonTrackTool   = InDetTrigRotCreator,
                                                                  AssosiationTool  = InDetTrigPrdAssociationTool,
                                                                  usePixel         = DetFlags.haveRIO.pixel_on(),
                                                                  useSCT           = DetFlags.haveRIO.SCT_on(),
                                                                  PixelClusterContainer = 'PixelClusterCache',
                                                                  SCT_ClusterContainer = 'SCT_ClusterCache',
                                                                  PixelSummarySvc = InDetTrigPixelConditionsSummarySvc,
                                                                  SctSummarySvc = InDetTrigSCTConditionsSummarySvc)

            ToolSvc += TrackFinder_BC

            from SiTrackMakerTool_xk.SiTrackMakerTool_xkConf import InDet__SiTrackMaker_xk

            TrackMaker_BC = InDet__SiTrackMaker_xk( name = 'InDetTrigSiTrackMaker_BC_'+instName,
                                                    RoadTool       = InDetTrigSiDetElementsRoadMaker,
                                                    CombinatorialTrackFinder = TrackFinder_BC,
                                                    pTmin          = self.settingsBC[('pTmin',instName)],
                                                    nClustersMin   = self.settingsBC[('nClustersMin',instName)],
                                                    nHolesMax      = self.settingsBC[('nHolesMax',instName)],
                                                    nHolesGapMax   = self.settingsBC[('nHolesGapMax',instName)],
                                                    #SeedsFilterLevel = InDetTrigCutValues.seedFilterLevel(),
                                                    SeedsFilterLevel = self.settingsBC[('seedsFilterLevel',instName)],
                                                    Xi2max         = self.settingsBC[('Xi2max',instName)],
                                                    Xi2maxNoAdd    = self.settingsBC[('Xi2maxNoAdd',instName)],
                                                    nWeightedClustersMin= self.settingsBC[('nWeightedClustersMin',instName)],
                                                    UseAssociationTool       = False)

            ToolSvc += TrackMaker_BC

            print TrackMaker_BC

            #self.strategy.SeedsTool = SeedMaker_C
            #self.strategy.ZvertexTool = ZvertexMaker_C

            self.strategy.TrackTool = TrackMaker_BC
             

            return self.strategy
        else :
            print "Instance "+instName+" of the Strategy B is not supported !"
            return None



