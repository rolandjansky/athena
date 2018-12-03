## Job options file for Geant4 simulations

from PerfMonComps.PerfMonFlags import jobproperties
jobproperties.PerfMonFlags.doMonitoring = True

## Detector flags
from AthenaCommon.DetFlags import DetFlags
DetFlags.ID_setOn()
DetFlags.Calo_setOn()
DetFlags.Muon_setOn()
DetFlags.simulate.Truth_setOn()

## AthenaCommon flags
from AthenaCommon.AthenaCommonFlags import athenaCommonFlags
athenaCommonFlags.PoolHitsOutput.set_Off()
athenaCommonFlags.EvtMax = 500

## Global conditions tag
from AthenaCommon.GlobalFlags import globalflags
globalflags.ConditionsTag = "OFLCOND-RUN12-SDR-19"

## Simulation flags
from G4AtlasApps.SimFlags import simFlags
simFlags.load_atlas_flags()
simFlags.SimLayout.set_On() # uses the default value

## User algorithms
from AthenaCommon.AlgSequence import AlgSequence
job = AlgSequence()
## Use single particle generator
import AthenaCommon.AtlasUnixGeneratorJob
import ParticleGun as PG
pg = PG.ParticleGun()
pg.sampler.pid = PG.CyclicSeqSampler([-211,211,-13,13])
pg.sampler.mom = PG.EEtaMPhiSampler(energy=50000, eta=[-4,4])
job += pg
## job.ParticleGun.AtRndmGenSvc = simFlags.RandomSvc.get_Value()

include("G4AtlasApps/fragment.SimCopyWeights.py")

include("G4AtlasApps/G4Atlas.flat.configuration.py")

from AthenaCommon.CfgGetter import getAlgorithm
job += getAlgorithm("G4AtlasAlg",tryDefaultConfigurable=True)
from G4AtlasTests.G4AtlasTestsConf import AtlasTestAlg
job += AtlasTestAlg()

## Histogramming
from AthenaCommon.AppMgr import ServiceMgr
from GaudiSvc.GaudiSvcConf import THistSvc
ServiceMgr += THistSvc("THistSvc")
ServiceMgr.THistSvc.Output = ["atlasTest DATAFILE='atlasTest.histo.root' OPT='RECREATE'"];
