
from D3PDMakerConfig.D3PDMakerFlags import D3PDMakerFlags
from AthenaCommon.AlgSequence import AlgSequence,AthSequencer
from ROOT import egammaPID

sequencer = AthSequencer("HSG5ZHLLunb_Sequencer",
                         StopOverride=False)

from HSG5DPDUtils.HSG5DPDUtilsConf import HSG5__LeptonFilter
leptonFilter=HSG5__LeptonFilter("HSG5ZHLLunb_DileptonFilter",
                               ElectronFilterNameAndType="D2PDElectronSelector/HSG5ZHLLunb_ElectronSelector",
                               MuonFilterNameAndTypeVec=["D2PDMuonSelector/HSG5ZHLLunb_MuidMuonSelector",
                                                         "D2PDMuonSelector/HSG5ZHLLunb_StacoMuonSelector"])


# create electron and muon selectors
from D2PDMaker.D2PDMakerConf import D2PDElectronSelector
leptonFilter += D2PDElectronSelector( "HSG5ZHLLunb_ElectronSelector",
                                      inputCollection      = 'ElectronAODCollection',
                                      outputLinkCollection = 'HSG5ZHLLunb_LooseElectronLinkCollection',
                                      minNumberPassed      = 2,
                                      clusterEtMin         = 15.0*Units.GeV,
                                      clusterEtaMin        = -2.6,
                                      clusterEtaMax        = 2.6,
                                      electronIsEM         = egammaPID.ElectronLoose
                                      )

from HSG5DPDUtils.HSG5Selectors import MuonSelector
muSelector = MuonSelector( minNumberPassed = 2,
                           ptMin = 15.0,
                           absEtaMax = 2.6,
                           requireIsTight = True)

leptonFilter += muSelector.getMuonSelector('HSG5ZHLLunb_MuidMuonSelector', 'MuidMuonCollection',
                                           'HSG5ZHLLunb_LooseMuidMuonLinkCollection')
leptonFilter += muSelector.getMuonSelector('HSG5ZHLLunb_StacoMuonSelector', 'StacoMuonCollection',
                                           'HSG5ZHLLunb_LooseStacoMuonLinkCollection')
leptonFilter += muSelector.getMuonSelector('HSG5ZHLLunb_ThirdChainMuonSelector', 'Muons',
                                           'HSG5ZHLLunb_LooseThirdChainMuonLinkCollection')

sequencer += leptonFilter

HSG5D3PD_Stream.AcceptAlgs.append("HSG5ZHLLunb_DileptonFilter")

if False:
    # (for private production only) insert in beginning of PreD3PDSequencer
    mainSequencer = AlgSequence(D3PDMakerFlags.PreD3PDAlgSeqName(),
                                StopOverride = False)

    if not hasattr( topSequence, D3PDMakerFlags.PreD3PDAlgSeqName() ):
        topSequence += mainSequencer

    mainSequencer.insert(0,sequencer)

else:
    topSequence+=sequencer
