# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

################################################################################
##
#@file TauRecAODBuilder.py
#
#@brief Main steering file to rerun parts of tau reconstruction on AODs. 
#
# Only algorithms which don't need cell level informations can be used.
# This is an example of how to run tauRec on AODs and should later be called by AODCalib, AODFix or TauD3PDMaker.
# Copied from TauRecBuilder.py/TauRecBuilder::TauRecVariablesProcessor.
#
# Algorithms schedule here are independent from the standard tauRec chain.
#
#@author Felix Friedrich
#
################################################################################


import os, sys, string

from AthenaCommon.Logging import logging
from AthenaCommon.SystemOfUnits import *
from AthenaCommon.Constants import *
from AthenaCommon.AlgSequence import AlgSequence
import traceback

from RecExConfig.Configured import Configured
from TauRecConfigured import TauRecConfigured

################################################################################
## @class TauRecAODProcessor
# Calculate Tau variables and properties on AODs.
################################################################################
class TauRecAODProcessor ( TauRecConfigured ) :
    """Calculate remaining Tau variables and properties. Use informations available also in AODs, so no cell level is needed."""
    
    _outputType = "xAOD::TauJetContainer"
    _outputKey = "TauJets"
    _outputDetailsType = "xAOD::TauJetAuxContainer"
    _outputDetailsKey = "TauJetsAux."
    
    def __init__(self, name = "TauProcessorAODTools", inAODmode=True, doPi0Clus=False, msglevel=3, ignoreExistingDataObject=True, sequence = None):
        self.name = name
        self.doPi0Clus = doPi0Clus
        self.msglevel = msglevel
        self.AODmode = inAODmode
        self.sequence = sequence #not used at the moment
        if sequence is None:
           self.sequence = AlgSequence() 
        TauRecConfigured.__init__(self, name)
    
    
    def configure(self):
        mlog = logging.getLogger ('TauRecAODProcessor::configure:')
        mlog.info('entering')
        
        import tauRec.TauAlgorithmsHolder as taualgs
        
        ########################################################################
        # Tau Modifier Algos 
        ########################################################################
        try:
            from tauRecTools.tauRecToolsConf import TauProcessorTool
            #TauProcessor.OutputLevel = 2
            self._TauProcessorHandle = TauProcessorTool(
                name = self.name,
                TauContainer             = self._outputKey,
                #TauDetailsContainer      = self._outputDetailsKey,
                TauAuxContainer          = self._outputDetailsKey,
                runOnAOD                 = self.AODmode,
                )
        
        except Exception:
            mlog.error("could not get handle to TauProcessor")
            print traceback.format_exc()
            return False
        
        
        tools = []
        try:
            taualgs.setAODmode(True)
            ## ATTENTION ##################################################################################
            # running these tau tools on AODs will lead to inconsistency with standard tau reconstruction
            ###############################################################################################
            try :
                if InDetFlags.doVertexFinding():
                    tools.append(taualgs.getTauVertexFinder(doUseTJVA=True)) 
                    pass                
            except : pass
            tools.append(taualgs.getTauAxis()) ##needed to set correct variables for energy calibration
            #tools.append(taualgs.getTauTrackFinder())
            tools.append(taualgs.getEnergyCalibrationLC(correctEnergy=True, correctAxis=False, postfix='_onlyEnergy'))
            
            tools.append(taualgs.getTauTrackFilter())     #TauTrackFilter
            tools.append(taualgs.getTauGenericPi0Cone())  #TauGenericPi0Cone

            # Run the conversion tagger if flagged to do so
            import tauRec.TauConversionAlgorithms
            from tauRec.tauRecFlags import jobproperties
            if jobproperties.tauRecFlags.useNewPIDBasedConvFinder():
                tools.append(tauRec.TauConversionAlgorithms.getTauConversionTaggerTool())
            else:
                #Need to run together, they will select either PID or vertex based on another flag
                tools.append(tauRec.TauConversionAlgorithms.getPhotonConversionTool())
                tools.append(tauRec.TauConversionAlgorithms.getTauConversionFinderTool())
                pass


            #this tool cannot recreate a 2nd vertex w/o ESD style tracks
            #tools.append(taualgs.getTauVertexVariables())
            tools.append(taualgs.getTauCommonCalcVars())
            tools.append(taualgs.getTauSubstructure())

            tools.append(taualgs.getPi0ClusterScaler())   #TauPi0ClusterScaler
            tools.append(taualgs.getPi0ScoreCalculator()) #TauPi0ScoreCalculator
            tools.append(taualgs.getPi0Selector())        #TauPi0Selector

            tools.append(taualgs.getEnergyCalibrationLC(correctEnergy=False, correctAxis=True, postfix='_onlyAxis'))                    

            tools.append(taualgs.getIDPileUpCorrection())

            tools.append(taualgs.getMvaTESVariableDecorator())
            tools.append(taualgs.getMvaTESEvaluator())
                
            # for testing purpose
            #tools.append(taualgs.getTauTestDump())
                        
            TauRecConfigured.AddToolsToToolSvc(self, tools)
            self.TauProcessorHandle().Tools = tools
        
        except Exception:
            mlog.error("could not append tools to TauProcessor")
            print traceback.format_exc()
            return False
        
        TauRecConfigured.WrapTauRecToolExecHandle(self, tool=self.TauProcessorHandle())        
        return True    
    
    #############################################################################################
    # Helpers
    #############################################################################################
    
    def TauProcessorHandle(self):
        return self._TauProcessorHandle

