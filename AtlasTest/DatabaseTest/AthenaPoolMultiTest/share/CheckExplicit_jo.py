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
EventSelector.CollectionType = "ExplicitROOT"
EventSelector.InputCollections  = [ "PFN:Collection_Split1.root" ]; 
EventSelector.InputCollections += [ "PFN:Collection_Split23.root" ]; 
EventSelector.InputCollections += [ "PFN:Collection_Split456.root" ];
EventSelector.InputCollections += [ "PFN:Collection_Split348.root" ]; 
EventSelector.InputCollections += [ "PFN:Collection_SplitBar.root" ]; 
EventSelector.InputCollections += [ "PFN:Collection_SplitTrig.root" ]; 
EventSelector.InputCollections += [ "Collection_Split1plus7" ];

#--------------------------------------------------------------
# Configure the filters
#--------------------------------------------------------------
from AthenaPoolExampleAlgorithms.AthenaPoolExampleAlgorithmsConf import AthPoolEx__ReadData

topSequence += AthPoolEx__ReadData("ReadData")
#from AthenaPoolMultiTest.AthenaPoolMultiTestConf import EventTagReader

from AthenaPoolMultiTest.AthenaPoolMultiTestConf import *

RunEventTagWriter   = RunEventTagWriter("RunEventTagWriter")
#RunEventTagWriter.OutputLevel = DEBUG

topSequence        += RunEventTagWriter

from RegistrationServices.RegistrationServicesConf import InputCollectionMetadataCopy
from RegistrationServices.RegistrationServicesConf import RegistrationStreamDefMeta
from RegistrationServices.RegistrationServicesConf import RegistrationStream
from RegistrationServices.RegistrationServicesConf import RegistrationStreamTagTool

TagTool = RegistrationStreamTagTool("TagTool")
#TagTool.PrimaryKey = ['RunNumber','EventNumber']

# Add tool for copying input collection metadata
copyTool = InputCollectionMetadataCopy("copyTool")
copyTool.OutputLevel = DEBUG
ToolSvc += copyTool

# Add algorithm to add default collection metadata
DefaultCollMeta = RegistrationStreamDefMeta("DefaultCollMeta")
DefaultCollMeta.Project = "AthenaPoolMultiTest"
DefaultCollMeta.Stream = "CheckCollections"
DefaultCollMeta.Pass = 2
DefaultCollMeta.ConfigTag = "ATN"
DefaultCollMeta.OutputLevel = DEBUG   
topSequence+=DefaultCollMeta  

# output a collection to test multi input metadata copy
FullColl = RegistrationStream("FullColl")
FullColl.WriteInputDataHeader = TRUE
FullColl.OutputCollection = "CheckCollections.root"
FullColl.ItemList        += [ "DataHeader#*" ]
FullColl.ItemList        += [ "AthenaAttributeList#RunEventTag" ]
FullColl.ItemList        += [ "CollectionMetadataContainer#Default" ]
FullColl.Tool = TagTool
FullColl.OutputLevel = DEBUG

athRegSeq += FullColl

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
