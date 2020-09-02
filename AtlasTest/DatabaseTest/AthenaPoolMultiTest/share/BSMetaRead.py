## @file BSMetaExample_Read.py
## @brief Example job options file to illustrate how to read event data from Pool.
## $Id: BSMetaExample_Read.py,v 1.35 2009-05-20 18:02:24 gemmeren Exp $
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
if not hasattr(svcMgr,"ByteStreamCnvSvc"):
   from ByteStreamCnvSvc import ReadByteStream
   # Define the input
   theApp.ExtSvc += [ "ByteStreamCnvSvc"]

svcMgr.ByteStreamCnvSvc.OutputLevel = DEBUG
svcMgr.ByteStreamInputSvc.OutputLevel = DEBUG

## get a handle on the ServiceManager
from AthenaCommon.AppMgr import ServiceMgr as svcMgr

from ByteStreamCnvSvc.ByteStreamCnvSvcConf import ByteStreamMetadataTool
svcMgr.MetaDataSvc.MetaDataTools += [ "ByteStreamMetadataTool" ]

#svcMgr.EventSelector.InputCollections = [ "test_defl.data" ]
svcMgr.EventSelector.Input = [ "metatest.data" ]

#--------------------------------------------------------------
# Private Application Configuration options
#--------------------------------------------------------------
# Load "user algorithm" top algorithms to be run, and the libraries that house them

from AthenaPoolMultiTest.AthenaPoolMultiTestConf import StoreDump
sd = StoreDump("StoreDump")
sd.Key="RunEventTag"
topSequence += sd

#--------------------------------------------------------------
# Set output level threshold (2=DEBUG, 3=INFO, 4=WARNING, 5=ERROR, 6=FATAL)
#--------------------------------------------------------------
svcMgr.MessageSvc.OutputLevel = INFO
svcMgr.MessageSvc.defaultLimit = 100000

svcMgr.MetaDataSvc.OutputLevel = INFO
svcMgr.EventSelector.OutputLevel = DEBUG

#
# End of job options file
#
###############################################################
