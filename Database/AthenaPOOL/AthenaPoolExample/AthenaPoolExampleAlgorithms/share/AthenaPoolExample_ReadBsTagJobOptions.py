## @file AthenaPoolExample_ReadJobOptions.py
## @brief Example job options file to illustrate how to read event data from Pool.
## @author Peter van Gemmeren <gemmeren@anl.gov>
## $Id: AthenaPoolExample_ReadJobOptions.py,v 1.35 2009-05-20 18:02:24 gemmeren Exp $
###############################################################
#
# This Job option:
# ----------------
# 1. Reads the data from files have been written with AthneaPoolExample_WriteJobOptions.py
#    and AthneaPoolExample_RWJobOptions.py using global BackNavigation and skipping first 10 events.
# 2. Same as 1., but using TAG collections. Instead of SkipEvents, a HelperTool is used to skip the
#    first 10 events using their MagicTag attribute (example for computational TAG processing).
# 3. Same as 1., but using scoped BackNavigation to ExampleHit.
#
#==============================================================

## basic job configuration
import AthenaCommon.AtlasUnixStandardJob

## get a handle on the default top-level algorithm sequence
from AthenaCommon.AlgSequence import AlgSequence
topSequence = AlgSequence()

#--------------------------------------------------------------
# Event related parameters
#--------------------------------------------------------------
from AthenaCommon.AppMgr import theApp
theApp.EvtMax = 200000

#--------------------------------------------------------------
# Load POOL support
#--------------------------------------------------------------
import AthenaPoolCnvSvc.ReadAthenaPool

## get a handle on the ServiceManager
from AthenaCommon.AppMgr import ServiceMgr as svcMgr

from ByteStreamCnvSvc.ByteStreamCnvSvcConf import ByteStreamMetadataTool
svcMgr.MetaDataSvc.MetaDataTools += [ "ByteStreamMetadataTool" ]

svcMgr.EventSelector.InputCollections = [ "SimplePoolFromRaw.root" ]
svcMgr.EventSelector.InputCollections = [ "SimpleRawCollection.root" ]
svcMgr.EventSelector.CollectionType = "ExplicitROOT"
svcMgr.EventSelector.RefName = "StreamRAW"
svcMgr.EventSelector.Query="EventNumber==14356"

from ByteStreamCnvSvc.ByteStreamCnvSvcConf import ByteStreamCnvSvc
svcMgr += ByteStreamCnvSvc()
svcMgr.EventPersistencySvc.CnvServices += [ "ByteStreamCnvSvc" ]

# ByteStreamAddressProviderSvc
from ByteStreamCnvSvcBase. ByteStreamCnvSvcBaseConf import ByteStreamAddressProviderSvc
svcMgr += ByteStreamAddressProviderSvc()
ByteStreamAddressProviderSvc = svcMgr.ByteStreamAddressProviderSvc

# specific for tag
from ByteStreamCnvSvc.ByteStreamCnvSvcConf import ByteStreamNavigationProviderSvc
svcMgr += ByteStreamNavigationProviderSvc( "ByteStreamNavigationProviderSvc" )
svcMgr.ProxyProviderSvc.ProviderNames += [ "ByteStreamNavigationProviderSvc" ]

#--------------------------------------------------------------
# Private Application Configuration options
#--------------------------------------------------------------
# Load "user algorithm" top algorithms to be run, and the libraries that house them
from AthenaPoolExampleAlgorithms.AthenaPoolExampleAlgorithmsConf import AthPoolEx__ReadBs
topSequence += AthPoolEx__ReadBs("ReadBs")

from AthenaPoolExampleAlgorithms.AthenaPoolExampleAlgorithmsConf import AthPoolEx__ReadData
topSequence += AthPoolEx__ReadData("ReadData")

#--------------------------------------------------------------
# Set output level threshold (2=DEBUG, 3=INFO, 4=WARNING, 5=ERROR, 6=FATAL)
#--------------------------------------------------------------
svcMgr.MessageSvc.OutputLevel = 3
svcMgr.MessageSvc.defaultLimit = 100000

svcMgr.MetaDataSvc.OutputLevel = 2
svcMgr.EventSelector.OutputLevel = 2
svcMgr.AthenaPoolAddressProviderSvc.OutputLevel = 2
svcMgr.PoolSvc.OutputLevel = 2
topSequence.ReadData.OutputLevel = 2

#
# End of job options file
#
###############################################################
