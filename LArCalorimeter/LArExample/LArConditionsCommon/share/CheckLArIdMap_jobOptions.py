###############################################################
#
# Python script for Checking LAr Identifier and mapping
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
MessageSvc = Service( "MessageSvc" )
MessageSvc.OutputLevel      = DEBUG
theApp.EvtMax = 2

#--------------------------------------------------------------
# pick a DetDescrVersion
#--------------------------------------------------------------

from AthenaCommon.GlobalFlags import globalflags
globalflags.DataSource.set_Value_and_Lock('data')
globalflags.InputFormat.set_Value_and_Lock('bytestream')
globalflags.DetGeo.set_Value_and_Lock('commis')

from AthenaCommon.JobProperties import jobproperties
jobproperties.Global.DetDescrVersion = "ATLAS-CommNF-00-00-00"

#--------------------------------------------------------------
# load shared libs, converter, etc
#--------------------------------------------------------------
include ("LArConditionsCommon/DumpLArCondData_Config.py")

# 
include( "LArConditionsCommon/LArIdMap_comm_jobOptions.py" )

svcMgr.IOVDbSvc.GlobalTag = "COMCOND-ES1C-001-00"

# DetDescrCnvSvc.DoIdChecks = True

#--------------------------------------------------------------
# initialize Athena, then go to first event 
#--------------------------------------------------------------
theApp.initialize() 
theApp.nextEvent()

# IdHelpers
onlineID = PyKernel.retrieveDet(g.LArOnlineID,"LArOnlineID") 
caloCellID = PyKernel.retrieveDet(g.CaloCell_ID,"CaloCell_ID") 
emID = PyKernel.retrieveDet(g.LArEM_ID,"LArEM_ID") 
hecID = PyKernel.retrieveDet(g.LArHEC_ID,"LArHEC_ID") 
fcalID = PyKernel.retrieveDet(g.LArFCAL_ID,"LArFCAL_ID") 

# CablingService 
import AthenaPython.PyAthena as PyAthena
larCablingSvc = PyAthena.py_tool("LArCablingService")


#--------------------------------------------------------------
# access by identifier 
#--------------------------------------------------------------

# Identifier = PyLCGDict.makeClass('Identifier')  


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

# loop over all FCAL channels 

# disconnected
n_disc=0 
iter_disc = fcalID.disc_fcal_begin()
iter_disc_e = fcalID.disc_fcal_end()
while iter_disc != iter_disc_e :
   
  id = iter_disc.__deref__() 

  sid = larCablingSvc.createSignalChannelID(id) 
  print "disc off/onl id = ", fcalID.print_to_string(id),onlineID.print_to_string(sid)
  n_disc= n_disc+1
  iter_disc.__preinc__()


# connected
n=0 
iter = fcalID.fcal_begin()
iter_e = fcalID.fcal_end()
while iter != iter_e :
  id = iter.__deref__() 
  sid = larCablingSvc.createSignalChannelID(id)   
  print "conn off/onl id = ", id.get_compact(), sid.get_compact(), fcalID.print_to_string(id),onlineID.print_to_string(sid)
  iter.__preinc__()
  n=n+1

print "  number of connected FCAL channels ", n 
print "  number of disconnected FCAL channels ", n_disc



