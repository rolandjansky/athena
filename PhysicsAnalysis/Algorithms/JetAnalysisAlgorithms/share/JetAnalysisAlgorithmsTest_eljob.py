#!/usr/bin/env python
#
# Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
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

# this forces the jet algorithms dictionary to be loaded before
# anything else, which works around some strange dictionary issues I
# don't understand.
ROOT.CP.JetCalibrationAlg ("dummy", None)

# ideally we'd run over all of them, but we don't have a mechanism to
# configure per-sample right now

dataType = options.data_type

inputfile = {"data": 'ASG_TEST_FILE_DATA',
             "mc":   'ASG_TEST_FILE_MC',
             "afii": 'ASG_TEST_FILE_MC_AFII'}

jetContainer = "AntiKt4EMTopoJets"

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

# Set up the systematics loader/handler algorithm:
from AnaAlgorithm.AnaAlgorithmConfig import AnaAlgorithmConfig
config = AnaAlgorithmConfig( 'CP::SysListLoaderAlg/SysLoaderAlg' )
config.sigmaRecommended = 1
job.algsAdd( config )

# Include, and then set up the jet analysis algorithm sequence:
from JetAnalysisAlgorithms.JetAnalysisSequence import makeJetAnalysisSequence
jetSequence = makeJetAnalysisSequence( dataType, jetContainer )
jetSequence.configure( inputName = jetContainer, outputName = 'AnalysisJetsBase' )
print( jetSequence ) # For debugging

# Include, and then set up the jet analysis algorithm sequence:
from JetAnalysisAlgorithms.JetJvtAnalysisSequence import makeJetJvtAnalysisSequence
jvtSequence = makeJetJvtAnalysisSequence( dataType, jetContainer )
jvtSequence.configure( inputName = { 'eventInfo' : 'EventInfo',
                                     'jets'      : 'AnalysisJetsBase_%SYS%' },
                       outputName = { 'eventInfo' : 'EventInfo_%SYS%',
                                      'jets'      : 'AnalysisJets_%SYS%' },
                       affectingSystematics = { 'jets' : '(^$)|(^JET_.*)' } )
print( jvtSequence ) # For debugging

# Add all algorithms to the job:
for alg in jetSequence:
    job.algsAdd( alg )
    pass

for alg in jvtSequence:
    job.algsAdd( alg )
    pass

# Set up an ntuple to check the job with:
from AnaAlgorithm.DualUseConfig import createAlgorithm
ntupleMaker = createAlgorithm( 'CP::AsgxAODNTupleMakerAlg', 'NTupleMaker' )
ntupleMaker.TreeName = 'jets'
ntupleMaker.Branches = [
    'EventInfo.runNumber   -> runNumber',
    'EventInfo.eventNumber -> eventNumber',
    'AnalysisJets_%SYS%.pt -> jet_%SYS%_pt',
]
if dataType != 'data':
    ntupleMaker.Branches += [
        'EventInfo_%SYS%.jvt_efficiency -> jvtSF_%SYS%',
        'EventInfo_%SYS%.fjvt_efficiency -> fjvtSF_%SYS%',
        'AnalysisJets_%SYS%.jvt_efficiency -> jet_%SYS%_jvtEfficiency',
        'AnalysisJets_%SYS%.fjvt_efficiency -> jet_%SYS%_fjvtEfficiency',
    ]
ntupleMaker.systematicsRegex = '.*'
job.algsAdd( ntupleMaker )

# Set up an output file for the job:
job.outputAdd( ROOT.EL.OutputStream( 'ANALYSIS' ) )

# Find the right output directory:
submitDir = options.submission_dir
if options.unit_test:
    import os
    import tempfile
    submitDir = tempfile.mkdtemp( prefix = 'jetTest_'+dataType+'_', dir = os.getcwd() )
    os.rmdir( submitDir )
    pass

# Run the job using the direct driver.
driver = ROOT.EL.DirectDriver()
driver.submit( job, submitDir )
