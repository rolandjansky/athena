doMonitoring=False
doWriteTAG=False
doWriteAOD=False
doWriteESD=False
doCBNT=False
doAOD=True
doTruth=False


#PoolRDOInput=[""]
# number of event to process (can be modified externally)
if not 'EvtMax' in dir():
     EvtMax=2000

OutputLevel = INFO 

#-----------------------------------------------------------
include("RecExCommon/RecExCommon_flags.py")
#-----------------------------------------------------------



#from TriggerJobOpts.TriggerConfigGetter import TriggerConfigGetter
#TriggerConfigGetter("ReadPoolRDO")

include ("RecExCommon/RecExCommon_topOptions.py")

from TrigDecisionMaker.TrigDecisionMakerConfig import WriteTrigDecision
trigDecWriter = WriteTrigDecision()


from TrigDecisionTool.TrigDecisionToolConf import Trig__TrigDecisionTool
tdt = Trig__TrigDecisionTool("TrigDecisionTool")
ToolSvc += tdt

 
 
########Setup egamma monitoring ################
from GaudiSvc.GaudiSvcConf import THistSvc
THistSvc = Service ("THistSvc")
THistSvc.Output += ["egammaMon DATAFILE='egammaMon.root' OPT='RECREATE'"]

from AthenaMonitoring.DQMonFlags import DQMonFlags
DQMonFlags.monManEnvironment         = "tier0ESD"
DQMonFlags.monManFileKey = "egammaMon"
DQMonFlags.useTrigger = True 

include ("egammaPerformance/egammaMonitoring_jobOptions.py")
#include ("egammaPerformance/egamma_MonitorTool.py")

