## Add RadLengthIntegrator to the UserActions, and produce a histogram

from G4AtlasServices.G4AtlasUserActionConfig import UAStore
UAStore.addAction('LengthIntegrator',['BeginOfEvent','EndOfEvent','Step'])


from AthenaCommon.AppMgr import ServiceMgr
from GaudiSvc.GaudiSvcConf import THistSvc
ServiceMgr += THistSvc("THistSvc")
ServiceMgr.THistSvc.Output  = ["lengths DATAFILE='rad_intLength.histo.root' OPT='RECREATE'"]
