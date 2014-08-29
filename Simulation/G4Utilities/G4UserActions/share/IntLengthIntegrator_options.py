## Add InteractionLengthIntegrator to the UserActions, and histogram

def add_intlengthaction():
    from G4AtlasApps import PyG4Atlas, AtlasG4Eng
    intLengthAction = PyG4Atlas.UserAction( 'G4UserActions','InteractionLengthIntegrator', ['BeginOfEvent','EndOfEvent','Step'])
    AtlasG4Eng.G4Eng.menu_UserActions.add_UserAction(intLengthAction)

from G4AtlasApps.SimFlags import SimFlags
SimFlags.InitFunctions.add_function("postInit", add_intlengthaction)

from AthenaCommon.AppMgr import ServiceMgr
from GaudiSvc.GaudiSvcConf import THistSvc
ServiceMgr += THistSvc("THistSvc")
ServiceMgr.THistSvc.Output  = ["intLen DATAFILE='intLength.histo.root' OPT='RECREATE'"]
