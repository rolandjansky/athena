#####################################################################
#
## @file EventSplit_jo.py
## @brief
## Second part of AthenaPoolMultiTest gmake check
##<br><br>
## Uses EventSplit algorithm, therefore needs file with FauxTriggerMap
##      object in event.
## Input:  Implicit collection from SplittableData_jo<br>
## Output: 5 output streams<br>
##         8 root collections<br>
## @author JackCranshaw (Jack.Cranshaw@cern.ch)
## $Id: EventSplit_jo.py,v 1.27 2009-05-20 13:34:21 cranshaw Exp $
#====================================================================

#--------------------------------------------------------------
# Event related parameters
#--------------------------------------------------------------
from AthenaCommon.AlgSequence import AthSequencer
topSequence = AthSequencer("AthAlgSeq")
athOutSeq = AthSequencer("AthOutSeq")
athRegSeq = AthSequencer("AthRegSeq")

from xAODEventInfoCnv.xAODEventInfoCnvConf import xAODMaker__EventInfoCnvAlg
alg = xAODMaker__EventInfoCnvAlg()
topSequence += alg


from AthenaCommon.AppMgr import theApp
theApp.EvtMax = 200000

#--------------------------------------------------------------
# Load POOL support
#--------------------------------------------------------------
import AthenaPoolCnvSvc.ReadAthenaPool
#include( "AthenaPoolExampleConverter/AthenaPoolExampleConverter_jobOption.py" )
#include( "AthenaPoolTestAthenaPool/AthenaPoolTestAthenaPool_joboptions.py" )

from AthenaCommon.AppMgr import ServiceMgr as svcMgr

# If you need to change or add input file catalogs
from PoolSvc.PoolSvcConf import PoolSvc
svcMgr += PoolSvc()
svcMgr.PoolSvc.ReadCatalog = [ "XMLFileCatalog_file:SplittableData.xml" ]
# Or if you need to change the name of the output file catalog
svcMgr.PoolSvc.WriteCatalog = "XMLFileCatalog_file:EventSplit.xml"
#PoolSvc.FileOpen = "update"
 
#svcMgr.EventSelector.InputCollections =  ["AthenaPoolMultiTest_Splittable0.root"]; # The input file name
svcMgr.EventSelector.InputCollections =  ["PFN:SplittableCollection.root"]; # The input file name
svcMgr.EventSelector.CollectionType = "ExplicitROOT"
svcMgr.AthenaPoolAddressProviderSvc.DataHeaderIterator = False

#--------------------------------------------------------------
# Private Application Configuration options
#--------------------------------------------------------------
# include stream transform
include( "AthenaPoolMultiTest/ExampleStreamConfig.py" )

#--------------------------------------------------------------
# Add the tag object ** needed to write collections at all **
#--------------------------------------------------------------
from AthenaPoolMultiTest.AthenaPoolMultiTestConf import EventTagWriter

EventTagWriter             = EventTagWriter("EventTagWriter")
EventTagWriter.OutputLevel = INFO
topSequence += EventTagWriter

from AthenaPoolMultiTest.AthenaPoolMultiTestConf import RunEventTagWriter

RunEventTagWriter             = RunEventTagWriter("RunEventTagWriter")
#RunEventTagWriter.OutputLevel = DEBUG
topSequence += RunEventTagWriter

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
Splitter456 = EventSplit("Splitter456")  # Accept bits 4,5,6
Splitter456.L1bitmask = 8
Splitter456.L1bitmask += 16
Splitter456.L1bitmask += 32
Splitter456.OutputLevel = INFO
Splitter7 = EventSplit("Splitter7")  # Accept bit 7
Splitter7.L1bitmask = 64
Splitter7.OutputLevel = INFO
Splitter48 = EventSplit("Splitter48") # Accept bits 4,8
Splitter48.L1bitmask = 136
Splitter48.OutputLevel = INFO
Triggered = EventSplit("Triggered")   # Accept all bits
Triggered.L1bitmask = 65535   # assume -1 will set all bits on
Triggered.OutputLevel = INFO

topSequence+=Splitter1
topSequence+=Splitter2
topSequence+=Splitter3
topSequence+=Splitter456
topSequence+=Splitter7
topSequence+=Splitter48
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

# We use 5 test output streams
# Define them
Stream2 = AthenaPoolOutputStream( "Stream2", "AthenaPoolMultiTest_Split2.root", True, noTag=True )
Stream2.CheckNumberOfWrites = False
Stream1 = AthenaPoolOutputStream( "Stream1", "AthenaPoolMultiTest_Split1.root", True, noTag=True )
Stream1.CheckNumberOfWrites = False
Stream3 = AthenaPoolOutputStream( "Stream3", "AthenaPoolMultiTest_Split3.root", True, noTag=True )
Stream3.CheckNumberOfWrites = False
Others  = AthenaPoolOutputStream( "Others", "AthenaPoolMultiTest_Missed.root", True, noTag=True )
Others.CheckNumberOfWrites = False
Bad     = AthenaPoolOutputStream( "Bad", "AthenaPoolMultiTest_Missed.root", True, noTag=True )
Bad.CheckNumberOfWrites = False

# Configure them using filter methods and itemlist
# Must make sure that no OutStream's have been declared
theApp.OutStream    = [];

# bit 2
Stream2.TakeItemsFromInput = True
#Stream2.MetadataItemList   += exampleMetadataList
Stream2.ForceRead=TRUE
Stream2.AcceptAlgs  = ["Splitter2"]
Stream2.VetoAlgs    = ["Splitter1"]
# bit 1
Stream1.TakeItemsFromInput = True
#Stream1.MetadataItemList   += exampleMetadataList
Stream1.ForceRead=TRUE
Stream1.AcceptAlgs = ["Splitter1"]
# bit 3
Stream3.TakeItemsFromInput = True
#Stream3.MetadataItemList   += exampleMetadataList
Stream3.ForceRead=TRUE
Stream3.AcceptAlgs = ["Splitter3"]
Stream3.VetoAlgs   = ["Splitter1"]
Stream3.VetoAlgs  += ["Splitter2"]
# missed
Others.TakeItemsFromInput = True
Others.ForceRead=TRUE
Others.ExcludeList = ['FauxTriggerMap#ExcludeTestTrigMap']
Others.AcceptAlgs = ["Triggered"]
Others.VetoAlgs   = ["Splitter1"]
Others.VetoAlgs  += ["Splitter2"]
Others.VetoAlgs  += ["Splitter3"]
# corrupted
Bad.TakeItemsFromInput = True
Bad.VetoAlgs   = ["Triggered"]

# Add the outputstreams to the execution sequence
athOutSeq+=Stream1
athOutSeq+=Stream2
athOutSeq+=Stream3
athOutSeq+=Others
athOutSeq+=Bad

#--------------------------------------------------------------
# Now do collections
#--------------------------------------------------------------
# Explicit (i.e. not just files) collections (ROOT)
#
from RegistrationServices.RegistrationServicesConf import RegistrationStream
from RegistrationServices.RegistrationServicesConf import RegistrationStreamLCGTool
from RegistrationServices.RegistrationServicesConf import RegistrationStreamTagTool

TagTool = RegistrationStreamTagTool("TagTool")
TagTool.OutputLevel = INFO


# bit 1
CollBit1                  = RegistrationStream("CollBit1")
CollBit1.CollectionType = "ExplicitROOT"
#CollBit1.OutputCollection = "AthenaPoolMultiTest_Split1"; # The output file name
CollBit1.OutputCollection = "Collection_Split1.root"
CollBit1.ItemList        += [ "DataHeader#*" ]
CollBit1.ItemList        += [ "AthenaAttributeList#SimpleTag" ]
CollBit1.ItemList        += [ "CollectionMetadataContainer#*" ]
CollBit1.OutputLevel      = INFO
CollBit1.AcceptAlgs       = ["Splitter1"]
#CollBit1.CollectionOpenMode = "UPDATE"

# bit ( 2 | 3 )
Coll23 = RegistrationStream("Coll23")
Coll23.CollectionType = "ExplicitROOT"
Coll23.OutputCollection = "Collection_Split23.root"
Coll23.ItemList        += [ "DataHeader#*" ]
Coll23.ItemList        += [ "AthenaAttributeList#SimpleTag" ]
Coll23.OutputLevel      = INFO
Coll23.AcceptAlgs       = ["Splitter2"]
Coll23.AcceptAlgs      += ["Splitter3"]
Coll23.WriteAllProv     = False
Coll23.IncludeProvStages = ["NonExisting"]
Coll23.ExcludeProvStages = ["Stream1"]

# bit ( 4 | 5 | 6 )
Coll456 = RegistrationStream("Coll456")
Coll456.CollectionType = "ExplicitROOT"
Coll456.OutputCollection = "Collection_Split456.root"
Coll456.ItemList        += [ "DataHeader#*" ]
Coll456.ItemList        += [ "AthenaAttributeList#SimpleTag" ]
Coll456.OutputLevel      = INFO
Coll456.AcceptAlgs       = ["Splitter456"]

# not ( 1 & 2 & 3 & 4 & 5 & 6 )   complement of previous 3
CollBar = RegistrationStream("CollBar")
CollBar.CollectionType = "ExplicitROOT"
CollBar.OutputCollection = "Collection_SplitBar.root"
#CollBar.Tool = TagTool
CollBar.ItemList        += [ "DataHeader#*" ]
CollBar.ItemList        += [ "AthenaAttributeList#RunEventTag" ]
CollBar.OutputLevel      = INFO
CollBar.VetoAlgs         = ["Splitter1"]
CollBar.VetoAlgs        += ["Splitter2"]
CollBar.VetoAlgs        += ["Splitter3"]
CollBar.VetoAlgs        += ["Splitter456"]

#--------------------------------------------------------------
# Overlapping collections
#

# bit ( 1 & 7 )
Coll1and7 = RegistrationStream("Coll1and7")
Coll1and7.CollectionType = "ExplicitROOT"
Coll1and7.OutputCollection = "Collection_Split1plus7"
Coll1and7.ItemList        += [ "DataHeader#*" ]
Coll1and7.ItemList        += [ "AthenaAttributeList#SimpleTag" ]
Coll1and7.OutputLevel      = INFO
Coll1and7.RequireAlgs      = ["Splitter1"]
Coll1and7.RequireAlgs     += ["Splitter7"]

# bit ( 3 | 4 | 8 )
Coll348 = RegistrationStream("Coll348")
Coll348.CollectionType = "ExplicitROOT"
Coll348.OutputCollection = "Collection_Split348.root"
Coll348.ItemList        += [ "DataHeader#*" ]
Coll348.ItemList        += [ "AthenaAttributeList#SimpleTag" ]
Coll348.OutputLevel      = INFO
Coll348.AcceptAlgs       = ["Splitter48"]
Coll348.AcceptAlgs      += ["Splitter3"]
#Coll348.CollectionOpenMode = "UPDATE"

#--------------------------------------------------------------
# Complete collection
#
# All bits, but with valid trigger info
CTTool = RegistrationStreamLCGTool("CollTrigTool")
CollTrig = RegistrationStream("CollTrig")
CollTrig.CollectionType = "ExplicitROOT"
CollTrig.OutputCollection = "Collection_SplitTrig.root"
CollTrig.ItemList        += [ "DataHeader#*" ]
CollTrig.ItemList        += [ "AthenaAttributeList#SimpleTag" ]
CollTrig.AcceptAlgs       = ["Triggered"]
#CollTrig.OutputLevel = DEBUG
CollTrig.Tool = CTTool

# not Trigger, i.e. corrupted
CollBad = RegistrationStream("CollBad")
CollBad.CollectionType = "ExplicitROOT"
CollBad.OutputCollection = "Collection_SplitBad.root"
CollBad.WriteInputDataHeader = TRUE
CollBad.ItemList        += [ "DataHeader#EventSelector" ]
CollBad.ItemList        += [ "AthenaAttributeList#SimpleTag" ]
CollBad.OutputLevel      = INFO
CollBad.VetoAlgs         = ["Triggered"]

athRegSeq+=CollBit1
athRegSeq+=Coll23
athRegSeq+=Coll456
athRegSeq+=CollBar
athRegSeq+=Coll1and7
athRegSeq+=Coll348
athRegSeq+=CollTrig
athRegSeq+=CollBad

#--------------------------------------------------------------
# Set output level threshold (2=DEBUG, 3=INFO, 4=WARNING, 5=ERROR, 6=FATAL)
#--------------------------------------------------------------
svcMgr.MessageSvc.OutputLevel = INFO
svcMgr.MessageSvc.debugLimit = 5000
import AthenaCommon.CfgMgr as CfgMgr
if not hasattr(svcMgr, 'DecisionSvc'): svcMgr += CfgMgr.DecisionSvc()
svcMgr.DecisionSvc.CalcStats = True
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
