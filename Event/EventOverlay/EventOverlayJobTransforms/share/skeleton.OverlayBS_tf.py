from AthenaCommon.Logging import logging
overlaylog = logging.getLogger('overlay_bs')
overlaylog.info( '****************** STARTING OVERLAY *****************' )

overlaylog.info( '**** Transformation run arguments' )
overlaylog.info( str(runArgs) )

from PerfMonComps.PerfMonFlags import jobproperties as jp
jp.PerfMonFlags.doMonitoring = True # to enable monitoring
jp.PerfMonFlags.doFastMon = True    # to only enable a lightweight monitoring

if hasattr(runArgs, "preExec") and runArgs.preExec != 'NONE':
    for cmd in runArgs.preExec:
        exec(cmd)

#TileFrameLength=7

from AthenaCommon.AppMgr import ServiceMgr
from AthenaCommon.GlobalFlags  import globalflags
from AthenaCommon.AthenaCommonFlags  import athenaCommonFlags
from OverlayCommonAlgs.OverlayFlags import overlayFlags

from MuonRecExample.MuonRecFlags import muonRecFlags
muonRecFlags.doCSCs.set_Value_and_Lock(True)

from LArConditionsCommon.LArCondFlags import larCondFlags
larCondFlags.LArCoolChannelSelection.set_Value_and_Lock("")
larCondFlags.useShape.set_Value_and_Lock(True)
larCondFlags.OFCShapeFolder.set_Value_and_Lock("")

globalflags.isOverlay.set_Value_and_Lock(True)
isRealData = globalflags.isOverlay()

from AthenaCommon.BeamFlags import jobproperties
jobproperties.Beam.beamType.set_Value_and_Lock("collisions")

import AthenaCommon.AtlasUnixStandardJob #FIXME: Is this needed?

if hasattr(runArgs,"preInclude"):
    for cf in runArgs.preInclude:
        include(cf)

#==============================================================
# Job definition parameters:
#==============================================================

if hasattr(runArgs,"inputBS_SKIMFile"):
    athenaCommonFlags.FilesInput.set_Value_and_Lock( runArgs.inputBS_SKIMFile )
    DataInputCollections=runArgs.inputBS_SKIMFile # Remove this line
else:
    raise RuntimeError ("No input BS file defined")

if hasattr(runArgs,"skipEvents"): athenaCommonFlags.SkipEvents.set_Value_and_Lock( runArgs.skipEvents )
if hasattr(runArgs,"maxEvents"): athenaCommonFlags.EvtMax.set_Value_and_Lock( runArgs.maxEvents )

if hasattr(runArgs,"inputHITSFile"):
    athenaCommonFlags.PoolHitsInput.set_Value_and_Lock( runArgs.inputHITSFile )
    SignalInputCollections = runArgs.inputHITSFile #remove this line
else:
    raise RuntimeError ("No input HITS file defined")

if hasattr(runArgs,"outputRDOFile"):
    athenaCommonFlags.PoolRDOOutput.set_Value_and_Lock( runArgs.outputRDOFile )
    OverlayCollection = runArgs.outputRDOFile #remove this line

if not hasattr(runArgs, 'outputRDO_SGNLFile') or runArgs.outputRDO_SGNLFile=="NONE":
    overlayFlags.doSignal=False
    SignalCollection = "NONE"
else:
    overlayFlags.doSignal=True
    SignalCollection = runArgs.outputRDO_SGNLFile

if hasattr(runArgs,"geometryVersion"): globalflags.DetDescrVersion.set_Value_and_Lock( runArgs.geometryVersion )
if hasattr(runArgs,"conditionsTag"): globalflags.ConditionsTag.set_Value_and_Lock( runArgs.conditionsTag )

from Digitization.DigitizationFlags import digitizationFlags
if hasattr(runArgs,"digiSeedOffset1"): digitizationFlags.rndmSeedOffset1=int(runArgs.digiSeedOffset1)
if hasattr(runArgs,"digiSeedOffset2"): digitizationFlags.rndmSeedOffset2=int(runArgs.digiSeedOffset2)
if hasattr(runArgs,"samplingFractionDbTag"): digitizationFlags.physicsList=runArgs.samplingFractionDbTag
if hasattr(runArgs,"digiRndmSvc"): digitizationFlags.rndmSvc=runArgs.digiRndmSvc
#if hasattr(runArgs, "AddCaloDigi"): digitizationFlags.experimentalDigi+=["AddCaloDigi"]

readBS = True
overlayFlags.doBkg=False #ACH

#GlobalFlags.InputFormat.set_bytestream()
globalflags.InputFormat.set_Value_and_Lock('bytestream')
globalflags.DetGeo = 'commis'
globalflags.DataSource.set_Value_and_Lock('data')
#GlobalFlags.DataSource.set_data()

import PyUtils.AthFile as af
def getHITSFile(runArgs):
    if hasattr(runArgs,"inputHITSFile"):
        return runArgs.inputHITSFile[0]
    elif hasattr(runArgs,"inputHitsFile"):
        return runArgs.inputHitsFile[0]
    else:
        raise SystemExit("No HITS file in runArgs!!")
try:
    f = af.fopen(getHITSFile(runArgs))
except AssertionError:
    skeletonLog.error("Failed to open input file: %s", getHITSFile(runArgs))
metadatadict = dict()
if 'metadata' in f.infos.keys():
    if '/Simulation/Parameters' in f.infos['metadata'].keys():
        metadatadict = f.infos['metadata']['/Simulation/Parameters']
        if isinstance(metadatadict, list):
            skeletonLog.warning("%s inputfile: %s contained %s sets of Simulation Metadata. Using the final set in the list.",inputtype,inputfile,len(metadatadict))
            metadatadict=metadatadict[-1]
        if 'RunNumber' in metadatadict.keys():
            year = metadatadict['RunNumber']%100
            print "Found Year = %s", year
            from RecExConfig.RecFlags import rec
            rec.projectName = 'data'+str(year)
            pass
        pass
    pass
pass

import MagFieldServices.SetupField

from IOVDbSvc.CondDB import conddb

if hasattr(runArgs, 'conditionsTag') and runArgs.conditionsTag!='NONE' and runArgs.conditionsTag!='':
   globalflags.ConditionsTag=runArgs.conditionsTag
   if len(globalflags.ConditionsTag())!=0:
      conddb.setGlobalTag(globalflags.ConditionsTag())

# LVL1 Trigger Menu
if hasattr(runArgs, "triggerConfig") and runArgs.triggerConfig!="NONE":
    # LVL1 Trigger Menu
    # PJB 9/2/2009 Setup the new triggerConfig flags here
    from TriggerJobOpts.TriggerFlags import TriggerFlags
    triggerArg = runArgs.triggerConfig
    #if not prefixed with LVL1: add it here
    Args = triggerArg.split(":")
    if Args[0] != "LVL1":
        TriggerFlags.triggerConfig ="LVL1:"+triggerArg
    else:
        TriggerFlags.triggerConfig =triggerArg
    overlaylog.info( 'triggerConfig argument is: %s ', TriggerFlags.triggerConfig.get_Value() )
    from TriggerJobOpts.TriggerConfigGetter import TriggerConfigGetter
    cfg = TriggerConfigGetter("HIT2RDO")


print "================ DetFlags ================ "
if 'DetFlags' in dir():
    overlaylog.warning("DetFlags already defined! This means DetFlags should have been fully configured already..")
else:
    from AthenaCommon.DetFlags import DetFlags

    DetFlags.ID_setOn()
    DetFlags.Muon_setOn()
    DetFlags.LAr_setOn()
    DetFlags.Tile_setOn()
    if not hasattr(runArgs, "triggerConfig") or runArgs.triggerConfig=="NONE":
        DetFlags.LVL1_setOff()
    else:
        DetFlags.LVL1_setOn()

    DetFlags.BCM_setOn()
    DetFlags.Lucid_on()
    DetFlags.simulateLVL1.Lucid_setOff()
    #DetFlags.simulateLVL1.LAr_setOn()
    #DetFlags.simulateLVL1.Tile_setOn()
    #DetFlags.overlay.LAr_setOff()
    DetFlags.overlay.Truth_setOn()

DetFlags.Print()

include ( "RecExCond/AllDet_detDescr.py" )

from AthenaCommon.AppMgr import theApp
theApp.EventLoop = "PileUpEventLoopMgr"
if hasattr( runArgs, 'maxEvents'):
    theApp.EvtMax = runArgs.maxEvents

include ( "EventOverlayJobTransforms/ConfiguredOverlay_jobOptions.py" )

if DetFlags.overlay.Truth_on():
   include ( "EventOverlayJobTransforms/TruthOverlay_jobOptions.py" )

if DetFlags.overlay.BCM_on() or DetFlags.overlay.Lucid_on():
   include ( "EventOverlayJobTransforms/BeamOverlay_jobOptions.py" )

if DetFlags.overlay.pixel_on() or DetFlags.overlay.SCT_on() or DetFlags.overlay.TRT_on():

   include ( "EventOverlayJobTransforms/InnerDetectorOverlay_jobOptions.py" )

if DetFlags.overlay.LAr_on() or DetFlags.overlay.Tile_on():
   include ( "EventOverlayJobTransforms/CaloOverlay_jobOptions.py" )

if DetFlags.overlay.CSC_on() or DetFlags.overlay.MDT_on() or DetFlags.overlay.RPC_on() or DetFlags.overlay.TGC_on():
   include ( "EventOverlayJobTransforms/MuonOverlay_jobOptions.py" )

if DetFlags.overlay.LVL1_on():
   include ( "EventOverlayJobTransforms/Level1Overlay_jobOptions.py" )

# save the overlay output first
include ( "EventOverlayJobTransforms/OverlayOutputItemList_jobOptions.py" )

# now save the signal information in the same job
if overlayFlags.doSignal==True:
   include ( "EventOverlayJobTransforms/SignalOutputItemList_jobOptions.py" )

# For random number initialization
from AthenaCommon.ConfigurableDb import getConfigurable
ServiceMgr += getConfigurable(digitizationFlags.rndmSvc.get_Value())()
digitizationFlags.rndmSeedList.addtoService()
digitizationFlags.rndmSeedList.printSeeds()

# Write Digitization MetaData if WriteRDOFileMetaData is set to true in jO (false by default)
if hasattr(runArgs,"WriteRDOFileMetaData"):
    if runArgs.WriteRDOFileMetaData:
        from Digitization.DigitizationWriteMetaData import writeDigitizationMetadata
        writeDigitizationMetadata()

#================================================================
print "overlay_trf: final outStream = ", outStream

#================================================================
# Logging
#--------------------------------------------------------------
#ServiceMgr.MessageSvc.OutputLevel = INFO
ServiceMgr.MessageSvc.OutputLevel = INFO
ServiceMgr.MessageSvc.Format = "% F%45W%S%7W%R%T %0W%M"

if hasattr(runArgs, 'fSampltag'):
    #conddb.addFolder("LAR","/LAR/ElecCalib/fSampl/Symmetry")
    #conddb.addOverride( "/LAR/ElecCalib/fSampl/Symmetry", runArgs.fSampltag + digitizationFlags.physicsList.get_Value() )
    conddb.addFolderWithTag("LAR_OFL","/LAR/ElecCalibMC/fSampl", runArgs.fSampltag + digitizationFlags.physicsList.get_Value(),force=True,forceMC=True) 
else:
    raise RuntimeError ("--fSampltag not specified on command-line - see --help message")
#if DetFlags.overlay.Signal_on():
#   InputDBConnection = "COOLOFL_LAR/COMP200"
#   conddb.addFolder("","/LAR/ElecCalibOfl/AutoCorrs/AutoCorr"+"<dbConnection>"+InputDBConnection+"</dbConnection>")
#   conddb.addOverride("/LAR/ElecCalibOfl/AutoCorrs/AutoCorr","")

#ServiceMgr.TileInfoLoader.filenameDeltaPhysicsSuffix="of2_Delta_Phys_7Samples"

# Post-include
if hasattr(runArgs,"postInclude"):
    for fragment in runArgs.postInclude:
        include(fragment)

if hasattr(runArgs, "postExec") and runArgs.postExec != 'NONE':
    for cmd in runArgs.postExec:
        exec(cmd)

#print "OverlayBS_tf.py: at the end. job=\n", job
print "\nOverlayBS_tf.py: at the end. ServiceMgr=\n", ServiceMgr
