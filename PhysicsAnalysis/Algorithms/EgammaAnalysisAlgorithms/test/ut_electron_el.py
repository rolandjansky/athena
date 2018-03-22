#!/usr/bin/env python

# Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
#
# @author Nils Krumnack


# Read the submission directory as a command line argument. You can
# extend the list of arguments with your private ones later on.
import optparse
parser = optparse.OptionParser()
parser.add_option( '-s', '--submission-dir', dest = 'submission_dir',
                   action = 'store', type = 'string', default = 'submitDir',
                   help = 'Submission directory for EventLoop' )
( options, args ) = parser.parse_args()

# Set up (Py)ROOT.
import ROOT
import os
ROOT.xAOD.Init().ignore()

from AnaAlgorithm.AnaAlgorithmConfig import AnaAlgorithmConfig

# ideally we'd run over all of them, but we don't have a mechanism to
# configure per-sample right now
dataType = "data"
#dataType = "mc"
#dataType = "afii"
electronContainer = "Electrons"

if not dataType in ["data", "mc", "afii"] :
    raise Exception ("invalid data type: " + dataType)

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

# Create the algorithm's configuration. Note that we'll be able to add
# algorithm property settings here later on.
config = AnaAlgorithmConfig( 'CP::SysListLoaderAlg/SysLoaderAlg' )
config.sigmaRecommended = 1
job.algsAdd( config )


from AsgAnalysisAlgorithms.PileupAnalysisSequence import makePileupAnalysisSequence

sequence = makePileupAnalysisSequence (dataType=dataType)


from AsgAnalysisAlgorithms.SequencePostConfiguration import sequencePostConfiguration

sequencePostConfiguration (sequence, "EventInfo")

for alg in sequence :
    config = alg["alg"]

    # set everything to debug output
    config.OutputLevel = 1

    job.algsAdd( config )
    pass


from EgammaAnalysisAlgorithms.ElectronAnalysisSequence import makeElectronAnalysisSequence

sequence = makeElectronAnalysisSequence (electronContainer=electronContainer,dataType=dataType)


#from AsgAnalysisAlgorithms.SequencePostConfiguration import sequencePostConfiguration

sequencePostConfiguration (sequence, electronContainer)

for alg in sequence :
    config = alg["alg"]

    # set everything to debug output
    config.OutputLevel = 1

    job.algsAdd( config )
    pass


# Run the job using the direct driver.
driver = ROOT.EL.DirectDriver()
driver.submit( job, options.submission_dir )
