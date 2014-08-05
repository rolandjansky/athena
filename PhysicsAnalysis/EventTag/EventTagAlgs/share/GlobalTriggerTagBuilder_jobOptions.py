from EventTagAlgs.EventTagGlobal import EventTagGlobal

if not EventTagGlobal.InitEventTagBuilder:
    raise RunTimeError('EventTagBuilder has not been initialized')

include ( "EventTagUtils/GlobalTriggerTagTool_jobOptions.py" )

from EventTagAlgs.EventTagAlgsConf import GlobalTriggerTagBuilder
GlobalTriggerTagBuilder   = GlobalTriggerTagBuilder(
     name                 = "GlobalTriggerTagBuilder",
     GlobalTriggerTagTool = GlobalTriggerTagTool,
     AttributeList        = EventTagGlobal.AttributeList,
     MaxNumber            = [24, 32, 32],
     CheckAttribute       = False)

topSequence += GlobalTriggerTagBuilder
