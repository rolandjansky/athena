## @file EventTagCollectionDeepCopy.py
## @brief Utility to extract events from a collection that satisfy a query
## @author K. Karr (Kristo.Karr@cern.ch)
## $Id: EventTagCollectionDeepCopy.py,v 1.14 2007-12-10 22:23:02 cranshaw Exp $

###############################################################
#
# Job options file
#
#==============================================================
# Input:  One or more collections of event references and their
#         associated event tag metadata. The input may also
#         consist of one or more ROOT data files (i.e. "implicit"
#         collections). By default, a single collection called
#         "InputCollection" of type ROOT is assumed but this may
#         be changed on the athena command line via the arguments
#         "InColl", "InCollType" and "InCollConnect".
# Output: A ROOT file containing the persistent data of all
#         events referenced by the input collection that pass a
#         specified query. By default, the file is called
#         "OutputData.root" but this may be changed on the athena
#         command line via the argument "OutFile".
#==============================================================

#--------------------------------------------------------------
# Message service output level
# (VERBOSE, DEBUG, INFO, WARNING, ERROR or FATAL )
from AthenaCommon.AlgSequence import AlgSequence
topSequence = AlgSequence()

from AthenaCommon.AppMgr import theApp
from AthenaCommon.AppMgr import ServiceMgr as svcMgr

#--------------------------------------------------------------
MessageSvc = svcMgr.MessageSvc
try:
  MessageSvc.OutputLevel = MsgLevel
except:
  MessageSvc.OutputLevel = WARNING

#--------------------------------------------------------------
# Message service printout limits
#--------------------------------------------------------------
MessageSvc.debugLimit = 10000

#--------------------------------------------------------------
# POOL support
#--------------------------------------------------------------
import AthenaPoolCnvSvc.ReadAthenaPool

from PoolSvc.PoolSvcConf import PoolSvc

svcMgr += PoolSvc()
PoolSvc = svcMgr.PoolSvc

# Input file catalog name
try:
  PoolSvc.ReadCatalog = InCatalog
except:
  PoolSvc.ReadCatalog = [ "xmlcatalog_file:PoolFileCatalog.xml" ]

# Output file catalog name
try:
  PoolSvc.WriteCatalog = OutCatalog
except:
  PoolSvc.WriteCatalog = "xmlcatalog_file:PoolFileCatalog.xml"

#--------------------------------------------------------------
# Event selector
#--------------------------------------------------------------
EventSelector = svcMgr.EventSelector

# Input collection name
try:
  EventSelector.InputCollections  = InColl 
except:
  EventSelector.InputCollections  = [ "InputCollection.root" ]

# Input collection type
try:
  EventSelector.CollectionType = InCollType
except:
  EventSelector.CollectionType = "ExplicitROOT"

# Input collection connection string
try:
  EventSelector.Connection = InCollConnect
except:
  EventSelector.Connection = ""

# Query predicates
try:
  EventSelector.Query = Query
except:
  EventSelector.Query = ""


include ('EventTagAlgs/EventTagFlags.py')

#--------------------------------------------------------------
# Data to be persistified
#--------------------------------------------------------------
include( "ParticleBuilderOptions/AOD_OutputList_jobOptions.py")

#--------------------------------------------------------------
# Output Streams
#--------------------------------------------------------------
from AthenaServices.AthenaServicesConf import AthenaOutputStream

theApp.OutStream = []; 

Stream1 = AthenaPoolOutputStream( "Stream1" )

topSequence+=Stream1

# Output data file name
try:
  Stream1 = AthenaPoolOutputStream( "Stream1", OutFile, True )
except:
  Stream1 = AthenaPoolOutputStream( "Stream1", "OutputData.root", True )

try:
  ToolSvc.Stream1Tool.ProcessingTag = StageOutName
except:
  print "No processing tag set for stream1"

# List of data objects to be persistified:
Stream1.ItemList = AOD_ItemList

topSequence+=Stream1

#==============================================================
#
# End of job options file
#
###############################################################
