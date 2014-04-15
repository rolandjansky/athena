## @file MakeSimpleCollection.py
## @brief Creates a ROOT collection of dummy run and event number metadata
## @author J. Cranshaw (Jack.Cranshaw@cern.ch)
## $Id: TestSplitCollection.py,v 1.1 2008-07-21 21:47:46 cranshaw Exp $

###############################################################
#
# Job options file
#
#==============================================================
# Input:  Local ROOT file called "In" containing references to
#         to the persistified event data and dummy event tag
#         metadata.
# Output: Local ROOT file called "Out" containing references
#         to the persistified event data and dummy event tag
#         metadata.
#==============================================================

#--------------------------------------------------------------
# Load POOL support
#--------------------------------------------------------------
from AthenaCommon.AlgSequence import AlgSequence
topSequence = AlgSequence()

from AthenaCommon.AppMgr import theApp

theApp.EvtMax=200000

import AthenaPoolCnvSvc.ReadAthenaPool

# If you need to change or add input file catalogs
from AthenaCommon.AppMgr import ServiceMgr as svcMgr

# If you need to change or add input file catalogs
from PoolSvc.PoolSvcConf import PoolSvc
svcMgr += PoolSvc()
svcMgr.PoolSvc.ReadCatalog = [ "XMLFileCatalog_file:SplittableData.xml" ]
#
#--------------------------------------------------------------
# Event related parameters
#--------------------------------------------------------------
# Set the following when reading back to adjust the run/event numbers
#   (don't forget ApplicationMgr.EvtMax)
#svcMgr.EventSelector = Service( "EventSelector" )
svcMgr.EventSelector.InputCollections =  ["PFN:Collection_SplitBar.root"];
svcMgr.EventSelector.CollectionType = 'ExplicitROOT'
svcMgr.EventSelector.Query = "RunNumber != 1000000 && EventNumber > 0"
#svcMgr.EventSelector.Query = "RunNumber != 1000000  && test3 > 0"

#--------------------------------------------------------------
# Load and Set Tag Processing
#--------------------------------------------------------------
from AthenaPoolMultiTest.AthenaPoolMultiTestConf import RunEventTagWriter

theApp.TopAlg   += [ "RunEventTagWriter" ]

# Converters:
include( "EventAthenaPool/EventAthenaPool_joboptions.py" )

include ("AthenaPoolTools/EventCount_jobOptions.py")

#--------------------------------------------------------------
# Event Collection Registration
#--------------------------------------------------------------
# Run OutputStream as an algorithm
from RegistrationServices.RegistrationServicesConf import RegistrationStream

RegStream1 = RegistrationStream( "RegStream1" )
RegStream1.OutputLevel = INFO
RegStream1.CollectionType = "ExplicitROOT"
RegStream1.WriteInputDataHeader = True
RegStream1.OutputCollection = "test.coll"; # The output file name
# List of DO's to register
RegStream1.ItemList += [ "DataHeader#*" ]
# Key name of AthenaAttributeList used for the tag:
RegStream1.ItemList += [ "AthenaAttributeList#RunEventTag" ]

topSequence+=RegStream1

#--------------------------------------------------------------
# Set output level threshold (2=DEBUG, 3=INFO, 4=WARNING, 5=ERROR, 6=FATAL )
#--------------------------------------------------------------
svcMgr.MessageSvc.OutputLevel = DEBUG
#MessageSvc.OutputLevel = DEBUG

#==============================================================
#
# End of job options file
#
###############################################################
