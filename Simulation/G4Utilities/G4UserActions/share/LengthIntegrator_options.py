## Add RadLengthIntegrator to the UserActions, and produce a histogram
from G4AtlasApps.SimFlags import simFlags
from AtlasGeoModel.InDetGMJobProperties import InDetGeometryFlags as geoFlags
simFlags.OptionalUserActionList.addAction('G4UA::LengthIntegratorTool',['Event','Step'])
simFlags.UserActionConfig.addConfig('G4UA::LengthIntegratorTool','isITk',geoFlags.isSLHC())
#simFlags.UserActionConfig.addConfig('G4UA::LengthIntegratorTool','doElements',True) #Uncomment to split every volume by elements as well - will add more output in the ntuple



from AthenaCommon.AppMgr import ServiceMgr
from GaudiSvc.GaudiSvcConf import THistSvc
ServiceMgr += THistSvc("THistSvc")
ServiceMgr.THistSvc.Output  = ["lengths DATAFILE='LengthIntegrator.root' OPT='RECREATE'"]
