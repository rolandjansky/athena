from EventTagAlgs.EventTagGlobal import EventTagGlobal

if not EventTagGlobal.InitEventTagBuilder:
    raise RunTimeError('EventTagBuilder has not been initialized')

include ( "ElectronPhotonTagTools/PhotonTagTool_jobOptions.py" )

from EventTagAlgs.EventTagAlgsConf import PhotonTagBuilder
PhotonTagBuilder   = PhotonTagBuilder(
    name           = "PhotonTagBuilder",
    PhotonTagTool  = PhotonTagTool,
    AttributeList  = EventTagGlobal.AttributeList,
    MaxNumber      = 4,
    CheckAttribute = True)
topSequence += PhotonTagBuilder
