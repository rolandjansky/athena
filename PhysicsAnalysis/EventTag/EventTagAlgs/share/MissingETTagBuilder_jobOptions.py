from EventTagAlgs.EventTagGlobal import EventTagGlobal
from ihooks import VERBOSE

if not EventTagGlobal.InitEventTagBuilder:
    raise RunTimeError('EventTagBuilder has not been initialized')

include ( "JetMissingEtTagTools/MissingETTagTool_jobOptions.py" )

# The tools are accessible via the configurations in metFlags
from AthenaCommon import CfgMgr
from AthenaCommon.AppMgr import ToolSvc
metMaker = CfgMgr.met__METMaker("METMaker")
ToolSvc += metMaker

metMakerAlg = CfgMgr.met__METMakerAlg( "METMakerAlg",
                                       Maker = metMaker,
                                       InputJets = "AntiKt4TopoJets_TAGcalib", # calibrated jet collection for TAG file format
                                       InputElectrons = "Electrons",
                                       InputPhotons = "Photons",
                                       InputTaus = "TauJets",
                                       InputMuons = "Muons",
                                       METName = "MET_Reference_AntiKt4Topo_TAGcalib",
                                       METMapName = "METAssoc_AntiKt4EMTopo",
                                       METCoreName= "MET_Core_AntiKt4EMTopo"
                                       #OutputLevel=Lvl.VERBOSE
                                       )

topSequence += metMakerAlg

from EventTagAlgs.EventTagAlgsConf import MissingETTagBuilder
MissingETTagBuilder     = MissingETTagBuilder(
    name                = "MissingETTagBuilder",
    JetMissingEtTagTool = JetMissingEtTagTool,
    AttributeList       = EventTagGlobal.AttributeList,
    CheckAttribute      = True)
topSequence += MissingETTagBuilder
