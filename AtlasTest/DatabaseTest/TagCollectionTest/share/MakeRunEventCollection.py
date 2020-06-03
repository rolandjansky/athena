## @file MakeSimpleCollection.py
## @brief Utility to create a collection with run and event number metadata
## @author J. Cranshaw (Jack.Cranshaw@cern.ch)
## @author K. Karr (Kristo.Karr@cern.ch)
## $Id: MakeRunEventCollection.py,v 1.3 2009-02-23 21:29:38 cranshaw Exp $

###############################################################
#
# Job options file
#
#==============================================================
# Input:  A collection or set of collections, which may simply
#         consist of a set of ROOT persistent data files
#         (i.e. "implicit" collections). By default, a single
#         file called "test.root" is assumed but this may be
#         changed on the athena command line via the argument "In".
# Output: A ROOT or MySQL collection containing references to
#         the persistified events along with their associated
#         run and event number metadata. By default, the
#         collection is of type ROOT and called "test.coll"
#         but this may be changed on the athena command line
#         via the arguments "Out", "Type", "Connect" and
#         "Mode".
#==============================================================

from AthenaCommon.AlgSequence import AlgSequence
topSequence = AlgSequence()

from RecExConfig.ObjKeyStore import objKeyStore
if not objKeyStore.isInInput( "xAOD::EventInfo" ):
    if not hasattr( topSequence, "xAODMaker::EventInfoCnvAlg" ):
        from xAODEventInfoCnv.xAODEventInfoCreator import xAODMaker__EventInfoCnvAlg
        topSequence += xAODMaker__EventInfoCnvAlg()
        pass
else:
    if not hasattr( topSequence, "xAODMaker::EventInfoNonConstCnvAlg" ):
        topSequence += CfgMgr.xAODMaker__EventInfoNonConstCnvAlg()
        pass


from AthenaCommon.AppMgr import theApp
from AthenaCommon.AppMgr import ServiceMgr as svcMgr

#--------------------------------------------------------------
# Message service output level threshold
# (1=VERBOSE, 2=DEBUG, 3=INFO, 4=WARNING, 5=ERROR, 6=FATAL )
#--------------------------------------------------------------
try:
  svcMgr.MessageSvc.OutputLevel = Level
except:
  svcMgr.MessageSvc.OutputLevel = INFO

#--------------------------------------------------------------
# Number of events to write
#--------------------------------------------------------------
try:
  theApp.EvtMax = EvtMax
except:
  theApp.EvtMax = 200000

#--------------------------------------------------------------
# Load POOL support
#--------------------------------------------------------------
include( "AthenaPoolCnvSvc/ReadAthenaPool_jobOptions.py" )
from PoolSvc.PoolSvcConf import PoolSvc
svcMgr += PoolSvc()
try:
  PoolSvc.ReadCatalog = [ Catalog ]
  PoolSvc.WriteCatalog = Catalog
except:
  PoolSvc.ReadCatalog = [ "xmlcatalog_file:PoolFileCatalog.xml" ]
  PoolSvc.WriteCatalog = "xmlcatalog_file:PoolFileCatalog.xml"

#--------------------------------------------------------------
# Input collection (default type is "implicit" collection)
#--------------------------------------------------------------

# Input collection name:
try:
  svcMgr.EventSelector.InputCollections  = In 
except:
  svcMgr.EventSelector.InputCollections  = [ "test.root" ]

# Input collection type:
try:
  svcMgr.EventSelector.CollectionType = InType
except:
  svcMgr.EventSelector.CollectionType = "ImplicitROOT"

# Input collection connection string:
try:
  svcMgr.EventSelector.Connection = InConnect
except:
  svcMgr.EventSelector.Connection = ""

#--------------------------------------------------------------
# Load and Set Tag Processing
#--------------------------------------------------------------
from AthenaPoolMultiTest.AthenaPoolMultiTestConf import RunEventTagWriter

RunEventTagWriter = RunEventTagWriter( "RunEventTagWriter" )

topSequence+=RunEventTagWriter

# Converters:
include( "EventAthenaPool/EventAthenaPool_joboptions.py" )

#--------------------------------------------------------------
# Event Collection Registration
#--------------------------------------------------------------
# Run OutputStream as an algorithm
from RegistrationServices.RegistrationServicesConf import RegistrationStream
from RegistrationServices.RegistrationServicesConf import RegistrationStreamLCGTool

TagTool = RegistrationStreamLCGTool("RegistrationStreamLCGTool")

RegStream1 = RegistrationStream( "RegStream1" )
RegStream1.WriteInputDataHeader = True
RegStream1.Tool = TagTool

# Full path name of output collection:
try:
  RegStream1.OutputCollection = Out
except:
  RegStream1.OutputCollection = "test.coll.root"

# Collection type (ExplicitROOT, ExplicitMySQL or ExplicitMySQLlt):
try:
  RegStream1.CollectionType = Type
except:
  RegStream1.CollectionType = "ExplicitROOT"

# Database connection string (Set to "" for ExplicitROOT collection):
try:
  RegStream1.Connection = Connect
except:
  RegStream1.Connection = ""

# Open mode of collection (CREATE_AND_OVERWRITE or CREATE):
try:
  RegStream1.CollectionOpenMode = Mode
except:
  RegStream1.CollectionOpenMode = "CREATE_AND_OVERWRITE"

# List of DO's to register
RegStream1.ItemList += [ "DataHeader#*" ]

# Key name of AthenaAttributeList used for the tag:
RegStream1.ItemList += [ "TagAthenaAttributeList#RunEventTag" ]

topSequence+=RegStream1

#==============================================================
#
# End of job options file
#
###############################################################
