## @file AthenaPoolExample_RWBsJobOptions.py
## @brief Example job options file to illustrate how to write event data to Pool.
## @author Peter van Gemmeren <gemmeren@anl.gov>
## $Id: $
###############################################################
#
# This Job option:
# ----------------
# 1. Read ByteStream test data file
# 2. Writes a SimplePoolFromRaw.root file without any 'real' data
# ------------------------------------------------------------
# Expected output file (3 events):
# -rw-r--r--  1 gemmeren zp  19470 Nov 25 18:47 SimplePoolFromRaw.root
# ------------------------------------------------------------
# File:SimplePoolFromRaw.root
# Size:       19.014 kb
# Nbr Events: 3
# 
# ================================================================================
#      Mem Size       Disk Size        Size/Evt      MissZip/Mem  items  (X) Container Name (X=Tree|Branch)
# ================================================================================
#        5.722 kb        0.875 kb        0.292 kb        0.000        3  (T) DataHeader
# --------------------------------------------------------------------------------
#        2.180 kb        0.270 kb        0.090 kb        0.523        3  (B) EventInfo_p3_ByteStreamEventInfo
#        3.021 kb        0.436 kb        0.145 kb        0.532        1  (T) MetaDataHdrDataHeaderForm
#        4.051 kb        0.576 kb        0.192 kb        0.397        3  (T) POOLContainer_DataHeaderForm
#        8.606 kb        0.767 kb        0.256 kb        0.395        1  (T) MetaDataHdrDataHeader
#       18.452 kb        1.199 kb        0.400 kb        0.383        1  (B) EventStreamInfo_p2_Stream1
# ================================================================================
#       42.031 kb        4.122 kb        1.374 kb        0.000        3  TOTAL (POOL containers)
# ================================================================================
#
#==============================================================

## basic job configuration
import AthenaCommon.AtlasUnixStandardJob

include( "ByteStreamCnvSvc/BSEventStorageEventSelector_jobOptions.py" )

## get a handle on the ServiceManager
from AthenaCommon.AppMgr import ServiceMgr as svcMgr

## get a handle on the default top-level algorithm sequence
from AthenaCommon.AlgSequence import AlgSequence
topSequence = AlgSequence()

import os
fname = "/afs/cern.ch/atlas/offline/test/daq.m4_combined.0020720.extract.L1TT-b00000010._0001.data"
if not os.path.exists (fname) and os.environ.has_key('ATLAS_REFERENCE_DATA'):
    fname = os.environ['ATLAS_REFERENCE_DATA'] + "/bstest/daq.m4_combined.0020720.extract.L1TT-b00000010._0001.data"
svcMgr.ByteStreamInputSvc.FullFileName = [ fname ]
#svcMgr.ByteStreamInputSvc.FullFileName = [ "test_defl.data" ]

svcMgr.ByteStreamInputSvc.ValidateEvent = True
svcMgr.ByteStreamInputSvc.ProcessBadEvent = True

import AthenaPoolCnvSvc.WriteAthenaPool

#--------------------------------------------------------------
# Private Application Configuration options
#--------------------------------------------------------------
# Load "user algorithm" top algorithms to be run, and the libraries that house them
from AthenaPoolExampleAlgorithms.AthenaPoolExampleAlgorithmsConf import AthPoolEx__ReadBs
topSequence += AthPoolEx__ReadBs("ReadBs")

from AthenaPoolExampleAlgorithms.AthenaPoolExampleAlgorithmsConf import AthPoolEx__ReadData
topSequence += AthPoolEx__ReadData("ReadData")

from AthenaPoolCnvSvc.WriteAthenaPool import AthenaPoolOutputStream
Stream1 = AthenaPoolOutputStream( "Stream1" , "SimplePoolFromRaw.root" , True, noTag=True )
Stream1.MetadataItemList += [ "ByteStreamMetadataContainer#*" ]

from AthenaPoolExampleAlgorithms.AthenaPoolExampleAlgorithmsConf import AthPoolEx__WriteTag
topSequence += AthPoolEx__WriteTag( "WriteTag" )

from RegistrationServices.RegistrationServicesConf import RegistrationStreamTagTool
TagTool = RegistrationStreamTagTool("TagTool")

from RegistrationServices.RegistrationServicesConf import RegistrationStream
RegStream1 = RegistrationStream( "RegStream1" , CollectionType="ExplicitROOT" , Tool=TagTool )
RegStream1.WriteInputDataHeader = False
RegStream1.OutputCollection = "SimpleRawCollection.root"
RegStream1.ItemList += [ "DataHeader#Stream1" ]
RegStream1.ItemList += [ "TagAthenaAttributeList#RunEventTag" ]
topSequence += RegStream1

#--------------------------------------------------------------
# Set output level threshold (2=DEBUG, 3=INFO, 4=WARNING, 5=ERROR, 6=FATAL)
#--------------------------------------------------------------
svcMgr.MessageSvc.OutputLevel = 3
svcMgr.PoolSvc.OutputLevel = 2 
svcMgr.AthenaPoolCnvSvc.OutputLevel = 2
svcMgr.MetaDataSvc.OutputLevel = 2 
topSequence.ReadBs.OutputLevel = 2
topSequence.ReadData.OutputLevel = 2
Stream1.OutputLevel = 2
Stream1.WritingTool.OutputLevel = 3
Stream1.HelperTools[0].OutputLevel = 3
RegStream1.OutputLevel = 2
RegStream1.Tool.OutputLevel = 3

#
# End of job options file
#
###############################################################
