## Add RadLengthIntegrator to the UserActions, and produce a histogram
from G4AtlasApps.SimFlags import simFlags
if hasattr(simFlags, 'UseV2UserActions') and simFlags.UseV2UserActions():
    simFlags.OptionalUserActionList.addAction('G4UA::LengthIntegratorTool',['BeginOfEvent','EndOfEvent','Step'])
    
else:
    # V1 userActions
    try:
        from G4AtlasServices.G4AtlasUserActionConfig import UAStore
    except ImportError:
        from G4AtlasServices.UserActionStore import UAStore
        
    UAStore.addAction('LengthIntegrator',['BeginOfEvent','EndOfEvent','Step'])
        

from AthenaCommon.AppMgr import ServiceMgr
from GaudiSvc.GaudiSvcConf import THistSvc
ServiceMgr += THistSvc("THistSvc")
ServiceMgr.THistSvc.Output  = ["lengths DATAFILE='rad_intLength.histo.root' OPT='RECREATE'"]
