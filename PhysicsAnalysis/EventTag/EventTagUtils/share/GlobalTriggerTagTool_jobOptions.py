include.block ("EventTagUtils/GlobalTriggerTagTool_jobOptions.py")

########### Global Trigger Tag Options ################

from RecExConfig.RecFlags import rec
if rec.doTrigger() :
    ToolSvc = Service( "ToolSvc" )
    from TrigDecisionTool.TrigDecisionToolConf import Trig__TrigDecisionTool
    tdt = Trig__TrigDecisionTool("TrigDecisionTool")
    ToolSvc += tdt

    from EventTagUtils.EventTagUtilsConf import \
    GlobalTriggerTagTool as ConfiguredGlobalTriggerTagTool
    GlobalTriggerTagTool=ConfiguredGlobalTriggerTagTool()
    ToolSvc += GlobalTriggerTagTool

else:
    print "GlobalTriggerTagTool_jobOptions.py :: WARNING: Cannot configure GlobaltriggerTagTool if rec.doTrigger()=false"
