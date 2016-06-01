# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

def HLTGeneralTool():
    from AthenaCommon.AppMgr import ToolSvc
    from TrigHLTMonitoring.TrigHLTMonitoringConf import HLTMonTool
    HLTMon = HLTMonTool(    name               = 'HLTMon',
                            histoPathBase      = "/Trigger/HLT/ResultMon",
                            IgnoreTruncationCheck = True,
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
                            # same as RegExes, but for Run-1;
                            # this is only useful for processing Run-1 data 
                            Run1RegExes = {
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
