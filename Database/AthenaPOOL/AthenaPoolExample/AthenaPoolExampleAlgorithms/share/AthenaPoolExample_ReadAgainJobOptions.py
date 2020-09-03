## @file AthenaPoolExample_ReadJobOptions.py
## @brief Example job options file to illustrate how to read event data from Pool.
## @author Peter van Gemmeren <gemmeren@anl.gov>
## $Id: AthenaPoolExample_ReadJobOptions.py,v 1.35 2009-05-20 18:02:24 gemmeren Exp $
###############################################################
#
# This Job option:
# ----------------
# 1. Reads the data from files have been written with AthneaPoolExample_WriteJobOptions.py
#    and AthneaPoolExample_RWJobOptions.py
# 2. Same as 1., but using TAG collections. Instead of SkipEvents, a HelperTool is used to skip the
#    first 10 events using their MagicTag attribute (example for computational TAG processing).
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

#svcMgr.EventSelector.InputCollections = [ "EmptyPoolFile.root", "SimplePoolReplica1.root", "EmptyPoolFile.root", "SimplePoolFile2.root", "NotTherePoolFile.root", "NotFoundPoolFile.root", "TruncatedPoolFile.root", "SimplePoolFile4.root" ]
svcMgr.EventSelector.InputCollections = [ "EmptyPoolFile.root", "SimplePoolReplica1.root", "EmptyPoolFile.root", "SimplePoolFile2.root", "SimplePoolFile4.root" ]
#svcMgr.EventSelector.InputCollections = [ "EmptyPoolCollection.root", "SimplePoolCollection1.root", "EmptyPoolCollection.root", "SimplePoolCollection2.root", "SimplePoolCollection3.root" ]
svcMgr.EventSelector.SkipEvents = 8; # // skip the first 8 events
svcMgr.EventSelector.SkipEventSequence = [ 9, 10 ]; # // skip two more events
#svcMgr.EventSelector.SkipEventSequence = " 4-6,7 , 8 , 9 - 10 "; # // skip seven more events
#svcMgr.EventSelector.ProcessEventSequence = "11, 12, 13,14,15, 16-20, 21 - 25 , 26- 100"; # // skip two more events
#svcMgr.EventSelector.CollectionType = "ExplicitROOT"
#svcMgr.EventSelector.CounterTool = "EventSelectorCounterTool/Counter"

#Switch Off for TAG - start
# Turn on the tree cache for the CollectionTree - tree cache only works for one tree.
# Set number of events for learning before turning on cache - default is 5
svcMgr.AthenaPoolCnvSvc.InputPoolAttributes += [ "DatabaseName = '*'; TREE_CACHE_LEARN_EVENTS = '6'" ]
# And set tree cache size - default is 10 MB (10 000 000)
svcMgr.AthenaPoolCnvSvc.InputPoolAttributes += [ "DatabaseName = '*'; ContainerName = 'CollectionTree'; TREE_CACHE = '100000'" ]

# Print out values - must have PoolSvc in info mode
svcMgr.AthenaPoolCnvSvc.InputPoolAttributes += [ "DatabaseName = '*'; TREE_CACHE_LEARN_EVENTS = 'int'" ]
svcMgr.AthenaPoolCnvSvc.InputPoolAttributes += [ "DatabaseName = '*'; TREE_CACHE_SIZE = 'int'" ]

# Print out for each event the number of bytes read and the number of
# read calls. With the tree cache, one should see jumps in the bytes
# read by the cache size, i.e. the bytes read should not change each
# event. However, the cache only works on one tree - the main event
# tree (CollectionTree) - and we read some things from other trees, so
# one does see a small increase event-by-event.
svcMgr.AthenaPoolCnvSvc.PrintInputAttrPerEvt += [ "DatabaseName = '*'; BYTES_READ = 'double'" ]
svcMgr.AthenaPoolCnvSvc.PrintInputAttrPerEvt += [ "DatabaseName = '*'; READ_CALLS = 'int'" ]
#Switch Off for TAG - end

from AthenaPoolExampleAlgorithms.AthenaPoolExampleAlgorithmsConf import AthPoolEx__QueryTag
QueryTag = AthPoolEx__QueryTag("QueryTag")
#svcMgr.EventSelector.HelperTools = [ QueryTag ]

#PoolSvc.ReadCatalog = [ "mysqlcatalog_mysql://CollTester:CtAl_204@atlasdbdev.cern.ch:3306/CollectionTest" ]
svcMgr.PoolSvc.WriteCatalog = "file:Catalog1.xml"
#svcMgr.PoolSvc.AttemptCatalogPatch = True;

#--------------------------------------------------------------
# Private Application Configuration options
#--------------------------------------------------------------
# Create xAOD::EventInfo
from xAODEventInfoCnv.xAODEventInfoCreator import xAODMaker__EventInfoCnvAlg
topSequence += xAODMaker__EventInfoCnvAlg("EventInfoCnvAlg")

# Load "user algorithm" top algorithms to be run, and the libraries that house them
from AthenaPoolExampleAlgorithms.AthenaPoolExampleAlgorithmsConf import AthPoolEx__ReadData
topSequence += AthPoolEx__ReadData("ReadData")

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

svcMgr.MetaDataSvc.OutputLevel = 2
svcMgr.EventSelector.OutputLevel = 2
svcMgr.AthenaPoolAddressProviderSvc.OutputLevel = 2
svcMgr.PoolSvc.OutputLevel = 2
topSequence.ReadData.OutputLevel = 2

#
# End of job options file
#
###############################################################
