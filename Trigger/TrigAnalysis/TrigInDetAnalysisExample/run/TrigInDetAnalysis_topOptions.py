# get a handle on the ServiceManager which holds all the services
from AthenaCommon.AppMgr import ToolSvc

from AthenaCommon.AppMgr import ServiceMgr
# Event selector
import AthenaPoolCnvSvc.ReadAthenaPool
#EventSelector.BackNavigation = True

# Import relevant libs
import os
import commands
import string

#### Define ESD location on castor, start lumi block, end lumi block --> to be used for non merged ESDs
#path = "/castor/cern.ch/grid/atlas/caf/atlcal/perm/trigger/data09_900GeV/physics_MinBias/0142191/data09_900GeV.00142191.physics_MinBias.hltrecon.ESD.c53_c55"
#startlb = 141
#endlb   = 233
#
## Actual code to add ESD on InputCollections
#status,output = commands.getstatusoutput('nsls '+path)
#output = output.splitlines()
#istart=0
#for i in xrange(0,len(output)) :
#    if output[i].count(path) < 1:
#        output[i] = path+"/"+output[i]
#    output[i] = output[i].replace('//','/')
#    pos = output[i].find("lb")
#    ilb = int(output[i][pos+2:pos+6])
#    if  pos>0:
#        if ilb>=startlb and ilb<=endlb:
#            ESDfile = "root://castoratlas/"+output[i]
#            print ESDfile
#            if istart==0:
#                ServiceMgr.EventSelector.InputCollections  = [ ESDfile ]
#                istart=1
#            else:
#                ServiceMgr.EventSelector.InputCollections += [ ESDfile ]

### Define ESD location on castor, start file, end file --> to be used for merged ESDs
path = "/castor/cern.ch/grid/atlas/caf/atlcal/perm/trigger/data09_900GeV/physics_MinBias/0142191/data09_900GeV.00142191.physics_MinBias.hltmerge.ESD.c53_c55_m261"
startlb = 1
endlb   = 43

# Actual code to add ESD on InputCollections
status,output = commands.getstatusoutput('nsls '+path)
output = output.splitlines()
istart=0
for i in xrange(0,len(output)) :
    if output[i].count(path) < 1:
        output[i] = path+"/"+output[i]
    output[i] = output[i].replace('//','/')
    pos = output[i].find("_0")
    ilb = int(output[i][pos+2:pos+5])
    if  pos>0:
        if ilb>=startlb and ilb<=endlb:
            ESDfile = "root://castoratlas/"+output[i]
            print ESDfile
            if istart==0:
                ServiceMgr.EventSelector.InputCollections  = [ ESDfile ]
                istart=1
            else:
                ServiceMgr.EventSelector.InputCollections += [ ESDfile ]


# Athena-Aware NTuple making Tools
CBNTAthenaAware = True
include ("CBNT_Athena/CBNT_AthenaAware_jobOptions.py")
include ("CBNT_Athena/CBNT_EventInfo_jobOptions.py")

# list of the algorithms to be executed at run time
from TrigInDetAnalysisExample.TrigInDetAnalysisExampleConf import TrigInDetAnalysisConfigSkeleton
topSequence.CBNT_AthenaAware += TrigInDetAnalysisConfigSkeleton()
TrigInDetAnalysisConfigSkeleton = TrigInDetAnalysisConfigSkeleton()
TrigInDetAnalysisConfigSkeleton.OutputLevel = DEBUG

##### Trigger Decision Part #####################

## set up trigger decision tool
from TrigDecisionTool.TrigDecisionToolConf import Trig__TrigDecisionTool
tdt = Trig__TrigDecisionTool("TrigDecisionTool")
ToolSvc += tdt
tdt.OutputLevel = INFO

# might be needed for TriggerConfigGetter...
from RecExConfig.RecFlags  import rec
rec.readRDO=False
rec.readESD=True
#rec.readAOD=True
rec.doWriteAOD=False
rec.doWriteESD=False

# To read files with trigger config stored as in-file meta-data, i.e. 13.0.40
# and above: ds. To read AOD produced with 13.0.30 you need to change ds to aod:
from TriggerJobOpts.TriggerFlags import TriggerFlags
TriggerFlags.configurationSourceList = ['ds']
# set up trigger config service
from TriggerJobOpts.TriggerConfigGetter import TriggerConfigGetter
cfg =  TriggerConfigGetter()

##########################################
# setup TTree registration Service
# save ROOT histograms and Tuple
from GaudiSvc.GaudiSvcConf import THistSvc
ServiceMgr += THistSvc()

ServiceMgr.THistSvc.Output = [ "AANT DATAFILE='IDTrackingCBNT.aan.root' OPT='RECREATE'" ]
from AnalysisTools.AnalysisToolsConf import AANTupleStream
topSequence += AANTupleStream()
AANTupleStream = AANTupleStream()
AANTupleStream.ExtraRefNames = [ "StreamESD","Stream1" ]
AANTupleStream.OutputName = 'IDTrackingCBNT.aan.root'
AANTupleStream.WriteInputDataHeader = True
AANTupleStream.OutputLevel = WARNING

# Set output level threshold (2=DEBUG, 3=INFO, 4=WARNING, 5=ERROR, 6=FATAL )
ServiceMgr.MessageSvc.OutputLevel = DEBUG

theApp.EvtMax = -1

from GaudiSvc.GaudiSvcConf import AuditorSvc
ServiceMgr.AuditorSvc.Auditors  += [ "ChronoAuditor"]

AthenaPoolCnvSvc = Service("AthenaPoolCnvSvc")
AthenaPoolCnvSvc.UseDetailChronoStat = TRUE

from TriggerJobOpts.TriggerConfigGetter import TriggerConfigGetter
TriggerConfigGetter( "ReadPool" )

#to dump ESD/AOD content (do only if theApp.EvtMax = 1)
#StoreGateSvc = Service( "StoreGateSvc" )
#StoreGateSvc.Dump = True 

