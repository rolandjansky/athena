
#disable DCS for MagField??
#MagFieldAthenaSvc = ServiceMgr.MagFieldAthenaSvc
#MagFieldAthenaSvc.UseDCS = False

from G4AtlasApps.SimFlags import SimFlags
#SimFlags.load_atlas_flags()
#get currents from COOL
#MagFieldAthenaSvc.NameOfTheSource="COOL"
SimFlags.MagneticField="AtlasFieldSvc"

