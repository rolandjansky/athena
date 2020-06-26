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

from AthenaCommon.GlobalFlags import globalflags
globalflags.DataSource.set_Value_and_Lock('geant4')
globalflags.DetGeo.set_Value_and_Lock('atlas')
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
#LArConditionsTestAlg.ReadCondObjs     = True;
# Write conditions objects to detector store
LArConditionsTestAlg.WriteCondObjs    = True;
# Write correction
LArConditionsTestAlg.WriteCorrections = True;
# Apply corrections
LArConditionsTestAlg.ApplyCorrections = True;
#LArConditionsTestAlg.WriteCondObjs    = True;
#LArConditionsTestAlg.TestFill         = True;
#LArConditionsTestAlg.TestReadDBDirect = True;
#LArConditionsTestAlg.TestHV           = True;
#LArConditionsTestAlg.Testbeam         = True;


#--------------------------------------------------------------
# Set the following when reading back to adjust the run/event numbers
#   (don't forget ApplicationMgr.EvtMax)
EventSelector = svcMgr.EventSelector
#EventSelector.RunNumber         = 1
#EventSelector.EventsPerRun      = 5
#EventSelector.FirstEvent        = 1
#EventSelector.InitialTimeStamp  = 0
#EventSelector.TimeStampInterval = 5
# For the moment, only read one event to get conditions
theApp.EvtMax = 1

#--------------------------------------------------------------
#  Include the needed converters
#--------------------------------------------------------------

include ( "LArCondAthenaPool/LArCondAthenaPool_joboptions.py" )

#--------------------------------------------------------------
#  Use OutputConditionsAlg to write
#--------------------------------------------------------------

# Explicitly specify the output file catalog
# to avoid races when running tests in parallel.
PoolSvc = Service( "PoolSvc" )
PoolSvc.WriteCatalog = "file:Catalog3.xml"

from RegistrationServices.OutputConditionsAlg import OutputConditionsAlg
OutputConditionsAlg=OutputConditionsAlg("OutputConditionsAlg","LarCondTestNoReg.root")

OutputConditionsAlg.ObjectList =  [ "LArRampMC#/LArCalorimeter/LArTests/LArRampsSingleGroup" ]
OutputConditionsAlg.ObjectList += [ "LArRampMC#/LArCalorimeter/LArTests/LArRampsSubDetectorGrouping" ]
OutputConditionsAlg.ObjectList += [ "LArRampMC#/LArCalorimeter/LArTests/LArRampsFeedThroughGrouping" ]
#OutputConditionsAlg.WriteIOV=True
#OutputConditionsAlg.Run1=3
#OutputConditionsAlg.Event1=4
#OutputConditionsAlg.Run2=9999
#OutputConditionsAlg.Event2=999999
#OutputConditionsAlg.IOVTagList=["mytag"]
# specify the COOL datatbase connection string (this one is development account)
#IOVDbSvc.dbConnection  = "impl=cool;techno=oracle;devdb10:TESTCOOL:ATLAS_COOLTEST:cool4devdb"
#IOVDbSvc.dbConnection  = "impl=cool;techno=oracle;devdb10:LARNOREG:ATLAS_COOLTEST:cool4devdb"

conddb.setGlobalTag("OFLCOND-CSC-00-01-00")

# Suppress useless GeoModelSvc messages.
from AthenaCommon import Constants
from GeoModelSvc.GeoModelSvcConf import GeoModelSvc
GeoModelSvc().OutputLevel=Constants.WARNING
