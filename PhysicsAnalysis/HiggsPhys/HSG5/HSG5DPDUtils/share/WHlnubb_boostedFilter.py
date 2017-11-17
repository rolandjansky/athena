
from D3PDMakerConfig.D3PDMakerFlags import D3PDMakerFlags
from AthenaCommon.AlgSequence import AlgSequence,AthSequencer

from ROOT import egammaPID

sequencer = AthSequencer("HSG5WH_Sequencer",
                         StopOverride=False)

from D2PDMaker.D2PDMakerConf import D2PDJetSelector
sequencer += D2PDJetSelector( "HSG5WH_BoostedJetFilter",
                              inputCollection      = 'CamKt12LCTopoJets',
                              outputLinkCollection = 'HSG5WH_LooseBoostedJetLinkCollection',
                              minNumberPassed      = 1,
                              ptMin                = 150.0*Units.GeV)
HSG5D3PD_Stream.RequireAlgs.append("HSG5WH_BoostedJetFilter")

from HSG5DPDUtils.HSG5DPDUtilsConf import HSG5__LeptonFilter
leptonFilter=HSG5__LeptonFilter("HSG5WH_SingleLeptonFilter",
                                ElectronFilterNameAndType="D2PDElectronSelector/HSG5WH_ElectronSelector",
                                MuonFilterNameAndTypeVec=["D2PDMuonSelector/HSG5WH_MuidMuonSelector",
                                                          "D2PDMuonSelector/HSG5WH_StacoMuonSelector",
                                                          "D2PDMuonSelector/HSG5WH_ThirdChainMuonSelector"])

# create electron and muon selectors
from D2PDMaker.D2PDMakerConf import D2PDElectronSelector
leptonFilter += D2PDElectronSelector( "HSG5WH_ElectronSelector",
                                      inputCollection      = 'ElectronAODCollection',
                                      outputLinkCollection = 'HSG5WH_LooseElectronLinkCollection',
                                      minNumberPassed      = 1,
                                      etMin                = 15.0*Units.GeV,
                                      clusterEtaMin        = -2.5,
                                      clusterEtaMax        = 2.5
                                      #clusterEtaVetoRanges = "[-1.52, -1.37], [1.37, 1.52]"
                                      )

from HSG5DPDUtils.HSG5Selectors import MuonSelector
muSelector = MuonSelector( minNumberPassed = 1,
                           ptMin = 18.0,
                           absEtaMax = 2.5,
                           requireIsLoose = True )

leptonFilter += muSelector.getMuonSelector('HSG5WH_MuidMuonSelector','MuidMuonCollection',
                                           'HSG5WH_LooseMuidMuonLinkCollection')
leptonFilter += muSelector.getMuonSelector('HSG5WH_StacoMuonSelector','StacoMuonCollection',
                                           'HSG5WH_LooseStacoMuonLinkCollection')
leptonFilter += muSelector.getMuonSelector('HSG5WH_ThirdChainMuonSelector','Muons',
                                           'HSG5WH_LooseThirdChainMuonLinkCollection')

sequencer += leptonFilter

HSG5D3PD_Stream.RequireAlgs.append("HSG5WH_SingleLeptonFilter")

# MET selector
from D2PDMaker.D2PDMakerConf import D2PDMissingEtSelector
sequencer += D2PDMissingEtSelector( "HSG5WH_MissingEtFilter",
                                    inputCollection  = 'MET_RefFinal',
                                    outputCollection = 'HSG5WH_NeutrinoRefFinal',
                                    minNumberPassed  = 1,
                                    missingEtMin     = 15.0*Units.GeV)
HSG5D3PD_Stream.RequireAlgs.append("HSG5WH_MissingEtFilter")


if False:
    # (for private production ony) insert in beginning of PreD3PDSequencer
    mainSequencer = AlgSequence(D3PDMakerFlags.PreD3PDAlgSeqName(),
                                StopOverride = False)

    if not hasattr( topSequence, D3PDMakerFlags.PreD3PDAlgSeqName() ):
        topSequence += mainSequencer

    mainSequencer.insert(0,sequencer)

else:
    topSequence+=sequencer
