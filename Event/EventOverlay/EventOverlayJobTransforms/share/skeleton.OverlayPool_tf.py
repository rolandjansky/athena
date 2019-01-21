from AthenaCommon.Logging import logging
overlaylog = logging.getLogger('overlay')
overlaylog.info( '****************** STARTING OVERLAY *****************' )

overlaylog.info( '**** Transformation run arguments' )
overlaylog.info( str(runArgs) )

from PerfMonComps.PerfMonFlags import jobproperties as jp
jp.PerfMonFlags.doMonitoring = True # to enable monitoring
jp.PerfMonFlags.doFastMon = True    # to only enable a lightweight monitoring

if hasattr(runArgs, "preExec") and runArgs.preExec != 'NONE':
    for cmd in runArgs.preExec:
        exec(cmd)

import AthenaCommon.AtlasUnixStandardJob

if hasattr(runArgs, 'preInclude'):
    for cf in runArgs.preInclude:
        include(cf)

#==============================================================
# Job definition parameters:
#==============================================================
from AthenaCommon.AppMgr import ServiceMgr
from AthenaCommon.GlobalFlags  import globalflags
from AthenaCommon.AthenaCommonFlags  import athenaCommonFlags
from OverlayCommonAlgs.OverlayFlags import overlayFlags

globalflags.isOverlay.set_Value_and_Lock(True)
overlayFlags.isDataOverlay.set_Value_and_Lock(False)

if hasattr(runArgs,"skipEvents"): athenaCommonFlags.SkipEvents.set_Value_and_Lock( runArgs.skipEvents )
if hasattr(runArgs,"maxEvents"): athenaCommonFlags.EvtMax.set_Value_and_Lock( runArgs.maxEvents )

if hasattr(runArgs,"inputHITSFile"):
    athenaCommonFlags.PoolHitsInput.set_Value_and_Lock( runArgs.inputHITSFile )
    SignalInputCollections = runArgs.inputHITSFile
else:
    raise RuntimeError ("No input HITS file defined")

if hasattr(runArgs,"outputRDOFile"):
    athenaCommonFlags.PoolRDOOutput.set_Value_and_Lock( runArgs.outputRDOFile )

if hasattr(runArgs, 'outputRDO_SGNLFile'):
    raise RuntimeError ("Not supported in master")

if hasattr(runArgs,"geometryVersion"): globalflags.DetDescrVersion.set_Value_and_Lock( runArgs.geometryVersion )
if hasattr(runArgs,"conditionsTag"): globalflags.ConditionsTag.set_Value_and_Lock( runArgs.conditionsTag )

from Digitization.DigitizationFlags import digitizationFlags
if hasattr(runArgs,"digiSeedOffset1"): digitizationFlags.rndmSeedOffset1=int(runArgs.digiSeedOffset1)
if hasattr(runArgs,"digiSeedOffset2"): digitizationFlags.rndmSeedOffset2=int(runArgs.digiSeedOffset2)
if hasattr(runArgs,"samplingFractionDbTag"): digitizationFlags.physicsList=runArgs.samplingFractionDbTag
if hasattr(runArgs,"digiRndmSvc"): digitizationFlags.rndmSvc=runArgs.digiRndmSvc
if hasattr(runArgs, "AddCaloDigi"): digitizationFlags.experimentalDigi+=["AddCaloDigi"]

from RecExConfig.RecFlags import rec
rec.projectName = 'IS_SIMULATION'

DataInputCollections=runArgs.inputRDO_BKGFile
athenaCommonFlags.PoolRDOInput=runArgs.inputRDO_BKGFile

print "================ DetFlags ================ "
if 'DetFlags' in dir():
    overlaylog.warning("DetFlags already defined! This means DetFlags should have been fully configured already..")
else:
    from AthenaCommon.DetFlags import DetFlags

    DetFlags.ID_setOn()
    DetFlags.Muon_setOn()
    DetFlags.LAr_setOn()
    DetFlags.Tile_setOn()
    DetFlags.BCM_setOn()
    DetFlags.Lucid_setOn()
    DetFlags.Truth_setOn()

    if hasattr(runArgs, "triggerConfig") and runArgs.triggerConfig == "NONE":
        DetFlags.LVL1_setOff()
    else:
        DetFlags.LVL1_setOn()

    DetFlags.digitize.LVL1_setOff()

DetFlags.Print()

globalflags.DataSource.set_Value_and_Lock('geant4')

#--------------------------------------------------------------
# Read Simulation MetaData (unless override flag set to True)
#--------------------------------------------------------------
if 'ALL' in digitizationFlags.overrideMetadata.get_Value():
    overlaylog.info("Skipping input file MetaData check.")
else :
    from EventOverlayJobTransforms.OverlayReadMetaData import readInputFileMetadata
    readInputFileMetadata()

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


print "================ Start ================= "
from AthenaCommon.AlgSequence import AlgSequence
topSeq = AlgSequence()
## Set Overall per-Algorithm time-limit on the AlgSequence
topSeq.TimeOut = 43200 * Units.s

# Timings
try:
    from RecAlgs.RecAlgsConf import TimingAlg
    topSeq += TimingAlg("OverlayTimerBegin", TimingObjOutputName = "HITStoRDO_timings")
except:
    overlaylog.warning('Could not add TimingAlg, no timing info will be written out.')


include ( "RecExCond/AllDet_detDescr.py" ) #FIXME Dangerous to use this one

from AthenaCommon.AppMgr import theApp
theApp.EventLoop = "PileUpEventLoopMgr"
if hasattr( runArgs, 'maxEvents'):
    theApp.EvtMax = runArgs.maxEvents

include ( "EventOverlayJobTransforms/ConfiguredOverlay_jobOptions.py" )

include("Digitization/RunNumberOverride.py")

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

# For random number initialization
from AthenaCommon.ConfigurableDb import getConfigurable
ServiceMgr += getConfigurable(digitizationFlags.rndmSvc.get_Value())()
digitizationFlags.rndmSeedList.addtoService()
digitizationFlags.rndmSeedList.printSeeds()


#================================================================
print "overlay_trf: final outStream = ", outStream

#================================================================
# Logging
#--------------------------------------------------------------
#ServiceMgr.MessageSvc.OutputLevel = INFO
ServiceMgr.MessageSvc.OutputLevel = INFO
ServiceMgr.MessageSvc.Format = "% F%45W%S%7W%R%T %0W%M"


# Post-include
if hasattr(runArgs,"postInclude"):
    for fragment in runArgs.postInclude:
        include(fragment)

if hasattr(runArgs, "postExec") and runArgs.postExec != 'NONE':
    for cmd in runArgs.postExec:
        exec(cmd)

from AthenaCommon.AppMgr import ServiceMgr

#Patch /TagInfo metadata container
from OverlayCommonAlgs.OverlayFlags import overlayFlags
for key in overlayFlags.extraTagInfoPairs.get_Value().keys():
    ServiceMgr.TagInfoMgr.ExtraTagValuePairs += [str(key), str(overlayFlags.extraTagInfoPairs.get_Value()[key])]
if hasattr(runArgs, 'AMITag'):
    if runArgs.AMITag != "NONE":
        ServiceMgr.TagInfoMgr.ExtraTagValuePairs += ["AMITag", runArgs.AMITag]

#print "OverlayPool_tf.py: at the end. job=\n", job
print "\nOverlayPool_tf.py: at the end. ServiceMgr=\n", ServiceMgr
