## get a handle on the default top-level algorithm sequence
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
## basic job configuration
import AthenaCommon.AtlasUnixStandardJob

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
import AthenaPoolCnvSvc.ReadAthenaPool

svcMgr.PoolSvc.ReadCatalog = ["xmlcatalog_file:SplittableData.xml"]

svcMgr.EventSelector.InputCollections = [ "PFN:NullableCollection.root" ]
svcMgr.EventSelector.CollectionType = "ExplicitROOT"

#--------------------------------------------------------------
# Private Application Configuration options
#--------------------------------------------------------------
# Load "user algorithm" top algorithms to be run, and the libraries that house them
from AthenaPoolExampleAlgorithms.AthenaPoolExampleAlgorithmsConf import AthPoolEx__ReadData
topSequence += AthPoolEx__ReadData("ReadData")

#--------------------------------------------------------------
# Output options
#--------------------------------------------------------------

from AthenaPoolMultiTest.AthenaPoolMultiTestConf import *

RunEventTagWriter   = RunEventTagWriter("RunEventTagWriter")

topSequence        += RunEventTagWriter

from RegistrationServices.RegistrationServicesConf import RegistrationStream
from RegistrationServices.RegistrationServicesConf import RegistrationStreamTagTool

TagTool = RegistrationStreamTagTool("TagTool")

# output a collection to test multi input metadata copy
FullColl = RegistrationStream("FullColl")
FullColl.WriteInputDataHeader = TRUE
FullColl.OutputCollection = "CheckNull.root"
FullColl.ItemList        += [ "DataHeader#*" ]
FullColl.ItemList        += [ "AthenaAttributeList#RunEventTag" ]
FullColl.OutputLevel = DEBUG
FullColl.Tool.OutputLevel = INFO

athRegSeq += FullColl


#--------------------------------------------------------------
# Set output level threshold (2=DEBUG, 3=INFO, 4=WARNING, 5=ERROR, 6=FATAL)
#--------------------------------------------------------------
svcMgr.MessageSvc.OutputLevel = 3
svcMgr.MessageSvc.defaultLimit = 100000

svcMgr.MetaDataSvc.OutputLevel = 2
svcMgr.EventSelector.OutputLevel = 2
svcMgr.PoolSvc.OutputLevel = 2
topSequence.ReadData.OutputLevel = 2

#
# End of job options file
#
###############################################################
