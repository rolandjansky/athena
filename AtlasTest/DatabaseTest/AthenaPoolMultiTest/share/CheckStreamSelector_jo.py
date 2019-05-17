## @file CheckExplicit_jo.py
## @brief TopOptions (but dependent SplittableData_jo.py, EventSplit_jo.py):
## Checks that the root collection is readable with the EventSelector
## @author Jack Cranshaw (Jack.Cranshaw@cern.ch)
## $Id: CheckExplicit_jo.py,v 1.9 2009-05-20 13:34:21 cranshaw Exp $
###############################################################
#
# Job options file
#
#==============================================================

#--------------------------------------------------------------
# Event related parameters
#--------------------------------------------------------------
from AthenaCommon.AlgSequence import AthSequencer
topSequence = AthSequencer("AthAlgSeq")
athRegSeq = AthSequencer("AthRegSeq")

from AthenaCommon.AppMgr import theApp

from AthenaCommon.AppMgr import ServiceMgr as svcMgr

theApp.EvtMax = 200000

#--------------------------------------------------------------
# Load POOL support
#--------------------------------------------------------------
include( "AthenaPoolExampleConverter/AthenaPoolExampleConverter_jobOption.py" )
include( "AthenaPoolTestAthenaPool/AthenaPoolTestAthenaPool_joboptions.py" )
#--------------------------------------------------------------
#---   Read portion  -----
#--------------------------------------------------------------
import AthenaPoolCnvSvc.ReadAthenaPool

from PoolSvc.PoolSvcConf import PoolSvc
svcMgr += PoolSvc()
PoolSvc = svcMgr.PoolSvc
PoolSvc.ReadCatalog = ["xmlcatalog_file:EventSplit.xml"]

EventSelector = svcMgr.EventSelector
EventSelector.InputCollections  = [ "AthenaPoolMultiTest_StreamAll2.root" ]; 

# Include the StreamSelectorTool
from EventBookkeeperTools.EventBookkeeperToolsConf import StreamSelectorTool
# Select existing Stream1
selector1 = StreamSelectorTool("Selector1")
selector1.AcceptStreams = ["Stream1"]
selector1.OutputLevel = DEBUG
svcMgr.EventSelector.HelperTools += [selector1]

# Select existing Stream1, but use a separate CutFlowSvc
# Create the CutFlowSvc instance(s)
from EventBookkeeperTools.EventBookkeeperToolsConf import CutFlowSvc
cfs = CfgMgr.CutFlowSvc("SS_CutFlowSvc")
svcMgr += cfs
cfs.InputStream = "StreamAll"
cfs.OutputCollName = "StreamSelect"
cfs.OutputIncompleteCollName = "IncompleteStreamSelect"
#
selector2 = StreamSelectorTool("Selector2")
selector2.AcceptStreams = ["Stream1"]
selector2.CutFlowSvc = cfs
selector2.OutputLevel = DEBUG
svcMgr.EventSelector.HelperTools += [selector2]

# Select non-existing StreamX
selectorx = StreamSelectorTool("SelectorX")
selectorx.AcceptStreams = ["StreamX"]
selectorx.OutputLevel = DEBUG
svcMgr.EventSelector.HelperTools += [selectorx]

#--------------------------------------------------------------
# Configure the filters
#--------------------------------------------------------------
from AthenaPoolExampleAlgorithms.AthenaPoolExampleAlgorithmsConf import AthPoolEx__ReadData

topSequence += AthPoolEx__ReadData("ReadData")
#from AthenaPoolMultiTest.AthenaPoolMultiTestConf import EventTagReader

from AthenaPoolMultiTest.AthenaPoolMultiTestConf import *

#--------------------------------------------------------------
# Set output level threshold (2=DEBUG, 3=INFO, 4=WARNING, 5=ERROR, 6=FATAL)
#--------------------------------------------------------------
svcMgr.MessageSvc.infoLimit = 5000
svcMgr.MessageSvc.debugLimit = 5000
svcMgr.MessageSvc.OutputLevel = INFO
#==============================================================
#
# End of job options file
#
###############################################################
