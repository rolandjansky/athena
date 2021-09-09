# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

## Algorithm sequence
from AthenaCommon.AlgSequence import AlgSequence
topSeq = AlgSequence()

## Output threshold (DEBUG, INFO, WARNING, ERROR, FATAL)
ServiceMgr.MessageSvc.OutputLevel = INFO

## Detector flags
from AthenaCommon.DetFlags import DetFlags
DetFlags.ID_setOn()
DetFlags.Calo_setOn()
DetFlags.Muon_setOn()
DetFlags.Truth_setOn()

## Global conditions tag
from AthenaCommon.GlobalFlags import globalflags
globalflags.ConditionsTag = options['condition']

## Simulation flags
from G4AtlasApps.SimFlags import simFlags
simFlags.load_atlas_flags()
simFlags.CalibrationRun.set_Off()
simFlags.EventFilter.set_off()

## Layout tags: see simFlags.SimLayout for allowed values
## Use the default layout:
#simFlags.SimLayout.set_On()
## Set a specific layout tag:
if len(options['geometry']) > 0 :
    simFlags.SimLayout = options['geometry']
else :
    simFlags.SimLayout.set_On()

if len(options['physlist']) > 0 :
    simFlags.PhysicsList = options['physlist']

## AthenaCommon flags
from AthenaCommon.AthenaCommonFlags import athenaCommonFlags
athenaCommonFlags.EvtMax = options['nevents']
athenaCommonFlags.SkipEvents = options['skipevents']
athenaCommonFlags.PoolHitsOutput.set_Off()

athenaCommonFlags.PoolEvgenInput=options['inputevt']

## Set the LAr parameterization
simFlags.LArParameterization = 0

# get service manager
from AthenaCommon.AppMgr import ServiceMgr

include("G4AtlasApps/G4Atlas.flat.configuration.py")

## Populate alg sequence
from random import randint
simFlags.RandomSeedOffset = randint(1,443921180)

#add G4 function

from G4AtlasApps.SimFlags import simFlags
simFlags.OptionalUserActionList.addAction('G4UA::TestActionShowerLibTool')

from AthenaCommon.CfgGetter import getAlgorithm
topSeq += getAlgorithm("G4AtlasAlg",tryDefaultConfigurable=True)

from AthenaCommon.CfgGetter import getAlgorithm
topSeq += getAlgorithm("LArG4GenShowerLib")
topSeq.LArG4GenShowerLib.LibStructFiles = options['inputstruct']
