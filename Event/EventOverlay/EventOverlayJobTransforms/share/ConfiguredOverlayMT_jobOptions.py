# Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration

include.block("EventOverlayJobTransforms/ConfiguredOverlayMT_jobOptions.py")

#-------------------------
# Initialisation
#-------------------------
# Initialise the app
from AthenaCommon.AppMgr import theApp
if hasattr(runArgs, 'maxEvents'):
    theApp.EvtMax = runArgs.maxEvents

# Initialise the algorithm sequence
from AthenaCommon import CfgGetter
from AthenaCommon.AlgSequence import AlgSequence
job = AlgSequence()
topSequence = job

# Set Overall per-Algorithm time-limit on the AlgSequence
topSequence.TimeOut = 43200 * Units.s

from AthenaCommon.ConcurrencyFlags import jobproperties as jp
nThreads = jp.ConcurrencyFlags.NumThreads()

from AthenaCommon.Logging import logging
logOverlay = logging.getLogger('Overlay')

#-------------------------
# Timings
#-------------------------
if nThreads > 0:
    logOverlay.info("MT mode: Not scheduling TimingAlg")    
else:
    try:
        from RecAlgs.RecAlgsConf import TimingAlg
        job += TimingAlg("OverlayTimerBegin",
                         TimingObjOutputName="HITStoRDO_timings")
    except Exception:
        logOverlay.warning('Could not add TimingAlg, no timing info will be written out.')

# Copy over timings if needed
if not overlayFlags.isDataOverlay():
    job += CfgGetter.getAlgorithm("CopyTimings")

#-------------------------
# Double event selector
#-------------------------
import AthenaPoolCnvSvc.ReadAthenaPoolDouble
from AthenaCommon.AppMgr import ServiceMgr
from AthenaCommon.AthenaCommonFlags import athenaCommonFlags
skipPrimary=0
skipSecondary=0
if athenaCommonFlags.SkipEvents.statusOn:
    skipPrimary = athenaCommonFlags.SkipEvents()
    skipSecondary = athenaCommonFlags.SkipEvents()
if overlayFlags.SkipSecondaryEvents.statusOn and overlayFlags.SkipSecondaryEvents() >= 0:
    skipSecondary = overlayFlags.SkipSecondaryEvents()

if overlayFlags.isDataOverlay():
    ServiceMgr.EventSelector.InputCollections = athenaCommonFlags.PoolHitsInput()
    ServiceMgr.EventSelector.SkipEvents = skipPrimary
    ServiceMgr.SecondaryEventSelector.Input = athenaCommonFlags.FilesInput()
    ServiceMgr.SecondaryEventSelector.ProcessBadEvent = True
    ServiceMgr.SecondaryEventSelector.SkipEvents = skipSecondary
else:
    ServiceMgr.EventSelector.ProcessMetadata = False
    ServiceMgr.EventSelector.InputCollections = athenaCommonFlags.PoolRDOInput()
    ServiceMgr.EventSelector.SkipEvents = skipSecondary
    ServiceMgr.SecondaryEventSelector.InputCollections = athenaCommonFlags.PoolHitsInput()
    ServiceMgr.SecondaryEventSelector.SkipEvents = skipPrimary

# Properly generate event context
if nThreads > 0:
    EventLoop = Service("AthenaHiveEventLoopMgr")
else:
    EventLoop = Service("AthenaEventLoopMgr")
EventLoop.RequireInputAttributeList = True
EventLoop.UseSecondaryEventNumber = True
svcMgr += EventLoop

# Write digi metadata
if not overlayFlags.isDataOverlay():
    from EventOverlayJobTransforms.OverlayWriteMetaData import loadOverlayDigitizationMetadata
    loadOverlayDigitizationMetadata()

    if not hasattr(ServiceMgr.ToolSvc, 'IOVDbMetaDataTool'):
        ServiceMgr.ToolSvc += CfgMgr.IOVDbMetaDataTool()
    from Digitization.DigitizationFlags import digitizationFlags
    runNumber = digitizationFlags.dataRunNumber.get_Value()
    ServiceMgr.ToolSvc.IOVDbMetaDataTool.MinMaxRunNumbers = [runNumber, runNumber+1]


#-------------------------
# Common infrastructure
#-------------------------
# GeoModel
from AtlasGeoModel import SetGeometryVersion
from AtlasGeoModel import GeoModelInit
from AtlasGeoModel import SetupRecoGeometry
# MagneticField Service
import MagFieldServices.SetupField


#------------------------------------------------------------
# Check Beam and Digitization jobproperties are synchronised
#------------------------------------------------------------
from Digitization.DigiConfigCheckers import syncBeamAndDigitizationJobProperties
syncBeamAndDigitizationJobProperties()


#------------------------------------------------------------
# xAOD::EventInfo setup
#------------------------------------------------------------
from OverlayCommonAlgs.OverlayFlags import overlayFlags
# Support legacy EventInfo
if overlayFlags.processLegacyEventInfo() and not hasattr(job, "xAODMaker::EventInfoCnvAlg"):
    from xAODEventInfoCnv.xAODEventInfoCreator import xAODMaker__EventInfoCnvAlg
    alg = xAODMaker__EventInfoCnvAlg("EventInfoCnvAlg")
    alg.AODKey = overlayFlags.sigPrefix() + 'McEventInfo'
    alg.xAODKey = overlayFlags.sigPrefix() + 'EventInfo'
    job += alg

# Run the xAOD::EventInfo overlay
job += CfgGetter.getAlgorithm("EventInfoOverlay")

# Setup BS conversion for data overlay
if overlayFlags.isDataOverlay():
    include("RecExCommon/BSRead_config.py")


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
