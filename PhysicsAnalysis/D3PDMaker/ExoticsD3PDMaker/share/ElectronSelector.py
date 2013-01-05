include.block( "PhysicsD3PDMaker/ElectronSelector.py" )

from D3PDMakerConfig.D3PDMakerFlags                    import D3PDMakerFlags

from D2PDMaker.D2PDMakerConf                           import D2PDElectronSelector
from D2PDMaker.D2PDMakerConf                           import D2PDParticleCombiner



## Import the module that allows to use named units, e.g. GeV
import AthenaCommon.SystemOfUnits as Units

## Get pre sequence
from AthenaCommon.AlgSequence import AlgSequence
preseq = AlgSequence (D3PDMakerFlags.PreD3PDAlgSeqName())

## Define event selector
from D2PDMaker.D2PDMakerConf import D2PDElectronSelector
preseq += D2PDElectronSelector( "WPrimeElectronSelector",
                               OutputLevel          = INFO,
                               inputCollection      = 'ElectronAODCollection',
                               minNumberPassed      = 1,
                               ptMin                = 20.0*Units.GeV,
                               )