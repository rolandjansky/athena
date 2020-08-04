# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

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


#-------------------------
# Timings
#-------------------------
try:
    from RecAlgs.RecAlgsConf import TimingAlg
    job += TimingAlg("OverlayTimerBegin",
                     TimingObjOutputName="HITStoRDO_timings")
except Exception:
    from AthenaCommon.Logging import logging
    logOverlay = logging.getLogger('Overlay')
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
if overlayFlags.isDataOverlay():
    ServiceMgr.DoubleEventSelector.InputCollections = athenaCommonFlags.PoolHitsInput()
    ServiceMgr.SecondaryEventSelector.Input = athenaCommonFlags.FilesInput()
    ServiceMgr.SecondaryEventSelector.ProcessBadEvent = True
else:
    ServiceMgr.DoubleEventSelector.InputCollections = athenaCommonFlags.PoolRDOInput()
    ServiceMgr.SecondaryEventSelector.InputCollections = athenaCommonFlags.PoolHitsInput()
if athenaCommonFlags.SkipEvents.statusOn:
    ServiceMgr.DoubleEventSelector.SkipEvents = athenaCommonFlags.SkipEvents()

# Properly generate event context
from AthenaCommon.ConcurrencyFlags import jobproperties as jp
nThreads = jp.ConcurrencyFlags.NumThreads()
if nThreads > 0:
    EventLoop = Service("AthenaHiveEventLoopMgr")
else:
    EventLoop = Service("AthenaEventLoopMgr")
EventLoop.UseSecondaryEventNumber = True
svcMgr += EventLoop


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

# TODO: Override run number if needed
# include("Digitization/RunNumberOverride.py")


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
