include.block( 'QcdD3PDMaker/ElectronSelector.py' )

from D3PDMakerConfig.D3PDMakerFlags                    import D3PDMakerFlags

from D2PDMaker.D2PDMakerConf                           import D2PDElectronSelector
from D2PDMaker.D2PDMakerConf                           import D2PDParticleCombiner

from AthenaCommon.AlgSequence import AlgSequence
preseq = AlgSequence (D3PDMakerFlags.PreD3PDAlgSeqName())

preseq += D2PDElectronSelector( "SMQCD_GoodElectronFilter",
                                inputCollection      = 'ElectronAODCollection',
                                outputCollection     = 'GoodElectrons',
                                ptMin                = 10.0*Units.GeV
                                )
