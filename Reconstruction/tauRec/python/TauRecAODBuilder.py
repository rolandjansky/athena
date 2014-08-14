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

################################################################################
## @class TauRecAODProcessor
# Calculate Tau variables and properties on AODs.
################################################################################
class TauRecAODProcessor ( Configured ) :
    """Calculate remaining Tau variables and properties. Use informations available also in AODs, so no cell level is needed."""
    
    _outputType = "Analysis::TauJetContainer"
    _outputKey = "TauRecContainer"
    _outputDetailsType = "Analysis::TauDetailsContainer"
    _outputDetailsKey = "TauRecDetailsContainer"
    
    def __init__(self, name = "TauProcessorAODTools", inAODmode=True, doBonnPi0Clus=False, msglevel=3, ignoreExistingDataObject=True, sequence = None):
        self.name = name
        self.doBonnPi0Clus = doBonnPi0Clus
        self.msglevel = msglevel
        self.AODmode = inAODmode
        self.sequence = sequence
        if sequence is None:
           self.sequence = AlgSequence() 
        Configured.__init__(self, ignoreExistingDataObject=ignoreExistingDataObject)
    
    
    def configure(self):
        mlog = logging.getLogger ('TauRecAODProcessor::configure:')
        mlog.info('entering')
        
        import tauRec.TauAlgorithmsHolder as taualgs
        
        ########################################################################
        # Tau Modifier Algos 
        ########################################################################
        try:
            from tauRec.tauRecConf import TauProcessor
            #TauProcessor.OutputLevel = 2
            self._TauProcessorHandle = TauProcessor(
                name = self.name,
                TauContainer             = self._outputKey,
                TauDetailsContainer      = self._outputDetailsKey,
                runOnAOD                 = self.AODmode)
        
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
            #tools.append(taualgs.getTauVertexFinder(doUseTJVA=True)) 
            tools.append(taualgs.getTauAxis()) ##needed to set correct variables for energy calibration
            #tools.append(taualgs.getTauTrackFinder())
            tools.append(taualgs.getEnergyCalibrationLC(correctEnergy=True, correctAxis=False, postfix='_onlyEnergy'))
            
            tools.append(taualgs.getTauVertexVariables())
            tools.append(taualgs.getTauCommonCalcVars())
            tools.append(taualgs.getTauSubstructure())
            tools.append(taualgs.getEnergyCalibrationLC(correctEnergy=False, correctAxis=True, postfix='_onlyAxis'))                    

            # Run the conversion tagger if flagged to do so
            import tauRec.TauConversionAlgorithms
            from tauRec.tauRecFlags import jobproperties
            if jobproperties.tauRecFlags.useNewPIDBasedConvFinder():
                tools.append(tauRec.TauConversionAlgorithms.getTauConversionTaggerTool())

            # for testing purpose
            #tools.append(taualgs.getTauTestDump())
                        
            self.TauProcessorHandle().Tools = tools
        
        except Exception:
            mlog.error("could not append tools to TauProcessor")
            print traceback.format_exc()
            return False
        
        self.sequence += self.TauProcessorHandle()  
        
        return True    
    
    #############################################################################################
    # Helpers
    #############################################################################################
    
    def TauProcessorHandle(self):
        return self._TauProcessorHandle

