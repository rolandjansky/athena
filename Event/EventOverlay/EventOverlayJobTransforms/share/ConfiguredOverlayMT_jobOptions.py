# Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration

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
except:
    OverlayLog.warning('Could not add TimingAlg, no timing info will be written out.')

# Copy over timings if needed
job += CfgGetter.getAlgorithm("CopyTimings")


#-------------------------
# Double event selector
#-------------------------
import AthenaPoolCnvSvc.ReadAthenaPoolDouble
from AthenaCommon.AppMgr import ServiceMgr
from AthenaCommon.AthenaCommonFlags import athenaCommonFlags
from AthenaCommon.ConcurrencyFlags import jobproperties as jp
OverlayEventSelector = ServiceMgr.DoubleEventSelector
OverlayEventSelector.PrimaryInputCollections = athenaCommonFlags.PoolRDOInput()
OverlayEventSelector.SecondaryaryInputCollections = athenaCommonFlags.PoolHitsInput()
OverlayEventSelector.SkipEvents = athenaCommonFlags.SkipEvents()

# Properly generate event context
nThreads = jp.ConcurrencyFlags.NumThreads()
if nThreads > 0:
    svcMgr.AthenaHiveEventLoopMgr.UseSecondaryEventNumber = True
elif hasattr(svcMgr, "AthenaHiveEventLoopMgr"):
    svcMgr.AthenaEventLoopMgr.UseSecondaryEventNumber = True


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
