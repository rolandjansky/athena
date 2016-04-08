include.block( "PhysicsD3PDMaker/PhotonSelector.py" )


from D3PDMakerConfig.D3PDMakerFlags                    import D3PDMakerFlags

from D2PDMaker.D2PDMakerConf import D2PDPhotonSelector

from AthenaCommon.AlgSequence import AlgSequence
preseq = AlgSequence (D3PDMakerFlags.PreD3PDAlgSeqName())

preseq += D2PDPhotonSelector( "SMWZ_HighPtPhotonFilter",
                              inputCollection      = 'PhotonAODCollection',
                              outputCollection     = 'HighPtPhotons',
                              photonVetoAuthorList = [ 128 ],
                              photonID             = egammaPID.PhotonIDLoose,
                              etMin                = 10.0*Units.GeV )
