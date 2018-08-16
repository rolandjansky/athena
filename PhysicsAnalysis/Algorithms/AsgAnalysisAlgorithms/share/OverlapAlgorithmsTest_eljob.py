#!/usr/bin/env python
#
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
dataType = "data"
#dataType = "mc"
#dataType = "afii"
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

# Skip events with no primary vertex:
from AnaAlgorithm.AnaAlgorithmConfig import AnaAlgorithmConfig
config = AnaAlgorithmConfig( 'CP::VertexSelectionAlg/PrimaryVertexSelectorAlg',
                             VertexContainer = 'PrimaryVertices',
                             MinVertices = 1 )
job.algsAdd( config )

# Set up the systematics loader/handler algorithm:
config = AnaAlgorithmConfig( 'CP::SysListLoaderAlg/SysLoaderAlg' )
config.sigmaRecommended = 1
job.algsAdd( config )

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

# Include, and then set up the electron analysis sequence:
from EgammaAnalysisAlgorithms.ElectronAnalysisSequence import \
    makeElectronAnalysisSequence
electronSequence = makeElectronAnalysisSequence( dataType,
                                                 recomputeLikelihood = True )
electronSequence.configure( inputName = 'Electrons',
                            outputName = 'AnalysisElectrons_%SYS%' )
print( electronSequence ) # For debugging

# Add the electron algorithm(s) to the job:
for alg in electronSequence:
    job.algsAdd( alg )
    pass

# Include, and then set up the photon analysis sequence:
from EgammaAnalysisAlgorithms.PhotonAnalysisSequence import \
    makePhotonAnalysisSequence
photonSequence = makePhotonAnalysisSequence( dataType, recomputeIsEM = True )
photonSequence.configure( inputName = 'Photons',
                          outputName = 'AnalysisPhotons_%SYS%' )
print( photonSequence ) # For debugging

# Add the photon algorithm(s) to the job:
for alg in photonSequence:
    job.algsAdd( alg )
    pass

# Include, and then set up the muon analysis algorithm sequence:
from MuonAnalysisAlgorithms.MuonAnalysisSequence import makeMuonAnalysisSequence
muonSequence = makeMuonAnalysisSequence( dataType )
muonSequence.configure( inputName = 'Muons',
                        outputName = 'AnalysisMuons_%SYS%' )
print( muonSequence ) # For debugging

# Add all algorithms to the job:
for alg in muonSequence:
    job.algsAdd( alg )
    pass

# Include, and then set up the jet analysis algorithm sequence:
jetContainer = 'AntiKt4EMTopoJets'
from JetAnalysisAlgorithms.JetAnalysisSequence import makeJetAnalysisSequence
jetSequence = makeJetAnalysisSequence( dataType, jetContainer )
jetSequence.configure( inputName = jetContainer,
                       outputName = 'AnalysisJets_%SYS%' )
print( jetSequence ) # For debugging

# Add all algorithms to the job:
for alg in jetSequence:
    job.algsAdd( alg )
    pass

# Include, and then set up the tau analysis algorithm sequence:
from TauAnalysisAlgorithms.TauAnalysisSequence import makeTauAnalysisSequence
tauSequence = makeTauAnalysisSequence( dataType )
tauSequence.configure( inputName = 'TauJets',
                       outputName = 'AnalysisTauJets_%SYS%' )
print( tauSequence ) # For debugging

# Add all algorithms to the job:
for alg in tauSequence:
  job.algsAdd( alg )
  pass

# Include, and then set up the overlap analysis algorithm sequence:
from AsgAnalysisAlgorithms.OverlapAnalysisSequence import \
    makeOverlapAnalysisSequence
overlapSequence = makeOverlapAnalysisSequence( dataType )
overlapSequence.configure(
    inputName = {
      'electrons' : 'AnalysisElectrons_%SYS%',
      'photons'   : 'AnalysisPhotons_%SYS%',
      'muons'     : 'AnalysisMuons_%SYS%',
      'jets'      : 'AnalysisJets_%SYS%',
      'taus'      : 'AnalysisTauJets_%SYS%' },
    outputName = {
      'electrons' : 'AnalysisElectronsOR_%SYS%',
      'photons'   : 'AnalysisPhotonsOR_%SYS%',
      'muons'     : 'AnalysisMuonsOR_%SYS%',
      'jets'      : 'AnalysisJetsOR_%SYS%',
      'taus'      : 'AnalysisTauJetsOR_%SYS%' },
    affectingSystematics = {
      'electrons' : '(^$)|(^EG_.*)|(^EL_.*)',
      'photons'   : '(^$)|(^EG_.*)|(^PH_.*)',
      'muons'     : '(^$)|(^MUON_.*)',
      'jets'      : '(^$)|(^JET_.*)',
      'taus'      : '(^$)|(^TAUS_.*)' } )
print( overlapSequence ) # For debugging

# Add all algorithms to the job:
for alg in overlapSequence:
    job.algsAdd( alg )
    pass

# Set up an ntuple to check the job with:
ntupleMaker = AnaAlgorithmConfig( 'CP::AsgxAODNTupleMakerAlg/NTupleMaker' )
ntupleMaker.TreeName = 'particles'
ntupleMaker.Branches = [
    'EventInfo.runNumber   -> runNumber',
    'EventInfo.eventNumber -> eventNumber',
    'AnalysisElectrons_%SYS%.eta -> el_%SYS%_eta',
    'AnalysisElectrons_%SYS%.phi -> el_%SYS%_phi',
    'AnalysisElectrons_%SYS%.pt  -> el_%SYS%_pt',
    'AnalysisElectronsOR_%SYS%.eta -> el_OR_%SYS%_eta',
    'AnalysisElectronsOR_%SYS%.phi -> el_OR_%SYS%_phi',
    'AnalysisElectronsOR_%SYS%.pt  -> el_OR_%SYS%_pt',
    'AnalysisPhotons_%SYS%.eta -> ph_%SYS%_eta',
    'AnalysisPhotons_%SYS%.phi -> ph_%SYS%_phi',
    'AnalysisPhotons_%SYS%.pt  -> ph_%SYS%_pt',
    'AnalysisPhotonsOR_%SYS%.eta -> ph_OR_%SYS%_eta',
    'AnalysisPhotonsOR_%SYS%.phi -> ph_OR_%SYS%_phi',
    'AnalysisPhotonsOR_%SYS%.pt  -> ph_OR_%SYS%_pt',
    'AnalysisMuons_%SYS%.eta -> mu_%SYS%_eta',
    'AnalysisMuons_%SYS%.phi -> mu_%SYS%_phi',
    'AnalysisMuons_%SYS%.pt  -> mu_%SYS%_pt',
    'AnalysisMuonsOR_%SYS%.eta -> mu_OR_%SYS%_eta',
    'AnalysisMuonsOR_%SYS%.phi -> mu_OR_%SYS%_phi',
    'AnalysisMuonsOR_%SYS%.pt  -> mu_OR_%SYS%_pt',
    'AnalysisJets_%SYS%.eta -> jet_%SYS%_eta',
    'AnalysisJets_%SYS%.phi -> jet_%SYS%_phi',
    'AnalysisJets_%SYS%.pt  -> jet_%SYS%_pt',
    'AnalysisJetsOR_%SYS%.eta -> jet_OR_%SYS%_eta',
    'AnalysisJetsOR_%SYS%.phi -> jet_OR_%SYS%_phi',
    'AnalysisJetsOR_%SYS%.pt  -> jet_OR_%SYS%_pt',
    'AnalysisTauJets_%SYS%.eta -> tau_%SYS%_eta',
    'AnalysisTauJets_%SYS%.phi -> tau_%SYS%_phi',
    'AnalysisTauJets_%SYS%.pt  -> tau_%SYS%_pt',
    'AnalysisTauJetsOR_%SYS%.eta -> tau_OR_%SYS%_eta',
    'AnalysisTauJetsOR_%SYS%.phi -> tau_OR_%SYS%_phi',
    'AnalysisTauJetsOR_%SYS%.pt  -> tau_OR_%SYS%_pt' ]
ntupleMaker.systematicsRegex = '.*'
job.algsAdd( ntupleMaker )
job.outputAdd( ROOT.EL.OutputStream( 'ANALYSIS' ) )

# Find the right output directory:
submitDir = options.submission_dir
if options.unit_test:
    import os
    import tempfile
    submitDir = tempfile.mkdtemp( prefix = 'overlapTest_', dir = os.getcwd() )
    os.rmdir( submitDir )
    pass

# Run the job using the direct driver.
driver = ROOT.EL.DirectDriver()
driver.submit( job, submitDir )
