# Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
#
# @author Nils Krumnack

from AnaAlgorithm.AlgSequence import AlgSequence
from AnaAlgorithm.DualUseConfig import createAlgorithm

def makeSequence (dataType) :

    # config parameters
    jetContainer = "AntiKt4EMTopoJets"

    algSeq = AlgSequence()

    # Set up the systematics loader/handler algorithm:
    sysLoader = createAlgorithm( 'CP::SysListLoaderAlg', 'SysLoaderAlg' )
    sysLoader.sigmaRecommended = 1
    algSeq += sysLoader

    # Include, and then set up the jet analysis algorithm sequence:
    from JetAnalysisAlgorithms.JetAnalysisSequence import makeJetAnalysisSequence
    jetSequence = makeJetAnalysisSequence( dataType, jetContainer )
    from FTagAnalysisAlgorithms.FTagAnalysisSequence import makeFTagAnalysisSequence
    makeFTagAnalysisSequence( jetSequence, dataType, jetContainer, noEfficiency = True )
    jetSequence.configure( inputName = jetContainer, outputName = 'AnalysisJets' )

    # Add the sequence to the job:
    algSeq += jetSequence

    return algSeq
