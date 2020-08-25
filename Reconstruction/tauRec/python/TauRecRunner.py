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

# global tauRec config keys
_outputType = "xAOD::TauJetContainer"
_outputKey = "TauJets"
_outputAuxType = "xAOD::TauJetAuxContainer"
_outputAuxKey = "TauJetsAux."
_track_collection = "InDetTrackParticles"

################################################################################
## @class TauRecRunner
# Build proper tau candidates and associate tracks, vertex and cells
################################################################################
class TauRecRunner ( TauRecRunConfigured ) :
    """Build proper tau candidates and associate tracks, vertex and cells. 
    Calculate properties based on cell informations. 
    Find clusters used for Pi0 identification and eflow variables.
    PhotonConversion will be run here too.
    """
  
    _output     = { _outputType:_outputKey , _outputAuxType:_outputAuxKey }
    
    def __init__(self, name = "TauRecRunner",doPi0Clus=False, doTJVA=False):
        self.name = name
        self.doPi0Clus = doPi0Clus
        self.do_TJVA = doTJVA
        TauRecRunConfigured.__init__(self, name)


 
    def configure(self):
        mlog = logging.getLogger ('TauRecRunner.py::configure:')
        mlog.info('entering')
                
        from RecExConfig.RecFlags import rec    
        
        # xxx ToDo: still needed?        
        from RecExConfig.ObjKeyStore import objKeyStore
        objKeyStore.addManyTypesStreamESD(self._output)
        objKeyStore.addManyTypesStreamAOD(self._output)              
        
        import tauRec.TauAlgorithmsHolder as taualgs
        
        from tauRec.tauRecFlags import tauFlags
                
        
        tools = []
                        
        ### TauRecPi0EflowProcessor ###
        # Tools in this section were originally in TauRecPi0EflowProcessor
        if self.doPi0Clus: tools.append(taualgs.getPi0ClusterCreator())
            
        ###############################
        
        ### TauRecVariablesProcessor ###
        from InDetRecExample.InDetJobProperties import InDetFlags
        # Tools in this section were originally in TauRecVariablesProcessor
        if tauFlags.isStandalone() or InDetFlags.doVertexFinding():
            tools.append(taualgs.getTauVertexVariables())

        tools.append(taualgs.getTauCommonCalcVars())
        tools.append(taualgs.getTauSubstructure())

        if self.doPi0Clus:
            tools.append(taualgs.getPi0ClusterScaler())
            tools.append(taualgs.getPi0ScoreCalculator())
            # SWITCHED OFF SELECTOR< SINCE NO CHARGED PFOS AVAILABLE ATM
            tools.append(taualgs.getPi0Selector())

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
            tools.append(taualgs.getTauJetBDTEvaluator("TauJetBDT1P", weightsFile="vars2016_pt_gamma_1p_isofix.root", minNTracks=0, maxNTracks=1)) #update config?
            tools.append(taualgs.getTauJetBDTEvaluator("TauJetBDT3P", weightsFile="vars2016_pt_gamma_3p_isofix.root", minNTracks=2, maxNTracks=1000)) #update config?
            tools.append(taualgs.getTauWPDecoratorJetBDT())
            tools.append(taualgs.getTauJetRNNEvaluator("TauJetRNN",
                                                       NetworkFile1P="rnnid_mc16d_config_1p.json",
                                                       NetworkFile3P="rnnid_mc16d_config_3p.json",
                                                       OutputVarname="RNNJetScore", MaxTracks=10, MaxClusters=6))
            tools.append(taualgs.getTauWPDecoratorJetRNN())
            tools.append(taualgs.getTauJetBDTEvaluator("TauEleBDT_def", weightsFile="", outputVarName="BDTEleScore"))#just inits values
            tools.append(taualgs.getTauJetBDTEvaluator("TauEleBDT_bar", 
                                                       weightsFile="EleBDT1PBar.root", minNTracks=1, maxAbsTrackEta=1.37, 
                                                       outputVarName="BDTEleScore")) #update config?
            tools.append(taualgs.getTauJetBDTEvaluator("TauEleBDT_end1", 
                                                       weightsFile="EleBDT1PEnd1.root", minNTracks=1, minAbsTrackEta=1.37, 
                                                       maxAbsTrackEta=2.0, outputVarName="BDTEleScore")) #update config?
            tools.append(taualgs.getTauJetBDTEvaluator("TauEleBDT_end23", 
                                                       weightsFile="EleBDT1PEnd23.root", minNTracks=1, minAbsTrackEta=2.0, 
                                                       maxAbsTrackEta=3.0, outputVarName="BDTEleScore")) #update config?
            tools.append(taualgs.getTauWPDecoratorEleBDT())
            tools.append(taualgs.getTauEleOLRDecorator())              
            
            ################################

        from tauRec.tauRecFlags import tauFlags
        tools+=tauFlags.tauRecToolsDevToolList()
        #self.TauBuilderToolHandle().Tools = tools
            
                
        # run first part of Tau Builder
        TauRecRunConfigured.WrapTauRecToolExecHandle(self, tool=tools)
        return True
        

    def outputKey(self):
         return self._output[self._outputType]
    
    def outputType(self):
         return self._outputType


#end
