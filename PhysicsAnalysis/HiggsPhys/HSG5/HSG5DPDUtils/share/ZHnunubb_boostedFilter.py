
from D3PDMakerConfig.D3PDMakerFlags import D3PDMakerFlags
from AthenaCommon.AlgSequence import AlgSequence,AthSequencer

sequencer = AthSequencer("HSG5ZHMET_Sequencer",
                         StopOverride=False)

# boosted jet selector
from D2PDMaker.D2PDMakerConf import D2PDJetSelector
sequencer += D2PDJetSelector( "HSG5ZHMET_BoostedJetFilter",
                              inputCollection      = 'CamKt12LCTopoJets',
                              outputLinkCollection = 'HSG5ZHMET_boosted_LooseBoostedJetLinkCollection',
                              minNumberPassed      = 1,
                              ptMin                = 20.0*Units.GeV)
HSG5D3PD_Stream.RequireAlgs.append("HSG5ZHMET_BoostedJetFilter")


# MET selector
from D2PDMaker.D2PDMakerConf import D2PDMissingEtSelector
sequencer += D2PDMissingEtSelector( "HSG5ZHMET_MissingEtFilter",
                                    inputCollection      = 'MET_RefFinal',
                                    outputCollection = 'HSG5ZHMET_NeutrinoRefFinal',
                                    minNumberPassed      = 1,
                                    missingEtMin         = 40.0*Units.GeV)
                                    #missingEtSignificanceMin = 5.0)
HSG5D3PD_Stream.RequireAlgs.append("HSG5ZHMET_MissingEtFilter")


if False:
    # (for private production only) insert in beginning of PreD3PDSequencer
    mainSequencer = AlgSequence(D3PDMakerFlags.PreD3PDAlgSeqName(),
                                StopOverride = False)

    if not hasattr( topSequence, D3PDMakerFlags.PreD3PDAlgSeqName() ):
        topSequence += mainSequencer

    mainSequencer.insert(0,sequencer)

else:
    topSequence+=sequencer
