# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration


def TrigMuonMonitoringTool():
        from TrigMuonMonitoring.MuZTPChains import GetMuZTPChains
        from TrigMuonMonitoring.TrigMuonMonitoringConf import HLTMuonMonTool
        HLTMuonMon = HLTMuonMonTool(name                 = 'HLTMuonMon',
                                    histoPathBase        = "/Trigger/HLT",
				    ChainsForZTP         = GetMuZTPChains(),
				    ZTPPtCone20RelCut    = 0.1)

        from TrigBunchCrossingTool.BunchCrossingTool import BunchCrossingTool	
        HLTMuonMon.BCTool = BunchCrossingTool()
        from AthenaCommon.AppMgr import ToolSvc
        ToolSvc += HLTMuonMon;
        list = [ "HLTMuonMonTool/HLTMuonMon" ];
        return list


