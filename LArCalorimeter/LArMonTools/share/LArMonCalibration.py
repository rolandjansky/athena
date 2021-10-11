import commands
import os
import os.path

online = True
ReadCOOL=True
Type='Delay'

if online:
   include("RecExOnline/SimpleLarCondFlags.py")
   if 'partition' not in dir():                
      print("problem: partition not defined")
   if not 'runAccumulator' in dir(): 
      runAccumulator = False # :average mode, = True:transparent mode
else:
   include("LArMonTools/LArMonVariables.py")   

include("LArConditionsCommon/LArMinimalSetup.py")
include ("LArMonTools/LArMonByteStream.py")      

## get a handle to the default top-level algorithm sequence
from AthenaCommon.AlgSequence import AlgSequence           
topSequence = AlgSequence()                                

## get a handle to the ApplicationManager, to the ServiceManager and to the ToolSvc
from AthenaCommon.AppMgr import (theApp, ServiceMgr as svcMgr,ToolSvc)             

include ("LArROD/LArFebErrorSummaryMaker_jobOptions.py")
topSequence.LArFebErrorSummaryMaker.CheckAllFEB=False   
topSequence.LArFebErrorSummaryMaker.PartitionId = partition
include("LArMonTools/LArMonManager.py")                    
svcMgr.IOVDbSvc.GlobalTag="COMCOND-MONC-003-00"            

## Define ONE LArBadChannelMask for all Monitoring tools
ProblemsToMask=[
    "deadReadout","deadPhys","short","almostDead",
    "highNoiseHG","highNoiseMG","highNoiseLG","sporadicBurstNoise"
]

conddb.addFolder("LAR","/LAR/BadChannels/BadChannels")
conddb.addFolder("LAR","/LAR/BadChannels/MissingFEBs")

## Use different jobOpts, to ignore PS FEBs in EM calib runs, and reversed.
if online and partition=='LArgEm':
   include("LArMonTools/LArFEBMon_calibEMB_jobOptions.py")
elif online and partition=='BarrelPS':
   include("LArMonTools/LArFEBMon_calibPS_jobOptions.py")
else:
   include("LArMonTools/LArFEBMon_jobOptions.py")

## To select additional monitoring tools from the TDAQ GUI
if online:
   cmd='is_ls -p %(partition)s -n Monitoring -v -R "Monitoring"' % {"partition" : os.environ['TDAQ_PARTITION'] }

   fi, fo = os.popen2(cmd,'t')
   for line in fo:
      job = line.strip().split(' ',15)[0]
      print('line=%(line)s job=%(job)s' % {"line" : line, "job": job})
      if job == 'DigitNoiseMon' :
         include("LArMonTools/LArDigitNoiseMonTool_jobOptions.py")
      if job == 'FebNoiseMon' :
            include("LArMonTools/LArFebNoiseMonTool_jobOptions.py")
      if job == 'FebNoiseMonAlt' :
         include("LArMonTools/LArFebNoiseMonAltTool_jobOptions.py")
      if job == 'OddCellMon' :
         include("LArMonTools/LArOddCellsMonTool_jobOptions.py")
   fo.close

else:
   include("LArMonTools/LArFebNoiseMonTool_jobOptions.py")

## Online database path
if online:
   svcMgr.PoolSvc.ReadCatalog+=["xmlcatalog_file:/sw/DbData/poolcond/PoolCat_comcond.xml"]
   svcMgr.PoolSvc.SortReplicas = False

include("LArMonTools/LArMonCommonTrailer_jobOptions.py")
