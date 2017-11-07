## Job options file for Geant4 Simulations

## Algorithm sequence
from AthenaCommon.AlgSequence import AlgSequence
topSeq = AlgSequence()

## ServiceMgr
from AthenaCommon.AppMgr import ServiceMgr

## Output printout level
ServiceMgr.MessageSvc.OutputLevel = INFO

## Detector flags
from AthenaCommon.DetFlags import DetFlags
DetFlags.ID_setOn()
DetFlags.LAr_setOn()
DetFlags.Tile_setOff()
DetFlags.Muon_setOff()
DetFlags.Truth_setOn()

## AthenaCommon flags
from AthenaCommon.AthenaCommonFlags import athenaCommonFlags
#athenaCommonFlags.PoolHitsOutput.set_Off()
athenaCommonFlags.PoolHitsOutput = "test.HITS.pool.root"

if 'EvtMax' not in dir():
   EvtMax = 201
athenaCommonFlags.EvtMax = EvtMax

## Set global conditions tag
from AthenaCommon.GlobalFlags import globalflags
globalflags.ConditionsTag = "OFLCOND-MC12-SIM-00"

## Simulation flags
from G4AtlasApps.SimFlags import simFlags
simFlags.load_atlas_flags()
simFlags.SimLayout.set_On()
simFlags.SimLayout='ATLAS-GEO-20-00-01_VALIDATION'
simFlags.RandomSeedOffset=12  # use different events then SimCoreTests
simFlags.RandomSvc='AtDSFMTGenSvc'


## Enable the EtaPhi, VertexSpread and VertexRange checks
simFlags.EventFilter.set_On()

## Use single particle generator
athenaCommonFlags.PoolEvgenInput.set_Off()
athenaCommonFlags.SkipEvents.set_Off()
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
topSeq += ParticleGenerator()
topSeq.ParticleGenerator.orders = sorted(spgorders)

## Do not release GeoModel memory once sim is configured (needed by G4TestAlg)
simFlags.ReleaseGeoModel = False


## Add the G4 sim to the alg sequence after the generator
from G4AtlasApps.PyG4Atlas import PyG4AtlasAlg
topSeq += PyG4AtlasAlg()
from AthenaCommon.CfgGetter import getAlgorithm
topSeq += getAlgorithm("G4AtlasAlg",tryDefaultConfigurable=True)


## User algorithms
from GaudiSvc.GaudiSvcConf import THistSvc
ServiceMgr += THistSvc("THistSvc")
ServiceMgr.THistSvc.Output  = ["truth DATAFILE='truth.root' OPT='RECREATE'"];


from G4AtlasTests.G4AtlasTestsConf import G4TestAlg
topSeq += G4TestAlg()
G4TestAlg.SubAlgorithms =  [ "TruthTestAlg/Truth" ]
G4TestAlg.SubAlgorithms += [ "SiHitsTestAlg/Pixel" ]
G4TestAlg.SubAlgorithms += [ "SiHitsTestAlg/SCT" ]
G4TestAlg.SubAlgorithms += [ "TrtHitsTestAlg/TRT" ]
G4TestAlg.SubAlgorithms += [ "LArHitsTestAlg/EMB" ]
G4TestAlg.SubAlgorithms += [ "LArHitsTestAlg/EMEC" ]
G4TestAlg.SubAlgorithms += [ "LArHitsTestAlg/FCAL" ]
G4TestAlg.SubAlgorithms += [ "LArHitsTestAlg/HEC" ]
#G4TestAlg.SubAlgorithms += [ "TileHitsTestAlg/Tile" ] 
#G4TestAlg.SubAlgorithms += [ "MDTHitsTestAlg/MDT" ]
#G4TestAlg.SubAlgorithms += [ "RPCHitsTestAlg/RPC" ]
#G4TestAlg.SubAlgorithms += [ "CSCHitsTestAlg/CSC" ]
#G4TestAlg.SubAlgorithms += [ "TGCHitsTestAlg/TGC" ]


# Control random number seeds so that daily runs are identical
from G4AtlasAlg.G4AtlasAlgConf import G4AtlasAlg
g4AtlasAlg = G4AtlasAlg()
g4AtlasAlg.RandomGenerator = "athena"
g4AtlasAlg.AtRndmGenSvc = simFlags.RandomSvc.get_Value()

# from AthenaServices.AthenaServicesConf import AtRndmGenSvc
# atRndmGenSvc = AtRndmGenSvc()
# atRndmGenSvc.Seeds += ["AtlasG4 1234 5678"]
# atRndmGenSvc.Seeds += ["SINGLE 2345 6789"]
# atRndmGenSvc.OutputLevel = WARNING
# ServiceMgr += atRndmGenSvc

from AthenaCommon.ConfigurationShelve import saveToAscii
saveToAscii("config.txt")
