## Get the logger
from AthenaCommon.Logging import *
atlasG4log = logging.getLogger('TestBeam')
atlasG4log.info('****************** STARTING ATLASG4 Test Beam ******************')

## Include common skeleton
include("SimuJobTransforms/skeleton.EVGENtoHIT.py")

svcMgr.MessageSvc.defaultLimit = 1000000
svcMgr.MessageSvc.Format = "% F%60W%S%7W%R%T %0W%M"
svcMgr.MessageSvc.useColors = False

if hasattr(runArgs, 'useISF') and runArgs.useISF:
    raise RuntimeError("Unsupported configuration! If you want to run with useISF=True, please use Sim_tf.py!")

## Simulation flags need to be imported first
from G4AtlasApps.SimFlags import simFlags

## Set simulation geometry tag
if hasattr(runArgs, 'geometryVersion'):
    simFlags.SimLayout.set_Value_and_Lock(runArgs.geometryVersion)
    atlasG4log.debug('SimLayout set to %s' % simFlags.SimLayout)
else:
    atlasG4log.info('no geometryVersion specified, so assuming tb_Tile2000_2003_5B')
    simFlags.SimLayout.set_Value_and_Lock('tb_Tile2000_2003_5B')

## Load the test beam flags
if hasattr(runArgs, 'testBeamConfig'):
    attrname = "load_"+runArgs.testBeamConfig+"_flags"
    checkFlags = getattr(simFlags, attrname, None)
    if checkFlags is not None:
        checkFlags()
    else:
        raise RuntimeError("testBeamConfig %s is unknown." % runArgs.testBeamConfig)
else:
    atlasG4log.info('no testBeamConfig specified, so assuming tbtile')
    simFlags.load_tbtile_flags()

## AthenaCommon flags
#from AthenaCommon.AthenaCommonFlags import athenaCommonFlags
## Jobs should stop if an include fails.
#if hasattr(runArgs, "IgnoreConfigError"):
#    athenaCommonFlags.AllowIgnoreConfigError = runArgs.IgnoreConfigError
#else:
#    athenaCommonFlags.AllowIgnoreConfigError = False

## Input Files
if hasattr(runArgs, "inputFile"):
    athenaCommonFlags.FilesInput.set_Value_and_Lock( runArgs.inputFile )
from AthenaCommon.BeamFlags import jobproperties
if hasattr(runArgs, "inputEvgenFile"):
    globalflags.InputFormat.set_Value_and_Lock('pool')
    athenaCommonFlags.PoolEvgenInput.set_Value_and_Lock( runArgs.inputEvgenFile )
    # We don't expect both inputFile and inputEvgenFile to be specified
    athenaCommonFlags.FilesInput.set_Value_and_Lock( runArgs.inputEvgenFile )
else:
    atlasG4log.info('No inputEvgenFile provided. Assuming that you are running a generator on the fly.')
    athenaCommonFlags.PoolEvgenInput.set_Off()

## Output hits file config
if hasattr(runArgs, "outputHITSFile"):
    athenaCommonFlags.PoolHitsOutput.set_Value_and_Lock( runArgs.outputHITSFile )
elif hasattr(runArgs, "outputHitsFile"):
    athenaCommonFlags.PoolHitsOutput.set_Value_and_Lock( runArgs.outputHitsFile )
else:
    atlasG4log.info('No outputHitsFile provided. This simulation job will not write out any HITS file.')
    athenaCommonFlags.PoolHitsOutput = ""
    athenaCommonFlags.PoolHitsOutput.statusOn = False


## Write out runArgs configuration
atlasG4log.info( '**** Transformation run arguments' )
atlasG4log.info( str(runArgs) )


#==============================================================
# Job Configuration parameters:
#==============================================================
## Pre-exec
if hasattr(runArgs, "preExec"):
    atlasG4log.info("transform pre-exec")
    for cmd in runArgs.preExec:
        atlasG4log.info(cmd)
        exec(cmd)

## Pre-include
if hasattr(runArgs, "preInclude"):
    for fragment in runArgs.preInclude:
        include(fragment)

include( "AthenaCommon/MemTraceInclude.py" )

## Select detectors
if 'DetFlags' not in dir():
    ## If you configure one det flag, you're responsible for configuring them all!
    from AthenaCommon.DetFlags import DetFlags
    DetFlags.all_setOff()
DetFlags.simulate.Truth_setOn()
#DetFlags.Truth_setOn()
DetFlags.Tile_setOn()
if hasattr(runArgs, 'testBeamConfig') and 'tbtile' != runArgs.testBeamConfig:
    DetFlags.Calo_setOn()
    DetFlags.LAr_setOn()
    DetFlags.em_setOn()

## Geometry flags
from AthenaCommon.GlobalFlags import jobproperties
jobproperties.Global.DetDescrVersion = "ATLAS-CTB-01"
from AtlasGeoModel import SetGeometryVersion
jobproperties.Global.DetGeo = "ctbh8"
from AtlasGeoModel import GeoModelInit

## Set the PhysicsList
if hasattr(runArgs, 'physicsList'):
    simFlags.PhysicsList = runArgs.physicsList

## Random seed
if hasattr(runArgs, "randomSeed"):
    simFlags.RandomSeedOffset = int(runArgs.randomSeed)
else:
    atlasG4log.warning('randomSeed not set')

if hasattr(runArgs,"Eta") and ( hasattr(runArgs,"Theta") or hasattr(runArgs,"Z") ):
    raise RuntimeError("Eta cannot be specified at the same time as Theta and Z.")

if hasattr(runArgs,"Eta"):
    atlasG4log.info('Overriding simFlags.Eta from command-line. simFlags.Eta = %s', runArgs.Eta)
    simFlags.Eta = runArgs.Eta

if hasattr(runArgs,"Theta") or hasattr(runArgs,"Z"):
    if hasattr(runArgs,"Theta"):
        atlasG4log.info('Overriding simFlags.Theta from command-line. simFlags.Theta = %s', runArgs.Theta)
        simFlags.Theta = runArgs.Theta
    else:
        atlasG4log.info('Z specified on the commmand-line without Theta, so will use simFlags.Theta = 90')
        simFlags.Theta=90
    if hasattr(runArgs,"Z"):
        atlasG4log.info('Overriding simFlags.Z from command-line. simFlags.Z = %s', runArgs.Z)
        simFlags.Z = runArgs.Z
    else:
        atlasG4log.info('Theta specified on the commmand-line without Z, so will use simFlags.Z = 2550.0')
        simFlags.Z = 2550.0

if hasattr(runArgs,"Phi"):
    simFlags.Phi=runArgs.Phi
if hasattr(runArgs,"Y"):
    simFlags.Y=runArgs.Y

## Set the Run Number (if required)
if hasattr(runArgs,"DataRunNumber"):
    if runArgs.DataRunNumber>0:
        atlasG4log.info( 'Overriding run number to be: %s ', runArgs.DataRunNumber )
        simFlags.RunNumber=runArgs.DataRunNumber
elif hasattr(runArgs,'jobNumber'):
    if runArgs.jobNumber>=0:
        atlasG4log.info( 'Using job number '+str(runArgs.jobNumber)+' to derive run number.' )
        simFlags.RunNumber = simFlags.RunDict.GetRunNumber( runArgs.jobNumber )
        atlasG4log.info( 'Set run number based on dictionary to '+str(simFlags.RunNumber) )

# uncomment and modify any of options below to have non-standard simulation
from TileSimUtils.TileSimInfoConfigurator import TileSimInfoConfigurator
tileSimInfoConfigurator=TileSimInfoConfigurator()
# tileSimInfoConfigurator.DeltaTHit = [ 1. ]
# tileSimInfoConfigurator.TimeCut = 200.5
# tileSimInfoConfigurator.TileTB = True
# tileSimInfoConfigurator.PlateToCell = True
# tileSimInfoConfigurator.DoExpAtt = False
# tileSimInfoConfigurator.DoTileRow = False
# tileSimInfoConfigurator.DoTOFCorrection = True
# Birks' law
if 'DoBirk' in dir():
    tileSimInfoConfigurator.DoBirk = DoBirk
# U-shape
if 'TileUshape' in dir():
    tileSimInfoConfigurator.Ushape = TileUshape

print tileSimInfoConfigurator

if hasattr(runArgs, 'testBeamConfig') and 'tbtile' == runArgs.testBeamConfig:
    # avoid reading CaloTTMap from COOL
    include.block ( "CaloConditions/CaloConditions_jobOptions.py" )

#---- Needed for the U-shape (any value of Ushape > 0- means that tile size equal to size of master plate, for Ushape <=0 - size of the tile is like in old geometry )
if 'TileUshape' in dir():
    from GeoModelSvc.GeoModelSvcConf import GeoModelSvc
    GeoModelSvc = GeoModelSvc()
    from AtlasGeoModel import TileGM
    GeoModelSvc.DetectorTools[ "TileDetectorTool" ].Ushape = TileUshape

from AthenaCommon.AlgSequence import AlgSequence
topSeq = AlgSequence()

## Set Overall per-Algorithm time-limit on the AlgSequence
topSeq.TimeOut = 43200 * Units.s

if not athenaCommonFlags.PoolEvgenInput.statusOn:
    import AthenaCommon.AtlasUnixGeneratorJob
    import ParticleGun as PG
    pg = PG.ParticleGun(randomSvcName=simFlags.RandomSvc.get_Value(), randomStream="SINGLE")

    # 50 GeV pions
    #pg.sampler.pid = 211
    #pg.sampler.pos = PG.PosSampler(x=-27500, y=[-10,15], z=[-15,15], t=-27500)
    #pg.sampler.mom = PG.EEtaMPhiSampler(energy=50000, eta=0, phi=0)

    # 100 GeV electrons - use for sampling faction calculation
    #pg.sampler.pid = 11
    #pg.sampler.pos = PG.PosSampler(x=-27500, y=[-20,20], z=[-15,15], t=-27500)
    #pg.sampler.mom = PG.EEtaMPhiSampler(energy=100000, eta=0, phi=0)

    if not 'PID' in dir():
        PID=11
    if not 'E' in dir():
        E=100000
    if not 'Ybeam' in dir():
        Ybeam=[-20,20]
    if not 'Zbeam' in dir():
        Zbeam=[-15,15]
    pg.sampler.pid = PID
    pg.sampler.pos = PG.PosSampler(x=-27500, y=Ybeam, z=Zbeam, t=-27500)
    pg.sampler.mom = PG.EEtaMPhiSampler(energy=E, eta=0, phi=0)

    topSeq += pg

    include("G4AtlasApps/fragment.SimCopyWeights.py")

from AthenaCommon.CfgGetter import getAlgorithm
topSeq += getAlgorithm("BeamEffectsAlg")

try:
    from RecAlgs.RecAlgsConf import TimingAlg
    topSeq+=TimingAlg("SimTimerBegin", TimingObjOutputName = "EVNTtoHITS_timings")
except:
    atlasG4log.warning('Could not add TimingAlg, no timing info will be written out.')

include('G4AtlasApps/Tile2000_2003.flat.configuration.py')#HACK - has to be here for TBDetDescrLoader

## Add G4 alg to alg sequence
from AthenaCommon.CfgGetter import getAlgorithm
topSeq += getAlgorithm("G4AtlasAlg",tryDefaultConfigurable=True)

print topSeq

## Add AMITag MetaData to TagInfoMgr
if hasattr(runArgs, 'AMITag'):
    if runArgs.AMITag != "NONE":
        from AthenaCommon.AppMgr import ServiceMgr as svcMgr
        svcMgr.TagInfoMgr.ExtraTagValuePairs += ["AMITag", runArgs.AMITag]


## Increase max RDO output file size to 10 GB
## NB. We use 10GB since Athena complains that 15GB files are not supported
from AthenaCommon.AppMgr import ServiceMgr as svcMgr
svcMgr.AthenaPoolCnvSvc.MaxFileSizes = [ "10000000000" ]


## Post-include
if hasattr(runArgs, "postInclude"):
    for fragment in runArgs.postInclude:
        include(fragment)

## Post-exec
if hasattr(runArgs, "postExec"):
    atlasG4log.info("transform post-exec")
    for cmd in runArgs.postExec:
        atlasG4log.info(cmd)
        exec(cmd)


## enable the looper killer, if requested
if hasattr(runArgs, "enableLooperKiller") and runArgs.enableLooperKiller:
    # add non-MT looperKiller
    from G4AtlasServices.G4AtlasUserActionConfig import UAStore
    # add default configurable

    UAStore.addAction('LooperKiller',['Step'])
    # add MT looperkiller
    from G4UserActions import G4UserActionsConfig
    try:
        G4UserActionsConfig.addLooperKillerTool()
    except AttributeError:
        atlasG4log.warning("Could not add the MT-version of the LooperKiller")
else:
    atlasG4log.warning("The looper killer will NOT be run in this job.")
