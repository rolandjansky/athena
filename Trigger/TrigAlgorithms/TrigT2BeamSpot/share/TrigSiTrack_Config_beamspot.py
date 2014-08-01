from TrigSiTrack.TrigSiTrackConf import TrigSiTrack
from TrigOnlineSpacePointTool.TrigOnlineSpacePointToolConf import OnlineSpacePointProviderTool
from TrigTRT_TrackExtensionTool.TrigTRT_TrackExtensionToolConf import TrigTRT_TrackExtensionTool
from TrigInDetTrackFitter.TrigInDetTrackFitterConf import TrigInDetTrackFitter
from TrigTimeMonitor.TrigTimeHistToolConfig import TrigTimeHistToolConfig

from TrigMonitorBase.TrigGenericMonitoringToolConfig import defineHistogram, TrigGenericMonitoringToolConfig

from __main__ import ToolSvc

ospTool = OnlineSpacePointProviderTool(UseStoreGate=False)
ToolSvc += ospTool


#Monitoring
class TrigSiTrack_OnlineMonitoring(TrigGenericMonitoringToolConfig):
    def __init__ (self, name="TrigSiTrack_OnlineMonitoring"):
        super(TrigSiTrack_OnlineMonitoring, self).__init__(name)
        self.defineTarget("Online")
        self.Histograms += [ defineHistogram('ChiSqd',
                                             type='TH1F',
                                             title="ChiSqd",
                                             xbins = 100, xmin=0.0, xmax=5)]
        self.Histograms += [ defineHistogram('pt',
                                             type='TH1F',
                                             title="pt",
                                             xbins = 100, xmin=-40000, xmax=40000)]
        self.Histograms += [ defineHistogram('a0',
                                             type='TH1F',
                                             title="a0",
                                             xbins = 100, xmin=-1.5, xmax=1.5)]
        self.Histograms += [ defineHistogram('z0',
                                             type='TH1F',
                                             title="z0",
                                             xbins = 100, xmin=-200, xmax=200)]
        self.Histograms += [ defineHistogram('phi',
                                             type='TH1F',
                                             title="phi",
                                             xbins = 100, xmin=-3.2, xmax=3.2)]
        self.Histograms += [ defineHistogram('eta',
                                             type='TH1F',
                                             title="eta",
                                             xbins = 100, xmin=-5, xmax=5)]
        self.Histograms += [ defineHistogram('NHit_Si',
                                             type='TH1F',
                                             title="NHit_Si",
                                             xbins = 10, xmin=0, xmax=10)]
        self.Histograms += [ defineHistogram('numtracks',
                                             type='TH1F',
                                             title="Number of Tracks",
                                             xbins = 100, xmin=0., xmax=100.)]

        self.Histograms += [ defineHistogram('TRTDataPrepTime',
                                             type='TH1F',
                                             title="TRT DataPrep total time",
                                             xbins = 250, xmin=0., xmax=100.)]
        self.Histograms += [ defineHistogram('TRTBStoDCTime',
                                             type='TH1F',
                                             title="BS-to-DriftCircles TRT",
                                             xbins = 250, xmin=0., xmax=100.)]
        self.Histograms += [ defineHistogram('TRTRobProvTime',
                                             type='TH1F',
                                             title="RobDataProvider time TRT",
                                             xbins = 250 , xmin=0., xmax=100.)]

        self.Histograms += [ defineHistogram('TRTDataTotalTime',
                                             type='TH1F',
                                             title="TRT DataProvider Time",
                                             xbins = 250, xmin=0., xmax=100.)]
        self.Histograms += [ defineHistogram('TRTRobPreloadTime',
                                             type='TH1F',
                                             title="TRT RobPreloader Time",
                                             xbins = 250, xmin=0., xmax=100.)]
        self.Histograms += [ defineHistogram('TRTRoadsTime',
                                             type='TH1F',
                                             title="TRT Roads",
                                             xbins = 250 , xmin=0., xmax=100.)]
        self.Histograms += [ defineHistogram('TRTPDAFTime',
                                             type='TH1F',
                                             title="TRT PDAF Tracking",
                                             xbins = 250 , xmin=0., xmax=100.)]
        self.Histograms += [ defineHistogram('SiDataRegSelTime',
                                             type='TH1F',
                                             title="RegSel Time Pixel and SCT",
                                             xbins = 250 , xmin=0., xmax=50.)]
        self.Histograms += [ defineHistogram('SiDataTotalTime',
                                             type='TH1F',
                                             title="SiDataPreparation Total",
                                             xbins = 250 , xmin=0., xmax=50.)]
                
        self.Histograms += [ defineHistogram('PixSPformationTime',
                                             type='TH1F',
                                             title="SP formation time Pixel",
                                             xbins = 250 , xmin=0., xmax=50.)]
        self.Histograms += [ defineHistogram('SCTSPformationTime',
                                             type='TH1F',
                                             title="SP formation time SCT",
                                             xbins = 250 , xmin=0., xmax=50.)]
        self.Histograms += [ defineHistogram('PixBStoClustersTime',
                                             type='TH1F',
                                             title="BS-to-Clusters Pixel",
                                             xbins = 250 , xmin=0., xmax=50.)]
        self.Histograms += [ defineHistogram('SCTBStoClustersTime',
                                             type='TH1F',
                                             title="BS-to-Clusters SCT",
                                             xbins = 250 , xmin=0., xmax=50.)]
        
class TrigSiTrack_ValidationMonitoring(TrigGenericMonitoringToolConfig):
    def __init__ (self, name="TrigSiTrack_ValidationMonitoring"):
        super(TrigSiTrack_ValidationMonitoring, self).__init__(name)
        self.defineTarget("Validation")
        self.Histograms += [ defineHistogram('ChiSqd',
                                             type='TH1F',
                                             title="ChiSqd",
                                             xbins = 100, xmin=0.0, xmax=5)]
        self.Histograms += [ defineHistogram('pt',
                                             type='TH1F',
                                             title="pt",
                                             xbins = 100, xmin=-40000, xmax=40000)]
        self.Histograms += [ defineHistogram('a0',
                                             type='TH1F',
                                             title="a0",
                                             xbins = 100, xmin=-1.5, xmax=1.5)]
        self.Histograms += [ defineHistogram('z0',
                                             type='TH1F',
                                             title="z0",
                                             xbins = 100, xmin=-200, xmax=200)]
        self.Histograms += [ defineHistogram('phi',
                                             type='TH1F',
                                             title="phi",
                                             xbins = 100, xmin=-3.2, xmax=3.2)]
        self.Histograms += [ defineHistogram('eta',
                                             type='TH1F',
                                             title="eta",
                                             xbins = 100, xmin=-5, xmax=5)]
        self.Histograms += [ defineHistogram('NHit_Si',
                                             type='TH1F',
                                             title="NHit_Si",
                                             xbins = 10, xmin=0, xmax=10)]
        self.Histograms += [ defineHistogram('numtracks',
                                             type='TH1F',
                                             title="Number of Tracks",
                                             xbins = 100, xmin=0., xmax=100.)]

        self.Histograms += [ defineHistogram('TRTDataPrepTime',
                                             type='TH1F',
                                             title="TRT DataPrep total time",
                                             xbins = 250, xmin=0., xmax=100.)]
        self.Histograms += [ defineHistogram('TRTBStoDCTime',
                                             type='TH1F',
                                             title="BS-to-DriftCircles TRT",
                                             xbins = 250, xmin=0., xmax=100.)]
        self.Histograms += [ defineHistogram('TRTRobProvTime',
                                             type='TH1F',
                                             title="RobDataProvider time TRT",
                                             xbins = 250 , xmin=0., xmax=100.)]

        self.Histograms += [ defineHistogram('TRTDataTotalTime',
                                             type='TH1F',
                                             title="TRT DataProvider Time",
                                             xbins = 250, xmin=0., xmax=100.)]
        self.Histograms += [ defineHistogram('TRTRobPreloadTime',
                                             type='TH1F',
                                             title="TRT RobPreloader Time",
                                             xbins = 250, xmin=0., xmax=100.)]
        self.Histograms += [ defineHistogram('TRTRoadsTime',
                                             type='TH1F',
                                             title="TRT Roads",
                                             xbins = 250 , xmin=0., xmax=100.)]
        self.Histograms += [ defineHistogram('TRTPDAFTime',
                                             type='TH1F',
                                             title="TRT PDAF Tracking",
                                             xbins = 250 , xmin=0., xmax=100.)]
        self.Histograms += [ defineHistogram('SiDataRegSelTime',
                                             type='TH1F',
                                             title="RegSel Time Pixel and SCT",
                                             xbins = 250 , xmin=0., xmax=50.)]
        self.Histograms += [ defineHistogram('SiDataTotalTime',
                                             type='TH1F',
                                             title="SiDataPreparation Total",
                                             xbins = 250 , xmin=0., xmax=50.)]
                
        self.Histograms += [ defineHistogram('PixSPformationTime',
                                             type='TH1F',
                                             title="SP formation time Pixel",
                                             xbins = 250 , xmin=0., xmax=50.)]
        self.Histograms += [ defineHistogram('SCTSPformationTime',
                                             type='TH1F',
                                             title="SP formation time SCT",
                                             xbins = 250 , xmin=0., xmax=50.)]
        self.Histograms += [ defineHistogram('PixBStoClustersTime',
                                             type='TH1F',
                                             title="BS-to-Clusters Pixel",
                                             xbins = 250 , xmin=0., xmax=50.)]
        self.Histograms += [ defineHistogram('SCTBStoClustersTime',
                                             type='TH1F',
                                             title="BS-to-Clusters SCT",
                                             xbins = 250 , xmin=0., xmax=50.)]
        
#Cosmic Monitoring
class TrigSiTrack_Cosmic_Monitoring(TrigGenericMonitoringToolConfig):
    def __init__ (self, name="TrigSiTrack_Cosmic_Monitoring"):
        super(TrigSiTrack_Cosmic_Monitoring, self).__init__(name)
        self.defineTarget("Cosmic")
        self.Histograms += [ defineHistogram('ChiSqd',
                                             type='TH1F',
                                             title="ChiSqd",
                                             xbins = 100, xmin=0.0, xmax=5)]
        self.Histograms += [ defineHistogram('NStraw_TRT',
                                             type='TH1F',
                                             title="NStraw_TRT",
                                             xbins = 100, xmin=0.0, xmax=100)]
        self.Histograms += [ defineHistogram('NTR_TRT',
                                             type='TH1F',
                                             title="NTR_TRT",
                                             xbins = 100, xmin=0.0, xmax=100)]
        self.Histograms += [ defineHistogram('pt',
                                             type='TH1F',
                                             title="pt",
                                             xbins = 100, xmin=0.0, xmax=40000)]
        self.Histograms += [ defineHistogram('a0',
                                             type='TH1F',
                                             title="a0",
                                             xbins = 100, xmin=-300, xmax=300)]
        self.Histograms += [ defineHistogram('z0',
                                             type='TH1F',
                                             title="z0",
                                             xbins = 100, xmin=-800, xmax=800)]
        self.Histograms += [ defineHistogram('phi',
                                             type='TH1F',
                                             title="phi",
                                             xbins = 100, xmin=-3.2, xmax=3.2)]
        self.Histograms += [ defineHistogram('eta',
                                             type='TH1F',
                                             title="eta",
                                             xbins = 100, xmin=-5, xmax=5)]
        self.Histograms += [ defineHistogram('NHit_Si',
                                             type='TH1F',
                                             title="NHit_Si",
                                             xbins = 10, xmin=0, xmax=10)]
        self.Histograms += [ defineHistogram('numtracks',
                                             type='TH1F',
                                             title="Number of Tracks",
                                             xbins = 100, xmin=0., xmax=100.)]        
        
        self.Histograms += [ defineHistogram('TRTDataPrepTime',
                                             type='TH1F',
                                             title="TRT DataPrep total time",
                                             xbins = 250, xmin=0., xmax=100.)]
        self.Histograms += [ defineHistogram('TRTBStoDCTime',
                                             type='TH1F',
                                             title="BS-to-DriftCircles TRT",
                                             xbins = 250, xmin=0., xmax=100.)]
        self.Histograms += [ defineHistogram('TRTRobProvTime',
                                             type='TH1F',
                                             title="RobDataProvider time TRT",
                                             xbins = 250 , xmin=0., xmax=100.)]

        self.Histograms += [ defineHistogram('TRTDataTotalTime',
                                             type='TH1F',
                                             title="TRT DataProvider Time",
                                             xbins = 250, xmin=0., xmax=100.)]
        self.Histograms += [ defineHistogram('TRTRobPreloadTime',
                                             type='TH1F',
                                             title="TRT RobPreloader Time",
                                             xbins = 250, xmin=0., xmax=100.)]
        self.Histograms += [ defineHistogram('TRTRoadsTime',
                                             type='TH1F',
                                             title="TRT Roads",
                                             xbins = 250 , xmin=0., xmax=100.)]
        self.Histograms += [ defineHistogram('TRTPDAFTime',
                                             type='TH1F',
                                             title="TRT PDAF Tracking",
                                             xbins = 250 , xmin=0., xmax=100.)]
        self.Histograms += [ defineHistogram('SiDataRegSelTime',
                                             type='TH1F',
                                             title="RegSel Time Pixel and SCT",
                                             xbins = 250 , xmin=0., xmax=50.)]
        self.Histograms += [ defineHistogram('SiDataTotalTime',
                                             type='TH1F',
                                             title="SiDataPreparation Total",
                                             xbins = 250 , xmin=0., xmax=50.)]
                
        self.Histograms += [ defineHistogram('PixSPformationTime',
                                             type='TH1F',
                                             title="SP formation time Pixel",
                                             xbins = 250 , xmin=0., xmax=50.)]
        self.Histograms += [ defineHistogram('SCTSPformationTime',
                                             type='TH1F',
                                             title="SP formation time SCT",
                                             xbins = 250 , xmin=0., xmax=50.)]
        self.Histograms += [ defineHistogram('PixBStoClustersTime',
                                             type='TH1F',
                                             title="BS-to-Clusters Pixel",
                                             xbins = 250 , xmin=0., xmax=50.)]
        self.Histograms += [ defineHistogram('SCTBStoClustersTime',
                                             type='TH1F',
                                             title="BS-to-Clusters SCT",
                                             xbins = 250 , xmin=0., xmax=50.)]

class TrigSiTrackFlags:
    def __init__(self):

        self.LoosenCuts = False

TrigSiTrackFlags = TrigSiTrackFlags()

class TrigSiTrackLoosenCuts:
    def __init__(self):
                
        self.Seeding_PreCutRZ            = 10.0
        self.Seeding_PreCutRPhi          = 0.005
        self.Seeding_CutRZ               = 1.5
            
        self.Extension_PreCutRZ          = 10.0
        self.Extension_PreCutRPhi        = 0.03
        self.Extension_PreCutRPhiDiff    = 0.03
        self.Extension_CutRZ             = 4.0
        self.Extension_CutRPhi           = 0.7

TrigSiTrackLoosenCuts = TrigSiTrackLoosenCuts()

class TrigSiTrack_eGamma(TrigSiTrack):
    __slots__ = []
    def __init__(self, name = "TrigSiTrack_eGamma"):
        TrigSiTrack.__init__(self, name)

        from __main__ import ToolSvc
        
        trtTool = TrigTRT_TrackExtensionTool(OfflineClusters=False)
        ToolSvc += trtTool
        fitTool = TrigInDetTrackFitter(OfflineClusters=False)
        ToolSvc += fitTool
        
        self.Tool_SpacePointProviderTool  = ospTool
        self.Tool_TRTExtensionTool        = trtTool
        self.Tool_ReFitterTool            = fitTool
        
        self.Map_Layer1                   = [0,1,7]
        self.Map_Layer2                   = [1,2,7,8]
        self.Map_Layer3                   = [2,8,9]
        self.Map_Layer4                   = [3,10,11]
        self.Map_Layer5                   = [4,12,13]
        self.Map_Layer6                   = [5,14,15]
        self.Map_Layer7                   = [6,16,17,18]
        self.Map_ExtraEtaLayer1           = 1.95
        self.Map_ExtraEtaLayer2           = 2.15
        self.Map_ExtraEtaLayer3           = 2.35
        
        self.RoI_HalfWidthPhi             = 0.1
        self.RoI_HalfWidthEta             = 0.1

        if TrigSiTrackFlags.LoosenCuts:

            self.Seeding_PreCutRZ         = TrigSiTrackLoosenCuts.Seeding_PreCutRZ
            self.Seeding_PreCutRPhi       = TrigSiTrackLoosenCuts.Seeding_PreCutRPhi
            self.Seeding_CutRZ            = TrigSiTrackLoosenCuts.Seeding_CutRZ
        
        else:

            self.Seeding_PreCutRZ         = 10.0
            self.Seeding_PreCutRPhi       = 0.0001
            self.Seeding_CutRZ            = 1.5
        
        self.Vertexing_Enable             = False
        self.Vertexing_CutRZ              = 200.0

        if TrigSiTrackFlags.LoosenCuts:
            
            self.Extension_PreCutRZ       = TrigSiTrackLoosenCuts.Extension_PreCutRZ
            self.Extension_PreCutRPhi     = TrigSiTrackLoosenCuts.Extension_PreCutRPhi
            self.Extension_PreCutRPhiDiff = TrigSiTrackLoosenCuts.Extension_PreCutRPhiDiff
            self.Extension_CutRZ          = TrigSiTrackLoosenCuts.Extension_CutRZ
            self.Extension_CutRPhi        = TrigSiTrackLoosenCuts.Extension_CutRPhi

        else:

            self.Extension_PreCutRZ       = 10.0
            self.Extension_PreCutRPhi     = 0.0001
            self.Extension_PreCutRPhiDiff = 0.0001
            self.Extension_CutRZ          = 4.0
            self.Extension_CutRPhi        = 0.002
            
        self.Extension_CutSpacePoints     = 2
        self.Extension_MergeSeeds         = True
        self.Extension_MergeSeedsCut      = 20.0
        self.Extension_SolveTracks        = True
        self.Extension_SolveTracksCut     = 20.0
        self.Extension_Refit              = True
        self.Extension_TRTExtension       = True
        
# monitoring histograms
        from TrigTimeMonitor.TrigTimeHistToolConfig import TrigTimeHistToolConfig
        time = TrigTimeHistToolConfig("TimeHistogramForTrigSiTrack")
        time.TimerHistLimits = [0,50]
                        
        self.AthenaMonTools = [ TrigSiTrack_ValidationMonitoring(), TrigSiTrack_OnlineMonitoring(),
                                time ]
        
class TrigSiTrack_Muon(TrigSiTrack):
    __slots__ = []
    def __init__(self, name = "TrigSiTrack_Muon"):
        TrigSiTrack.__init__(self, name)

        from __main__ import ToolSvc

        trtTool = TrigTRT_TrackExtensionTool(OfflineClusters=False)
        ToolSvc += trtTool
        fitTool = TrigInDetTrackFitter(OfflineClusters=False)
        ToolSvc += fitTool

        self.Tool_SpacePointProviderTool  = ospTool
        self.Tool_TRTExtensionTool        = trtTool
        self.Tool_ReFitterTool            = fitTool

        self.Map_Layer1                   = [0,1,7]
        self.Map_Layer2                   = [1,2,7,8]
        self.Map_Layer3                   = [2,8,9]
        self.Map_Layer4                   = [3,10,11]
        self.Map_Layer5                   = [4,12,13]
        self.Map_Layer6                   = [5,14,15]
        self.Map_Layer7                   = [6,16,17,18]
        self.Map_ExtraEtaLayer1           = 1.95
        self.Map_ExtraEtaLayer2           = 2.15
        self.Map_ExtraEtaLayer3           = 2.35
        
        self.RoI_HalfWidthPhi             = 0.1
        self.RoI_HalfWidthEta             = 0.1

        if TrigSiTrackFlags.LoosenCuts:

            self.Seeding_PreCutRZ         = TrigSiTrackLoosenCuts.Seeding_PreCutRZ
            self.Seeding_PreCutRPhi       = TrigSiTrackLoosenCuts.Seeding_PreCutRPhi
            self.Seeding_CutRZ            = TrigSiTrackLoosenCuts.Seeding_CutRZ
        
        else:
            
            self.Seeding_PreCutRZ         = 10.0
            self.Seeding_PreCutRPhi       = 0.0001
            self.Seeding_CutRZ            = 1.5

        self.Vertexing_Enable             = False
        self.Vertexing_CutRZ              = 200.0

        if TrigSiTrackFlags.LoosenCuts:
            
            self.Extension_PreCutRZ       = TrigSiTrackLoosenCuts.Extension_PreCutRZ
            self.Extension_PreCutRPhi     = TrigSiTrackLoosenCuts.Extension_PreCutRPhi
            self.Extension_PreCutRPhiDiff = TrigSiTrackLoosenCuts.Extension_PreCutRPhiDiff
            self.Extension_CutRZ          = TrigSiTrackLoosenCuts.Extension_CutRZ
            self.Extension_CutRPhi        = TrigSiTrackLoosenCuts.Extension_CutRPhi

        else:
            
            self.Extension_PreCutRZ       = 10.0
            self.Extension_PreCutRPhi     = 0.0001
            self.Extension_PreCutRPhiDiff = 0.0001
            self.Extension_CutRZ          = 4.0
            self.Extension_CutRPhi        = 0.002

        self.Extension_CutSpacePoints     = 2
        self.Extension_MergeSeeds         = True
        self.Extension_MergeSeedsCut      = 20.0
        self.Extension_SolveTracks        = True
        self.Extension_SolveTracksCut     = 20.0
        self.Extension_SpacePoints        = 4
        self.Extension_Refit              = True
        self.Extension_TRTExtension       = True
        
# monitoring histograms
        from TrigTimeMonitor.TrigTimeHistToolConfig import TrigTimeHistToolConfig
        time = TrigTimeHistToolConfig("TimeHistogramForTrigSiTrack")
        time.TimerHistLimits = [0,50]
                        
        self.AthenaMonTools = [ TrigSiTrack_ValidationMonitoring(), TrigSiTrack_OnlineMonitoring(),
                                time ]

        
class TrigSiTrack_Tau(TrigSiTrack):
    __slots__ = []
    def __init__(self, name = "TrigSiTrack_Tau"):
        TrigSiTrack.__init__(self, name)

        from __main__ import ToolSvc

        trtTool = TrigTRT_TrackExtensionTool(OfflineClusters=False)
        ToolSvc += trtTool
        fitTool = TrigInDetTrackFitter(OfflineClusters=False)
        ToolSvc += fitTool

        self.Tool_SpacePointProviderTool  = ospTool
        self.Tool_TRTExtensionTool        = trtTool
        self.Tool_ReFitterTool            = fitTool
        
        self.Map_Layer1                   = [0,1,7]
        self.Map_Layer2                   = [1,2,7,8]
        self.Map_Layer3                   = [2,8,9]
        self.Map_Layer4                   = [3,10,11]
        self.Map_Layer5                   = [4,12,13]
        self.Map_Layer6                   = [5,14,15]
        self.Map_Layer7                   = [6,16,17,18]
        self.Map_ExtraEtaLayer1           = 1.95
        self.Map_ExtraEtaLayer2           = 2.15
        self.Map_ExtraEtaLayer3           = 2.35
        
        self.RoI_HalfWidthPhi             = 0.3
        self.RoI_HalfWidthEta             = 0.3

        if TrigSiTrackFlags.LoosenCuts:

            self.Seeding_PreCutRZ         = TrigSiTrackLoosenCuts.Seeding_PreCutRZ
            self.Seeding_PreCutRPhi       = TrigSiTrackLoosenCuts.Seeding_PreCutRPhi
            self.Seeding_CutRZ            = TrigSiTrackLoosenCuts.Seeding_CutRZ
            
        else:
            
            self.Seeding_PreCutRZ         = 10.0
            self.Seeding_PreCutRPhi       = 0.0001
            self.Seeding_CutRZ            = 1.5

        self.Vertexing_Enable             = False
        self.Vertexing_CutRZ              = 200.0

        if TrigSiTrackFlags.LoosenCuts:
            
            self.Extension_PreCutRZ       = TrigSiTrackLoosenCuts.Extension_PreCutRZ
            self.Extension_PreCutRPhi     = TrigSiTrackLoosenCuts.Extension_PreCutRPhi
            self.Extension_PreCutRPhiDiff = TrigSiTrackLoosenCuts.Extension_PreCutRPhiDiff
            self.Extension_CutRZ          = TrigSiTrackLoosenCuts.Extension_CutRZ
            self.Extension_CutRPhi        = TrigSiTrackLoosenCuts.Extension_CutRPhi

        else:

            self.Extension_PreCutRZ       = 10.0
            self.Extension_PreCutRPhi     = 0.0001
            self.Extension_PreCutRPhiDiff = 0.0001
            self.Extension_CutRZ          = 4.0
            self.Extension_CutRPhi        = 0.002
            
        self.Extension_CutSpacePoints     = 2
        self.Extension_MergeSeeds         = True
        self.Extension_MergeSeedsCut      = 20.0
        self.Extension_SolveTracks        = True
        self.Extension_SolveTracksCut     = 20.0
        self.Extension_SolveShared        = True
        self.Extension_SpacePoints        = 4
        self.Extension_Refit              = True
        self.Extension_TRTExtension       = False
        self.Extension_UseMagneticField   = False
        
# monitoring histograms
        from TrigTimeMonitor.TrigTimeHistToolConfig import TrigTimeHistToolConfig
        time = TrigTimeHistToolConfig("TimeHistogramForTrigSiTrack")
        time.TimerHistLimits = [0,50]
                        
        self.AthenaMonTools = [ TrigSiTrack_ValidationMonitoring(), TrigSiTrack_OnlineMonitoring(),
                                time ]
        

class TrigSiTrack_Jet(TrigSiTrack):
    __slots__ = []
    def __init__(self, name = "TrigSiTrack_Jet"):
        TrigSiTrack.__init__(self, name)

        from __main__ import ToolSvc

        trtTool = TrigTRT_TrackExtensionTool(OfflineClusters=False)
        ToolSvc += trtTool
        fitTool = TrigInDetTrackFitter(OfflineClusters=False)
        ToolSvc += fitTool

        self.Tool_SpacePointProviderTool  = ospTool
        self.Tool_TRTExtensionTool        = trtTool
        self.Tool_ReFitterTool            = fitTool
        
        self.Map_Layer1                   = [0,1,7]
        self.Map_Layer2                   = [1,2,7,8]
        self.Map_Layer3                   = [2,8,9]
        self.Map_Layer4                   = [3,10,11]
        self.Map_Layer5                   = [4,12,13]
        self.Map_Layer6                   = [5,14,15]
        self.Map_Layer7                   = [6,16,17,18]
                
        self.RoI_HalfWidthPhi             = 0.2
        self.RoI_HalfWidthEta             = 0.2

        if TrigSiTrackFlags.LoosenCuts:

            self.Seeding_PreCutRZ         = TrigSiTrackLoosenCuts.Seeding_PreCutRZ
            self.Seeding_PreCutRPhi       = TrigSiTrackLoosenCuts.Seeding_PreCutRPhi
            self.Seeding_CutRZ            = TrigSiTrackLoosenCuts.Seeding_CutRZ
            
        else:
            
            self.Seeding_PreCutRZ         = 10.0
            self.Seeding_PreCutRPhi       = 0.0002
            self.Seeding_CutRZ            = 1.0

        self.Vertexing_Enable             = True
        self.Vertexing_CutRZ              = 200.0
        self.Vertexing_CutRPhi            = 0.00025
        self.Vertexing_MaxVertices        = 3
        self.Vertexing_WindowSize         = 2.0
        self.Vertexing_WindowSegments     = 4

        if TrigSiTrackFlags.LoosenCuts:
            
            self.Extension_PreCutRZ       = TrigSiTrackLoosenCuts.Extension_PreCutRZ
            self.Extension_PreCutRPhi     = TrigSiTrackLoosenCuts.Extension_PreCutRPhi
            self.Extension_PreCutRPhiDiff = TrigSiTrackLoosenCuts.Extension_PreCutRPhiDiff
            self.Extension_CutRZ          = TrigSiTrackLoosenCuts.Extension_CutRZ
            self.Extension_CutRPhi        = TrigSiTrackLoosenCuts.Extension_CutRPhi

        else:
            
            self.Extension_PreCutRZ       = 10.0
            self.Extension_PreCutRPhi     = 0.00015
            self.Extension_PreCutRPhiDiff = 0.00015
            self.Extension_CutRZ          = 4.0
            self.Extension_CutRPhi        = 0.0035
        
        self.Extension_CutSpacePoints     = 2
        self.Extension_MergeSeeds         = True
        self.Extension_MergeSeedsCut      = 20.0
        self.Extension_SolveTracks        = True
        self.Extension_SolveTracksCut     = 20.0
        self.Extension_SolveShared        = True
        self.Extension_SpacePoints        = 4
        self.Extension_Refit              = True
        self.Extension_TRTExtension       = False
        self.Extension_UseMagneticField   = False

# monitoring histograms
        from TrigTimeMonitor.TrigTimeHistToolConfig import TrigTimeHistToolConfig
        time = TrigTimeHistToolConfig("TimeHistogramForTrigSiTrack")
        time.TimerHistLimits = [0,50]
                        
        self.AthenaMonTools = [ TrigSiTrack_ValidationMonitoring(), TrigSiTrack_OnlineMonitoring(),
                                time ]

class TrigSiTrack_Bphysics(TrigSiTrack):
    __slots__ = []
    def __init__(self, name = "TrigSiTrack_Bphysics"):
        TrigSiTrack.__init__(self, name)

        from __main__ import ToolSvc

        trtTool = TrigTRT_TrackExtensionTool(OfflineClusters=False)
        ToolSvc += trtTool
        fitTool = TrigInDetTrackFitter(OfflineClusters=False)
        ToolSvc += fitTool

        self.Tool_SpacePointProviderTool  = ospTool
        self.Tool_TRTExtensionTool        = trtTool
        self.Tool_ReFitterTool            = fitTool
        
        self.Map_Layer1                   = [0,1,7]
        self.Map_Layer2                   = [1,2,7,8]
        self.Map_Layer3                   = [2,8,9]
        self.Map_Layer4                   = [3,10,11]
        self.Map_Layer5                   = [4,12,13]
        self.Map_Layer6                   = [5,14,15]
        self.Map_Layer7                   = [6,16,17,18]
        
        self.RoI_HalfWidthPhi             = 0.75
        self.RoI_HalfWidthEta             = 0.75

        if TrigSiTrackFlags.LoosenCuts:

            self.Seeding_PreCutRZ         = TrigSiTrackLoosenCuts.Seeding_PreCutRZ
            self.Seeding_PreCutRPhi       = TrigSiTrackLoosenCuts.Seeding_PreCutRPhi
            self.Seeding_CutRZ            = TrigSiTrackLoosenCuts.Seeding_CutRZ
            
        else:
            
            self.Seeding_PreCutRZ         = 10.0
            self.Seeding_PreCutRPhi       = 0.0004
            self.Seeding_CutRZ            = 1.5
            
        self.Vertexing_Enable             = True
        self.Vertexing_CutRZ              = 200.0
        self.Vertexing_CutRPhi            = 0.00025
        self.Vertexing_MaxVertices        = 3
        self.Vertexing_WindowSize         = 2.0
        self.Vertexing_WindowSegments     = 4

        if TrigSiTrackFlags.LoosenCuts:
            
            self.Extension_PreCutRZ       = TrigSiTrackLoosenCuts.Extension_PreCutRZ
            self.Extension_PreCutRPhi     = TrigSiTrackLoosenCuts.Extension_PreCutRPhi
            self.Extension_PreCutRPhiDiff = TrigSiTrackLoosenCuts.Extension_PreCutRPhiDiff
            self.Extension_CutRZ          = TrigSiTrackLoosenCuts.Extension_CutRZ
            self.Extension_CutRPhi        = TrigSiTrackLoosenCuts.Extension_CutRPhi

        else:
            
            self.Extension_PreCutRZ       = 10.0
            self.Extension_PreCutRPhi     = 0.0003
            self.Extension_PreCutRPhiDiff = 0.0003
            self.Extension_CutRZ          = 4.0
            self.Extension_CutRPhi        = 0.007
            
        self.Extension_CutSpacePoints     = 2
        self.Extension_MergeSeeds         = True
        self.Extension_MergeSeedsCut      = 20.0
        self.Extension_SolveTracks        = True
        self.Extension_SolveTracksCut     = 20.0
        self.Extension_SolveShared        = True
        self.Extension_Refit              = True
        self.Extension_TRTExtension       = True
        self.Extension_UseMagneticField   = False
        
# monitoring histograms
        from TrigTimeMonitor.TrigTimeHistToolConfig import TrigTimeHistToolConfig
        time = TrigTimeHistToolConfig("TimeHistogramForTrigSiTrack")
        time.TimerHistLimits = [0,50]
                        
        self.AthenaMonTools = [ TrigSiTrack_ValidationMonitoring(), TrigSiTrack_OnlineMonitoring(),
                                time ]
                
class TrigSiTrack_FullScan(TrigSiTrack):
    __slots__ = []
    def __init__(self, name = "TrigSiTrack_FullScan"):
        TrigSiTrack.__init__(self, name)

        from __main__ import ToolSvc

        trtTool = TrigTRT_TrackExtensionTool(OfflineClusters=False)
        ToolSvc += trtTool
        fitTool = TrigInDetTrackFitter(OfflineClusters=False)
        ToolSvc += fitTool

        self.Tool_SpacePointProviderTool  = ospTool
        self.Tool_TRTExtensionTool        = trtTool
        self.Tool_ReFitterTool            = fitTool

        self.Map_Layer1                   = [0,1,7]
        self.Map_Layer2                   = [1,2,7,8]
        self.Map_Layer3                   = [2,8,9]
        self.Map_Layer4                   = [3,10,11]
        self.Map_Layer5                   = [4,12,13]
        self.Map_Layer6                   = [5,14,15]
        self.Map_Layer7                   = [6,16,17,18]
        
        self.RoI_FullScan                 = True
        self.RoI_HalfWidthPhi             = 3.141
        self.RoI_HalfWidthEta             = 5.0

        if TrigSiTrackFlags.LoosenCuts:

            self.Seeding_PreCutRZ         = TrigSiTrackLoosenCuts.Seeding_PreCutRZ
            self.Seeding_PreCutRPhi       = TrigSiTrackLoosenCuts.Seeding_PreCutRPhi
            self.Seeding_CutRZ            = TrigSiTrackLoosenCuts.Seeding_CutRZ
            
        else:
            
            self.Seeding_PreCutRZ         = 10.0
            self.Seeding_PreCutRPhi       = 0.0004
            self.Seeding_CutRZ            = 1.5
        
        self.Vertexing_Enable             = True
        self.Vertexing_CutRZ              = 200.0
        self.Vertexing_CutRPhi            = 0.00025
        self.Vertexing_MaxVertices        = 3
        self.Vertexing_WindowSize         = 2.0
        self.Vertexing_WindowSegments     = 4

        if TrigSiTrackFlags.LoosenCuts:
            
            self.Extension_PreCutRZ       = TrigSiTrackLoosenCuts.Extension_PreCutRZ
            self.Extension_PreCutRPhi     = TrigSiTrackLoosenCuts.Extension_PreCutRPhi
            self.Extension_PreCutRPhiDiff = TrigSiTrackLoosenCuts.Extension_PreCutRPhiDiff
            self.Extension_CutRZ          = TrigSiTrackLoosenCuts.Extension_CutRZ
            self.Extension_CutRPhi        = TrigSiTrackLoosenCuts.Extension_CutRPhi

        else:
            
            self.Extension_PreCutRZ       = 10.0
            self.Extension_PreCutRPhi     = 0.0003
            self.Extension_PreCutRPhiDiff = 0.0003
            self.Extension_CutRZ          = 4.0
            self.Extension_CutRPhi        = 0.007
            
        self.Extension_CutSpacePoints     = 2
        self.Extension_MergeSeeds         = True
        self.Extension_MergeSeedsCut      = 20.0
        self.Extension_SolveTracks        = True
        self.Extension_SolveTracksCut     = 20.0
        self.Extension_SolveShared        = True
        self.Extension_Refit              = True
        self.Extension_TRTExtension       = True
        self.Extension_UseMagneticField   = False

# monitoring histograms
        from TrigTimeMonitor.TrigTimeHistToolConfig import TrigTimeHistToolConfig
        time = TrigTimeHistToolConfig("TimeHistogramForTrigSiTrack")
        time.TimerHistLimits = [0,50]
                        
        self.AthenaMonTools = [ TrigSiTrack_ValidationMonitoring(), TrigSiTrack_OnlineMonitoring(),
                                time ]

class TrigSiTrack_Cosmics(TrigSiTrack):
    __slots__ = []
    def __init__(self, name = "TrigSiTrack_Cosmics"):
        TrigSiTrack.__init__(self, name)

        from __main__ import ToolSvc
        
        trtTool = TrigTRT_TrackExtensionTool(OfflineClusters=False)
        ToolSvc += trtTool
        fitTool = TrigInDetTrackFitter(OfflineClusters=False)
        ToolSvc += fitTool

        self.Tool_SpacePointProviderTool  = ospTool
        self.Tool_TRTExtensionTool        = trtTool
        self.Tool_ReFitterTool            = fitTool

        self.Map_Layer1                   = [0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18]
        self.Map_Layer2                   = [0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18]
        self.Map_Layer3                   = [0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18]
        self.Map_Layer4                   = [0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18]
        self.Map_Layer5                   = [0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18]
        self.Map_Layer6                   = [0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18]
        self.Map_Layer7                   = [0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18]
        self.Map_UsePixel = False
        
        self.RoI_HalfWidthPhi             = 3.141
        self.RoI_HalfWidthEta             = 5.0

        self.Seeding_PreCutRZ             = 10.0*50
        self.Seeding_PreCutRPhi           = 0.005      ### TUNED
        self.Seeding_CutRZ                = 1.5*50
        
        self.Vertexing_Enable             = False
        self.Vertexing_CutRZ              = 1000.0     ### TUNED
            
        self.Extension_PreCutRZ           = 10.0*50
        self.Extension_PreCutRPhi         = 0.03       ### TUNED
        self.Extension_PreCutRPhiDiff     = 0.03       ### TUNED
        self.Extension_CutRZ              = 400.0      ### TUNED
        self.Extension_CutRPhi            = 0.7        ### TUNED            
        self.Extension_CutSpacePoints     = 2
        self.Extension_MergeSeeds         = True
        self.Extension_MergeSeedsCut      = 20.0
        self.Extension_SolveTracks        = True
        self.Extension_SolveTracksCut     = 20.0
        self.Extension_Refit              = True
        self.Extension_TRTExtension       = False
        self.Extension_UseMagneticField   = False
       
        self.Debug_Tuning12               = False
        self.Debug_Tuning123              = False
        self.Debug_Shift                  = False
        self.Debug_ForcedShift            = False
        self.Debug_ShiftX                 = 0.0
        self.Debug_ShiftY                 = 0.0

# monitoring histograms
        from TrigTimeMonitor.TrigTimeHistToolConfig import TrigTimeHistToolConfig
        time  = TrigTimeHistToolConfig("TimeHistogramForTrigSiTrack")
        time.TimerHistLimits = [0,50]
                        
        self.AthenaMonTools = [ TrigSiTrack_Cosmic_Monitoring(),
                                TrigSiTrack_ValidationMonitoring(), TrigSiTrack_OnlineMonitoring(),
                                time ]
