# Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration

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

from EvgenJobTransforms.EvgenConfig import evgenConfig

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

## Global flags needed by externals
from AthenaCommon.GlobalFlags import globalflags
globalflags.DataSource = 'geant4'
globalflags.DetGeo = 'atlas'
globalflags.DetDescrVersion = simFlags.SimLayout.get_Value()

if not options ['simulate']:
    ## Translate conditions tag into IOVDbSvc global tag: must be done before job propertie are locked!!!
    from AthenaCommon.AppMgr import ServiceMgr
    from IOVDbSvc.IOVDbSvcConf import IOVDbSvc
    ServiceMgr += IOVDbSvc()
    if not hasattr(ServiceMgr.IOVDbSvc, 'GlobalTag') or not ServiceMgr.IOVDbSvc.GlobalTag:
        ServiceMgr.IOVDbSvc.GlobalTag = globalflags.ConditionsTag.get_Value()


if len(options['physlist']) > 0 :
    simFlags.PhysicsList = options['physlist']

if options['input'] is not None:
    athenaCommonFlags.PoolEvgenInput=[options['input']]
else:
    ## Use single particle generator
    import AthenaCommon.AtlasUnixGeneratorJob
    import ParticleGun as PG
    topSequence += PG.ParticleGun(randomSvcName=simFlags.RandomSvc.get_Value(), randomStream="SINGLE")
    topSequence.ParticleGun.sampler.pid = int(options["pid"])
    if options["configFileName"] != "" :
        energy={}
        inputfile=open(options["ConfigFileName"])
        for line in inputfile:
            for v in line.split():
                data.append(int(v))
        energyBins=set(data)
        topSequence.ParticleGun.sampler.mom = PG.EEtaMPhiSampler(energy=energyBins, eta=[3.8, 4.8])
    else:
        print (options["energyMin"], options["energyMax"])
        topSequence.ParticleGun.sampler.mom = PG.EEtaMPhiSampler(energy=set([100000,200000,500000]), eta=[-5.0,-3.0, 3.0,5.0])
    evgenConfig.generators += ["ParticleGun"]
    athenaCommonFlags.PoolEvgenInput.set_Off()

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

if options['simulate']:
    include("G4AtlasApps/G4Atlas.flat.configuration.py")

    from AthenaCommon.CfgGetter import getAlgorithm
    topSequence += getAlgorithm("G4AtlasAlg",tryDefaultConfigurable=True)
    topSequence.G4AtlasAlg.InputTruthCollection = "GEN_EVENT"
    from MagFieldServices import SetupField

else:
    if athenaCommonFlags.PoolEvgenInput.statusOn:
        ## Tell the event selector about the evgen input files and event skipping
        if not hasattr(svcMgr, 'EventSelector'):
            import AthenaPoolCnvSvc.ReadAthenaPool
        svcMgr.EventSelector.InputCollections = athenaCommonFlags.PoolEvgenInput()
        if athenaCommonFlags.SkipEvents.statusOn:
            svcMgr.EventSelector.SkipEvents = athenaCommonFlags.SkipEvents()

    ## GeoModel stuff
    from AtlasGeoModel import SetGeometryVersion
    from AtlasGeoModel import GeoModelInit
    from AtlasGeoModel import SimEnvelopes
    from MagFieldServices import SetupField

from LArG4Validation.LArG4ValidationConf import SingleTrackValidation
topSequence += SingleTrackValidation()

# The following lines are to construct our ntuple:
RootFileName = options['output'] + '.tuple.root'

NtupleSvc = Service( "NTupleSvc" )
NtupleSvc.Output = [ "FILE DATAFILE=\'" + RootFileName + "\' OPT='NEW'" ]

theApp.Dlls   +=  [  "RootHistCnv"   ]
theApp.HistogramPersistency = "ROOT"
