## @file AthenaPoolExample_RCondJobOptions.py
## @brief Example job options file to illustrate how to read conditions data from Pool.
## @author Peter van Gemmeren <gemmeren@anl.gov>
## $Id: AthenaPoolExample_RCondJobOptions.py,v 1.20 2008-04-24 15:42:18 gemmeren Exp $
###############################################################
#
# This Job option:
# ----------------
# 1. Reads the data from the SimplePoolFile1.root file
#    had been written with the AthneaPoolExample_WriteJobOptions.py
# 2. Reads another SimplePoolFile4.root file using ReadCond algorithm
#    which contains conditions for the event sample written by
#    AthneaPoolExample_WCondJobOptions.py
#
#==============================================================

## basic job config
import AthenaCommon.AtlasUnixStandardJob

## get a handle on the default top-level algorithm sequence
from AthenaCommon.AlgSequence import AlgSequence
topSequence = AlgSequence()

## get a handle on the ServiceManager
from AthenaCommon.AppMgr import ServiceMgr as svcMgr

#--------------------------------------------------------------
# Event related parameters
#--------------------------------------------------------------
from AthenaCommon.AppMgr import theApp
theApp.EvtMax = 200000

#--------------------------------------------------------------
# Load POOL support
#--------------------------------------------------------------
import AthenaPoolCnvSvc.ReadAthenaPool

#Explicitly specify the output file catalog
svcMgr.PoolSvc.WriteCatalog = "file:Catalog2.xml"

svcMgr.EventSelector.InputCollections = [ "SimplePoolFile1.root" ] # ** mandatory parameter ** // The input file name

from EventSelectorAthenaPool.EventSelectorAthenaPoolConf import CondProxyProvider
svcMgr += CondProxyProvider()

svcMgr.ProxyProviderSvc.ProviderNames += [ "CondProxyProvider" ]

svcMgr.CondProxyProvider.InputCollections = [ "SimplePoolFile4.root" ]

#Explicitly specify the output file catalog
svcMgr.PoolSvc.ReadCatalog = [ "file:Catalog0.xml" ]
svcMgr.PoolSvc.ReadCatalog += [ "file:Catalog1.xml" ]

#--------------------------------------------------------------
# Private Application Configuration options
#--------------------------------------------------------------
# Load "user algorithm" top algorithms to be run, and the libraries that house them
from AthenaPoolExampleAlgorithms.AthenaPoolExampleAlgorithmsConf import AthPoolEx__ReadData,AthPoolEx__ReadCond
topSequence += AthPoolEx__ReadCond( "ReadCond" )
topSequence += AthPoolEx__ReadData( "ReadData" )

#--------------------------------------------------------------
# Set output level threshold (2=DEBUG, 3=INFO, 4=WARNING, 5=ERROR, 6=FATAL)
#--------------------------------------------------------------
svcMgr.MessageSvc.OutputLevel = 3
svcMgr.EventSelector.OutputLevel = 2
svcMgr.AthenaPoolAddressProviderSvc.OutputLevel = 2
svcMgr.MetaDataSvc.OutputLevel = 2
svcMgr.CondProxyProvider.OutputLevel = 2
svcMgr.PoolSvc.OutputLevel = 2
topSequence.ReadCond.OutputLevel = 2
topSequence.ReadData.OutputLevel = 2

#
# End of job options file
#
###############################################################
