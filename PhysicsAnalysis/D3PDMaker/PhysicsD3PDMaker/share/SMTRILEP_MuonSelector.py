include.block( "SMEWTrilepSkim/SMTRILEP_MuonSelector.py" )

from D3PDMakerConfig.D3PDMakerFlags                    import D3PDMakerFlags

from D2PDMaker.D2PDMakerConf                           import D2PDMuonSelector
from D2PDMaker.D2PDMakerConf                           import D2PDParticleCombiner

from AthenaCommon.AlgSequence import AlgSequence
preseq = AlgSequence (D3PDMakerFlags.PreD3PDAlgSeqName())


# STACO

preseq += D2PDMuonSelector( "SMTRILEP_TrilepStacoMuonFilter",
                            inputCollection      = 'StacoMuonCollection',
                            outputCollection     = 'TrilepStacoMuons',
                            ptMin                = 4.0*Units.GeV,
                            minNumberPassed      = 1 )

# THIRD Muons
preseq += D2PDMuonSelector( "SMTRILEP_TrilepThirdMuonFilter",
                            inputCollection      = 'Muons',
                            outputCollection     = 'TrilepThirdMuons',
                            ptMin                = 4.0*Units.GeV,
                            minNumberPassed      = 1 )

# CALO muons - apply eta cut
preseq += D2PDMuonSelector( "SMTRILEP_TrilepCaloMuonFilter",
                            inputCollection      = 'CaloMuonCollection',
                            outputCollection     = 'TrilepCaloMuons',
                            ptMin                = 12.0*Units.GeV,
                            etaMin = -0.2,
                            etaMax = 0.2,
                            minNumberPassed      = 1 )


# MUID

#preseq += D2PDMuonSelector( "SMTRILEP_HighPtMuidMuonFilter",
#                            inputCollection      = 'MuidMuonCollection',
#                            outputCollection     = 'HighPtMuidMuons',
#                            ptMin                = 20.0*Units.GeV,
#                            minNumberPassed      = 1 )
