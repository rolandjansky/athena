## Job options file for Geant4 Simulations

## Algorithm sequence
from AthenaCommon.AlgSequence import AlgSequence
job = AlgSequence()

## Output printout level
ServiceMgr.MessageSvc.OutputLevel = INFO

from PerfMonComps.PerfMonFlags import jobproperties
jobproperties.PerfMonFlags.doMonitoring = True

## Detector flags
from AthenaCommon.DetFlags import DetFlags
DetFlags.ID_setOn()
DetFlags.Calo_setOn()
DetFlags.Muon_setOn()
DetFlags.Truth_setOn()

## AthenaCommon flags
from AthenaCommon.AthenaCommonFlags import athenaCommonFlags
athenaCommonFlags.PoolHitsOutput.set_Off()
athenaCommonFlags.EvtMax = 500

## Set global conditions tag
from AthenaCommon.GlobalFlags import jobproperties
jobproperties.Global.ConditionsTag = "OFLCOND-RUN12-SDR-19"

## Simulation flags
from G4AtlasApps.SimFlags import simFlags
simFlags.load_atlas_flags()
simFlags.SimLayout.set_On()

## Enable the EtaPhi, VertexSpread and VertexRange checks
simFlags.EventFilter.set_On()

## Use single particle generator
from AthenaCommon.AthenaCommonFlags import athenaCommonFlags
athenaCommonFlags.PoolEvgenInput.set_Off()
athenaCommonFlags.SkipEvents.set_Off()
import AthenaCommon.AtlasUnixGeneratorJob
spgorders = ['pdgcode: sequence 13 -13 211 -211',
             'vertX: constant 0.0',
             'vertY: constant 0.0',
             'vertZ: constant 0.0',
             't: constant 0.0',
             'eta: flat -4.0 4.0',
             'phi: flat  0 6.28318',
             'pt: constant 50000']
from ParticleGenerator.ParticleGeneratorConf import ParticleGenerator
job += ParticleGenerator()
job.ParticleGenerator.orders = sorted(spgorders)
job.ParticleGenerator.AtRndmGenSvc = simFlags.RandomSvc.get_Value()

## Release GeoModel memory once sim is configured
simFlags.ReleaseGeoModel = False


## Add the G4 sim to the alg sequence after the generator
from G4AtlasApps.PyG4Atlas import PyG4AtlasAlg
job += PyG4AtlasAlg()


## User algorithms
from AthenaCommon.AppMgr import ServiceMgr
from GaudiSvc.GaudiSvcConf import THistSvc
ServiceMgr += THistSvc("THistSvc")
ServiceMgr.THistSvc.Output  = ["atlasTest DATAFILE='atlasTest.histo.root' OPT='RECREATE'"];

from G4AtlasTests.G4AtlasTestsConf import AtlasTestAlg
job += AtlasTestAlg()

# Control random number seeds so that daily runs are identical
from G4AtlasAlg.G4AtlasAlgConf import G4AtlasAlg
g4AtlasAlg = G4AtlasAlg()
g4AtlasAlg.RandomGenerator = "athena"
from AthenaCommon.AppMgr import ServiceMgr
from AthenaServices.AthenaServicesConf import AtRndmGenSvc
atRndmGenSvc = AtRndmGenSvc()
atRndmGenSvc.Seeds += ["AtlasG4 1234 5678"]
atRndmGenSvc.Seeds += ["SINGLE 2345 6789"]
atRndmGenSvc.OutputLevel = WARNING
ServiceMgr += atRndmGenSvc

## For saving seeds
#from G4AtlasApps import AtlasG4Eng
#pp = AtlasG4Eng.G4Eng.menu_G4RandomNrMenu()
#pp.set_SaveOn()

## Check of memory during the RTT tests
from G4AtlasApps.atlas_utilities import MemorySnooper
job += MemorySnooper()
