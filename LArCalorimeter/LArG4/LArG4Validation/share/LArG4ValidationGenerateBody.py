# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

from AthenaCommon.DetFlags import DetFlags
# - Select detectors 
DetFlags.ID_setOn()
DetFlags.Calo_setOn()
DetFlags.Muon_setOn()
DetFlags.Truth_setOn()


# get a handle to the default top-level algorithm sequence
from AthenaCommon.AlgSequence import AlgSequence
from AthenaCommon.AppMgr import ServiceMgr
topSequence = AlgSequence()

## Output threshold (DEBUG, INFO, WARNING, ERROR, FATAL)
ServiceMgr.MessageSvc.OutputLevel = INFO

from AthenaCommon.GlobalFlags import globalflags
if len(options['condition']) > 0 :
    globalflags.ConditionsTag = options['condition']
else :
    printfunc ("No condition tag specified.\nThere is no use in validating FS with some arbitrary condition tag.\n Please, specify one.")
    import sys
    sys.exit(1)


from AthenaCommon.AthenaCommonFlags import athenaCommonFlags

athenaCommonFlags.PoolHitsOutput=options['output'] + '.HITS.pool.root'

athenaCommonFlags.EvtMax=options['nevents']


#--- Simulation flags -----------------------------------------
from G4AtlasApps.SimFlags import simFlags
simFlags.load_atlas_flags()
simFlags.CalibrationRun.set_Off()

# Random seeds get random values
from random import randint
simFlags.RandomSeedOffset = randint(1,1000000000)

if len(options['geometry']) > 0 :
    simFlags.SimLayout = options['geometry']
else :
    simFlags.SimLayout.set_On()

if len(options['physlist']) > 0 :
    simFlags.PhysicsList = options['physlist']

if options['input'] is not None:
    athenaCommonFlags.PoolEvgenInput=[options['input']]
else:
    ## Use single particle generator
    import AthenaCommon.AtlasUnixGeneratorJob
    spgorders = ['pdgcode: constant '+options['pid'],
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

if (options['parameterize'] > 0): 
    simFlags.LArParameterization=options['parameterize']

    if len(options['fsLibs']) > 0 :
        printfunc ("Setting up ShowerLib Service")
        from LArG4ShowerLibSvc.LArG4ShowerLibSvcConf import LArG4ShowerLibSvc
        if not hasattr( ServiceMgr, 'LArG4ShowerLibSvc' ):
             ServiceMgr += LArG4ShowerLibSvc()
        ServiceMgr.LArG4ShowerLibSvc.FileNameList = options['fsLibs']
    

## Set Event #
simFlags.RunNumber = options['runNumber']

include("G4AtlasApps/G4Atlas.flat.configuration.py")

from AthenaCommon.CfgGetter import getAlgorithm
topSequence += getAlgorithm("G4AtlasAlg",tryDefaultConfigurable=True)

from LArG4Validation.LArG4ValidationConf import SingleTrackValidation
topSequence += SingleTrackValidation()

# The following lines are to construct our ntuple:
RootFileName = options['output'] + '.tuple.root'

NtupleSvc = Service( "NTupleSvc" )
NtupleSvc.Output = [ "FILE DATAFILE=\'" + RootFileName + "\' OPT='NEW'" ]

theApp.Dlls   +=  [  "RootHistCnv"   ]
theApp.HistogramPersistency = "ROOT"
