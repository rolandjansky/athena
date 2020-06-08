#=====================================================================
#  This job options are for testing with Athena, e.g. run with
#
#  athena TrigExL2muCalibTest/AthenaL2muCalibTestOptions.py
#
#=====================================================================
#
from AthenaCommon.Constants import VERBOSE, DEBUG, INFO, ERROR
	   
# Add timestamp to python logger
from AthenaCommon.Logging import log
log.setFormat("%(asctime)s  Py:%(name)-31s %(levelname)7s %(message)s")	

from AthenaCommon.Logging import logging
log = logging.getLogger( 'TriggerUnixStandardSetup::setupCommonServices:' )
	     
# Do the default Atlas job configuration first
import AthenaCommon.AtlasUnixStandardJob
	
# Now do HLT specific configuration
from AthenaCommon import CfgMgr
from AthenaCommon.AppMgr import theApp
from AthenaCommon.AppMgr import ServiceMgr as svcMgr
from AthenaCommon.AppMgr import ToolSvc
	
# make the message service available
svcMgr.MessageSvc = theApp.service( "MessageSvc" )     # already instantiated
MessageSvc = svcMgr.MessageSvc
MessageSvc.defaultLimit = 0
MessageSvc.enableSuppression = False
MessageSvc.OutputLevel = DEBUG

# StoreGateSvc
svcMgr.StoreGateSvc.ActivateHistory = False
	   
# ProxyProviderSvc services configuration
svcMgr += CfgMgr.ProxyProviderSvc()
	
# --- ByteStreamAddressProviderSvc configuration
svcMgr += CfgMgr.ByteStreamAddressProviderSvc()
svcMgr.ProxyProviderSvc.ProviderNames += [ "ByteStreamAddressProviderSvc" ]
theApp.CreateSvc += [ svcMgr.ByteStreamAddressProviderSvc.getFullName() ]
svcMgr.ByteStreamAddressProviderSvc.OutputLevel = DEBUG
svcMgr.ByteStreamAddressProviderSvc.TypeNames += [
    "HLT::HLTResult/DataScouting_03",
    "HLT::HLTResult/DataScouting_05"
]
	
# Initialization of DetDescrCnvSvc
svcMgr += CfgMgr.DetDescrCnvSvc(
    # specify primary Identifier dictionary to be used
    IdDictName = "IdDictParser/ATLAS_IDS.xml"
    )
theApp.CreateSvc += [ svcMgr.DetDescrCnvSvc.getFullName() ]
svcMgr.EventPersistencySvc.CnvServices += [ "DetDescrCnvSvc" ]
	
# --- ByteStreamCnvSvc configuration
svcMgr += CfgMgr.ByteStreamCnvSvc("ByteStreamCnvSvc")
svcMgr.EventPersistencySvc.CnvServices += [ "ByteStreamCnvSvc" ]
  
# Explicitly set a few OutputLevels (needed because some services are created in
# different order when running with the PSC)
svcMgr.IncidentSvc.OutputLevel = theApp.OutputLevel
svcMgr.ProxyProviderSvc.OutputLevel = theApp.OutputLevel
svcMgr.StoreGateSvc.OutputLevel = theApp.OutputLevel

# Input
include( "ByteStreamCnvSvc/BSEventStorageEventSelector_jobOptions.py" )
svcMgr.EventSelector.OutputLevel = DEBUG
svcMgr.ByteStreamInputSvc.OutputLevel = DEBUG
svcMgr.ByteStreamCnvSvc.OutputLevel = DEBUG
svcMgr.ByteStreamAddressProviderSvc.OutputLevel = DEBUG

#**************************************************************
#
# give input BS file here
#
#==============================================================

svcMgr.EventSelector.Input = [ "bla-muon-calib._0001.data"]

#**************************************************************
#
# set number of events
#
#==============================================================
theApp.EvtMax = 5

#**************************************************************
#
# L2muCalibTest algorithm example
#
#==============================================================

#--------------------------------------------------------------
# Private Application Configuration options
#--------------------------------------------------------------
from TrigExL2muCalibTest.TrigExL2muCalibTestConf import L2muCalibTest
HelloWorld = L2muCalibTest("HelloWorld")

from AthenaCommon.AlgSequence import AlgSequence
topSequence = AlgSequence()
topSequence += HelloWorld
#
#--------------------------------------------------------------
# Algorithms Private Options
#--------------------------------------------------------------
#
#--- Allow overwrite of muon calibration buffer name from OKS/
#--- online configuration
#
HelloWorld.AllowOksConfig = False
#
#--- dump the property catalogue (when AllowOksConfig = TRUE) 
#
HelloWorld.DumpPropertyCatalogue = False
#
#--- define the muon calibration buffer name and size
#
HelloWorld.MuonCalBufferName = "file://dummy-file-from-py-jobOptions"
HelloWorld.MuonCalBufferSize = 12345678
HelloWorld.ReadBackDataScoutingCollection = True
#
#==============================================================
#
# End of L2muCalibTest example
#
#**************************************************************
