###############################################################
#
# Python script for accessing LAr Conditions data in DetectorStore
#
#    0. Select run number, and load Conditions jobOpt
#    1. Access Conditions Data using Identifier 
#         Retrieve container from DetectorStore
#         make identifier using onlineID or larCablingSvc
#         use Container's get method to access data object.
#         use LArCondDataDumper.printLarCondObject(obj) to print object
#    2. Print all channel
#         Define Type,Key to retrieve (ListofType, ListofKey)
#         print_all_lar_cond_data (ListofType,ListofKey) 
#         (set MessageSvc.OutputLevel = DEBUG for printing all channels)
#==============================================================

include( "AthenaCommon/Atlas_Gen.UnixStandardJob.py" )

#--------------------------------------------------------------
# pick a proper run number for conditions
#--------------------------------------------------------------
from AthenaCommon.AppMgr import ServiceMgr
EventSelector = ServiceMgr.EventSelector
EventSelector.RunNumber=3008077
EventSelector.EventsPerRun=100000;
EventSelector.FirstEvent=1
MessageSvc = Service( "MessageSvc" )
MessageSvc.OutputLevel      = INFO
theApp.EvtMax = 2

#--------------------------------------------------------------
# pick a DetDescrVersion
#--------------------------------------------------------------
# DetDescrVersion = "DC2" 
# DetDescrVersion = "Rome-Initial" 
#DetDescrVersion = "ATLAS-DC3-05"
# DetDescrVersion = "ATLAS-CommNF-00-00-00"

from AthenaCommon.GlobalFlags import globalflags
globalflags.DataSource.set_Value_and_Lock('data')
globalflags.InputFormat.set_Value_and_Lock('bytestream')
globalflags.DetGeo.set_Value_and_Lock('commis')

from AthenaCommon.JobProperties import jobproperties
jobproperties.Global.DetDescrVersion = "ATLAS-CommNF-07-00-00"

from IOVDbSvc.CondDB import conddb
conddb.setGlobalTag('COMCOND-ES1C-001-00')


from LArConditionsCommon.LArCondFlags import larCondFlags

larCondFlags.OFCShapeFolder='5samples3bins17phases'
larCondFlags.useShape=False
larCondFlags.SingleVersion=True
larCondFlags.SingleVersionKey="Single"

#--------------------------------------------------------------
# load shared libs, converter, etc
#--------------------------------------------------------------
include ("LArConditionsCommon/DumpLArCondData_Config.py")

#--------------------------------------------------------------
# load dumper class
#--------------------------------------------------------------
from LArConditionsCommon.LArCondDataDumper import *


#--------------------------------------------------------------
# 0. load conditions data 
#--------------------------------------------------------------
include( "LArConditionsCommon/LArConditionsCommon_comm_jobOptions.py" )
include( "LArConditionsCommon/LArIdMap_comm_jobOptions.py" )


#--------------------------------------------------------------
# initialize Athena, then go to first event 
#--------------------------------------------------------------
theApp.initialize() 
theApp.nextEvent()

#sg = PyAthena.py_svc('StoreGateSvc')
#evtInfo = sg.retrieve("EventInfo","McEventInfo")

import AthenaPython.PyAthena as PyAthena
larCablingSvc = PyAthena.py_tool("LArCablingService")

dumper =  LArCondDataDumper(larCablingSvc, INFO)
#dumper =  LArCondDataDumper(larCablingSvc, DEBUG)

#--------------------------------------------------------------
# 1. access by identifier 
#--------------------------------------------------------------

Identifier = PyLCGDict.makeClass('Identifier')  

# retrieve from DetectorStore 

onlineID = PyKernel.retrieveDet("LArOnlineID","LArOnlineID") 
caloCellID = PyKernel.retrieveDet("CaloCell_ID","CaloCell_ID") 
emID = PyKernel.retrieveDet("LArEM_ID","LArEM_ID") 
hecID = PyKernel.retrieveDet("LArHEC_ID","LArHEC_ID") 
fcalID = PyKernel.retrieveDet("LArFCAL_ID","LArFCAL_ID") 

ramps = PyKernel.retrieveDet("LArRampComplete","LArRamp")
peds = PyKernel.retrieveDet("LArPedestalComplete","Pedestal")

be = 0 # barrel
pn=  0  # 1:positive, 0:negative 
ft= 18  # feedthrough
slot=11 # slot 
chan=0 # channel 

print "*****************************"
print " "
print "Begin single channel print out"
print " "
print "*****************************"

sid = onlineID.channel_Id(be,pn,ft,slot,chan)
id_str1 = onlineID.print_to_string(sid)
print id_str1

off_id = larCablingSvc.cnvToIdentifier(sid)
id_str2 = onlineID.print_to_string(off_id)
print id_str2

for gain in range(3):
  # access vector<float> 
  print " for gain = ", gain 
  r = ramps.get(sid,gain)
  if r.isEmpty() : 
    print " data is empty for this gain" 
  else: 
    dumper.printLarCondObject(r)

  r = peds.get(sid,gain)
  if r.isEmpty() : 
    print " pedestal data is empty for this gain" 
  else: 
    dumper.printLarCondObject(r)

print "*****************************"
print " "
print "End single channel print out"
print " "
print "*****************************"


#--------------------------------------------------------------
# 2. loop over all valid data, for all containers 
#--------------------------------------------------------------

ListofType =[]
ListofKey  =[]

ListofType +=[g.LArPedestalComplete]
ListofKey +=["Pedestal"]

ListofType +=[g.LArAutoCorrComplete]
ListofKey +=["LArAutoCorr"]

ListofType +=[g.LArOFCComplete]
ListofKey +=["LArOFC"]

#ListofType +=[g.LArCaliWaveContainer]
#ListofKey +=["LArCaliWave"]

#ListofType +=[g.LArShapeComplete]
#ListofKey +=["LArShape"]

ListofType +=[g.LArRampComplete]
ListofKey +=["LArRamp"]

ListofType +=[g.LArMphysOverMcalComplete]
ListofKey +=["LArMphysOverMcal"]

dumper.printAllLArCondData (ListofType,ListofKey) 


# retrieve uA2MeV and DAC2MeV, loop over all ids.

uA2MeVs =  PyKernel.retrieveDet(g.LAruA2MeVMC,"LAruA2MeV")
DAC2uAs =  PyKernel.retrieveDet(g.LArDAC2uAMC,"LArDAC2uA")

iter = onlineID.channel_begin()
iter_e = onlineID.channel_end()

n=0 

while iter != iter_e :
  hid = iter.__deref__() 
  if larCablingSvc.isOnlineConnected(hid) : 
    x = uA2MeVs.UA2MEV( hid) 
    if x<-900. : 
      print " can not find uA2MeV for this channel ",onlineID.print_to_string(hid) 
      
    y = DAC2uAs.DAC2UA( hid) 
    if y<-900. : 
      print " can not find DAC2uA2 for this channel ",onlineID.print_to_string(hid) 
    n = n + 1  
  iter.__preinc__() 

print " total number of online connected channels ", n 

iter = emID.em_begin()
iter_e = emID.em_end()

n=0 

while iter != iter_e :
  hid = iter.__deref__() 
  x = uA2MeVs.UA2MEV( hid) 
  y = DAC2uAs.DAC2UA( hid) 
  n = n + 1  
  iter.__preinc__() 

print " total number of EM channels ", n 



theApp.exit()
