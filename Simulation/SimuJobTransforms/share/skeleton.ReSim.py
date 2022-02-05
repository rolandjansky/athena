
## Get the logger
from AthenaCommon.Logging import *
atlasG4log = logging.getLogger('ReSim')
atlasG4log.info('****************** STARTING ISF ******************')

## Include common skeleton
include("SimuJobTransforms/CommonSkeletonJobOptions.py")

if hasattr(runArgs, 'useISF') and not runArgs.useISF:
    raise RuntimeError("Unsupported configuration! If you want to run with useISF=False, please use AtlasG4_tf.py!")

## Simulation flags need to be imported first
from G4AtlasApps.SimFlags import simFlags
simFlags.load_atlas_flags()
simFlags.ISFRun=True
from ISF_Config.ISF_jobProperties import ISF_Flags

## Set simulation geometry tag
if hasattr(runArgs, 'geometryVersion'):
    simFlags.SimLayout.set_Value_and_Lock(runArgs.geometryVersion)
    globalflags.DetDescrVersion = simFlags.SimLayout.get_Value()
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
if hasattr(runArgs, "inputHITSFile"):
    from AthenaCommon.GlobalFlags import globalflags
    globalflags.InputFormat.set_Value_and_Lock('pool')
    from AthenaCommon.AthenaCommonFlags import athenaCommonFlags
    athenaCommonFlags.PoolEvgenInput.set_Value_and_Lock( runArgs.inputHITSFile )
    athenaCommonFlags.FilesInput.set_Value_and_Lock( runArgs.inputHITSFile )
    ISF_Flags.ReSimulation = True
else:
    atlasG4log.error('No inputHITSFile provided. Please check whether you intended to run ReSim_tf.py or Sim_tf.py')
    raise RuntimeError("No outputHITSFile provided.")

## Output hits file config
if hasattr(runArgs, "outputHITS_RSMFile"):
    athenaCommonFlags.PoolHitsOutput.set_Value_and_Lock( runArgs.outputHITS_RSMFile )
else:
    atlasG4log.info('No outputHITSFile provided. This re-simulation job will not write out a HITS_RSM file.')
    athenaCommonFlags.PoolHitsOutput = ""
    athenaCommonFlags.PoolHitsOutput.statusOn = False

## Simulator
from ISF_Config.ISF_jobProperties import ISF_Flags
if hasattr(runArgs, 'simulator'):
    ISF_Flags.Simulator.set_Value_and_Lock(runArgs.simulator)
else:
    ISF_Flags.Simulator.set_Value_and_Lock('FullG4')

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

## Include common skeleton after the preExec/preInclude
include("SimuJobTransforms/skeleton.EVGENtoHIT.py")

try:
    from ISF_Config import FlagSetters
    FlagSetters.configureFlagsBase()
## Check for any simulator-specific configuration
    configureFlags = getattr(FlagSetters, ISF_Flags.Simulator.configFlagsMethodName(), None)
    if configureFlags is not None:
        configureFlags()
except:
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

## Configure Forward Detector DetFlags based on command-line options
from AthenaCommon.DetFlags import DetFlags
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
    atlasG4log.info('randomSeed not set')
## Don't use the SeedsG4 override
simFlags.SeedsG4.set_Off()


## Set the Run Number (if required)
if hasattr(runArgs,"DataRunNumber"):
    if runArgs.DataRunNumber>0:
        atlasG4log.info( 'Overriding run number to be: %s ' % runArgs.DataRunNumber )
        simFlags.RunNumber=runArgs.DataRunNumber
elif hasattr(runArgs,'jobNumber'):
    if runArgs.jobNumber>=0:
        atlasG4log.info( 'Using job number '+str(runArgs.jobNumber)+' to derive run number.' )
        simFlags.RunNumber = simFlags.RunDict.GetRunNumber( runArgs.jobNumber )
        atlasG4log.info( 'Set run number based on dictionary to '+str(simFlags.RunNumber) )
else:
    atlasG4log.info( 'Using run number: %s ', simFlags.RunNumber )

## The looper killer is on by default. Disable it if this is requested.
if hasattr(runArgs, "enableLooperKiller") and not runArgs.enableLooperKiller:
    simFlags.OptionalUserActionList.removeAction('G4UA::LooperKillerTool')
    atlasG4log.warning("The looper killer will NOT be run in this job.")

## Set up the top sequence
from AthenaCommon.AlgSequence import AlgSequence
athAlgSeq = AlgSequence()

if not hasattr( athAlgSeq, "xAODMaker::EventInfoCnvAlg" ):
    athAlgSeq += CfgMgr.xAODMaker__EventInfoCnvAlg(AODKey="McEventInfo")

from McEventCollectionFilter.McEventCollectionFilterConf import TruthResetAlg
athAlgSeq += TruthResetAlg("TruthResetAlg",InputMcEventCollection="TruthEventOLD", OutputMcEventCollection="BeamTruthEvent")

# Here we are going to run conditional re-simulation
simSequence = AlgSequence('SimSequence')
topSeq = simSequence
athAlgSeq += simSequence
copyHitSequence = AlgSequence('CopyHitSequence')
athAlgSeq += copyHitSequence

# Set Overall per-Algorithm time-limit on the AlgSequence
topSeq.TimeOut = 43200 * Units.s

from AthenaCommon.CfgGetter import getAlgorithm
topSeq += getAlgorithm("ISF_SimEventFilter")

## try:
##     from RecAlgs.RecAlgsConf import TimingAlg
##     topSeq+=TimingAlg("SimTimerBegin", TimingObjOutputName = "EVNTtoHITS_timings")
## except:
##     atlasG4log.warning('Could not add TimingAlg, no timing info will be written out.')

copyHitSequence += getAlgorithm("ISF_InvertedSimEventFilter")
copyHitSequence += getAlgorithm('RenameHitCollections')

#### *********** import ISF_Example code here **************** ####
include("ISF_Config/ISF_ConfigJobInclude.py")

#Configure AddressRemappingSvc here
from SGComps import AddressRemappingSvc
AddressRemappingSvc.addInputRename("McEventCollection","TruthEvent","TruthEventOLD")
AddressRemappingSvc.addInputRename("RecoTimingObj", "EVNTtoHITS_timings", "EVNTtoHITS_timingsOLD")
## Inner Detector
if DetFlags.ID_on():
    if DetFlags.BCM_on():
        AddressRemappingSvc.addInputRename("SiHitCollection", "BCMHits", "BCMHitsOLD")
        AddressRemappingSvc.addInputRename("SiHitCollection", "BLMHits", "BLMHitsOLD")
    if DetFlags.pixel_on():
        AddressRemappingSvc.addInputRename("SiHitCollection", "PixelHits", "PixelHitsOLD")
    if DetFlags.SCT_on():
        AddressRemappingSvc.addInputRename("SiHitCollection", "SCT_Hits", "SCT_HitsOLD")
    if DetFlags.TRT_on():
        AddressRemappingSvc.addInputRename("TRTUncompressedHitCollection", "TRTUncompressedHits", "TRTUncompressedHitsOLD")
    AddressRemappingSvc.addInputRename("TrackRecordCollection", "CaloEntryLayer", "CaloEntryLayerOLD")

## Calo
if DetFlags.Calo_on():
    if DetFlags.LAr_on():
        AddressRemappingSvc.addInputRename("LArHitContainer", "LArHitEMB", "LArHitEMBOLD")
        AddressRemappingSvc.addInputRename("LArHitContainer", "LArHitEMEC", "LArHitEMECOLD")
        AddressRemappingSvc.addInputRename("LArHitContainer", "LArHitFCAL", "LArHitFCALOLD")
        AddressRemappingSvc.addInputRename("LArHitContainer", "LArHitHEC", "LArHitHECOLD")
        AddressRemappingSvc.addInputRename("LArHitContainer", "LArHitMiniFCAL", "LArHitMiniFCALOLD") # Obsolete
        AddressRemappingSvc.addInputRename("CaloCalibrationHitContainer", "LArCalibrationHitActive", "LArCalibrationHitActiveOLD")
        AddressRemappingSvc.addInputRename("CaloCalibrationHitContainer", "LArCalibrationHitDeadMaterial", "LArCalibrationHitDeadMaterialOLD")
        AddressRemappingSvc.addInputRename("CaloCalibrationHitContainer", "LArCalibrationHitInactive", "LArCalibrationHitInactiveOLD")
    if DetFlags.Tile_on():
        AddressRemappingSvc.addInputRename("TileHitVector", "MBTSHits", "MBTSHitsOLD")
        AddressRemappingSvc.addInputRename("TileHitVector", "TileHitVec", "TileHitVecOLD")
        AddressRemappingSvc.addInputRename("CaloCalibrationHitContainer", "TileCalibHitActiveCell", "TileCalibHitActiveCellOLD")
        AddressRemappingSvc.addInputRename("CaloCalibrationHitContainer", "TileCalibHitInactiveCell", "TileCalibHitInactiveCellOLD")
        AddressRemappingSvc.addInputRename("CaloCalibrationHitContainer", "TileCalibHitDeadMaterial", "TileCalibHitDeadMaterialOLD")
    AddressRemappingSvc.addInputRename("TrackRecordCollection", "MuonEntryLayer", "MuonEntryLayerOLD")

## Muon
if DetFlags.Muon_on():
    from AtlasGeoModel.MuonGMJobProperties import MuonGeometryFlags
    if MuonGeometryFlags.hasCSC() and DetFlags.CSC_on():
        AddressRemappingSvc.addInputRename("CSCSimHitCollection", "CSC_Hits", "CSC_HitsOLD")
    if DetFlags.MDT_on():
        AddressRemappingSvc.addInputRename("MDTSimHitCollection", "MDT_Hits", "MDT_HitsOLD")
    if DetFlags.RPC_on():
        AddressRemappingSvc.addInputRename("RPCSimHitCollection", "RPC_Hits", "RPC_HitsOLD")
    if DetFlags.TGC_on():
        AddressRemappingSvc.addInputRename("TGCSimHitCollection", "TGC_Hits", "TGC_HitsOLD")
    if MuonGeometryFlags.hasMM() and DetFlags.Micromegas_on():
        AddressRemappingSvc.addInputRename("MMSimHitCollection", "MicromegasSensitiveDetector", "MicromegasSensitiveDetectorOLD")
    if MuonGeometryFlags.hasSTGC() and DetFlags.sTGC_on():
        AddressRemappingSvc.addInputRename("sTGCSimHitCollection", "sTGCSensitiveDetector", "sTGCSensitiveDetectorOLD")
    AddressRemappingSvc.addInputRename("TrackRecordCollection", "MuonExitLayer", "MuonExitLayerOLD")

#FIXME Add Renaming for Fwd Detector sim hits
## FwdRegion
if DetFlags.FwdRegion_on():
    #AddressRemappingSvc.addInputRename("SimulationHitCollection",)
    pass

## Lucid
if DetFlags.Lucid_on():
    #AddressRemappingSvc.addInputRename("LUCID_SimHitCollection",)
    pass

## ZDC
if DetFlags.ZDC_on():
    #AddressRemappingSvc.addInputRename("ZDC_SimPixelHit_Collection",)
    #AddressRemappingSvc.addInputRename("ZDC_SimStripHit_Collection",)
    pass

## ALFA
if DetFlags.ALFA_on():
    #AddressRemappingSvc.addInputRename("ALFA_HitCollection",)
    #AddressRemappingSvc.addInputRename("ALFA_ODHitCollection",)
    pass

## AFP
if DetFlags.AFP_on():
    #AddressRemappingSvc.addInputRename("AFP_TDSimHitCollection")
    #AddressRemappingSvc.addInputRename("AFP_SIDSimHitCollection")
    pass

svcMgr.StoreGateSvc.Dump = True #FIXME remove this line

## Add AMITag MetaData to TagInfoMgr
from PyUtils import AMITagHelper
AMITagHelper.SetAMITag(runArgs=runArgs)

# Set AutoFlush to 10 as per ATLASSIM-4274
# These outputs are meant to be read sequentially
if athenaCommonFlags.PoolHitsOutput():
    from AthenaPoolCnvSvc import PoolAttributeHelper as pah
    Out = athenaCommonFlags.PoolHitsOutput()
    svcMgr.AthenaPoolCnvSvc.PoolAttributes += [ pah.setTreeAutoFlush( Out, "CollectionTree", 10 ) ]
    svcMgr.AthenaPoolCnvSvc.PoolAttributes += [ pah.setTreeAutoFlush( Out, "POOLContainer", 10 ) ]
    svcMgr.AthenaPoolCnvSvc.PoolAttributes += [ pah.setTreeAutoFlush( Out, "POOLContainerForm", 10 ) ]

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
