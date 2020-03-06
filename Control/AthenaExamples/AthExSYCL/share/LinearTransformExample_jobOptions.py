# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
#
# "Standalone" jobOptions for running AthSYCL::LinearTransformExampleAlg.
#

# Set up / access the algorithm sequence.
from AthenaCommon.AlgSequence import AlgSequence
algSequence = AlgSequence()

# Add the algorithm to the sequence.
from AthExSYCL.AthExSYCLConf import AthSYCL__LinearTransformExampleAlg
algSequence += AthSYCL__LinearTransformExampleAlg( OutputLevel = DEBUG )

# Run for 10 "events".
theApp.EvtMax = 10
