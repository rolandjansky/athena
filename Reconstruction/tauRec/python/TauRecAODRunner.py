# Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration

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

        tools_mod.append(taualgs.getMuonRemoval())
        tools_mod.append(taualgs.getElecRemoval())

        tools_after = []
        tools_after.append(taualgs.getTauVertexedClusterDecorator())      # WORK!!!
        tools_after.append(taualgs.getTauTrackRNNClassifier())
        tools_after.append(taualgs.getEnergyCalibrationLC())              # WORK!!!
        tools_after.append(taualgs.getTauCommonCalcVars())
        tools_after.append(taualgs.getTauSubstructure())                  # WORK!!!
        tools_after.append(taualgs.getPi0ClusterCreator())                # WORK!!!
        tools_after.append(taualgs.getPi0ClusterScaler())                 # WORK!!!
        tools_after.append(taualgs.getPi0ScoreCalculator())               # WORK!!!
        tools_after.append(taualgs.getPi0Selector())                      # WORK!!!

        import PanTauAlgs.JobOptions_Main_PanTau as pantau
        tools_after.append(pantau.getPanTau())                            # WORK!!!
        tools_after.append(taualgs.getTauCombinedTES())                   # WORK!!!
        tools_after.append(taualgs.getMvaTESVariableDecorator())          # WORK!!!
        tools_after.append(taualgs.getMvaTESEvaluator())                  # WORK!!!
        tools_after.append(taualgs.getTauIDVarCalculator())               # WORK!!!
        tools_after.append(taualgs.getTauJetRNNEvaluator())               # WORK!!!
        tools_after.append(taualgs.getTauWPDecoratorJetRNN())             # WORK!!!
        tools_after.append(taualgs.getTauEleRNNEvaluator())               # WORK!!!
        tools_after.append(taualgs.getTauWPDecoratorEleRNN())             # WORK!!!
        # tools_after.append(taualgs.getTauDecayModeNNClassifier())       # Charged PFO problem again!

        TauRecAODRunConfigured.WrapTauRecToolExecHandle(self, tools_mod=tools_mod, tools_after=tools_after)
        return True

# end
