# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
## Algorithm sequence
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
    spgorders = ['pdgcode: constant '+ options['pid'],
                 'vertX: constant 0.0',
                 'vertY: constant 0.0',
                 'vertZ: constant 0.0',
                 't: constant 0.0',
                 'eta: flat ' + options['etaMin'] + ' ' + options['etaMax'],
                 'phi: flat 0 6.28318',
                 'e: flat ' + options['energyMin'] + ' ' + options['energyMax']]

    athenaCommonFlags.PoolEvgenInput.set_Off()
    from ParticleGenerator.ParticleGeneratorConf import ParticleGenerator
    topSequence += ParticleGenerator()
    topSequence.ParticleGenerator.orders = sorted(spgorders)
"""
    from RngComps.RngCompsConf import AtRanluxGenSvc
    if not hasattr(ServiceMgr, 'AtRanluxGenSvc' ) :
        ServiceMgr += AtRanluxGenSvc()
    seed = "SINGLE "+str(randint(1,294967296))+" "+str(randint(1,294967296))
    ServiceMgr.AtRanluxGenSvc.Seeds += [seed]
"""

# get service manager
from AthenaCommon.AppMgr import ServiceMgr

from LArG4FastSimSvc.LArG4FastSimSvcInit import LArG4FastSimSvcInit
LArG4FastSimSvcInit()

ServiceMgr.LArG4EMBFastSimSvc.GeneratedStartingPointsFile="EMB."+options['output']
ServiceMgr.LArG4EMBFastSimSvc.GeneratedStartingPointsRatio=options['spratio']
ServiceMgr.LArG4EMECFastSimSvc.GeneratedStartingPointsFile="EMEC."+options['output']
ServiceMgr.LArG4EMECFastSimSvc.GeneratedStartingPointsRatio=options['spratio']
ServiceMgr.LArG4FCALFastSimSvc.GeneratedStartingPointsFile="FCAL1."+options['output']
ServiceMgr.LArG4FCALFastSimSvc.GeneratedStartingPointsRatio=options['spratio']
ServiceMgr.LArG4FCAL2FastSimSvc.GeneratedStartingPointsFile="FCAL2."+options['output']
ServiceMgr.LArG4FCAL2FastSimSvc.GeneratedStartingPointsRatio=options['spratio']

include("G4AtlasApps/G4Atlas.flat.configuration.py")

## Populate alg sequence
from AthenaCommon.CfgGetter import getAlgorithm
topSequence += getAlgorithm("G4AtlasAlg",tryDefaultConfigurable=True)
