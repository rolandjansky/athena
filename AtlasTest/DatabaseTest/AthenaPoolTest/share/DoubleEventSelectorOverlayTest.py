###############################################################
#
# Job options file
#
## @file DoubleEventSelectorOverlayTest.py
##
## @brief For Athena POOL test: read an RDO and a HITS file, output a RDO file
##
## @author Miha Muskinja <miha.muskinja@cern.ch>
#
#==============================================================

# basic job configuration
import AthenaCommon.AtlasUnixStandardJob

# get a handle to the default top-level algorithm sequence
from AthenaCommon.AlgSequence import AlgSequence
topSequence = AlgSequence()

# get a handle to the ServiceManager
from AthenaCommon.AppMgr import ServiceMgr as svcMgr

# get a handle to the ApplicationManager
from AthenaCommon.AppMgr import theApp

# Set that we are running MC+MC overlay in MT mode
from OverlayCommonAlgs.OverlayFlags import overlayFlags
overlayFlags.isDataOverlay.set_Value_and_Lock(False)
overlayFlags.isOverlayMT.set_Value_and_Lock(True)

#--------------------------------------------------------------
# Load POOL support for DoubleEventSelector
#--------------------------------------------------------------
import AthenaPoolCnvSvc.ReadAthenaPoolDouble

#--------------------------------------------------------------
# Set flags and load det descr
#--------------------------------------------------------------
from RecExConfig.RecFlags      import rec
from OverlayCommonAlgs.OverlayFlags import overlayFlags

overlayFlags.isOverlayMT.set_Value_and_Lock(True)

# For general flags
rec.doAOD       = False
rec.doTrigger   = False
rec.doWriteTAG  = False
DetDescrVersion = "ATLAS-R2-2016-01-00-01"

#--------------------------------------------------------------
# Input options
#--------------------------------------------------------------
import os
data_dir = os.environ.get ('ATLAS_REFERENCE_DATA', '/cvmfs/atlas-nightlies.cern.ch/repo/data/data-art')
svcMgr.DoubleEventSelector.InputCollections = [ data_dir + "/OverlayMonitoringRTT/PileupPremixing/22.0/RDO.merged-pileup-MT.unittest.pool.root" ]
svcMgr.DoubleEventSelector.OutputLevel = DEBUG
svcMgr.SecondaryEventSelector.InputCollections = [ data_dir + "/OverlayMonitoringRTT/valid1.410000.PowhegPythiaEvtGen_P2012_ttbar_hdamp172p5_nonallhad.simul.HITS.e4993_s3091/HITS.10504490._000765.pool.root.1" ]
svcMgr.SecondaryEventSelector.OutputLevel = DEBUG

#--------------------------------------------------------------
# Remapping Service
#--------------------------------------------------------------
from SGComps import AddressRemappingSvc
AddressRemappingSvc.addInputRename("EventInfo","McEventInfo" ,"Sig_McEventInfo")
AddressRemappingSvc.addInputRename("McEventCollection","TruthEvent" ,"Sig_TruthEvent")
AddressRemappingSvc.addInputRename("RecoTimingObj","EVNTtoHITS_timings" ,"Sig_EVNTtoHITS_timings")
svcMgr.AddressRemappingSvc.OutputLevel = DEBUG

#--------------------------------------------------------------
# Event related parameters
#--------------------------------------------------------------
theApp.EvtMax = 10

#--------------------------------------------------------------
# Algorithms
#--------------------------------------------------------------
from AthenaCommon import CfgGetter
topSequence += CfgGetter.getAlgorithm("CopyMcEventCollection")
topSequence += CfgGetter.getAlgorithm("CopyTimings")

#--------------------------------------------------------------
# Athena EventLoop Manager
#--------------------------------------------------------------
from AthenaCommon.ConcurrencyFlags import jobproperties as jp
nThreads = jp.ConcurrencyFlags.NumThreads()
from AthenaServices import AthenaServicesConf
if nThreads > 0:
    EventLoop = AthenaServicesConf.AthenaHiveEventLoopMgr()
else:
    EventLoop = AthenaServicesConf.AthenaEventLoopMgr()
EventLoop.UseSecondaryEventNumber = True
EventLoop.OutputLevel = INFO
svcMgr += EventLoop

#--------------------------------------------------------------
# DEBUG messaging
#--------------------------------------------------------------
svcMgr.ProxyProviderSvc.OutputLevel = DEBUG
svcMgr.AthenaPoolAddressProviderSvcPrimary.OutputLevel = DEBUG
svcMgr.AthenaPoolAddressProviderSvcSecondary.OutputLevel = DEBUG
svcMgr.DoubleEventSelector.OutputLevel = DEBUG

#--------------------------------------------------------------
# Output options
#--------------------------------------------------------------
from AthenaPoolCnvSvc.WriteAthenaPool import AthenaPoolOutputStream
# TODO: noTag=True to avoid warning, needs EventInfo overlay implemented
Stream1 = AthenaPoolOutputStream( "Stream1", asAlg=True, noTag=True )
Stream1.OutputLevel = INFO

Stream1.OutputFile  = "OutputRDO.root"
# List of DO's to write out
Stream1.ItemList =  []
Stream1.ItemList += ["McEventCollection#TruthEvent"]
Stream1.ItemList += ["RecoTimingObj#EVNTtoHITS_timings"]

#--------------------------------------------------------------
# Set output level threshold (2=DEBUG, 3=INFO, 4=WARNING, 5=ERROR, 6=FATAL )
#--------------------------------------------------------------
svcMgr.MessageSvc.debugLimit  = 100000

# No stats printout
include( "AthenaPoolTest/NoStats_jobOptions.py" )

#==============================================================
#
# End of job options file
#
###############################################################
