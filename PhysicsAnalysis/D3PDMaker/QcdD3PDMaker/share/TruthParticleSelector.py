include.block( 'QcdD3PDMaker/TruthParticleSelector.py' )

from D3PDMakerConfig.D3PDMakerFlags                    import D3PDMakerFlags

from D2PDMaker.D2PDMakerConf                           import D2PDTruthParticleSelector
from D2PDMaker.D2PDMakerConf                           import D2PDParticleCombiner

from AthenaCommon.AlgSequence import AlgSequence
preseq = AlgSequence (D3PDMakerFlags.PreD3PDAlgSeqName())

preseq += D2PDTruthParticleSelector( "SMQCD_GoodTruthParticleFilter",
                                     inputCollection      = 'SpclMC',
                                     outputLinkCollection     = 'GoodTruthParticles',
                                     ptMin=400.0*Units.MeV)
