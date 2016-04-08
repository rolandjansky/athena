include.block( "PhysicsD3PDMaker/LowPtMuonPairSelector.py" )

from D3PDMakerConfig.D3PDMakerFlags                    import D3PDMakerFlags

from D2PDMaker.D2PDMakerConf                           import D2PDMuonSelector
from D2PDMaker.D2PDMakerConf                           import D2PDParticleCombiner

from AthenaCommon.AlgSequence import AlgSequence
preseq = AlgSequence (D3PDMakerFlags.PreD3PDAlgSeqName())


# STACO

preseq += D2PDMuonSelector( "SMWZ_LowPtMuonFilter",
                            inputCollection      = 'StacoMuonCollection',
                            outputCollection     = 'LowPtStacoMuons',
                            ptMin                = 5.0*Units.GeV )

preseq += D2PDParticleCombiner( "SMWZ_JPsimumuStacoFilter",
                                inputCollectionList     = [ 'LowPtStacoMuons', 'LowPtStacoMuons' ],
                                outputCollection        = 'JPsimumuStacoSelection',
                                minNumberPassed         = 1,
                                massMax                 = 15.0*Units.GeV )


preseq += D2PDParticleCombiner( "SMWZ_ZmumuStacoFilter",
                                inputCollectionList     = [ 'LowPtStacoMuons', 'LowPtStacoMuons' ],
                                outputCollection        = 'ZmumuStacoSelection',
                                minNumberPassed         = 1,
                                massMin                 = 50.0*Units.GeV )

# MUID

preseq += D2PDMuonSelector( "SMWZ_LowPtMuidMuonFilter",
                            inputCollection      = 'MuidMuonCollection',
                            outputCollection     = 'LowPtMuidMuons',
                            ptMin                = 5.0*Units.GeV )

preseq += D2PDParticleCombiner( "SMWZ_JPsimumuMuidFilter",
                                inputCollectionList     = [ 'LowPtMuidMuons', 'LowPtMuidMuons' ],
                                outputCollection        = 'JPsimumuMuidSelection',
                                minNumberPassed         = 1,
                                massMax                 = 15.0*Units.GeV )

preseq += D2PDParticleCombiner( "SMWZ_ZmumuMuidFilter",
                                inputCollectionList     = [ 'LowPtMuidMuons', 'LowPtMuidMuons' ],
                                outputCollection        = 'ZmumuMuidSelection',
                                minNumberPassed         = 1,
                                massMin                 = 50.0*Units.GeV )
