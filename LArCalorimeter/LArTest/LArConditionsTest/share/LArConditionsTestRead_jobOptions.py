###############################################################
#
# Job options file
#
#==============================================================
#use McEventSelector
include( "AthenaCommon/Atlas_Gen.UnixStandardJob.py" )


#--------------------------------------------------------------
# Detector Description
#--------------------------------------------------------------
from AthenaCommon.DetFlags import DetFlags
DetFlags.Calo_setOn()
DetFlags.ID_setOff()
DetFlags.Muon_setOff()
DetFlags.Truth_setOff()
DetFlags.LVL1_setOff()
DetFlags.digitize.all_setOff()

from AthenaCommon.GlobalFlags import GlobalFlags
GlobalFlags.DataSource.set_geant4()
GlobalFlags.DetGeo.set_atlas()
include ("AtlasGeoModel/SetGeometryVersion.py")
from AtlasGeoModel import GeoModelInit
include ("LArRawConditions/LArIdMap_ATLAS_jobOptions.py")

# Other LAr related 
include( "LArIdCnv/LArIdCnv_joboptions.py" )
#include( "LArDetMgrDetDescrCnv/LArDetMgrDetDescrCnv_joboptions.py" )

include( "CaloDetMgrDetDescrCnv/CaloDetMgrDetDescrCnv_joboptions.py" )

include( "IdDictDetDescrCnv/IdDictDetDescrCnv_joboptions.py" )



#--------------------------------------------------------------
# Private Application Configuration options
#--------------------------------------------------------------
#load relevant libraries
theApp.Dlls += [ "LArRawUtils" ]
theApp.Dlls += [ "LArTools" ]
theApp.Dlls += [ "LArConditionsTest" ]
# include the converters
include( "LArCondAthenaPool/LArCondAthenaPool_joboptions.py" )


theApp.TopAlg = [ "LArConditionsTestAlg" ]

#--------------------------------------------------------------
# Set output level threshold (1=VERBOSE, 2=DEBUG, 3=INFO, 4=WARNING, 5=ERROR, 6=FATAL )
#--------------------------------------------------------------
MessageSvc = Service( "MessageSvc" )
MessageSvc.OutputLevel      = 3
MessageSvc.debugLimit       = 100000
MessageSvc.infoLimit        = 100000
LArConditionsTestAlg = Algorithm( "LArConditionsTestAlg" )
LArConditionsTestAlg.OutputLevel    = 2

#--------------------------------------------------------------
#

# Test condition objects - created on the fly
LArConditionsTestAlg.TestCondObjs     = True;
# Read conditions objects for detector store
LArConditionsTestAlg.ReadCondObjs     = True;
# Write conditions objects to detector store
#LArConditionsTestAlg.WriteCondObjs    = True;
# Write correction
#LArConditionsTestAlg.WriteCorrections = True;
# Apply corrections
LArConditionsTestAlg.ApplyCorrections = True;
#LArConditionsTestAlg.TestFill         = True;
#LArConditionsTestAlg.TestReadDBDirect = True;
#LArConditionsTestAlg.TestHV           = True;
#LArConditionsTestAlg.Testbeam         = True;


#--------------------------------------------------------------
# Set the following when reading back to adjust the run/event numbers
#   (don't forget ApplicationMgr.EvtMax)
EventSelector = Service( "EventSelector" )
#EventSelector.RunNumber         = 1
#EventSelector.EventsPerRun      = 5
#EventSelector.FirstEvent        = 1
#EventSelector.InitialTimeStamp  = 0
#EventSelector.TimeStampInterval = 5
# For the moment, only read one event to get conditions
theApp.EvtMax = 1

#--------------------------------------------------------------
# Access to IOVSvc and IOVDbSvc
# Must list the folders to be used for reading
#--------------------------------------------------------------
include ( "IOVDbSvc/IOVDbSvc_jobOptions.py" )

IOVDbSvc = Service( "IOVDbSvc" )

# Implicit list of folders without local tag
#IOVDbSvc.Folders += [ "/IOVDbTest/*" ]

# Explicit list of folders with tags
# NOTE: we override an previously defined folders
IOVDbSvc.Folders  = [ "/LArCalorimeter/LArTests/LArRampsSingleGroup" ]
IOVDbSvc.Folders += [ "/LArCalorimeter/LArTests/LArRampsSubDetectorGrouping" ]
IOVDbSvc.Folders += [ "/LArCalorimeter/LArTests/LArRampsFeedThroughGrouping" ]
# Set DB connection
try:
    IOVDbSvc.dbConnection  = "impl=cool;techno=oracle;devdb10:"
    IOVDbSvc.dbConnection  += TESTCOOL
    IOVDbSvc.dbConnection  += ":ATLAS_COOLTEST:cool4devdb"
except:
    IOVDbSvc.dbConnection  = "impl=cool;techno=oracle;devdb10:LARTEST:ATLAS_COOLTEST:cool4devdb"
#IOVDbSvc.OutputLevel     = 1

#--------------------------------------------------------------
# Load POOL support
#--------------------------------------------------------------
# Make sure AthenaPoolServices is loaded for custom streamer
include ("AthenaPoolServices/AthenaRootStreamerSvc_jobOptions.py")

include( "AthenaPoolCnvSvc/AthenaPool_jobOptions.py" )

EventPersistencySvc = Service( "EventPersistencySvc" )
EventPersistencySvc.CnvServices += [ "AthenaPoolCnvSvc" ]

#Explicitly specify the output file catalog
#PoolSvc = Service( "PoolSvc" )
#PoolSvc.WriteCatalog = "file:Catalog1.xml"


