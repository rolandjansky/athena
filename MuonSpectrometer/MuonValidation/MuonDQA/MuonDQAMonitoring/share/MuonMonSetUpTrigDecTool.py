#--------------------------------------------------------------
# Configure Trigger Decision Tool
# Compatible with the New Trigger Decision Tool (from v. 15.1.0 onward)
#--------------------------------------------------------------

# Set up trigger configuration service and metadata service is relies on, for analysis job without RecExCommon
from TrigDecisionTool.TrigDecisionToolConf import Trig__TrigDecisionTool
tdt = Trig__TrigDecisionTool("TrigDecisionTool")
ToolSvc += tdt
tdt.OutputLevel=ERROR
ToolSvc.TrigDecisionTool.Navigation.OutputLevel = ERROR

# flags needed for TriggerConfigGetter
from RecExConfig.RecFlags import rec
rec.readAOD = True

rec.readRDO = False
rec.readESD = False
rec.readTAG = False
rec.doWriteESD = False
rec.doWriteAOD = False
rec.doWriteTAG = False
 
# To read files with trigger config stored as in-file meta-data, i.e. 13.0.40 and above: ds
from TriggerJobOpts.TriggerFlags import TriggerFlags
TriggerFlags.configurationSourceList = ['ds']
 
# set up trigger config service
from TriggerJobOpts.TriggerConfigGetter import TriggerConfigGetter
cfg =  TriggerConfigGetter()
