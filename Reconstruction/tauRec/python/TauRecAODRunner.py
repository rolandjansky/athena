# Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration

################################################################################
##
# @file TauRecAODBuilder.py
#
# @brief Main steering file to set up the different tau reconstruction steps in AOD.
#
# @author D. Qichen
#
################################################################################

from AthenaCommon.Logging import logging
from .TauRecAODRunConfigured import TauRecAODRunConfigured

################################################################################
# @class TauRecAODRunner
# Build proper tau candidates and associate tracks, vertex and cells
################################################################################


class TauRecAODRunner (TauRecAODRunConfigured):
    """Re-reconstruct Tau candidates in AOD level
    Run algorithms that require pi0 cells, and all downstream algorithms.
    """

    def __init__(self, name="TauRecAODRunner",msglevel=3):
        self.name = name
        TauRecAODRunConfigured.__init__(self, name, msglevel=msglevel)

    def configure(self):
        mlog = logging.getLogger('TauRecAODRunner.py::configure:')
        mlog.info('entering')
        import tauRec.TauAlgorithmsHolder as taualgs
        tools_mod = []
        tools_mod.append(taualgs.getTauAODMuonRemovalTool())
        tools_after = []
        tools_after.append(taualgs.getTauVertexedClusterDecorator())
        tools_after.append(taualgs.getTauTrackRNNClassifier())
        tools_after.append(taualgs.getEnergyCalibrationLC())        
        tools_after.append(taualgs.getTauCommonCalcVars())
        tools_after.append(taualgs.getTauSubstructure())            
        tools_after.append(taualgs.getPi0ClusterCreator())          
        tools_after.append(taualgs.getPi0ClusterScaler())           
        tools_after.append(taualgs.getPi0ScoreCalculator())         
        tools_after.append(taualgs.getPi0Selector())
        tools_after.append(taualgs.getTauVertexVariables())
        import PanTauAlgs.JobOptions_Main_PanTau as pantau
        tools_after.append(pantau.getPanTau())                      
        tools_after.append(taualgs.getTauCombinedTES())             
        tools_after.append(taualgs.getMvaTESVariableDecorator())    
        tools_after.append(taualgs.getMvaTESEvaluator())            
        tools_after.append(taualgs.getTauIDVarCalculator())         
        tools_after.append(taualgs.getTauJetRNNEvaluator())         
        tools_after.append(taualgs.getTauWPDecoratorJetRNN())       
        tools_after.append(taualgs.getTauEleRNNEvaluator())         
        tools_after.append(taualgs.getTauWPDecoratorEleRNN())       
        tools_after.append(taualgs.getTauDecayModeNNClassifier())
        TauRecAODRunConfigured.WrapTauRecToolExecHandle(self, tools_mod=tools_mod, tools_after=tools_after)
        return True
