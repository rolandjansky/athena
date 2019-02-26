## @file AthenaPoolExample_WriteJobOptions.py
## @brief Example job options file to illustrate how to write event data to Pool.
## @author Peter van Gemmeren <gemmeren@anl.gov>
## $Id: AthenaPoolExample_WriteJobOptions.py,v 1.34 2009-02-02 21:51:22 gemmeren Exp $
###############################################################
#
# This Job option:
# ----------------
# 1. Creates a direct pers. to pers. copy of SimplePoolFile1.root file with ExampleHit DataObjects
# ------------------------------------------------------------
# Expected output file (20 events):
# -rw-r--r--  1 gemmeren zp 28071 Aug  5 18:59 SimplePoolReplica1.root
# ------------------------------------------------------------
# File:SimplePoolReplica1.root
# Size:       27.413 kb
# Nbr Events: 20
# 
# ================================================================================
#      Mem Size       Disk Size        Size/Evt      MissZip/Mem  items  (X) Container Name (X=Tree|Branch)
# ================================================================================
#       11.985 kb        1.420 kb        0.071 kb        0.000       20  (T) DataHeader
# --------------------------------------------------------------------------------
#        3.846 kb        0.313 kb        0.016 kb        0.288       20  (B) EventInfo_p3_McEventInfo
#        3.001 kb        0.427 kb        0.021 kb        0.535        1  (T) MetaDataHdrDataHeaderForm
#       10.312 kb        0.745 kb        0.037 kb        0.156       20  (T) POOLContainer_DataHeaderForm
#       10.578 kb        0.903 kb        0.045 kb        0.379        1  (T) MetaDataHdrDataHeader
#       18.451 kb        1.198 kb        0.060 kb        0.383        1  (B) EventStreamInfo_p2_Stream1
#       18.785 kb        2.686 kb        0.134 kb        0.131       20  (B) ExampleHitContainer_p1_MyHits
# ================================================================================
#       76.958 kb        7.692 kb        0.385 kb        0.000       20  TOTAL (POOL containers)
# 
# ================================================================================

## basic job configuration
import AthenaCommon.AtlasUnixStandardJob

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

svcMgr.EventSelector.InputCollections = [ "SimplePoolFile1.root" ]

#--------------------------------------------------------------
# Load POOL support
#--------------------------------------------------------------
import AthenaPoolCnvSvc.WriteAthenaPool

#Explicitly specify the output file catalog
from PoolSvc.PoolSvcConf import PoolSvc
svcMgr += PoolSvc()
svcMgr.PoolSvc.WriteCatalog = "xmlcatalog_file:Catalog1.xml"

#--------------------------------------------------------------
# Private Application Configuration options
#--------------------------------------------------------------
# Load "user algorithm" top algorithms to be run, and the libraries that house them

from AthenaPoolCnvSvc.WriteAthenaPool import AthenaPoolOutputStream
Stream1 = AthenaPoolOutputStream( "Stream1" , "SimplePoolReplica1.root" )
Stream1.ItemList += [ "ExampleHitContainer#MyHits" ]
Stream1.ExtendProvenanceRecord = FALSE;
Stream1.ForceRead = True
#Stream1.PersToPers = True

#--------------------------------------------------------------
# Set output level threshold (2=DEBUG, 3=INFO, 4=WARNING, 5=ERROR, 6=FATAL)
#--------------------------------------------------------------
svcMgr.MessageSvc.OutputLevel = 3

svcMgr.EventSelector.OutputLevel = 2
svcMgr.AthenaPoolAddressProviderSvc.OutputLevel = 2
svcMgr.PoolSvc.OutputLevel = 2 
svcMgr.AthenaPoolCnvSvc.OutputLevel = 2
Stream1.OutputLevel = 2
Stream1.WritingTool.OutputLevel = 3
Stream1.HelperTools[0].OutputLevel = 3

#
# End of job options file
#
###############################################################
