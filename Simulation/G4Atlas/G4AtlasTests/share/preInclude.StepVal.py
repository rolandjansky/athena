#from PerfMonComps.PerfMonFlags import jobproperties
#jobproperties.PerfMonFlags.doMonitoring = True
#jobproperties.PerfMonFlags.doMallocMonitoring = True
#jobproperties.PerfMonFlags.doPostProcessing = True

## Detector flags
from AthenaCommon.DetFlags import DetFlags
DetFlags.ID_setOn()
DetFlags.Calo_setOff()
DetFlags.Muon_setOff()
DetFlags.Truth_setOn()

## Simulation flags
from G4AtlasApps.SimFlags import simFlags
## Do not want any vertex spread for this job
simFlags.EventFilter.set_Off()

simFlags.CalibrationRun.set_Off()

## AthenaCommon flags
from AthenaCommon.AthenaCommonFlags import athenaCommonFlags
athenaCommonFlags.PoolEvgenInput.set_Off()
athenaCommonFlags.SkipEvents.set_Off()

from math import pi

## Use single particle generator
import AthenaCommon.AtlasUnixGeneratorJob
spgorders = ['pdgcode: sequence 11',
             'vertX: constant 0.0',
             'vertY: constant 0.0',
             'vertZ: constant 0.0',
             't: constant 0.0',
             'eta: flat 0.8 1.0',
             'phi: flat  0 %f'%(2*pi),
             'energy: constant 15000']
from ParticleGenerator.ParticleGeneratorConf import ParticleGenerator
## Algorithm sequence
from AthenaCommon.AlgSequence import AlgSequence
job = AlgSequence()
job += ParticleGenerator()
job.ParticleGenerator.orders = sorted(spgorders)
job.ParticleGenerator.AtRndmGenSvc = simFlags.RandomSvc.get_Value()

## Release GeoModel memory once sim is configured
#simFlags.ReleaseGeoModel = False

from AthenaCommon.AppMgr import ServiceMgr
if not hasattr(ServiceMgr, 'THistSvc'):
    from GaudiSvc.GaudiSvcConf import THistSvc
    ServiceMgr += THistSvc("THistSvc")
    ServiceMgr.THistSvc.Output  = ["truth DATAFILE='stepping.root' OPT='NEW'"];

