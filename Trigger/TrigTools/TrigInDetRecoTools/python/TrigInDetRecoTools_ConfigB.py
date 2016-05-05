# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

from TrigInDetRecoTools.TrigInDetRecoToolsConf import TrigL2PattRecoStrategyB
from TrigInDetRecoTools.ConfiguredSettings import SettingsForStrategyB

from AthenaCommon.Logging import logging  
log = logging.getLogger("FactoryForStrategyB")

class FactoryForStrategyB() :
    __slots__=[]
    def __init__(self, name = 'FactoryForStrategyB') :
        self.strategy = None
        self.settings = SettingsForStrategyB()

    def createStrategy(self, instName) :
        if instName in self.settings.allowedInstanceNames :
            self.strategy = TrigL2PattRecoStrategyB("StrategyB_"+instName)

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

            return self.strategy
        else :
            print "Instance "+instName+" of the Strategy B is not supported !"
            return None

