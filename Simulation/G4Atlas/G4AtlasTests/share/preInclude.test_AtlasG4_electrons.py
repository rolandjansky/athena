from PerfMonComps.PerfMonFlags import jobproperties
jobproperties.PerfMonFlags.doMonitoring = True

## Detector flags
from AthenaCommon.DetFlags import DetFlags
DetFlags.ID_setOn()
DetFlags.LAr_setOn()
DetFlags.Tile_setOff()
DetFlags.Muon_setOff()
DetFlags.Truth_setOn()

## Simulation flags
from G4AtlasApps.SimFlags import simFlags
## Enable the EtaPhi, VertexSpread and VertexRange checks
simFlags.EventFilter.set_On()

## AthenaCommon flags
from AthenaCommon.AthenaCommonFlags import athenaCommonFlags
athenaCommonFlags.PoolEvgenInput.set_Off()
athenaCommonFlags.SkipEvents.set_Off()

## Use single particle generator
import AthenaCommon.AtlasUnixGeneratorJob
spgorders = ['pdgcode: sequence 11 -11',
             'vertX: constant 0.0',
             'vertY: constant 0.0',
             'vertZ: constant 0.0',
             't: constant 0.0',
             'eta: flat -3.0 3.0',
             'phi: flat  0 6.28318',
             'pt: constant 10000']
from ParticleGenerator.ParticleGeneratorConf import ParticleGenerator
## Algorithm sequence
from AthenaCommon.AlgSequence import AlgSequence
job = AlgSequence()
job += ParticleGenerator()
job.ParticleGenerator.orders = sorted(spgorders)
job.ParticleGenerator.AtRndmGenSvc = simFlags.RandomSvc.get_Value()

## Release GeoModel memory once sim is configured
simFlags.ReleaseGeoModel = False
