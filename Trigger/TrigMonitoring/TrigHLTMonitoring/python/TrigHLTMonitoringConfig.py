# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

def HLTGeneralTool():
    from AthenaCommon.AppMgr import ToolSvc
    from TrigHLTMonitoring.TrigHLTMonitoringConf import HLTMonTool
    HLTMon = HLTMonTool(    name               = 'HLTMon',
                            histoPathBase      = "/Trigger/HLT/ResultMon",
                            RegExes = {
                                'AllChains' : 'HLT_.*', 
                                'Electrons' : 'HLT_e[0-9]+.*',
                                'Gamma'     : 'HLT_g[0-9]+.*',
                                'Muons'     : 'HLT_[0-9]*mu[0-9]+.*',
                                'MinBias'   : 'HLT_mb.*',
                                'MissingET' : 'HLT_(t|x)e[0-9]+.*',
                                'Taus'      : 'HLT_(tau[0-9]*|trk.*Tau).*',
                                'Jets'      : 'HLT_(FJ|j)[0-9]+.*'
                                },
                            LumiKey = 'LumiBlocks'
                            )
    ToolSvc += HLTMon
    list = [ "HLTMonTool/HLTMon" ]
    return list
