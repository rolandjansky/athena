###############################################################
#
# Python script for accessing LAr Conditions data in DetectorStore
#
#==============================================================
#use McEventSelector

include( "AthenaCommon/Atlas_Gen.UnixStandardJob.py" )

#--------------------------------------------------------------
# pick a proper run number for conditions
#--------------------------------------------------------------
EventSelector = svcMgr.EventSelector
EventSelector.RunNumber=9000 
EventSelector.EventsPerRun=100000;
EventSelector.FirstEvent=1
MessageSvc = svcMgr.MessageSvc
MessageSvc.OutputLevel      = INFO
theApp.EvtMax = 2

#--------------------------------------------------------------
# pick a DetDescrVersion
#--------------------------------------------------------------
# DetDescrVersion = "DC2" 
# DetDescrVersion = "Rome-Initial" 

from AthenaCommon.GlobalFlags import globalflags
globalflags.DataSource.set_Value_and_Lock('geant4')
globalflags.InputFormat.set_Value_and_Lock('bytestream')
globalflags.DetGeo.set_Value_and_Lock('atlas')

from AthenaCommon.JobProperties import jobproperties
#jobproperties.Global.DetDescrVersion = "ATLAS-CSC-03-00-00"
#jobproperties.Global.DetDescrVersion = "ATLAS-CSC-01-00-00"
jobproperties.Global.DetDescrVersion = "ATLAS-GEO-16-00-00"
# jobproperties.Global.DetDescrVersion = "ATLAS-CSC-02-00-00"
# LArIOVDbTag = "CSC02-I-QGSP_BERT"


from IOVDbSvc.CondDB import conddb
#conddb.setGlobalTag('OFLCOND-CSC-00-01-00')
conddb.setGlobalTag('OFLCOND-SIM-01-00-00')


#--------------------------------------------------------------
# load shared libs, converter, etc
#--------------------------------------------------------------
include ("LArConditionsCommon/DumpLArCondData_Config.py")

#--------------------------------------------------------------
# load dumper class
#--------------------------------------------------------------
from LArConditionsCommon.LArCondDataDumper import *


#--------------------------------------------------------------
# pick conditions data 
#--------------------------------------------------------------
# UseFullLArShape = True
include( "LArConditionsCommon/LArConditionsCommon_MC_jobOptions.py" )
include( "LArConditionsCommon/LArIdMap_MC_jobOptions.py" )

#--------------------------------------------------------------
# initialize Athena, then go to first event 
#--------------------------------------------------------------
theApp.initialize() 
theApp.nextEvent()

ToolSvc =svcMgr.ToolSvc

from LArCabling.LArCabingConf import LArCablingService

ToolSvc += LArCablingService()
# Create a dumper object with output level for printout

#--------------------------------------------------------------
# access by identifier 
#--------------------------------------------------------------

Identifier = PyLCGDict.makeClass('Identifier')  

# retrieve from DetectorStore 
ramps = PyKernel.retrieveDet(g.LArRampMC,"LArRamp")
onlineID = ramps.onlineHelper()
larCablingSvc =ramps.larCablingSvc()

dumper =  LArCondDataDumper(larCablingSvc, INFO)

be = 0 # barrel
pn= 1  # positive negative 
ft= 0  # feedthrough
slot=10 # slot 
chan=0 # channel 

print "*****************************"
print " "
print "Begin single channel print out"
print " "
print "*****************************"

sid = onlineID.channel_Id(be,pn,ft,slot,chan)
off_id = larCablingSvc.cnvToIdentifier(sid)
id_str1 = onlineID.print_to_string(sid)
id_str2 = onlineID.print_to_string(off_id)
print id_str1
print id_str2

for gain in range(3):
 # access vector<float> 
 print " for gain = ", gain 
 r = ramps.get(sid,gain)
 if r.isEmpty() : 
  print " data is empty for this gain" 
 else: 
  dumper.printLarCondObject(r)

print "*****************************"
print " "
print "End single channel print out"
print " "
print "*****************************"


#--------------------------------------------------------------
# loop over all valid data, for all containers 
#--------------------------------------------------------------

ListofType =[]
ListofKey  =[]

ListofType +=[ROOT.LArRampMC]
ListofKey +=["LArRamp"]

ListofType +=[ROOT.LArAutoCorrMC]
ListofKey +=["LArAutoCorr"]

ListofType +=[ROOT.LArShape32MC]
ListofKey +=["LArShape"]

ListofType +=[ROOT.LAruA2MeVMC]
ListofKey +=["LAruA2MeV"]

ListofType +=[ROOT.LArDAC2uAMC]
ListofKey +=["LArDAC2uA"]

ListofType +=[ROOT.LArfSamplMC]
ListofKey +=["LArfSampl"]

ListofType +=[ROOT.LArMinBiasMC]
ListofKey +=["LArMinBias"]

ListofType +=[ROOT.LArNoiseMC]
ListofKey +=["LArNoise"]

ListofType +=[ROOT.LArMphysOverMcalMC]
ListofKey +=["LArMphysOverMcal"]

dumper.outputLevel = INFO
dumper.printAllLArCondData (ListofType,ListofKey) 


theApp.exit()
