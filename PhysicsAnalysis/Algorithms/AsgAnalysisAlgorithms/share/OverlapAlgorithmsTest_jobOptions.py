# Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
#
# @author Nils Krumnack

# Ideally we'd run over all of them, but we don't have a mechanism to
# configure per-sample right now
dataType = "data"
#dataType = "mc"
#dataType = "afii"
inputfile = {"data": 'ASG_TEST_FILE_DATA',
             "mc":   'ASG_TEST_FILE_MC',
             "afii": 'ASG_TEST_FILE_MC_AFII'}

# Set up the reading of the input file:
import AthenaRootComps.ReadAthenaxAODHybrid
theApp.EvtMax = 500
testFile = os.getenv ( inputfile[dataType] )
svcMgr.EventSelector.InputCollections = [testFile]

# Access the main algorithm sequence of the job:
from AthenaCommon.AlgSequence import AlgSequence
topSeq = AlgSequence()

# Set up a job-specific sequence on top of that, one that we can stop with
# filter algorithms if necessary:
algSeq = AlgSequence( "AnalysisSequence" )
topSeq += algSeq

# Skip events with no primary vertex:
algSeq += CfgMgr.CP__VertexSelectionAlg( 'PrimaryVertexSelectorAlg',
                                         VertexContainer = 'PrimaryVertices',
                                         MinVertices = 1 )

# Set up the systematics loader/handler algorithm:
sysLoader = CfgMgr.CP__SysListLoaderAlg( 'SysLoaderAlg' )
sysLoader.sigmaRecommended = 1
algSeq += sysLoader

# Include, and then set up the pileup analysis sequence:
from AsgAnalysisAlgorithms.PileupAnalysisSequence import \
    makePileupAnalysisSequence
pileupSequence = makePileupAnalysisSequence( dataType )
pileupSequence.configure( inputName = 'EventInfo', outputName = 'EventInfo' )
algSeq += pileupSequence

# Include, and then set up the electron analysis sequence:
from EgammaAnalysisAlgorithms.ElectronAnalysisSequence import \
    makeElectronAnalysisSequence
electronSequence = makeElectronAnalysisSequence( dataType,
                                                 recomputeLikelihood = True )
electronSequence.configure( inputName = 'Electrons',
                            outputName = 'AnalysisElectrons_%SYS%' )
algSeq += electronSequence

# Include, and then set up the photon analysis sequence:
from EgammaAnalysisAlgorithms.PhotonAnalysisSequence import \
    makePhotonAnalysisSequence
photonSequence = makePhotonAnalysisSequence( dataType, recomputeIsEM = True )
photonSequence.configure( inputName = 'Photons',
                          outputName = 'AnalysisPhotons_%SYS%' )
algSeq += photonSequence

# Include, and then set up the muon analysis algorithm sequence:
from MuonAnalysisAlgorithms.MuonAnalysisSequence import makeMuonAnalysisSequence
muonSequence = makeMuonAnalysisSequence( dataType )
muonSequence.configure( inputName = 'Muons',
                        outputName = 'AnalysisMuons_%SYS%' )
algSeq += muonSequence

# Include, and then set up the jet analysis algorithm sequence:
jetContainer = 'AntiKt4EMTopoJets'
from JetAnalysisAlgorithms.JetAnalysisSequence import makeJetAnalysisSequence
jetSequence = makeJetAnalysisSequence( dataType, jetContainer )
jetSequence.configure( inputName = jetContainer,
                       outputName = 'AnalysisJets_%SYS%' )
algSeq += jetSequence

# Include, and then set up the tau analysis algorithm sequence:
from TauAnalysisAlgorithms.TauAnalysisSequence import makeTauAnalysisSequence
tauSequence = makeTauAnalysisSequence( dataType )
tauSequence.configure( inputName = 'TauJets',
                       outputName = 'AnalysisTauJets_%SYS%' )
algSeq += tauSequence

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
algSeq += overlapSequence

# Set up an ntuple to check the job with:
ntupleMaker = CfgMgr.CP__AsgxAODNTupleMakerAlg( 'NTupleMaker' )
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
algSeq += ntupleMaker

# For debugging:
print( algSeq )

# Set up a histogram output file for the job:
ServiceMgr += CfgMgr.THistSvc()
ServiceMgr.THistSvc.Output += [
    "ANALYSIS DATAFILE='OverlapAlgorithmsTest.root' OPT='RECREATE'"
    ]

# Reduce the printout from Athena:
include( "AthAnalysisBaseComps/SuppressLogging.py" )
