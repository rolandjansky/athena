# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
#
# "Standalone" jobOptions for running
# AthCUDAExamples::LinearTransformStandaloneExampleAlg.
#

# Set up / access the algorithm sequence.
from AthenaCommon.AlgSequence import AlgSequence
algSequence = AlgSequence()

# Add the algorithm to the sequence.
from AthExCUDA.AthExCUDAConf import \
   AthCUDAExamples__LinearTransformStandaloneExampleAlg
algSequence += AthCUDAExamples__LinearTransformStandaloneExampleAlg()

# Run for 10 "events".
theApp.EvtMax = 10
