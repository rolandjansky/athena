###############################################################
#
# Job options file 1
#
#==============================================================
#use McEventSelector
include( "AthenaCommon/Atlas_Gen.UnixStandardJob.py" )

from AthenaCommon.AlgSequence import AlgSequence
topSequence = AlgSequence()

EventSelector = svcMgr.EventSelector
EventSelector.RunNumber=300000
EventSelector.EventsPerRun=10;
EventSelector.FirstEvent=1

theApp.Dlls += [ "GaudiAud" ] 
theAuditorSvc = svcMgr.AuditorSvc
theAuditorSvc.Auditors =  [ "ChronoAuditor" ] 


from AthenaCommon.GlobalFlags import GlobalFlags
GlobalFlags.DataSource.set_geant4()
#GlobalFlags.InputFormat.set_bytestream()
GlobalFlags.DetGeo.set_atlas()

#--------------------------------------------------------------
# Detector Description
#--------------------------------------------------------------
from AthenaCommon.DetFlags import DetFlags
DetFlags.Calo_setOff()
DetFlags.ID_setOff()
DetFlags.Muon_setOff()
DetFlags.Truth_setOff()
DetFlags.LVL1_setOff()
DetFlags.digitize.all_setOff()

from AthenaCommon.JobProperties import jobproperties
jobproperties.Global.DetDescrVersion = 'ATLAS-CommNF-04-00-00'

from AtlasGeoModel import SetGeometryVersion
from AtlasGeoModel import GeoModelInit

# svcMgr.IOVDbSvc.GlobalTag = "COMCOND-006-00"

# These flags should remain false,if you just want to write new objects
RecreateFolder  = False
SchemaPrivilege = False 

# Objects  and its tag
ObjectList = []
TagList = []


#  Keep WriteIOV false, and OnOffIdTag and FebRodTag not defined, 
#   test the fix in the algorithm 
#  when everythingis tested OK, then enable WriteIOV and one of the tag. 

# turn on output flags to enable POOL writes
# OnOffIdTag = "013" 
# FebRodTag  = "007" 
OnOffIdTag = "test" 
FebRodTag  = "test" 

# Turn on flag for writing to COOL IOV
# WriteIOV = False
WriteIOV = True

if "OnOffIdTag" in dir() : 
 ObjectList +=["LArOnOffIdMap#LArOnOffIdMapAtlas#/LAR/Identifier/OnOffIdAtlas"]
 TagList += ["LARIdentifierOnOffIdAtlas-"+OnOffIdTag]

elif  "FebRodTag" in dir() : 
 ObjectList += ["LArFebRodMap#LArFebRodMapAtlas#/LAR/Identifier/FebRodAtlas"]
 TagList += ["LARIdentifierFebRodAtlas-"+FebRodTag]
# OutputPOOLFileName = "LArIdMap.FebRodMap."+FebRodTag+".pool.root"

OutputPOOLFileName = "LArIdMapFix_test.pool.root"

#/--------------------------------------------------------------
# Algorithm to fix the LAr Id, if needed
#/------------------------------- 
theApp.Dlls += [ "LArConditionsTest" ]
theApp.TopAlg += [ "FixLArIdMap" ]
FixLArIdMap = Algorithm("FixLArIdMap")
FixLArIdMap.FixFlag = 0
# FixLArIdMap.FixFlag =8
# FixLArIdMap.FEBtoRODMap ="ROB-ROD-Feblist-Jul2008.txt"
FixLArIdMap.OutputLevel = 2

# FixLArIdMap.FixFlag =0
FixLArIdMap.DumpIds = True 


#--------------------------------------------------------------
# Private Application Configuration options
#--------------------------------------------------------------

# Other LAr related
include( "LArIdCnv/LArIdCnv_joboptions.py" )
include( "CaloDetMgrDetDescrCnv/CaloDetMgrDetDescrCnv_joboptions.py" )
include( "IdDictDetDescrCnv/IdDictDetDescrCnv_joboptions.py" )

include( "LArConditionsCommon/LArConditionsCommon_MC_jobOptions.py" )
include( "LArConditionsCommon/LArIdMap_MC_jobOptions.py" )

include( "AthenaPoolCnvSvc/AthenaPool_jobOptions.py" )

theApp.EvtMax=1 

MessageSvc = svcMgr.MessageSvc
MessageSvc.OutputLevel  = INFO
MessageSvc.defaultLimit = 1000000;
MessageSvc.Format       = "% F%20W%S%7W%R%T %0W%M"

##############################################
# Writing POOL and COOL 
if len(ObjectList)>0 : 

 include("RegistrationServices/IOVRegistrationSvc_jobOptions.py")

 from AthenaCommon.AlgSequence import AlgSequence
 from RegistrationServices.OutputConditionsAlg import OutputConditionsAlg
 topSequence += OutputConditionsAlg()
 OutputConditionsAlg = topSequence.OutputConditionsAlg
 
 # List of objects container type#key#foldername
 OutputConditionsAlg.ObjectList = ObjectList 
 OutputConditionsAlg.IOVTagList = TagList
 svcMgr.ToolSvc.OutputConditionsAlgTool.OutputFile = OutputPOOLFileName

 # Set flag to register and run interval Run1/Event1 to Run2/Event2
 #   Usually, only need to set Run1, others go to default
 # #

 OutputConditionsAlg.WriteIOV = WriteIOV
 OutputConditionsAlg.Run1     = 0
 OutputConditionsAlg.LB1   = 0

 #  OutputConditionsAlg.Run2     = 56411

 # Set the connection string
 include ( "IOVDbSvc/IOVDbSvc_jobOptions.py" )

 svcMgr.IOVDbSvc.dbConnection="impl=cool;techno=sqlite;schema=LArIdMapFix_test.db;X:OFLP200"

 # For schema creation - only should be used when creating the folder,
 # i.e. the first time
 IOVRegSvc = Service( "IOVRegistrationSvc" )
 IOVRegSvc.OutputLevel  = DEBUG
 IOVRegSvc.RecreateFolders = False

