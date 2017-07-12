include.block( "SMEWTrilepSkim/SMTRILEP_ElectronSelector.py" )

from ROOT import egammaPID
from ROOT import egammaParameters

from D3PDMakerConfig.D3PDMakerFlags                    import D3PDMakerFlags

from D2PDMaker.D2PDMakerConf                           import D2PDElectronSelector
from D2PDMaker.D2PDMakerConf                           import D2PDParticleCombiner

from AthenaCommon.AlgSequence import AlgSequence
preseq = AlgSequence (D3PDMakerFlags.PreD3PDAlgSeqName())

preseq += D2PDElectronSelector( "SMTRILEP_TriLepElectronFilter",
                                inputCollection      = 'ElectronAODCollection',
                                outputCollection     = 'TrilepElectrons',
                                etMin                = 4.0*Units.GeV,
                                clusterEtaMin        = -5,
                                clusterEtaMax        = 5,
                               #electronID           = egammaPID.ElectronIDLoosePP,
                                minNumberPassed      = 1
                                )

#from RecExConfig.RecAlgsFlags import recAlgs
#if recAlgs.doEgammaBremReco():
#    gsfElectronContainer = 'HighPtGSFElectrons'
#    preseq += D2PDElectronSelector( "SMTRILEP_HighPtGSFElectronFilter",
#                                    inputCollection      = 'GSFElectronAODCollection',
#                                    outputCollection     = 'HighPtGSFElectrons',
#                                    etMin                = 20.0*Units.GeV,
#                                    clusterEtaMin        = -2.5,
#                                    clusterEtaMax        = 2.5,
#                                    electronID           = egammaPID.ElectronIDLoosePP,
#                                    minNumberPassed      = 1
#                                    )
#else:
#    gsfElectronContainer = 'None'

