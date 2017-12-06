include("SimuJobTransforms/CommonSkeletonJobOptions.py")

## Get the logger
from AthenaCommon.Logging import *
atlasG4log = logging.getLogger('AtlasG4')
atlasG4log.info('****************** STARTING ATLASG4 ******************')


## Simulation flags need to be imported first
from G4AtlasApps.SimFlags import SimFlags, simFlags #FIXME drop import of SimFlags rather than simFlags asap
simFlags.load_atlas_flags()


## Set simulation geometry tag
if hasattr(runArgs, 'geometryVersion'):
    simFlags.SimLayout.set_Value_and_Lock(runArgs.geometryVersion)
    atlasG4log.debug('SimLayout set to %s' % simFlags.SimLayout)
else:
    raise RuntimeError("No geometryVersion provided.")


## AthenaCommon flags
from AthenaCommon.AthenaCommonFlags import athenaCommonFlags
# Jobs should stop if an include fails.
if hasattr(runArgs, "IgnoreConfigError"):
    athenaCommonFlags.AllowIgnoreConfigError = runArgs.IgnoreConfigError
else:
    athenaCommonFlags.AllowIgnoreConfigError = False
if hasattr(runArgs, "inputFile"):
    athenaCommonFlags.FilesInput.set_Value_and_Lock( runArgs.inputFile )
from AthenaCommon.BeamFlags import jobproperties
if hasattr(runArgs, "inputEvgenFile"):
    globalflags.InputFormat.set_Value_and_Lock('pool')
    athenaCommonFlags.PoolEvgenInput.set_Value_and_Lock( runArgs.inputEvgenFile )
    # We don't expect both inputFile and inputEvgenFile to be specified
    athenaCommonFlags.FilesInput.set_Value_and_Lock( runArgs.inputEvgenFile )
elif jobproperties.Beam.beamType.get_Value() == 'cosmics':
    atlasG4log.debug('No inputEvgenFile provided. OK, as performing cosmics simulation.')
    athenaCommonFlags.PoolEvgenInput.set_Off()
else:
    atlasG4log.info('No inputEvgenFile provided. Assuming that you are running a generator on the fly.')
    athenaCommonFlags.PoolEvgenInput.set_Off()

import os

os.system("rm -f combinedinputfile.txt")

f = open('overlayfile.txt', 'r')
overlay = f.readline()
overlay = overlay.rstrip()

for evgenfile in runArgs.inputEvgenFile:
  combinedinput = "grep \"inputEvgenFile=" + evgenfile + " " + overlay + "\" matchfile.txt | sed s/" + overlay + "//  >> combinedinputfile.txt"
  os.system(combinedinput)

## Handle cosmics configs
if jobproperties.Beam.beamType.get_Value() == 'cosmics':
    if hasattr(runArgs, "inputEvgenFile"):
        if simFlags.CosmicFilterVolumeName.statusOn and simFlags.CosmicFilterVolumeName.get_Value() != "Muon":
            atlasG4log.warning("Filtering was already done. Using CosmicFilterVolumeName=Muon rather than "
                               "provided value (%s)" % str(runArgs.CosmicFilterVolumeName))
        simFlags.CosmicFilterVolumeName = "Muon"
    else:
        simFlags.CosmicFilterVolumeName = getattr(runArgs, "CosmicFilterVolume", "InnerDetector")
        simFlags.CosmicFilterVolumeName2 = getattr(runArgs, "CosmicFilterVolume2", "NONE")
        simFlags.CosmicPtSlice = getattr(runArgs, "CosmicPtSlice", "NONE")


## Output hits file config
if hasattr(runArgs, "outputHitsFile"):
    athenaCommonFlags.PoolHitsOutput.set_Value_and_Lock( runArgs.outputHitsFile )
else:
    #raise RuntimeError("No outputHitsFile provided.")
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
    DetFlags.all_setOn()
DetFlags.LVL1_setOff() # LVL1 is not part of G4 sim
DetFlags.Truth_setOn()
## Forward dets are off by default
DetFlags.Lucid_setOff()
DetFlags.ALFA_setOff()
DetFlags.ZDC_setOff()
if hasattr(runArgs, "LucidOn"):
    if runArgs.LucidOn:
        DetFlags.Lucid_setOn()
if hasattr(runArgs, "ALFAOn"):
    if runArgs.ALFAOn:
        DetFlags.ALFA_setOn()
if hasattr(runArgs, "ZDCOn"):
    if runArgs.ZDCOn:
        DetFlags.ZDC_setOn()


## Random seed
if hasattr(runArgs, "randomSeed"):
    simFlags.RandomSeedOffset = int(runArgs.randomSeed)
else:
    atlasG4log.warning('randomSeed not set')
if hasattr(runArgs, 'runNumber'):
    simFlags.RunNumber = runArgs.runNumber

if hasattr(runArgs, 'physicsList'):
    simFlags.PhysicsList = runArgs.physicsList

## Handle cosmics track record
from AthenaCommon.BeamFlags import jobproperties
if jobproperties.Beam.beamType.get_Value() == 'cosmics':
    if hasattr(runArgs, "inputEvgenFile"):
        simFlags.ReadTR = athenaCommonFlags.PoolEvgenInput()[0]
    else:
        if hasattr(runArgs, "outputEvgenFile"):
            simFlags.WriteTR = runArgs.outputEvgenFile
        include( 'CosmicGenerator/jobOptions_ConfigCosmicProd.py' )


## Add filters for non-cosmics simulation
if jobproperties.Beam.beamType.get_Value() != 'cosmics':
    if simFlags.CavernCuts:
        simFlags.EventFilter.set_Off()
        # Make a bigger world volume for cavern bg
        simFlags.WorldZRange.set_Value(24000)
    else:
        simFlags.EventFilter.set_On()

## Always enable the looper killer, unless it's been disabled
if not hasattr(runArgs, "enableLooperKiller") or runArgs.enableLooperKiller:
    simFlags.OptionalUserActionList.addAction('G4UA::LooperKillerTool', ['Step'])
else:
    atlasG4log.warning("The looper killer will NOT be run in this job.")

include("G4AtlasApps/G4Atlas.flat.configuration.py")

## Add G4 alg to alg sequence
from AthenaCommon.AlgSequence import AlgSequence
topSeq = AlgSequence()
from AthenaCommon.CfgGetter import getAlgorithm
topSeq += getAlgorithm("G4AtlasAlg",tryDefaultConfigurable=True)


## Add AMITag MetaData to TagInfoMgr
if hasattr(runArgs, 'AMITag'):
    if runArgs.AMITag != "NONE":
        from AthenaCommon.AppMgr import ServiceMgr as svcMgr
        svcMgr.TagInfoMgr.ExtraTagValuePairs += ["AMITag", runArgs.AMITag]


## Random number configuration
from G4AtlasAlg.G4AtlasAlgConf import G4AtlasAlg
g4AtlasAlg = G4AtlasAlg()
g4AtlasAlg.RandomGenerator = "athena"
from AthenaCommon.AppMgr import ServiceMgr
from AthenaServices.AthenaServicesConf import AtRndmGenSvc
## Set up AtRndmGenSvc (don't use the SeedsG4 override)
simFlags.SeedsG4.set_Off()
atRndmGenSvc = AtRndmGenSvc()
ServiceMgr += atRndmGenSvc
if jobproperties.Beam.beamType.get_Value() == 'cosmics':
    if athenaCommonFlags.SkipEvents.statusOn:
        atRndmGenSvc.Seeds += ["COSMICS %d %d" % (athenaCommonFlags.SkipEvents.get_Value()+2040160768, simFlags.RandomSeedOffset.get_Value())]
    else:
        atRndmGenSvc.Seeds += ["COSMICS 2040160768 %d" % (simFlags.RandomSeedOffset.get_Value())]

if athenaCommonFlags.SkipEvents.statusOn:
    atRndmGenSvc.Seeds += ["AtlasG4 %d %d " % (athenaCommonFlags.SkipEvents.get_Value()+1, simFlags.RandomSeedOffset.get_Value())]
else:
    atRndmGenSvc.Seeds += ["AtlasG4 1 %d " % (simFlags.RandomSeedOffset.get_Value())]
atlasG4log.info( "Random seeds set to: '%s'", repr(atRndmGenSvc.Seeds) )


## Set firstEvent for cosmics jobs
if jobproperties.Beam.beamType.get_Value() == 'cosmics':
    if hasattr(runArgs, "firstEvent"):
        #print runArgs.firstEvent
        svcMgr.EventSelector.FirstEvent = runArgs.firstEvent
    else:
        svcMgr.EventSelector.FirstEvent = 0


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

