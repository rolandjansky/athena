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
DetFlags.LAr_setOff()
DetFlags.Tile_setOff()
DetFlags.Muon_setOff()
DetFlags.Truth_setOn()

## AthenaCommon flags
from AthenaCommon.AthenaCommonFlags import athenaCommonFlags
athenaCommonFlags.EvtMax = 50#0
athenaCommonFlags.PoolEvgenInput = ['/afs/cern.ch/atlas/offline/ProdData/16.6.X/16.6.7.Y/ttbar_muplusjets-pythia6-7000.evgen.pool.root']
athenaCommonFlags.PoolHitsOutput = "atlasG4.hits.pool.root"

## Set global conditions tag
from AthenaCommon.GlobalFlags import globalflags
globalflags.ConditionsTag = "OFLCOND-RUN12-SDR-19"
#globalflags.DetDescrVersion = 'ATLAS-R2-2015-03-01-00'

## Simulation flags
from G4AtlasApps.SimFlags import simFlags
simFlags.load_atlas_flags()
simFlags.SimLayout.set_On()
simFlags.SimLayout='ATLAS-R2-2015-03-01-00_VALIDATION'

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

## Add the G4 sim to the alg sequence after the generator
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
job.G4TestAlg.SimTestTools += [CfgGetter.getPrivateTool("TruthTestTool", checkType=True)]
job.G4TestAlg.SimTestTools += [CfgGetter.getPrivateTool("EvgenTruthTestTool", checkType=True)]
job.G4TestAlg.SimTestTools += [CfgGetter.getPrivateTool("PixelHitsTestTool", checkType=True)]
job.G4TestAlg.SimTestTools += [CfgGetter.getPrivateTool("SCT_HitsTestTool", checkType=True)]
job.G4TestAlg.SimTestTools += [CfgGetter.getPrivateTool("TrtHitsTestTool", checkType=True)]

## Check of memory during the RTT tests
from G4AtlasApps.atlas_utilities import MemorySnooper
job += MemorySnooper()
