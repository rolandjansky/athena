include.block( "PhysicsD3PDMaker/TauSelector.py" )

from D2PDMaker.D2PDMakerConf import D2PDTauSelector

from AthenaCommon.AlgSequence import AlgSequence
preseq = AlgSequence (D3PDMakerFlags.PreD3PDAlgSeqName())

preseq += D2PDTauSelector( "SMWZ_HighPtTauFilter",
                           inputCollection      = 'TauRecContainer',
                           outputCollection     = 'HighPtTaus',
                           etMin                = 15.0*Units.GeV )
