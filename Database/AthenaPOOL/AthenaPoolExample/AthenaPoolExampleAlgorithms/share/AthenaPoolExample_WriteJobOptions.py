## @file AthenaPoolExample_WriteJobOptions.py
## @brief Example job options file to illustrate how to write event data to Pool.
## @author Peter van Gemmeren <gemmeren@anl.gov>
## $Id: AthenaPoolExample_WriteJobOptions.py,v 1.34 2009-02-02 21:51:22 gemmeren Exp $
###############################################################
#
# This Job option:
# ----------------
# 1. Writes a SimplePoolFile1.root file with ExampleHit DataObjects and in-file MetaData
# 2. Writes another SimplePoolFile2.root file using the AthenaPool support for multiple OutputStreams.
# 3. Writes an additional event-less file EmptyPoolFile.root.
# ------------------------------------------------------------
# Expected output file (20 events):
# -rw-r--r--  1 gemmeren zp 16345 Aug  5 16:56 EmptyPoolFile.root
# -rw-r--r--  1 gemmeren zp 25161 Aug  5 16:56 SimplePoolFile1.root
# -rw-r--r--  1 gemmeren zp 20654 Aug  5 16:56 SimplePoolFile2.root
# ------------------------------------------------------------
# File:SimplePoolFile1.root
# Size:       24.564 kb
# Nbr Events: 20
# 
# ================================================================================
#      Mem Size       Disk Size        Size/Evt      MissZip/Mem  items  (X) Container Name (X=Tree|Branch)
# ================================================================================
#       11.985 kb        1.420 kb        0.071 kb        0.000       20  (T) DataHeader
# --------------------------------------------------------------------------------
#        3.846 kb        0.313 kb        0.016 kb        0.288       20  (B) EventInfo_p3_McEventInfo
#        3.001 kb        0.427 kb        0.021 kb        0.535        1  (T) MetaDataHdrDataHeaderForm
#       10.312 kb        0.743 kb        0.037 kb        0.156       20  (T) POOLContainer_DataHeaderForm
#       10.578 kb        0.902 kb        0.045 kb        0.379        1  (T) MetaDataHdrDataHeader
#       18.451 kb        1.198 kb        0.060 kb        0.383        1  (B) EventStreamInfo_p2_Stream1
#       18.785 kb        2.686 kb        0.134 kb        0.131       20  (B) ExampleHitContainer_p1_MyHits
# ================================================================================
#       76.958 kb        7.689 kb        0.384 kb        0.000       20  TOTAL (POOL containers)
# ================================================================================
#
# File:SimplePoolFile2.root
# Size:       20.269 kb
# Nbr Events: 20
# 
# ================================================================================
#      Mem Size       Disk Size        Size/Evt      MissZip/Mem  items  (X) Container Name (X=Tree|Branch)
# ================================================================================
#       11.634 kb        1.396 kb        0.070 kb        0.000       20  (T) DataHeader
# --------------------------------------------------------------------------------
#        3.846 kb        0.313 kb        0.016 kb        0.288       20  (B) EventInfo_p3_McEventInfo
#        3.001 kb        0.428 kb        0.021 kb        0.535        1  (T) MetaDataHdrDataHeaderForm
#        8.358 kb        0.646 kb        0.032 kb        0.192       20  (T) POOLContainer_DataHeaderForm
#       10.578 kb        0.903 kb        0.045 kb        0.379        1  (T) MetaDataHdrDataHeader
#       18.440 kb        1.188 kb        0.059 kb        0.383        1  (B) EventStreamInfo_p2_Stream2
# ================================================================================
#       55.857 kb        4.873 kb        0.244 kb        0.000       20  TOTAL (POOL containers)
# ================================================================================
#
#==============================================================

## basic job configuration (for generator)
import AthenaCommon.AtlasUnixGeneratorJob

## get a handle on the default top-level algorithm sequence
from AthenaCommon.AlgSequence import AlgSequence
topSequence = AlgSequence()
from AthenaCommon.AlgSequence import AthSequencer
outSequence = AthSequencer("AthOutSeq")

#--------------------------------------------------------------
# Event related parameters
#--------------------------------------------------------------
from AthenaCommon.AppMgr import theApp
theApp.EvtMax = 20

#--------------------------------------------------------------
# Load POOL support
#--------------------------------------------------------------
import AthenaPoolCnvSvc.WriteAthenaPool

## get a handle on the ServiceManager
from AthenaCommon.AppMgr import ServiceMgr as svcMgr

#Explicitly specify the output file catalog
from PoolSvc.PoolSvcConf import PoolSvc
svcMgr += PoolSvc()
svcMgr.PoolSvc.WriteCatalog = "xmlcatalog_file:Catalog1.xml"

from AthenaPoolCnvSvc.AthenaPoolCnvSvcConf import AthenaPoolCnvSvc
svcMgr += AthenaPoolCnvSvc()

svcMgr.EventSelector.RunNumber = 1

#--------------------------------------------------------------
# Private Application Configuration options
#--------------------------------------------------------------
# Load "user algorithm" top algorithms to be run, and the libraries that house them
from AthenaPoolExampleAlgorithms.AthenaPoolExampleAlgorithmsConf import AthPoolEx__WriteData
topSequence += AthPoolEx__WriteData( "WriteData" )

from AthenaPoolExampleAlgorithms.AthenaPoolExampleAlgorithmsConf import AthPoolEx__WriteTag
WriteTag = AthPoolEx__WriteTag( "WriteTag" )
WriteTag.Magic = 1
topSequence += WriteTag
MagicWriteTag = AthPoolEx__WriteTag( "MagicWriteTag" )
MagicWriteTag.Key = "MagicTag"
MagicWriteTag.TagKey = "MagicTag"
MagicWriteTag.Magic = 24
topSequence += MagicWriteTag

from AthenaPoolCnvSvc.WriteAthenaPool import AthenaPoolOutputStream
Stream1 = AthenaPoolOutputStream( "Stream1" , "SimplePoolFile1.root" )
Stream1.ItemList += [ "ExampleHitContainer#MyHits" ]
Stream1.ItemList += [ "ExampleHitContainer#PetersHits" ]
Stream1.WritingTool.AttributeListKey = MagicWriteTag.Key

Stream2 = AthenaPoolOutputStream( "Stream2" , "SimplePoolFile2.root" )
from AthenaCommon.AppMgr import theApp
outStream = theApp.getOutputStream( "Stream2" )
outStream.ItemList += [ "ExampleHitContainer#*" ]
outStream.ExcludeList += [ "ExampleHitContainer#MyHits" ]
outStream.WritingTool.AttributeListKey = "RunEventTag"

from AthenaPoolExampleAlgorithms.AthenaPoolExampleAlgorithmsConf import AthPoolEx__PassNoneFilter
topSequence += AthPoolEx__PassNoneFilter( "PassNoneFilter" )

Stream3 = AthenaPoolOutputStream( "Stream3" , "EmptyPoolFile.root" )
Stream3.RequireAlgs = [ "PassNoneFilter" ]

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
Stream2.OutputLevel = 2
Stream2.WritingTool.OutputLevel = 3
Stream2.HelperTools[0].OutputLevel = 3

#
# End of job options file
#
###############################################################
