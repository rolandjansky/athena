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

muonSel = CfgMgr.CP__MuonSelectionTool("MuonSelectionTool",
                                       MuQuality=1) # Medium
ToolSvc += muonSel
	
elecSelLH = CfgMgr.AsgElectronLikelihoodTool("EleSelLikelihood",
                                             ConfigFile="ElectronPhotonSelectorTools/offline/mc15_20150429/ElectronLikelihoodMediumOfflineConfig2015.conf")
ToolSvc += elecSelLH

from ROOT import egammaPID
photonSelIsEM = CfgMgr.AsgPhotonIsEMSelector("PhotonSelIsEM",
                                             isEMMask=egammaPID.PhotonTight,
                                             ConfigFile="ElectronPhotonSelectorTools/offline/mc15_20150429/PhotonIsEMTightSelectorCutDefs.conf")
ToolSvc += photonSelIsEM
	
tauSel = CfgMgr.TauAnalysisTools__TauSelectionTool("TauSelectionTool")
ToolSvc += tauSel


metMakerAlg = CfgMgr.met__METMakerAlg( "METMakerAlg",
                                       Maker = metMaker,
                                       InputJets = "AntiKt4TopoJets_TAGcalibskim", # calibrated jet collection for TAG file format
                                       InputElectrons = "Electrons",
                                       InputPhotons = "Photons",
                                       InputTaus = "TauJets",
                                       InputMuons = "Muons_TAG_skim",
                                       METName = "MET_Reference_AntiKt4Topo_TAGcalibskim",
                                       METMapName = "METAssoc_AntiKt4EMTopo",
                                       METCoreName= "MET_Core_AntiKt4EMTopo",
                                       MuonSelectionTool=muonSel,
                                       ElectronLHSelectionTool=elecSelLH,
                                       PhotonIsEMSelectionTool=photonSelIsEM,
                                       TauSelectionTool=tauSel	
                                       )

topSequence += metMakerAlg

from EventTagAlgs.EventTagAlgsConf import MissingETTagBuilder
MissingETTagBuilder     = MissingETTagBuilder(
    name                = "MissingETTagBuilder",
    JetMissingEtTagTool = JetMissingEtTagTool,
    AttributeList       = EventTagGlobal.AttributeList,
    CheckAttribute      = True)
topSequence += MissingETTagBuilder
