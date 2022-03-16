# Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
#
# @author Nils Krumnack

from AnaAlgorithm.AlgSequence import AlgSequence
from AnaAlgorithm.DualUseConfig import createService
from AnalysisAlgorithmsConfig.ConfigSequence import ConfigSequence
from AnalysisAlgorithmsConfig.ConfigAccumulator import ConfigAccumulator

def makeSequence (dataType) :

    algSeq = AlgSequence()

    # Set up the systematics loader/handler service:
    sysService = createService( 'CP::SystematicsSvc', 'SystematicsSvc', sequence = algSeq )
    sysService.sigmaRecommended = 1


    configSeq = ConfigSequence ()


    # Include, and then set up the tau analysis algorithm sequence:
    from TauAnalysisAlgorithms.TauAnalysisConfig import makeTauCalibrationConfig, makeTauWorkingPointConfig

    makeTauCalibrationConfig (configSeq, 'AnalysisTauJets')
    makeTauWorkingPointConfig (configSeq, 'AnalysisTauJets', workingPoint='Tight', postfix='tight')


    # temporarily disabled until di-taus are supported in R22
    # Include, and then set up the tau analysis algorithm sequence:
    # from TauAnalysisAlgorithms.DiTauAnalysisConfig import makeDiTauCalibrationConfig, makeDiTauWorkingPointConfig

    # disabling this, the standard test files don't have DiTauJets
    # makeDiTauCalibrationConfig (configSeq, 'AnalysisDiTauJets')
    # makeDiTauWorkingPointConfig (configSeq, 'AnalysisDiTauJets', workingPoint='Tight', postfix='tight')


    configAccumulator = ConfigAccumulator (dataType, algSeq)
    configSeq.fullConfigure (configAccumulator)

    return algSeq
