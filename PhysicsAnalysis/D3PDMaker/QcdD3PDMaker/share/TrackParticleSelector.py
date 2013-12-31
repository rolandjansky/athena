include.block( 'QcdD3PDMaker/TrackParticleSelector.py' )

from D3PDMakerConfig.D3PDMakerFlags                    import D3PDMakerFlags

from D2PDMaker.D2PDMakerConf                           import D2PDTrackParticleSelector
from D2PDMaker.D2PDMakerConf                           import D2PDParticleCombiner

from AthenaCommon.AlgSequence import AlgSequence
preseq = AlgSequence (D3PDMakerFlags.PreD3PDAlgSeqName())

preseq += D2PDTrackParticleSelector( "SMQCD_GoodTrackFilter",
                                     inputCollection      = D3PDMakerFlags.TrackSGKey(),
                                     outputCollection     = 'GoodTracks',
                                     ptMin=500.0*Units.MeV,
                                     numberSCTHitsMin = 6,
                                     numberPixelHitsMin = 1)

