## @file AthenaPoolExample_ConcatJobOptions.py
## @brief Example job options file to illustrate how to concatinate two write jobs.
## @author Peter van Gemmeren <gemmeren@anl.gov>
## $Id: AthenaPoolExample_ConcatJobOptions.py,v 1.24 2009-03-20 15:30:21 gemmeren Exp $
###############################################################
#
# Job options file
# ------------------------------------------------------------
# Expected output file (20 events):
# -rw-r--r--    1 gemmeren zp          34657 Dec  8 18:58 SimplePoolFile1.root
# -rw-r--r--    1 gemmeren zp          35967 Dec  8 18:58 SimplePoolFile3.root
#==============================================================

## basic (generator) job configuration
import AthenaCommon.AtlasUnixGeneratorJob

## get a handle on the default top-level algorithm sequence
from AthenaCommon.AlgSequence import AlgSequence
topSequence = AlgSequence()
from AthenaCommon.AlgSequence import AthSequencer
outSequence = AthSequencer("AthOutSeq")

## get a handle on the ServiceManager
from AthenaCommon.AppMgr import ServiceMgr as svcMgr

svcMgr.EventSelector.RunNumber = 1

#--------------------------------------------------------------
# Event related parameters
#--------------------------------------------------------------
from AthenaCommon.AppMgr import theApp
theApp.EvtMax = 20

#--------------------------------------------------------------
# Load POOL support
#--------------------------------------------------------------
import AthenaPoolCnvSvc.AthenaPool

#Explicitly specify the output file catalog
svcMgr.PoolSvc.WriteCatalog = "xmlcatalog_file:Catalog1.xml"

#--------------------------------------------------------------
# Private Application Configuration options
#--------------------------------------------------------------
# Load "user algorithm" top algorithms to be run, and the libraries that house them
from AthenaPoolExampleAlgorithms.AthenaPoolExampleAlgorithmsConf import AthPoolEx__WriteData,AthPoolEx__ReWriteData
topSequence += AthPoolEx__WriteData("WriteData")

# Run OutputStream as an algorithm
from AthenaPoolCnvSvc.WriteAthenaPool import AthenaPoolOutputStream
Stream1 = AthenaPoolOutputStream( "Stream1" , "SimplePoolFile1.root" )
Stream1.ItemList += [ "ExampleHitContainer#My*" ]

#--------------------------------------------------------------
# Private Application Configuration options
#--------------------------------------------------------------
from OutputStreamAthenaPool.OutputStreamAthenaPoolConf import MakeInputDataHeader
outSequence += MakeInputDataHeader( StreamName = "Stream1" )
topSequence += AthPoolEx__ReWriteData( "ReWriteData" )

# Run OutputStream as an algorithm
Stream2 = AthenaPoolOutputStream( "Stream2" , "SimplePoolFile3.root" )
Stream2.ItemList += [ "ExampleTrackContainer#*Tracks" ]

# Must make sure that no OutStream's have been declared
theApp.OutStream = []

#--------------------------------------------------------------
# Set output level threshold (2=DEBUG, 3=INFO, 4=WARNING, 5=ERROR, 6=FATAL)
#--------------------------------------------------------------
svcMgr.MessageSvc.OutputLevel = 3
svcMgr.MessageSvc.defaultLimit = 100000

svcMgr.PoolSvc.OutputLevel = 2
svcMgr.AthenaPoolCnvSvc.OutputLevel = 2
topSequence.WriteData.OutputLevel = 2
topSequence.ReWriteData.OutputLevel = 2
Stream1.OutputLevel = 2
Stream1.WritingTool.OutputLevel = 3
Stream1.HelperTools[0].OutputLevel = 3
Stream2.OutputLevel = 2
Stream2.WritingTool.OutputLevel = 3
Stream2.HelperTools[0].OutputLevel = 3

from StoreGate.StoreGateConf import StoreGateSvc
svcMgr += StoreGateSvc()

#
# End of job options file
#
###############################################################
