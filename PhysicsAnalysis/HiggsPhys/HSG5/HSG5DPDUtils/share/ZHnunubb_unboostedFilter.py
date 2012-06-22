
from D3PDMakerConfig.D3PDMakerFlags import D3PDMakerFlags
from AthenaCommon.AlgSequence import AlgSequence,AthSequencer

# create electron and muon selectors
sequencer = AthSequencer("HSG5ZHMETU_Sequencer",
                         StopOverride=False)

# MET selector
from D2PDMaker.D2PDMakerConf import D2PDMissingEtSelector
sequencer += D2PDMissingEtSelector( "HSG5ZHMETU_MissingEtFilter",
                                    inputCollection      = 'MET_RefFinal',
                                    outputCollection = 'HSG5ZHMETU_NeutrinoRefFinal',
                                    minNumberPassed      = 1,
                                    missingEtMin         = 40.0*Units.GeV)
                                    #missingEtSignificanceMin = 5.0)
HSG5D3PD_Stream.RequireAlgs.append("HSG5ZHMETU_MissingEtFilter")


if False:
    # (for private production only) insert in beginning of PreD3PDSequencer
    mainSequencer = AlgSequence(D3PDMakerFlags.PreD3PDAlgSeqName(),
                                StopOverride = False)

    if not hasattr( topSequence, D3PDMakerFlags.PreD3PDAlgSeqName() ):
        topSequence += mainSequencer

    mainSequencer.insert(0,sequencer)

else:
    topSequence+=sequencer
