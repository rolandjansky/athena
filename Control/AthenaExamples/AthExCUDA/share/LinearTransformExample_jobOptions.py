# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
#
# "Standalone" jobOptions for running AthCUDA::LinearTransformExampleAlg.
#

# Set up / access the algorithm sequence.
from AthenaCommon.AlgSequence import AlgSequence
algSequence = AlgSequence()

# Add the algorithm to the sequence.
from AthExCUDA.AthExCUDAConf import AthCUDA__LinearTransformExampleAlg
algSequence += AthCUDA__LinearTransformExampleAlg()

# Run for 10 "events".
theApp.EvtMax = 10
