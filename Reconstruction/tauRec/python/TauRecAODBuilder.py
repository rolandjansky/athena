# Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration

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


from AthenaCommon.Logging import logging
import traceback
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
    
    def __init__(self, name = "TauProcessorAODTools"):
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
                TauContainer                 = self._outputKey,
                TauAuxContainer              = self._outputDetailsKey,
                deepCopyChargedPFOContainer  = False,
                deepCopyHadronicPFOContainer = False,
                deepCopyNeutralPFOContainer  = False,
                runOnAOD                     = True,
                )
        
        except Exception:
            mlog.error("could not get handle to TauProcessor")
            traceback.print_exc()
            return False
        
        tools = []
        try:
            taualgs.setAODmode(True)
            ## ATTENTION ##################################################################################
            # running these tau tools on AODs will lead to inconsistency with standard tau reconstruction
            ###############################################################################################
            tools.append(taualgs.getMvaTESVariableDecorator())
            tools.append(taualgs.getMvaTESEvaluator())
            tools.append(taualgs.getTauCombinedTES())
            tools.append(taualgs.getIDPileUpCorrection())
            
            # TauDiscriminant:
            from tauRec.tauRecFlags import tauFlags
            if tauFlags.doRunTauDiscriminant() :
                import TauDiscriminant.TauDiscriGetter as tauDisc
                tauDiscTools=tauDisc.getTauDiscriminantTools(mlog)
                if len(tauDiscTools)==0:
                    mlog.error("No TauDiscriminantTools appended")
                    traceback.print_stack()
                    return False
                tools+=tauDiscTools
                pass
            
            tools.append(taualgs.getTauWPDecorator())
            tools.append(taualgs.getTauWPDecoratorEleBDT())

            TauRecConfigured.AddToolsToToolSvc(self, tools)
            self.TauProcessorHandle().Tools = tools
        
        except Exception:
            mlog.error("could not append tools to TauProcessor")
            traceback.print_exc()
            return False
        
        TauRecConfigured.WrapTauRecToolExecHandle(self, tool=self.TauProcessorHandle())        
        return True    
    
    #############################################################################################
    # Helpers
    #############################################################################################
    
    def TauProcessorHandle(self):
        return self._TauProcessorHandle


class TauRecAODPi0Processor ( TauRecConfigured ) :
    """Calculate remaining Tau variables and properties. Use informations available also in AODs, so no cell level is needed."""
    
    _outputType = "xAOD::TauJetContainer"
    _outputKey = "TauJets"
    _outputDetailsType = "xAOD::TauJetAuxContainer"
    _outputDetailsKey = "TauJetsAux."
    
    def __init__(self, name = "TauProcessorAODPi0Tools"):
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
                TauContainer                 = self._outputKey,
                TauAuxContainer              = self._outputDetailsKey,
                deepCopyChargedPFOContainer  = True, 
                deepCopyHadronicPFOContainer = True,
                deepCopyNeutralPFOContainer  = True,
                runOnAOD                     = True,
                )
        
        except Exception:
            mlog.error("could not get handle to TauProcessor")
            traceback.print_exc()
            return False
        
        
        tools = []
        try:
            taualgs.setAODmode(True)
            ## ATTENTION ##################################################################################
            # running these tau tools on AODs will lead to inconsistency with standard tau reconstruction
            ###############################################################################################
            tools.append(taualgs.getPi0ClusterScaler())
            tools.append(taualgs.getPi0ScoreCalculator())
            tools.append(taualgs.getPi0Selector())
            
            TauRecConfigured.AddToolsToToolSvc(self, tools)
            self.TauProcessorHandle().Tools = tools
        
        except Exception:
            mlog.error("could not append tools to TauProcessor")
            traceback.print_exc()
            return False
        
        TauRecConfigured.WrapTauRecToolExecHandle(self, tool=self.TauProcessorHandle())        
        return True    
    
    #############################################################################################
    # Helpers
    #############################################################################################
    
    def TauProcessorHandle(self):
        return self._TauProcessorHandle



