## @file MakeDummyCollection.py
## @brief Utility to create an event tag collection with dummy metadata
## @author K. Karr (Kristo.Karr@cern.ch)
## $Id: test_justRSProperties.py,v 1.2 2008-04-11 19:40:10 cranshaw Exp $

###############################################################
#
# Job options file
#
#==============================================================
# Input:  1) Monte Carlo data, by default, but can change to
#            ROOT data file or explicit collection input.
# Output: 1) Local ROOT data file called "OutputData.root",
#            by default, containing the persistified event data.
#         2) Collection called "OutputCollection", by default,
#            containing references to the persistified event data
#            along with dummy event tag metadata.
#==============================================================

from AthenaCommon.AlgSequence import AlgSequence
topSequence = AlgSequence()

from AthenaCommon.AppMgr import theApp
from AthenaCommon.AppMgr import ServiceMgr as svcMgr

#--------------------------------------------------------------
# Message service output level threshold
# (1=VERBOSE, 2=DEBUG, 3=INFO, 4=WARNING, 5=ERROR, 6=FATAL )
#--------------------------------------------------------------
svcMgr.MessageSvc.OutputLevel = INFO

#--------------------------------------------------------------
# Message service printout limits
#--------------------------------------------------------------
svcMgr.MessageSvc.debugLimit = 10000

#--------------------------------------------------------------
# Number of events to write
#--------------------------------------------------------------
theApp.Evtmax = 1

#--------------------------------------------------------------
# POOL support
#--------------------------------------------------------------
include ("AthenaPoolCnvSvc/AthenaPool_jobOptions.py")

from PoolSvc.PoolSvcConf import PoolSvc
svcMgr += PoolSvc()
svcMgr.PoolSvc.WriteCatalog = "xmlcatalog_file:PoolFileCatalog.xml"

#--------------------------------------------------------------
# Application: Creates and registers dummy event tag metadata
#--------------------------------------------------------------
from AthenaPoolMultiTest.AthenaPoolMultiTestConf import EventTagWriter

EventTagWriter = EventTagWriter( "EventTagWriter" )

topSequence+=EventTagWriter

#--------------------------------------------------------------
# Converter
#--------------------------------------------------------------
include( "EventAthenaPool/EventAthenaPool_joboptions.py" )

#--------------------------------------------------------------
# Output streams
#--------------------------------------------------------------
theApp.OutStream = []; 

from AthenaServices.AthenaServicesConf import AthenaOutputStream

ToolSvc = Service( "ToolSvc" )

# Declare output streams:
Stream1 = AthenaOutputStream( "Stream1" )

# Tool for output stream:
Stream1.WritingTool = "AthenaPoolOutputStreamTool"
# Set processing stage name for <>_ref
Stream1.ProcessingTag = 'PROP_ONLY'
Stream1.OutputFile = "OutputData.root"
# List of data objects to be persistified:
Stream1.ItemList += ["EventInfo#*"]

topSequence+=Stream1

#--------------------------------------------------------------
# Event tag collection registration
#--------------------------------------------------------------
# Registration stream:
from RegistrationServices.RegistrationServicesConf import RegistrationStream

RegStream1 = RegistrationStream( "RegStream1" )

# List of data object references to be added to collection:
RegStream1.ItemList += [ "DataHeader#*" ]
RegStream1.ItemList +=  [ "AthenaAttributeList#SimpleTag" ]
# Output collection type:
RegStream1.CollectionType = "ExplicitROOT"
# Output collection database connection string:
RegStream1.Connection = ""
# Output collection name:
RegStream1.OutputCollection = "OutputCollection"
# Output collection open mode:
RegStream1.CollectionOpenMode = "CREATE_AND_OVERWRITE"
# Specifies whether collection references input or output data file:
RegStream1.WriteInputDataHeader = FALSE
# Reference to input file
RegStream1.WriteAllProv = False
# prov stage included
RegStream1.IncludeProvStages = []
RegStream1.IncludeProvStages = ['test']
# prov stage excluded
RegStream1.ExcludeProvStages = []
RegStream1.ExcludeProvStages = ['test0']
RegStream1.ExcludeProvStages += ['nottest']
# Persistency service
RegStream1.PersistencySvc = 'EventPersistencySvc'
# Storename
RegStream1.StoreName = 'StoreGateSvc'
# Accept alg list
RegStream1.AcceptAlgs = []
# Requre alg list
RegStream1.RequireAlgs = []
# Veto alg list
RegStream1.VetoAlgs = []

topSequence+=RegStream1

#==============================================================
#
# End of job options file
#
###############################################################
