## @file AthenaPoolExample_WriteJobOptions.py
## @brief Example job options file to illustrate how to write event data to Pool.
## @author Peter van Gemmeren <gemmeren@anl.gov>
## $Id: AthenaPoolExample_WMetaJobOptions.py,v 1.8 2008-03-25 22:38:54 gemmeren Exp $
###############################################################
#
# This Job option:
# ----------------
# 1. Writes a SimplePoolFile5.root file with ExampleHit
#    using WriteData algorithm
# ------------------------------------------------------------
# Expected output file (20 events):
# -rw-r--r--    1 gemmeren zp          36266 Dec  8 19:08 SimplePoolFile5.root
#
#==============================================================
import AthenaCommon.AtlasUnixGeneratorJob

## get a handle on the default top-level algorithm sequence
from AthenaCommon.AlgSequence import AlgSequence
topSequence = AlgSequence()

## get a handle on the ServiceManager
from AthenaCommon.AppMgr import ServiceMgr as svcMgr

#--------------------------------------------------------------
# Event related parameters
#--------------------------------------------------------------
from AthenaCommon.AppMgr import theApp
theApp.EvtMax = 20

#--------------------------------------------------------------
# Load POOL support
#--------------------------------------------------------------
import AthenaPoolCnvSvc.WriteAthenaPool

#Explicitly specify the output file catalog
svcMgr.PoolSvc.WriteCatalog = "xmlcatalog_file:Catalog2.xml"

from AthenaPoolCnvSvc.WriteAthenaPool import AthenaPoolOutputStream
Stream1 = AthenaPoolOutputStream( "Stream1", "ROOTTREE:SimplePoolFile5.root" )
Stream1.ItemList += [ "ExampleHitContainer#MyHits" ]
Stream1.MetadataItemList += [ "ExampleHitContainer#PedestalWriteData" ]
"""
Stream1.MetadataItemList += [ "EventBookkeeperCollection#EventBookkeepers" ]

from EventBookkeeperTools.BookkeepingInfoWriter import EventBookkeepersWriter
EBWriter1 = EventBookkeepersWriter()
EBWriter1.setDoMC( False )
EBWriter1.setCycle( 0 )

EBWriter1.OutputCollectionName = "EventBookkeepers"
topSequence += EBWriter1
"""

#--------------------------------------------------------------
# Private Application Configuration options
#--------------------------------------------------------------
# Load "user algorithm" top algorithms to be run, and the libraries that house them
from AthenaPoolExampleAlgorithms.AthenaPoolExampleAlgorithmsConf import AthPoolEx__WriteData,AthPoolEx__WriteCond
topSequence += AthPoolEx__WriteData( "WriteData" )
from StoreGate.StoreGateConf import StoreGateSvc
topSequence += AthPoolEx__WriteCond( "WriteCond", DetStore = StoreGateSvc( "MetaDataStore" ),
                                     ConditionName = "PedestalWriteData" )

#--------------------------------------------------------------
# Set output level threshold (2=DEBUG, 3=INFO, 4=WARNING, 5=ERROR, 6=FATAL)
#--------------------------------------------------------------
svcMgr.MessageSvc.OutputLevel = 3
svcMgr.PoolSvc.OutputLevel = 2 
svcMgr.AthenaPoolCnvSvc.OutputLevel = 2
topSequence.WriteData.OutputLevel = 2
Stream1.OutputLevel = 2
Stream1.WritingTool.OutputLevel = 3
Stream1.HelperTools[0].OutputLevel = 3

#
# End of job options file
#
###############################################################
