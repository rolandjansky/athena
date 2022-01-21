# Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration

## Job options file for launching Geant4 ATLAS detector simulations
## on the Tile geometry built from a standalone SQLite file
##
## 2022 Jan, Riccardo Maria BIANCHI, <riccardo.maria.bianchi@cern.ch>
##


from __future__ import print_function

## Algorithm sequence
from AthenaCommon.AlgSequence import AlgSequence
topSeq = AlgSequence()

## Output threshold (DEBUG, INFO, WARNING, ERROR, FATAL)
ServiceMgr.MessageSvc.OutputLevel = DEBUG

## Detector flags
from AthenaCommon.DetFlags import DetFlags
DetFlags.ID_setOff()
DetFlags.Calo_setOff() # LAr
DetFlags.Tile_setOn()  # Tile
DetFlags.Muon_setOff()
DetFlags.Lucid_setOff()
DetFlags.Truth_setOff()
DetFlags.Print()


from GeoModelSvc.GeoModelSvcConf import GeoModelSvc
GeoModelSvc = GeoModelSvc()


## Global conditions tag
from AthenaCommon.GlobalFlags import jobproperties
jobproperties.Global.ConditionsTag = "OFLCOND-RUN12-SDR-21"

## AthenaCommon flags
from AthenaCommon.AthenaCommonFlags import athenaCommonFlags
athenaCommonFlags.PoolEvgenInput = ['/cvmfs/atlas-nightlies.cern.ch/repo/data/data-art/SimCoreTests/minbias_Inelastic-pythia8-7000.evgen.pool.root']
athenaCommonFlags.PoolHitsOutput = "test.HITS.pool.root"
athenaCommonFlags.EvtMax = 1 #5

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

## Use single particle generator
from AthenaCommon.AthenaCommonFlags import athenaCommonFlags
#athenaCommonFlags.PoolEvgenInput.set_Off()
athenaCommonFlags.SkipEvents.set_Off()

# This should only be used when evgen is run in the simulation step
include('G4AtlasApps/fragment.SimCopyWeights.py')


include("G4AtlasApps/G4Atlas.flat.configuration.py")

#from TruthExamples.TruthExamplesConf import DumpMC
#topSeq += DumpMC()


#GeoModelSvc.SQLiteDB = "atlas-from-newDD.db"
GeoModelSvc.SQLiteDB = "tile.db"



from AthenaCommon.CfgGetter import getAlgorithm
topSeq += getAlgorithm("BeamEffectsAlg", tryDefaultConfigurable=True)

## Add the G4 sim to the alg sequence after the generator
from AthenaCommon.CfgGetter import getAlgorithm
topSeq += getAlgorithm("G4AtlasAlg",tryDefaultConfigurable=True)
print (topSeq)


