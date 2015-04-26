from EventTagAlgs.EventTagGlobal import EventTagGlobal

if not EventTagGlobal.InitEventTagBuilder:
    raise RunTimeError('EventTagBuilder has not been initialized')

include ( "EventTagUtils/RawInfoSummaryForTagTool_jobOptions.py" )

from EventTagAlgs.EventTagAlgsConf import RawInfoSummaryTagBuilder
RawInfoSummaryTagBuilder   = RawInfoSummaryTagBuilder(
     name               = "RawInfoSummaryTagBuilder",
     RawInfoSummaryForTagTool = RawInfoSummaryForTagTool,
     AttributeList      = EventTagGlobal.AttributeList,
     CheckAttribute     = False)
topSequence += RawInfoSummaryTagBuilder
