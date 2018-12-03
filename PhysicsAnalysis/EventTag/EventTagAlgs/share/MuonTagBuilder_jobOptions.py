from EventTagAlgs.EventTagGlobal import EventTagGlobal

if not EventTagGlobal.InitEventTagBuilder:
    raise RunTimeError('EventTagBuilder has not been initialized')

include ( "MuonTagTools/MuonTagTool_jobOptions.py" )

from EventTagAlgs.EventTagAlgsConf import MuonTagBuilder
MuonTagBuilder  = MuonTagBuilder(
    name           = "MuonTagBuilder",
    MuonTagTool    = MuonTagTool,
    AttributeList  = EventTagGlobal.AttributeList,
    MaxNumber      = 6,
    CheckAttribute = True)
topSequence += MuonTagBuilder

