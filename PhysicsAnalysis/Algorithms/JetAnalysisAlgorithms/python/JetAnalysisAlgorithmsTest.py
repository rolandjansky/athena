# Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
#
# @author Nils Krumnack

from AnaAlgorithm.AlgSequence import AlgSequence
from AnaAlgorithm.DualUseConfig import createAlgorithm

def makeSequence (dataType) :

    # config
    jetContainer = "AntiKt4EMTopoJets"

    algSeq = AlgSequence()

    # Set up the systematics loader/handler algorithm:
    sysLoader = createAlgorithm( 'CP::SysListLoaderAlg', 'SysLoaderAlg' )
    sysLoader.sigmaRecommended = 1
    algSeq += sysLoader

    # Include, and then set up the pileup analysis sequence:
    from AsgAnalysisAlgorithms.PileupAnalysisSequence import \
        makePileupAnalysisSequence
    pileupSequence = makePileupAnalysisSequence( dataType )
    pileupSequence.configure( inputName = 'EventInfo', outputName = 'EventInfo' )
    print( pileupSequence ) # For debugging

    # Include, and then set up the jet analysis algorithm sequence:
    from JetAnalysisAlgorithms.JetAnalysisSequence import makeJetAnalysisSequence
    jetSequence = makeJetAnalysisSequence( dataType, jetContainer )
    jetSequence.configure( inputName = jetContainer, outputName = 'AnalysisJetsBase' )
    print( jetSequence ) # For debugging

    # Include, and then set up the jet analysis algorithm sequence:
    from JetAnalysisAlgorithms.JetJvtAnalysisSequence import makeJetJvtAnalysisSequence
    jvtSequence = makeJetJvtAnalysisSequence( dataType, jetContainer )
    jvtSequence.configure( inputName = { 'eventInfo' : 'EventInfo_%SYS%',
                                         'jets'      : 'AnalysisJetsBase_%SYS%' },
                           outputName = { 'jets'      : 'AnalysisJets_%SYS%' },
                           affectingSystematics = { 'jets' : jetSequence.affectingSystematics() } )
    print( jvtSequence ) # For debugging

    # Add the sequences to the job:
    algSeq += pileupSequence
    algSeq += jetSequence
    algSeq += jvtSequence

    # Set up an ntuple to check the job with:
    treeMaker = createAlgorithm( 'CP::TreeMakerAlg', 'TreeMaker' )
    treeMaker.TreeName = 'jets'
    algSeq += treeMaker
    ntupleMaker = createAlgorithm( 'CP::AsgxAODNTupleMakerAlg', 'NTupleMaker' )
    ntupleMaker.TreeName = 'jets'
    ntupleMaker.Branches = [
        'EventInfo.runNumber   -> runNumber',
        'EventInfo.eventNumber -> eventNumber',
        'AnalysisJets_%SYS%.pt -> jet_%SYS%_pt',
        ]
    if dataType != 'data':
        ntupleMaker.Branches += [
            # 'EventInfo.jvt_effSF_%SYS% -> jvtSF_%SYS%',
            # 'EventInfo.fjvt_effSF_%SYS% -> fjvtSF_%SYS%',
            'AnalysisJets_%SYS%.jvt_effSF_NOSYS -> jet_%SYS%_jvtEfficiency',
            'AnalysisJets_%SYS%.fjvt_effSF_NOSYS -> jet_%SYS%_fjvtEfficiency',
            ]
        ntupleMaker.systematicsRegex = '(^$)|(^JET_.*)'
        algSeq += ntupleMaker
    treeFiller = createAlgorithm( 'CP::TreeFillerAlg', 'TreeFiller' )
    treeFiller.TreeName = 'jets'
    algSeq += treeFiller

    return algSeq
