## @file AthenaPoolExample_RWJobOptions.py
## @brief Example job options file to illustrate how to read/write data from/to Pool.
## @author Peter van Gemmeren <gemmeren@anl.gov>
## $Id: AthenaPoolExample_RWJobOptions.py,v 1.34 2009-03-20 15:30:21 gemmeren Exp $
###############################################################
#
# This Job option:
# ----------------
# 1. Reads the data (EventInfo, ExampleHits) from the SimplePoolReplica1.root file
#    that has been written with the AthneaPoolExample_WriteJobOptions.py
# 2. Writes SimplePoolFile3.root file with ExampleTracks using ReWriteData algorithm
# ------------------------------------------------------------
# Expected output file (20 events):
# -rw-r--r--  1 gemmeren zp 11395 Aug  5 17:34 SimplePoolCollection3.root
# -rw-r--r--  1 gemmeren zp 27536 Aug  5 17:34 SimplePoolFile3.root
# ------------------------------------------------------------
# File:SimplePoolFile3.root
# Size:       26.891 kb
# Nbr Events: 20
# 
# ================================================================================
#      Mem Size       Disk Size        Size/Evt      MissZip/Mem  items  (X) Container Name (X=Tree|Branch)
# ================================================================================
#       12.317 kb        1.429 kb        0.071 kb        0.000       20  (T) DataHeader
# --------------------------------------------------------------------------------
#        3.846 kb        0.313 kb        0.016 kb        0.288       20  (B) EventInfo_p3_McEventInfo
#        3.001 kb        0.428 kb        0.021 kb        0.535        1  (T) MetaDataHdrDataHeaderForm
#       11.894 kb        0.794 kb        0.040 kb        0.135       20  (T) POOLContainer_DataHeaderForm
#       10.578 kb        0.903 kb        0.045 kb        0.379        1  (T) MetaDataHdrDataHeader
#       18.453 kb        1.200 kb        0.060 kb        0.383        1  (B) EventStreamInfo_p2_Stream1
#       49.426 kb        3.881 kb        0.194 kb        0.261       20  (B) ExampleTrackContainer_p1_MyTracks
# ================================================================================
#      109.515 kb        8.948 kb        0.447 kb        0.000       20  TOTAL (POOL containers)
# ================================================================================
#
#==============================================================

## basic job configuration
import AthenaCommon.AtlasUnixStandardJob

## get a handle on the default top-level algorithm sequence
from AthenaCommon.AlgSequence import AlgSequence
topSequence = AlgSequence()
from AthenaCommon.AlgSequence import AthSequencer
regSequence = AthSequencer("AthRegSeq")

## get a handle on the ServiceManager
from AthenaCommon.AppMgr import ServiceMgr as svcMgr

#--------------------------------------------------------------
# Event related parameters
#--------------------------------------------------------------
from AthenaCommon.AppMgr import theApp
theApp.EvtMax = 200000

#--------------------------------------------------------------
#---   Read portion  -----
#--------------------------------------------------------------
import AthenaPoolCnvSvc.ReadAthenaPool

svcMgr.EventSelector.InputCollections = [ "SimplePoolReplica1.root" ]; # ** mandatory parameter ** // The input file name

#Explicitly specify the output file catalog
svcMgr.PoolSvc.ReadCatalog = [ "file:Catalog.xml" ]
svcMgr.PoolSvc.WriteCatalog = "file:Catalog1.xml"

#--------------------------------------------------------------
# Private Application Configuration options
#--------------------------------------------------------------
# Load "user algorithm" top algorithms to be run, and the libraries that house them
from AthenaPoolExampleAlgorithms.AthenaPoolExampleAlgorithmsConf import AthPoolEx__ReadData,AthPoolEx__ReWriteData
topSequence += AthPoolEx__ReadData("ReadData")
topSequence += AthPoolEx__ReWriteData("ReWriteData")

from AthenaPoolExampleAlgorithms.AthenaPoolExampleAlgorithmsConf import AthPoolEx__WriteTag
topSequence += AthPoolEx__WriteTag("WriteTag")
MagicWriteTag = AthPoolEx__WriteTag( "MagicWriteTag" )
MagicWriteTag.Key = "MagicTag"
MagicWriteTag.TagKey = "MagicTag"
MagicWriteTag.Magic = 24
topSequence += MagicWriteTag

#--------------------------------------------------------------
#---   Secondary Write portion  ----- Don't change it !!!
#--------------------------------------------------------------
from AthenaPoolCnvSvc.WriteAthenaPool import AthenaPoolOutputStream
Stream1 = AthenaPoolOutputStream( "Stream1" , "SimplePoolFile3.root" )
Stream1.ItemList += [ "ExampleTrackContainer#MyTracks" ]
#Stream1.ExtendProvenanceRecord = FALSE;
#Stream1.WritingTool.ProvenanceTags = []
Stream1.WritingTool.AttributeListKey = MagicWriteTag.Key

#--------------------------------------------------------------
# ROOT streaming service
#--------------------------------------------------------------
from AthenaPoolServices.AthenaRootStreamerSvcConf import AthenaRootStreamerSvc
StreamerSvc = AthenaRootStreamerSvc()
StreamerSvc.Streamers  += [ "ExampleHitStreamer_p0" ]
svcMgr += StreamerSvc

#--------------------------------------------------------------
# Event Collection Registration
#--------------------------------------------------------------
from RegistrationServices.RegistrationServicesConf import RegistrationStreamTagTool
TagTool = RegistrationStreamTagTool("TagTool")

from RegistrationServices.RegistrationServicesConf import RegistrationStream
RegStream1 = RegistrationStream( "RegStream1" , CollectionType="ExplicitROOT" , Tool=TagTool )
RegStream1.WriteInputDataHeader = False
RegStream1.OutputCollection = "SimplePoolCollection3.root"
RegStream1.ItemList += [ "DataHeader#Stream1" ]
RegStream1.ItemList += [ "TagAthenaAttributeList#" + MagicWriteTag.Key ]
regSequence += RegStream1

#--------------------------------------------------------------
# Set output level threshold (2=DEBUG, 3=INFO, 4=WARNING, 5=ERROR, 6=FATAL)
#--------------------------------------------------------------
svcMgr.MessageSvc.OutputLevel = 3
svcMgr.MessageSvc.defaultLimit = 100000

svcMgr.EventSelector.OutputLevel = 2
svcMgr.AthenaPoolAddressProviderSvc.OutputLevel = 2
svcMgr.MetaDataSvc.OutputLevel = 2
svcMgr.PoolSvc.OutputLevel = 2
svcMgr.AthenaPoolCnvSvc.OutputLevel = 2
topSequence.ReadData.OutputLevel = 2
topSequence.ReWriteData.OutputLevel = 2
Stream1.OutputLevel = 2
Stream1.WritingTool.OutputLevel = 3
Stream1.HelperTools[0].OutputLevel = 3
RegStream1.OutputLevel = 2
RegStream1.Tool.OutputLevel = 3

#
# End of job options file
#
###############################################################
