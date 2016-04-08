# might be needed for TriggerConfigGetter...
#from RecExConfig.RecFlags  import rec
#rec.readRDO=False
#rec.readAOD=True
#rec.doWriteAOD=False
#rec.doWriteESD=False
#rec.doWriteTAG=False

# To read files with trigger config stored as in-file meta-data,
# i.e. 13.0.40 and above: ds
# To read AOD produced with 13.0.30 you need to change ds to aod: 
from TriggerJobOpts.TriggerFlags import TriggerFlags
if AnalysisTriggerToolsFlags.Release15AOD:
    from TrigDecisionTool.TrigDecisionToolConf import Trig__TrigDecisionTool
    tdt=Trig__TrigDecisionTool("TrigDecisionTool")
    ToolSvc+=tdt
    TriggerFlags.configurationSourceList = ['ds']
if AnalysisTriggerToolsFlags.Release14AOD:
    from TrigDecision.TrigDecisionConfig import TrigDecisionTool
    tdt = TrigDecisionTool()
    from AnalysisTriggerTools.AnalysisTriggerToolsFlags import AnalysisTriggerToolsFlags
    if AnalysisTriggerToolsFlags.DoFDR:
        tdt.UseFDRMode = True
    ToolSvc += tdt
    #print "AnalysisTriggerTools: release 14 AOD"
    TriggerFlags.configurationSourceList = ['ds']
if AnalysisTriggerToolsFlags.Release13AOD:
    from TrigDecision.TrigDecisionConfig import TrigDecisionTool
    tdt = TrigDecisionTool()
    from AnalysisTriggerTools.AnalysisTriggerToolsFlags import AnalysisTriggerToolsFlags
    if AnalysisTriggerToolsFlags.DoFDR:
        tdt.UseFDRMode = True
    ToolSvc += tdt
    #print "AnalysisTriggerTools: release 13 AOD"
    TriggerFlags.configurationSourceList = ['aod']
# set up trigger config service
from TriggerJobOpts.TriggerConfigGetter import TriggerConfigGetter
cfg =  TriggerConfigGetter()

