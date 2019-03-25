# skeleton.EVGENtoRDO.py
# skeleton file for running simulation+digi in one job for FastChain
# currently using full simulation and digi, will swap in fast components later
# Richard Hawkings, Dec 2014, based on work by Robert Harrington
# started by merging simulation and digitisaton skeletons, then modifying
# removed cosmics, beam gas/halo and pileup configuration
# trigger will not be run

from AthenaCommon.Logging import *
#from AthenaCommon.Logging import logging
fast_chain_log = logging.getLogger('FastChainSkeleton')
digilog = fast_chain_log
#logDigitization_flags = logging.getLogger( 'Digitization' )
#digilog = logging.getLogger('Digi_trf')
#logConfigDigitization = logging.getLogger( 'ConfigDigitization' )

fast_chain_log.info( '****************** STARTING EVNTtoRDO *****************' )

fast_chain_log.info( '**** Transformation run arguments' )
fast_chain_log.info( str(runArgs) )

from AthenaCommon import CfgGetter
import AthenaCommon.SystemOfUnits as Units


### Start of Sim

## Include common skeleton
#include("SimuJobTransforms/skeleton.EVGENtoHIT.py")
###########################Skeleton.EVGENtoHIT.py##########################

#include("SimuJobTransforms/CommonSkeletonJobOptions.py")

#####################CommonSkeletonJobOptions.py##########################

from PerfMonComps.PerfMonFlags import jobproperties as pmon_properties
pmon_properties.PerfMonFlags.doMonitoring=True
pmon_properties.PerfMonFlags.doSemiDetailedMonitoring=True

######################################################################
#                                                                    #
# Place holder for numerous common job options of skeleton.XXX files #
# Creation: David Cote (September 2009)                              #
#                                                                    #
######################################################################
from AthenaCommon.GlobalFlags import globalflags
from AthenaCommon.BeamFlags import jobproperties
from AthenaCommon.AthenaCommonFlags import athenaCommonFlags
from G4AtlasApps.SimFlags import simFlags
from ISF_Config.ISF_jobProperties import ISF_Flags
from Digitization.DigitizationFlags import digitizationFlags
#from AthenaCommon.BFieldFlags import jobproperties ##Not sure if this is appropriate for G4 sim

## Max/skip events
if hasattr(runArgs,"skipEvents"):
    athenaCommonFlags.SkipEvents.set_Value_and_Lock( runArgs.skipEvents )


if hasattr(runArgs,"maxEvents"):
    athenaCommonFlags.EvtMax.set_Value_and_Lock( runArgs.maxEvents )
else:
    athenaCommonFlags.EvtMax=-1


if hasattr(runArgs,"conditionsTag"):
    if runArgs.conditionsTag != 'NONE':
        globalflags.ConditionsTag.set_Value_and_Lock( runArgs.conditionsTag ) #make this one compulsory?
        digitizationFlags.IOVDbGlobalTag = runArgs.conditionsTag


if hasattr(runArgs,"beamType"):
    if runArgs.beamType != 'NONE':
        # Setting beamType='cosmics' keeps cavern in world volume for g4sim also with non-commissioning geometries
        jobproperties.Beam.beamType.set_Value_and_Lock( runArgs.beamType )


# Avoid command line preInclude for event service
if hasattr(runArgs, "eventService") and runArgs.eventService:
    include('AthenaMP/AthenaMP_EventService.py')


#####################Back to Skeleton.EVGENtoHIT.py######################
if hasattr(runArgs, "jobNumber"):
    if runArgs.jobNumber < 1:
        raise ValueError('jobNumber must be a postive integer. %s lies outside this range', str(runArgs.jobNumber))

if hasattr(runArgs, "inputTXT_EVENTIDFile"):
    from OverlayCommonAlgs.OverlayFlags import overlayFlags
    overlayFlags.EventIDTextFile = runArgs.inputTXT_EVENTIDFile[0]
    simFlags.load_atlas_flags()
    if hasattr(simFlags, 'IsEventOverlayInputSim'):
        simFlags.IsEventOverlayInputSim = True
    if hasattr(runArgs, 'inputVertexPosFile'):
        simFlags.VertexOverrideFile= runArgs.inputVertexPosFile

if hasattr(runArgs, 'truthStrategy'):
    simFlags.TruthStrategy    = runArgs.truthStrategy
    try:
        from BarcodeServices.BarcodeServicesConfig import barcodeOffsetForTruthStrategy
        simFlags.SimBarcodeOffset  = barcodeOffsetForTruthStrategy(runArgs.truthStrategy)
    except RuntimeError:
        Offset200kStrings = ['MC12', 'MC15a', 'MC16']
        is200kOffset = any(x in runArgs.truthStrategy for x in Offset200kStrings)
        if is200kOffset:
            simFlags.SimBarcodeOffset  = 200000 #MC12, MC15a, MC16 setting
        else:
            simFlags.SimBarcodeOffset  = 1000000 #MC15 setting
        atlasG4log.warning('Using unknown truth strategy '+str(runArgs.truthStrategy)+' guessing that barcode offset is '+str(simFlags.SimBarcodeOffset))
else:
    simFlags.TruthStrategy    = 'MC12'
    simFlags.SimBarcodeOffset  = 200000 #MC12 setting



##############################Back to MyCustomSkeleton########################

## Get the logger
fast_chain_log.info('****************** STARTING ISF ******************')

### Force trigger to be off
from RecExConfig.RecFlags import rec
rec.doTrigger.set_Value_and_Lock(False)


## Simulation flags need to be imported first
simFlags.load_atlas_flags()
if hasattr(runArgs, 'useISF') and not runArgs.useISF:
    raise RuntimeError("Unsupported configuration! If you want to run with useISF=False, please use AtlasG4_tf.py!")
simFlags.ISFRun=True

## Set simulation geometry tag
if hasattr(runArgs, 'geometryVersion'):
    simFlags.SimLayout.set_Value_and_Lock(runArgs.geometryVersion)
    fast_chain_log.debug('SimLayout set to %s' % simFlags.SimLayout)
    if runArgs.geometryVersion.endswith("_VALIDATION"):
        pos=runArgs.geometryVersion.find("_VALIDATION")
        globalflags.DetDescrVersion.set_Value_and_Lock( runArgs.geometryVersion[:pos] )
    else:
        globalflags.DetDescrVersion.set_Value_and_Lock( runArgs.geometryVersion )
    fast_chain_log.debug('DetDescrVersion set to %s' % globalflags.DetDescrVersion)
else:
    raise RuntimeError("No geometryVersion provided.")


## AthenaCommon flags
# Jobs should stop if an include fails.
if hasattr(runArgs, "IgnoreConfigError"):
    athenaCommonFlags.AllowIgnoreConfigError = runArgs.IgnoreConfigError
else:
    athenaCommonFlags.AllowIgnoreConfigError = False

athenaCommonFlags.DoFullChain=True

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
elif hasattr(runArgs, "inputEVNT_TRFile"):
    setInputEvgenFileJobProperties( runArgs.inputEVNT_TRFile )
elif jobproperties.Beam.beamType.get_Value() == 'cosmics':
    fast_chain_log.debug('No inputEVNTFile provided. OK, as performing cosmics simulation.')
    athenaCommonFlags.PoolEvgenInput.set_Off()
else:
    fast_chain_log.info('No inputEVNTFile provided. Assuming that you are running a generator on the fly.')
    athenaCommonFlags.PoolEvgenInput.set_Off()

## Output hits file config
if hasattr(runArgs, "outputHITSFile"):
    athenaCommonFlags.PoolHitsOutput.set_Value_and_Lock( runArgs.outputHITSFile )
else:
    if hasattr(runArgs, "outputEVNT_TRFile"):
        if hasattr(runArgs,"trackRecordType") and runArgs.trackRecordType=="stopped":
            simFlags.StoppedParticleFile.set_Value_and_Lock( runArgs.outputEVNT_TRFile )
    #raise RuntimeError("No outputHITSFile provided.")
    fast_chain_log.info('No outputHITSFile provided. This simulation job will not write out any HITS file.')
    athenaCommonFlags.PoolHitsOutput = ""
    athenaCommonFlags.PoolHitsOutput.statusOn = False

#==============================================================
# Job Configuration parameters:
#==============================================================
## Pre-exec
if hasattr(runArgs, "preSimExec"):
    fast_chain_log.info("transform pre-sim exec")
    for cmd in runArgs.preSimExec:
        fast_chain_log.info(cmd)
        exec(cmd)

## Pre-include
if hasattr(runArgs, "preSimInclude"):
    for fragment in runArgs.preSimInclude:
        include(fragment)

if hasattr(runArgs, "inputEVNT_TRFile"):
    if hasattr(runArgs,"trackRecordType") and runArgs.trackRecordType=="stopped":
        include('SimulationJobOptions/preInclude.ReadStoppedParticles.py')

# get top sequence
from AthenaCommon.AlgSequence import AlgSequence
topSequence = AlgSequence()

# Avoid command line preInclude for cavern background
if jobproperties.Beam.beamType.get_Value() != 'cosmics':
    # If it was already there, then we have a stopped particle file
    if hasattr(runArgs, "inputEVNT_TRFile") and\
        not hasattr(topSequence,'TrackRecordGenerator'):
        include('SimulationJobOptions/preInclude.G4ReadCavern.py')
    # If there's a stopped particle file, don't do all the cavern stuff
    if hasattr(runArgs, "outputEVNT_TRFile") and\
        not (hasattr(simFlags,'StoppedParticleFile') and simFlags.StoppedParticleFile.statusOn and simFlags.StoppedParticleFile.get_Value()!=''):
        include('SimulationJobOptions/preInclude.G4WriteCavern.py')

if jobproperties.Beam.beamType.get_Value() == 'cosmics':
    ISF_Flags.Simulator.set_Value_and_Lock('CosmicsG4')
elif hasattr(runArgs, 'simulator'):
    ISF_Flags.Simulator.set_Value_and_Lock(runArgs.simulator)
else:
    ISF_Flags.Simulator.set_Value_and_Lock('MC12G4')

from AthenaCommon.DetFlags import DetFlags

#Trial block: Set off tasks at start
## Switch off tasks
#    DetFlags.pileup.all_setOff()
#    DetFlags.simulateLVL1.all_setOff()
#    DetFlags.digitize.all_setOff()
#if not simFlags.IsEventOverlayInputSim():
#DetFlags.overlay.all_setOff()

#    DetFlags.readRDOPool.all_setOff()
#    DetFlags.makeRIO.all_setOff()
#    DetFlags.writeBS.all_setOff()
#    DetFlags.readRDOBS.all_setOff()
#    DetFlags.readRIOBS.all_setOff()
#    DetFlags.readRIOPool.all_setOff()
#    DetFlags.writeRIOPool.all_setOff()
#    DetFlags.writeRDOPool.all_setOff()


#set flags ON:

#Tasks we want switched ON (write RDOPool) - want this for all detectors that we want ON:
#DetFlags.writeRDOPool.all_setOn()

#### this flag turns all the detectors ON that we want for simulation.
try:
    from ISF_Config import FlagSetters
    FlagSetters.configureFlagsBase()
    ## Check for any simulator-specific configuration
    configureFlags = getattr(FlagSetters, ISF_Flags.Simulator.configFlagsMethodName(), None)
    if configureFlags is not None:
        configureFlags()
    possibleSubDetectors=['pixel','SCT','TRT','BCM','Lucid','ZDC','ALFA','AFP','FwdRegion','LAr','HGTD','Tile','MDT','CSC','TGC','RPC','Micromegas','sTGC','Truth']
    for subdet in possibleSubDetectors:
        simattr = subdet+"_on"
        simcheck = getattr(DetFlags.simulate, simattr, None)
        if simcheck is not None and simcheck():
            attrname = subdet+"_setOn"
            checkfn = getattr(DetFlags, attrname, None)
            if checkfn is not None:
                checkfn()

except:
    ## Select detectors
    if 'DetFlags' not in dir():
        # from AthenaCommon.DetFlags import DetFlags
        ## If you configure one det flag, you're responsible for configuring them all!
        DetFlags.all_setOn()


#DetFlags.all_setOn()
DetFlags.LVL1_setOff()
DetFlags.Truth_setOn()
DetFlags.Forward_setOff() # Forward dets are off by default
DetFlags.Micromegas_setOff()
DetFlags.sTGC_setOff()
DetFlags.FTK_setOff()
checkHGTDOff = getattr(DetFlags, 'HGTD_setOff', None)
if checkHGTDOff is not None:
    checkHGTDOff() #Default for now

# from AthenaCommon.DetFlags import DetFlags

# from AthenaCommon.DetFlags import DetFlags
    ## Tidy up DBM DetFlags: temporary measure
DetFlags.DBM_setOff()

if hasattr(simFlags, 'SimulateNewSmallWheel'):
    if simFlags.SimulateNewSmallWheel():
        DetFlags.sTGC_setOn()
        DetFlags.Micromegas_setOn()

#if simFlags.ForwardDetectors.statusOn:
#    if DetFlags.geometry.FwdRegion_on():
#        from AthenaCommon.AppMgr import ToolSvc
#        ToolSvc += CfgGetter.getPublicTool("ForwardRegionProperties")

### Set digitize all except forward detectors
DetFlags.digitize.all_setOn()
DetFlags.digitize.LVL1_setOff()
DetFlags.digitize.ZDC_setOff()
DetFlags.digitize.Micromegas_setOff()
DetFlags.digitize.sTGC_setOff()
DetFlags.digitize.Forward_setOff()
DetFlags.digitize.Lucid_setOff()
DetFlags.digitize.AFP_setOff()
DetFlags.digitize.ALFA_setOff()

#set all detdescr on except fwd.
#DetFlags.detdescr.all_setOn()
#DetFlags.detdescr.LVL1_setOff()
#DetFlags.detdescr.ZDC_setOff()
#DetFlags.detdescr.Micromegas_setOff()
#DetFlags.detdescr.sTGC_setOff()
#DetFlags.detdescr.Forward_setOff()
#DetFlags.detdescr.Lucid_setOff()
#DetFlags.detdescr.AFP_setOff()
#DetFlags.detdescr.ALFA_setOff()

#--------------------------------------------------------------
# Set Detector flags for this run
#--------------------------------------------------------------
if 'DetFlags' in dir():

    DetFlags.Print()
    #DetFlags.overlay.all_setOff()

#DetFlags.simulate.all_setOff()
DetFlags.makeRIO.all_setOff()
DetFlags.writeBS.all_setOff()
DetFlags.readRDOBS.all_setOff()
DetFlags.readRIOBS.all_setOff()
DetFlags.readRIOPool.all_setOff()
DetFlags.writeRIOPool.all_setOff()


# temporary fix to ensure TRT will record hits if using FATRAS
# this should eventually be removed when it is configured properly in ISF
if hasattr(runArgs, 'simulator') and runArgs.simulator.find('ATLFASTIIF')>=0:
    from TrkDetDescrSvc.TrkDetDescrJobProperties import TrkDetFlags
    TrkDetFlags.TRT_BuildStrawLayers=True
    fast_chain_log.info('Enabled TRT_BuildStrawLayers to get hits in ATLFASTIIF')


DetFlags.Print()

# removed configuration of forward detectors from standard simulation config
# corresponding code block removed

## Set the PhysicsList
if hasattr(runArgs, 'physicsList'):
    simFlags.PhysicsList = runArgs.physicsList


## Random seed
if hasattr(runArgs, "randomSeed"):
    simFlags.RandomSeedOffset = int(runArgs.randomSeed)
else:
    fast_chain_log.warning('randomSeed not set')
## Don't use the SeedsG4 override
simFlags.SeedsG4.set_Off()

## The looper killer is on by default. Disable it if this is requested.
if hasattr(runArgs, "enableLooperKiller") and not runArgs.enableLooperKiller:
    simFlags.OptionalUserActionList.removeAction('G4UA::LooperKillerTool')
    fast_chain_log.warning("The looper killer will NOT be run in this job.")


## Set the Run Number (if required)
if hasattr(runArgs,"DataRunNumber"):
    if runArgs.DataRunNumber>0:
        fast_chain_log.info( 'Overriding run number to be: %s ', runArgs.DataRunNumber )
        simFlags.RunNumber=runArgs.DataRunNumber
        digitizationFlags.dataRunNumber=runArgs.DataRunNumber
elif hasattr(runArgs,'jobNumber'):
    if runArgs.jobNumber>=0:
        fast_chain_log.info( 'Using job number '+str(runArgs.jobNumber)+' to derive run number.' )
        simFlags.RunNumber = simFlags.RunDict.GetRunNumber( runArgs.jobNumber )
        fast_chain_log.info( 'Set run number based on dictionary to '+str(simFlags.RunNumber) )

## removed code block for handling cosmics track record

#--------------------------------------------------------------
# Override pile-up configuration on the command-line
#--------------------------------------------------------------

PileUpConfigOverride=False
import math

## First check for depreacted command-line options
if hasattr(runArgs,"numberOfLowPtMinBias"):
    if not math.fabs(digitizationFlags.numberOfLowPtMinBias.get_Value()-runArgs.numberOfLowPtMinBias)<0.00000001: #FIXME comparing two floats
        fast_chain_log.info( "Changing digitizationFlags.numberOfLowPtMinBias from %s to %s", digitizationFlags.numberOfLowPtMinBias.get_Value(),runArgs.numberOfLowPtMinBias)
        digitizationFlags.numberOfLowPtMinBias=float(runArgs.numberOfLowPtMinBias)
        PileUpConfigOverride=True
if hasattr(runArgs,"numberOfHighPtMinBias"):
    if not math.fabs(digitizationFlags.numberOfHighPtMinBias.get_Value()-runArgs.numberOfHighPtMinBias)<0.00000001: #FIXME comparing two floats
        fast_chain_log.info( "Changing digitizationFlags.numberOfHighPtMinBias from %s to %s", digitizationFlags.numberOfHighPtMinBias.get_Value(),runArgs.numberOfHighPtMinBias)
        digitizationFlags.numberOfHighPtMinBias=float(runArgs.numberOfHighPtMinBias)
        PileUpConfigOverride=True
if hasattr(runArgs,"numberOfBeamHalo"):
    if not math.fabs(digitizationFlags.numberOfBeamHalo.get_Value()-runArgs.numberOfBeamHalo)<0.00000001: #FIXME comparing two floats
        fast_chain_log.info( "Changing digitizationFlags.numberOfBeamHalo from %s to %s", digitizationFlags.numberOfBeamHalo.get_Value(),runArgs.numberOfBeamHalo)
        digitizationFlags.numberOfBeamHalo=float(runArgs.numberOfBeamHalo)
        PileUpConfigOverride=True
if hasattr(runArgs,"numberOfBeamGas"):
    if not math.fabs(digitizationFlags.numberOfBeamGas.get_Value()-runArgs.numberOfBeamGas)<0.00000001: #FIXME comparing two floats
        fast_chain_log.info( "Changing digitizationFlags.numberOfBeamGas from %s to %s", digitizationFlags.numberOfBeamGas.get_Value(),runArgs.numberOfBeamGas)
        digitizationFlags.numberOfBeamGas=float(runArgs.numberOfBeamGas)
        PileUpConfigOverride=True
if hasattr(runArgs,"numberOfCavernBkg"):
    if not digitizationFlags.numberOfCavern.get_Value()==runArgs.numberOfCavernBkg:
        fast_chain_log.info( "Changing digitizationFlags.cavernEvents from %s to %s", digitizationFlags.numberOfCavern.get_Value(),runArgs.numberOfCavernBkg)
        digitizationFlags.numberOfCavern=runArgs.numberOfCavernBkg
        PileUpConfigOverride=True
if hasattr(runArgs,"bunchSpacing"):
    if  digitizationFlags.BeamIntensityPattern.statusOn:
        fast_chain_log.warning("Redefine bunch-structure with a fixed %s ns bunchSpacing. Overwriting the previous setting of %s", runArgs.bunchSpacing,digitizationFlags.BeamIntensityPattern.get_Value())
    else:
        fast_chain_log.info( "Setting up job to run with a fixed %s ns bunchSpacing.", runArgs.bunchSpacing)
    digitizationFlags.bunchSpacing = 25
    digitizationFlags.BeamIntensityPattern.createConstBunchSpacingPattern(int(runArgs.bunchSpacing)) #FIXME This runArg should probably inherit from argInt rather than argFloat
    fast_chain_log.info( "New bunch-structure = %s", digitizationFlags.BeamIntensityPattern.get_Value())
    jobproperties.Beam.bunchSpacing = int(runArgs.bunchSpacing) #FIXME This runArg should probably inherit from argInt rather than argFloat
    PileUpConfigOverride=True
if hasattr(runArgs,"pileupInitialBunch"):
    if not (digitizationFlags.initialBunchCrossing.get_Value()==runArgs.pileupInitialBunch):
        fast_chain_log.info( "Changing digitizationFlags.initialBunchCrossing from %s to %s", digitizationFlags.initialBunchCrossing.get_Value(),runArgs.pileupInitialBunch)
        digitizationFlags.initialBunchCrossing=runArgs.pileupInitialBunch
        PileUpConfigOverride=True
if hasattr(runArgs,"pileupFinalBunch"):
    if not (digitizationFlags.finalBunchCrossing.get_Value()==runArgs.pileupFinalBunch):
        fast_chain_log.info( "Changing digitizationFlags.finalBunchCrossing from %s to %s", digitizationFlags.finalBunchCrossing.get_Value(),runArgs.pileupFinalBunch)
        digitizationFlags.finalBunchCrossing=runArgs.pileupFinalBunch
        PileUpConfigOverride=True
if hasattr(runArgs,"digiSteeringConf"):
    if not (digitizationFlags.digiSteeringConf.get_Value()==runArgs.digiSteeringConf+"PileUpToolsAlg"):
        fast_chain_log.info( "Changing digitizationFlags.digiSteeringConf from %s to %s", digitizationFlags.digiSteeringConf.get_Value(),runArgs.digiSteeringConf)
        digitizationFlags.digiSteeringConf=runArgs.digiSteeringConf+"PileUpToolsAlg"
        PileUpConfigOverride=True
if PileUpConfigOverride:
    fast_chain_log.info( "NB Some pile-up (re-)configuration was done on the command-line.")
del PileUpConfigOverride


#--------------------------------------------------------------
# Get the flags
#--------------------------------------------------------------
if hasattr(runArgs,"digiSeedOffset1"):
    digitizationFlags.rndmSeedOffset1=int(runArgs.digiSeedOffset1)
else:
    fast_chain_log.warning( 'digiSeedOffset1 not set' )
    digitizationFlags.rndmSeedOffset1=1

if hasattr(runArgs,"digiSeedOffset2"):
    digitizationFlags.rndmSeedOffset2=int(runArgs.digiSeedOffset2)
else:
    fast_chain_log.warning( 'digiSeedOffset2 not set' )
    digitizationFlags.rndmSeedOffset2=2

if hasattr(runArgs,"samplingFractionDbTag"): #FIXME change this to PhysicsList?
    digitizationFlags.physicsList=runArgs.samplingFractionDbTag

if hasattr(runArgs,"digiRndmSvc"):
    digitizationFlags.rndmSvc=runArgs.digiRndmSvc

if hasattr(runArgs,"PileUpPremixing"):
    fast_chain_log.info("Doing pile-up premixing")
    digitizationFlags.PileUpPremixing = runArgs.PileUpPremixing

#--------------------------------------------------------------
# Pileup configuration
#--------------------------------------------------------------
from SimuJobTransforms.SimTransformUtils import makeBkgInputCol
def HasInputFiles(runArgs, key):
    if hasattr(runArgs, key):
        cmd='runArgs.%s' % key
        if eval(cmd):
            return True
    return False

## Low Pt minbias set-up
bkgArgName="LowPtMinbiasHitsFile"
if hasattr(runArgs, "inputLowPtMinbiasHitsFile"):
    bkgArgName="inputLowPtMinbiasHitsFile"
if HasInputFiles(runArgs, bkgArgName):
    exec("bkgArg = runArgs."+bkgArgName)
    digitizationFlags.LowPtMinBiasInputCols = makeBkgInputCol(bkgArg,
                                                              digitizationFlags.numberOfLowPtMinBias.get_Value(), True, fast_chain_log)
if digitizationFlags.LowPtMinBiasInputCols.statusOn:
    digitizationFlags.doLowPtMinBias = True
else:
    digitizationFlags.doLowPtMinBias = False

## High Pt minbias set-up
bkgArgName="HighPtMinbiasHitsFile"
if hasattr(runArgs, "inputHighPtMinbiasHitsFile"):
    bkgArgName="inputHighPtMinbiasHitsFile"
if HasInputFiles(runArgs, bkgArgName):
    exec("bkgArg = runArgs."+bkgArgName)
    digitizationFlags.HighPtMinBiasInputCols = makeBkgInputCol(bkgArg,
                                                               digitizationFlags.numberOfHighPtMinBias.get_Value(), True, fast_chain_log)
if digitizationFlags.HighPtMinBiasInputCols.statusOn:
    digitizationFlags.doHighPtMinBias = True
else:
    digitizationFlags.doHighPtMinBias = False

## Cavern Background set-up
bkgArgName="cavernHitsFile"
if hasattr(runArgs, "inputCavernHitsFile"):
    bkgArgName="inputCavernHitsFile"
if HasInputFiles(runArgs, bkgArgName):
    exec("bkgArg = runArgs."+bkgArgName)
    digitizationFlags.cavernInputCols = makeBkgInputCol(bkgArg,
                                                        digitizationFlags.numberOfCavern.get_Value(), (not digitizationFlags.cavernIgnoresBeamInt.get_Value()), fast_chain_log)
if digitizationFlags.cavernInputCols.statusOn:
    digitizationFlags.doCavern = True
else:
    digitizationFlags.doCavern = False

## Beam Halo set-up
bkgArgName="beamHaloHitsFile"
if hasattr(runArgs, "inputBeamHaloHitsFile"):
    bkgArgName="inputBeamHaloHitsFile"
if HasInputFiles(runArgs, bkgArgName):
    exec("bkgArg = runArgs."+bkgArgName)
    digitizationFlags.beamHaloInputCols = makeBkgInputCol(bkgArg,
                                                          digitizationFlags.numberOfBeamHalo.get_Value(), True, fast_chain_log)
if digitizationFlags.beamHaloInputCols.statusOn:
    digitizationFlags.doBeamHalo = True
else:
    digitizationFlags.doBeamHalo = False

## Beam Gas set-up
bkgArgName="beamGasHitsFile"
if hasattr(runArgs, "inputBeamGasHitsFile"):
    bkgArgName="inputBeamGasHitsFile"
if HasInputFiles(runArgs, bkgArgName):
    exec("bkgArg = runArgs."+bkgArgName)
    digitizationFlags.beamGasInputCols = makeBkgInputCol(bkgArg,
                                                         digitizationFlags.numberOfBeamGas.get_Value(), True, fast_chain_log)
if digitizationFlags.beamGasInputCols.statusOn:
    digitizationFlags.doBeamGas = True
else:
    digitizationFlags.doBeamGas = False


#--------------------------------------------------------------
# Other configuration: LVL1, turn off sub detectors, calo noise
#--------------------------------------------------------------

if hasattr(runArgs,"doAllNoise"):
    if runArgs.doAllNoise!="NONE":
        fast_chain_log.info('doAllNoise = %s: Overriding doInDetNoise, doCaloNoise and doMuonNoise', runArgs.doAllNoise)
        if runArgs.doAllNoise=="True":
            digitizationFlags.doInDetNoise=True
            digitizationFlags.doCaloNoise=True
            digitizationFlags.doMuonNoise=True
        else:
            digitizationFlags.doInDetNoise=False
            digitizationFlags.doCaloNoise=False
            digitizationFlags.doMuonNoise=False



### No RDO output
## Output RDO File
if hasattr(runArgs,"outputRDOFile") or hasattr(runArgs,"tmpRDO"):
    if hasattr(runArgs,"outputRDOFile"):
        if hasattr(runArgs,"tmpRDO"):
            fast_chain_log.fatal("Both outputRDOFile and tmpRDO specified - this configuration should not be used!")
            raise SystemError
        athenaCommonFlags.PoolRDOOutput.set_Value_and_Lock( runArgs.outputRDOFile )
    if hasattr(runArgs,"tmpRDO"):
        athenaCommonFlags.PoolRDOOutput.set_Value_and_Lock( runArgs.tmpRDO )
    if hasattr(runArgs, "AddCaloDigi"):
        AddCaloDigi = runArgs.AddCaloDigi
        if AddCaloDigi:
            fast_chain_log.info("Will write out all LArDigitContainers and TileDigitsContainers to RDO file.")
            digitizationFlags.experimentalDigi+=["AddCaloDigi"]
else:
    fast_chain_log.info("no output file (outputRDOFile or tmpRDO) specified - switching off output StreamRDO")

## Set Overall per-Algorithm time-limit on the AlgSequence
topSequence.TimeOut = 43200 * Units.s

try:
    from RecAlgs.RecAlgsConf import TimingAlg
    topSequence+=TimingAlg("SimTimerBegin", TimingObjOutputName = "EVNTtoHITS_timings")
except:
    fast_chain_log.warning('Could not add TimingAlg, no timing info will be written out.')

#### *********** import ISF_Example code here **************** ####

#include("ISF_Config/ISF_ConfigJobInclude.py")




########## ISF_ConfigJobInclude.py #################




"""
Common configurations for ISF
KG Tan, 17/06/2012
"""

#include.block('ISF_Config/ISF_ConfigJobInclude.py')

#--------------------------------------------------------------
# Set to monte carlo
#--------------------------------------------------------------
import AthenaCommon.AtlasUnixStandardJob
from AthenaCommon.AppMgr import theApp
from AthenaCommon.AppMgr import ServiceMgr
from AthenaCommon.AppMgr import ServiceMgr as svcMgr

# TODO: ELLI: remove this once the envelopes are stored in the DDDB
#             -> currently a fallback python definition is used
#import shutil
#import os.path
#if os.path.exists('geomDB') and os.path.islink('geomDB'):
    #os.unlink('geomDB')
#os.symlink('/afs/cern.ch/atlas/groups/Simulation/ISF/Envelopes_geomDB', 'geomDB')


#--------------------------------------------------------------
# Set the flags automatically here
# (move it to the job options if not meant to be automatic!)
#--------------------------------------------------------------

# --- set globalflags
globalflags.DataSource.set_Value_and_Lock('geant4')
if jobproperties.Beam.beamType == "cosmics" :
    globalflags.DetGeo.set_Value_and_Lock('commis')
else:
    globalflags.DetGeo.set_Value_and_Lock('atlas')
globalflags.Luminosity.set_Off()

# --- set SimLayout (synchronised to globalflags)
if globalflags.DetDescrVersion() not in simFlags.SimLayout.get_Value():
    print "ERROR globalFlags.DetDescrVersion and simFlags.SimLayout do not match!"
    print "Please correct your job options."
    # TODO: theApp.exit(1)?
    import sys
    sys.exit(1)
simFlags.load_atlas_flags()
simFlags.EventFilter.set_Off()

# --- Read in special simulation job option fragments based on
# --- metadata passed by the evgen stage (move earlier?)
from ISF_Example.ISF_Metadata import checkForSpecialConfigurationMetadata
checkForSpecialConfigurationMetadata()
#--------------------------------------------------------------
# Read Simulation MetaData (unless override flag set to True)
#--------------------------------------------------------------
#if 'ALL' in digitizationFlags.overrideMetadata.get_Value():
#    fast_chain_log.info("Skipping input file MetaData check.")
#else :
#    from Digitization.DigitizationReadMetaData import readHITSFileMetadata
#    readHITSFileMetadata()


# all det description
include('ISF_Config/AllDet_detDescr.py')

DetFlags.Print()

#check job configuration
from Digitization.DigiConfigCheckers import checkDetFlagConfiguration
checkDetFlagConfiguration()

from FullChainTransforms.FastChainConfigCheckers import syncDigitizationAndSimulationJobProperties
syncDigitizationAndSimulationJobProperties()

#--------------------------------------------------------------
# Pileup configuration
#--------------------------------------------------------------
from Digitization.DigiConfigCheckers import syncDetFlagsAndDigitizationJobProperties
syncDetFlagsAndDigitizationJobProperties()
DetFlags.Print()

if len(globalflags.ConditionsTag()):
    from IOVDbSvc.CondDB import conddb
    conddb.setGlobalTag(globalflags.ConditionsTag())

## Translate conditions tag into IOVDbSvc global tag: must be done before job properties are locked!!!
if not hasattr(ServiceMgr, 'IOVDbSvc'):
    from IOVDbSvc.IOVDbSvcConf import IOVDbSvc
    ServiceMgr += IOVDbSvc()
if not hasattr(globalflags, "ConditionsTag") or not globalflags.ConditionsTag.get_Value():
    raise SystemExit("AtlasSimSkeleton._do_jobproperties :: Global ConditionsTag not set")
if not hasattr(ServiceMgr.IOVDbSvc, 'GlobalTag') or not ServiceMgr.IOVDbSvc.GlobalTag:
        ServiceMgr.IOVDbSvc.GlobalTag = globalflags.ConditionsTag.get_Value()

# Temporary work-around - see ATLASSIM-2351
if ISF_Flags.UsingGeant4():
    #include("G4AtlasApps/G4Atlas.flat.configuration.py") #HACK

    ####################G4Atlas.flat.configuration.py#################


    ## _PyG4AtlasComp.__init__
    ## If the random number service hasn't been set up already, do it now.
    simFlags.RandomSeedList.useDefaultSeeds()

    ## AtlasSimSkeleton._do_jobproperties
    ## Import extra flags if it hasn't already been done
    if "atlas_flags" not in simFlags.extra_flags:
        simFlags.load_atlas_flags()
    if jobproperties.Beam.beamType() == "cosmics" and "cosmics_flags" not in simFlags.extra_flags:
        simFlags.load_cosmics_flags()

    # Switch off GeoModel Release in the case of parameterization
    if simFlags.LArParameterization.get_Value()>0 and simFlags.ReleaseGeoModel():
        simFlags.ReleaseGeoModel = False

    ## Enable floating point exception handling
    ## FIXME! This seems to cause the jobs to crash in the FpeControlSvc, so commenting this out for now...
    #athenaCommonFlags.RuntimeStrictness = 'abort'

    from AthenaCommon.JobProperties import jobproperties

    DetFlags.Print()


    jobproperties.print_JobProperties('tree&value')

    ## AtlasSimSkeleton._do_external
    from AthenaCommon.AppMgr import ToolSvc
    from Geo2G4.Geo2G4Conf import Geo2G4Svc
    geo2G4Svc = Geo2G4Svc()
    theApp.CreateSvc += ["Geo2G4Svc"]
    ServiceMgr += geo2G4Svc
    ## Enable top transforms for the ATLAS geometry
    geo2G4Svc.GetTopTransform = True

    ## GeoModel stuff
    ## TODO: Tidy imports etc.
    from GeoModelSvc.GeoModelSvcConf import GeoModelSvc
    from AtlasGeoModel import SetGeometryVersion

    ## Forward Region Twiss files - needed before geometry setup!

    if simFlags.ForwardDetectors.statusOn:
        if DetFlags.geometry.FwdRegion_on():
            from AthenaCommon.AppMgr import ToolSvc
            ToolSvc += CfgGetter.getPublicTool("ForwardRegionProperties")


    from AtlasGeoModel import GeoModelInit
    from AtlasGeoModel import SimEnvelopes
    gms = GeoModelSvc()
    ## Cosmics GeoModel tweaks
    if jobproperties.Beam.beamType() == 'cosmics' or \
            (simFlags.CavernBG.statusOn and not 'Signal' in simFlags.CavernBG.get_Value() ):
        from CavernInfraGeoModel.CavernInfraGeoModelConf import CavernInfraDetectorTool
        gms.DetectorTools += [ CavernInfraDetectorTool() ]
    ## Protects GeoModelSvc in the simulation from the AlignCallbacks
    gms.AlignCallbacks = False
    ## Muon GeoModel tweaks
    if DetFlags.Muon_on():
        ## Turn off caching in the muon system
        from MuonGeoModel.MuonGeoModelConf import MuonDetectorTool
        MuonDetectorTool = MuonDetectorTool()
        MuonDetectorTool.FillCacheInitTime = 0 # default is 1
        if hasattr(simFlags, 'SimulateNewSmallWheel'):
            if simFlags.SimulateNewSmallWheel():
                MuonDetectorTool.StationSelection  = 2
                MuonDetectorTool.SelectedStations  = [ "EIL1" ]
                MuonDetectorTool.SelectedStations  += [ "EIL2" ]
                MuonDetectorTool.SelectedStations  += [ "EIL6" ]
                MuonDetectorTool.SelectedStations  += [ "EIL7" ]
                MuonDetectorTool.SelectedStations  += [ "EIS*" ]
                MuonDetectorTool.SelectedStations  += [ "EIL10" ]
                MuonDetectorTool.SelectedStations  += [ "EIL11" ]
                MuonDetectorTool.SelectedStations  += [ "EIL12" ]
                MuonDetectorTool.SelectedStations  += [ "EIL17" ]
                MuonDetectorTool.SelectedStations  += [ "CSS*" ]
                MuonDetectorTool.SelectedStations  += [ "CSL*" ]
                MuonDetectorTool.SelectedStations  += [ "T4E*" ]
                MuonDetectorTool.SelectedStations  += [ "T4F*" ]

        ## Additional material in the muon system
        from AGDD2GeoSvc.AGDD2GeoSvcConf import AGDDtoGeoSvc
        AGDD2Geo = AGDDtoGeoSvc()
        if not "MuonAGDDTool/MuonSpectrometer" in AGDD2Geo.Builders:
            ToolSvc += CfgGetter.getPublicTool("MuonSpectrometer", checkType=True)
            AGDD2Geo.Builders += ["MuonAGDDTool/MuonSpectrometer"]
        if hasattr(simFlags, 'SimulateNewSmallWheel'):
            if simFlags.SimulateNewSmallWheel():
                if not "NSWAGDDTool/NewSmallWheel" in AGDD2Geo.Builders:
                    ToolSvc += CfgGetter.getPublicTool("NewSmallWheel", checkType=True)
                    AGDD2Geo.Builders += ["NSWAGDDTool/NewSmallWheel"]
        theApp.CreateSvc += ["AGDDtoGeoSvc"]
        ServiceMgr += AGDD2Geo

    ## Add configured GeoModelSvc to service manager
    ServiceMgr += gms


    ###################Back to ISF_ConfigJobInclude.py################

#--------------------------------------------------------------
# Setup the ISF Services
#--------------------------------------------------------------

#--------------------------------------------------------------
# ISF input
#--------------------------------------------------------------

# Note: automatically adds generator to TopSequence if applicable
# if an input sting identifier was given, use ISF input definitions
if ISF_Flags.Input()!="NONE":
    CfgGetter.getAlgorithm('ISF_Input_' + ISF_Flags.Input())
# cosmics input
elif jobproperties.Beam.beamType.get_Value() == 'cosmics':
    simFlags.load_cosmics_flags()
    if simFlags.ReadTR.statusOn:
        CfgGetter.getAlgorithm('TrackRecordCosmicGenerator')
    else:
        CfgGetter.getAlgorithm('EvgenCosmicGenerator')
# non of the above

elif not athenaCommonFlags.PoolEvgenInput.statusOn:
    print "ISF Input Configuration: PoolEvgenInput off, likely running with a particle gun preInclude"
# non of the above -> input via ISF_Flags
else :
    if ISF_Flags.OverrideInputFiles():
        athenaCommonFlags.PoolEvgenInput = ISF_Flags.OverrideInputFiles()
    CfgGetter.getAlgorithm('ISF_Input_GenericFiles')

from ISF_Example.ISF_Input import ISF_Input

topSequence += CfgGetter.getAlgorithm("BeamEffectsAlg")

#--------------------------------------------------------------
# ISF kernel configuration
#--------------------------------------------------------------

# keep reference to collection merger algorithm to guarantee that
# any subsequent simulator configuration gets a reference to the same
# instance when calling confgetter's getAlgorithm
collection_merger_alg = CfgGetter.getAlgorithm('ISF_CollectionMerger')

SimKernel = CfgGetter.getAlgorithm(ISF_Flags.Simulator.KernelName())

if ISF_Flags.HITSMergingRequired():
    topSequence += collection_merger_alg

#--------------------------------------------------------------
# Post kernel configuration
#--------------------------------------------------------------

# proper GeoModel initialization
from GeoModelSvc.GeoModelSvcConf import GeoModelSvc
GeoModelSvc = GeoModelSvc()
if ( DetFlags.detdescr.LAr_on() ):
    GeoModelSvc.DetectorTools["LArDetectorToolNV"].GeometryConfig = "FULL"
if ( DetFlags.detdescr.Tile_on() ):
    GeoModelSvc.DetectorTools["TileDetectorTool"].GeometryConfig = "FULL"


if ISF_Flags.DumpStoreGate() :
    StoreGateSvc.Dump = True


if ISF_Flags.RunVP1() :
    # VP1 part (better switch off PerMon when using VP1)
    from VP1Algs.VP1AlgsConf import VP1Alg
    topSequence += VP1Alg()
    # Performance Monitoring (VP1 does not like this)
    # https://twiki.cern.ch/twiki/bin/viewauth/Atlas/PerfMonSD
    pmon_properties.PerfMonFlags.doMonitoring=False
    pmon_properties.PerfMonFlags.doSemiDetailedMonitoring=False

if not ISF_Flags.DoPerfMonStats() :
    pmon_properties.PerfMonFlags.doMonitoring=False
    pmon_properties.PerfMonFlags.doSemiDetailedMonitoring=False


if ISF_Flags.DumpMcEvent() :
    # McEventCollection Dumper
    DumpMC = CfgMgr.DumpMC("DumpMC")
    DumpMC.McEventKey = "TruthEvent"
    topSequence += DumpMC


if ISF_Flags.RunValgrind() :
    # code profiling with valgrind
    from Valkyrie.JobOptCfg import ValgrindSvc
    valgrindSvc = ValgrindSvc ( OutputLevel = DEBUG )
    valgrindSvc.ProfiledAlgs +=  SimKernel
    #valgrindSvc.IgnoreFirstNEvents
    ServiceMgr += valgrindSvc

# useful for debugging:
print topSequence


#########Back to MyCustomSkeleton.py##############


## check to see if  pileup emulation is being used, if so do post-ISF-config
## actions to enable simulation of pileup collection
if 'AthSequencer/EvgenGenSeq' in topSequence.getSequence():
    fast_chain_log.info("Pileup emulation enabled - setup GenEventStackFiller")
    #include("FastChainPileup/FastPileupSimConfig.py")

    ################FastPileupSimConfig.py#########################
    # configure simulation to read in pileup events with GenEventStackFiller
    # Richard Hawkings 23/4/15
    InputPileupCollection='GEN_EVENT_PU'
    OutputPileupTruthCollection='TruthEvent_PU'
    # ToolSvc.ISF_StackFiller.OutputLevel=DEBUG


    ###############Back to MyCustomSkeleton######################

## Add AMITag MetaData to TagInfoMgr
if hasattr(runArgs, 'AMITag'):
    if runArgs.AMITag != "NONE":
        svcMgr.TagInfoMgr.ExtraTagValuePairs += ["AMITag", runArgs.AMITag]

from ISF_Example.ISF_Metadata import patch_mc_channel_numberMetadata
patch_mc_channel_numberMetadata()

### Changing to post-sim include/exec
## Post-include
if hasattr(runArgs, "postSimInclude"):
    for fragment in runArgs.postSimInclude:
        include(fragment)

if hasattr(runArgs, "outputEVNT_TRFile"):
    if hasattr(runArgs,"trackRecordType") and runArgs.trackRecordType=="stopped":
        include('SimulationJobOptions/postInclude.StoppedParticleWrite.py')

## Post-exec
if hasattr(runArgs, "postSimExec"):
    fast_chain_log.info("transform post-sim exec")
    for cmd in runArgs.postSimExec:
        fast_chain_log.info(cmd)
        exec(cmd)

### End of Sim


### Start of Digi

#include("SimuJobTransforms/CommonSkeletonJobOptions.py")
################CommonSkeletonJobOptions.py again############################

######################################################################
#                                                                    #
# Place holder for numerous common job options of skeleton.XXX files #
# Creation: David Cote (September 2009)                              #
#                                                                    #
######################################################################
#from AthenaCommon.BFieldFlags import jobproperties ##Not sure if this is appropriate for G4 sim

## autoConfiguration keywords triggering pre-defined functions
## if hasattr(runArgs,"autoConfiguration"):
##     for key in runArgs.autoConfiguration:
##         rec.AutoConfiguration.append(key)

#################Back to MyCustomSkeleton##################################

#==============================================================
# Job Configuration parameters:
#==============================================================
### Changing pre-exec to pre-digi exec
## Pre-exec
if hasattr(runArgs,"preDigiExec"):
    fast_chain_log.info("transform pre-digi exec")
    for cmd in runArgs.preDigiExec:
        fast_chain_log.info(cmd)
        exec(cmd)

## Pre-include
if hasattr(runArgs,"preDigiInclude"):
    for fragment in runArgs.preDigiInclude:
        include(fragment)

#--------------------------------------------------------------
# Go for it
#--------------------------------------------------------------
print "lvl1: -14... " + str(DetFlags.digitize.LVL1_on())



#include ("Digitization/Digitization.py")

########## Digitization.py ##########################




###############################################################
#
# Job options file to run:
#        Digitization
#        LVL1 Simulation
#        ByteStream conversion
#
# Author: Sven Vahsen and John Chapman
#==============================================================

#--------------------------------------------------------------
# Get Digitization Flags (This sets Global and Det Flags)
#--------------------------------------------------------------

#-------------------------------------------
# Print Job Configuration
#-------------------------------------------
DetFlags.Print()
fast_chain_log.info("Global jobProperties values:")
globalflags.print_JobProperties()
fast_chain_log.info("Digitization jobProperties values:")
digitizationFlags.print_JobProperties()

#--------------------------------------------------------------
# Configure the job using jobproperties
#--------------------------------------------------------------

#include("Digitization/ConfigDigitization.py")

####### Digitization/ConfigDigitization.py

#Pool input
if not hasattr(ServiceMgr, 'EventSelector'):
    import AthenaPoolCnvSvc.ReadAthenaPool
if hasattr(ServiceMgr, 'PoolSvc'):
    ServiceMgr.PoolSvc.MaxFilesOpen = 0 # Never close Input Files
#Settings the following attributes reduces the job size slightly
#ServiceMgr.AthenaPoolCnvSvc.PoolAttributes += [  "TREE_BRANCH_OFFSETTAB_LEN ='100'" ]
#ServiceMgr.AthenaPoolCnvSvc.PoolAttributes += [ "DEFAULT_BUFFERSIZE = '2048'" ]

#--------------------------------------------------------------
# GeoModel
#--------------------------------------------------------------
from AtlasGeoModel import SetGeometryVersion
from AtlasGeoModel import GeoModelInit
from AtlasGeoModel import SetupRecoGeometry

#--------------------------------------------------------------
# Magnetic field service
#--------------------------------------------------------------
import MagFieldServices.SetupField

#--------------------------------------------------------------
# Configure Run and Lumi Block and Pile-Up Lumi overriding
# (if required)
#--------------------------------------------------------------
if digitizationFlags.RunAndLumiOverrideList.statusOn:
    if not(DetFlags.pileup.any_on()):
        AthError( "This job will try to override pile-up luminosity configuration, but no pile-up will be set up!" )
    include("Digitization/LumiBlockOverrides.py")
    if digitizationFlags.dataRunNumber.statusOn:
        fast_chain_log.warning('digitizationFlags.RunAndLumiOverrideList has been set! digitizationFlags.dataRunNumber (set to %s) will be ignored. ', digitizationFlags.dataRunNumber.get_Value() )
else:
    include("Digitization/RunNumberOverride.py")

#-----------------------------------------------------------
# Check Beam and Digitization jobproperties are synchronised
#-----------------------------------------------------------
from Digitization.DigiConfigCheckers import syncBeamAndDigitizationJobProperties
syncBeamAndDigitizationJobProperties()

digitizationFlags.lockMostFlags()
#--------------------------------------------------------------
# More Pileup configuration
#--------------------------------------------------------------
if DetFlags.pileup.any_on() or digitizationFlags.doXingByXingPileUp():
    # protection for SteppingCache usage - currently incompatible with PileUpTools
    if digitizationFlags.SignalPatternForSteppingCache.statusOn and digitizationFlags.doXingByXingPileUp():
        raise RuntimeError("SteppingCache is incompatible with PileUpTools. Please switch off either digitizationFlags.SignalPatternForSteppingCache or digitizationFlags.doXingByXingPileUp.")
    include( "Digitization/ConfigPileUpEventLoopMgr.py" )
if DetFlags.pileup.any_on():
    fast_chain_log.info("PILEUP CONFIGURATION:")
    fast_chain_log.info(" -----> Luminosity = %s cm^-2 s^-1", jobproperties.Beam.estimatedLuminosity())
    fast_chain_log.info(" -----> Bunch Spacing = %s ns", digitizationFlags.bunchSpacing.get_Value())

# in any case we need the PileUpMergeSvc for the digitize algos
if not hasattr(ServiceMgr, 'PileUpMergeSvc'):
    ServiceMgr += CfgGetter.getService("PileUpMergeSvc")


#--------------------------------------------------------------
# Subdetector-specific configuration
#--------------------------------------------------------------
#include( "Digitization/DetectorDigitization.py" )
#########Digitization/DetectorDigitization.py

##############################################################
#
# Job options file to configure:
#        Digitization
#        LVL1 Simulation
#        ByteStream conversion
#
# Author: Davide Costanzo
# Contacts:
#      Inner Detector:   Davide Costanzo
#      LArCalorimeter:   Guillaume Unal
#      TileCalorimter:   Sasha Solodkov
#      MuonSpectrometer: Daniela Rebuzzi, Ketevi Assamagam
#      LVL1 Simulation:  Tadashi Maeno
#      ByteStream:       Hong Ma
#
#==============================================================
#

try:
    from RecAlgs.RecAlgsConf import TimingAlg
    topSequence+=TimingAlg("DigiTimerBegin", TimingObjOutputName = "HITStoRDO_timings")
except:
    fast_chain_log.warning('Could not add TimingAlg, no timing info will be written out.')


# Set up PileupMergeSvc used by subdetectors
#from AthenaCommon.DetFlags import DetFlags

# Set up ComTimeRec for cosmics digitization
if jobproperties.Beam.beamType == "cosmics" :
    from CommissionRec.CommissionRecConf import ComTimeRec
    comTimeRec = ComTimeRec("ComTimeRec")
    topSequence += comTimeRec

topSequence += CfgGetter.getAlgorithm(digitizationFlags.digiSteeringConf.get_Value(), tryDefaultConfigurable=True)
if 'doFastPixelDigi' in digitizationFlags.experimentalDigi() or 'doFastSCT_Digi' in digitizationFlags.experimentalDigi() or 'doFastTRT_Digi' in digitizationFlags.experimentalDigi():
    print "WARNING  Setting doFastPixelDigi ,doFastSCT_Digi or doFastTRT_Digi in digitizationFlags.experimentalDigi no longer overrides digitizationFlags.digiSteeringConf."
elif 'doSplitDigi' in digitizationFlags.experimentalDigi():
    print "WARNING  Setting doSplitDigi in digitizationFlags.experimentalDigi no longer overrides digitizationFlags.digiSteeringConf. Use --digiSteeringConf 'Split' on the command-line instead."


# MC Truth info
#if DetFlags.Truth_on():
#    include( "Digitization/TruthDigitization.py" )

# Forward Detectors
#if DetFlags.Forward_on():
#    include( "Digitization/FwdDetDigitization.py" )

# Inner Detector
if DetFlags.ID_on():
    include( "Digitization/InDetDigitization.py" )

# Calorimeters
if DetFlags.Calo_on():
    include( "Digitization/CaloDigitization.py" )

# Muon Digitization
if DetFlags.Muon_on():
    include( "Digitization/MuonDigitization.py" )

# LVL1 trigger simulation
#if DetFlags.digitize.LVL1_on():
include( "Digitization/LVL1Digitization.py" )





########## back to Digitization/ConfigDigitization.py

#--------------------------------------------------------------
# Random Number Engine and Seeds
#--------------------------------------------------------------
if not athenaCommonFlags.PoolEvgenInput.statusOn:
    if not simFlags.RandomSeedList.checkForExistingSeed('SINGLE'):
        simFlags.RandomSeedList.addSeed( "SINGLE", 43657613, 78935670 )
if not simFlags.RandomSeedList.checkForExistingSeed('VERTEX'):
  simFlags.RandomSeedList.addSeed( 'VERTEX', 9123448, 829143 )
simFlags.RandomSeedList.addtoService()
simFlags.RandomSeedList.printSeeds()

# attach digi and pileup seeds to random number service configurable and print them out
from AthenaCommon.ConfigurableDb import getConfigurable
ServiceMgr += getConfigurable(digitizationFlags.rndmSvc.get_Value())()
digitizationFlags.rndmSeedList.addtoService()
digitizationFlags.rndmSeedList.printSeeds()
rndmSvc = getConfigurable(digitizationFlags.rndmSvc.get_Value())()
rndmSvc.OutputLevel = WARNING
if digitizationFlags.readSeedsFromFile.get_Value():
    rndmSvc.Seeds=[]
    rndmSvc.ReadFromFile=True
    rndmSvc.FileToRead=digitizationFlags.rndmSeedInputFile.get_Value()
    fast_chain_log.info("Random seeds for Digitization will be read from the file %s",digitizationFlags.rndmSeedInputFile.get_Value())

# write out a summary of the time spent
from AthenaCommon.AppMgr import theAuditorSvc
from GaudiAud.GaudiAudConf import ChronoAuditor, MemStatAuditor
if not 'ChronoAuditor/ChronoAuditor' in theAuditorSvc.Auditors:
    theAuditorSvc += ChronoAuditor()
if not 'MemStatAuditor/MemStatAuditor' in theAuditorSvc.Auditors:
    theAuditorSvc += MemStatAuditor()
# --- load AuditorSvc
# --- write out a short message upon entering or leaving each algorithm
#from AthenaCommon.ConfigurableDb import getConfigurable
# ServiceMgr.AuditorSvc += getConfigurable("NameAuditor")()
#
theApp.AuditAlgorithms = True
theApp.AuditServices   = True
#
# --- Display detailed size and timing statistics for writing and reading
ServiceMgr.AthenaPoolCnvSvc.UseDetailChronoStat = True

# LSFTimeLimi. Temporary disable
# include( "LSFTimeKeeper/LSFTimeKeeperOptions.py" )

# LSFTimeKeeperSvc = Service( "LSFTimeKeeperSvc" )
# LSFTimeKeeperSvc.OutputLevel=2; # remaining CPU at each event

#### BACK TO Digitization.py ###########

#

#include("Digitization/ConfigDigitization.py")

fast_chain_log.info("Digitization Configured Successfully.")

#--------------------------------------------------------------
# jobproperties should not be changed after this point
# except in the special case of overriding the run number
#--------------------------------------------------------------
if digitizationFlags.simRunNumber.statusOn or not digitizationFlags.dataRunNumber.statusOn :
    digitizationFlags.lock_JobProperties()

#--------------------------------------------------------------
# Write Digitization MetaData
#--------------------------------------------------------------
#--------------------------------------------------------------
# Setup the ISF Output
#--------------------------------------------------------------
from ISF_Example.ISF_Output import ISF_HITSStream
from FullChainTransforms.FastChainWriteMetadata import createSimulationParametersMetadata, writeDigitizationMetadata
createSimulationParametersMetadata()
writeDigitizationMetadata()

#--------------------------------------------------------------
# Pool Output (Change this to use a different file)
#--------------------------------------------------------------
if DetFlags.writeRDOPool.any_on():
    from AthenaPoolCnvSvc.WriteAthenaPool import AthenaPoolOutputStream
    streamRDO = AthenaPoolOutputStream("StreamRDO", athenaCommonFlags.PoolRDOOutput.get_Value(), asAlg=True)
    streamRDO.ForceRead = True
    from Digitization.DigiOutput import getStreamRDO_ItemList
    streamRDO.ItemList = getStreamRDO_ItemList(fast_chain_log)
    streamRDO.AcceptAlgs += [ digitizationFlags.digiSteeringConf.get_Value() ]
    streamRDO.OutputFile = athenaCommonFlags.PoolRDOOutput()
    ServiceMgr.AthenaPoolCnvSvc.PoolAttributes += [ "DatabaseName = '" + athenaCommonFlags.PoolRDOOutput() + "'; COMPRESSION_ALGORITHM = '2'" ]
    ServiceMgr.AthenaPoolCnvSvc.PoolAttributes += [ "DatabaseName = '" + athenaCommonFlags.PoolRDOOutput() + "'; COMPRESSION_LEVEL = '1'" ]
    ServiceMgr.AthenaPoolCnvSvc.PoolAttributes += [ "DatabaseName = '" + athenaCommonFlags.PoolRDOOutput() + "'; ContainerName = 'TTree=CollectionTree'; TREE_AUTO_FLUSH = '1'" ]
    # Switch on splitting for the 4 largest container (default off)
    ServiceMgr.AthenaPoolCnvSvc.PoolAttributes += [ "DatabaseName = '" + athenaCommonFlags.PoolRDOOutput() + "'; ContainerName = 'TTree=CollectionTree(InDetSimDataCollection_p1/PixelSDO_Map)'; CONTAINER_SPLITLEVEL = '99'" ]
    ServiceMgr.AthenaPoolCnvSvc.PoolAttributes += [ "DatabaseName = '" + athenaCommonFlags.PoolRDOOutput() + "'; ContainerName = 'TTree=CollectionTree(InDetSimDataCollection_p1/SCT_SDO_Map)'; CONTAINER_SPLITLEVEL = '99'" ]
    ServiceMgr.AthenaPoolCnvSvc.PoolAttributes += [ "DatabaseName = '" + athenaCommonFlags.PoolRDOOutput() + "'; ContainerName = 'TTree=CollectionTree(InDetSimDataCollection_p1/TRT_SDO_Map)'; CONTAINER_SPLITLEVEL = '99'" ]
    ServiceMgr.AthenaPoolCnvSvc.PoolAttributes += [ "DatabaseName = '" + athenaCommonFlags.PoolRDOOutput() + "'; ContainerName = 'TTree=CollectionTree(LArRawChannelContainer_p4/LArRawChannels)'; CONTAINER_SPLITLEVEL = '99'" ]
#--------------------------------------------------------------
# Set output level threshold (2=DEBUG, 3=INFO, 4=WARNING, 5=ERROR, 6=FATAL )
#--------------------------------------------------------------
ServiceMgr.MessageSvc.OutputLevel      = 3

#--------------------------------------------------------------
# Event related parameters
#--------------------------------------------------------------
# Number of events to be processed
theApp.EvtMax = athenaCommonFlags.EvtMax()
# Number of input events to be skipped
ServiceMgr.EventSelector.SkipEvents = athenaCommonFlags.SkipEvents()


######## Back to MyCustomSkeleton#####################

## Post-include
if hasattr(runArgs,"postInclude"):
    for fragment in runArgs.postInclude:
        include(fragment)

## Post-exec
if hasattr(runArgs,"postExec"):
    fast_chain_log.info("transform post-exec")
    for cmd in runArgs.postExec:
        fast_chain_log.info(cmd)
        exec(cmd)

### End of Digi

#from AthenaCommon.ConfigurationShelve import saveToAscii
#saveToAscii('config.txt')
