# get a handle on the ServiceManager which holds all the services
from AthenaCommon.AppMgr import ToolSvc

from AthenaCommon.AppMgr import ServiceMgr
# Event selector
import AthenaPoolCnvSvc.ReadAthenaPool
#EventSelector.BackNavigation = True

from AthenaCommon.GlobalFlags import GlobalFlags
GlobalFlags.DetGeo.set_atlas()
GlobalFlags.DataSource.set_data()
from IOVDbSvc.CondDB import conddb

# Import relevant libs
import os
import commands
import string


############################################################################################################################################################
# INPUT FILES
############################################################################################################################################################
from AthenaCommon.AthenaCommonFlags import jobproperties as jp

### Define AOD/ESD location on castor, start lumi block, end lumi block --> to be used for merged AOD/ESD
path = "/castor/cern.ch/grid/atlas/tzero/prod1/perm/data10_7TeV/express_express/0155116/data10_7TeV.00155116.express_express.merge.AOD.f255_m471"
startlb = 1  #1
endlb   = 10 #10

# Actual code to add ESD on InputCollections
status,output = commands.getstatusoutput('nsls '+path)
output = output.splitlines()
istart=0
for i in xrange(0,len(output)) :
    if output[i].count(path) < 1:
        output[i] = path+"/"+output[i]
    output[i] = output[i].replace('//','/')
    if i>=startlb and i<=endlb:
        ESDfile = "root://castoratlas/"+output[i]
        print ESDfile
        if istart==0:
            jp.AthenaCommonFlags.FilesInput = [ ESDfile ]
            istart=1
        else:
            jp.AthenaCommonFlags.FilesInput += [ ESDfile ]
            
from RecExConfig.RecFlags import rec
rec.AutoConfiguration=['everything']
rec.readESD.set_Value_and_Lock(True)

ServiceMgr.EventSelector.InputCollections = jp.AthenaCommonFlags.FilesInput()


############################################################################################################################################################
# DEFINE ANALYSIS
############################################################################################################################################################

# Athena-Aware NTuple making Tools
CBNTAthenaAware = True
include ("CBNT_Athena/CBNT_AthenaAware_jobOptions.py")
include ("CBNT_Athena/CBNT_EventInfo_jobOptions.py")

# list of the algorithms to be executed at run time
from TrigInDetAnalysisExample.TrigInDetAnalysisExampleConf import TrigInDetAnalysisConfigSkeleton
topSequence.CBNT_AthenaAware += TrigInDetAnalysisConfigSkeleton()
TrigInDetAnalysisConfigSkeleton = TrigInDetAnalysisConfigSkeleton()
TrigInDetAnalysisConfigSkeleton.OutputLevel = FATAL

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

# Set output level threshold (2=DEBUG, 3=INFO, 4=WARNING, 5=ERROR, 6=FATAL )
ServiceMgr.MessageSvc.OutputLevel = INFO

ServiceMgr.THistSvc.Output = [ "AANT DATAFILE='run_0152116.root' OPT='RECREATE'" ]
from AnalysisTools.AnalysisToolsConf import AANTupleStream
topSequence += AANTupleStream()
AANTupleStream = AANTupleStream()
AANTupleStream.ExtraRefNames = [ "StreamESD","Stream1" ]
AANTupleStream.OutputName = 'run_0152116.root'
AANTupleStream.WriteInputDataHeader = True
AANTupleStream.OutputLevel = FATAL

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



MessageSvc.Format = "% F%80W%S%7W%R%T %0W%M"
MessageSvc.defaultLimit = 9999999  # all messages
