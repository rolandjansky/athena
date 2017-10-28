## Job options file for Geant4 Simulations

from AthenaCommon.AlgSequence import AlgSequence
job = AlgSequence()

## Output printout level
ServiceMgr.MessageSvc.OutputLevel = WARNING

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
athenaCommonFlags.PoolHitsOutput = 'atlas_MyOutputFile.root'
athenaCommonFlags.EvtMax = 1000

## Simulation flags
from G4AtlasApps.SimFlags import simFlags
simFlags.load_atlas_flags()
simFlags.SimLayout.set_On() # use the default value

## Set global conditions tag
from AthenaCommon.GlobalFlags import jobproperties
jobproperties.Global.ConditionsTag = "OFLCOND-RUN12-SDR-19"

## Use single particle generator
from AthenaCommon.AthenaCommonFlags import athenaCommonFlags
athenaCommonFlags.PoolEvgenInput.set_Off()
athenaCommonFlags.SkipEvents.set_Off()
import AthenaCommon.AtlasUnixGeneratorJob
spgorders = ['pdgcode: constant 11',
             'vertX: constant 0.0',
             'vertY: constant 0.0',
             'vertZ: constant 0.0',
             't: constant 0.0',
             'eta: flat -3.0 3.0',
             'phi: flat  0 6.28318',
             'e: constant 10000']
from ParticleGenerator.ParticleGeneratorConf import ParticleGenerator
job += ParticleGenerator()
job.ParticleGenerator.orders = sorted(spgorders)
job.ParticleGenerator.AtRndmGenSvc = simFlags.RandomSvc.get_Value()

## Enable the EtaPhi, VertexSpread and VertexRange checks
simFlags.EventFilter.set_On()

#simFlags.LArParameterization = 2

## No magnetic field
simFlags.MagneticField.set_Off()

## Don't release geometry (still needed)
simFlags.ReleaseGeoModel = False

## Add sim framework to the alg sequence (sim pre-init happens on construction)
from G4AtlasApps.PyG4Atlas import PyG4AtlasAlg
job += PyG4AtlasAlg()
from AthenaCommon.CfgGetter import getAlgorithm
job += getAlgorithm("G4AtlasAlg",tryDefaultConfigurable=True)

# User algorithms
from AthenaCommon.AppMgr import ServiceMgr
from GaudiSvc.GaudiSvcConf import THistSvc
ServiceMgr += THistSvc("THistSvc")
ServiceMgr.THistSvc.Output  = ["truth DATAFILE='truth.root' OPT='RECREATE'"];

from AthenaCommon.AlgSequence import AlgSequence
job = AlgSequence()

from G4AtlasTests.G4AtlasTestsConf import G4TestAlg
job += G4TestAlg()
from AthenaCommon import CfgGetter
job.G4TestAlg.SimTestTools += [CfgGetter.getPrivateTool("PixelHitsTestTool", checkType=True)]
job.G4TestAlg.SimTestTools += [CfgGetter.getPrivateTool("SCT_HitsTestTool", checkType=True)]
job.G4TestAlg.SimTestTools += [CfgGetter.getPrivateTool("TrtHitsTestTool", checkType=True)]
job.G4TestAlg.SimTestTools += [CfgGetter.getPrivateTool("EMBHitsTestTool", checkType=True)]
job.G4TestAlg.SimTestTools += [CfgGetter.getPrivateTool("EMECHitsTestTool", checkType=True)]
job.G4TestAlg.SimTestTools += [CfgGetter.getPrivateTool("FCALHitsTestTool", checkType=True)]
job.G4TestAlg.SimTestTools += [CfgGetter.getPrivateTool("HECHitsTestTool", checkType=True)]
job.G4TestAlg.SimTestTools += [CfgGetter.getPrivateTool("TileHitsTestTool", checkType=True)]
job.G4TestAlg.SimTestTools += [CfgGetter.getPrivateTool("MDTHitsTestTool", checkType=True)]
job.G4TestAlg.SimTestTools += [CfgGetter.getPrivateTool("RPCHitsTestTool", checkType=True)]
job.G4TestAlg.SimTestTools += [CfgGetter.getPrivateTool("CSCHitsTestTool", checkType=True)]
job.G4TestAlg.SimTestTools += [CfgGetter.getPrivateTool("TGCHitsTestTool", checkType=True)]
job.G4TestAlg.SimTestTools += [CfgGetter.getPrivateTool("TruthTestTool", checkType=True)]

# Control random number seeds to ensure identical runs from day to day
from G4AtlasAlg.G4AtlasAlgConf import G4AtlasAlg
g4AtlasAlg = G4AtlasAlg()
g4AtlasAlg.RandomGenerator = "athena"
from AthenaCommon.AppMgr import ServiceMgr
from AthenaServices.AthenaServicesConf import AtRndmGenSvc
atRndmGenSvc = AtRndmGenSvc()
ServiceMgr += atRndmGenSvc
atRndmGenSvc.Seeds += ["AtlasG4 1234 5678"]
atRndmGenSvc.Seeds += ["SINGLE 2345 6789"]
atRndmGenSvc.OutputLevel = INFO

## Check of memory during the RTT tests
from G4AtlasApps.atlas_utilities import MemorySnooper
job += MemorySnooper()

## For saving seeds
#from G4AtlasApps import AtlasG4Eng
#pp = AtlasG4Eng.G4Eng.menu_G4RandomNrMenu()
#pp.set_SaveOn()
