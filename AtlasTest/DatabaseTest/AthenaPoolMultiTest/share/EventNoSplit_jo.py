#####################################################################
#
## @file EventNoSplit_jo.py
## @brief
## Second part of AthenaPoolMultiTest gmake check
##<br><br>
## Uses EventSplit algorithm, therefore needs file with FauxTriggerMap
##      object in event.
## Input:  Implicit collection from SplittableData_jo<br>
## Output: 5 output streams<br>
##         8 root collections<br>
## @author JackCranshaw (Jack.Cranshaw@cern.ch)
## $Id: EventNoSplit_jo.py,v 1.27 2009-05-20 13:34:21 cranshaw Exp $
#====================================================================

#--------------------------------------------------------------
# Event related parameters
#--------------------------------------------------------------
from AthenaCommon.AlgSequence import AthSequencer
topSequence = AthSequencer("AthAlgSeq")
athOutSeq = AthSequencer("AthOutSeq")

from xAODEventInfoCnv.xAODEventInfoCnvConf import xAODMaker__EventInfoCnvAlg
alg = xAODMaker__EventInfoCnvAlg()
topSequence += alg


from AthenaCommon.AppMgr import theApp
theApp.EvtMax = 200000

#--------------------------------------------------------------
# Load POOL support
#--------------------------------------------------------------
import AthenaPoolCnvSvc.ReadAthenaPool

from AthenaCommon.AppMgr import ServiceMgr as svcMgr

# If you need to change or add input file catalogs
from PoolSvc.PoolSvcConf import PoolSvc
svcMgr += PoolSvc()
svcMgr.PoolSvc.ReadCatalog = [ "XMLFileCatalog_file:SplittableData.xml" ]
# Or if you need to change the name of the output file catalog
svcMgr.PoolSvc.WriteCatalog = "XMLFileCatalog_file:EventSplit.xml"
 
svcMgr.EventSelector.InputCollections =  ["AthenaPoolMultiTest_Splittable0.root"]
svcMgr.AthenaPoolAddressProviderSvc.DataHeaderIterator = False

#--------------------------------------------------------------
# Private Application Configuration options
#--------------------------------------------------------------
# include stream transform
include( "AthenaPoolMultiTest/ExampleStreamConfig.py" )

#--------------------------------------------------------------
# Configure the filters
#--------------------------------------------------------------
#theApp.TopAlg += [ "ReadData" ]  # print out some data to chk its there
from AthenaPoolMultiTest.AthenaPoolMultiTestConf import EventSplit

Splitter1 = EventSplit("Splitter1")  # Accept bit 1
Splitter1.L1bitmask = 1 
Splitter1.OutputLevel = DEBUG 
Splitter2 = EventSplit("Splitter2")  # Accept bit 2
Splitter2.L1bitmask = 2 
Splitter2.OutputLevel = INFO
Splitter3 = EventSplit("Splitter3")  # Accept bit 3
Splitter3.L1bitmask = 4 
Splitter3.OutputLevel = DEBUG
Triggered = EventSplit("Triggered")   # Accept all bits
Triggered.L1bitmask = 65535   # assume -1 will set all bits on
Triggered.OutputLevel = INFO

topSequence+=Splitter1
topSequence+=Splitter2
topSequence+=Splitter3
topSequence+=Triggered

#--------------------------------------------------------------
#---  Set up the streams for the filters
#     Note that this uses a cascading exclusive stream model
#     for the outstreams and uses inclusive only for the collections.
#     Cascading means that the first stream in the list for which an 
#     event qualifies gets it. Another exclusive model that has been 
#     explored puts all multistream events in a single overlap stream.
#--------------------------------------------------------------

from AthenaPoolCnvSvc.WriteAthenaPool import AthenaPoolOutputStream

ToolSvc = Service( "ToolSvc" )

# Filtered stream 2
Stream2 = AthenaPoolOutputStream( "Stream2", "AthenaPoolMultiTest_NoSplit2.root", False, noTag=False )
Stream2.CheckNumberOfWrites = False
# Filtered stream 1
Stream1 = AthenaPoolOutputStream( "Stream1", "AthenaPoolMultiTest_NoSplit1.root", False, noTag=False )
Stream1.WritingTool.AttributeListKey="SimpleTagDecisions"
Stream1.CheckNumberOfWrites = False
# Filtered stream 3
Stream3 = AthenaPoolOutputStream( "Stream3", "AthenaPoolMultiTest_NoSplit3.root", False, noTag=False )
Stream3.CheckNumberOfWrites = False
# Unfiltered stream
StreamAll = AthenaPoolOutputStream( "StreamAll", "AthenaPoolMultiTest_StreamAll.root", False, noTag=False )
StreamAll.CheckNumberOfWrites = False
StreamAll.WritingTool.AttributeListKey="SimpleTagDecisions"

# Configure them using filter methods and itemlist
# Must make sure that no OutStream's have been declared
#theApp.OutStream    = [];

# bit 2
Stream2.TakeItemsFromInput = True
Stream2.ForceRead=TRUE
Stream2.AcceptAlgs  = ["Splitter2"]
Stream2.VetoAlgs    = ["Splitter1"]
# bit 1
Stream1.TakeItemsFromInput = True
Stream1.ForceRead=TRUE
Stream1.AcceptAlgs = ["Splitter1"]
# bit 3
Stream3.TakeItemsFromInput = True
Stream3.ForceRead=TRUE
Stream3.AcceptAlgs = ["Splitter3"]
Stream3.VetoAlgs   = ["Splitter2"]
# Unfiltered
StreamAll.TakeItemsFromInput = True
StreamAll.ForceRead=TRUE

# Add the outputstreams to the execution sequence
athOutSeq+=Stream1
athOutSeq+=Stream2
athOutSeq+=Stream3

for stream in athOutSeq:
  stream.WriteOnExecute = False
  stream.MetadataItemList = []

athOutSeq+=StreamAll

#--------------------------------------------------------------
# Set output level threshold (2=DEBUG, 3=INFO, 4=WARNING, 5=ERROR, 6=FATAL)
#--------------------------------------------------------------
svcMgr.MessageSvc.OutputLevel = INFO
svcMgr.MessageSvc.debugLimit = 5000
import AthenaCommon.CfgMgr as CfgMgr
if not hasattr(svcMgr, 'DecisionSvc'): svcMgr += CfgMgr.DecisionSvc()
svcMgr.DecisionSvc.CalcStats = True
svcMgr.DecisionSvc.SaveDecisions = True
if not hasattr(svcMgr, 'ItemListSvc'): svcMgr += CfgMgr.ItemListSvc()
svcMgr.ItemListSvc.OutputLevel = DEBUG
#svcMgr.DecisionSvc.OutputLevel = VERBOSE
#--------------------------------------------------------------
#  To restrict the POOL messages one has to set the POOL_OUTMSG_LEVEL env in advance.
#  (see: http://savannah.cern.ch/support/?func=detailsupport&support_id=100139&group_id=25)
#  -----------
#  For example:
#  -----------
#   > setenv POOL_OUTMSG_LEVEL <custom_POOL_output_level>
#   > athena AthenaPoolDExample_ReadJobOptions.txt
#  -----------
#==============================================================
#
# End of job options file
#
###############################################################
