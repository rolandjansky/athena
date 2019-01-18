## Histogram Service
from AthenaCommon.AppMgr import ServiceMgr
if not hasattr(ServiceMgr, 'THistSvc'):
    from GaudiSvc.GaudiSvcConf import THistSvc
    ServiceMgr += THistSvc()
ServiceMgr.THistSvc.Output  = ["stepHisto DATAFILE='StepHistograms.root' OPT='RECREATE'"];
print ServiceMgr.THistSvc

## User Actions
from G4AtlasApps.SimFlags import simFlags
simFlags.OptionalUserActionList.addAction('G4UA::StepHistogramTool', ['Step'])
simFlags.UserActionConfig.addConfig('G4UA::StepHistogramTool', 'doGeneralHistograms', True)
simFlags.UserActionConfig.addConfig('G4UA::StepHistogramTool', 'do2DHistograms', False)
