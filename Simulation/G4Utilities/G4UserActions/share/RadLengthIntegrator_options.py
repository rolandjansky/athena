## Add RadLengthIntegrator to the UserActions, and produce a histogram

def add_radlengthaction():
    from G4AtlasApps import PyG4Atlas, AtlasG4Eng
    radLengthAction = PyG4Atlas.UserAction( 'G4UserActions','RadLengthIntegrator',['BeginOfEvent','EndOfEvent','Step'])
    AtlasG4Eng.G4Eng.menu_UserActions.add_UserAction(radLengthAction)

from G4AtlasApps.SimFlags import SimFlags
SimFlags.InitFunctions.add_function("postInit", add_radlengthaction)

from AthenaCommon.AppMgr import ServiceMgr
from GaudiSvc.GaudiSvcConf import THistSvc
ServiceMgr += THistSvc("THistSvc")
ServiceMgr.THistSvc.Output  = ["radLen DATAFILE='radLength.histo.root' OPT='RECREATE'"]
