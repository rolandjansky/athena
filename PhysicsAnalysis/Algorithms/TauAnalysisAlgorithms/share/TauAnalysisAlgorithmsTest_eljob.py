#!/usr/bin/env python
#
# Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
#
# @author Nils Krumnack


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

# this forces the tau algorithms dictionary to be loaded before
# anything else, which works around some strange dictionary issues I
# don't understand.
ROOT.CP.TauSmearingAlg ("dummy", None)

# ideally we'd run over all of them, but we don't have a mechanism to
# configure per-sample right now

dataType = options.data_type

if not dataType in ["data", "mc", "afii"] :
    raise ValueError ("invalid data type: " + dataType)

# Set up the sample handler object. See comments from the C++ macro
# for the details about these lines.
import os
sh = ROOT.SH.SampleHandler()
sh.setMetaString( 'nc_tree', 'CollectionTree' )
sample = ROOT.SH.SampleLocal (dataType)
if dataType == "data" :
    sample.add (os.getenv ('ASG_TEST_FILE_DATA'))
    pass
if dataType == "mc" :
    sample.add (os.getenv ('ASG_TEST_FILE_MC'))
    pass
if dataType == "afii" :
    sample.add (os.getenv ('ASG_TEST_FILE_MC_AFII'))
    pass
sh.add (sample)
sh.printContent()

# Create an EventLoop job.
job = ROOT.EL.Job()
job.sampleHandler( sh )
job.options().setDouble( ROOT.EL.Job.optMaxEvents, 500 )

# Set up the systematics loader/handler algorithm:
from AnaAlgorithm.AnaAlgorithmConfig import AnaAlgorithmConfig
config = AnaAlgorithmConfig( 'CP::SysListLoaderAlg/SysLoaderAlg' )
config.sigmaRecommended = 1
job.algsAdd( config )

# Include, and then set up the tau analysis algorithm sequence:
from TauAnalysisAlgorithms.TauAnalysisSequence import makeTauAnalysisSequence
tauSequence = makeTauAnalysisSequence( dataType, 'Tight', postfix = 'tight', deepCopyOutput = True )
tauSequence.configure( inputName = 'TauJets', outputName = 'AnalysisTauJets' )
print( tauSequence ) # For debugging

# Add all algorithms to the job:
for alg in tauSequence:
    job.algsAdd( alg )
    pass

# Include, and then set up the di-tau analysis algorithm sequence:
from TauAnalysisAlgorithms.DiTauAnalysisSequence import makeDiTauAnalysisSequence
diTauSequence = makeDiTauAnalysisSequence( dataType, 'Tight', postfix = 'tight', deepCopyOutput = True )
diTauSequence.configure( inputName = 'DiTauJets', outputName = 'AnalysisDiTauJets' )
print( diTauSequence ) # For debugging

# Add all algorithms to the job:
for alg in diTauSequence:
    job.algsAdd( alg )
    pass

# Find the right output directory:
submitDir = options.submission_dir
if options.unit_test:
    import os
    import tempfile
    submitDir = tempfile.mkdtemp( prefix = 'tauTest_'+dataType+'_', dir = os.getcwd() )
    os.rmdir( submitDir )
    pass

# Run the job using the direct driver.
driver = ROOT.EL.DirectDriver()
driver.submit( job, submitDir )
