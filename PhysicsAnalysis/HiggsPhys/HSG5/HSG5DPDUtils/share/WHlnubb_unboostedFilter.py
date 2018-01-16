'''
Created on 28 Feb 2012

@author: wvazquez
'''

from D3PDMakerConfig.D3PDMakerFlags import D3PDMakerFlags
from AthenaCommon.AlgSequence import AlgSequence,AthSequencer

from ROOT import egammaPID

sequencer = AthSequencer("HSG5WHU_Sequencer",
                         StopOverride=False)

from HSG5DPDUtils.HSG5DPDUtilsConf import HSG5__LeptonFilter
leptonFilter=HSG5__LeptonFilter("HSG5WHU_SingleLeptonFilter",
                                ElectronFilterNameAndType="D2PDElectronSelector/HSG5WHU_ElectronSelector",
                                MuonFilterNameAndTypeVec=["D2PDMuonSelector/HSG5WHU_MuidMuonSelector",
                                                          "D2PDMuonSelector/HSG5WHU_StacoMuonSelector",
                                                          "D2PDMuonSelector/HSG5WHU_ThirdChainMuonSelector" ])


# create electron and muon selectors
from D2PDMaker.D2PDMakerConf import D2PDElectronSelector
leptonFilter += D2PDElectronSelector( "HSG5WHU_ElectronSelector",
                                      inputCollection      = 'ElectronAODCollection',
                                      outputLinkCollection = 'HSG5WHU_LooseElectronLinkCollection',
                                      minNumberPassed      = 1,
                                      ptMin                = 20.0*Units.GeV,
                                      clusterEtaMin        = -2.5,
                                      clusterEtaMax        = 2.5,
                                      electronIsEM         = egammaPID.ElectronTight )

from HSG5DPDUtils.HSG5Selectors import MuonSelector
muSelector = MuonSelector( minNumberPassed = 1,
                           ptMin = 18.0,
                           absEtaMax = 2.5,
                           acceptIsCombined = True,
                           acceptIsSegmentTagged = True,
                           doRelPtCone20  = True,
                           relPtCone20Max = 0.15 )

leptonFilter += muSelector.getMuonSelector('HSG5WHU_MuidMuonSelector','MuidMuonCollection',
                                           'HSG5WHU_LooseMuidMuonLinkCollection')
leptonFilter += muSelector.getMuonSelector('HSG5WHU_StacoMuonSelector','StacoMuonCollection',
                                           'HSG5WHU_LooseStacoMuonLinkCollection')
leptonFilter += muSelector.getMuonSelector('HSG5WHU_ThirdChainMuonSelector','Muons',
                                           'HSG5WHU_LooseThirdChainMuonLinkCollection')

sequencer += leptonFilter

HSG5D3PD_Stream.RequireAlgs.append("HSG5WHU_SingleLeptonFilter")

# jet selector
from AssociationComps.AssociationCompsConf import DeltaRAssociationTool
ToolSvc += DeltaRAssociationTool( "HSG5WHU_emDeltaRAssociationTool",
                                  OutputLevel = INFO,
                                  inputAssociateToCollection = 'HSG5WHU_LooseElectronLinkCollection',
                                  deltaRMax = 0.3,
                                  writeUserData = False)

from D2PDMaker.D2PDMakerConf import D2PDJetSelector
sequencer += D2PDJetSelector( "HSG5WHU_JetFilter",
                              inputCollection      = 'AntiKt4TopoEMJets',
                              outputLinkCollection = 'HSG5WHU_JetLinkCollection',
                              associationToolList = [ ToolSvc.HSG5WHU_emDeltaRAssociationTool ],
                              outputAssociationContainerList = [ "HSG5WHU_jetsMatchedToElectrons" ],
                              numberOfAssociationsMaxCutList = [ 0 ],
                              minNumberPassed      = 1,
                              ptMin                = 20.0*Units.GeV,
                              etaMax               = 2.5)

HSG5D3PD_Stream.RequireAlgs.append("HSG5WHU_JetFilter")

if False:
    # (for private production ony) insert in beginning of PreD3PDSequencer
    mainSequencer = AlgSequence(D3PDMakerFlags.PreD3PDAlgSeqName(),
                                StopOverride = False)

    if not hasattr( topSequence, D3PDMakerFlags.PreD3PDAlgSeqName() ):
        topSequence += mainSequencer

    mainSequencer.insert(0,sequencer)

else:
    topSequence+=sequencer
