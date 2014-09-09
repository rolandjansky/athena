# @file AthenaServices/AthenaJobOptionsSvc_jobOptions.py
# @purpose simple fragment to inject the special Athena::JobOptionsSvc

import AthenaCommon.CfgMgr as CfgMgr
from AthenaCommon.AppMgr import ServiceMgr as svcMgr, theApp
if hasattr(svcMgr, 'JobOptionsSvc'):
    del svcMgr.JobOptionsSvc
    import AthenaCommon.Configurable as _acc
    try:
        del _acc.Configurable.allConfigurables['JobOptionsSvc']
    except KeyError:
        pass
svcMgr += CfgMgr.Athena__JobOptionsSvc('JobOptionsSvc')
theApp.JobOptionsSvcType = 'Athena::JobOptionsSvc'

## EOF ##

