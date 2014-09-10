# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration


from AthenaCommon.AppMgr import ToolSvc
def TrigXAODBphysMonitoringTool():
    from TrigBphysMonitoring.TrigBphysMonitoringConf import HLTXAODBphysMonTool
    HLTXAODBphysMon = HLTXAODBphysMonTool(name       = 'HLTXAODBphysMon',
                                  histoPathBase      = "/Trigger/HLT",
                                  TrigNames          = ["2mu6_DiMu"],
                                  TrigNames1D        = [True],
                                  BphysicsItems      = ["HLT_2mu6_bDimu","HLT_2mu6_bDimu_novtx_noos","HLT_2mu6_bJpsimumu","HLT_2mu6_bUpsimumu","HLT_2mu10_bJpsimumu"]
                                  )
    from AthenaCommon.AppMgr import ToolSvc
    ToolSvc += HLTXAODBphysMon;
    list = [ "HLTXAODBphysMonTool/HLTXAODBphysMon" ];
    return list

def TrigBphysMonitoringTool():
        print "WARNING: Update this method: TrigBphysMonitoringTool"
        return TrigXAODBphysMonitoringTool()
        #from TrigBphysMonitoring.TrigBphysMonitoringConf import HLTBphysMonTool
        #HLTBphysMon = HLTBphysMonTool(name               = 'HLTBphysMon',
        #                              histoPathBase      = "/Trigger/HLT",
        #                              TrigNames          = ["2mu6_DiMu"],
        #                              TrigNames1D        = [True],
        #                              )
        #from AthenaCommon.AppMgr import ToolSvc
        #ToolSvc += HLTBphysMon;
        #list = [ "HLTBphysMonTool/HLTBphysMon" ];
        #return list



