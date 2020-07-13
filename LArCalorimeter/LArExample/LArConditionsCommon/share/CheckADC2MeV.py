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

doMC = False

#--------------------------------------------------------------
# pick a proper run number for conditions
#--------------------------------------------------------------
EventSelector = svcMgr.EventSelector
EventSelector.RunNumber=8035 
EventSelector.EventsPerRun=100000;
EventSelector.FirstEvent=1
svcMgr.MessageSvc.OutputLevel      = INFO 
theApp.EvtMax = 2

#--------------------------------------------------------------
# pick a DetDescrVersion
#--------------------------------------------------------------

from AthenaCommon.GlobalFlags import globalflags
if doMC:
  globalflags.DataSource="geant4"
else:
  globalflags.DataSource="data"
globalflags.InputFormat="bytestream"
globalflags.DetGeo="atlas"
globalflags.DetDescrVersion = "ATLAS-GEO-16-00-00"
globalflags.ConditionsTag="DEFAULTCOND"

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


if not doMC: 
#   LArDataArea ="/usatlas/scratch2/hma/LArCondData/intr130"
  include( "LArConditionsCommon/LArConditionsCommon_comm_jobOptions.py" )
  include( "LArConditionsCommon/LArIdMap_comm_jobOptions.py" )
  svcMgr.IOVDbSvc.GlobalTag = "COMCOND-ES1C-001-00"

else: 
  include( "LArRawConditions/LArRawConditions_MC_jobOptions.py" )
  include( "LArRawConditions/LArIdMap_ATLAS_jobOptions.py" )
  ToolSvc.LArADC2MeVTool.MCSym = True


theApp.Dlls += ["LArCalibUtils"]

#--------------------------------------------------------------
# initialize Athena, then go to first event 
#--------------------------------------------------------------
theApp.initialize() 
theApp.nextEvent()

Identifier = PyLCGDict.makeClass('Identifier')  
HWIdentifier = PyLCGDict.makeClass('HWIdentifier')  
onlineID = PyKernel.retrieveDet(g.LArOnlineID,"LArOnlineID") 
# emID = PyKernel.retrieveDet(g.LArEM_ID,"LArEM_ID") 

# Create a dumper object with output level for printout

import AthenaPython.PyAthena as PyAthena
larCablingSvc = PyAthena.py_tool("LArCablingService")

if doMC: 
  ramps = PyKernel.retrieveDet(g.LArRampMC,"LArRamp")
else :
  ramps = PyKernel.retrieveDet(g.LArRampComplete,"LArRamp")
  LArConditionsContainer("LArRampComplete")


print " created ramps"
uA2MeVs =  PyKernel.retrieveDet(g.LAruA2MeVMC,"LAruA2MeV")
print " created us2MeVs"
DAC2uAs =  PyKernel.retrieveDet(g.LArDAC2uAMC,"LArDAC2uA")
print " created DAC2uAs"

# adc2MeV = theApp.toolsvc().create("LArADC2MeVTool","LArADC2MeVTool",interface=g.ILArADC2MeVTool)

#-------------------------------------------------
def print_adc2mev(sid) :   
   id_str = onlineID.print_to_string(sid)
   off_id = larCablingSvc.cnvToIdentifier(sid)
   off_str = onlineID.print_to_string(off_id)
   print "compact hw id = ", sid.get_compact()
   print id_str
   print off_str

   ramp = ramps.ADC2DAC(sid,0) ; 
   uA2MeV = uA2MeVs.UA2MEV(sid) ; 
   DAC2uA = DAC2uAs.DAC2UA(sid) ; 
 
   print " uA2MeV ",uA2MeV 
   print " DAC2uA ",DAC2uA

   print " Ramp = " 

   for i in ramp : 
      print i , i*uA2MeV*DAC2uA 


def get_histogram(febid,gain) :
  
   from ROOT import TH1F 
   int_id = febid.get_compact() + (gain<<32)
   str_id = onlineID.print_to_string(feb_id)
   str_id = str_id+"gain"+str(gain) 
   if int_id not in histmap[gain].keys():
     ramp_hist = TH1F("h"+str(int_id),str_id,128,0.,128.)
     histmap[gain][int_id] = ramp_hist
     print " ramp histogram created for ", int_id, str_id 

   return histmap[gain][int_id]


#-------------------------------------------------------------------

#--------------------------------------------------------------
# 1. access by identifier 
#--------------------------------------------------------------

be = 0 # barrel
pn=  1  # 1:positive, 0:negative 
ft= 18  # feedthrough

slots = [1,2,3,4,5,6,7,8,9,10,11,12,13,14] 
for slot in slots : 
   #  for chan in range(0,128): 
   # slot=11 # slot 
   chan=10 # channel 
   # print " slot, chan = ", slot, chan 

   sid = onlineID.channel_Id(be,pn,ft,slot,chan)

   print_adc2mev(sid) 


#-------------------------------------------------


from ROOT import TFile
file = TFile("RampFeb.root","RECREATE") 
histmap1 = {} 
histmap2 = {} 
histmap3 = {} 

histmap = {0:histmap1,1:histmap2,2:histmap3}

# loop over all ramps
for gain in range(ramps.minGain(), ramps.nGains()):

   print "    Accessing Ramp, gain =  ", gain
   # Give Conditions

   n=0
   for obj,id in ramps.conditionsIter(gain):
     if ( obj.isEmpty() ):
        continue 

     str_id1 = onlineID.print_to_string(id)
     feb_id = onlineID.feb_Id(id) 
     ramp = ramps.ADC2DAC(id,gain) ; 

     if(larCablingSvc.isOnlineConnected(id)):
       hist = get_histogram(feb_id,gain) 
       chan = onlineID.channel(id) 
       hist.Fill(chan+0.1,ramp[1]) 
       slope = ramp[1]
       if gain==1 : 
          slope = slope/10 
       if gain==2 : 
          slope = slope/100 
 
       if( slope>5 or slope<0.15 ): 
        print " problem channel ", str_id1 ," gain ", gain, " ramp[1]=",ramp[1],hist.GetName();
        uA2MeV = uA2MeVs.UA2MEV(id) ; 
        DAC2uA = DAC2uAs.DAC2UA(id) ; 
 
        print " uA2MeV ",uA2MeV 
        print " DAC2uA ",DAC2uA

        print " Ramp, ADC2MeV = " 

        for i in ramp : 
           print i , i*uA2MeV*DAC2uA 


file.Write()     
file.Close()     

#adc2MeV = adc2MeV.ADC2MEV(sid,0) ; 
#print " AD2MeV = " 
#for i in adc2MeV : 
# print i 

# theApp.finalize() 

