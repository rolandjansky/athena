# Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
#
# @author Nils Krumnack

# Ideally we'd run over all of them, but we don't have a mechanism to
# configure per-sample right now
# dataType = "data"
dataType = "mc"
#dataType = "afii"
inputfile = {"data": 'ASG_TEST_FILE_DATA',
             "mc":   'ASG_TEST_FILE_MC',
             "afii": 'ASG_TEST_FILE_MC_AFII'}
jetContainer = "AntiKt4EMTopoJets"

# Set up the reading of the input file:
import AthenaRootComps.ReadAthenaxAODHybrid
theApp.EvtMax = 500
testFile = os.getenv ( inputfile[dataType] )
svcMgr.EventSelector.InputCollections = [testFile]

# Access the main algorithm sequence of the job:
from AthenaCommon.AlgSequence import AlgSequence
algSeq = AlgSequence()

# Set up the systematics loader/handler algorithm:
sysLoader = CfgMgr.CP__SysListLoaderAlg( 'SysLoaderAlg' )
sysLoader.sigmaRecommended = 1
algSeq += sysLoader

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

# Add the sequences to the job:
algSeq += jetSequence
algSeq += jvtSequence

# Set up an ntuple to check the job with:
ntupleMaker = CfgMgr.CP__AsgxAODNTupleMakerAlg( 'NTupleMaker' )
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
algSeq += ntupleMaker

# Set up a histogram output file for the job:
ServiceMgr += CfgMgr.THistSvc()
ServiceMgr.THistSvc.Output += [
    "ANALYSIS DATAFILE='JetAnalysisAlgorithmsTest.hist.root' OPT='RECREATE'"
    ]

# Reduce the printout from Athena:
include( "AthAnalysisBaseComps/SuppressLogging.py" )
