#!/usr/bin/env python
# Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
#
# @author Jon Burr
# adapted from GeneratorAlgorithmsTest_eljob.py

# Only test on 'mc'
# Read the submission directory as a command line argument. You can
# extend the list of arguments with your private ones later on.
import optparse
parser = optparse.OptionParser()
parser.add_option( '-s', '--submission-dir', dest = 'submission_dir',
                   action = 'store', type = 'string', default = 'submitDir',
                   help = 'Submission directory for EventLoop' )
parser.add_option( '-u', '--unit-test', dest='unit_test',
                   action = 'store_true', default = False,
                   help = 'Run the job in "unit test mode"' )
( options, args ) = parser.parse_args()

# Set up (Py)ROOT.
import ROOT
ROOT.xAOD.Init().ignore()

dataType = "mc"

# Set up the sample handler object. See comments from the C++ macro
# for the details about these lines.
import os
sh = ROOT.SH.SampleHandler()
sh.setMetaString( 'nc_tree', 'CollectionTree' )
sample = ROOT.SH.SampleLocal (dataType)
sample.add (os.getenv ("ASG_TEST_FILE_MC"))
sh.add (sample)
sh.printContent()

# Create an EventLoop job.
job = ROOT.EL.Job()
job.sampleHandler( sh )
job.options().setDouble( ROOT.EL.Job.optMaxEvents, 500 )

from AsgAnalysisAlgorithms.AsgAnalysisAlgorithmsTest import makePileupSequence
algSeq = makePileupSequence()
print( algSeq ) # For debugging
algSeq.addSelfToJob( job )

# Set up an output file for the job:
job.outputAdd( ROOT.EL.OutputStream( 'ANALYSIS' ) )

# Find the right output directory:
submitDir = options.submission_dir
if options.unit_test:
    import os
    import tempfile
    submitDir = tempfile.mkdtemp( prefix = 'genTest_', dir = os.getcwd() )
    os.rmdir( submitDir )
    pass

# Run the job using the direct driver.
driver = ROOT.EL.DirectDriver()
driver.submit( job, submitDir )
