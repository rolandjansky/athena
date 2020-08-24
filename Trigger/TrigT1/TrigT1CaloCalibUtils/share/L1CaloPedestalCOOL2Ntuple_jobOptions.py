###########################################################################
#
# Retrieve L1CaloPedestalContainer from COOL and dump it to an ntuple
#
###########################################################################

#use McEventSelector
include( "AthenaCommon/Atlas_Gen.UnixStandardJob.py" )

###########################################################################

from AthenaCommon.GlobalFlags import GlobalFlags
GlobalFlags.DetGeo.set_atlas()
GlobalFlags.DataSource.set_geant4()

from AthenaCommon.DetFlags import DetFlags
# - Select detectors
DetFlags.ID_setOff()
DetFlags.Calo_setOn()
DetFlags.Tile_setOff()
DetFlags.Muon_setOff()

DetDescrVersion="ATLAS-CSC-02-00-00"
#DetDescrCnvSvc.IdDictFromRDB = False

#setup GeoModel
include( "AtlasGeoModel/SetGeometryVersion.py" )
include( "AtlasGeoModel/GeoModelInit.py" )

#--------------------------------------------------------------
# CaloTriggerTowerService
#--------------------------------------------------------------

#required for use of the CaloTriggerTowerService
include("CaloConditions/CaloConditions_jobOptions.py")
include("LArDetDescr/LArDetDescr_joboptions.py")


#--------------------------------------------------------------
# L1CaloCondSvc settings
#--------------------------------------------------------------

theApp.Dlls += ["TrigT1CaloCondSvc"]
L1CaloCondSvc = Service("L1CaloCondSvc")
L1CaloCondSvc.OutputLevel = INFO


#--------------------------------------------------------------
# L1CaloPedestals2Ntuple settings
#--------------------------------------------------------------

theApp.Dlls += ["TrigT1CaloCalibTools"]

#to dump pedestals information to ntuples
theApp.TopAlg += [ "L1CaloPedestals2Ntuple" ]
L1CaloPedestals2Ntuple = Algorithm( "L1CaloPedestals2Ntuple" )
L1CaloPedestals2Ntuple.PedestalsKey ="L1CaloPedestals"
L1CaloPedestals2Ntuple.NtupleFilename = "pedestals_ntuple.root"
L1CaloPedestals2Ntuple.OutputLevel = INFO


#--------------------------------------------------------------
# EventSelector settings
#--------------------------------------------------------------

EventSelector = Service( "EventSelector" )
EventSelector.RunNumber         = 35
EventSelector.EventsPerRun      = 1
EventSelector.FirstEvent        = 1
EventSelector.InitialTimeStamp  = 0
EventSelector.TimeStampInterval = 5

theApp.EvtMax = 1


#--------------------------------------------------------------
# Access to IOVSvc, IOVDbSvc and CondDBMySQLCnvSvc
#--------------------------------------------------------------
include ( "IOVDbSvc/IOVDbSvc_jobOptions.py" )

#--------------------------------------------------------------
# Input options
#--------------------------------------------------------------

# Dictionaries:
#include( "TrigT1CaloCalibConditions/TrigT1CaloCalibConditionsDict_joboptions.py" )

# Converters:
#include( "TrigT1CaloCalibAthenaPool/TrigT1CaloCalibAthenaPool_joboptions.py" )

# this preload causes callbacks for read in objects to be activated,
IOVSvc=Service("IOVSvc")
IOVSvc.PreLoadData=True


#--------------------------------------------------------------
# DB options
#--------------------------------------------------------------

IOVDbSvc = Service( "IOVDbSvc" )
#IOVDbSvc.OutputLevel = VERBOSE

#IOVDbSvc.dbConnection="impl=cool;techno=sqlite;schema=mysqlfile.db;DUMMY:COOLTEST"

DBServer     = "devdb10" #  Server name - physical name
DataBase     = "xxxxxxx" #  DB name
User         = "xxxxxxx" #  user name
#DBConnection = " <dbConnection>impl=cool;techno=oracle;"+DBServer+":"+DataBase+":"+User+"</dbConnection> "
DBConnection = " <dbConnection>impl=cool;techno=sqlite;schema=mysqlfile.db;DUMMY:COOLTEST</dbConnection> "

FolderName = "/L1Calo/L1CaloPedestals"
FolderTag = "COOL-TEST-001"
#FolderTag = ""

if ( FolderTag == "" ):
	TagSpec = "HEAD"
else:
	from string import *
	TagSpec = join(split(FolderName,'/'),'') + FolderTag

IOVDbSvc.Folders+=[ FolderName + DBConnection + "<tag>"+TagSpec+"</tag>" ]


###########################################################################

AthenaEventLoopMgr=Service("AthenaEventLoopMgr")
AthenaEventLoopMgr.OutputLevel = INFO

MessageSvc = Service( "MessageSvc" )
MessageSvc.OutputLevel = INFO
