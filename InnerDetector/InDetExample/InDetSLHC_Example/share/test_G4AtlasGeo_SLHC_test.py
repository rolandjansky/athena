## Job options file for Geant4 Simulations

from AthenaCommon.AlgSequence import AlgSequence
topSeq = AlgSequence()

## Output printout level
ServiceMgr.MessageSvc.OutputLevel = WARNING

from AthenaCommon.GlobalFlags import globalflags
globalflags.ConditionsTag = "OFLCOND-SDR-BS14T-ATLAS-00"

include("InDetSLHC_Example/preInclude.SLHC.py")

include("InDetSLHC_Example/preInclude.SiliconOnly.py")

## AthenaCommon flags
from AthenaCommon.AthenaCommonFlags import athenaCommonFlags
athenaCommonFlags.PoolHitsOutput = 'g4atlas.hits.pool.root'
athenaCommonFlags.EvtMax = 3

## Simulation flags
from G4AtlasApps.SimFlags import simFlags
simFlags.load_atlas_flags()
simFlags.SimLayout='ATLAS-SLHC-01-02-01_VALIDATION'
simFlags.SimLayout.set_On()
simFlags.EventFilter.set_On()
#simFlags.init_Level.set_Value(1)


## Use single particle generator
from AthenaCommon.AthenaCommonFlags import athenaCommonFlags
athenaCommonFlags.PoolEvgenInput.set_Off()
athenaCommonFlags.SkipEvents.set_Off()
import AthenaCommon.AtlasUnixGeneratorJob
import ParticleGun as PG
pg = PG.ParticleGun(randomSvcName=simFlags.RandomSvc.get_Value(), randomStream="SINGLE")
pg.sampler.pid = 13
pg.sampler.mom = PG.PtEtaMPhiSampler(pt=10000, eta=[-3,3])
topSeq += pg

try:
    include("G4AtlasApps/fragment.SimCopyWeights.py")
except:
    from GeneratorModules.GeneratorModulesConf import CopyEventWeight
    topSeq += CopyEventWeight(TruthCollKey="GEN_EVENT")

## Enable the EtaPhi, VertexSpread and VertexRange checks
simFlags.EventFilter.set_On()

# ## Use the G4 UI commands via a callback function at level 1
# def setup_g4geo():
#     from G4AtlasApps import AtlasG4Eng

#     AtlasG4Eng.G4Eng._ctrl.G4Command("/run/initialize")
#     AtlasG4Eng.G4Eng._ctrl.G4Command("/geometry/test/recursion_start 0")
#     AtlasG4Eng.G4Eng._ctrl.G4Command("/geometry/test/recursion_depth 2")
#     AtlasG4Eng.G4Eng._ctrl.G4Command("/geometry/test/recursive_test")

#     ## Turn off processes other than transport
#     # TODO! Do this using the UI commands
#     print "WARNING: the fast physics list is dead! We need to tell this JO to run particle transport only"
# simFlags.InitFunctions.add_function(1, setup_g4geo)

def test_preInit():
    print "CALLBACK AT PREINIT"
def test_postInit():
    print "CALLBACK AT POSTINIT"
def use_geometry_check():
    print "CALLBACK use_geometry_check"
    from G4AtlasApps import AtlasG4Eng
    AtlasG4Eng.G4Eng._ctrl.geometryMenu.SetGeometryCheck(100)

simFlags.InitFunctions.add_function("preInitG4", use_geometry_check)
simFlags.InitFunctions.add_function("preInit", test_preInit)
simFlags.InitFunctions.add_function("postInit", test_postInit)

# ## Exit before instantiation to level 2
# def force_exit():
#     from AthenaCommon.AppMgr import theApp
#     theApp.exit(0)
# simFlags.InitFunctions.add_function(1, force_exit)

include("G4AtlasApps/G4Atlas.flat.configuration.py")

## Add app to alg sequence
from AthenaCommon.CfgGetter import getAlgorithm
topSeq += getAlgorithm("G4AtlasAlg",tryDefaultConfigurable=True)

include("InDetSLHC_Example/postInclude.SLHC_Setup.py")

