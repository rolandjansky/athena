# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
#
# JobOptions for running AthHIPExamples::LinearTransformExampleAlg.
#

# Set up / access the algorithm sequence.
from AthenaCommon.AlgSequence import AlgSequence
algSequence = AlgSequence()

# Add the algorithm to the sequence.
from AthExHIP.AthExHIPConf import AthHIPExamples__LinearTransformExampleAlg
algSequence += AthHIPExamples__LinearTransformExampleAlg()

# Run for 10 "events".
theApp.EvtMax = 10
