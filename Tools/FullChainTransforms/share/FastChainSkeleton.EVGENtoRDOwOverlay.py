# Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration

#=============================================================================
# Skeleton file for running simulation and MC-overlay in one job for FastChain
#=============================================================================

from AthenaCommon.Logging import logging
FastChainLog = logging.getLogger('FastChain')
FastChainLog.info('****************** STARTING ISF AND OVERLAY *****************')

# PerfMon
from PerfMonComps.PerfMonFlags import jobproperties as pm
pm.PerfMonFlags.doMonitoring = True  # to enable monitoring
pm.PerfMonFlags.doFastMon = True     # to only enable a lightweight monitoring
#pm.PerfMonFlags.doSemiDetailedMonitoring = True

# Import flags
from AthenaCommon.GlobalFlags  import globalflags
from AthenaCommon.AthenaCommonFlags import athenaCommonFlags
from G4AtlasApps.SimFlags import simFlags
from ISF_Config.ISF_jobProperties import ISF_Flags
from AthenaCommon.BeamFlags import jobproperties as beamFlags
from Digitization.DigitizationFlags import digitizationFlags
from OverlayCommonAlgs.OverlayFlags import overlayFlags

# Set global flags
globalflags.DataSource.set_Value_and_Lock('geant4')
globalflags.DetGeo.set_Value_and_Lock('atlas')

# Do full chain: simulation + MC-overlay
athenaCommonFlags.DoFullChain=True

# Import simulation flags (need to be done first)
simFlags.load_atlas_flags()
simFlags.ISFRun=True
simFlags.EventFilter.set_Off()

# Set overlay specific flags
globalflags.isOverlay.set_Value_and_Lock(True)
overlayFlags.isDataOverlay.set_Value_and_Lock(False)
overlayFlags.isOverlayMT.set_Value_and_Lock(True) # Force MT mode

# Initialise the app
from AthenaCommon.AppMgr import ServiceMgr
from AthenaCommon.AppMgr import theApp

# Initialise the algorithm sequence
from AthenaCommon import CfgGetter
from AthenaCommon.AlgSequence import AlgSequence
job = AlgSequence()
topSequence = job

# Set overall per-Algorithm time-limit on the AlgSequence
import AthenaCommon.SystemOfUnits as Units
topSequence.TimeOut = 43200 * Units.s

#==============================================================
# Job Configuration parameters
#==============================================================

# Pre-exec
if hasattr(runArgs, "preExec") and runArgs.preExec != 'NONE':
    FastChainLog.info("transform pre-exec")
    for cmd in runArgs.preExec:
        FastChainLog.info(cmd)
        exec(cmd)

# Pre-include
if hasattr(runArgs, 'preInclude'):
    for cmd in runArgs.preInclude:
        include(cmd)

# Post-include
if hasattr(runArgs, "postInclude"):
    for fragment in runArgs.postInclude:
        include(fragment)

# Post-exec
if hasattr(runArgs, "postExec") and runArgs.postExec != 'NONE':
    for cmd in runArgs.postExec:
        exec(cmd)

# Pre-exec for simulation
if hasattr(runArgs, "preSimExec"):
    FastChainLog.info("transform pre-sim exec")
    for cmd in runArgs.preSimExec:
        FastChainLog.info(cmd)
        exec(cmd)

# Pre-include for simulation
if hasattr(runArgs, "preSimInclude"):
    for fragment in runArgs.preSimInclude:
        include(fragment)

#--------------------------------------------------------------
# Simulation flags
#--------------------------------------------------------------

# Set the PhysicsList
if hasattr(runArgs, 'physicsList'):
    simFlags.PhysicsList = runArgs.physicsList

# Random seed
simFlags.RandomSeedList.useDefaultSeeds()
if hasattr(runArgs, "randomSeed"):
    simFlags.RandomSeedOffset = int(runArgs.randomSeed)
else:
    FastChainLog.info('randomSeed not set')

# Don't use the SeedsG4 override
simFlags.SeedsG4.set_Off()

# Set the Run Number
if hasattr(runArgs,"DataRunNumber"):
    if runArgs.DataRunNumber>0:
        FastChainLog.info( 'Overriding run number to be: %s ' % runArgs.DataRunNumber )
        simFlags.RunNumber=runArgs.DataRunNumber
        digitizationFlags.dataRunNumber=runArgs.DataRunNumber
elif hasattr(runArgs,'jobNumber'):
    if runArgs.jobNumber>=0:
        FastChainLog.info( 'Using job number '+str(runArgs.jobNumber)+' to derive run number.' )
        simFlags.RunNumber = simFlags.RunDict.GetRunNumber( runArgs.jobNumber )
        FastChainLog.info( 'Set run number based on dictionary to '+str(simFlags.RunNumber) )
else:
    #FastChainLog.info( 'Run number should be defined!' )
    raise RuntimeError("No run number defined!")

# Handle cosmics track record
if beamFlags.Beam.beamType.get_Value() == 'cosmics':
    if hasattr(runArgs, "inputEVNT_TRFile"):
        simFlags.ReadTR = athenaCommonFlags.PoolEvgenInput()[0]
    else:
        if hasattr(runArgs, "outputEVNT_TRFile"):
            simFlags.WriteTR = runArgs.outputEVNT_TRFile

# Add filters for non-cosmics simulation
if beamFlags.Beam.beamType.get_Value() != 'cosmics':
    if simFlags.CavernCuts:
        simFlags.EventFilter.set_Off()
        # Make a bigger world volume for cavern bg
        simFlags.WorldZRange.set_Value(26050)
    else:
        simFlags.EventFilter.set_On()

# The looper killer is on by default. Disable it if this is requested.
if hasattr(runArgs, "enableLooperKiller") and not runArgs.enableLooperKiller:
    simFlags.OptionalUserActionList.removeAction('G4UA::LooperKillerTool')
    FastChainLog.warning("The looper killer will NOT be run in this job.")

# Truth strategy
if hasattr(runArgs, 'truthStrategy'):
     simFlags.TruthStrategy = runArgs.truthStrategy

try:
     from BarcodeServices.BarcodeServicesConfig import barcodeOffsetForTruthStrategy
     simFlags.SimBarcodeOffset  = barcodeOffsetForTruthStrategy(str(simFlags.TruthStrategy.get_Value()))
except RuntimeError:
     Offset200kStrings = ['MC12', 'MC15a', 'MC16']
     is200kOffset = any(x in str(simFlags.TruthStrategy.get_Value()) for x in Offset200kStrings)
     if is200kOffset:
         simFlags.SimBarcodeOffset  = 200000 #MC12, MC15a, MC16 setting
     else:
         simFlags.SimBarcodeOffset  = 1000000 #MC15 setting
     FastChainLog.warning('Using unknown truth strategy '+str(simFlags.TruthStrategy.get_Value())+' guessing that barcode offset is '+str(simFlags.SimBarcodeOffset.get_Value()))

#--------------------------------------------------------------
# Digitization flags
#--------------------------------------------------------------

if hasattr(runArgs,"digiSeedOffset1"):
    digitizationFlags.rndmSeedOffset1=int(runArgs.digiSeedOffset1)
else:
    FastChainLog.warning( 'digiSeedOffset1 not set' )
    digitizationFlags.rndmSeedOffset1=1

if hasattr(runArgs,"digiSeedOffset2"):
    digitizationFlags.rndmSeedOffset2=int(runArgs.digiSeedOffset2)
else:
    FastChainLog.warning( 'digiSeedOffset2 not set' )
    digitizationFlags.rndmSeedOffset2=2

if hasattr(runArgs,"samplingFractionDbTag"): 
    digitizationFlags.physicsList=runArgs.samplingFractionDbTag

if hasattr(runArgs,"digiRndmSvc"):
    digitizationFlags.rndmSvc=runArgs.digiRndmSvc

#--------------------------------------------------------------
# Overlay flags
#--------------------------------------------------------------

if hasattr(runArgs, 'skipSecondaryEvents'):
    overlayFlags.SkipSecondaryEvents.set_Value_and_Lock(runArgs.skipSecondaryEvents)

if hasattr(runArgs, 'detectors'):
    overlayDetectors = runArgs.detectors
else:
    overlayDetectors = None

# Jobs should stop if an include fails
if hasattr(runArgs, "IgnoreConfigError"):
    athenaCommonFlags.AllowIgnoreConfigError = runArgs.IgnoreConfigError
else:
    athenaCommonFlags.AllowIgnoreConfigError = False

#==============================================================
# Inputs/outputs
#==============================================================

#--------------------------------------------------------------
# Inputs
#--------------------------------------------------------------

def setInputEvgenFileJobProperties(InputEvgenFile):
    from AthenaCommon.GlobalFlags import globalflags
    globalflags.InputFormat.set_Value_and_Lock('pool')
    athenaCommonFlags.PoolEvgenInput.set_Value_and_Lock( InputEvgenFile )
    athenaCommonFlags.FilesInput.set_Value_and_Lock( InputEvgenFile )

# Input EVNT file
if hasattr(runArgs, "inputEVNTFile"):
    setInputEvgenFileJobProperties( runArgs.inputEVNTFile )
else:
    raise RuntimeError("No input EVNT file defined")

# Input RDO_BKGFile
if hasattr(runArgs, "inputRDO_BKGFile"):
    athenaCommonFlags.PoolRDOInput.set_Value_and_Lock(runArgs.inputRDO_BKGFile)
else:
    raise RuntimeError("No input RDO_BKG file defined")

#--------------------------------------------------------------
# Outputs 
#--------------------------------------------------------------

# Output RDO File
if hasattr(runArgs,"outputRDOFile") or hasattr(runArgs,"tmpRDO"):
    if hasattr(runArgs,"outputRDOFile"):
        if hasattr(runArgs,"tmpRDO"):
            FastChainLog.fatal("Both outputRDOFile and tmpRDO specified - this configuration should not be used!")
            raise SystemError
        athenaCommonFlags.PoolRDOOutput.set_Value_and_Lock( runArgs.outputRDOFile )
    if hasattr(runArgs,"tmpRDO"):
        athenaCommonFlags.PoolRDOOutput.set_Value_and_Lock( runArgs.tmpRDO )
    if hasattr(runArgs, "AddCaloDigi"):
        AddCaloDigi = runArgs.AddCaloDigi
        if AddCaloDigi:
            FastChainLog.info("Will write out all LArDigitContainers and TileDigitsContainers to RDO file.")
            digitizationFlags.experimentalDigi+=["AddCaloDigi"]
else:
    FastChainLog.info("no output file (outputRDOFile or tmpRDO) specified - switching off output StreamRDO")

# Output RDO_SGNLFile
if hasattr(runArgs, "outputRDO_SGNLFile"): 
    overlayFlags.signalOnlyOutput.set_Value_and_Lock(True)
    overlayFlags.PoolSignalRDOOutput.set_Value_and_Lock(runArgs.outputRDO_SGNLFile)
else:
    overlayFlags.signalOnlyOutput.set_Value_and_Lock(False)

#--------------------------------------------------------------
# Double event selector
#--------------------------------------------------------------

import AthenaPoolCnvSvc.ReadAthenaPoolDouble
skipPrimary=0
skipSecondary=0
if athenaCommonFlags.SkipEvents.statusOn:
    skipPrimary = athenaCommonFlags.SkipEvents()
    skipSecondary = athenaCommonFlags.SkipEvents()
if overlayFlags.SkipSecondaryEvents.statusOn and overlayFlags.SkipSecondaryEvents() >= 0:
    skipSecondary = overlayFlags.SkipSecondaryEvents()

ServiceMgr.EventSelector.ProcessMetadata = False
ServiceMgr.EventSelector.InputCollections = athenaCommonFlags.PoolRDOInput()
ServiceMgr.EventSelector.SkipEvents = skipSecondary
ServiceMgr.SecondaryEventSelector.InputCollections = athenaCommonFlags.PoolEvgenInput()
ServiceMgr.SecondaryEventSelector.SkipEvents = skipPrimary

# Allow the run number, etc. to be modified via the event selector via its OverrideRunNumberFromInput property
if hasattr(ServiceMgr.EventSelector,'OverrideRunNumberFromInput'): ServiceMgr.EventSelector.OverrideRunNumberFromInput = True

#--------------------------------------------------------------
# Athena EventLoop Manager
#--------------------------------------------------------------

from AthenaCommon.ConcurrencyFlags import jobproperties as jp
nThreads = jp.ConcurrencyFlags.NumThreads()
if nThreads > 0:
    EventLoop = Service("AthenaHiveEventLoopMgr")
else:
    EventLoop = Service("AthenaEventLoopMgr")

EventLoop.RequireInputAttributeList = True 
EventLoop.UseSecondaryEventNumber = False
svcMgr += EventLoop

FastChainLog.info('**** Transformation run arguments')
FastChainLog.info(str(runArgs))

#==============================================================
# Simulation
#==============================================================

FastChainLog.info('****************** STARTING SIMULATION *****************')

# Include common skeleton
include("SimuJobTransforms/CommonSkeletonJobOptions.py") # skipEvents, maxEvents, conditionsTag, beamType, eventService

# Simulator
if beamFlags.Beam.beamType.get_Value() == 'cosmics':
    ISF_Flags.Simulator.set_Value_and_Lock('CosmicsG4')
elif hasattr(runArgs, 'simulator'):
    ISF_Flags.Simulator.set_Value_and_Lock(runArgs.simulator)
else:
    ISF_Flags.Simulator.set_Value_and_Lock('FullG4')

if hasattr(runArgs, 'useISF') and not runArgs.useISF:
    raise RuntimeError("Unsupported configuration! If you want to run with useISF=False, please use AtlasG4_tf.py!")

# Set simulation geometry tag
if hasattr(runArgs, 'geometryVersion'):
    simFlags.SimLayout.set_Value_and_Lock(runArgs.geometryVersion)
    globalflags.DetDescrVersion = simFlags.SimLayout.get_Value()
    FastChainLog.debug('SimLayout set to %s' % simFlags.SimLayout)
else:
    raise RuntimeError("No geometryVersion provided.")

# Set trigger config
if hasattr(runArgs,"triggerConfig"):
    from AthenaConfiguration.AllConfigFlags import ConfigFlags
    ConfigFlags.Trigger.triggerConfig=runArgs.triggerConfig

# Add AMITag MetaData to TagInfoMgr
if hasattr(runArgs, 'AMITag'):
    if runArgs.AMITag != "NONE":
        ServiceMgr.TagInfoMgr.ExtraTagValuePairs.update({"AMITag" : runArgs.AMITag})

#--------------------------------------------------------------
# Setup detector flags
#--------------------------------------------------------------

FastChainLog.info("================ DetFlags ================ ")

from AthenaCommon.DetFlags import DetFlags

from OverlayConfiguration.OverlayHelpersLegacy import setupOverlayLegacyDetectorFlags
DetFlags = setupOverlayLegacyDetectorFlags(overlayDetectors)

DetFlags.Truth_setOn()
DetFlags.Forward_setOff() 

if hasattr(runArgs, 'triggerConfig') and runArgs.triggerConfig == 'NONE':
    DetFlags.LVL1_setOff()

# Running the LVL1 simulation during digitization is currently not supported
DetFlags.digitize.LVL1_setOff()

from AtlasGeoModel.MuonGMJobProperties import MuonGeometryFlags
if not MuonGeometryFlags.hasCSC():
    DetFlags.CSC_setOff()
if not MuonGeometryFlags.hasSTGC():
    DetFlags.sTGC_setOff()
if not MuonGeometryFlags.hasMM():
    DetFlags.MM_setOff()

# Check for any simulator-specific configuration
from ISF_Config import FlagSetters
configureFlags = getattr(FlagSetters, ISF_Flags.Simulator.configFlagsMethodName(), None)
if configureFlags is not None:
    configureFlags()

# DetFlags.makeRIO.all_setOff() # needed for MT TRT conditions
DetFlags.pileup.all_setOff()
DetFlags.readRDOBS.all_setOff()
DetFlags.readRIOBS.all_setOff()
DetFlags.readRIOPool.all_setOff()
DetFlags.readRDOPool.all_setOff()
DetFlags.writeBS.all_setOff()
DetFlags.writeRIOPool.all_setOff()

DetFlags.Print()

# Set SimLayout (synchronised to globalflags)
if globalflags.DetDescrVersion() not in simFlags.SimLayout.get_Value():
    printfunc ("ERROR globalFlags.DetDescrVersion and simFlags.SimLayout do not match!")
    printfunc ("Please correct your job options.")
    import sys
    sys.exit(1)

#--------------------------------------------------------------
# Job setup
#--------------------------------------------------------------

# All det description
include('ISF_Config/AllDet_detDescr.py')

DetFlags.Print()

if len(globalflags.ConditionsTag()):
    from IOVDbSvc.CondDB import conddb
    conddb.setGlobalTag(globalflags.ConditionsTag())

# BeamEffectsAlg
topSequence += CfgGetter.getAlgorithm("BeamEffectsAlg")

# CopyMcEventCollection
job += CfgGetter.getAlgorithm("CopyMcEventCollection")

#--------------------------------------------------------------
# ISF kernel configuration
#--------------------------------------------------------------

# To make sure that any subsequent simulator configuration gets a reference to the same instance when calling confgetter's getAlgorithm
collection_merger_alg = CfgGetter.getAlgorithm('ISF_CollectionMerger')

# SimKernel
topSequence += CfgGetter.getAlgorithm(ISF_Flags.Simulator.KernelName())

if ISF_Flags.HITSMergingRequired.anyOn():
    topSequence += collection_merger_alg

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
# Write MetaData
#--------------------------------------------------------------

# Write geometry tag info
import EventInfoMgt.EventInfoMgtInit

athenaCommonFlags.DoFullChain = True

# Tweak to enable pileupMetaDataCheck
digitizationFlags.overrideMetadata=['OptionalContainers']

# Create and write simulation MetaData
from ISF_Example.ISF_Metadata import  configureRunNumberOverrides, patch_mc_channel_numberMetadata, checkForSpecialConfigurationMetadata
configureRunNumberOverrides() # Check on run numbers and update them if necessary (create EvtIdModifierSvc if needed)
patch_mc_channel_numberMetadata() # Check whether mc_channel_number is set in tag_info metadata and add if required
checkForSpecialConfigurationMetadata() # Read in special simulation job option fragments based on metadata passed by the EVGEN

from FullChainTransforms.FastChainWriteMetadata import createSimulationParametersMetadata
createSimulationParametersMetadata()

# Read digi MetaData
from EventOverlayJobTransforms.OverlayReadMetaData import readPresampledMetadata
readPresampledMetadata()

# Write digi MetaData
from EventOverlayJobTransforms.OverlayWriteMetaData import loadOverlayDigitizationMetadata
loadOverlayDigitizationMetadata()

#--------------------------------------------------------------
# Post kernel configuration
#--------------------------------------------------------------

# GeoModel initialization
from GeoModelSvc.GeoModelSvcConf import GeoModelSvc
GeoModelSvc = GeoModelSvc()
if ( DetFlags.detdescr.LAr_on() ):
    GeoModelSvc.DetectorTools["LArDetectorToolNV"].GeometryConfig = "FULL"
if ( DetFlags.detdescr.Tile_on() ):
    GeoModelSvc.DetectorTools["TileDetectorTool"].GeometryConfig = "FULL"

# True will dump data store contents
#ISF_Flags.DumpStoreGate = True
if ISF_Flags.DumpStoreGate() :
    StoreGateSvc.Dump = True

# Useful for debugging:
printfunc (topSequence)

# Changing to post-sim include/exec

# Post-include
if hasattr(runArgs, "postSimInclude"):
    for fragment in runArgs.postSimInclude:
        include(fragment)

# Post-exec
if hasattr(runArgs, "postSimExec"):
    FastChainLog.info("transform post-sim exec")
    for cmd in runArgs.postSimExec:
        FastChainLog.info(cmd)
        exec(cmd)

#==============================================================
# Overlay
#==============================================================

FastChainLog.info('****************** STARTING OVERLAY *****************')

#--------------------------------------------------------------
# Common infrastructure
#--------------------------------------------------------------

# GeoModel
from AtlasGeoModel import SetGeometryVersion
from AtlasGeoModel import GeoModelInit
from AtlasGeoModel import SetupRecoGeometry

# MagneticField Service
import MagFieldServices.SetupField

#------------------------------------------------------------
# xAOD::EventInfo setup
#------------------------------------------------------------

overlayFlags.processLegacyEventInfo.set_Value_and_Lock(True)
overlayFlags.optionalContainerMap = []
if overlayFlags.processLegacyEventInfo() and not hasattr(job, "xAODMaker::EventInfoCnvAlg"):
    from xAODEventInfoCnv.xAODEventInfoCreator import xAODMaker__EventInfoCnvAlg
    alg = xAODMaker__EventInfoCnvAlg("EventInfoCnvAlg")
    alg.AODKey = overlayFlags.sigPrefix() + 'McEventInfo'
    alg.xAODKey = overlayFlags.sigPrefix() + 'EventInfo'
    job += alg

# Run the xAOD::EventInfo overlay
job += CfgGetter.getAlgorithm("EventInfoOverlay")

#------------------------------------------------------------
# Auditors
#------------------------------------------------------------
from AthenaCommon.AppMgr import theAuditorSvc
theApp.AuditAlgorithms=True  
theApp.AuditServices=True
theApp.AuditTools=True
 
if 'FPEAuditor/FPEAuditor' not in theAuditorSvc.Auditors:
    from AthenaAuditors.AthenaAuditorsConf import FPEAuditor
    theAuditorSvc += FPEAuditor()
    import signal
    try:
        ServiceMgr.CoreDumpSvc.Signals.remove (signal.SIGFPE)
    except ValueError:
        pass

#--------------------------------------------------------------
# Configuration
#--------------------------------------------------------------

FastChainLog.info("================ Configure ================= ")

# Load the input properly
include("EventOverlayJobTransforms/OverlayInput_jobOptions.py")
# Collections in EventInfo container are renamed (e.g. McEventInfo to Sig_McEventInfo) because EventInfo is in the EVNT input file
# Collections in McEventCollection, TrackRecordCollection etc are not renamed because these containers are not in the EVNT input file (e.g. TruthEvent remains named TruthEvent and MuonEntryLayer remains named MuonEntryLayer)

# Beam overlay
if DetFlags.overlay.BCM_on() or DetFlags.overlay.Lucid_on():
    include ( "EventOverlayJobTransforms/BeamOverlay_jobOptions.py" )

# ID overlay
if DetFlags.overlay.pixel_on() or DetFlags.overlay.SCT_on() or DetFlags.overlay.TRT_on():
    include("EventOverlayJobTransforms/InnerDetectorOverlay_jobOptions.py")

# Calorimenter overlay
if DetFlags.overlay.LAr_on() or DetFlags.overlay.Tile_on():
    include ( "EventOverlayJobTransforms/CaloOverlay_jobOptions.py" )

# Muon overlay
if (MuonGeometryFlags.hasCSC() and DetFlags.overlay.CSC_on()) or DetFlags.overlay.MDT_on() or DetFlags.overlay.RPC_on() or DetFlags.overlay.TGC_on() or (MuonGeometryFlags.hasSTGC() and DetFlags.overlay.sTGC_on()) or (MuonGeometryFlags.hasMM() and DetFlags.overlay.MM_on()):
    include ( "EventOverlayJobTransforms/MuonOverlay_jobOptions.py" )

# Level1 overlay
if DetFlags.overlay.LVL1_on():
   include ( "EventOverlayJobTransforms/Level1Overlay_jobOptions.py" )

# Truth overlay
if DetFlags.overlay.Truth_on():

   job += CfgGetter.getAlgorithm("CopyInTimeAntiKt4JetTruthInfo")
   job += CfgGetter.getAlgorithm("CopyOutOfTimeAntiKt4JetTruthInfo")
   job += CfgGetter.getAlgorithm("CopyInTimeAntiKt6JetTruthInfo")
   job += CfgGetter.getAlgorithm("CopyOutOfTimeAntiKt6JetTruthInfo")
   job += CfgGetter.getAlgorithm("CopyPileupParticleTruthInfo")

# Run calculation of weight for the beam spot size reweighting
if digitizationFlags.doBeamSpotSizeReweighting() and digitizationFlags.OldBeamSpotZSize() > 0:
     include('Digitization/BeamSpotReweight.py')

# Save the overlay output
include("EventOverlayJobTransforms/OverlayOutput_jobOptions.py")

# Output for truth
if DetFlags.overlay.Truth_on():
    outStream.ItemList += ['TrackRecordCollection#*']

    outStream.ItemList += ['xAOD::JetContainer#InTimeAntiKt4TruthJets']
    outStream.ItemList += ['xAOD::AuxContainerBase!#InTimeAntiKt4TruthJetsAux.-constituentLinks.-constituentWeights']
    outStream.ItemList += ['xAOD::JetContainer#OutOfTimeAntiKt4TruthJets']
    outStream.ItemList += ['xAOD::AuxContainerBase!#OutOfTimeAntiKt4TruthJetsAux.-constituentLinks.-constituentWeights']
    outStream.ItemList += ['xAOD::JetContainer#InTimeAntiKt6TruthJets']
    outStream.ItemList += ['xAOD::AuxContainerBase!#InTimeAntiKt6TruthJetsAux.-constituentLinks.-constituentWeights']
    outStream.ItemList += ['xAOD::JetContainer#OutOfTimeAntiKt6TruthJets']
    outStream.ItemList += ['xAOD::AuxContainerBase!#OutOfTimeAntiKt6TruthJetsAux.-constituentLinks.-constituentWeights']
    outStream.ItemList += ["xAOD::TruthParticleContainer#TruthPileupParticles"]
    outStream.ItemList += ["xAOD::TruthParticleAuxContainer#TruthPileupParticlesAux."]

    if DetFlags.overlay.LAr_on() or DetFlags.overlay.Tile_on():
        outStream.ItemList += ['CaloCalibrationHitContainer#*']

# Save the signal-only output
if overlayFlags.signalOnlyOutput() == True:
    include("EventOverlayJobTransforms/SignalOutput_jobOptions.py")

#------------------------------------------------------------
# Check Beam and Digitization jobproperties are synchronised
#------------------------------------------------------------

from Digitization.DigiConfigCheckers import syncBeamAndDigitizationJobProperties
syncBeamAndDigitizationJobProperties()

#--------------------------------------------------------------
# Random numbers
#--------------------------------------------------------------

from AthenaCommon.ConfigurableDb import getConfigurable
ServiceMgr += getConfigurable(digitizationFlags.rndmSvc.get_Value())()
digitizationFlags.rndmSeedList.addtoService()
digitizationFlags.rndmSeedList.printSeeds()

#--------------------------------------------------------------
# Logging
#--------------------------------------------------------------

ServiceMgr.MessageSvc.OutputLevel = INFO
ServiceMgr.MessageSvc.Format = "% F%45W%S%5W%e%s%7W%R%T %0W%M"
