include.block( "PhysicsD3PDMaker/MuonSelector.py" )

from D3PDMakerConfig.D3PDMakerFlags                    import D3PDMakerFlags

from D2PDMaker.D2PDMakerConf                           import D2PDMuonSelector
from D2PDMaker.D2PDMakerConf                           import D2PDParticleCombiner

## Import the module that allows to use named units, e.g. GeV
import AthenaCommon.SystemOfUnits as Units

## Get pre sequence
from AthenaCommon.AlgSequence import AlgSequence
preseq = AlgSequence (D3PDMakerFlags.PreD3PDAlgSeqName())

## Define event selector
from D2PDMaker.D2PDMakerConf import D2PDMuonSelector

WPrimeStacoMuonSelector = D2PDMuonSelector( "WPrimeStacoMuonSelector",
                                            OutputLevel          = INFO,
                                            inputCollection      ='StacoMuonCollection',
                                            minNumberPassed      = 1,
                                            ptMin                = 25.0*Units.GeV,
                                            muonRequireIsCombined= True
                                            )

WPrimeMuonsSelector     = D2PDMuonSelector( "WPrimeMuonsSelector",
                                            OutputLevel          = INFO,
                                            inputCollection      = 'Muons',
                                            minNumberPassed      = 1,
                                            ptMin                = 25.0*Units.GeV,
                                            )

preseq+= WPrimeStacoMuonSelector
preseq+= WPrimeMuonsSelector
