## @file MakeEventTagCollection.py
## @brief Utility to create a collection with all event tag metadata
## @author K. Karr (Kristo.Karr@cern.ch)
## $Id: MakeEventTagCollection.py,v 1.6 2006-08-21 17:27:14 tmaeno Exp $

###############################################################
#
# Job options file
#
#==============================================================
# Input:  A collection or set of collections, which may simply
#         consist of a set of ROOT persistent data files
#         (i.e. "implicit" collections). By default, a single
#         file called "InputData.root" is assumed but this may be
#         changed on the athena command line via the argument
#         "InColl". A read file catalog must be specified when
#         "explicit" collections are used as input and the full
#         path name of the catalog is different from the default.
# Output: A ROOT or MySQL collection containing references to
#         the persistified events along with their associated
#         event tag metadata. By default, the collection is of
#         type ROOT and called "OutputCollection.root" but this may
#         be changed on the athena command line via the arguments
#         "OutColl", "OutCollType" and "OutCollConnect".
#==============================================================

#--------------------------------------------------------------
# Message service output level
# (VERBOSE, DEBUG, INFO, WARNING, ERROR or FATAL )
#--------------------------------------------------------------
MessageSvc = Service( "MessageSvc" )
try:
  MessageSvc.OutputLevel = MsgLevel
except:
  MessageSvc.OutputLevel = WARNING

#--------------------------------------------------------------
# Message service printout limits
#--------------------------------------------------------------
MessageSvc.debugLimit = 10000

#--------------------------------------------------------------
# Number of events to write
#--------------------------------------------------------------
try:
  theApp.EvtMax = EvtMax
except:
  theApp.EvtMax = 200000

#--------------------------------------------------------------
# POOL support
#--------------------------------------------------------------
include( "AthenaPoolCnvSvc/ReadAthenaPool_jobOptions.py" )
PoolSvc = Service( "PoolSvc" )
try:
  PoolSvc.ReadCatalog = InCatalog
except:
  PoolSvc.ReadCatalog = [ "xmlcatalog_file:PoolFileCatalog.xml" ]

#--------------------------------------------------------------
# Event selector
#--------------------------------------------------------------
EventSelector = Service( "EventSelector" )

# Input collection name
try:
  EventSelector.InputCollections  = InColl
except:
  EventSelector.InputCollections  = [ "InputData.root" ]

# Input collection type
try:
  EventSelector.CollectionType = InCollType
except:
  EventSelector.CollectionType = "ImplicitROOT"

# Input collection database connection string
try:
  EventSelector.Connection = InCollConnect
except:
  EventSelector.Connection = ""

#--------------------------------------------------------------
# Application: Builds and registers event tag metadata
#--------------------------------------------------------------
include( "EventTagAlgs/EventTag_jobOptions.py" )

#--------------------------------------------------------------
# Converters
#--------------------------------------------------------------
include( "ParticleBuilderOptions/ESD_PoolCnv_jobOptions.py" )
include( "ParticleBuilderOptions/AOD_PoolCnv_jobOptions.py" )

#--------------------------------------------------------------
# Event tag collection registration
#--------------------------------------------------------------
theApp.TopAlg += [ "RegistrationStream/RegStream1" ]
RegStream1 = Algorithm( "RegStream1" )
RegStream1.WriteInputDataHeader = True

# Output collection name
try:
  RegStream1.OutputCollection = OutColl
except:
  RegStream1.OutputCollection = "OutputCollection"

# Output collection type
try:
  RegStream1.CollectionType = OutCollType
except:
  RegStream1.CollectionType = "ExplicitROOT"

# Output collection database connection string
try:
  RegStream1.Connection = OutCollConnect
except:
  RegStream1.Connection = ""

# Output collection open mode
try:
  RegStream1.CollectionOpenMode = OpenMode
except:
  RegStream1.CollectionOpenMode = "CREATE_AND_OVERWRITE"

# List of data object references to be added to collection
RegStream1.ItemList += [ "DataHeader#*" ]

# List of attribute list objects to be added to collection
RegStream1.ItemList +=  [ "AthenaAttributeList#"+EventTagGlobal.AttributeList ]

#==============================================================
#
# End of job options file
#
###############################################################

