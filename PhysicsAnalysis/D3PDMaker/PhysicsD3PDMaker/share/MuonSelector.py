include.block( "PhysicsD3PDMaker/MuonSelector.py" )

from D3PDMakerConfig.D3PDMakerFlags                    import D3PDMakerFlags

from D2PDMaker.D2PDMakerConf                           import D2PDMuonSelector
from D2PDMaker.D2PDMakerConf                           import D2PDParticleCombiner

from AthenaCommon.AlgSequence import AlgSequence
preseq = AlgSequence (D3PDMakerFlags.PreD3PDAlgSeqName())


# STACO

preseq += D2PDMuonSelector( "SMWZ_HighPtStacoMuonFilter",
                            inputCollection      = 'StacoMuonCollection',
                            outputCollection     = 'HighPtStacoMuons',
                            ptMin                = 20.0*Units.GeV,
                            minNumberPassed      = 1 )


# MUID

preseq += D2PDMuonSelector( "SMWZ_HighPtMuidMuonFilter",
                            inputCollection      = 'MuidMuonCollection',
                            outputCollection     = 'HighPtMuidMuons',
                            ptMin                = 20.0*Units.GeV,
                            minNumberPassed      = 1 )
