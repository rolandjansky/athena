# Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration

#-------------------------------------
# Common data and MC overlay skeleton
#-------------------------------------
from AthenaCommon.Logging import logging
OverlayLog = logging.getLogger('overlay')
OverlayLog.info('****************** STARTING OVERLAY *****************')

OverlayLog.info('**** Transformation run arguments')
OverlayLog.info(str(runArgs))

# PerfMon
from PerfMonComps.PerfMonFlags import jobproperties as pm
pm.PerfMonFlags.doMonitoring = True  # to enable monitoring
pm.PerfMonFlags.doFastMon = True     # to only enable a lightweight monitoring

# Pre-exec
if hasattr(runArgs, "preExec") and runArgs.preExec != 'NONE':
    for cmd in runArgs.preExec:
        exec(cmd)

# Pre-include
if hasattr(runArgs, 'preInclude'):
    for cf in runArgs.preInclude:
        include(cf)

#----------------------------
# Set flags from job options
#----------------------------
from AthenaCommon.GlobalFlags  import globalflags
from Digitization.DigitizationFlags import digitizationFlags
from AthenaCommon.AthenaCommonFlags import athenaCommonFlags
from OverlayCommonAlgs.OverlayFlags import overlayFlags

# TODO: handle MC and data overlay in the same transform
globalflags.isOverlay.set_Value_and_Lock(True)
globalflags.DataSource.set_Value_and_Lock('geant4')

overlayFlags.isDataOverlay.set_Value_and_Lock(False)
# TODO: temporarily force MT
overlayFlags.isOverlayMT.set_Value_and_Lock(True)

# Common athena flags
if hasattr(runArgs, "skipEvents"):
    athenaCommonFlags.SkipEvents.set_Value_and_Lock(runArgs.skipEvents)
if hasattr(runArgs, "maxEvents"):
    athenaCommonFlags.EvtMax.set_Value_and_Lock(runArgs.maxEvents)

if hasattr(runArgs, "inputHITSFile"):
    athenaCommonFlags.PoolHitsInput.set_Value_and_Lock(runArgs.inputHITSFile)
else:
    raise RuntimeError("No input HITS file defined")

if hasattr(runArgs, "inputRDO_BKGFile"):
    athenaCommonFlags.PoolRDOInput.set_Value_and_Lock(runArgs.inputRDO_BKGFile)
else:
    raise RuntimeError("No input RDO file defined")

if hasattr(runArgs, "outputRDOFile"):
    athenaCommonFlags.PoolRDOOutput.set_Value_and_Lock(runArgs.outputRDOFile)
else:
    raise RuntimeError("No output RDO file defined")

if hasattr(runArgs, "outputRDO_SGNLFile"):
    overlayFlags.signalOnlyOutput.set_Value_and_Lock(True)
    overlayFlags.PoolSignalRDOOutput.set_Value_and_Lock(runArgs.outputRDO_SGNLFile)
else:
    overlayFlags.signalOnlyOutput.set_Value_and_Lock(False)

if hasattr(runArgs, "geometryVersion"):
    globalflags.DetDescrVersion.set_Value_and_Lock(runArgs.geometryVersion)
if hasattr(runArgs, "conditionsTag"):
    globalflags.ConditionsTag.set_Value_and_Lock(runArgs.conditionsTag)

# Digitization flags
if hasattr(runArgs, "digiSeedOffset1"):
    digitizationFlags.rndmSeedOffset1 = int(runArgs.digiSeedOffset1)
if hasattr(runArgs, "digiSeedOffset2"):
    digitizationFlags.rndmSeedOffset2 = int(runArgs.digiSeedOffset2)
if hasattr(runArgs, "samplingFractionDbTag"):
    digitizationFlags.physicsList = runArgs.samplingFractionDbTag
if hasattr(runArgs, "digiRndmSvc"):
    digitizationFlags.rndmSvc = runArgs.digiRndmSvc
if hasattr(runArgs, "AddCaloDigi"):
    digitizationFlags.experimentalDigi += ["AddCaloDigi"]

# TODO: investigate why this is even needed
from RecExConfig.RecFlags import rec
rec.projectName = 'IS_SIMULATION'


#----------------------------
# Setup detector flags
#----------------------------
OverlayLog.info("================ DetFlags ================ ")
if 'DetFlags' in dir():
    overlaylog.warning("DetFlags already defined! This means DetFlags should have been fully configured already..")
else:
    from AthenaCommon.DetFlags import DetFlags

    DetFlags.SCT_setOn() 

    DetFlags.Truth_setOn()

# TODO: need to do it better
DetFlags.pileup.all_setOff()

DetFlags.Print()


#-------------------------------------------------------------
# Read Simulation MetaData (unless override flag set to True)
# ------------------------------------------------------------
if 'ALL' in digitizationFlags.overrideMetadata.get_Value():
    overlaylog.info("Skipping input file MetaData check.")
else:
    from EventOverlayJobTransforms.OverlayReadMetaData import readInputFileMetadata
    readInputFileMetadata()


#-------------------------
# Conditions
#-------------------------
from IOVDbSvc.CondDB import conddb
if hasattr(runArgs, 'conditionsTag') and runArgs.conditionsTag not in ['', 'NONE']:
    globalflags.ConditionsTag = runArgs.conditionsTag
    if len(globalflags.ConditionsTag()) != 0:
        conddb.setGlobalTag(globalflags.ConditionsTag())


# TODO: setup LVL1 Trigger Menu


#-------------------------
# Configuration
#-------------------------
OverlayLog.info("================ Configure ================= ")

# Initialise overlay
include("EventOverlayJobTransforms/ConfiguredOverlayMT_jobOptions.py")

# load the input properly
include("EventOverlayJobTransforms/OverlayInput_jobOptions.py")

if DetFlags.overlay.Truth_on():
    include("EventOverlayJobTransforms/TruthOverlay_jobOptions.py")

if DetFlags.overlay.pixel_on() or DetFlags.overlay.SCT_on() or DetFlags.overlay.TRT_on():
    include("EventOverlayJobTransforms/InnerDetectorOverlay_jobOptions.py")

# save the overlay output
include("EventOverlayJobTransforms/OverlayOutput_jobOptions.py")

# save the signal-only output
if overlayFlags.signalOnlyOutput() == True:
    include("EventOverlayJobTransforms/SignalOutput_jobOptions.py")


#-------------------------
# Random numbers
#-------------------------
# TODO: move somewhere more appropriate
from AthenaCommon.ConfigurableDb import getConfigurable
from AthenaCommon.AppMgr import ServiceMgr
ServiceMgr += getConfigurable(digitizationFlags.rndmSvc.get_Value())()
digitizationFlags.rndmSeedList.addtoService()
digitizationFlags.rndmSeedList.printSeeds()


#-------------------------
# Logging
#-------------------------
ServiceMgr.MessageSvc.OutputLevel = INFO
ServiceMgr.MessageSvc.Format = "% F%45W%S%7W%R%T %0W%M"

# Post-include
if hasattr(runArgs, "postInclude"):
    for fragment in runArgs.postInclude:
        include(fragment)

# Post-exec
if hasattr(runArgs, "postExec") and runArgs.postExec != 'NONE':
    for cmd in runArgs.postExec:
        exec(cmd)

# Patch /TagInfo metadata container
# TODO: move somewhere more appropriate
for key in overlayFlags.extraTagInfoPairs.get_Value().keys():
    ServiceMgr.TagInfoMgr.ExtraTagValuePairs += [
        str(key), str(overlayFlags.extraTagInfoPairs.get_Value()[key])]
if hasattr(runArgs, 'AMITag'):
    if runArgs.AMITag != "NONE":
        ServiceMgr.TagInfoMgr.ExtraTagValuePairs += ["AMITag", runArgs.AMITag]

#================================================================
print "\nOverlay: OutputStream = \n", outStream
print "\nOverlay: ServiceMgr = \n", ServiceMgr
