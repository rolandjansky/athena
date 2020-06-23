# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

# Set up / access the algorithm sequence.
#
from AthenaCommon.AlgSequence import AlgSequence
athAlgSeq = AlgSequence()

# Set up the job.
from AthExOnnxRuntime.AthExOnnxRuntimeConf import AthONNX__CxxApiAlgorithm

# Create the algorithm's configuration.
from AnaAlgorithm.DualUseConfig import createAlgorithm
alg = createAlgorithm ('AthONNX__CxxApiAlgorithm', 'AthONNX')

# later on we'll add some configuration options for our algorithm that go here

athAlgSeq += alg

# limit the number of events (for testing purposes)
theApp.EvtMax = 10

# Get a random no. between 0 to 10k for test sample
from random import randint
alg.TestSample = randint(0, 9999)

