# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration


def TrigTauMonitoringTool():
        from TrigTauMonitoring.TrigTauMonitoringConf import HLTTauMonTool
        HLTTauMon = HLTTauMonTool(name                   = 'HLTTauMon',
                                  histoPathBase          = "/Trigger/HLT",
                                  tauTriggItem           = ["tauNoCut",
                                                            "tauNoCut_L1TAU40",
							    "tau20_medium1",
                                                            #ccuenca: for cosmics mon, not useless ;)
                                                            "tauNoCut_cosmic"
                                                            ],
                                  FakeJetEff             = False,
                                  OfflineEff             = True,
				  OfflineEffBDTMedium    = True,
				  EffOffTauPtCut         = 30.0,
				  TracksInfo             = True);
        from AthenaCommon.AppMgr import ToolSvc
        ToolSvc += HLTTauMon;
        list = [ "HLTTauMonTool/HLTTauMon" ];
        return list
from TriggerJobOpts.TriggerConfigGetter import TriggerConfigGetter
