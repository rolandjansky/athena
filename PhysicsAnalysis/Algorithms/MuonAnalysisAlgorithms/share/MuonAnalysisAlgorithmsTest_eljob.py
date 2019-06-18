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

# ideally we'd run over all of them, but we don't have a mechanism to
# configure per-sample right now

dataType = options.data_type

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

# Set up the systematics loader/handler algorithm:
from AnaAlgorithm.AnaAlgorithmConfig import AnaAlgorithmConfig
sysLoader = AnaAlgorithmConfig( 'CP::SysListLoaderAlg/SysLoaderAlg' )
sysLoader.sigmaRecommended = 1
job.algsAdd( sysLoader )

# Include, and then set up the pileup analysis sequence:
from AsgAnalysisAlgorithms.PileupAnalysisSequence import \
    makePileupAnalysisSequence
pileupSequence = makePileupAnalysisSequence( dataType )
pileupSequence.configure( inputName = 'EventInfo', outputName = 'EventInfo' )
print( pileupSequence ) # For debugging

# Add the pileup algorithm(s) to the job:
for alg in pileupSequence:
    job.algsAdd( alg )
    pass

# Include, and then set up the muon analysis algorithm sequence:
from MuonAnalysisAlgorithms.MuonAnalysisSequence import makeMuonAnalysisSequence
muonSequenceMedium = makeMuonAnalysisSequence( dataType, deepCopyOutput = True, shallowViewOutput = False,
                                               workingPoint = 'Medium.Iso', postfix = 'medium' )
muonSequenceMedium.configure( inputName = 'Muons',
                              outputName = 'AnalysisMuonsMedium_%SYS%' )
print( muonSequenceMedium ) # For debugging

# Add all algorithms to the job:
for alg in muonSequenceMedium:
    job.algsAdd( alg )
    pass

muonSequenceTight = makeMuonAnalysisSequence( dataType, deepCopyOutput = True, shallowViewOutput = False,
                                               workingPoint = 'Tight.Iso', postfix = 'tight' )
muonSequenceTight.removeStage ("calibration")
muonSequenceTight.configure( inputName = 'AnalysisMuonsMedium_%SYS%',
                             outputName = 'AnalysisMuons_%SYS%',
                             affectingSystematics = muonSequenceMedium.affectingSystematics())
print( muonSequenceTight ) # For debugging

# Add all algorithms to the job:
for alg in muonSequenceTight:
    job.algsAdd( alg )
    pass

# Add ntuple dumper algorithms:
from AnaAlgorithm.DualUseConfig import createAlgorithm
treeMaker = createAlgorithm( 'CP::TreeMakerAlg', 'TreeMaker' )
treeMaker.TreeName = 'muons'
job.algsAdd( treeMaker )
ntupleMaker = createAlgorithm( 'CP::AsgxAODNTupleMakerAlg', 'NTupleMakerEventInfo' )
ntupleMaker.TreeName = 'muons'
ntupleMaker.Branches = [ 'EventInfo.runNumber     -> runNumber',
                         'EventInfo.eventNumber   -> eventNumber', ]
ntupleMaker.systematicsRegex = '(^$)'
job.algsAdd( ntupleMaker )
ntupleMaker = createAlgorithm( 'CP::AsgxAODNTupleMakerAlg', 'NTupleMakerMuons' )
ntupleMaker.TreeName = 'muons'
ntupleMaker.Branches = [ 'AnalysisMuons_NOSYS.eta -> mu_eta',
                         'AnalysisMuons_NOSYS.phi -> mu_phi',
                         'AnalysisMuons_%SYS%.pt  -> mu_%SYS%_pt', ]
ntupleMaker.systematicsRegex = '(^MUON_.*)'
job.algsAdd( ntupleMaker )
treeFiller = createAlgorithm( 'CP::TreeFillerAlg', 'TreeFiller' )
treeFiller.TreeName = 'muons'
job.algsAdd( treeFiller )

# Set up a mini-xAOD writer algorithm:
xaodWriter = AnaAlgorithmConfig( 'CP::xAODWriterAlg/xAODWriter' )
xaodWriter.ItemList = \
   [ 'xAOD::EventInfo#EventInfo',
     'xAOD::EventAuxInfo#EventInfoAux.-',
     'xAOD::MuonContainer#AnalysisMuons_NOSYS',
     'xAOD::AuxContainerBase#AnalysisMuons_NOSYSAux.eta.phi.pt' ]
xaodWriter.systematicsRegex = '.*'
job.algsAdd( xaodWriter )

# Make sure that both the ntuple and the xAOD dumper have a stream to write to.
job.outputAdd( ROOT.EL.OutputStream( 'ANALYSIS' ) )

# Find the right output directory:
submitDir = options.submission_dir
if options.unit_test:
    import os
    import tempfile
    submitDir = tempfile.mkdtemp( prefix = 'muonTest_'+dataType+'_', dir = os.getcwd() )
    os.rmdir( submitDir )
    pass

# Run the job using the direct driver.
driver = ROOT.EL.DirectDriver()
driver.submit( job, submitDir )
