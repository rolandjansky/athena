from EventTagAlgs.EventTagGlobal import EventTagGlobal

if not EventTagGlobal.InitEventTagBuilder:
    raise RunTimeError('EventTagBuilder has not been initialized')

include ( "ElectronPhotonTagTools/ElectronTagTool_jobOptions.py" )

from EventTagAlgs.EventTagAlgsConf import ElectronTagBuilder
ElectronTagBuilder  = ElectronTagBuilder(
    name            = "ElectronTagBuilder",
    ElectronTagTool = ElectronTagTool, 
    AttributeList   = EventTagGlobal.AttributeList,
    MaxNumber       = 6,
    CheckAttribute  = True)
topSequence += ElectronTagBuilder
