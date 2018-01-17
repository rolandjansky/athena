# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

###############################################################
## Job options file for Geant4 Tile subdetector simulations
#==============================================================

## Algorithm sequence
from AthenaCommon.AlgSequence import AlgSequence
topSeq = AlgSequence()

## Set output level threshold (1=VERBOSE, 2=DEBUG, 3=INFO, 4=WARNING, 5=ERROR, 6=FATAL)
ServiceMgr.MessageSvc.OutputLevel = 3
ServiceMgr.MessageSvc.defaultLimit = 10000000

## AthenaCommon flags
from AthenaCommon.AthenaCommonFlags import athenaCommonFlags
#athenaCommonFlags.PoolEvgenInput = ['/afs/cern.ch/atlas/offline/ProdData/15.6.11.3/mu_E200_eta0-60-10000.evgen.pool.root']
#athenaCommonFlags.PoolHitsOutput = "Hits.pool.root"
athenaCommonFlags.EvtMax = 10

## Detector flags
from AthenaCommon.DetFlags import DetFlags
DetFlags.ID_setOff()
DetFlags.Calo_setOn()
DetFlags.LAr_setOff()
DetFlags.Tile_setOn()
DetFlags.Muon_setOff()
DetFlags.Lucid_setOff()
DetFlags.Truth_setOff()

## Global conditions tag
from AthenaCommon.GlobalFlags import jobproperties
jobproperties.Global.ConditionsTag = "OFLCOND-MC12-SDR-27"
#jobproperties.Global.ConditionsTag = "OFLCOND-MC12-SDR-06"

## Simulation flags
from G4AtlasApps.SimFlags import simFlags
simFlags.load_atlas_flags()

## Turns on calibration hits for Tile
simFlags.CalibrationRun = 'Tile'

## Layout tags: see simFlags.SimLayout for allowed values
## Use the default layout:
simFlags.SimLayout.set_On()

## Set a specific layout ATLAS-GEO tag:
#simFlags.SimLayout='ATLAS-GEO-20-00-01'
#simFlags.SimLayout='ATLAS-GEO-18-01-03'

## Set a specific Tile-Geo tag:
from GeoModelSvc.GeoModelSvcConf import GeoModelSvc
GeoModelSvc = GeoModelSvc()
GeoModelSvc.TileVersionOverride = "TileCal-GEO-08"

## Set the EtaPhi, VertexSpread and VertexRange checks on
simFlags.EventFilter.set_On()

## No magnetic field
#simFlags.MagneticField.set_Off()
#simFlags.ReleaseGeoModel.set_Off()

## Use single particle generator
from AthenaCommon.AthenaCommonFlags import athenaCommonFlags
athenaCommonFlags.PoolEvgenInput.set_Off()
import AthenaCommon.AtlasUnixGeneratorJob
spgorders = ['pdgcode: constant 211',
             'vertX: constant 0.0',
             'vertY: constant 0.0',
             'vertZ: constant 0.0',
             't: constant 0.0',
             'eta: flat -1.7 1.7',
             'phi: flat  0 6.28318',
             'e: constant 100000']
from ParticleGenerator.ParticleGeneratorConf import ParticleGenerator
topSeq += ParticleGenerator()
topSeq.ParticleGenerator.orders = sorted(spgorders)

#from TruthExamples.TruthExamplesConf import DumpMC
#topSeq += DumpMC()

include("G4AtlasApps/G4Atlas.flat.configuration.py")

## Add the G4 sim to the alg sequence after the generator
from AthenaCommon.CfgGetter import getAlgorithm
topSeq += getAlgorithm("G4AtlasAlg",tryDefaultConfigurable=True)
