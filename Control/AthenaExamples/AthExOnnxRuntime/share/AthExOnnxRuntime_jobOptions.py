# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

# Get a	random no. between 0 to	10k for	test sample
from random import randint

import optparse
parser = optparse.OptionParser()
parser.add_option( '-s', '--submission-dir', dest = 'submission_dir',
                   action = 'store', type = 'string', default = 'submitDir',
                   help = 'Submission directory for EventLoop' )
( options, args ) = parser.parse_args()


# Set up (Py)ROOT.
import ROOT

# Create an EventLoop job.
job = ROOT.EL.Job()
job.options().setDouble( ROOT.EL.Job.optMaxEvents, 10 )
job.options().setString( ROOT.EL.Job.optSubmitDirMode, 'unique-link')

# Create the algorithm's configuration.
from AnaAlgorithm.DualUseConfig import createAlgorithm
alg = createAlgorithm ('AthExOnnxRuntime', 'AthONNX__CxxApiAlgorithm')

# later on we'll add some configuration options for our algorithm that go here

# Add our algorithm to the job
job.algsAdd( alg )
alg.TestSample = randint(0, 9999)
# Run the job using the direct driver.
driver = ROOT.EL.DirectDriver()
driver.submit( job, options.submission_dir )
