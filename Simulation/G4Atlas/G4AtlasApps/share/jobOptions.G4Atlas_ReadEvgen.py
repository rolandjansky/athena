## Job options file for Geant4 ATLAS detector simulations

## Algorithm sequence
from AthenaCommon.AlgSequence import AlgSequence
topSeq = AlgSequence()

## Output threshold (DEBUG, INFO, WARNING, ERROR, FATAL)
#ServiceMgr.MessageSvc.OutputLevel = INFO

## Detector flags
from AthenaCommon.DetFlags import DetFlags
DetFlags.ID_setOn()
DetFlags.Calo_setOn()
DetFlags.Muon_setOn()
#DetFlags.Lucid_setOn()
DetFlags.Truth_setOn()

## Global conditions tag
from AthenaCommon.GlobalFlags import jobproperties
jobproperties.Global.ConditionsTag = "OFLCOND-RUN12-SDR-21"

## AthenaCommon flags
from AthenaCommon.AthenaCommonFlags import athenaCommonFlags
athenaCommonFlags.PoolEvgenInput = ['/afs/cern.ch/atlas/offline/ProdData/15.6.11.3/mu_E200_eta0-60-10000.evgen.pool.root']
athenaCommonFlags.PoolHitsOutput = "test.HITS.pool.root"
athenaCommonFlags.EvtMax = 3

## Simulation flags
from G4AtlasApps.SimFlags import simFlags
simFlags.load_atlas_flags()
#simFlags.RandomSvc = 'AtDSFMTGenSvc'

## Layout tags: see simFlags.SimLayout for allowed values
## Use the default layout:
simFlags.SimLayout.set_On()
## Set a specific layout tag:
simFlags.SimLayout='ATLAS-R2-2015-03-01-00'
## Set a specific non-officially-supported layout tag using the _VALIDATION suffix:
#simFlags.SimLayout = 'ATLAS-R2-2015-03-01-00_VALIDATION'
## Set a matching RunNumber for run-dependent conditions. See:
## https://twiki.cern.ch/twiki/bin/viewauth/AtlasComputing/ConditionsRun1RunNumbers
simFlags.RunNumber = 222510

## Set the EtaPhi, VertexSpread and VertexRange checks on
simFlags.EventFilter.set_On()

## Set the LAr parameterization
#simFlags.LArParameterization = 2

## No magnetic field
#simFlags.MagneticField.set_Off()

## Register callback functions at various init stages
#def test_preInit():
#    print "CALLBACK AT PREINIT"
#def test_postInit():
#    print "CALLBACK AT POSTINIT"
#simFlags.InitFunctions.add_function("preInit", test_preInit)
#simFlags.InitFunctions.add_function("postInit", test_postInit)

## Change the field stepper or use verbose G4 tracking
#from G4AtlasApps import callbacks
#callbacks.use_simplerunge_stepper()
#callbacks.use_verbose_tracking()

from AthenaCommon.CfgGetter import getAlgorithm
topSeq += getAlgorithm("BeamEffectsAlg", tryDefaultConfigurable=True)

## Add the G4 sim to the alg sequence
from G4AtlasApps.PyG4Atlas import PyG4AtlasAlg
topSeq += PyG4AtlasAlg()
from AthenaCommon.CfgGetter import getAlgorithm
topSeq += getAlgorithm("G4AtlasAlg",tryDefaultConfigurable=True)
