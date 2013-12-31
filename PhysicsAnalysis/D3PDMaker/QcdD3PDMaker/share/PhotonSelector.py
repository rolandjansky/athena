include.block( "QcdD3PDMaker/PhotonSelector.py" )


from D3PDMakerConfig.D3PDMakerFlags                    import D3PDMakerFlags

from D2PDMaker.D2PDMakerConf import D2PDPhotonSelector

from AthenaCommon.AlgSequence import AlgSequence
preseq = AlgSequence (D3PDMakerFlags.PreD3PDAlgSeqName())

preseq += D2PDPhotonSelector( "SMQCD_GoodPhotonFilter",
                              inputCollection      = 'PhotonAODCollection',
                              outputCollection     = 'GoodPhotons',
                              etMin                = 10.0*Units.GeV )

