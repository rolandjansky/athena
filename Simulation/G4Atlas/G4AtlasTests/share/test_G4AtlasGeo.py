## Job options file for Geant4 Simulations

from AthenaCommon.AlgSequence import AlgSequence
job = AlgSequence()

## Output printout level
ServiceMgr.MessageSvc.OutputLevel = WARNING

## Detector flags
from AthenaCommon.DetFlags import DetFlags
DetFlags.ID_setOn()
DetFlags.Calo_setOn()
DetFlags.Muon_setOn()
#DetFlags.Truth_setOn()

## AthenaCommon flags
from AthenaCommon.AthenaCommonFlags import athenaCommonFlags
athenaCommonFlags.PoolHitsOutput = 'g4atlas.hits.pool.root'
athenaCommonFlags.EvtMax = 1

## Set global conditions tag
from AthenaCommon.GlobalFlags import jobproperties
jobproperties.Global.ConditionsTag = "OFLCOND-RUN12-SDR-19"

## Simulation flags
from G4AtlasApps.SimFlags import simFlags
simFlags.load_atlas_flags()
simFlags.SimLayout.set_On()
simFlags.SimLayout='ATLAS-R2-2015-03-01-00_VALIDATION'
simFlags.RunNumber=222525

## Use single particle generator
from AthenaCommon.AthenaCommonFlags import athenaCommonFlags
athenaCommonFlags.PoolEvgenInput.set_Off()
athenaCommonFlags.SkipEvents.set_Off()
import AthenaCommon.AtlasUnixGeneratorJob
import ParticleGun as PG
pg = PG.ParticleGun(randomSvcName=simFlags.RandomSvc.get_Value(), randomStream="SINGLE")
pg.sampler.pid = PG.CyclicSeqSampler([-11,11])
pg.sampler.mom = PG.PtEtaMPhiSampler(pt=10000, eta=[-3,3])
job += pg

include("G4AtlasApps/fragment.SimCopyWeights.py")

## Enable the EtaPhi, VertexSpread and VertexRange checks
simFlags.EventFilter.set_On()

## Set up the geometry test
from G4AtlasApps.callbacks import do_recursive_geometry_test
do_recursive_geometry_test()

include("G4AtlasApps/G4Atlas.flat.configuration.py")

from AthenaCommon.CfgGetter import getAlgorithm
topSeq += getAlgorithm("BeamEffectsAlg", tryDefaultConfigurable=True)

## Add app to alg sequence
from AthenaCommon.CfgGetter import getAlgorithm
job += getAlgorithm("G4AtlasAlg",tryDefaultConfigurable=True)
