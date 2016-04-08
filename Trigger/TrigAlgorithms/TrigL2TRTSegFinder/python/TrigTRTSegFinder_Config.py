# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

from AthenaCommon.AppMgr import ToolSvc

from TrigL2TRTSegFinder.TrigL2TRTSegFinderConf import TrigTRTSegFinder

from TrigMonitorBase.TrigGenericMonitoringToolConfig import defineHistogram, TrigGenericMonitoringToolConfig

from TrigTimeMonitor.TrigTimeHistToolConfig import TrigTimeHistToolConfig
time = TrigTimeHistToolConfig("Time")

#Monitoring
class TrigTRTSegFinder_OnlineMonitoring(TrigGenericMonitoringToolConfig):
    def __init__ (self, name="TrigTRTSegFinder_OnlineMonitoring"):
        super(TrigTRTSegFinder_OnlineMonitoring, self).__init__(name)
        self.defineTarget("Online")
        self.Histograms += [ defineHistogram('trk_nStrawsTRT',
                                             type='TH1F',
                                             title="Number of TRT Straws on track",
                                             xbins = 100, xmin=0.0, xmax=100)]
        self.Histograms += [ defineHistogram('trk_nTRsTRT',
                                             type='TH1F',
                                             title="Number of TRT TR hits on track",
                                             xbins = 100, xmin=0.0, xmax=100)]
        self.Histograms += [ defineHistogram('trk_pt',
                                             type='TH1F',
                                             title="Track P_T",
                                             xbins = 100, xmin=0.0, xmax=40000)]
        self.Histograms += [ defineHistogram('trk_a0',
                                             type='TH1F',
                                             title="Track a0",
                                             xbins = 100, xmin=-1.5, xmax=1.5)]
        self.Histograms += [ defineHistogram('trk_z0',
                                             type='TH1F',
                                             title="Track z0",
                                             xbins = 100, xmin=-100, xmax=100)]
        self.Histograms += [ defineHistogram('trk_phi',
                                             type='TH1F',
                                             title="Track phi",
                                             xbins = 100, xmin=-3.2, xmax=3.2)]
        self.Histograms += [ defineHistogram('trk_eta',
                                             type='TH1F',
                                             title="Track eta",
                                             xbins = 100, xmin=-5, xmax=5)]
        self.Histograms += [ defineHistogram('roi_nTracks',
                                             type='TH1F',
                                             title="Number of Tracks",
                                             xbins = 20, xmin=0., xmax=20.)]
        self.Histograms += [ defineHistogram('dataErrors',
                                             type='TH1F',
                                             title="Data Errors",
                                             xbins=5,xmin=0.,xmax=5.,
                                             labels='Failure:Recoverable')]
        self.Histograms +=[defineHistogram('dataErrors_TRT',
                                           type='TH1F',
                                           title="TRT Data Errors",
                                           xbins=16,xmin=0.,xmax=16.,
                                           labels='TRT_BCID:TRT_LVL1ID:TRT_MISSING:TRT_ERROR:TRT_SID:TRT_ROB')]
        
     
class TrigTRTSegFinder_ValidationMonitoring(TrigGenericMonitoringToolConfig):
    def __init__ (self, name="TrigTRTSegFinder_ValidationMonitoring"):
        super(TrigTRTSegFinder_ValidationMonitoring, self).__init__(name)
        self.defineTarget("Validation")
        self.Histograms += [ defineHistogram('trk_nStrawsTRT',
                                             type='TH1F',
                                             title="Number of TRT Straws on track",
                                             xbins = 100, xmin=0.0, xmax=100)]
        self.Histograms += [ defineHistogram('trk_nTRsTRT',
                                             type='TH1F',
                                             title="Number of TRT TR hits on track",
                                             xbins = 100, xmin=0.0, xmax=100)]
        self.Histograms += [ defineHistogram('trk_pt',
                                             type='TH1F',
                                             title="Track pt",
                                             xbins = 100, xmin=0.0, xmax=40000)]
        self.Histograms += [ defineHistogram('trk_a0',
                                             type='TH1F',
                                             title="Track a0",
                                             xbins = 100, xmin=-1.5, xmax=1.5)]
        self.Histograms += [ defineHistogram('trk_z0',
                                             type='TH1F',
                                             title="Track z0",
                                             xbins = 100, xmin=-100, xmax=100)]
        self.Histograms += [ defineHistogram('trk_phi',
                                             type='TH1F',
                                             title="Track phi",
                                             xbins = 100, xmin=-3.2, xmax=3.2)]
        self.Histograms += [ defineHistogram('trk_eta',
                                             type='TH1F',
                                             title="Track eta",
                                             xbins = 100, xmin=-5, xmax=5)]
        self.Histograms += [ defineHistogram('roi_nTracks',
                                             type='TH1F',
                                             title="Number of Tracks",
                                             xbins = 20, xmin=0., xmax=20.)]
        self.Histograms += [ defineHistogram('dataErrors',
                                             type='TH1F',
                                             title="Data Errors",
                                             xbins=5,xmin=0.,xmax=5.,
                                             labels='Failure:Recoverable')]
        self.Histograms +=[defineHistogram('dataErrors_TRT',
                                           type='TH1F',
                                           title="TRT Data Errors",
                                           xbins=16,xmin=0.,xmax=16.,
                                           labels='TRT_BCID:TRT_LVL1ID:TRT_MISSING:TRT_ERROR:TRT_SID:TRT_ROB')] 


#Cosmic Monitoring
class TrigTRTSegFinder_CosmicMonitoring(TrigGenericMonitoringToolConfig):
    def __init__ (self, name="TrigTRTSegFinder_CosmicMonitoring"):
        super(TrigTRTSegFinder_CosmicMonitoring, self).__init__(name)
        self.defineTarget("Online")
        self.Histograms += [ defineHistogram('trk_nStrawsTRT',
                                             type='TH1F',
                                             title="Number of TRT Straws on track",
                                             xbins = 100, xmin=0.0, xmax=100)]
        self.Histograms += [ defineHistogram('trk_nTRsTRT',
                                             type='TH1F',
                                             title="Number of TRT TR hits on track",
                                             xbins = 100, xmin=0.0, xmax=100)]
        self.Histograms += [ defineHistogram('trk_pt',
                                             type='TH1F',
                                             title="Track pt",
                                             xbins = 100, xmin=0.0, xmax=40000)]
        self.Histograms += [ defineHistogram('trk_a0',
                                             type='TH1F',
                                             title="Track a0",
                                             xbins = 500, xmin=-1000, xmax=1000)]
        self.Histograms += [ defineHistogram('trk_z0',
                                             type='TH1F',
                                             title="z0",
                                             xbins = 540, xmin=-2700, xmax=2700)]
        self.Histograms += [ defineHistogram('trk_phi',
                                             type='TH1F',
                                             title="Track phi",
                                             xbins = 100, xmin=-3.2, xmax=3.2)]
        self.Histograms += [ defineHistogram('trk_eta',
                                             type='TH1F',
                                             title="Track eta",
                                             xbins = 120, xmin=-3, xmax=3.)]
        self.Histograms += [ defineHistogram('roi_nTracks',
                                             type='TH1F',
                                             title="Number of Tracks",
                                             xbins = 20, xmin=0., xmax=20.)]
        self.Histograms += [ defineHistogram('dataErrors',
                                             type='TH1F',
                                             title="Data Errors",
                                             xbins=5,xmin=0.,xmax=5.,
                                             labels='Failure:Recoverable')]
        self.Histograms +=[defineHistogram('dataErrors_TRT',
                                           type='TH1F',
                                           title="TRT Data Errors",
                                           xbins=16,xmin=0.,xmax=16.,
                                           labels='TRT_BCID:TRT_LVL1ID:TRT_MISSING:TRT_ERROR:TRT_SID:TRT_ROB')]
        

class TrigTRTSegFinder_eGamma(TrigTRTSegFinder) :
    __slots__ = []
    def __init__(self, name = "TrigTRTSegFinder_eGamma"):
        from AthenaCommon.AppMgr import ToolSvc
        TrigTRTSegFinder.__init__(self, name)
        self.RoIhalfWidthDeltaPhi = 0.1
        self.RoIhalfWidthDeltaEta = 0.1
        self.pTmin                = 1000.
#        self.PropagatorTool       = "Trk::RungeKuttaPropagator"
#        self.MagneticTool         = "Trk::MagneticFieldTool_xk"
        from TrigOfflineDriftCircleTool.TrigOfflineDriftCircleTool_Config import ConfiguredTrigTRT_DriftCircleProviderTool
        self.TrigDriftCircleProvider = ConfiguredTrigTRT_DriftCircleProviderTool


        from TRT_TrackExtensionTool_xk.TRT_TrackExtensionTool_xkConf import InDet__TRT_TrackExtensionTool_xk
        from InDetTrigRecExample.InDetTrigConfigRecLoadTools import InDetTrigTRTDriftCircleCut, \
             InDetTrigPatternPropagator,InDetTrigPatternUpdator
        from InDetTrigRecExample.InDetTrigConfigRecLoadToolsBack import InDetTrigTRTDetElementsRoadMaker
        from InDetTrigRecExample.InDetTrigFlags import InDetTrigFlags
        from InDetTrigRecExample.ConfiguredNewTrackingTrigCuts import EFIDTrackingCuts
        InDetTrigCutValues = EFIDTrackingCuts

        
        InDetL2TRTExtensionTool = InDet__TRT_TrackExtensionTool_xk(name = 'InDetL2TRTExtensionTool',
                                                                   TRT_ClustersContainer = 'Trig_OfflineDriftCircles',
                                                                   DriftCircleCutTool=InDetTrigTRTDriftCircleCut,
                                                                   TrtManagerLocation    = 'TRT',              # default
                                                                   PropagatorTool        = InDetTrigPatternPropagator,
                                                                   UpdatorTool           = InDetTrigPatternUpdator,
                                                                   UseDriftRadius        = not InDetTrigFlags.noTRTTiming(),
                                                                   RoadTool              = InDetTrigTRTDetElementsRoadMaker,
                                                                   MinNumberDriftCircles = InDetTrigCutValues.minTRTonTrk(),
                                                                   ScaleHitUncertainty   = 2.,
                                                                   RoadWidth             = 20.,
                                                                   UseParameterization   = InDetTrigCutValues.useParameterizedTRTCuts()
                                                                   )
        ToolSvc += InDetL2TRTExtensionTool
        self.TrackExtensionTool    =    ToolSvc.InDetL2TRTExtensionTool
#        print InDetL2TRTExtensionTool
        from TRT_TrackSegmentsTool_xk.TRT_TrackSegmentsTool_xkConf import InDet__TRT_TrackSegmentsMaker_ATLxk
        InDetL2_TRT_TrackSegmentsMaker = InDet__TRT_TrackSegmentsMaker_ATLxk(name = 'InDetL2_TRT_TrackSegmentsMaker',
                                                                             TrtManagerLocation    = "TRT",
                                                                             pTmin                 = self.pTmin,
                                                                             TRT_ClustersContainer = "Trig_OfflineDriftCircles",
                                                                             PropagatorTool          = InDetTrigPatternPropagator,
                                                                             AssosiationTool         = None,
                                                                             TrackExtensionTool = InDetL2TRTExtensionTool,
                                                                             UseAssosiationTool      = False,
                                                                             MinNumberDriftCircles   = InDetTrigCutValues.minTRTonly(),
                                                                             RemoveNoiseDriftCircles = InDetTrigFlags.removeTRTNoise()
                                                                             )
        ToolSvc += InDetL2_TRT_TrackSegmentsMaker
        self.SegmentsMakerTool    =    ToolSvc.InDetL2_TRT_TrackSegmentsMaker
#        print InDetL2_TRT_TrackSegmentsMaker
# monitoring histograms
        self.AthenaMonTools = [ time, TrigTRTSegFinder_ValidationMonitoring(), TrigTRTSegFinder_OnlineMonitoring() ]

class TrigTRTSegFinder_Muon(TrigTRTSegFinder) :
    __slots__ = []
    def __init__(self, name = "TrigTRTSegFinder_Muon"):
        from AthenaCommon.AppMgr import ToolSvc
        TrigTRTSegFinder.__init__(self, name)
        self.RoIhalfWidthDeltaPhi = 0.1
        self.RoIhalfWidthDeltaEta = 0.1
        self.pTmin                = 1000.
        from TrigOfflineDriftCircleTool.TrigOfflineDriftCircleTool_Config import ConfiguredTrigTRT_DriftCircleProviderTool
        self.TrigDriftCircleProvider = ConfiguredTrigTRT_DriftCircleProviderTool

        from TRT_TrackExtensionTool_xk.TRT_TrackExtensionTool_xkConf import InDet__TRT_TrackExtensionTool_xk
        from InDetTrigRecExample.InDetTrigConfigRecLoadTools import InDetTrigTRTDriftCircleCut, \
             InDetTrigPatternPropagator,InDetTrigPatternUpdator
        from InDetTrigRecExample.InDetTrigConfigRecLoadToolsBack import InDetTrigTRTDetElementsRoadMaker
        from InDetTrigRecExample.InDetTrigFlags import InDetTrigFlags
        from InDetTrigRecExample.ConfiguredNewTrackingTrigCuts import EFIDTrackingCuts
        InDetTrigCutValues = EFIDTrackingCuts
        
        InDetL2TRTExtensionTool = InDet__TRT_TrackExtensionTool_xk(name = 'InDetL2TRTExtensionTool',
								TRT_ClustersContainer = 'Trig_OfflineDriftCircles',
                                                                   DriftCircleCutTool=InDetTrigTRTDriftCircleCut)
        ToolSvc += InDetL2TRTExtensionTool
        self.TrackExtensionTool    =    ToolSvc.InDetL2TRTExtensionTool
        from TRT_TrackSegmentsTool_xk.TRT_TrackSegmentsTool_xkConf import InDet__TRT_TrackSegmentsMaker_ATLxk
        InDetL2_TRT_TrackSegmentsMaker = InDet__TRT_TrackSegmentsMaker_ATLxk(name = 'InDetL2_TRT_TrackSegmentsMaker',
								TrtManagerLocation    = "TRT",
								pTmin                 = self.pTmin,
								TRT_ClustersContainer = "Trig_OfflineDriftCircles",
								TrackExtensionTool = InDetL2TRTExtensionTool)
        ToolSvc += InDetL2_TRT_TrackSegmentsMaker
        self.SegmentsMakerTool    =    ToolSvc.InDetL2_TRT_TrackSegmentsMaker
        self.AthenaMonTools = [ time, TrigTRTSegFinder_ValidationMonitoring(), TrigTRTSegFinder_OnlineMonitoring() ]

class TrigTRTSegFinder_Tau(TrigTRTSegFinder) :
    __slots__ = []
    def __init__(self, name = "TrigTRTSegFinder_Tau"):
        from AthenaCommon.AppMgr import ToolSvc
        TrigTRTSegFinder.__init__(self, name)
        self.RoIhalfWidthDeltaPhi = 0.1
        self.RoIhalfWidthDeltaEta = 0.1
        self.pTmin                = 1000.
        from TrigOfflineDriftCircleTool.TrigOfflineDriftCircleTool_Config import ConfiguredTrigTRT_DriftCircleProviderTool
        self.TrigDriftCircleProvider = ConfiguredTrigTRT_DriftCircleProviderTool

        from TRT_TrackExtensionTool_xk.TRT_TrackExtensionTool_xkConf import InDet__TRT_TrackExtensionTool_xk
        from InDetTrigRecExample.InDetTrigConfigRecLoadTools import InDetTrigTRTDriftCircleCut, \
             InDetTrigPatternPropagator,InDetTrigPatternUpdator
        from InDetTrigRecExample.InDetTrigConfigRecLoadToolsBack import InDetTrigTRTDetElementsRoadMaker
        from InDetTrigRecExample.InDetTrigFlags import InDetTrigFlags
        from InDetTrigRecExample.ConfiguredNewTrackingTrigCuts import EFIDTrackingCuts
        InDetTrigCutValues = EFIDTrackingCuts
        
        InDetL2TRTExtensionTool = InDet__TRT_TrackExtensionTool_xk(name = 'InDetL2TRTExtensionTool',
								TRT_ClustersContainer = 'Trig_OfflineDriftCircles',
                                                                   DriftCircleCutTool=InDetTrigTRTDriftCircleCut)
        ToolSvc += InDetL2TRTExtensionTool
        self.TrackExtensionTool    =    ToolSvc.InDetL2TRTExtensionTool
        from TRT_TrackSegmentsTool_xk.TRT_TrackSegmentsTool_xkConf import InDet__TRT_TrackSegmentsMaker_ATLxk
        InDetL2_TRT_TrackSegmentsMaker = InDet__TRT_TrackSegmentsMaker_ATLxk(name = 'InDetL2_TRT_TrackSegmentsMaker',
								TrtManagerLocation    = "TRT",
								pTmin                 = self.pTmin,
								TRT_ClustersContainer = "Trig_OfflineDriftCircles",
								TrackExtensionTool = InDetL2TRTExtensionTool)
        ToolSvc += InDetL2_TRT_TrackSegmentsMaker
        self.SegmentsMakerTool    =    ToolSvc.InDetL2_TRT_TrackSegmentsMaker
        self.AthenaMonTools = [ time, TrigTRTSegFinder_ValidationMonitoring(), TrigTRTSegFinder_OnlineMonitoring() ]

class TrigTRTSegFinder_Jets(TrigTRTSegFinder) :
    __slots__ = []
    def __init__(self, name = "TrigTRTSegFinder_Jets"):
        from AthenaCommon.AppMgr import ToolSvc
        TrigTRTSegFinder.__init__(self, name)
        self.RoIhalfWidthDeltaPhi = 0.1
        self.RoIhalfWidthDeltaEta = 0.1
        self.pTmin                = 1000.
        from TrigOfflineDriftCircleTool.TrigOfflineDriftCircleTool_Config import ConfiguredTrigTRT_DriftCircleProviderTool
        self.TrigDriftCircleProvider = ConfiguredTrigTRT_DriftCircleProviderTool


        from TRT_TrackExtensionTool_xk.TRT_TrackExtensionTool_xkConf import InDet__TRT_TrackExtensionTool_xk
        from InDetTrigRecExample.InDetTrigConfigRecLoadTools import InDetTrigTRTDriftCircleCut, \
             InDetTrigPatternPropagator,InDetTrigPatternUpdator
        from InDetTrigRecExample.InDetTrigConfigRecLoadToolsBack import InDetTrigTRTDetElementsRoadMaker
        from InDetTrigRecExample.InDetTrigFlags import InDetTrigFlags
        from InDetTrigRecExample.ConfiguredNewTrackingTrigCuts import EFIDTrackingCuts
        InDetTrigCutValues = EFIDTrackingCuts
        
        InDetL2TRTExtensionTool = InDet__TRT_TrackExtensionTool_xk(name = 'InDetL2TRTExtensionTool',
								TRT_ClustersContainer = 'Trig_OfflineDriftCircles',
                                                                   DriftCircleCutTool=InDetTrigTRTDriftCircleCut)
        ToolSvc += InDetL2TRTExtensionTool
        self.TrackExtensionTool    =    ToolSvc.InDetL2TRTExtensionTool
        from TRT_TrackSegmentsTool_xk.TRT_TrackSegmentsTool_xkConf import InDet__TRT_TrackSegmentsMaker_ATLxk
        InDetL2_TRT_TrackSegmentsMaker = InDet__TRT_TrackSegmentsMaker_ATLxk(name = 'InDetL2_TRT_TrackSegmentsMaker',
								TrtManagerLocation    = "TRT",
								pTmin                 = self.pTmin,
								TRT_ClustersContainer = "Trig_OfflineDriftCircles",
								TrackExtensionTool = InDetL2TRTExtensionTool)
        ToolSvc += InDetL2_TRT_TrackSegmentsMaker
        self.SegmentsMakerTool    =    ToolSvc.InDetL2_TRT_TrackSegmentsMaker
        self.AthenaMonTools = [ time, TrigTRTSegFinder_ValidationMonitoring(), TrigTRTSegFinder_OnlineMonitoring() ]

class TrigTRTSegFinder_Bphysics(TrigTRTSegFinder) :
    __slots__ = []
    def __init__(self, name = "TrigTRTSegFinder_Bphysics"):
        from AthenaCommon.AppMgr import ToolSvc
        TrigTRTSegFinder.__init__(self, name)
        self.RoIhalfWidthDeltaPhi = 0.1
        self.RoIhalfWidthDeltaEta = 0.1
        self.pTmin                = 1000.
        from TrigOfflineDriftCircleTool.TrigOfflineDriftCircleTool_Config import ConfiguredTrigTRT_DriftCircleProviderTool
        self.TrigDriftCircleProvider = ConfiguredTrigTRT_DriftCircleProviderTool


        from TRT_TrackExtensionTool_xk.TRT_TrackExtensionTool_xkConf import InDet__TRT_TrackExtensionTool_xk
        from InDetTrigRecExample.InDetTrigConfigRecLoadTools import InDetTrigTRTDriftCircleCut, \
             InDetTrigPatternPropagator,InDetTrigPatternUpdator
        from InDetTrigRecExample.InDetTrigConfigRecLoadToolsBack import InDetTrigTRTDetElementsRoadMaker
        from InDetTrigRecExample.InDetTrigFlags import InDetTrigFlags
        from InDetTrigRecExample.ConfiguredNewTrackingTrigCuts import EFIDTrackingCuts
        InDetTrigCutValues = EFIDTrackingCuts
        
        InDetL2TRTExtensionTool = InDet__TRT_TrackExtensionTool_xk(name = 'InDetL2TRTExtensionTool',
								TRT_ClustersContainer = 'Trig_OfflineDriftCircles',
                                                                   DriftCircleCutTool=InDetTrigTRTDriftCircleCut)
        ToolSvc += InDetL2TRTExtensionTool
        self.TrackExtensionTool    =    ToolSvc.InDetL2TRTExtensionTool
        from TRT_TrackSegmentsTool_xk.TRT_TrackSegmentsTool_xkConf import InDet__TRT_TrackSegmentsMaker_ATLxk
        InDetL2_TRT_TrackSegmentsMaker = InDet__TRT_TrackSegmentsMaker_ATLxk(name = 'InDetL2_TRT_TrackSegmentsMaker',
								TrtManagerLocation    = "TRT",
								pTmin                 = self.pTmin,
								TRT_ClustersContainer = "Trig_OfflineDriftCircles",
								TrackExtensionTool = InDetL2TRTExtensionTool)
        ToolSvc += InDetL2_TRT_TrackSegmentsMaker
        self.SegmentsMakerTool    =    ToolSvc.InDetL2_TRT_TrackSegmentsMaker
        self.AthenaMonTools = [ time, TrigTRTSegFinder_ValidationMonitoring(), TrigTRTSegFinder_OnlineMonitoring() ]

class TrigTRTSegFinder_Cosmics(TrigTRTSegFinder) :
    __slots__ = []
    def __init__(self, name = "TrigTRTSegFinder_Cosmics"):
        from AthenaCommon.AppMgr import ToolSvc
        TrigTRTSegFinder.__init__(self, name)
        self.RoIhalfWidthDeltaPhi = 3.14
        self.RoIhalfWidthDeltaEta = 3.
        self.pTmin                = 1000.
        from TrigOfflineDriftCircleTool.TrigOfflineDriftCircleTool_Config import ConfiguredTrigTRT_DriftCircleProviderTool
        self.TrigDriftCircleProvider = ConfiguredTrigTRT_DriftCircleProviderTool


        from TRT_TrackExtensionTool_xk.TRT_TrackExtensionTool_xkConf import InDet__TRT_TrackExtensionTool_xk
        from InDetTrigRecExample.InDetTrigConfigRecLoadTools import InDetTrigTRTDriftCircleCut
        InDetL2TRTExtensionTool = InDet__TRT_TrackExtensionTool_xk(name = 'InDetL2TRTExtensionTool',
								TRT_ClustersContainer = 'Trig_OfflineDriftCircles',
                                                                   DriftCircleCutTool=InDetTrigTRTDriftCircleCut)
        ToolSvc += InDetL2TRTExtensionTool
        self.TrackExtensionTool    =    ToolSvc.InDetL2TRTExtensionTool
#        from TRT_TrackSegmentsTool_xk.TRT_TrackSegmentsTool_xkConf import InDet__TRT_TrackSegmentsMaker_ECcosmics
        from TRT_TrackSegmentsTool_xk.TRT_TrackSegmentsTool_xkConf import InDet__TRT_TrackSegmentsMaker_CTBxk
#        InDetL2_TRT_TrackSegmentsMaker = InDet__TRT_TrackSegmentsMaker_ECcosmics(name = 'InDetL2_TRT_TrackSegmentsMaker',
        InDetL2_TRT_TrackSegmentsMaker = InDet__TRT_TrackSegmentsMaker_CTBxk(name = 'InDetL2_TRT_TrackSegmentsMaker',
								TrtManagerLocation    = "TRT",
								TRT_ClustersContainer = "Trig_OfflineDriftCircles",
                                                                IsMagneticFieldOn = True)
#								TrackExtensionTool = InDetL2TRTExtensionTool)
        ToolSvc += InDetL2_TRT_TrackSegmentsMaker
        self.SegmentsMakerTool    =    ToolSvc.InDetL2_TRT_TrackSegmentsMaker
        self.AthenaMonTools = [ time, TrigTRTSegFinder_CosmicMonitoring(), TrigTRTSegFinder_ValidationMonitoring()]

class TrigTRTSegFinder_Cosmics_NoField(TrigTRTSegFinder) :
    __slots__ = []
    def __init__(self, name = "TrigTRTSegFinder_Cosmics_NoField"):
        from AthenaCommon.AppMgr import ToolSvc
        TrigTRTSegFinder.__init__(self, name)
        self.RoIhalfWidthDeltaPhi = 3.14
        self.RoIhalfWidthDeltaEta = 3.
        self.pTmin                = 1000.
        from TrigOfflineDriftCircleTool.TrigOfflineDriftCircleTool_Config import ConfiguredTrigTRT_DriftCircleProviderTool
        self.TrigDriftCircleProvider = ConfiguredTrigTRT_DriftCircleProviderTool


        from TRT_TrackExtensionTool_xk.TRT_TrackExtensionTool_xkConf import InDet__TRT_TrackExtensionTool_xk
        from InDetTrigRecExample.InDetTrigConfigRecLoadTools import InDetTrigTRTDriftCircleCut
        InDetL2TRTExtensionTool = InDet__TRT_TrackExtensionTool_xk(name = 'InDetL2TRTExtensionTool',
								TRT_ClustersContainer = 'Trig_OfflineDriftCircles',
                                                                   DriftCircleCutTool=InDetTrigTRTDriftCircleCut)
        ToolSvc += InDetL2TRTExtensionTool
        self.TrackExtensionTool    =    ToolSvc.InDetL2TRTExtensionTool
#        from TRT_TrackSegmentsTool_xk.TRT_TrackSegmentsTool_xkConf import InDet__TRT_TrackSegmentsMaker_ECcosmics
        from TRT_TrackSegmentsTool_xk.TRT_TrackSegmentsTool_xkConf import InDet__TRT_TrackSegmentsMaker_CTBxk
#        InDetL2_TRT_TrackSegmentsMaker = InDet__TRT_TrackSegmentsMaker_ECcosmics(name = 'InDetL2_TRT_TrackSegmentsMaker',
        InDetL2_TRT_TrackSegmentsMaker = InDet__TRT_TrackSegmentsMaker_CTBxk(name = 'InDetL2_TRT_TrackSegmentsMaker_NoField',
								TrtManagerLocation    = "TRT",
								TRT_ClustersContainer = "Trig_OfflineDriftCircles",
                                                                IsMagneticFieldOn = False)
#								TrackExtensionTool = InDetL2TRTExtensionTool)
        ToolSvc += InDetL2_TRT_TrackSegmentsMaker
        self.SegmentsMakerTool    =    ToolSvc.InDetL2_TRT_TrackSegmentsMaker_NoField
        self.AthenaMonTools = [ time, TrigTRTSegFinder_CosmicMonitoring(), TrigTRTSegFinder_ValidationMonitoring() ]

class TrigTRTSegFinder_Cosmics_NewAlgo(TrigTRTSegFinder) :
    __slots__ = []
    def __init__(self, name = "TrigTRTSegFinder_Cosmics_NewAlgo"):
        from AthenaCommon.AppMgr import ToolSvc
        TrigTRTSegFinder.__init__(self, name)
        self.RoIhalfWidthDeltaPhi = 3.14
        self.RoIhalfWidthDeltaEta = 3.
        self.pTmin                = 1000.

        from TrigOfflineDriftCircleTool.TrigOfflineDriftCircleTool_Config import ConfiguredTrigTRT_DriftCircleProviderTool
        self.TrigDriftCircleProvider = ConfiguredTrigTRT_DriftCircleProviderTool

        from TRT_TrackExtensionTool_xk.TRT_TrackExtensionTool_xkConf import InDet__TRT_TrackExtensionTool_xk
        from InDetTrigRecExample.InDetTrigConfigRecLoadTools import InDetTrigTRTDriftCircleCut
        InDetL2TRTExtensionTool = InDet__TRT_TrackExtensionTool_xk(name = 'InDetL2TRTExtensionTool',
								TRT_ClustersContainer = 'Trig_OfflineDriftCircles',
                                                                   DriftCircleCutTool=InDetTrigTRTDriftCircleCut)
        ToolSvc += InDetL2TRTExtensionTool
        self.TrackExtensionTool    =    ToolSvc.InDetL2TRTExtensionTool
#        from TRT_TrackSegmentsTool_xk.TRT_TrackSegmentsTool_xkConf import InDet__TRT_TrackSegmentsMaker_ECcosmics
        from TRT_TrackSegmentsTool_xk.TRT_TrackSegmentsTool_xkConf import InDet__TRT_TrackSegmentsMaker_BarrelCosmics
#        InDetL2_TRT_TrackSegmentsMaker = InDet__TRT_TrackSegmentsMaker_ECcosmics(name = 'InDetL2_TRT_TrackSegmentsMaker',
        InDetL2_TRT_TrackSegmentsMaker = InDet__TRT_TrackSegmentsMaker_BarrelCosmics(name = 'InDetL2_TRT_TrackSegmentsMaker_BarrelCosmics',
                                                                MinimalNumberOfTRTHits = 15, MinNumberOfHitsForSeed = 12, MinNumberOfHitsAboveTOT = 10,
								TrtManagerLocation    = "TRT",
								TRT_ClustersContainer = "Trig_OfflineDriftCircles",
								IsMagneticFieldOn = True)
#								TrackExtensionTool = InDetL2TRTExtensionTool)
        ToolSvc += InDetL2_TRT_TrackSegmentsMaker
        self.SegmentsMakerTool    =    ToolSvc.InDetL2_TRT_TrackSegmentsMaker_BarrelCosmics
        self.AthenaMonTools = [ time, TrigTRTSegFinder_CosmicMonitoring(), TrigTRTSegFinder_ValidationMonitoring() ]

class TrigTRTSegFinder_Cosmics_NewAlgo_NoField(TrigTRTSegFinder) :
    __slots__ = []
    def __init__(self, name = "TrigTRTSegFinder_Cosmics_NewAlgo_NoField"):
        from AthenaCommon.AppMgr import ToolSvc
        TrigTRTSegFinder.__init__(self, name)
        self.RoIhalfWidthDeltaPhi = 3.14
        self.RoIhalfWidthDeltaEta = 3.
        self.pTmin                = 1000.

        from TrigOfflineDriftCircleTool.TrigOfflineDriftCircleTool_Config import ConfiguredTrigTRT_DriftCircleProviderTool
        self.TrigDriftCircleProvider = ConfiguredTrigTRT_DriftCircleProviderTool


        from TRT_TrackExtensionTool_xk.TRT_TrackExtensionTool_xkConf import InDet__TRT_TrackExtensionTool_xk
        from InDetTrigRecExample.InDetTrigConfigRecLoadTools import InDetTrigTRTDriftCircleCut
        InDetL2TRTExtensionTool = InDet__TRT_TrackExtensionTool_xk(name = 'InDetL2TRTExtensionTool',
								TRT_ClustersContainer = 'Trig_OfflineDriftCircles',
                                                                   DriftCircleCutTool=InDetTrigTRTDriftCircleCut)
        ToolSvc += InDetL2TRTExtensionTool
        self.TrackExtensionTool    =    ToolSvc.InDetL2TRTExtensionTool
#        from TRT_TrackSegmentsTool_xk.TRT_TrackSegmentsTool_xkConf import InDet__TRT_TrackSegmentsMaker_ECcosmics
        from TRT_TrackSegmentsTool_xk.TRT_TrackSegmentsTool_xkConf import InDet__TRT_TrackSegmentsMaker_BarrelCosmics
#        InDetL2_TRT_TrackSegmentsMaker = InDet__TRT_TrackSegmentsMaker_ECcosmics(name = 'InDetL2_TRT_TrackSegmentsMaker',
        InDetL2_TRT_TrackSegmentsMaker = InDet__TRT_TrackSegmentsMaker_BarrelCosmics(name = 'InDetL2_TRT_TrackSegmentsMaker_BarrelCosmics_NoField',
								MinimalNumberOfTRTHits = 20, MinNumberOfHitsForSeed = 12, MinNumberOfHitsAboveTOT = 10,
								TrtManagerLocation    = "TRT",
								TRT_ClustersContainer = "Trig_OfflineDriftCircles",
								IsMagneticFieldOn = False)
#								TrackExtensionTool = InDetL2TRTExtensionTool)
        ToolSvc += InDetL2_TRT_TrackSegmentsMaker
        self.SegmentsMakerTool    =    ToolSvc.InDetL2_TRT_TrackSegmentsMaker_BarrelCosmics_NoField
        self.AthenaMonTools = [ time, TrigTRTSegFinder_CosmicMonitoring(), TrigTRTSegFinder_ValidationMonitoring() ]
