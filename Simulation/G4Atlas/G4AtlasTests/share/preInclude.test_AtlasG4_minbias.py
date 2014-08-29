## Output printout level
ServiceMgr.MessageSvc.OutputLevel = INFO

from PerfMonComps.PerfMonFlags import jobproperties
jobproperties.PerfMonFlags.doMonitoring = True

## Detector flags
from AthenaCommon.DetFlags import DetFlags
DetFlags.ID_setOn()
DetFlags.LAr_setOff()
DetFlags.Tile_setOff()
DetFlags.Muon_setOff()
DetFlags.Truth_setOn()

## Simulation flags
from G4AtlasApps.SimFlags import simFlags
## Enable the EtaPhi, VertexSpread and VertexRange checks
simFlags.EventFilter.set_On()
simFlags.EventFilter.get_Value()['BeamEffectTransformation'] = True

def verbose_beameffecttransformation():
    print "verbose BeamEffectTransformation"
    from G4AtlasApps import AtlasG4Eng
    AtlasG4Eng.G4Eng.menu_EventFilter().getFilter('BeamEffectTransformation').SetVerboseLevel(99)

#simFlags.InitFunctions.add_function("preInitG4", verbose_beameffecttransformation)

## Need to keep GeoModel around for plotting the HITS
simFlags.ReleaseGeoModel = False
