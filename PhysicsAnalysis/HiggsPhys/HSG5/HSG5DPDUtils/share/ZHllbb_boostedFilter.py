
from D3PDMakerConfig.D3PDMakerFlags import D3PDMakerFlags
from AthenaCommon.AlgSequence import AlgSequence,AthSequencer
from ROOT import egammaPID

sequencer = AthSequencer("HSG5ZHLL_Sequencer",
                         StopOverride=False)

from D2PDMaker.D2PDMakerConf import D2PDJetSelector
sequencer += D2PDJetSelector( "HSG5ZHLL_BoostedJetFilter",
                              inputCollection      = 'CamKt12LCTopoJets',
                              outputLinkCollection = 'HSG5ZHLL_LooseBoostedJetLinkCollection',
                              minNumberPassed      = 1,
                              ptMin                = 150.0*Units.GeV)
HSG5D3PD_Stream.RequireAlgs.append("HSG5ZHLL_BoostedJetFilter")

from HSG5DPDUtils.HSG5DPDUtilsConf import HSG5__LeptonFilter
leptonFilter=HSG5__LeptonFilter("HSG5ZHLL_DileptonFilter",
                               ElectronFilterNameAndType="D2PDElectronSelector/HSG5ZHLL_ElectronSelector",
                               MuonFilterNameAndTypeVec=["D2PDMuonSelector/HSG5ZHLL_MuidMuonSelector",
                                                         "D2PDMuonSelector/HSG5ZHLL_StacoMuonSelector"])

# create electron and muon selectors
from D2PDMaker.D2PDMakerConf import D2PDElectronSelector
leptonFilter += D2PDElectronSelector( "HSG5ZHLL_ElectronSelector",
                                      inputCollection      = 'ElectronAODCollection',
                                      outputLinkCollection = 'HSG5ZHLL_LooseElectronLinkCollection',
                                      minNumberPassed      = 2,
                                      etMin                = 15.0*Units.GeV,
                                      clusterEtaMin        = -2.5,
                                      clusterEtaMax        = 2.5,
                                      #clusterEtaVetoRanges = "[-1.52, -1.37], [1.37, 1.52]"
                                      )

from HSG5DPDUtils.HSG5Selectors import MuonSelector
muSelector = MuonSelector( minNumberPassed = 2,
                           ptMin = 15.0,
                           absEtaMax = 2.6,
                           requireIsTight = True)

leptonFilter += muSelector.getMuonSelector('HSG5ZHLL_MuidMuonSelector', 'MuidMuonCollection',
                                           'HSG5ZHLL_LooseMuidMuonLinkCollection')
leptonFilter += muSelector.getMuonSelector('HSG5ZHLL_StacoMuonSelector', 'StacoMuonCollection',
                                           'HSG5ZHLL_LooseStacoMuonLinkCollection')
leptonFilter += muSelector.getMuonSelector('HSG5ZHLL_ThirdChainMuonSelector', 'Muons',
                                           'HSG5ZHLL_LooseThirdChainMuonLinkCollection')

sequencer += leptonFilter

HSG5D3PD_Stream.AcceptAlgs.append("HSG5ZHLL_DileptonFilter")


from D2PDMaker.D2PDMakerConf import D2PDMissingEtSelector
sequencer += D2PDMissingEtSelector( "HSG5ZHLL_MissingEtFilter",
                                    inputCollection      = 'MET_RefFinal',
                                    outputCollection = 'HSG5ZHLL_NeutrinoRefFinal',
                                    minNumberPassed      = 1,
                                    missingEtMax         = 50.0*Units.GeV)
HSG5D3PD_Stream.RequireAlgs.append("HSG5ZHLL_MissingEtFilter")

if False:
    # (for private production only) insert in beginning of PreD3PDSequencer
    mainSequencer = AlgSequence(D3PDMakerFlags.PreD3PDAlgSeqName(),
                                StopOverride = False)

    if not hasattr( topSequence, D3PDMakerFlags.PreD3PDAlgSeqName() ):
        topSequence += mainSequencer

    mainSequencer.insert(0,sequencer)

else:
    topSequence+=sequencer
