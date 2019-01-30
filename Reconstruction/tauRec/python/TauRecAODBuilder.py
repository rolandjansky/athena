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
from AthenaCommon.Resilience import treatException
from AthenaCommon.Include import include
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
            print traceback.format_exc()
            return False
        
        tools = []
        try:
            taualgs.setAODmode(True)
            ## ATTENTION ##################################################################################
            # running these tau tools on AODs will lead to inconsistency with standard tau reconstruction
            ###############################################################################################
            tools.append(taualgs.getMvaTESVariableDecorator())
            tools.append(taualgs.getMvaTESEvaluator())
            tools.append(taualgs.getCombinedP4FromRecoTaus())
            tools.append(taualgs.getIDPileUpCorrection())
            
            # TauDiscriminant:
            from tauRec.tauRecFlags import tauFlags
            if tauFlags.doRunTauDiscriminant() :
                import TauDiscriminant.TauDiscriGetter as tauDisc
                tauDiscTools=tauDisc.getTauDiscriminantTools(mlog)
                if len(tauDiscTools)==0:
                    try: import DOESNOTEXIST
                    except Exception:
                        mlog.error("No TauDiscriminantTools appended")
                        print traceback.format_exc()
                        return False
                    pass                
                tools+=tauDiscTools
                pass
            
            tools.append(taualgs.getTauWPDecorator())
            tools.append(taualgs.getTauWPDecoratorEleBDT())

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
            print traceback.format_exc()
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
            print traceback.format_exc()
            return False
        
        TauRecConfigured.WrapTauRecToolExecHandle(self, tool=self.TauProcessorHandle())        
        return True    
    
    #############################################################################################
    # Helpers
    #############################################################################################
    
    def TauProcessorHandle(self):
        return self._TauProcessorHandle


class TauRecAODProcessor_RNN_ID ( TauRecConfigured ) :
    """Recalculate the RNN-based Tau-ID using information only available on AOD.
    Sets the RNNJetScore, RNNJetScoreSigTrans, and all four working points."""
    _outputType = "xAOD::TauJetContainer"
    _outputKey = "TauJets"
    _outputDetailsType = "xAOD::TauJetAuxContainer"
    _outputDetailsKey = "TauJetsAux."

    def __init__(self, name = "TauProcessorAODTools_RNN_ID"):
        TauRecConfigured.__init__(self, name)

    def configure(self):
        mlog = logging.getLogger ('TauRecAODProcessor_RNN_ID::configure:')
        mlog.info('entering')

        import tauRec.TauAlgorithmsHolder as taualgs
        ########################################################################
        # Tau Modifier Algos
        ########################################################################
        try:
            from tauRecTools.tauRecToolsConf import TauProcessorTool
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
            print traceback.format_exc()
            return False

        tools = []
        try:
            taualgs.setAODmode(True)

            # Calculate the RNN scores
            tools.append(taualgs.getTauJetRNNEvaluator(
                suffix="TauJetRNN",
                NetworkFile1P="rnnid_mc16d_config_1p.json",
                NetworkFile3P="rnnid_mc16d_config_3p.json",
                OutputVarname="RNNJetScore", MaxTracks=10, MaxClusters=6))

            # Decorate working points
            tools.append(taualgs.getTauWPDecoratorJetRNN(
                flatteningFile1Prong="rnnid_mc16d_flat_1p.root",
                flatteningFile3Prong="rnnid_mc16d_flat_3p.root"
            ))

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

class TauRecAODProcessor_FTau ( TauRecConfigured ) :
    """Add variables to the AODs based on Fake Tau Task Force needs.
    It means to run in Derivation Framework and THOR."""
    _outputType = "xAOD::TauJetContainer"
    _outputKey = "TauJets"
    _outputDetailsType = "xAOD::TauJetAuxContainer"
    _outputDetailsKey = "TauJetsAux."

    def __init__(self, name = "TauProcessorAODTools_FTau"):
        TauRecConfigured.__init__(self, name)

    def configure(self):
        mlog = logging.getLogger ('TauRecAODProcessor_FTau::configure:')
        mlog.info('entering')

        import tauRec.TauAlgorithmsHolder as taualgs
        ########################################################################
        # Tau Modifier Algos
        ########################################################################
        try:
            from tauRecTools.tauRecToolsConf import TauProcessorTool
            self._TauProcessorHandle = TauProcessorTool(
                name = self.name,
                TauContainer                 = self._outputKey,
                TauAuxContainer              = self._outputDetailsKey,
                deepCopyTauJetContainer      = True,
                deepCopyChargedPFOContainer  = False,
                deepCopyTauShotPFOContainer  = False,
                deepCopyHadronicPFOContainer = False,
                deepCopyNeutralPFOContainer  = False,
                deepCopyTauTrackContainer    = False,
                runOnAOD                     = True,
                )
            print self._TauProcessorHandle

        except Exception:
            mlog.error("could not get handle to TauProcessor")
            print traceback.format_exc()
            return False

        tools = []
        try:
            taualgs.setAODmode(True)

            # Calculate the RNN scores
            tools.append(taualgs.getFTauDecorator())

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

class TauRecAODProcessor_EleBDTFix ( TauRecConfigured ) :
    """Recalculate the EleBDT scores with BDT retrained on proper mc16 samples.
    Sets the BDTEleScore, BDTEleScoreSigTrans, and all three working points."""
    _outputType = "xAOD::TauJetContainer"
    _outputKey = "TauJets"
    _outputDetailsType = "xAOD::TauJetAuxContainer"
    _outputDetailsKey = "TauJetsAux."

    def __init__(self, name = "TauRecAODProcessor_EleBDTFix"):
        TauRecConfigured.__init__(self, name)

    def configure(self):
        mlog = logging.getLogger ('TauRecAODProcessor_EleBDTFix::configure:')
        mlog.info('entering')

        import tauRec.TauAlgorithmsHolder as taualgs
        ########################################################################
        # Tau Modifier Algos
        ########################################################################
        try:
            from tauRecTools.tauRecToolsConf import TauProcessorTool
            self._TauProcessorHandle = TauProcessorTool(
                name = self.name,
                TauContainer                 = self._outputKey,
                TauAuxContainer              = self._outputDetailsKey,
                deepCopyChargedPFOContainer  = False,
                deepCopyHadronicPFOContainer = False,
                deepCopyNeutralPFOContainer  = False,
                deepCopyTauTrackContainer    = False,
                runOnAOD                     = True,
                )

        except Exception:
            mlog.error("could not get handle to TauProcessor")
            print traceback.format_exc()
            return False

        tools = []
        try:
            taualgs.setAODmode(True)

            # Calculate the EleBDT scores
            tools.append(taualgs.getTauJetBDTEvaluator(suffix="TauEleBDT_def", weightsFile="", outputVarName="BDTEleScore_retuned"))
            tools.append(taualgs.getTauJetBDTEvaluator(suffix="TauEleBDT_bar", 
                                                       weightsFile="EleBDTBar_fix20190108.root", minNTracks=1, maxAbsTrackEta=1.37, 
                                                       outputVarName="BDTEleScore_retuned"))
            tools.append(taualgs.getTauJetBDTEvaluator(suffix="TauEleBDT_end1", 
                                                       weightsFile="EleBDTEnd1_fix20190108.root", minNTracks=1, minAbsTrackEta=1.37, 
                                                       maxAbsTrackEta=2.0, outputVarName="BDTEleScore_retuned"))
            tools.append(taualgs.getTauJetBDTEvaluator(suffix="TauEleBDT_end23", 
                                                       weightsFile="EleBDTEnd23_fix20190108.root", minNTracks=1, minAbsTrackEta=2.0, 
                                                       maxAbsTrackEta=3.0, outputVarName="BDTEleScore_retuned"))

            # Decorate working points
            tools.append(taualgs.getTauWPDecoratorEleBDT("EleBDTFlat_fix20190108.root",
                                                         "EleBDTFlat_fix20190108.root",
                                                         retuned=True))

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

