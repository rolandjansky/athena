# Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
#
# @author Nils Krumnack

from AnaAlgorithm.AlgSequence import AlgSequence
from AnaAlgorithm.DualUseConfig import createAlgorithm

def makeSequence (dataType) :

    algSeq = AlgSequence()

    # Set up the systematics loader/handler algorithm:
    sysLoader = createAlgorithm( 'CP::SysListLoaderAlg', 'SysLoaderAlg' )
    sysLoader.sigmaRecommended = 1
    algSeq += sysLoader

    # Include, and then set up the tau analysis algorithm sequence:
    from TauAnalysisAlgorithms.TauAnalysisSequence import makeTauAnalysisSequence
    tauSequence = makeTauAnalysisSequence( dataType, 'Tight', postfix = 'tight',
                                           enableCutflow=True, enableKinematicHistograms=True )
    tauSequence.configure( inputName = 'TauJets', outputName = 'AnalysisTauJets_%SYS%' )

    # Add the sequence to the job:
    algSeq += tauSequence

    # Include, and then set up the tau analysis algorithm sequence:
    from TauAnalysisAlgorithms.DiTauAnalysisSequence import makeDiTauAnalysisSequence
    diTauSequence = makeDiTauAnalysisSequence( dataType, 'Tight', postfix = 'tight' )
    diTauSequence.configure( inputName = 'DiTauJets', outputName = 'AnalysisDiTauJets_%SYS%' )

    # Add the sequence to the job:
    # disabling this, the standard test files don't have DiTauJets
    # algSeq += diTauSequence

    return algSeq
