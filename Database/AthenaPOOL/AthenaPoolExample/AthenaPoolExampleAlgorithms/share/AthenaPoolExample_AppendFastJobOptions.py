## @file AthenaPoolExample_AppendJobOptions.py
## @brief Example job options file to illustrate how to append event data to an existing Pool file.
## @author Peter van Gemmeren <gemmeren@anl.gov>
## $Id: AthenaPoolExample_AppendJobOptions.py,v 1.16 2008-09-29 15:25:22 gemmeren Exp $
###############################################################
#
# This Job option:
# ----------------
# 1. Updates (appends to) SimplePoolFileB.root file with EventInfo and EventStreamInfo MetaData.
# 2. Appends event TAGs to SimplePoolCollection2.root.
# ------------------------------------------------------------
# Expected output file (20 + 20 events):
# -rw-r--r--  1 gemmeren zp 11408 Aug  5 17:23 SimplePoolCollection2.root
# -rw-r--r--  1 gemmeren zp 31305 Aug  5 17:23 SimplePoolFileB.root
# ------------------------------------------------------------
# File:SimplePoolFileB.root
# Size:       30.571 kb
# Nbr Events: 40
# 
# ================================================================================
#      Mem Size       Disk Size        Size/Evt      MissZip/Mem  items  (X) Container Name (X=Tree|Branch)
# ================================================================================
#       17.616 kb        2.791 kb        0.070 kb        0.000       40  (T) DataHeader
# --------------------------------------------------------------------------------
#        6.089 kb        0.601 kb        0.015 kb        0.182       40  (B) EventInfo_p3_McEventInfo
#        3.454 kb        0.854 kb        0.021 kb        0.465        2  (T) MetaDataHdrDataHeaderForm
#       18.440 kb        1.188 kb        0.030 kb        0.383        1  (B) EventStreamInfo_p2_Stream2
#       18.440 kb        1.188 kb        0.030 kb        0.383        1  (B) EventStreamInfo_p2_Stream2
#       14.169 kb        1.290 kb        0.032 kb        0.113       40  (T) POOLContainer_DataHeaderForm
#       11.495 kb        1.808 kb        0.045 kb        0.349        2  (T) MetaDataHdrDataHeader
# ================================================================================
#       89.704 kb        9.719 kb        0.243 kb        0.000       40  TOTAL (POOL containers)
# ================================================================================
#
#==============================================================

## basic (generator) job configuration
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

svcMgr.EventSelector.RunNumber = 2
#Set first event number to 20 (to continue previous production)
svcMgr.EventSelector.FirstEvent = 20;

#Explicitly specify the output file catalog
svcMgr.PoolSvc.WriteCatalog = "xmlcatalog_file:Catalog1.xml"

#Open file in "update" mode
svcMgr.PoolSvc.FileOpen = "update";

svcMgr.AthenaPoolCnvSvc.CommitInterval = 10;

#--------------------------------------------------------------
# Private Application Configuration options
#--------------------------------------------------------------
# Load "user algorithm" top algorithms to be run, and the libraries that house them
from AthenaPoolExampleAlgorithms.AthenaPoolExampleAlgorithmsConf import AthPoolEx__WriteData
topSequence += AthPoolEx__WriteData("WriteData")

from AthenaPoolExampleAlgorithms.AthenaPoolExampleAlgorithmsConf import AthPoolEx__WriteTag
WriteTag = AthPoolEx__WriteTag( "WriteTag" )
WriteTag.Magic = 1
topSequence += WriteTag

from AthenaPoolCnvSvc.WriteAthenaPool import AthenaPoolOutputStream
Stream2 = AthenaPoolOutputStream ( "Stream2" , "SimplePoolFileB.root" , True, noTag=True )
Stream2.WritingTool.AttributeListKey = "RunEventTag"

#--------------------------------------------------------------
# Event Collection Registration
#--------------------------------------------------------------
from RegistrationServices.RegistrationServicesConf import RegistrationStreamTagTool
TagTool = RegistrationStreamTagTool("TagTool")

from RegistrationServices.RegistrationServicesConf import RegistrationStream
RegStream2 = RegistrationStream( "RegStream2" , CollectionType="ExplicitROOT" , Tool=TagTool )
RegStream2.WriteInputDataHeader = False
RegStream2.OutputCollection = "SimplePoolCollection2.root"
RegStream2.CollectionOpenMode = "UPDATE"
RegStream2.ItemList += [ "DataHeader#Stream2" ]
RegStream2.ItemList += [ "TagAthenaAttributeList#RunEventTag" ]
topSequence += RegStream2

#--------------------------------------------------------------
# Set output level threshold (2=DEBUG, 3=INFO, 4=WARNING, 5=ERROR, 6=FATAL)
#--------------------------------------------------------------
svcMgr.MessageSvc.OutputLevel = 3
svcMgr.PoolSvc.OutputLevel = 2
svcMgr.AthenaPoolCnvSvc.OutputLevel = 2
topSequence.WriteData.OutputLevel = 2
Stream2.OutputLevel = 2
Stream2.WritingTool.OutputLevel = 3
Stream2.HelperTools[0].OutputLevel = 3
RegStream2.OutputLevel = 2
RegStream2.Tool.OutputLevel = 3

#
# End of job options file
#
###############################################################
