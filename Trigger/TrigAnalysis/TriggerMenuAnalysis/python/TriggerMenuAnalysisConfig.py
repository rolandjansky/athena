# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

from TriggerMenuAnalysis.TriggerMenuAnalysisConf import *
from TrigDecisionTool.TrigDecisionToolConf import *

from AthenaCommon.AppMgr import ServiceMgr, ToolSvc

if not hasattr(ToolSvc, 'TrigDecisionTool'):
    ToolSvc += Trig__TrigDecisionTool('TrigDecisionTool')
    
from TrigNavigation.TrigNavigationConfig import HLTNavigationOffline
if not hasattr(ToolSvc, 'Navigation'):
    ToolSvc += HLTNavigationOffline()

class TrigAccessTool_default(TrigAccessTool):
    def __init__(self, name='TrigAccessTool'):
        TrigAccessTool.__init__(self, name)
        self.TrigDecisionTool = ToolSvc.TrigDecisionTool
ToolSvc += TrigAccessTool_default()

from TriggerJobOpts.TriggerEDM import TriggerL2List, TriggerEFList, EDMDetails

## class RoILinksCnvTool_default(RoILinksCnvTool):
##     def __init__(self, name='RoILinksCnvTool'):
##         RoILinksCnvTool.__init__(self, name)
##         self.TrigAccessTool = ToolSvc.TrigAccessTool
##         self.TrigDecisionTool = ToolSvc.TrigDecisionTool
##         self.OutputLevel = 2
## #ToolSvc += RoILinksCnvTool_default()
        
class ReadTriggerInfo_default(ReadTriggerInfo):
    def __init__(self, name='ReadTriggerInfo'):
        ReadTriggerInfo.__init__(self, name)
        self.TrigAccessTool = ServiceMgr.ToolSvc.TrigAccessTool

class TestNewTDT_default(TestNewTDT):
    def __init__(self, name='TestNewTDT'):
        TestNewTDT.__init__(self, name)
        #self.TrigDecisionTool = ServiceMgr.ToolSvc.TrigDecisionTool

class JustInitTDTAlgo_default(JustInitTDTAlgo):
    def __init__(self, name='JustInitTDTAlgo'):
        JustInitTDTAlgo.__init__(self, name)
        self.TrigAccessTool = ToolSvc.TrigAccessTool
        
class TrigMenuNtupleAlg_default(TrigMenuNtupleAlg):
    def __init__(self, name='TrigMenuNtupleAlg'):
        TrigMenuNtupleAlg.__init__(self, name)
        self.TrigAccessTool = ToolSvc.TrigAccessTool
        self.TrigDecisionTool = ToolSvc.TrigDecisionTool
        # self.RoILinksCnvTool = ToolSvc.RoILinksCnvTool
        
        
