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
athenaCommonFlags.EvtMax = 3

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
try:
    from G4AtlasApps.callbacks import do_recursive_geometry_test
    do_recursive_geometry_test()
except:
    ## Use the G4 UI commands via a callback function at level 1
    def setup_g4geo():
        from G4AtlasApps import AtlasG4Eng
        AtlasG4Eng.G4Eng._ctrl.G4Command("/run/initialize")
        AtlasG4Eng.G4Eng._ctrl.G4Command("/geometry/test/recursion_start 0")
        AtlasG4Eng.G4Eng._ctrl.G4Command("/geometry/test/recursion_depth 2")
        AtlasG4Eng.G4Eng._ctrl.G4Command("/geometry/test/recursive_test")
        ## Turn off processes other than transport
        # TODO! Do this using the UI commands
        print "WARNING: the fast physics list is dead! We need to tell this JO to run particle transport only"
    simFlags.InitFunctions.add_function("preInitG4", setup_g4geo)

## Exit before instantiation to level 2
def force_exit():
    from AthenaCommon.AppMgr import theApp
    theApp.exit(0)
simFlags.InitFunctions.add_function("preInitG4", force_exit)

## Add app to alg sequence
from G4AtlasApps.PyG4Atlas import PyG4AtlasAlg
job += PyG4AtlasAlg()
from AthenaCommon.CfgGetter import getAlgorithm
job += getAlgorithm("G4AtlasAlg",tryDefaultConfigurable=True)
