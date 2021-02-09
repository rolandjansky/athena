# Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration

################################################################################
##
#@file TauRecBuilder.py
#
#@brief Main steering file to set up the different tau reconstruction steps.
#
#@author N. Meyer
#@author A. Kaczmarska
#@author Felix Friedrich
#
################################################################################

from AthenaCommon.Logging import logging
from AthenaCommon.BeamFlags import jobproperties
from .TauRecRunConfigured import TauRecRunConfigured

################################################################################
## @class TauRecRunner
# Build proper tau candidates and associate tracks, vertex and cells
################################################################################
class TauRecRunner ( TauRecRunConfigured ) :
    """Build final tau candidates.
    Run algorithms that require pi0 cells, and all downstream algorithms.
    """
  
    def __init__(self, name = "TauRecRunner"):
        self.name = name
        TauRecRunConfigured.__init__(self, name)
 
    def configure(self):
        mlog = logging.getLogger ('TauRecRunner.py::configure:')
        mlog.info('entering')

        import tauRec.TauAlgorithmsHolder as taualgs        
        from tauRec.tauRecFlags import tauFlags                
        
        tools = []
        
        tools.append(taualgs.getPi0ClusterCreator())
        tools.append(taualgs.getPi0ClusterScaler())
        tools.append(taualgs.getPi0ScoreCalculator())
        tools.append(taualgs.getPi0Selector())
        
        from InDetRecExample.InDetJobProperties import InDetFlags
        if tauFlags.isStandalone() or InDetFlags.doVertexFinding():
            tools.append(taualgs.getTauVertexVariables())

        tools.append(taualgs.getTauCommonCalcVars())
        tools.append(taualgs.getTauSubstructure())

        # PanTau:
        if tauFlags.doPanTau() :
            import PanTauAlgs.JobOptions_Main_PanTau as pantau
            tools.append(pantau.getPanTau())
 
        # these tools need pantau info
        tools.append(taualgs.getTauCombinedTES())
        if jobproperties.Beam.beamType()!="cosmics":
            tools.append(taualgs.getMvaTESVariableDecorator())
            tools.append(taualgs.getMvaTESEvaluator())

        if tauFlags.doRunTauDiscriminant():
            tools.append(taualgs.getTauIDVarCalculator())
            tools.append(taualgs.getTauJetRNNEvaluator())
            tools.append(taualgs.getTauWPDecoratorJetRNN())
            tools.append(taualgs.getTauEleRNNEvaluator())
            tools.append(taualgs.getTauWPDecoratorEleRNN())
            tools.append(taualgs.getTauDecayModeNNClassifier())
            
            ################################

        from tauRec.tauRecFlags import tauFlags
        tools+=tauFlags.tauRecToolsDevToolList()
        #self.TauBuilderToolHandle().Tools = tools
            
                
        # run first part of Tau Builder
        TauRecRunConfigured.WrapTauRecToolExecHandle(self, tool=tools)
        return True        

#end
