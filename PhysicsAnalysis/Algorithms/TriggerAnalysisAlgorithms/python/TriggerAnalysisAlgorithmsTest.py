# Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
#
# @author Tadej Novak
# @author Nils Krumnack

from AnaAlgorithm.AlgSequence import AlgSequence
from AnaAlgorithm.DualUseConfig import createAlgorithm

def makeSequence (dataType) :

    # Config:
    triggerChains = [
        'HLT_2mu14',
        'HLT_mu20_mu8noL1',
        'HLT_2e17_lhvloose_nod0'
        ]


    algSeq = AlgSequence()

    # Set up the systematics loader/handler algorithm:
    alg = createAlgorithm( 'CP::SysListLoaderAlg', 'SysLoaderAlg' )
    alg.sigmaRecommended = 1
    algSeq += alg

    # Include, and then set up the pileup analysis sequence:
    from TriggerAnalysisAlgorithms.TriggerAnalysisSequence import \
        makeTriggerAnalysisSequence
    triggerSequence = makeTriggerAnalysisSequence( dataType, triggerChains=triggerChains )
    algSeq += triggerSequence

    # Set up an ntuple to check the job with:
    treeMaker = createAlgorithm( 'CP::TreeMakerAlg', 'TreeMaker' )
    treeMaker.TreeName = 'events'
    algSeq += treeMaker
    ntupleMaker = createAlgorithm( 'CP::AsgxAODNTupleMakerAlg', 'NTupleMaker' )
    ntupleMaker.TreeName = 'events'
    ntupleMaker.Branches = [
        'EventInfo.runNumber   -> runNumber',
        'EventInfo.eventNumber -> eventNumber',
        ]
    ntupleMaker.Branches += ['EventInfo.trigPassed_' + t + ' -> trigPassed_' + t for t in triggerChains]
    ntupleMaker.systematicsRegex = '.*'
    algSeq += ntupleMaker
    treeFiller = createAlgorithm( 'CP::TreeFillerAlg', 'TreeFiller' )
    treeFiller.TreeName = 'events'
    algSeq += treeFiller

    return algSeq
