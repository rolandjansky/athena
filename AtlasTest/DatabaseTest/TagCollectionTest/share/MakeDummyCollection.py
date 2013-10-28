## @file MakeDummyCollection.py
## @brief Utility to create an event tag collection with dummy metadata
## @author K. Karr (Kristo.Karr@cern.ch)
## $Id: MakeDummyCollection.py,v 1.18 2009-05-01 15:20:32 cranshaw Exp $

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
try:
  svcMgr.MessageSvc.OutputLevel = MsgLevel
except:
  svcMgr.MessageSvc.OutputLevel = INFO

#--------------------------------------------------------------
# Message service printout limits
#--------------------------------------------------------------
svcMgr.MessageSvc.debugLimit = 10000

#--------------------------------------------------------------
# Number of events to write
#--------------------------------------------------------------
try:
  theApp.EvtMax = EvtMax
except:
  theApp.Evtmax = 10

#--------------------------------------------------------------
# POOL support
#--------------------------------------------------------------
include ("AthenaPoolCnvSvc/AthenaPool_jobOptions.py")

from PoolSvc.PoolSvcConf import PoolSvc
svcMgr += PoolSvc()
try:
  svcMgr.PoolSvc.WriteCatalog = OutCatalog
except:
  svcMgr.PoolSvc.WriteCatalog = "xmlcatalog_file:PoolFileCatalog.xml"

#--------------------------------------------------------------
# Input collection type
#--------------------------------------------------------------
try:  
  inCollType = InCollType
except:
  inCollType = "MonteCarlo"

if inCollType == "MonteCarlo":

  include( "AthenaCommon/Atlas_Gen.UnixStandardJob.py" )
  # Event selector
  svcMgr.EventSelector.RunNumber=1
  svcMgr.EventSelector.FirstEvent=1

else:

  # Input file catalog
  import AthenaPoolCnvSvc.ReadAthenaPool
  try:
    svcMgr.PoolSvc.ReadCatalog = InCatalog
  except:
    svcMgr.PoolSvc.ReadCatalog = [ "xmlcatalog_file:PoolFileCatalog.xml" ]
  
  # Event selector

  # Input collection name
  try:
    svcMgr.EventSelector.InputCollections  = InColl 
  except:
    svcMgr.EventSelector.InputCollections  = [ "InputData.root" ]

  # Input collection type
  try:
    svcMgr.EventSelector.CollectionType = InCollType
  except:
    svcMgr.EventSelector.CollectionType = "ImplicitROOT"

  # Input collection database connection string
  try:
    svcMgr.EventSelector.Connection = InCollConnect
  except:
    svcMgr.EventSelector.Connection = ""
    
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
try:
  Stream1.ProcessingTag = StageOutName
except:
  print "No processing tag set for stream1"

# Persistent data file name:
try:
  Stream1.OutputFile = OutFile
except:
  Stream1.OutputFile = "OutputData.root"

#Extend or not provenance on event stream
try:
  Stream1.ExtendProvenanceRecord = Extend
except:
  print "Provenance record will be extended in DataHeader"

# List of data objects to be persistified:
Stream1.ItemList += ["EventInfo#*"]

topSequence+=Stream1

#--------------------------------------------------------------
# Event tag collection registration
#--------------------------------------------------------------
# Registration stream:

from RegistrationServices.RegistrationServicesConf import InputCollectionMetadataCopy
from RegistrationServices.RegistrationServicesConf import RegistrationStreamDefMeta

# Add tool for copying input collection metadata
copyTool = InputCollectionMetadataCopy("copyTool")
copyTool.OutputLevel = DEBUG
ToolSvc += copyTool
svcMgr.MetaDataSvc.MetaDataTools += [copyTool]

# Add algorithm to add default collection metadata
DefaultCollMeta = RegistrationStreamDefMeta("DefaultCollMeta")
DefaultCollMeta.Project = "TagCollectionTest"
try: 
  DefaultCollMeta.Stream = OutColl
except:
  DefaultCollMeta.Stream = "TEST"
topSequence+=DefaultCollMeta

from RegistrationServices.RegistrationServicesConf import RegistrationStream

RegStream1 = RegistrationStream( "RegStream1" )

RegStream1.OutputLevel = INFO

# Specifies whether collection references input or output data file:
try:
  RegStream1.WriteInputDataHeader = WriteInputHeader
except:
  RegStream1.WriteInputDataHeader = FALSE
if inCollType == "MonteCarlo":
  RegStream1.WriteInputDataHeader = FALSE

# Output collection name:
try:
  RegStream1.OutputCollection = OutColl
except:
  RegStream1.OutputCollection = "OutputCollection"

# Output collection type:
try:
  RegStream1.CollectionType = OutCollType
except:
  RegStream1.CollectionType = "ExplicitROOT"
  
# Output collection database connection string:
try:
  RegStream1.Connection = OutCollConnect
except:
  RegStream1.Connection = ""

# Output collection open mode:
try:
  RegStream1.CollectionOpenMode = OpenMode
except:
  RegStream1.CollectionOpenMode = "CREATE_AND_OVERWRITE"

# Reference to input file
addProv = False
try:  
  addProv = ProvOn
  RegStream1.WriteAllProv = ProvOn
except:
  print "Writing only primary token to collection"
  RegStream1.WriteAllProv = False

# prov stage included
if addProv:
  try:
    RegStream1.IncludeProvStages = ProvIncludes
  except:
    print "Prov in attributelist will be set based on first data header" 

# List of data object references to be added to collection:
RegStream1.ItemList += [ "DataHeader#*" ]

# List of attribute list objects to be added to collection:
RegStream1.ItemList +=  [ "AthenaAttributeList#SimpleTag" ]

# List of attribute list objects to be added to collection:
RegStream1.ItemList +=  [ "CollectionMetadataContainer#*" ]

topSequence+=RegStream1

try:
  # If the following flag is set, then it will write a collection WITH NO METADATA as well
  if nometa:
    RegStream2 = RegistrationStream( "RegStream2" )

    RegStream2.WriteInputDataHeader = FALSE
    # Output collection name:
    RegStream2.OutputCollection = "NoMetadata"

    # Output collection type:
    try:
      RegStream2.CollectionType = OutCollType
    except:
      RegStream2.CollectionType = "ExplicitROOT"
  
    # Output collection database connection string:
    try:
      RegStream2.Connection = OutCollConnect
    except:
      RegStream2.Connection = ""

    # Output collection open mode:
    RegStream2.CollectionOpenMode = "CREATE_AND_OVERWRITE"

    # Reference to input file
    addProv = False
    try:  
      addProv = ProvOn
      RegStream2.WriteAllProv = ProvOn
    except:
      print "Writing only primary token to collection"
      RegStream2.WriteAllProv = False

    # prov stage included
    if addProv:
      try:
        RegStream2.IncludeProvStages = ProvIncludes
      except:
        print "Prov in attributelist will be set based on first data header" 

    # List of data object references to be added to collection:
    RegStream2.ItemList += [ "DataHeader#*" ]

    topSequence+=RegStream2
except:
  print "nometa is not defined"

#==============================================================
#
# End of job options file
#
###############################################################
