from EventTagAlgs.EventTagGlobal import EventTagGlobal

if not EventTagGlobal.InitEventTagBuilder:
    raise RunTimeError('EventTagBuilder has not been initialized')

include ( "JetMissingEtTagTools/MissingETTagTool_jobOptions.py" )

from EventTagAlgs.EventTagAlgsConf import MissingETTagBuilder
MissingETTagBuilder     = MissingETTagBuilder(
    name                = "MissingETTagBuilder",
    JetMissingEtTagTool = JetMissingEtTagTool,
    AttributeList       = EventTagGlobal.AttributeList,
    CheckAttribute      = True)
topSequence += MissingETTagBuilder
