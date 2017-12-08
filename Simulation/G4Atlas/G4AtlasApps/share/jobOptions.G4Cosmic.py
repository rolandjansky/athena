## Job options file for Geant4 cosmics simulations

from AthenaCommon.AlgSequence import AlgSequence
topSeq = AlgSequence()

## Output threshold (DEBUG, INFO, WARNING, ERROR, FATAL)
ServiceMgr.MessageSvc.OutputLevel = WARNING

## Detector flags
from AthenaCommon.DetFlags import DetFlags
DetFlags.ID_setOn()
DetFlags.Calo_setOn()
DetFlags.Muon_setOn()
DetFlags.Truth_setOn()

## Global conditions tag (set for global MC15 sim)
from AthenaCommon.GlobalFlags import globalflags
globalflags.ConditionsTag = "OFLCOND-RUN12-SDR-21" # Probably want a different conditions tag for cosmics running

## AthenaCommon flags
from AthenaCommon.AthenaCommonFlags import athenaCommonFlags
athenaCommonFlags.PoolHitsOutput = 'test.HITS.pool.root'
athenaCommonFlags.EvtMax = 3

## Set beam type
from AthenaCommon.BeamFlags import jobproperties
jobproperties.Beam.beamType.set_Value_and_Lock('cosmics')

## Simulation flags
from G4AtlasApps.SimFlags import simFlags
simFlags.load_atlas_flags()
simFlags.load_cosmics_flags()
simFlags.RandomSvc = 'AtDSFMTGenSvc'
## Use the default layout:
simFlags.SimLayout.set_On()
## Set a specific layout tag:
simFlags.SimLayout = 'ATLAS-R2-2015-03-01-00'
## Set a matching RunNumber for run-dependent conditions. See:
## https://twiki.cern.ch/twiki/bin/viewauth/AtlasComputing/ConditionsRun1RunNumbers
simFlags.RunNumber = 222500

## There are three ways to run cosmics sim:
## 1) Generate and simulate directly without intermediates:
from AthenaCommon.AthenaCommonFlags import athenaCommonFlags
athenaCommonFlags.PoolEvgenInput.set_Off()
athenaCommonFlags.SkipEvents.set_Off()
include('CosmicGenerator/SetCosmicGenerator.py')
if hasattr(topSeq, 'CosmicGenerator'):
    topSeq.CosmicGenerator.AtRndmGenSvc = simFlags.RandomSvc.get_Value()

## 2) Use the cosmics generator to make a track truth record for input to (3)
#from AthenaCommon.AthenaCommonFlags import athenaCommonFlags
#athenaCommonFlags.PoolEvgenInput.set_Off()
#simFlags.WriteTR = 'testTR.root'
#include('CosmicGenerator/SetCosmicGenerator.py')

## 3) Read already-generated track records and simulate the detector interactions
#athenaCommonFlags.PoolEvgenInput = ["foo.evgen.pool.root"]
#simFlags.ReadTR = athenaCommonFlags.PoolEvgenInput()[0]

## No magnetic field
#simFlags.MagneticField.set_Off()

from AthenaCommon.CfgGetter import getAlgorithm
topSeq += getAlgorithm("BeamEffectsAlg", tryDefaultConfigurable=True)

## Add alg to alg sequence
from G4AtlasApps.PyG4Atlas import PyG4AtlasAlg
topSeq += PyG4AtlasAlg()
