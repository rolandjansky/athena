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
OverlayEventSelector = ServiceMgr.DoubleEventSelector
OverlayEventSelector.PrimaryInputCollections = athenaCommonFlags.PoolHitsInput()
OverlayEventSelector.SecondaryaryInputCollections = athenaCommonFlags.PoolRDOInput()
OverlayEventSelector.SkipEvents = athenaCommonFlags.SkipEvents()


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
