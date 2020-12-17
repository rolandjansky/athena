# Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration

###############################################################
#
# Standalone job options file to convert SiSPSeededTracks to xAODTrackParticles
#
#==============================================================

# Set input ESD files
if not "inputESDFiles" in dir():
    inputESDFiles = ["SiSPSeededTracksStandaloneFromESD.pool.root"]

# Configuration flags
if not "doPixel" in dir():
    doPixel = True
if not "doSCT" in dir():
    doSCT = True
if not "doPrint" in dir():
    doPrint = True

# Output track location
TracksLocation = "SiSPSeededTracks"
if doPixel and not doSCT:
    TracksLocation = "SiSPSeededPixelTracks"
if not doPixel and doSCT:
    TracksLocation = "SiSPSeededSCTTracks"

#--------------------------------------------------------------
# Standard includes
#--------------------------------------------------------------
import AthenaCommon.AtlasUnixStandardJob

# Common fragments
import sys
from AthenaCommon.AppMgr import ToolSvc
from AthenaCommon.AppMgr import ServiceMgr
from InDetRecExample.InDetJobProperties import InDetFlags
from InDetRecExample.InDetKeys import InDetKeys

#--------------------------------------------------------------
# Thread-specific setup
#--------------------------------------------------------------
from AthenaCommon.ConcurrencyFlags import jobproperties
numThreads = jobproperties.ConcurrencyFlags.NumThreads()
if numThreads > 0:
    from AthenaCommon.AlgScheduler import AlgScheduler
    AlgScheduler.CheckDependencies( True )
    AlgScheduler.ShowControlFlow( True )
    AlgScheduler.ShowDataDependencies( True )

#--------------------------------------------------------------
# use auditors
#--------------------------------------------------------------
from GaudiCommonSvc.GaudiCommonSvcConf import AuditorSvc
ServiceMgr += AuditorSvc()
theAuditorSvc = ServiceMgr.AuditorSvc
theAuditorSvc.Auditors  += [ "ChronoAuditor"]
theAuditorSvc.Auditors  += [ "MemStatAuditor" ]
theApp.AuditAlgorithms=True

#--------------------------------------------------------------
# Load Geometry
#--------------------------------------------------------------
from AthenaCommon.GlobalFlags import globalflags
globalflags.DetDescrVersion="ATLAS-R2-2016-01-00-01"
globalflags.DetGeo="atlas"
globalflags.InputFormat="pool"
globalflags.DataSource="geant4"
if doPrint:
    print globalflags

#--------------------------------------------------------------
# Set Detector setup
#--------------------------------------------------------------
from AthenaCommon.DetFlags import DetFlags
DetFlags.ID_setOn()
DetFlags.Calo_setOn()
DetFlags.Muon_setOn()
DetFlags.Truth_setOff()
DetFlags.LVL1_setOff()
DetFlags.BField_setOn()

# ---- switch parts of ID off/on as follows
DetFlags.digitize.all_setOff()
DetFlags.geometry.all_setOff()
DetFlags.overlay.all_setOff()
DetFlags.pileup.all_setOff()
DetFlags.readRIOBS.all_setOff()
DetFlags.readRIOPool.all_setOn()
DetFlags.simulate.all_setOff()
DetFlags.simulateLVL1.all_setOff()
DetFlags.writeBS.all_setOff()
DetFlags.writeRIOPool.all_setOff()

import AtlasGeoModel.SetGeometryVersion
import AtlasGeoModel.GeoModelInit
import MagFieldServices.SetupField

if doPrint:
    DetFlags.Print()

#--------------------------------------------------------------
# Load IOVDbSvc
#--------------------------------------------------------------
IOVDbSvc = Service("IOVDbSvc")
from IOVDbSvc.CondDB import conddb
IOVDbSvc.GlobalTag="OFLCOND-MC16-SDR-20"
IOVDbSvc.OutputLevel = WARNING

from AthenaCommon.AthenaCommonFlags import athenaCommonFlags
athenaCommonFlags.FilesInput = inputESDFiles

import AthenaPoolCnvSvc.ReadAthenaPool
from PoolSvc.PoolSvcConf import PoolSvc
from AthenaCommon.AppMgr import ServiceMgr as svcMgr
svcMgr += PoolSvc()
PoolSvc = svcMgr.PoolSvc
EventSelector = svcMgr.EventSelector
EventSelector.InputCollections = inputESDFiles

# Set up topSequence and condSeq
from AthenaCommon.AlgSequence import AlgSequence
topSequence = AlgSequence()

from InDetRecExample.ConfiguredNewTrackingCuts import ConfiguredNewTrackingCuts
InDetNewTrackingCuts = None
if doPixel and doSCT:
    InDetNewTrackingCuts = ConfiguredNewTrackingCuts("Offline")
elif doPixel:
    InDetNewTrackingCuts = ConfiguredNewTrackingCuts("Pixel")
elif doSCT:
    InDetNewTrackingCuts = ConfiguredNewTrackingCuts("SCT")

from InDetRecExample.ConfiguredVertexingCuts import ConfiguredVertexingCuts
InDetPrimaryVertexingCuts = ConfiguredVertexingCuts(InDetFlags.primaryVertexCutSetup())

InputTrackCollection = TracksLocation

include("InDetRecExample/InDetRecConditionsAccess.py")
include("InDetRecExample/InDetRecLoadTools.py")

include("InDetRecExample/InDetxAODCreator.py")
createTrackParticles(InputTrackCollection,
                     "",
                     InputTrackCollection+"TrackParticle",
                     topSequence)

# Set the number of events to be processed
theApp.EvtMax = -1

# Output file
from AthenaPoolCnvSvc.WriteAthenaPool import AthenaPoolOutputStream
outStream = AthenaPoolOutputStream("OutStream", "AOD.pool.root")
outStream.ItemList  = ["xAOD::EventInfo#EventInfo", "xAOD::EventAuxInfo#EventInfoAux."]
outStream.ItemList += ["xAOD::TrackParticleContainer#"+InputTrackCollection+"TrackParticle"]
outStream.ItemList += ["xAOD::TrackParticleAuxContainer#"+InputTrackCollection+"TrackParticle*"]

#--------------------------------------------------------------
# Set output lvl (VERBOSE, DEBUG, INFO, WARNING, ERROR, FATAL)
#--------------------------------------------------------------
ServiceMgr.MessageSvc.OutputLevel = INFO
ServiceMgr.MessageSvc.Format = "% F%50W%S%7W%R%T %0W%M"
