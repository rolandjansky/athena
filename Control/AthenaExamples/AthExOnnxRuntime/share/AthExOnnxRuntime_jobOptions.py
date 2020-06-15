# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

# Get a	random no. between 0 to	10k for	test sample
from random import randint

# Set up (Py)ROOT.
import ROOT

# Create an EventLoop job.
job = ROOT.EL.Job()
job.options().setDouble( ROOT.EL.Job.optMaxEvents, 10 )

# Create the algorithm's configuration.
from AnaAlgorithm.DualUseConfig import createAlgorithm
alg = createAlgorithm ('AthExOnnxRuntime', 'AthONNX__CxxApiAlgorithm')

# later on we'll add some configuration options for our algorithm that go here

# Add our algorithm to the job
job.algsAdd( alg )
alg.TestSample = randint(0, 9999)
# Run the job using the direct driver.
driver = ROOT.EL.DirectDriver()
