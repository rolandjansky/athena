# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

def HLTGeneralTool():
    from AthenaCommon.AppMgr import ToolSvc
    from TrigHLTMonitoring.TrigHLTMonitoringConf import HLTMonTool
    HLTMon = HLTMonTool(    name               = 'HLTMon',
                            histoPathBase      = "/Trigger/HLT/ResultMon",
                            RegExes = {
                                'AllChains' : '(L2|EF)_.*', 
                                'Electrons' : '(L2|EF)_e[0-9]+.*',
                                'Gamma'     : '(L2|EF)_g[0-9]+.*',
                                'Muons'     : '(L2|EF)_[0-9]*mu[0-9]+.*',
                                'MinBias'   : '(L2|EF)_mb.*',
                                'MissingET' : '(L2|EF)_(t|x)e[0-9]+.*',
                                'Taus'      : '(L2|EF)_(tau[0-9]*|trk.*Tau).*',
                                'Jets'      : '(L2|EF)_(FJ|j)[0-9]+.*'
                                },
                            LumiKey = 'LumiBlocks'
                            )
    ToolSvc += HLTMon
    list = [ "HLTMonTool/HLTMon" ]
    return list
