include.block( "PhysicsD3PDMaker/LowPtElectronPairSelector.py" )

from ROOT import egammaPID
from ROOT import egammaParameters

from D3PDMakerConfig.D3PDMakerFlags                    import D3PDMakerFlags

from D2PDMaker.D2PDMakerConf                           import D2PDElectronSelector
from D2PDMaker.D2PDMakerConf                           import D2PDParticleCombiner

from AthenaCommon.AlgSequence import AlgSequence
preseq = AlgSequence (D3PDMakerFlags.PreD3PDAlgSeqName())


preseq += D2PDElectronSelector( "SMWZ_LowPtElectronFilter",
                                inputCollection      = 'ElectronAODCollection',
                                outputCollection     = 'LowPtElectrons',
                                clusterEtaMin        = -2.5,
                                clusterEtaMax        = 2.5,
                                electronID           = egammaPID.ElectronIDMediumPP,
                                etMin                = 5.0*Units.GeV )

preseq += D2PDElectronSelector( "SMWZ_MediumPtElectronFilter",
                                inputCollection      = 'ElectronAODCollection',
                                outputCollection     = 'MediumPtElectrons',
                                clusterEtaMin        = -2.5,
                                clusterEtaMax        = 2.5,
                                electronID           = egammaPID.ElectronIDLoosePP,
                                etMin                = 10.0*Units.GeV )


from RecExConfig.RecAlgsFlags import recAlgs
if recAlgs.doEgammaBremReco():
    gsfElectronContainer = 'LowPtGSFElectrons'
    preseq += D2PDElectronSelector( "SMWZ_LowPtGSFElectronFilter",
                                    inputCollection      = 'GSFElectronAODCollection',
                                    outputCollection     = 'LowPtGSFElectrons',
                                    clusterEtaMin        = -2.5,
                                    clusterEtaMax        = 2.5,
                                    electronID           = egammaPID.ElectronIDMediumPP,
                                    etMin                = 5.0*Units.GeV )
else:
    gsfElectronContainer = 'None'


preseq += D2PDParticleCombiner( "SMWZ_JPsieeFilter",
                                inputCollectionList     = [ 'LowPtElectrons', 'LowPtElectrons' ],
                                outputCollection        = 'JPsieeSelection',
                                minNumberPassed         = 1,
                                massMax                 = 15.0*Units.GeV )

preseq += D2PDParticleCombiner( "SMWZ_ZeeFilter",
                                inputCollectionList     = [ 'MediumPtElectrons', 'MediumPtElectrons' ],
                                outputCollection        = 'ZeeSelection',
                                minNumberPassed         = 1,
                                massMin                 = 50.0*Units.GeV )
