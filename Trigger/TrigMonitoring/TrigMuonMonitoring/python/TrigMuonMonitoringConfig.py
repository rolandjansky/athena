# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration


def TrigMuonMonitoringTool():
        from TrigMuonMonitoring.MuZTPChains import GetMuZTPChains
        from TrigMuonMonitoring.TrigMuonMonitoringConf import HLTMuonMonTool
        HLTMuonMon = HLTMuonMonTool(name                 = 'HLTMuonMon',
                                    histoPathBase        = "/Trigger/HLT",
				    ZTPPtCone20RelCut    = 0.1)
        HLTMuonMon.chainsGeneric = ['mu4_cosmic_L1MU4_EMPTY','mu4_cosmic_L1MU11_EMPTY']
        HLTMuonMon.chainsMSonly= ['mu4_msonly_cosmic_L1MU11_EMPTY']

        from TrigBunchCrossingTool.BunchCrossingTool import BunchCrossingTool	
        HLTMuonMon.BCTool = BunchCrossingTool()
        from AthenaCommon.AppMgr import ToolSvc
        ToolSvc += HLTMuonMon;
        list = [ "HLTMuonMonTool/HLTMuonMon" ];
        return list


