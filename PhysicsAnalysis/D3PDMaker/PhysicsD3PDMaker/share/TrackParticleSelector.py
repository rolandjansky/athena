include.block( "PhysicsD3PDMaker/TrackParticleSelector.py" )


from D3PDMakerConfig.D3PDMakerFlags                    import D3PDMakerFlags

from D2PDMaker.D2PDMakerConf                           import D2PDTrackParticleSelector

from AthenaCommon.AlgSequence import AlgSequence
preseq = AlgSequence (D3PDMakerFlags.PreD3PDAlgSeqName())

preseq += D2PDTrackParticleSelector( "SMWZ_HighPtTrackFilter",
                                     inputCollection      = D3PDMakerFlags.TrackSGKey(),
                                     outputCollection     = 'Track_NSiHits6_Pt10',
                                     ptMin                = 10.0*Units.GeV,
                                     numberSiliconHitsMin = 6
                                     )

preseq += D2PDTrackParticleSelector( "SMWZ_GoodTrackFilter",
                                     inputCollection      = D3PDMakerFlags.TrackSGKey(),
                                     outputCollection     = 'Tracks_NSiHits6',
                                     numberSiliconHitsMin = 6
                                     )
