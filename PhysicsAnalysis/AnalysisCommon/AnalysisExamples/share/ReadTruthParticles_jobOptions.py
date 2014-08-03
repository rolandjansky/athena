# Fragment to run the ReadTruthParticles Algorithm
# Author : S.Binet <binet.At_cern.ch>

import AthenaCommon.Constants as Lvl
from AthenaCommon.AlgSequence import AlgSequence
topSequence = AlgSequence()

from McParticleTools.McParticleToolsConf import TruthParticleCnvTool
from AnalysisExamples.AnalysisExamplesConf import ReadTruthParticles

topSequence += ReadTruthParticles(
    "ReadTruthParticles",
    ## input location of the TruthParticles
    TruthParticles = "SpclMC",
    
    ## message level for this algorithm
    OutputLevel = Lvl.INFO
    )
