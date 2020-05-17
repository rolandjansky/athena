# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

"""
Job options file for Geant4 ATLAS detector simulations to measure the FCal
sampling fractions.

Set the 'module' variable to one of 'fcal1', 'fcal2' or 'fcal3' to simulate
particle interactions in the FCal1, FCal2 or FCal3 modules, respectively.

Additionaly parameters, such as the number of events to simulate and the
particle gun energy and coordinates, may be set in the 'params' variable.
"""

import random

## Common parameters
module = "fcal1"   # Choose from "fcal1", "fcal2" or "fcal3"

## Distance from IP to face of each FCal module [mm]

## Values based on Table 3 in JINST 3 P02010 (2008), "The ATLAS Forward Calorimeter"
## These numbers do not necessarily reflect the geometry and location of the FCal in the Geant4 model!
# fcal1_z = 4708.90  # Distance from IP to FCal1 face [mm]; = 4683.5 + 26.4 - 1.0
# fcal2_z = 5166.10  # Distance from IP to FCal2 face [mm]; = 4683.5 + 483.6 - 1.0
# fcal3_z = 5648.20  # Distance from IP to FCal3 face [mm]; = 4683.5 + 965.7 - 1.0

## Values for the Geant4 module
fcal1_z = 4713.5
fcal2_z = 5173.3
fcal3_z = 5647.8

params = {
    'n_event': 200,          # Number of events to simulate
    'pg_E': 40000,           # Particle gun energy [MeV]
    'pg_x': [212.5, 277.5],  # Particle gun x-coordinate; constant or range
    'pg_y': [7.5, 72.5],     # Particle gun y-coordinate; constant or range
    'pg_z': None,            # Particle gun z-coordinate (distance to IP); should be constant
    'pg_eta': None,          # Particle gun eta; constant or range
}

if module.lower() == "fcal1":
    params['pg_z'] = fcal1_z
    params['pg_eta'] = [3.5, 3.8]
elif module.lower() == "fcal2":
    params['pg_z'] = fcal2_z
    params['pg_eta'] = [3.5, 3.8]
elif module.lower() == "fcal3":
    params['pg_z'] = fcal3_z
    params['pg_eta'] = [3.5, 3.8]


## Algorithm sequence
from AthenaCommon.AlgSequence import AlgSequence
topSeq = AlgSequence()

## Output threshold (DEBUG, INFO, WARNING, ERROR, FATAL)
ServiceMgr.MessageSvc.OutputLevel = INFO

## Detector flags
from AthenaCommon.DetFlags import DetFlags
DetFlags.ID_setOff()
DetFlags.Calo_setOn()
DetFlags.Muon_setOff()
DetFlags.HEC_setOff()
DetFlags.em_setOff()
DetFlags.Tile_setOff()
# DetFlags.Lucid_setOn()
DetFlags.Truth_setOn()
DetFlags.simulate.Truth_setOn()

## Global conditions tag
from AthenaCommon.GlobalFlags import jobproperties
jobproperties.Global.ConditionsTag = "OFLCOND-MC16-SDR-16"

## AthenaCommon flags
from AthenaCommon.AthenaCommonFlags import athenaCommonFlags
athenaCommonFlags.PoolHitsOutput = "atlasG4.hits.pool.root"
athenaCommonFlags.EvtMax = params['n_event']

## Simulation flags
from G4AtlasApps.SimFlags import simFlags
simFlags.load_atlas_flags()
simFlags.RandomSvc = "AtDSFMTGenSvc"

## Layout tags: see simFlags.SimLayout for allowed values
## Use the default layout:
simFlags.SimLayout.set_On()
## Set a specific layout tag:
simFlags.SimLayout = "ATLAS-R2-2016-01-00-01"
## Set a specific non-officially-supported layout tag using the _VALIDATION suffix, e.g.:
# simFlags.SimLayout = "ATLAS-R2-2016-01-00-01_VALIDATION"

## Set the EtaPhi, VertexSpread and VertexRange checks on
simFlags.EventFilter.set_Off()

## Set CalibrationRun
simFlags.CalibrationRun = "LAr"

## Set random seeds
## Seeds need to explicitly set, otherwise default constants are used
simFlags.RandomSeedList.addSeed('AtlasG4',random.randint(10000, 99999999), random.randint(10000, 99999999))
simFlags.RandomSeedList.addSeed('VERTEX', random.randint(10000, 99999999), random.randint(10000, 99999999))
simFlags.RandomSeedList.addSeed('SINGLE', random.randint(10000, 99999999), random.randint(10000, 99999999))

## No magnetic field
# simFlags.MagneticField.set_Off()

## Register callback functions at various init stages
# def test_preInit():
#     print "CALLBACK AT PREINIT"
# def test_postInit():
#     print "CALLBACK AT POSTINIT"
# simFlags.InitFunctions.add_function("preInit", test_preInit)
# simFlags.InitFunctions.add_function("postInit", test_postInit)

## Change the field stepper or use verbose G4 tracking
# from G4AtlasApps import callbacks
# simFlags.InitFunctions.add_function("postInit", callbacks.use_simplerunge_stepper)
# simFlags.InitFunctions.add_function("postInit", callbacks.use_verbose_tracking)

## Use single particle generator
from AthenaCommon.AthenaCommonFlags import athenaCommonFlags
athenaCommonFlags.PoolEvgenInput.set_Off()
athenaCommonFlags.SkipEvents.set_Off()

## Set particle gun parameters
import AthenaCommon.AtlasUnixGeneratorJob
import ParticleGun as PG
pg = PG.ParticleGun(randomSvcName=simFlags.RandomSvc.get_Value(), randomStream="SINGLE")
pg.sampler.pid = 11
pg.sampler.mom = PG.EEtaMPhiSampler(energy=params['pg_E'], eta=params['pg_eta'])
pg.sampler.pos = PG.PosSampler(x=params['pg_x'], y=params['pg_y'], z=params['pg_z'], t=params['pg_z'])
topSeq += pg

## Dump truth information
# from TruthExamples.TruthExamplesConf import DumpMC
# topSeq += DumpMC()

StoreGateSvc = Service( "StoreGateSvc" )
StoreGateSvc.Dump = True

include("G4AtlasApps/G4Atlas.flat.configuration.py")

## Use GeoCheckAction
## If using GeoCheckAction, make sure the particle type id is set to 999 (geantinos)
# def add_GeoCheckAction():
#     from G4AtlasApps import PyG4Atlas, AtlasG4Eng
#     GeoCheckAction = PyG4Atlas.UserAction( 'G4UserActions', 'GeoCheckAction', ['BeginOfEvent','EndOfEvent','Step','EndOfRun'] )
#     AtlasG4Eng.G4Eng.menu_UserActions.add_UserAction(GeoCheckAction)

# simFlags.InitFunctions.add_function("postInit", add_GeoCheckAction)


from AthenaCommon.CfgGetter import getAlgorithm
topSeq += getAlgorithm("G4AtlasAlg", tryDefaultConfigurable=True)
topSeq.G4AtlasAlg.InputTruthCollection = "GEN_EVENT"

## Tool to merge dead material containers into a single container
from LArG4SD import LArG4SDConfig
topSeq += LArG4SDConfig.getDeadMaterialCalibrationHitMerger()

myAlg = CfgMgr.LArFCalSamplingFraction()
myAlg.Calibration = True
topSeq += myAlg


from GaudiSvc.GaudiSvcConf import THistSvc
ServiceMgr += THistSvc()
ServiceMgr.THistSvc.Output = [
    "AANT DATAFILE='LArFCalSamplingFraction.{}.{:g}GeV.aan.root' OPT='RECREATE'".format(module, params['pg_E']/1000)
]

print topSeq
