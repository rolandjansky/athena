# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

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

import os, sys, string
from AthenaCommon.Logging import logging
from AthenaCommon.SystemOfUnits import *
from AthenaCommon.Constants import *
from AthenaCommon.BeamFlags import jobproperties
import traceback
from RecExConfig.Configured import Configured
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
                
        from RecExConfig.RecFlags import rec                
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

        tools.append(taualgs.getEnergyCalibrationLC(correctEnergy=False, correctAxis=True, postfix='_onlyAxis'))

        # PanTau:
        if tauFlags.doPanTau() :
            import PanTauAlgs.JobOptions_Main_PanTau as pantau
            tools.append(pantau.getPanTau())
 
        # these tools need pantau info
        tools.append(taualgs.getCombinedP4FromRecoTaus())
        if jobproperties.Beam.beamType()!="cosmics":
            tools.append(taualgs.getMvaTESVariableDecorator())
            tools.append(taualgs.getMvaTESEvaluator())

        if tauFlags.doRunTauDiscriminant():
            tools.append(taualgs.getTauIDVarCalculator())
            tools.append(taualgs.getTauJetBDTEvaluator("TauJetBDT1P", weightsFile="vars2016_pt_gamma_1p_isofix.root", minNTracks=0, maxNTracks=1))
            tools.append(taualgs.getTauJetBDTEvaluator("TauJetBDT3P", weightsFile="vars2016_pt_gamma_3p_isofix.root", minNTracks=2, maxNTracks=1000))
            tools.append(taualgs.getTauWPDecoratorJetBDT())
            tools.append(taualgs.getTauJetRNNEvaluator("TauJetRNN",
                                                       NetworkFile1P="rnnid_mc16d_config_1p.json",
                                                       NetworkFile3P="rnnid_mc16d_config_3p.json",
                                                       OutputVarname="RNNJetScore", MaxTracks=10, MaxClusters=6))
            tools.append(taualgs.getTauWPDecoratorJetRNN())
            tools.append(taualgs.getTauJetBDTEvaluator("TauEleBDT_def", weightsFile="", outputVarName="BDTEleScore"))#just inits values
            tools.append(taualgs.getTauJetBDTEvaluator("TauEleBDT_bar", 
                                                       weightsFile="EleBDT1PBar.root", minNTracks=1, maxAbsTrackEta=1.37, 
                                                       outputVarName="BDTEleScore"))
            tools.append(taualgs.getTauJetBDTEvaluator("TauEleBDT_end1", 
                                                       weightsFile="EleBDT1PEnd1.root", minNTracks=1, minAbsTrackEta=1.37, 
                                                       maxAbsTrackEta=2.0, outputVarName="BDTEleScore"))
            tools.append(taualgs.getTauJetBDTEvaluator("TauEleBDT_end23", 
                                                       weightsFile="EleBDT1PEnd23.root", minNTracks=1, minAbsTrackEta=2.0, 
                                                       maxAbsTrackEta=3.0, outputVarName="BDTEleScore"))
            tools.append(taualgs.getTauWPDecoratorEleBDT())
            tools.append(taualgs.getTauEleRNNEvaluator("TauEleRNN",
                                                         NetworkFile1P="rnneveto_mc16d_config_1p.json",
                                                         NetworkFile3P="rnneveto_mc16d_config_3p.json",
                                                         OutputVarname="RNNEleScore", MaxTracks=10, MaxClusters=6))
            tools.append(taualgs.getTauWPDecoratorEleRNN())
            tools.append(taualgs.getTauDecayModeNNClassifier())
            tools.append(taualgs.getTauEleOLRDecorator())              
            
            ################################

        from tauRec.tauRecFlags import tauFlags
        tools+=tauFlags.tauRecToolsDevToolList()
        #self.TauBuilderToolHandle().Tools = tools
            
                
        # run first part of Tau Builder
        TauRecRunConfigured.WrapTauRecToolExecHandle(self, tool=tools)
        return True        

#end
