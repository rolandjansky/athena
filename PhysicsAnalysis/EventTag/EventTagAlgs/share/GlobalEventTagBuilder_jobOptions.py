from EventTagAlgs.EventTagGlobal import EventTagGlobal

EventTagGlobal.InitEventTagBuilder = True

if not EventTagGlobal.InitEventTagBuilder:
    raise RunTimeError('EventTagBuilder has not been initialized')

include ( "EventTagUtils/GlobalEventTagTool_jobOptions.py" )

from EventTagAlgs.EventTagAlgsConf import GlobalEventTagBuilder
GlobalEventTagBuilder   = GlobalEventTagBuilder(
     GlobalEventTagTool = GlobalEventTagTool,
     AttributeList      = EventTagGlobal.AttributeList,
     CheckAttribute     = True)

topSequence += GlobalEventTagBuilder
