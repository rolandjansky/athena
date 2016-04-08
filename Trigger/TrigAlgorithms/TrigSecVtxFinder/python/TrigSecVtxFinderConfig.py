# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

from TrigSecVtxFinder.TrigSecVtxFinderConf import TrigSecVtxFinder

class L2SecVtxFinder (TrigSecVtxFinder):
    __slots__ = []
    def __init__(self, name="L2SecVtxFinder"):
        super( L2SecVtxFinder, self ).__init__( name )
        
        from AthenaCommon.AppMgr import ToolSvc
        
        from TrigInDetVxInJetTool.TrigInDetVxInJetToolConf import TrigInDetVxInJetTool        
        TrigInDetVxInJetTool = TrigInDetVxInJetTool(name             = "TrigInDetVxInJetTool",
                                                    getNegativeTail  = False, # True for SV0 ; False for SV1
                                                    )
        
        #TrigInDetVxInJetTool.OutputLevel=2
        
        ToolSvc += TrigInDetVxInJetTool
        
        self.UseBeamSpotFlag   = False
        self.TrackJetTrkSelExt = False
        self.UseJetDirection   = 1
        
        from TrigSecVtxFinder.TrigSecVtxFinderMonitoring import TrigSecVtxFinderValidationMonitoring, TrigSecVtxFinderOnlineMonitoring
        validation = TrigSecVtxFinderValidationMonitoring()
        online     = TrigSecVtxFinderOnlineMonitoring()
        
        from TrigTimeMonitor.TrigTimeHistToolConfig import TrigTimeHistToolConfig
        time = TrigTimeHistToolConfig("TimeHistogramForTrigSecVtxFinder")
        time.TimerHistLimits = [0,10]
        
        self.AthenaMonTools = [ time, validation, online ]



class L2SecVtx (L2SecVtxFinder):
    __slots__ = []
    def __init__(self, name="L2SecVtx"):
        super( L2SecVtx, self ).__init__( name )
        
        self.AlgoId = 1



class L2SecVtx_IDScan (L2SecVtxFinder):
    __slots__ = []
    def __init__(self, name="L2SecVtx_IDScan"):
        super( L2SecVtx_IDScan, self ).__init__( name )
        
        self.AlgoId = 2



class L2SecVtx_JetA (L2SecVtxFinder):
    __slots__ = []
    def __init__(self, name="L2SecVtx_JetA"):
        super( L2SecVtx_JetA, self ).__init__( name )
        
        self.AlgoId = 5



class L2SecVtx_JetB (L2SecVtxFinder):
    __slots__ = []
    def __init__(self, name="L2SecVtx_JetB"):
        super( L2SecVtx_JetB, self ).__init__( name )
        
        self.AlgoId = 6



class L2SecVtx_JetF (L2SecVtxFinder):
    __slots__ = []
    def __init__(self, name="L2SecVtx_JetF"):
        super( L2SecVtx_JetF, self ).__init__( name )
        
        self.AlgoId = 8



class L2SecVtx_JetFR (L2SecVtxFinder):
    __slots__ = []
    def __init__(self, name="L2SecVtx_JetFR"):
        super( L2SecVtx_JetFR, self ).__init__( name )
       
        self.AlgoId = 13

