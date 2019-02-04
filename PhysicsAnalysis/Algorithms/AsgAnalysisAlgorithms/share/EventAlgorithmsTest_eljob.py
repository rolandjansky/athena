#!/usr/bin/env python
#
# Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
#
# @author Tadej Novak


# Read the submission directory as a command line argument. You can
# extend the list of arguments with your private ones later on.
import optparse
parser = optparse.OptionParser()
parser.add_option( '-d', '--data-type', dest = 'data_type',
                   action = 'store', type = 'string', default = 'data',
                   help = 'Type of data to run over. Valid options are data, mc, afii' )
parser.add_option( '-s', '--submission-dir', dest = 'submission_dir',
                   action = 'store', type = 'string', default = 'submitDir',
                   help = 'Submission directory for EventLoop' )
parser.add_option( '-u', '--unit-test', dest='unit_test',
                   action = 'store_true', default = False,
                   help = 'Run the job in "unit test mode"' )
( options, args ) = parser.parse_args()

# Set up (Py)ROOT.
import ROOT
import os
ROOT.xAOD.Init().ignore()

# ideally we'd run over all of them, but we don't have a mechanism to
# configure per-sample right now

dataType = options.data_type

inputfile = {"data": 'ASG_TEST_FILE_DATA',
             "mc":   'ASG_TEST_FILE_MC',
             "afii": 'ASG_TEST_FILE_MC_AFII'}

if not dataType in ["data", "mc", "afii"] :
    raise ValueError ("invalid data type: " + dataType)

# Set up the sample handler object. See comments from the C++ macro
# for the details about these lines.
import os
sh = ROOT.SH.SampleHandler()
sh.setMetaString( 'nc_tree', 'CollectionTree' )
sample = ROOT.SH.SampleLocal (dataType)
sample.add (os.getenv (inputfile[dataType]))
sh.add (sample)
sh.printContent()

# Create an EventLoop job.
job = ROOT.EL.Job()
job.sampleHandler( sh )
job.options().setDouble( ROOT.EL.Job.optMaxEvents, 500 )

# Config:
GRLFiles = ['GoodRunsLists/data16_13TeV/20180129/data16_13TeV.periodAllYear_DetStatus-v89-pro21-01_DQDefects-00-02-04_PHYS_StandardGRL_All_Good_25ns.xml']

# Import(s) needed for the job configuration.
from AnaAlgorithm.AlgSequence import AlgSequence
from AnaAlgorithm.DualUseConfig import createAlgorithm

# Set up the main analysis algorithm sequence.
algSeq = AlgSequence( 'AnalysisSequence' )

# Set up the systematics loader/handler algorithm:
algSeq += createAlgorithm( 'CP::SysListLoaderAlg', 'SysLoaderAlg' )
algSeq.SysLoaderAlg.sigmaRecommended = 1

# Include, and then set up the pileup analysis sequence:
from AsgAnalysisAlgorithms.EventSelectionAnalysisSequence import \
    makeEventSelectionAnalysisSequence
eventSelectionSequence = \
    makeEventSelectionAnalysisSequence( dataType, userGRLFiles=GRLFiles )
algSeq += eventSelectionSequence

# Set up an ntuple to check the job with:
ntupleMaker = createAlgorithm( 'CP::AsgxAODNTupleMakerAlg', 'NTupleMaker' )
ntupleMaker.TreeName = 'events'
ntupleMaker.Branches = [
    'EventInfo.runNumber   -> runNumber',
    'EventInfo.eventNumber -> eventNumber',
]
ntupleMaker.systematicsRegex = '.*'
algSeq += ntupleMaker

# For debugging.
print( algSeq )

# Add all algorithms from the sequence to the job.
for alg in algSeq:
    job.algsAdd( alg )
    pass

# Set up an output file for the job:
job.outputAdd( ROOT.EL.OutputStream( 'ANALYSIS' ) )

# Find the right output directory:
submitDir = options.submission_dir
if options.unit_test:
    import os
    import tempfile
    submitDir = tempfile.mkdtemp( prefix = 'evenTest_', dir = os.getcwd() )
    os.rmdir( submitDir )
    pass

# Run the job using the direct driver.
driver = ROOT.EL.DirectDriver()
driver.submit( job, submitDir )
