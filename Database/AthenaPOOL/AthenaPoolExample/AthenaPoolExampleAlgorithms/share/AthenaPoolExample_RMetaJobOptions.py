## @file AthenaPoolExample_ReadJobOptions.py
## @brief Example job options file to illustrate how to read event data from Pool.
## @author Peter van Gemmeren <gemmeren@anl.gov>
## $Id: AthenaPoolExample_RMetaJobOptions.py,v 1.9 2009-03-20 15:30:21 gemmeren Exp $
###############################################################
#
# This Job option:
# ----------------
# 1. Reads the data from the SimplePoolFile5.root and SimplePoolFile2.root files
#    that have been written with the AthneaPoolExample_WriteJobOptions.py
# 2. Reads the data from the SimplePoolFile3.root file
#    that has been written with the AthneaPoolExample_RWJobOptions.py
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

from AthenaPoolExampleAlgorithms.AthenaPoolExampleAlgorithmsConf import AthPoolEx__ReadMeta
svcMgr.MetaDataSvc.MetaDataTools += [ "AthPoolEx::ReadMeta" ]

"""
from EventBookkeeperTools.EventBookkeeperToolsConf import EventBookkeeperMetaDataTool
svcMgr.MetaDataSvc.MetaDataTools += [ "EventBookkeeperMetaDataTool" ]
"""

svcMgr.EventSelector.InputCollections = [ "SimplePoolFile5.root" ]

#PoolSvc.ReadCatalog = [ "mysqlcatalog_mysql://CollTester:CtAl_204@atlasdbdev.cern.ch:3306/CollectionTest" ]
svcMgr.PoolSvc.ReadCatalog += [ "xmlcatalog_file:Catalog2.xml" ]


#--------------------------------------------------------------
# Private Application Configuration options
#--------------------------------------------------------------
# Load "user algorithm" top algorithms to be run, and the libraries that house them
from AthenaPoolExampleAlgorithms.AthenaPoolExampleAlgorithmsConf import AthPoolEx__ReadData
topSequence += AthPoolEx__ReadData( "ReadData" )

#--------------------------------------------------------------
# ROOT streaming service
#--------------------------------------------------------------
from AthenaPoolServices.AthenaRootStreamerSvcConf import AthenaRootStreamerSvc
StreamerSvc = AthenaRootStreamerSvc()
StreamerSvc.Streamers  += [ "ExampleHitStreamer_p0" ]
svcMgr += StreamerSvc

#--------------------------------------------------------------
# Set output level threshold (2=DEBUG, 3=INFO, 4=WARNING, 5=ERROR, 6=FATAL)
#--------------------------------------------------------------
svcMgr.MessageSvc.OutputLevel = 3
svcMgr.MessageSvc.defaultLimit = 100000

svcMgr.EventSelector.OutputLevel = 2
svcMgr.AthenaPoolAddressProviderSvc.OutputLevel = 2
svcMgr.MetaDataSvc.OutputLevel = 2
svcMgr.PoolSvc.OutputLevel = 2
topSequence.ReadData.OutputLevel = 2

#
# End of job options file
#
###############################################################
