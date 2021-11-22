# Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
#
# @author Nils Krumnack

from AnaAlgorithm.AlgSequence import AlgSequence
from AnaAlgorithm.DualUseConfig import createAlgorithm, createService
from AsgAnalysisAlgorithms.AsgAnalysisAlgorithmsTest import pileupConfigFiles

def makeSequence (dataType, jetContainer="AntiKt4EMPFlowJets") :

    # config
    

    algSeq = AlgSequence()

    # Set up the systematics loader/handler service:
    sysService = createService( 'CP::SystematicsSvc', 'SystematicsSvc', sequence = algSeq )
    sysService.sigmaRecommended = 1

    # Include, and then set up the pileup analysis sequence:
    prwfiles, lumicalcfiles = pileupConfigFiles(dataType)

    from AsgAnalysisAlgorithms.PileupAnalysisSequence import \
        makePileupAnalysisSequence
    pileupSequence = makePileupAnalysisSequence(
        dataType,
        userPileupConfigs=prwfiles,
        userLumicalcFiles=lumicalcfiles,
    )
    pileupSequence.configure( inputName = {}, outputName = {} )
    print( pileupSequence ) # For debugging

    # Include, and then set up the jet analysis algorithm sequence:
    from JetAnalysisAlgorithms.JetAnalysisSequence import makeJetAnalysisSequence
    jetSequence = makeJetAnalysisSequence( dataType, jetContainer, enableCutflow=True, enableKinematicHistograms=True )
    jetSequence.configure( inputName = jetContainer, outputName = 'AnalysisJetsBase_%SYS%' )
    print( jetSequence ) # For debugging

    # Include, and then set up the jet analysis algorithm sequence:
    from JetAnalysisAlgorithms.JetJvtAnalysisSequence import makeJetJvtAnalysisSequence
    jvtSequence = makeJetJvtAnalysisSequence( dataType, jetContainer, enableCutflow=True )
    jvtSequence.configure( inputName = { 'jets'      : 'AnalysisJetsBase_%SYS%' },
                           outputName = { 'jets'      : 'AnalysisJets_%SYS%' } )
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
            'AnalysisJets_%SYS%.jvt_effSF_%SYS% -> jet_%SYS%_jvtEfficiency',
            # 'AnalysisJets_%SYS%.fjvt_effSF_NOSYS -> jet_%SYS%_fjvtEfficiency',
            ]
        algSeq += ntupleMaker
    treeFiller = createAlgorithm( 'CP::TreeFillerAlg', 'TreeFiller' )
    treeFiller.TreeName = 'jets'
    algSeq += treeFiller

    return algSeq
