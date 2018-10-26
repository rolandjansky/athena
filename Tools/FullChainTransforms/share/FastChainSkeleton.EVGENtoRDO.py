# skeleton.EVGENtoRDO.py
# skeleton file for running simulation+digi in one job for FastChain
# currently using full simulation and digi, will swap in fast components later
# Richard Hawkings, Dec 2014, based on work by Robert Harrington
# started by merging simulation and digitisaton skeletons, then modifying
# removed cosmics, beam gas/halo and pileup configuration
# trigger will not be run










### Start of Sim

## Include common skeleton
#include("SimuJobTransforms/skeleton.EVGENtoHIT.py")
###########################Skeleton.EVGENtoHIT.py##########################

#include("SimuJobTransforms/CommonSkeletonJobOptions.py")

#####################CommonSkeletonJobOptions.py##########################


######################################################################
#                                                                    #
# Place holder for numerous common job options of skeleton.XXX files #
# Creation: David Cote (September 2009)                              #
#                                                                    #
######################################################################
from AthenaCommon.GlobalFlags import globalflags
from AthenaCommon.BeamFlags import jobproperties
from AthenaCommon.AthenaCommonFlags import athenaCommonFlags
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
if hasattr(runArgs,"beamType"):
    if runArgs.beamType != 'NONE':
        # Setting beamType='cosmics' keeps cavern in world volume for g4sim also with non-commissioning geometries
        jobproperties.Beam.beamType.set_Value_and_Lock( runArgs.beamType )
## if hasattr(runArgs,"AMITag"): rec.AMITag=runArgs.AMITag
## if hasattr(runArgs,"userExec"): rec.UserExecs=runArgs.userExec
## if hasattr(runArgs,"RunNumber"): rec.RunNumber=runArgs.RunNumber
## if hasattr(runArgs,"projectName"): rec.projectName=runArgs.projectName
## if hasattr(runArgs,"trigStream"): rec.triggerStream=runArgs.trigStream
## if hasattr(runArgs,"triggerConfig"):
##     from TriggerJobOpts.TriggerFlags import TriggerFlags as tf
##     tf.triggerConfig=runArgs.triggerConfig

# Avoid command line preInclude for event service
if hasattr(runArgs, "eventService") and runArgs.eventService:
    include('AthenaMP/AthenaMP_EventService.py')

## autoConfiguration keywords triggering pre-defined functions
## if hasattr(runArgs,"autoConfiguration"):
##     for key in runArgs.autoConfiguration:
##         rec.AutoConfiguration.append(key)

from PerfMonComps.PerfMonFlags import jobproperties as pmon_properties
pmon_properties.PerfMonFlags.doMonitoring=True
pmon_properties.PerfMonFlags.doSemiDetailedMonitoring=True


# Conditions sequence for Athena MT
from AthenaCommon.AlgSequence import AthSequencer
condSeq = AthSequencer("AthCondSeq")
if not hasattr(condSeq, "BeamSpotCondAlg"):
   from BeamSpotConditions.BeamSpotConditionsConf import BeamSpotCondAlg
   condSeq += BeamSpotCondAlg( "BeamSpotCondAlg" )


#####################Back to Skeleton.EVGENtoHIT.py######################
if hasattr(runArgs, "jobNumber"):
    if runArgs.jobNumber < 1:
        raise ValueError('jobNumber must be a postive integer. %s lies outside this range', str(runArgs.jobNumber))


from G4AtlasApps.SimFlags import simFlags
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
if hasattr(runArgs, 'useISF') and not runArgs.useISF:
    raise RuntimeError("Unsupported configuration! If you want to run with useISF=False, please use AtlasG4_tf.py!")

## Get the logger
from AthenaCommon.Logging import *
fast_chain_log = logging.getLogger('ISF')
fast_chain_log.info('****************** STARTING ISF ******************')

### Force trigger to be off
from RecExConfig.RecFlags import rec
rec.doTrigger.set_Value_and_Lock(False)


## Simulation flags need to be imported first
from G4AtlasApps.SimFlags import simFlags
simFlags.load_atlas_flags()
simFlags.ISFRun=True
from ISF_Config.ISF_jobProperties import ISF_Flags

## Set simulation geometry tag
if hasattr(runArgs, 'geometryVersion'):
    simFlags.SimLayout.set_Value_and_Lock(runArgs.geometryVersion)
    globalflags.DetDescrVersion = simFlags.SimLayout.get_Value()
    fast_chain_log.debug('SimLayout set to %s' % simFlags.SimLayout)
else:
    raise RuntimeError("No geometryVersion provided.")

## AthenaCommon flags
from AthenaCommon.AthenaCommonFlags import athenaCommonFlags
# Jobs should stop if an include fails.
if hasattr(runArgs, "IgnoreConfigError"):
    athenaCommonFlags.AllowIgnoreConfigError = runArgs.IgnoreConfigError
else:
    athenaCommonFlags.AllowIgnoreConfigError = False

athenaCommonFlags.DoFullChain=True

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

# Avoid command line preInclude for cavern background
if jobproperties.Beam.beamType.get_Value() != 'cosmics':
    # If it was already there, then we have a stopped particle file
    if hasattr(runArgs, "inputEVNT_TRFile") and\
        not hasattr(topSeq,'TrackRecordGenerator'):
        include('SimulationJobOptions/preInclude.G4ReadCavern.py')
    # If there's a stopped particle file, don't do all the cavern stuff
    if hasattr(runArgs, "outputEVNT_TRFile") and\
        not (hasattr(simFlags,'StoppedParticleFile') and simFlags.StoppedParticleFile.statusOn and simFlags.StoppedParticleFile.get_Value()!=''):
        include('SimulationJobOptions/preInclude.G4WriteCavern.py')

# Avoid command line preInclude for event service
if hasattr(runArgs, "eventService") and runArgs.eventService:
    include('AthenaMP/AthenaMP_EventService.py')

from ISF_Config.ISF_jobProperties import ISF_Flags
if jobproperties.Beam.beamType.get_Value() == 'cosmics':
    ISF_Flags.Simulator.set_Value_and_Lock('CosmicsG4')
elif hasattr(runArgs, 'simulator'):
    ISF_Flags.Simulator.set_Value_and_Lock(runArgs.simulator)
else:
    ISF_Flags.Simulator.set_Value_and_Lock('MC12G4')

# temporary fix to ensure TRT will record hits if using FATRAS
# this should eventually be removed when it is configured properly in ISF
if hasattr(runArgs, 'simulator') and runArgs.simulator.find('ATLFASTIIF')>=0:
    from TrkDetDescrSvc.TrkDetDescrJobProperties import TrkDetFlags
    TrkDetFlags.TRT_BuildStrawLayers=True
    fast_chain_log.info('Enabled TRT_BuildStrawLayers to get hits in ATLFASTIIF')

try:
    from ISF_Config import FlagSetters
    FlagSetters.configureFlagsBase()
## Check for any simulator-specific configuration
    configureFlags = getattr(FlagSetters, ISF_Flags.Simulator.configFlagsMethodName(), None)
    if configureFlags is not None:
        configureFlags()
    possibleSubDetectors=['pixel','SCT','TRT','BCM','Lucid','ZDC','ALFA','AFP','FwdRegion','LAr','HGTD','Tile','MDT','CSC','TGC','RPC','Micromegas','sTGC','Truth']
    for subdet in possibleSubDetectors:
        simattr = "simulate."+subdet+"_on"
        simcheck = getattr(DetFlags, simattr, None)
        if simcheck is not None and simcheck():
            attrname = subdet+"_setOn"
            checkfn = getattr(DetFlags, attrname, None)
            if checkfn is not None:
                checkfn()
except:
    ## Select detectors
    if 'DetFlags' not in dir():
        from AthenaCommon.DetFlags import DetFlags
        ## If you configure one det flag, you're responsible for configuring them all!
        DetFlags.all_setOn()
    DetFlags.LVL1_setOff() # LVL1 is not part of G4 sim
    DetFlags.Truth_setOn()

DetFlags.Forward_setOff() # Forward dets are off by default
DetFlags.Micromegas_setOff()
DetFlags.sTGC_setOff()
DetFlags.FTK_setOff()
checkHGTDOff = getattr(DetFlags, 'HGTD_setOff', None)
if checkHGTDOff is not None:
    checkHGTDOff() #Default for now

from AthenaCommon.DetFlags import DetFlags
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
elif hasattr(runArgs,'jobNumber'):
    if runArgs.jobNumber>=0:
        fast_chain_log.info( 'Using job number '+str(runArgs.jobNumber)+' to derive run number.' )
        simFlags.RunNumber = simFlags.RunDict.GetRunNumber( runArgs.jobNumber )
        fast_chain_log.info( 'Set run number based on dictionary to '+str(simFlags.RunNumber) )

## removed code block for handling cosmics track record

# get top sequence
from AthenaCommon.AlgSequence import AlgSequence
topSeq = AlgSequence()

## Set Overall per-Algorithm time-limit on the AlgSequence
topSeq.TimeOut = 43200 * Units.s

try:
    from RecAlgs.RecAlgsConf import TimingAlg
    topSeq+=TimingAlg("SimTimerBegin", TimingObjOutputName = "EVNTtoHITS_timings")
except:
    fast_chain_log.warning('Could not add TimingAlg, no timing info will be written out.')

from ISF_Config.ISF_jobProperties import ISF_Flags
if hasattr(runArgs, 'simulator'):
    ISF_Flags.Simulator = runArgs.simulator
else:
    ISF_Flags.Simulator = 'MC12G4'

#### *********** import ISF_Example code here **************** ####

#include("ISF_Config/ISF_ConfigJobInclude.py")




########## ISF_ConfigJobInclude.py #################




"""
Common configurations for ISF
KG Tan, 17/06/2012
"""

include.block('ISF_Config/ISF_ConfigJobInclude.py')
from AthenaCommon.CfgGetter import getPrivateTool,getPrivateToolClone,getPublicTool,getPublicToolClone,\
        getService,getServiceClone,getAlgorithm,getAlgorithmClone

#--------------------------------------------------------------
# Set to monte carlo
#--------------------------------------------------------------
import AthenaCommon.AtlasUnixStandardJob
from AthenaCommon import AthenaCommonFlags
from AthenaCommon.AthenaCommonFlags import athenaCommonFlags
from AthenaCommon.AppMgr import theApp
from AthenaCommon.AppMgr import ServiceMgr
from AthenaCommon.AlgSequence import AlgSequence
topSequence = AlgSequence()

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
import AthenaCommon.SystemOfUnits as Units

from ISF_Config.ISF_jobProperties import ISF_Flags # IMPORTANT: Flags must be finalised before these functons are called

from AthenaCommon.GlobalFlags import globalflags
# --- set globalflags
globalflags.DataSource.set_Value_and_Lock('geant4')
globalflags.InputFormat.set_Value_and_Lock('pool')
globalflags.DetGeo.set_Value_and_Lock('atlas')
globalflags.Luminosity.set_Off()

# --- set SimLayout (synchronised to globalflags)
from G4AtlasApps.SimFlags import simFlags
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
# Job setup
#--------------------------------------------------------------
theApp.EvtMax = athenaCommonFlags.EvtMax()

# all det description
include('ISF_Config/AllDet_detDescr.py')
DetFlags.Print()

if len(globalflags.ConditionsTag()):
    from IOVDbSvc.CondDB import conddb
    conddb.setGlobalTag(globalflags.ConditionsTag())

# Temporary work-around - see ATLASSIM-2351
if ISF_Flags.UsingGeant4():
    #include("G4AtlasApps/G4Atlas.flat.configuration.py") #HACK

    ####################G4Atlas.flat.configuration.py#################


    ## _PyG4AtlasComp.__init__
    ## If the random number service hasn't been set up already, do it now.
    from G4AtlasApps.SimFlags import simFlags
    simFlags.RandomSeedList.useDefaultSeeds()

    ## AtlasSimSkeleton._do_jobproperties
    ## Import extra flags if it hasn't already been done
    from G4AtlasApps.SimFlags import simFlags
    if "atlas_flags" not in simFlags.extra_flags:
        simFlags.load_atlas_flags()
    from AthenaCommon.BeamFlags import jobproperties
    if jobproperties.Beam.beamType() == "cosmics" and "cosmics_flags" not in simFlags.extra_flags:
        simFlags.load_cosmics_flags()

    from AthenaCommon.DetFlags import DetFlags
    ## Tidy up DBM DetFlags: temporary measure
    DetFlags.DBM_setOff()

    ## Tidy up NSW DetFlags: temporary measure
    DetFlags.sTGC_setOff()
    DetFlags.Micromegas_setOff()
    if hasattr(simFlags, 'SimulateNewSmallWheel'):
        if simFlags.SimulateNewSmallWheel():
            DetFlags.sTGC_setOn()
            DetFlags.Micromegas_setOn()

    ## Switch off tasks
#    DetFlags.pileup.all_setOff()
#    DetFlags.simulateLVL1.all_setOff()
#    DetFlags.digitize.all_setOff()
    if not simFlags.IsEventOverlayInputSim():
        DetFlags.overlay.all_setOff()
#    DetFlags.readRDOPool.all_setOff()
#    DetFlags.makeRIO.all_setOff()
#    DetFlags.writeBS.all_setOff()
#    DetFlags.readRDOBS.all_setOff()
#    DetFlags.readRIOBS.all_setOff()
#    DetFlags.readRIOPool.all_setOff()
#    DetFlags.writeRIOPool.all_setOff()
#    DetFlags.writeRDOPool.all_setOff()

    ## Global flags needed by externals
    from AthenaCommon.GlobalFlags import globalflags
    globalflags.DataSource = 'geant4'
    if jobproperties.Beam.beamType() == 'cosmics':
        globalflags.DetGeo = 'commis'
    else:
        globalflags.DetGeo = 'atlas'

    ## At this point we can set the global job properties flag
    globalflags.DetDescrVersion = simFlags.SimLayout.get_Value()

    # Switch off GeoModel Release in the case of parameterization
    if simFlags.LArParameterization.get_Value()>0 and simFlags.ReleaseGeoModel():
        simFlags.ReleaseGeoModel = False

    ## Translate conditions tag into IOVDbSvc global tag: must be done before job properties are locked!!!
    from AthenaCommon.AppMgr import ServiceMgr
    from IOVDbSvc.IOVDbSvcConf import IOVDbSvc
    ServiceMgr += IOVDbSvc()
    if not hasattr(globalflags, "ConditionsTag") or not globalflags.ConditionsTag.get_Value():
        raise SystemExit("AtlasSimSkeleton._do_jobproperties :: Global ConditionsTag not set")
    if not hasattr(ServiceMgr.IOVDbSvc, 'GlobalTag') or not ServiceMgr.IOVDbSvc.GlobalTag:
        ServiceMgr.IOVDbSvc.GlobalTag = globalflags.ConditionsTag.get_Value()

    ## Enable floating point exception handling
    ## FIXME! This seems to cause the jobs to crash in the FpeControlSvc, so commenting this out for now...
    #from AthenaCommon.AthenaCommonFlags import athenaCommonFlags
    #athenaCommonFlags.RuntimeStrictness = 'abort'

    if not simFlags.ISFRun:
        from G4AtlasApps.G4Atlas_Metadata import checkForSpecialConfigurationMetadata
        checkForSpecialConfigurationMetadata()

    from AthenaCommon.JobProperties import jobproperties
    DetFlags.Print()
    jobproperties.print_JobProperties('tree&value')

    # Lock the job properties if not running ISF.
    if not simFlags.ISFRun:
        jobproperties.lock_JobProperties()

    ## AtlasSimSkeleton._do_external
    from AthenaCommon.AppMgr import ToolSvc,ServiceMgr
    from Geo2G4.Geo2G4Conf import Geo2G4Svc
    geo2G4Svc = Geo2G4Svc()
    theApp.CreateSvc += ["Geo2G4Svc"]
    ServiceMgr += geo2G4Svc
    ## Enable top transforms for the ATLAS geometry
    geo2G4Svc.GetTopTransform = True

    ## GeoModel stuff
    ## TODO: Tidy imports etc.
    from GeoModelSvc.GeoModelSvcConf import GeoModelSvc
    from AthenaCommon.GlobalFlags import jobproperties
    from AtlasGeoModel import SetGeometryVersion

    ## Forward Region Twiss files - needed before geometry setup!
    from G4AtlasApps.SimFlags import simFlags
    if simFlags.ForwardDetectors.statusOn:
        if DetFlags.geometry.FwdRegion_on():
            from AthenaCommon.CfgGetter import getPublicTool
            from AthenaCommon.AppMgr import ToolSvc
            ToolSvc += getPublicTool("ForwardRegionProperties")

    from AtlasGeoModel import GeoModelInit
    from AtlasGeoModel import SimEnvelopes
    from GeoModelSvc.GeoModelSvcConf import GeoModelSvc
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
        from AthenaCommon import CfgGetter
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

    ## AtlasSimSkeleton._do_metadata
    from G4AtlasApps.SimFlags import simFlags
    if not simFlags.ISFRun:
        from G4AtlasApps.G4Atlas_Metadata import createSimulationParametersMetadata
        createSimulationParametersMetadata()
        from AthenaCommon.AthenaCommonFlags import athenaCommonFlags
        if not athenaCommonFlags.PoolHitsOutput.statusOn:
            print 'AtlasSimSkeleton._do_metadata :: no output HITS file, so no metadata writing required.'
        else:
            from AthenaServices.AthenaServicesConf import AthenaOutputStream
            stream1_SimMetaData = AthenaOutputStream("StreamHITS_SimMetaData")
            stream1_SimMetaData.ItemList += [ "IOVMetaDataContainer#*" ]

    if not simFlags.ISFRun:
        def hits_persistency():
            """ HITS POOL file persistency
            """
            from G4AtlasApps.SimFlags import simFlags
            from AthenaCommon.DetFlags import DetFlags
            from AthenaCommon.AthenaCommonFlags import athenaCommonFlags
            from AthenaPoolCnvSvc.WriteAthenaPool import AthenaPoolOutputStream

            ## Not yet understood, but need to treat StreamHITS as alg in Hive.
            ## Seems to also work fine outside of Hive, but to be extra safe I'm
            ## only changing it in Hive.
            from AthenaCommon.ConcurrencyFlags import jobproperties as concurrencyProps
            if concurrencyProps.ConcurrencyFlags.NumThreads() > 0:
                as_alg = True
            else:
                as_alg = False
            ## NB. Two-arg constructor is needed, since otherwise metadata writing fails!
            stream1 = AthenaPoolOutputStream("StreamHITS", athenaCommonFlags.PoolHitsOutput(), asAlg=as_alg)

            ## Write geometry tag info - move to main method
            #import EventInfoMgt.EventInfoMgtInit

            ## EventInfo & TruthEvent always written by default
            stream1.ForceRead=True
            stream1.ItemList = ["EventInfo#*",
                                "McEventCollection#TruthEvent",
                                "JetCollection#*"]

            ## If we are running quasi-stable particle simulation, include the original event record
            if hasattr(simFlags,'IncludeParentsInG4Event') and simFlags.IncludeParentsInG4Event.statusOn and simFlags.IncludeParentsInG4Event():
                stream1.ItemList += ["McEventCollection#GEN_EVENT"]

            stream1.ItemList += ["xAOD::JetContainer#*",
                                 "xAOD::JetAuxContainer#*"]

            ## Make stream aware of aborted events
            stream1.AcceptAlgs = ["G4AtlasAlg"]

            ## Detectors

            ## Inner Detector
            if DetFlags.ID_on():
                stream1.ItemList += ["SiHitCollection#*",
                                     "TRTUncompressedHitCollection#*",
                                     "TrackRecordCollection#CaloEntryLayer"]
            ## Calo
            if DetFlags.Calo_on():
                stream1.ItemList += ["CaloCalibrationHitContainer#*",
                                     "LArHitContainer#*",
                                     "TileHitVector#*",
                                     #"SimpleScintillatorHitCollection#*",
                                     "TrackRecordCollection#MuonEntryLayer"]
            ## Muon
            if DetFlags.Muon_on():
                stream1.ItemList += ["RPCSimHitCollection#*",
                                     "TGCSimHitCollection#*",
                                     "CSCSimHitCollection#*",
                                     "MDTSimHitCollection#*",
                                     "TrackRecordCollection#MuonExitLayer"]
                if hasattr(simFlags, 'SimulateNewSmallWheel'):
                    if simFlags.SimulateNewSmallWheel():
                        stream1.ItemList += ["GenericMuonSimHitCollection#*"]
            ## Lucid
            if DetFlags.Lucid_on():
                stream1.ItemList += ["LUCID_SimHitCollection#*"]

            ## FwdRegion
            if DetFlags.FwdRegion_on():
                stream1.ItemList += ["SimulationHitCollection#*"]

            ## ZDC
            if DetFlags.ZDC_on():
                stream1.ItemList += ["ZDC_SimPixelHit_Collection#*",
                                     "ZDC_SimStripHit_Collection#*"]
            ## ALFA
            if DetFlags.ALFA_on():
                stream1.ItemList += ["ALFA_HitCollection#*",
                                     "ALFA_ODHitCollection#*"]

            ## AFP
            if DetFlags.AFP_on():
                stream1.ItemList += ["AFP_TDSimHitCollection#*",
                                     "AFP_SIDSimHitCollection#*"]

            ### Ancillary scintillators
            #stream1.ItemList += ["ScintillatorHitCollection#*"]

            ## TimingAlg
            stream1.ItemList +=["RecoTimingObj#EVNTtoHITS_timings"]

            ## Add cosmics and test beam configuration hit persistency if required cf. geom tag
            layout = simFlags.SimLayout.get_Value()
            if "tb" not in layout:
                from AthenaCommon.BeamFlags import jobproperties
                if jobproperties.Beam.beamType() == 'cosmics' or \
                        (hasattr(simFlags, "WriteTR") and simFlags.WriteTR.statusOn) or \
                        (hasattr(simFlags, "ReadTR") and simFlags.ReadTR.statusOn):
                    stream1.ItemList += ["TrackRecordCollection#CosmicRecord", "TrackRecordCollection#CosmicPerigee"]
            else:
                ## CTB-specific
                if layout.startswith("ctb"):
                    if simFlags.LArFarUpstreamMaterial.statusOn and simFlags.LArFarUpstreamMaterial.get_Value():
                        stream1.ItemList.append("TrackRecordCollection#LArFarUpstreamMaterialExitLayer")
                ## Persistency of test-beam layout
                if layout.startswith('ctb') or layout.startswith('tb_Tile2000_'):
                    stream1.ItemList += ["TBElementContainer#*"]


        def evgen_persistency():
            """ EVGEN POOL file persistency
            """
            from G4AtlasApps.SimFlags import simFlags
            from AthenaPoolCnvSvc.WriteAthenaPool import AthenaPoolOutputStream
            ## NB. Two-arg constructor is needed, since otherwise metadata writing fails!
            if hasattr(simFlags, "WriteTR") and simFlags.WriteTR.statusOn:
                stream2 = AthenaPoolOutputStream("StreamEVGEN", simFlags.WriteTR.get_Value())
                stream2.ItemList += ["IOVMetaDataContainer#*",
                                     "EventInfo#*"]
                if simFlags.CavernBG.statusOn and 'Write' in simFlags.CavernBG.get_Value():
                    stream2.ItemList += ["TrackRecordCollection#NeutronBG"]
                else:
                    stream2.ItemList += ["TrackRecordCollection#CosmicRecord"]
                stream2.AcceptAlgs = ["G4AtlasAlg"]
            if hasattr(simFlags,'StoppedParticleFile') and simFlags.StoppedParticleFile.statusOn:
                stream2 = AthenaPoolOutputStream("StreamEVGEN", simFlags.StoppedParticleFile.get_Value())
                stream2.ItemList += ["IOVMetaDataContainer#*",
                                     "EventInfo#*"]
                stream2.ItemList += ["TrackRecordCollection#StoppingPositions"]
                stream2.AcceptAlgs = ["G4AtlasAlg"]


        def will_write_output_files():
            """ Check if any POOL files will be written by this job
            """
            from G4AtlasApps.SimFlags import simFlags
            from AthenaCommon.AthenaCommonFlags import athenaCommonFlags
            if athenaCommonFlags.PoolHitsOutput.statusOn:
                return True
            elif ("tb" not in simFlags.SimLayout.get_Value()):
                if hasattr(simFlags, "WriteTR") and simFlags.WriteTR.statusOn:
                    return True
                elif hasattr(simFlags,'StoppedParticleFile') and simFlags.StoppedParticleFile.statusOn:
                    return True
            return False


        def do_run_number_modifications():
            """ Set the run number in the simulation.  In order of priority, use:
            - The RunNumber flag
            - The input file run number """
            # FIXME This method is called from both _do_persistency and _do_All for AtlasG4 jobs!
            from G4AtlasApps.G4Atlas_Metadata import configureRunNumberOverrides
            configureRunNumberOverrides()


        ## SimSkeleton._do_readevgen
        from G4AtlasApps.SimFlags import simFlags
        from AthenaCommon.AppMgr import ServiceMgr as svcMgr
        from AthenaCommon.AthenaCommonFlags import athenaCommonFlags
        ## ReadTR is only present in simFlags for ATLAS geometries with cosmics switched on
        if (not simFlags.ISFRun) and hasattr(simFlags, "ReadTR") and simFlags.ReadTR.statusOn:
            include("CosmicGenerator/SetCosmicGenerator.py")

        if athenaCommonFlags.PoolEvgenInput.statusOn:
            ## Tell the event selector about the evgen input files and event skipping
            if not hasattr(svcMgr, 'EventSelector'):
                import AthenaPoolCnvSvc.ReadAthenaPool
            svcMgr.EventSelector.InputCollections = athenaCommonFlags.PoolEvgenInput()
            if athenaCommonFlags.SkipEvents.statusOn:
                svcMgr.EventSelector.SkipEvents = athenaCommonFlags.SkipEvents()
            from G4AtlasApps.G4Atlas_Metadata import inputFileValidityCheck
            inputFileValidityCheck()
        else:
            ## No input file so assume that we are running a Generator in the same job
            if not hasattr(svcMgr, 'EventSelector'):
                import AthenaCommon.AtlasUnixGeneratorJob
            # TODO: Check that there is at least one algorithm already in the AlgSequence?
            ## Warn if attempting to skip events in a generator job
            if athenaCommonFlags.SkipEvents.statusOn and athenaCommonFlags.SkipEvents()!=0:
                msg = "SimSkeleton._do_readevgen :: athenaCommonFlags.SkipEvents set in a job without an active "
                msg += "athenaCommonFlags.PoolEvgenInput flag: ignoring event skip request"
                print msg

        ## SimSkeleton._do_persistency
        from G4AtlasApps.SimFlags import simFlags
        from AthenaCommon.AthenaCommonFlags import athenaCommonFlags
        if will_write_output_files():
            ## Write hits in POOL
            ## The following used to be in G4AtlasApps/HitAthenaPoolWriteOptions
            from AthenaCommon.DetFlags import DetFlags
            from AthenaCommon.Configurable import Configurable
            from AthenaPoolCnvSvc.WriteAthenaPool import AthenaPoolOutputStream

            ## Default setting for one output stream
            from AthenaCommon.AppMgr import ServiceMgr as svcMgr
            svcMgr.AthenaPoolCnvSvc.PoolAttributes += ["TREE_BRANCH_OFFSETTAB_LEN = '100'"]
            # Recommendations from Peter vG 16.08.25
            svcMgr.AthenaPoolCnvSvc.PoolAttributes += [ "DatabaseName = '" + athenaCommonFlags.PoolHitsOutput() + "'; ContainerName = 'TTree=CollectionTree'; TREE_AUTO_FLUSH = '1'" ]

            ## Write geometry tag info
            import EventInfoMgt.EventInfoMgtInit

            ## Patch metadata if required
            from G4AtlasApps.G4Atlas_Metadata import patch_mc_channel_numberMetadata
            patch_mc_channel_numberMetadata()

            ## Instantiate StreamHITS
            if athenaCommonFlags.PoolHitsOutput.statusOn:
                hits_persistency()

            ## StreamEVGEN: needed for cosmic simulations and cavern BG
            ## Separate stream of track record (TR) info -- it does not apply to the CTB simulations.
            # TODO: Can this be merged into the cosmics sec above, or do the AthenaPool includes *need* to be in-between?
            if "tb" not in simFlags.SimLayout.get_Value():
                evgen_persistency()

        # Check on run numbers and update them if necessary
        do_run_number_modifications()


    ## _PyG4AtlasComp.initialize
    from AthenaCommon.AthenaCommonFlags import athenaCommonFlags
    if athenaCommonFlags.EvtMax.statusOn:# and theApp.EvtMax == -1:
        theApp.EvtMax = athenaCommonFlags.EvtMax()




    ###################Back to ISF_ConfigJobInclude.py################

#--------------------------------------------------------------
# Setup the ISF Services
#--------------------------------------------------------------
# --- load AuditorSvc
from AthenaCommon.ConfigurableDb import getConfigurable
# --- write out summary of the memory usage
#   | number of events to be skip to detect memory leak
#   | 20 is default. May need to be made larger for complete jobs.
ServiceMgr.AuditorSvc += getConfigurable("ChronoAuditor")()
# --- write out a short message upon entering or leaving each algorithm
# ServiceMgr.AuditorSvc += getConfigurable("NameAuditor")()
#
theApp.AuditAlgorithms = True
theApp.AuditServices   = True
#
# --- Display detailed size and timing statistics for writing and reading
ServiceMgr.AthenaPoolCnvSvc.UseDetailChronoStat = True

#--------------------------------------------------------------
# ISF input
#--------------------------------------------------------------

# Note: automatically adds generator to TopSequence if applicable
from AthenaCommon.BeamFlags import jobproperties
# if an input sting identifier was given, use ISF input definitions
if ISF_Flags.Input()!="NONE":
    getAlgorithm('ISF_Input_' + ISF_Flags.Input())
# cosmics input
elif jobproperties.Beam.beamType.get_Value() == 'cosmics':
    from G4AtlasApps.SimFlags import simFlags
    simFlags.load_cosmics_flags()
    if simFlags.ReadTR.statusOn:
        getAlgorithm('TrackRecordCosmicGenerator')
    else:
        getAlgorithm('EvgenCosmicGenerator')
# non of the above

elif not athenaCommonFlags.PoolEvgenInput.statusOn:
    print "ISF Input Configuration: PoolEvgenInput off, likely running with a particle gun preInclude"
# non of the above -> input via ISF_Flags
else :
    if ISF_Flags.OverrideInputFiles():
        athenaCommonFlags.PoolEvgenInput = ISF_Flags.OverrideInputFiles()
    getAlgorithm('ISF_Input_GenericFiles')

from ISF_Example.ISF_Input import ISF_Input

from AthenaCommon.CfgGetter import getAlgorithm
topSeq += getAlgorithm("BeamEffectsAlg")

#--------------------------------------------------------------
# ISF kernel configuration
#--------------------------------------------------------------

# keep reference to collection merger algorithm to guarantee that
# any subsequent simulator configuration gets a reference to the same
# instance when calling confgetter's getAlgorithm
collection_merger_alg = getAlgorithm('ISF_CollectionMerger')

SimKernel = getAlgorithm(ISF_Flags.Simulator.KernelName())

#--------------------------------------------------------------
# Setup the random number streams
#--------------------------------------------------------------
from G4AtlasApps.SimFlags import SimFlags,simFlags;
if not athenaCommonFlags.PoolEvgenInput.statusOn:
    if not simFlags.RandomSeedList.checkForExistingSeed('SINGLE'):
        simFlags.RandomSeedList.addSeed( "SINGLE", 43657613, 78935670 )
if not simFlags.RandomSeedList.checkForExistingSeed('VERTEX'):
  simFlags.RandomSeedList.addSeed( 'VERTEX', 9123448, 829143 )
simFlags.RandomSeedList.addtoService()
simFlags.RandomSeedList.printSeeds()

#--------------------------------------------------------------
# Setup the ISF Output
#--------------------------------------------------------------
from ISF_Example.ISF_Output import ISF_HITSStream
from ISF_Example.ISF_Metadata import createSimulationParametersMetadata, configureRunNumberOverrides
createSimulationParametersMetadata()
configureRunNumberOverrides()

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

elif ISF_Flags.DoPerfMonStats() :
    # Performance Monitoring (VP1 does not like this)
    # https://twiki.cern.ch/twiki/bin/viewauth/Atlas/PerfMonSD
    from PerfMonComps.PerfMonFlags import jobproperties as pmon_properties
    pmon_properties.PerfMonFlags.doMonitoring=True
    pmon_properties.PerfMonFlags.doSemiDetailedMonitoring=True


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
if 'AthSequencer/EvgenGenSeq' in topSeq.getSequence():
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
        from AthenaCommon.AppMgr import ServiceMgr as svcMgr
        svcMgr.TagInfoMgr.ExtraTagValuePairs += ["AMITag", runArgs.AMITag]

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
from AthenaCommon.GlobalFlags import globalflags
from AthenaCommon.BeamFlags import jobproperties
from AthenaCommon.AthenaCommonFlags import athenaCommonFlags
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
if hasattr(runArgs,"beamType"):
    if runArgs.beamType != 'NONE':
        # Setting beamType='cosmics' keeps cavern in world volume for g4sim also with non-commissioning geometries
        jobproperties.Beam.beamType.set_Value_and_Lock( runArgs.beamType )
## if hasattr(runArgs,"AMITag"): rec.AMITag=runArgs.AMITag
## if hasattr(runArgs,"userExec"): rec.UserExecs=runArgs.userExec
## if hasattr(runArgs,"RunNumber"): rec.RunNumber=runArgs.RunNumber
## if hasattr(runArgs,"projectName"): rec.projectName=runArgs.projectName
## if hasattr(runArgs,"trigStream"): rec.triggerStream=runArgs.trigStream
## if hasattr(runArgs,"triggerConfig"):
##     from TriggerJobOpts.TriggerFlags import TriggerFlags as tf
##     tf.triggerConfig=runArgs.triggerConfig

# Avoid command line preInclude for event service
if hasattr(runArgs, "eventService") and runArgs.eventService:
    include('AthenaMP/AthenaMP_EventService.py')

## autoConfiguration keywords triggering pre-defined functions
## if hasattr(runArgs,"autoConfiguration"):
##     for key in runArgs.autoConfiguration:
##         rec.AutoConfiguration.append(key)

from PerfMonComps.PerfMonFlags import jobproperties as pmon_properties
pmon_properties.PerfMonFlags.doMonitoring=True
pmon_properties.PerfMonFlags.doSemiDetailedMonitoring=True


#################Back to MyCustomSkeleton##################################

if hasattr(runArgs, "jobNumber"):
    if runArgs.jobNumber < 1:
        raise ValueError('jobNumber must be a postive integer. %s lies outside this range', str(runArgs.jobNumber))

from AthenaCommon.GlobalFlags import globalflags
if hasattr(runArgs,"geometryVersion"):
    # strip _VALIDATION
    print "stripping _VALIDATION"
    if runArgs.geometryVersion.endswith("_VALIDATION"):
        pos=runArgs.geometryVersion.find("_VALIDATION")
        globalflags.DetDescrVersion.set_Value_and_Lock( runArgs.geometryVersion[:pos] )
    else:
        globalflags.DetDescrVersion.set_Value_and_Lock( runArgs.geometryVersion )

### Do not invoke another logger
# get the logger
#from AthenaCommon.Logging import logging
digilog = logging.getLogger('Digi_trf')
fast_chain_log.info( '****************** STARTING DIGITIZATION *****************' )


fast_chain_log.info( '**** Transformation run arguments' )
fast_chain_log.info( str(runArgs) )

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
# Override pile-up configuration on the command-line
#--------------------------------------------------------------

from Digitization.DigitizationFlags import digitizationFlags

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

if hasattr(runArgs,"conditionsTag"):
    if(runArgs.conditionsTag!='NONE'):
        digitizationFlags.IOVDbGlobalTag = runArgs.conditionsTag

### Avoid meta data reading
digitizationFlags.overrideMetadata=['ALL']

if hasattr(runArgs,"digiSteeringConf"):
    if not (digitizationFlags.digiSteeringConf.get_Value()==runArgs.digiSteeringConf+"PileUpToolsAlg"):
        digilog.info( "Changing digitizationFlags.digiSteeringConf from %s to %s", digitizationFlags.digiSteeringConf.get_Value(),runArgs.digiSteeringConf)
        digitizationFlags.digiSteeringConf=runArgs.digiSteeringConf+"PileUpToolsAlg"
        PileUpConfigOverride=True

#--------------------------------------------------------------
# Pileup configuration - removed as pileup will be handled on-the-fly
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


#--------------------------------------------------------------
# Go for it
#--------------------------------------------------------------
if hasattr(runArgs,"DataRunNumber"):
    if runArgs.DataRunNumber>0:
        fast_chain_log.info( 'Overriding run number to be: %s ', runArgs.DataRunNumber )
        digitizationFlags.dataRunNumber=runArgs.DataRunNumber

print "lvl1: -14... " + str(DetFlags.digitize.LVL1_on())

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





from AthenaCommon.AlgSequence import AlgSequence
topSeq = AlgSequence()

## Set Overall per-Algorithm time-limit on the AlgSequence
topSeq.TimeOut = 43200 * Units.s

try:
    from RecAlgs.RecAlgsConf import TimingAlg
    topSeq+=TimingAlg("DigiTimerBegin", TimingObjOutputName = "HITStoRDO_timings")
except:
    fast_chain_log.warning('Could not add TimingAlg, no timing info will be written out.')

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
from AthenaCommon.Logging import logging
logDigitization_flags = logging.getLogger( 'Digitization' )

#--------------------------------------------------------------
# Get Digitization Flags (This sets Global and Det Flags)
#--------------------------------------------------------------
from Digitization.DigitizationFlags import digitizationFlags
from AthenaCommon.BeamFlags import jobproperties

#--------------------------------------------------------------
# Set Global flags for this run
#--------------------------------------------------------------
from AthenaCommon.GlobalFlags import globalflags
if jobproperties.Beam.beamType == "cosmics" :
    globalflags.DetGeo = 'commis'
#else:
#    globalflags.DetGeo = 'atlas'
# These should be set anyway, but just to make sure
globalflags.DataSource = 'geant4'
globalflags.InputFormat = 'pool'

#--------------------------------------------------------------
# Set Detector flags for this run
#--------------------------------------------------------------
if 'DetFlags' in dir():
    logDigitization_flags.warning("DetFlags already defined! This means DetFlags should have been fully configured already..")
    DetFlags.Print()
else :
    # include DetFlags
    # by default everything is off
    from AthenaCommon.DetFlags import DetFlags
    DetFlags.ID_setOn()
    DetFlags.Lucid_setOn()
    #DetFlags.ZDC_setOn()
    #DetFlags.ALFA_setOn()
    #DetFlags.AFP_setOn()
    #DetFlags.FwdRegion_setOn()
    DetFlags.Calo_setOn()
    if hasattr(DetFlags, 'HGTD_setOff'):
        DetFlags.HGTD_setOff()
    DetFlags.Muon_setOn()
    DetFlags.Truth_setOn()
    DetFlags.LVL1_setOn()

checkOverlay = getattr(DetFlags, 'overlay', None)
if checkOverlay is not None:
    DetFlags.overlay.all_setOff()
else:
    logDigitization_flags.warning( 'DetFlags.overlay is not supported in this release' )
DetFlags.simulate.all_setOff()
DetFlags.makeRIO.all_setOff()
DetFlags.writeBS.all_setOff()
DetFlags.readRDOBS.all_setOff()
DetFlags.readRIOBS.all_setOff()
DetFlags.readRIOPool.all_setOff()
DetFlags.writeRIOPool.all_setOff()

#-------------------------------------------
# Print Job Configuration
#-------------------------------------------
DetFlags.Print()
logDigitization_flags.info("Global jobProperties values:")
globalflags.print_JobProperties()
logDigitization_flags.info("Digitization jobProperties values:")
digitizationFlags.print_JobProperties()

#--------------------------------------------------------------
# Ensure AthenaCommonFlags.FilesInput is set.
#--------------------------------------------------------------
from AthenaCommon.AthenaCommonFlags import athenaCommonFlags
athenaCommonFlags.FilesInput=athenaCommonFlags.PoolHitsInput.get_Value()

#--------------------------------------------------------------
# Read Simulation MetaData (unless override flag set to True)
#--------------------------------------------------------------
if 'ALL' in digitizationFlags.overrideMetadata.get_Value():
    logDigitization_flags.info("Skipping input file MetaData check.")
else :
    from Digitization.DigitizationReadMetaData import readHITSFileMetadata
    readHITSFileMetadata()

#--------------------------------------------------------------
# Configure the job using jobproperties
#--------------------------------------------------------------
include("Digitization/ConfigDigitization.py")
logDigitization_flags.info("Digitization Configured Successfully.")

#--------------------------------------------------------------
# jobproperties should not be changed after this point
# except in the special case of overriding the run number
#--------------------------------------------------------------
if digitizationFlags.simRunNumber.statusOn or not digitizationFlags.dataRunNumber.statusOn :
    digitizationFlags.lock_JobProperties()

#--------------------------------------------------------------
# Write Digitization MetaData
#--------------------------------------------------------------
from Digitization.DigitizationWriteMetaData import writeDigitizationMetadata
writeDigitizationMetadata()

#--------------------------------------------------------------
# Pool Output (Change this to use a different file)
#--------------------------------------------------------------
if DetFlags.writeRDOPool.any_on():
    from AthenaCommon.AthenaCommonFlags import athenaCommonFlags
    from AthenaPoolCnvSvc.WriteAthenaPool import AthenaPoolOutputStream
    streamRDO = AthenaPoolOutputStream("StreamRDO", athenaCommonFlags.PoolRDOOutput.get_Value(), asAlg=True)
    streamRDO.ForceRead = True
    from Digitization.DigiOutput import getStreamRDO_ItemList
    streamRDO.ItemList = getStreamRDO_ItemList(logDigitization_flags)
    streamRDO.AcceptAlgs += [ digitizationFlags.digiSteeringConf.get_Value() ]
    streamRDO.OutputFile = athenaCommonFlags.PoolRDOOutput()
    if athenaCommonFlags.UseLZMA():
        ServiceMgr.AthenaPoolCnvSvc.PoolAttributes += [ "DatabaseName = '" + athenaCommonFlags.PoolRDOOutput() + "'; COMPRESSION_ALGORITHM = '2'" ]
        ServiceMgr.AthenaPoolCnvSvc.PoolAttributes += [ "DatabaseName = '" + athenaCommonFlags.PoolRDOOutput() + "'; COMPRESSION_LEVEL = '1'" ]
    else:
        ServiceMgr.AthenaPoolCnvSvc.PoolAttributes += [ "DatabaseName = '" + athenaCommonFlags.PoolRDOOutput() + "'; COMPRESSION_ALGORITHM = '1'" ]
        ServiceMgr.AthenaPoolCnvSvc.PoolAttributes += [ "DatabaseName = '" + athenaCommonFlags.PoolRDOOutput() + "'; COMPRESSION_LEVEL = '4'" ]
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






from AthenaCommon.AppMgr import ServiceMgr as svcMgr
if hasattr(runArgs,"AMITag"):
    from AthenaCommon.AppMgr import ServiceMgr as svcMgr
    svcMgr.TagInfoMgr.ExtraTagValuePairs += ["AMITag", runArgs.AMITag ]

### No RDO output to increase file size of
# Increase max RDO output file size to 10 GB

#from AthenaCommon.AppMgr import ServiceMgr as svcMgr
#svcMgr.AthenaPoolCnvSvc.MaxFileSizes = [ "10000000000" ] #[ "15000000000" ] #Athena complains that 15GB files are not supported

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
