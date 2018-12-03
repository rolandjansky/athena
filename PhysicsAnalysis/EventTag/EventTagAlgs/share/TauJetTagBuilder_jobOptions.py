from EventTagAlgs.EventTagGlobal import EventTagGlobal

if not EventTagGlobal.InitEventTagBuilder:
    raise RunTimeError('EventTagBuilder has not been initialized')

include ( "TauTagTools/TauJetTagTool_jobOptions.py" )

from EventTagAlgs.EventTagAlgsConf import TauJetTagBuilder
TauJetTagBuilder = TauJetTagBuilder(
     name            = "TauJetTagBuilder",
     TauJetTagTool   = TauJetTagTool,
     AttributeList   = EventTagGlobal.AttributeList,
     MaxNumber       = 4,
     CheckAttribute  = True)
topSequence += TauJetTagBuilder
