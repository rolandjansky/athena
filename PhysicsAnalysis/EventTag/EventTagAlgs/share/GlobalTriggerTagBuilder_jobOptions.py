from EventTagAlgs.EventTagGlobal import EventTagGlobal

if not EventTagGlobal.InitEventTagBuilder:
    raise RunTimeError('EventTagBuilder has not been initialized')

include ( "EventTagUtils/GlobalTriggerTagTool_jobOptions.py" )

from EventTagAlgs.EventTagAlgsConf import GlobalTriggerTagBuilder
GlobalTriggerTagBuilder   = GlobalTriggerTagBuilder(
     name                 = "GlobalTriggerTagBuilder",
     GlobalTriggerTagTool = GlobalTriggerTagTool,
     AttributeList        = EventTagGlobal.AttributeList,
     MaxNumber            = [48, 256, 32], # first is L1 (TBP, TAP, TAV), second is HLT, last parameter is not used anylonger
     CheckAttribute       = False)

topSequence += GlobalTriggerTagBuilder
