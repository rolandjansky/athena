# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

from TrigBphysHypo.TrigBphysHypoConf import TrigL2BgammaXFex
#from TrigVertexFitter.TrigVertexFitterConf import TrigL2VertexFitter
#from TrigVertexFitter.TrigVertexFitterConf import TrigVertexFitter
#from TrigVertexFitter.TrigVertexFitterConf import TrigVertexingTool

#L2VFitTool = TrigL2VertexFitter()
#VFitTool = TrigVertexFitter()
#VertexTool = TrigVertexingTool()

from AthenaCommon.AppMgr import ToolSvc

#ToolSvc += L2VFitTool
#ToolSvc += VFitTool
#ToolSvc += VertexTool


# basic cut
class L2BgammaXFex_1 (TrigL2BgammaXFex):
    __slots__ = []
    def __init__(self, name = "L2BgammaXFex_1"):
        super( TrigL2BgammaXFex, self ).__init__( name )

        # AcceptAll flag: if true take events regardless of cuts
        self.AcceptAll = False

        # L2 BgammaX cuts
#        self.TrigL2VertexFitter = L2VFitTool
#        self.TrigVertexFitter = VFitTool
#        self.TrigVertexingTool = VertexTool
        
#        from TrigBphysHypo.TrigL2BgammaXFexMonitoring import TrigL2BgammaXFexValidationMonitoring
#        validation = TrigL2BgammaXFexValidationMonitoring()
        
#        from TrigTimeMonitor.TrigTimeHistToolConfig import TrigTimeHistToolConfig
#        time = TrigTimeHistToolConfig("Time")
                
#        self.AthenaMonTools = [ validation, time ]


# basic cut
class L2BgammaXFex_FS (TrigL2BgammaXFex):
    __slots__ = []
    def __init__(self, name = "L2BgammaXFex_FS"):
        super( TrigL2BgammaXFex, self ).__init__( name )

        # AcceptAll flag: if true take events regardless of cuts
        self.AcceptAll = False

        # L2 BgammaX cuts
#        self.TrigL2VertexFitter = L2VFitTool
#        self.TrigVertexFitter = VFitTool
#        self.TrigVertexingTool = VertexTool
        
#        from TrigBphysHypo.TrigL2BgammaXFexMonitoring import TrigL2BgammaXFexValidationMonitoring
#        validation = TrigL2BgammaXFexValidationMonitoring()
        
#        from TrigTimeMonitor.TrigTimeHistToolConfig import TrigTimeHistToolConfig
#        time = TrigTimeHistToolConfig("Time")
                
#        self.AthenaMonTools = [ validation, time ]


