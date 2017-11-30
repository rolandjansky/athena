## Get the logger
from AthenaCommon.Logging import *
atlasG4log = logging.getLogger('AtlasG4')
atlasG4log.info('****************** STARTING ATLASG4 ******************')

## Include common skeleton
include("SimuJobTransforms/skeleton.EVGENtoHIT.py")

if hasattr(runArgs, 'useISF') and runArgs.useISF:
    raise RuntimeError("Unsupported configuration! If you want to run with useISF=True, please use Sim_tf.py!")

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

from AthenaCommon.BeamFlags import jobproperties

## Input Files
def setInputEvgenFileJobProperties(InputEvgenFile):
    from AthenaCommon.GlobalFlags import globalflags
    globalflags.InputFormat.set_Value_and_Lock('pool')
    from AthenaCommon.AthenaCommonFlags import athenaCommonFlags
    athenaCommonFlags.PoolEvgenInput.set_Value_and_Lock( InputEvgenFile )
    athenaCommonFlags.FilesInput.set_Value_and_Lock( InputEvgenFile )

if hasattr(runArgs, "inputFile"):
    athenaCommonFlags.FilesInput.set_Value_and_Lock( runArgs.inputFile )
# We don't expect both inputFile and inputEVNT*File to be specified
if hasattr(runArgs, "inputEVNTFile"):
    setInputEvgenFileJobProperties( runArgs.inputEVNTFile )
elif hasattr(runArgs, "inputEVNT_COSMICSFile"):
    setInputEvgenFileJobProperties( runArgs.inputEVNT_COSMICSFile )
elif hasattr(runArgs, "inputEVNT_CAVERNFile"):
    setInputEvgenFileJobProperties( runArgs.inputEVNT_CAVERNFile )
elif hasattr(runArgs, "inputEVNT_STOPPEDFile"):
    setInputEvgenFileJobProperties( runArgs.inputEVNT_STOPPEDFile )
elif jobproperties.Beam.beamType.get_Value() == 'cosmics':
    atlasG4log.debug('No inputEVNTFile provided. OK, as performing cosmics simulation.')
    athenaCommonFlags.PoolEvgenInput.set_Off()
else:
    atlasG4log.info('No inputEVNTFile provided. Assuming that you are running a generator on the fly.')
    athenaCommonFlags.PoolEvgenInput.set_Off()

## Handle cosmics configs
if jobproperties.Beam.beamType.get_Value() == 'cosmics':
    simFlags.load_cosmics_flags()
    if hasattr(runArgs, "inputEVNT_COSMICSFile"):
        if simFlags.CosmicFilterVolumeName.statusOn and simFlags.CosmicFilterVolumeName.get_Value() != "Muon":
            atlasG4log.warning("Filtering was already done. Using CosmicFilterVolumeName=Muon rather than "
                               "provided value (%s)" % str(runArgs.CosmicFilterVolumeName))
        simFlags.CosmicFilterVolumeName = "Muon"
    else:
        simFlags.CosmicFilterVolumeName = getattr(runArgs, "CosmicFilterVolume", "InnerDetector")
        simFlags.CosmicFilterVolumeName2 = getattr(runArgs, "CosmicFilterVolume2", "NONE")
        simFlags.CosmicPtSlice = getattr(runArgs, "CosmicPtSlice", "NONE")


## Output hits file config
if hasattr(runArgs, "outputHITSFile"):
    athenaCommonFlags.PoolHitsOutput.set_Value_and_Lock( runArgs.outputHITSFile )
else:
    if hasattr(runArgs, "outputEVNT_STOPPEDFile"):
        simFlags.StoppedParticleFile.set_Value_and_Lock( runArgs.outputEVNT_STOPPEDFile )
    #raise RuntimeError("No outputHITSFile provided.")
    atlasG4log.info('No outputHITSFile provided. This simulation job will not write out any HITS file.')
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

# Avoid command line preInclude for stopped particles
if hasattr(runArgs, "inputEVNT_STOPPEDFile"):
    include('SimulationJobOptions/preInclude.ReadStoppedParticles.py')

# Avoid command line preInclude for cavern background
if hasattr(runArgs, "inputEVNT_CAVERNFile"):
    include('SimulationJobOptions/preInclude.G4ReadCavern.py')
if hasattr(runArgs, "outputEVNT_CAVERNTRFile"):
    include('SimulationJobOptions/preInclude.G4WriteCavern.py')

# Avoid command line preInclude for event service
if hasattr(runArgs, "eventService") and runArgs.eventService:
    include('AthenaMP/AthenaMP_EventService.py')

if jobproperties.Beam.beamType.get_Value() == 'cosmics':
    include('SimulationJobOptions/preInclude.Cosmics.py')

## Select detectors
if 'DetFlags' not in dir():
    ## If you configure one det flag, you're responsible for configuring them all!
    from AthenaCommon.DetFlags import DetFlags
    DetFlags.all_setOn()
DetFlags.LVL1_setOff() # LVL1 is not part of G4 sim
DetFlags.Truth_setOn()
DetFlags.Forward_setOff() # Forward dets are off by default
checkHGTDOff = getattr(DetFlags, 'HGTD_setOff', None)
if checkHGTDOff is not None:
    checkHGTDOff() #Default for now

if hasattr(runArgs, "AFPOn"):
    if runArgs.AFPOn:
        DetFlags.AFP_setOn()
if hasattr(runArgs, "ALFAOn"):
    if runArgs.ALFAOn:
        DetFlags.ALFA_setOn()
if hasattr(runArgs, "FwdRegionOn"):
    if runArgs.FwdRegionOn:
        DetFlags.FwdRegion_setOn()
if hasattr(runArgs, "LucidOn"):
    if runArgs.LucidOn:
        DetFlags.Lucid_setOn()
if hasattr(runArgs, "ZDCOn"):
    if runArgs.ZDCOn:
        DetFlags.ZDC_setOn()
if hasattr(runArgs, "HGTDOn"):
    if runArgs.HGTDOn:
        checkHGTDOn = getattr(DetFlags, 'HGTD_setOn', None)
        if checkHGTDOn is not None:
            checkHGTDOn()
        else:
            atlasG4log.warning('The HGTD DetFlag is not supported in this release')

DetFlags.Print()

if DetFlags.Forward_on():
    if DetFlags.FwdRegion_on() or DetFlags.ZDC_on() or DetFlags.ALFA_on() or DetFlags.AFP_on():
        ## Do not filter high eta particles
        if simFlags.EventFilter.statusOn:
            simFlags.EventFilter.get_Value()['EtaPhiFilters'] = False
        ## ForwardTransport is applied to particles hitting BeamPipe::SectionF46
        DetFlags.bpipe_setOn()

    if DetFlags.FwdRegion_on():
        # Do full simulation rather than beam transport
        simFlags.ForwardDetectors = 1
        atlasG4log.info( 'FwdRegion switched on, so will run Full Simulation of the Forward Region rather than Forward Transport.' )
    elif DetFlags.ZDC_on() or DetFlags.ALFA_on() or DetFlags.AFP_on():
        ## Use the ForwardTransport package to do the beam transport
        atlasG4log.info( 'FwdRegion switched off, so will run Full Simulation of the Forward Region rather than Forward Transport.' )
        simFlags.ForwardDetectors = 2

## Set the PhysicsList
if hasattr(runArgs, 'physicsList'):
    simFlags.PhysicsList = runArgs.physicsList


## Random seed
if hasattr(runArgs, "randomSeed"):
    simFlags.RandomSeedOffset = int(runArgs.randomSeed)
else:
    atlasG4log.warning('randomSeed not set')
## Don't use the SeedsG4 override
simFlags.SeedsG4.set_Off()


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

## Handle cosmics track record
from AthenaCommon.BeamFlags import jobproperties
if jobproperties.Beam.beamType.get_Value() == 'cosmics':
    if hasattr(runArgs, "inputEVNT_COSMICSFile"):
        simFlags.ReadTR = athenaCommonFlags.PoolEvgenInput()[0]
    else:
        if hasattr(runArgs, "outputEVNT_COSMICSTRFile"):
            simFlags.WriteTR = runArgs.outputEVNT_COSMICSTRFile
        include( 'CosmicGenerator/jobOptions_ConfigCosmicProd.py' )


## Add filters for non-cosmics simulation
## FIXME: This block should be moved out of the skeleton into job options.
if jobproperties.Beam.beamType.get_Value() != 'cosmics':
    if simFlags.CavernCuts:
        simFlags.EventFilter.set_Off()
        # Make a bigger world volume for cavern bg
        simFlags.WorldZRange.set_Value(26050)
    else:
        simFlags.EventFilter.set_On()

include("G4AtlasApps/G4Atlas.flat.configuration.py")

## Always enable the looper killer, unless it's been disabled
if not hasattr(runArgs, "enableLooperKiller") or runArgs.enableLooperKiller:
    simFlags.OptionalUserActionList.addAction('G4UA::LooperKillerTool', ['Step'])
else:
    atlasG4log.warning("The looper killer will NOT be run in this job.")

from AthenaCommon.AlgSequence import AlgSequence
topSeq = AlgSequence()

## Set Overall per-Algorithm time-limit on the AlgSequence
topSeq.TimeOut = 43200 * Units.s

try:
    from RecAlgs.RecAlgsConf import TimingAlg
    topSeq+=TimingAlg("SimTimerBegin", TimingObjOutputName = "EVNTtoHITS_timings")
except:
    atlasG4log.warning('Could not add TimingAlg, no timing info will be written out.')

from AthenaCommon.CfgGetter import getAlgorithm
topSeq += getAlgorithm("BeamEffectsAlg", tryDefaultConfigurable=True)

#from G4AtlasApps.PyG4Atlas import PyG4AtlasAlg #HACK
#topSeq += PyG4AtlasAlg() #HACK

from AthenaCommon.CfgGetter import getAlgorithm
topSeq += getAlgorithm("G4AtlasAlg",tryDefaultConfigurable=True)

## Add AMITag MetaData to TagInfoMgr
if hasattr(runArgs, 'AMITag'):
    if runArgs.AMITag != "NONE":
        from AthenaCommon.AppMgr import ServiceMgr as svcMgr
        svcMgr.TagInfoMgr.ExtraTagValuePairs += ["AMITag", runArgs.AMITag]

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

# Avoid command line postInclude for stopped particles
if hasattr(runArgs, "outputEVNT_STOPPEDFile"):
    include('SimulationJobOptions/postInclude.StoppedParticleWrite.py')

## Post-exec
if hasattr(runArgs, "postExec"):
    atlasG4log.info("transform post-exec")
    for cmd in runArgs.postExec:
        atlasG4log.info(cmd)
        exec(cmd)
