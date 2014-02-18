
from LArG4FastSimSvc.LArG4FastSimSvcInit import LArG4FastSimSvcInit

from G4AtlasApps.SimFlags import SimFlags
SimFlags.import_JobProperties('G4AtlasApps.atlas_flags')
#from G4AtlasApps import AtlasG4Eng
SimFlags.LArParameterization = 2
LArG4FastSimSvcInit()

theApp.EvtMax = 1

# get service manager
from AthenaCommon.AppMgr import ServiceMgr
print ServiceMgr
