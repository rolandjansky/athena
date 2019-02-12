## @file AthenaPoolExample_WriteJobOptions.py
## @brief Example job options file to illustrate how to write event data to Pool.
## @author Peter van Gemmeren <gemmeren@anl.gov>
## $Id: SplittableData_jo.py,v 1.24 2009-05-01 13:51:40 cranshaw Exp $
###############################################################
#
# This Job option:
# ----------------
# 1. Writes a SimplePoolFile1.root file with ExampleClass, ExampleHit
#    and MyObjContainer DataObjects using WriteData algorithm
# 2. Writes another SimplePoolFile2.root file using the AthenaPool
#    support for multiple OutputStreams.
# ------------------------------------------------------------
# Expected output file (20 events):
# -rw-r--r--    1 gemmeren zp          34657 Dec  8 19:08 SimplePoolFile1.root
# -rw-r--r--    1 gemmeren zp          21872 Dec  8 19:08 SimplePoolFile2.root
#
#==============================================================

## basic job configuration (for generator)
import AthenaCommon.AtlasUnixGeneratorJob

from AthenaCommon.AlgSequence import AthSequencer
topSequence = AthSequencer("AthAlgSeq")
athOutSeq = AthSequencer("AthOutSeq")
athRegSeq = AthSequencer("AthRegSeq")

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

#--------------------------------------------------------------
# Private Application Configuration options
#--------------------------------------------------------------
# Load "user algorithm" top algorithms to be run, and the libraries that house them

from xAODEventInfoCnv.xAODEventInfoCnvConf import xAODMaker__EventInfoCnvAlg
alg = xAODMaker__EventInfoCnvAlg()
topSequence += alg

from AthenaPoolMultiTest.AthenaPoolMultiTestConf import *

PassAllFilter       = PassAllFilter("PassAllFilter")
PassNoneFilter      = PassNoneFilter("PassNoneFilter")
AddTrigMap          = AddTrigMap("AddTrigMap")
#EventTagWriter      = EventTagWriter("EventTagWriter")
#TagEventTagWriter   = TagEventTagWriter("TagEventTagWriter")
DummyLumirangeTool  = DummyLumirangeTool("DummyLumirangeTool")

PassAllFilter.OutputLevel  = WARNING
PassNoneFilter.OutputLevel = WARNING
AddTrigMap.OutputLevel     = INFO
#EventTagWriter.OutputLevel = INFO 
#TagEventTagWriter.OutputLevel = DEBUG
DummyLumirangeTool.OutputLevel = INFO

topSequence        += PassAllFilter
topSequence        += PassNoneFilter
topSequence        += AddTrigMap
#topSequence        += EventTagWriter
#topSequence        += TagEventTagWriter
topSequence        += DummyLumirangeTool

theApp.CreateSvc += ['xAODMaker::EventFormatSvc']

from AthenaPoolExampleAlgorithms.AthenaPoolExampleAlgorithmsConf import AthPoolEx__WriteData
topSequence += AthPoolEx__WriteData("WriteData")

#Explicitly specify the output file catalog
from PoolSvc.PoolSvcConf import PoolSvc
svcMgr += PoolSvc()
svcMgr.PoolSvc.WriteCatalog = "xmlcatalog_file:SplittableData.xml"

from AthenaPoolCnvSvc.AthenaPoolCnvSvcConf import AthenaPoolCnvSvc
svcMgr += AthenaPoolCnvSvc()

include( "AthenaPoolMultiTest/ExampleStreamConfig.py" )

from AthenaPoolCnvSvc.WriteAthenaPool import AthenaPoolOutputStream
DataStream = AthenaPoolOutputStream( "DataStream" , "AthenaPoolMultiTest_Splittable0.root", False, noTag=False )
DataStream.ItemList    = exampleItemList  # comes from ExampleStreamConfig
DataStream.ItemList   += [ "FauxTriggerMap#*" ] # add item not in StreamConfig
DataStream.MetadataItemList   += exampleMetadataList
DataStream.AcceptAlgs  = ["PassAllFilter"]
DataStream.RequireAlgs = ["PassAllFilter"]
DataStream.VetoAlgs    = ["PassNoneFilter"]
DataStream.WritingTool.AttributeListKey="SimpleTagDecisions"

#--------------------------------------------------------------
# Output options
#--------------------------------------------------------------
include( "AthenaPoolExampleConverter/AthenaPoolExampleConverter_jobOption.py" )

#--------------------------------------------------------------
# Set output level threshold (2=DEBUG, 3=INFO, 4=WARNING, 5=ERROR, 6=FATAL)
#--------------------------------------------------------------
svcMgr.MessageSvc.OutputLevel = INFO
svcMgr.PoolSvc.OutputLevel = DEBUG
svcMgr.AthenaPoolCnvSvc.OutputLevel = DEBUG
topSequence.WriteData.OutputLevel = DEBUG
DataStream.OutputLevel = DEBUG

#
# End of job options file
#
###############################################################
