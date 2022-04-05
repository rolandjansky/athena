# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
## Algorithm sequence
from AthenaCommon.AppMgr import ToolSvc
from AthenaCommon.AlgSequence import AlgSequence
topSequence = AlgSequence()

## Output threshold (DEBUG, INFO, WARNING, ERROR, FATAL)
ServiceMgr.MessageSvc.OutputLevel = INFO

## Detector flags
from AthenaCommon.DetFlags import DetFlags
DetFlags.ID_setOn()
DetFlags.Calo_setOn()
DetFlags.Muon_setOn()
#DetFlags.Lucid_setOn()
DetFlags.Truth_setOn()

## Global conditions tag
from AthenaCommon.GlobalFlags import globalflags
print(options)
globalflags.ConditionsTag = options['condition']

## Simulation flags
from G4AtlasApps.SimFlags import SimFlags
SimFlags.load_atlas_flags()
SimFlags.CalibrationRun.set_Off()

if len(options['geometry']) > 0 :
    SimFlags.SimLayout = options['geometry']
else :
    SimFlags.SimLayout.set_On()

if len(options['physlist']) > 0 :
    SimFlags.PhysicsList = options['physlist']

## AthenaCommon flags
from AthenaCommon.AthenaCommonFlags import athenaCommonFlags
athenaCommonFlags.EvtMax = options['nevents']
athenaCommonFlags.SkipEvents = options['skipevents']
athenaCommonFlags.PoolHitsOutput.set_Off()

## Set the LAr parameterization
SimFlags.LArParameterization = 1

from random import randint
SimFlags.RandomSeedOffset = randint(1,443921180)

if options['input'] is not None:
    athenaCommonFlags.PoolEvgenInput=options['input']
else:
    ## Use single particle generator
    import AthenaCommon.AtlasUnixGeneratorJob
    import ParticleGun as PG
    topSequence += PG.ParticleGun(randomSvcName=simFlags.RandomSvc.get_Value(), randomStream="SINGLE")
    topSequence.ParticleGun.sampler.pid = int(options["pid"])

    topSequence.ParticleGun.sampler.mom = PG.EEtaMPhiSampler(energy=[options['energyMin'],options['energyMax']], eta=[options['etaMin'],options['etaMax']])
    evgenConfig.generators += ["ParticleGun"]

    athenaCommonFlags.PoolEvgenInput.set_Off()


include("G4AtlasApps/G4Atlas.flat.configuration.py")

## Populate alg sequence
from AthenaCommon.CfgGetter import getAlgorithm
topSequence += getAlgorithm("G4AtlasAlg",tryDefaultConfigurable=True)
topSequence.G4AtlasAlg.InputTruthCollection = "GEN_EVENT"

ToolSvc.FastSimulationMasterTool.FastSimulations["EMBFastShower"].GeneratedStartingPointsFile  = "EMB."+options['output']
ToolSvc.FastSimulationMasterTool.FastSimulations["EMBFastShower"].GeneratedStartingPointsRatio = options['spratio']
ToolSvc.FastSimulationMasterTool.FastSimulations["EMECFastShower"].GeneratedStartingPointsFile ="EMEC."+options['output']
ToolSvc.FastSimulationMasterTool.FastSimulations["EMECFastShower"].GeneratedStartingPointsRatio=options['spratio']
ToolSvc.FastSimulationMasterTool.FastSimulations["FCALFastShower"].GeneratedStartingPointsFile ="FCAL1."+options['output']
ToolSvc.FastSimulationMasterTool.FastSimulations["FCALFastShower"].GeneratedStartingPointsRatio=options['spratio']
ToolSvc.FastSimulationMasterTool.FastSimulations["FCAL2FastShower"].GeneratedStartingPointsFile="FCAL2."+options['output']
ToolSvc.FastSimulationMasterTool.FastSimulations["FCAL2FastShower"].GeneratedStartingPointsRatio=options['spratio']

