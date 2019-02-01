## @file BSMetaExample_Write.py
## @brief Example job options file to illustrate how to read event data from Pool.
## $Id: BSMetaExample_Write.py,v 1.35 2009-05-20 18:02:24 gemmeren Exp $
###############################################################
#
# This Job option:
# ----------------
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
# Load ByteStream support
#--------------------------------------------------------------
from AthenaPoolMultiTest.AthenaPoolMultiTestConf import *

PassNoneFilter      = PassNoneFilter("PassNoneFilter")
topSequence += PassNoneFilter

if not hasattr(svcMgr,"ByteStreamCnvSvc"):
   from ByteStreamCnvSvc import ReadByteStream
   # Define the input
   svcMgr.ByteStreamInputSvc.FullFileName = [ "metatest.data" ]
   theApp.ExtSvc += [ "ByteStreamCnvSvc"]

   from ByteStreamCnvSvc import WriteByteStream
   # Define the output
   StreamBSFileOutput = WriteByteStream.getStream("EventStorage","StreamBSFileOutput")
   try:
      os.remove('metatest2.data')
   except OSError:
      pass
   svcMgr.ByteStreamEventStorageOutputSvc.SimpleFileName = "metatest2.data"
   svcMgr.ByteStreamEventStorageOutputSvc.AttributeListKeys = ['RunEventTag']
   StreamBSFileOutput.AcceptAlgs = ["PassNoneFilter"]
   StreamBSFileOutput.OutputLevel = DEBUG

# The piece which does the AttList conversion for WRITING
from ByteStreamCnvSvc.ByteStreamCnvSvcConf import ByteStreamAttListMetadataSvc
svcMgr += ByteStreamAttListMetadataSvc("ByteStreamAttListMetadataSvc")
svcMgr.ByteStreamAttListMetadataSvc.InputStoreName = 'DetectorStore'
svcMgr.ByteStreamAttListMetadataSvc.OutputLevel = DEBUG

## get a handle on the ServiceManager
from AthenaCommon.AppMgr import ServiceMgr as svcMgr

from ByteStreamCnvSvc.ByteStreamCnvSvcConf import ByteStreamMetadataTool
svcMgr.MetaDataSvc.MetaDataTools += [ "ByteStreamMetadataTool" ]

#svcMgr.EventSelector.InputCollections = [ "test_defl.data" ]

#--------------------------------------------------------------
# Private Application Configuration options
#--------------------------------------------------------------
# Load "user algorithm" top algorithms to be run, and the libraries that house them

from AthenaPoolMultiTest.AthenaPoolMultiTestConf import RunEventMetaWriter
attmaker = RunEventMetaWriter("RunEventMetaWriter")
attmaker.OutputLevel = DEBUG
topSequence += attmaker

from AthenaPoolMultiTest.AthenaPoolMultiTestConf import StoreDump
sd = StoreDump("StoreDump")
sd.Key="RunEventTag"
topSequence += sd

#--------------------------------------------------------------
# Set output level threshold (2=DEBUG, 3=INFO, 4=WARNING, 5=ERROR, 6=FATAL)
#--------------------------------------------------------------
svcMgr.MessageSvc.OutputLevel = INFO
svcMgr.MessageSvc.defaultLimit = 100000

svcMgr.MetaDataSvc.OutputLevel = DEBUG
svcMgr.EventSelector.OutputLevel = INFO

#
# End of job options file
#
###############################################################
