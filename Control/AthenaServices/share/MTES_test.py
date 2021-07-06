## Job options file for MT EventService standalone test
## using Geant4 ATLAS detector simulations as a base

## Algorithm sequence
from AthenaCommon.AlgSequence import AlgSequence
topSeq = AlgSequence()

## Output threshold (DEBUG, INFO, WARNING, ERROR, FATAL)
#ServiceMgr.MessageSvc.OutputLevel = INFO

## Detector flags
from AthenaCommon.DetFlags import DetFlags
DetFlags.ID_setOn()
DetFlags.Calo_setOff()
DetFlags.Muon_setOff()
#DetFlags.Lucid_setOn()
DetFlags.Truth_setOn()

## Global conditions tag
from AthenaCommon.GlobalFlags import jobproperties
jobproperties.Global.ConditionsTag = "OFLCOND-RUN12-SDR-21"

## AthenaCommon flags
from AthenaCommon.AthenaCommonFlags import athenaCommonFlags
athenaCommonFlags.PoolEvgenInput = ['/cvmfs/atlas-nightlies.cern.ch/repo/data/data-art/ISF_Validation/mu_E200_eta0-60-10000.evgen.pool.root']
athenaCommonFlags.PoolHitsOutput = "test.HITS.pool.root"
athenaCommonFlags.EvtMax = -1 

## Simulation flags
from G4AtlasApps.SimFlags import simFlags
simFlags.load_atlas_flags()

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

## Use single particle generator
from AthenaCommon.AthenaCommonFlags import athenaCommonFlags
athenaCommonFlags.SkipEvents.set_Off()

include("G4AtlasApps/G4Atlas.flat.configuration.py")

#from TruthExamples.TruthExamplesConf import DumpMC
#topSeq += DumpMC()

from AthenaCommon.CfgGetter import getAlgorithm
topSeq += getAlgorithm("BeamEffectsAlg", tryDefaultConfigurable=True)

## Add the G4 sim to the alg sequence after the generator
from AthenaCommon.CfgGetter import getAlgorithm
topSeq += getAlgorithm("G4AtlasAlg",tryDefaultConfigurable=True)
print( topSeq )

# Configure EventSerice incident name
from AthenaCommon.AppMgr import ServiceMgr as svcMgr
from AthenaServices.AthenaServicesConf import OutputStreamSequencerSvc
outputStreamSequencerSvc = OutputStreamSequencerSvc()
outputStreamSequencerSvc.SequenceIncidentName = "NextEventRange"
svcMgr += outputStreamSequencerSvc

# Define event ranges to process
from AthenaServices.AthenaServicesConf import AthenaMtesEventLoopMgr
AthenaMtesEventLoopMgr.OutputLevel = DEBUG
#mtes_input_dir = "u'/afs/cern.ch/user/t/tsulaia/workspace/public/Marcin/mc12_13TeV/"
mtes_input_dir = "u'/cvmfs/atlas-nightlies.cern.ch/repo/data/data-art/EventServiceTest/mc12_13TeV_"
mtes_inputfile1 = mtes_input_dir+"EVNT.01469903._009501.pool.root.1'"
mtes_inputfile2 = mtes_input_dir+"EVNT.01469903._009502.pool.root.1'"
AthenaMtesEventLoopMgr.ESTestPilotMessages = [
    "[{u'lastEvent': 1, u'eventRangeID': u'Range-1', u'startEvent': 1, u'PFN': "+mtes_inputfile1+"}]",
    "[{u'lastEvent': 2, u'eventRangeID': u'Range-2', u'startEvent': 2, u'PFN': "+mtes_inputfile2+"}]",
    "[{u'lastEvent': 3, u'eventRangeID': u'Range-3', u'startEvent': 3, u'PFN': "+mtes_inputfile2+"}]",
    "[{u'lastEvent': 4, u'eventRangeID': u'Range-4', u'startEvent': 4, u'PFN': "+mtes_inputfile1+"}]",
    "[{u'lastEvent': 5, u'eventRangeID': u'Range-5', u'startEvent': 5, u'PFN': "+mtes_inputfile1+"}]",
    "[{u'lastEvent': 6, u'eventRangeID': u'Range-6', u'startEvent': 6, u'PFN': "+mtes_inputfile2+"}]",
    "[{u'lastEvent': 7, u'eventRangeID': u'Range-7', u'startEvent': 7, u'PFN': "+mtes_inputfile2+"}]"
    ]
print( "================= End Job options file for Geant4 ATLAS detector simulations" ) 
