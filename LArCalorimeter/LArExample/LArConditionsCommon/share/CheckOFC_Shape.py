###############################################################
#
# Python script for accessing LAr Conditions data in DetectorStore
#
#       Select run number, and load Conditions jobOpt
#       Access OFC/Shape data in conditions DB. 
#       generate 5 sample according to shape, with a fixed delay
#       apply ofc to reconstruct peak/time.
#==============================================================

include( "AthenaCommon/Atlas_Gen.UnixStandardJob.py" )

#--------------------------------------------------------------
# pick a proper run number for conditions
#--------------------------------------------------------------
EventSelector = svcMgr.EventSelector
# EventSelector.RunNumber=8035 
EventSelector.RunNumber=7815 
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

#--------------------------------------------------------------
# load dumper class
#--------------------------------------------------------------
from LArConditionsCommon.LArCondDataDumper import *


#--------------------------------------------------------------
# 0. load conditions data 
#--------------------------------------------------------------

#  LArDataArea ="/usatlas/scratch2/hma/LArCondData/intr130"
include( "LArConditionsCommon/LArConditionsCommon_comm_jobOptions.py" )
include( "LArConditionsCommon/LArIdMap_comm_jobOptions.py" )

# IOVDbSvc.GlobalTag="HEAD"
IOVDbSvc.GlobalTag="LAR-comm-02"

#--------------------------------------------------------------
# initialize Athena, then go to first event 
#--------------------------------------------------------------
theApp.initialize() 
theApp.nextEvent()

# Create a dumper object with output level for printout
larCablingSvc = theApp.toolsvc().create("LArCablingService","LArCablingService")._itool
# dumper =  LArCondDataDumper(larCablingSvc, INFO)
dumper =  LArCondDataDumper(larCablingSvc, DEBUG)

#--------------------------------------------------------------
# 1. access by identifier 
#--------------------------------------------------------------

Identifier = PyLCGDict.makeClass('Identifier')  

# retrieve from DetectorStore 
physCals = PyKernel.retrieveDet(g.LArMphysOverMcalComplete,"LArMphysOverMcal")
ofcs = PyKernel.retrieveDet(g.LArOFCComplete,"LArOFC")
shapes = PyKernel.retrieveDet(g.LArShapeComplete,"LArShape")
# waves = PyKernel.retrieveDet(g.LArPhysWaveContainer,"LArPhysWave")
onlineID = ofcs.onlineHelper()

LArConditionsContainer("LArOFCComplete")
LArConditionsContainer("LArShapeComplete")

####################################################
def measure (samples,ofc_a,ofc_b) :
 sum=0
 sum_t=0
 for i in range(0,5): 
 #   print " sample, ofc_a,b =" , samples[i], " " ,ofc_a[i] , " " , ofc_b[i]
   sum += samples[i]*ofc_a[i]   
   sum_t += samples[i]*ofc_b[i]   

 t = sum_t/sum 
 result = [sum,t]
 return result 
###################################################


# Loop Over Gains
ehist = ROOT.TH1D("e","e",100,0.9,1.1)
thist = ROOT.TH1D("t","t",100,-5.,5.)


for gain in range(ofcs.minGain(), ofcs.nGains()):

   print "    Accessing OFC, gain =  ", gain
   # Give Conditions
   n=0
   for obj,id in ofcs.conditionsIter(gain):
     if ( not obj.isEmpty() ):
       n=n+1  
       str_id1 = onlineID.print_to_string(id)
       feb_id = onlineID.feb_Id(id) 

       shape = shapes.get(id,gain).m_vShape  
       mphysovermcal = physCals.MphysOverMcal(id,gain) 
       if len(shape)==0 :
         print str_id1, " shape  size = 0 "
         continue 

       smax=0 
       j=-1
       for i in range(0,len(shape)): 
        if(shape[i]>smax):
          smax=shape[i] 
          j=i 
       print str_id1, " Shape max =", smax, "at i= ", j 

       shape_length  = len(shape) 

       if(j < 48):
           j=48
       if(j>shape_length-48-1):
           j=shape_length-48-1

       delay = j-48
       print   " OFC size = ", len(obj.m_vOFC_a)," shape size ",len(shape)," delay = ", delay 
       if(smax<=0): 
         print " ERROR smax = 0" 
         continue 

       samples = [shape[j-48],shape[j-24],shape[j],shape[j+24],shape[j+48] ]
       if(delay>=len(obj.m_vOFC_a)):
           print "delay too large, ofc size = ",len(obj.m_vOFC_a)
           shape_str = " shape= "
           for sh in shape :
               shape_str = shape_str+" " +str(sh)
           print shape_str
           continue
       
       ofc_a = obj.m_vOFC_a[delay]
       ofc_b = obj.m_vOFC_b[delay]
       result = measure(samples,ofc_a,ofc_b)

#       e = result[0] / smax
       e = result[0] 
       t   = result[1] 
       ehist.Fill(e)
       thist.Fill(t)
       print " e and t = ", e, " " , t ," phys_cal",mphysovermcal 

   print " Number of OFCs ", n


be = 0 # barrel
pn=  1  # 1:positive, 0:negative 
ft= 18  # feedthrough
slot=11 # slot 
chan=0 # channel 

sid = onlineID.channel_Id(be,pn,ft,slot,chan)
id_str = onlineID.print_to_string(sid)
off_id = larCablingSvc.cnvToIdentifier(sid)
print id_str

shape = shapes.get(sid,0).m_vShape  
if len(shape)==0 :
     print id_str, " shape  size = 0 "

j = 85 
delay = j-48  

samples = [shape[j-48],shape[j-24],shape[j],shape[j+24],shape[j+48] ]
ofc = ofcs.get(sid,0) 
ofc_a = ofc.m_vOFC_a[delay]
ofc_b = ofc.m_vOFC_b[delay]

result = measure(samples,ofc_a,ofc_b)
shape = shapes.Shape(sid,0,delay,10)

for i in range(0,5):
 print i, samples[i], shape[i]

print " e and t = ", result[0], " " , result[1] 
