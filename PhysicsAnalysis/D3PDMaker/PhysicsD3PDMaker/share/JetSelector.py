include.block( "PhysicsD3PDMaker/JetSelector.py" )

from D2PDMaker.D2PDMakerConf import D2PDJetSelector

from AthenaCommon.AlgSequence import AlgSequence
preseq = AlgSequence (D3PDMakerFlags.PreD3PDAlgSeqName())

preseq += D2PDJetSelector( "SMWZ_HighPtJetFilter",
                           inputCollection      = 'AntiKt4TopoEMJets',
                           outputCollection     = 'HighPtJets',
                           ptMin                = 20.0*Units.GeV )

preseq += D2PDJetSelector( "SMWZ_HighPtJetEM6Filter",
                           inputCollection      = 'AntiKt6TopoEMJets',
                           outputCollection     = 'HighPtJetsEM6',
                           ptMin                = 20.0*Units.GeV )

preseq += D2PDJetSelector( "SMWZ_HighPtJetLC4Filter",
                           inputCollection      = 'AntiKt4LCTopoJets',
                           outputCollection     = 'HighPtJetsLC4',
                           ptMin                = 20.0*Units.GeV )

preseq += D2PDJetSelector( "SMWZ_HighPtJetLC6Filter",
                           inputCollection      = 'AntiKt4LCTopoJets',
                           outputCollection     = 'HighPtJetsLC6',
                           ptMin                = 20.0*Units.GeV )

