# Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration

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
from TauRecConfigured import TauRecConfigured

# global tauRec config keys
_outputType = "xAOD::TauJetContainer"
_outputKey = "TauJets"
_outputAuxType = "xAOD::TauJetAuxContainer"
_outputAuxKey = "TauJetsAux."
_track_collection = "InDetTrackParticles"
_jet_collection = "AntiKt4LCTopoJets"

################################################################################
## @class TauRecCoreBuilder
# Build proper tau candidates and associate tracks, vertex and cells
################################################################################
class TauRecCoreBuilder ( TauRecConfigured ) :
    """Build proper tau candidates and associate tracks, vertex and cells. 
    Calculate properties based on cell informations. 
    Find clusters used for Pi0 identification and eflow variables.
    PhotonConversion will be run here too.
    """
  
    _output     = { _outputType:_outputKey , _outputAuxType:_outputAuxKey,
                    'xAOD::TauTrackContainer' : 'TauTracks',
                    'xAOD::CaloClusterContainer' : 'TauShotClusters',
                    'xAOD::PFOContainer' : 'TauShotParticleFlowObjects',
                    'CaloCellContainer' : 'TauCommonPi0Cells',
                    }
    
    def __init__(self, name = "TauCoreBuilder",doPi0Clus=False, doTJVA=False):
        self.name = name
        self.doPi0Clus = doPi0Clus
        self.do_TJVA = doTJVA
        TauRecConfigured.__init__(self, name)


 
    def configure(self):
        mlog = logging.getLogger ('TauCoreBuilder.py::configure:')
        mlog.info('entering')
        
        
        from RecExConfig.RecFlags import rec    
        
        from RecExConfig.ObjKeyStore import objKeyStore
        objKeyStore.addManyTypesStreamESD(self._output)
        objKeyStore.addManyTypesStreamAOD(self._output)              
        objKeyStore.addManyTypesTransient(self._output)              
        
        import tauRec.TauAlgorithmsHolder as taualgs
        from tauRec.tauRecFlags import tauFlags
        
        #switch off TJVA if jet reco don't use tracks.
        from JetRec.JetRecFlags import jetFlags
        if not jetFlags.useTracks():
            self.do_TJVA = False  # switch off TJVA
        
        tools = []
        try:
            tools.append(taualgs.getJetSeedBuilder(_jet_collection))

            # run vertex finder only in case vertexing is available. This check can also be done in TauAlgorithmsHolder instead doing it here. 
            from InDetRecExample.InDetJobProperties import InDetFlags
            from tauRec.tauRecFlags import jobproperties
            doMVATrackClassification = jobproperties.tauRecFlags.tauRecMVATrackClassification()

            if InDetFlags.doVertexFinding():
                tools.append(taualgs.getTauVertexFinder(doUseTJVA=self.do_TJVA))
            tools.append(taualgs.getTauAxis())
            tools.append(taualgs.getTauTrackFinder(removeDuplicateTracks=(not doMVATrackClassification) ))
            if doMVATrackClassification : tools.append(taualgs.getTauTrackClassifier())
            if jobproperties.Beam.beamType()!="cosmics":
                tools.append(taualgs.getEnergyCalibrationLC(correctEnergy=True, correctAxis=False, postfix='_onlyEnergy'))
            tools.append(taualgs.getCellVariables())
            tools.append(taualgs.getElectronVetoVars())
            #
            tools.append(taualgs.getTauTrackFilter())
            tools.append(taualgs.getTauGenericPi0Cone())
            #
            #tools.append(taualgs.getPi0EflowCreateROI())
            tools.append(taualgs.getTauShotFinder()) 
            if self.doPi0Clus:
                tools.append(taualgs.getPi0ClusterFinder())
            
            #####################################################################
            ## Tau Conversation Finder (found no one talking here...)
            ## TODO: talk with KG about the status of the new PhotonConversionFinder 
            ## new PhotonConversionFinder is currently disabled (time consumption!)
            ## old one is still in use
            import tauRec.TauConversionAlgorithms
            from tauRec.tauRecFlags import jobproperties
            if jobproperties.tauRecFlags.useNewPIDBasedConvFinder():
                #Needs to run alone
                tools.append(tauRec.TauConversionAlgorithms.getTauConversionTaggerTool())
            # are these even used? Probably not any more
            # else:
                #Need to run together, they will select either PID or vertex based on another flag
                #tools.append(tauRec.TauConversionAlgorithms.getPhotonConversionTool())
                #tools.append(tauRec.TauConversionAlgorithms.getTauConversionFinderTool())

            # tools.append(taualgs.getContainerLock())

            ################################

            from tauRec.tauRecFlags import tauFlags
            tools+=tauFlags.tauRecToolsDevToolList()
                        
        except Exception:
            mlog.error("could not append tools to TauBuilder")
            print traceback.format_exc()
            return False
        
        # run first part of Tau Builder
        TauRecConfigured.WrapTauRecToolExecHandle(self, tool=tools)
        return True
        
    # Helpers 
    def TauBuilderToolHandle(self):
        return self._TauBuilderToolHandle

    def outputKey(self):
         return self._output[self._outputType]
    
    def outputType(self):
         return self._outputType


# No longer used
################################################################################
## @class TauRecPi0EflowProcessor
# Calculate eflow information and run the Pi0 finder algorithms
################################################################################
class TauRecPi0EflowProcessor ( TauRecConfigured ) :
    """Calculate eflow information and run the Pi0 finder algorithms.
    This needs to be done in a separate step, because first special cluster and cell container have to be build.
    """
       
    def __init__(self, name = "TauProcessorPi0EflowTools",doPi0Clus=False):
        self.name = name
        self.doPi0Clus = doPi0Clus
        TauRecConfigured.__init__(self, name)
    
    def configure(self):
        mlog = logging.getLogger ('TauRecPi0EflowProcessor::configure:')
        mlog.info('entering')
        
        
        import tauRec.TauAlgorithmsHolder as taualgs
        
        ########################################################################
        # Tau Modifier Algos 
        ########################################################################
        try:
            from tauRecTools.tauRecToolsConf import TauProcessorTool
            self._TauProcessorToolHandle = TauProcessorTool(
                name = self.name,
                TauContainer             = _outputKey,
                TauAuxContainer          = _outputAuxKey,
            )
        except Exception:
            mlog.error("could not get handle to TauProcessor")
            print traceback.format_exc()
            return False
             
        tools = []
        try:
            #tools.append(taualgs.getTauEflowTrackMatchCells())
            #tools.append(taualgs.getTauEflowAddCaloInfo())  
            #tools.append(taualgs.getTauEflowVariables())     
      
            if self.doPi0Clus: tools.append(taualgs.getPi0ClusterCreator())
            
            TauRecConfigured.AddToolsToToolSvc(self, tools)
            self.TauProcessorToolHandle().Tools = tools
            
        except Exception:
            mlog.error("could not append tools to TauProcessor")
            print traceback.format_exc()
            return False   
        
        TauRecConfigured.WrapTauRecToolExecHandle(self, self.TauProcessorToolHandle())
        
        return True    
    
    # Helpers
    def TauProcessorToolHandle(self):
        return self._TauProcessorToolHandle

# No longer used
################################################################################
## @class TauRecVariablesProcessor
# Calculate remaining Tau variables and properties
################################################################################
class TauRecVariablesProcessor ( TauRecConfigured ) :
    """Calculate remaining Tau variables and properties. 
    Use informations available also in AODs, so no cell level is needed.
    """
    
    def __init__(self, name = "TauRecVariablesProcessor", inAODmode=False, doPi0Clus=False):
        self.name = name
        self.doPi0Clus = doPi0Clus
        self.AODmode = inAODmode 
        TauRecConfigured.__init__(self, name)
    
    
    def configure(self):
        mlog = logging.getLogger ('TauRecVariablesProcessor::configure:')
        mlog.info('entering')
        
        
        import tauRec.TauAlgorithmsHolder as taualgs
        
        ########################################################################
        # Tau Modifier Algos 
        ########################################################################
        try:
            from tauRecTools.tauRecToolsConf import TauProcessorTool
            self._TauProcessorToolHandle = TauProcessorTool(
                name = self.name,
                TauContainer             = _outputKey,
                TauAuxContainer          = _outputAuxKey,
                runOnAOD                 = self.AODmode)
        
        except Exception:
            mlog.error("could not get handle to TauProcessorTool")
            print traceback.format_exc()
            return False
             
        tools = []
        try:
            #tools.append(taualgs.getEnergyCalibrationEM())
            # run vertex finder only in case vertexing is available. This check can also be done in TauAlgorithmsHolder instead doing it here. 
            from InDetRecExample.InDetJobProperties import InDetFlags
            if InDetFlags.doVertexFinding():
                tools.append(taualgs.getTauVertexVariables())
            tools.append(taualgs.getTauCommonCalcVars())
            tools.append(taualgs.getTauSubstructure())
            if self.doPi0Clus: 
                tools.append(taualgs.getPi0ClusterScaler())
                tools.append(taualgs.getPi0ScoreCalculator())
                # SWITCHED OFF SELECTOR< SINCE NO CHARGED PFOS AVAILABLE ATM
                tools.append(taualgs.getPi0Selector())
            tools.append(taualgs.getEnergyCalibrationLC(correctEnergy=False, correctAxis=True, postfix='_onlyAxis'))
            #
            ## for testing purpose
            #tools.append(taualgs.getTauTestDump())
            #

            from tauRec.tauRecFlags import tauFlags

            # PanTau:
            if tauFlags.doPanTau() :
                import PanTauAlgs.JobOptions_Main_PanTau as pantau
                tools.append(pantau.getPanTau())
            
            #these tools need pantau info
            tools.append(taualgs.getCombinedP4FromRecoTaus())
            if jobproperties.Beam.beamType()!="cosmics":
                tools.append(taualgs.getMvaTESVariableDecorator())
            tools.append(taualgs.getMvaTESEvaluator())

            if tauFlags.doRunTauDiscriminant():
                tools.append(taualgs.getTauIDVarCalculator())
                tools.append(taualgs.getTauJetBDTEvaluator("TauJetBDT1P", weightsFile="vars2016_pt_gamma_1p_isofix.root", minNTracks=0, maxNTracks=1)) #update config?
                tools.append(taualgs.getTauJetBDTEvaluator("TauJetBDT3P", weightsFile="vars2016_pt_gamma_3p_isofix.root", minNTracks=2, maxNTracks=1000)) #update config?
                tools.append(taualgs.getTauWPDecoratorJetBDT())
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
                pass

            tools+=tauFlags.tauRecToolsDevToolListProcessor()
            ## lock tau containers -> must be the last tau tool!!
            #tools.append(taualgs.getContainerLock())
            TauRecConfigured.AddToolsToToolSvc(self, tools)
            self.TauProcessorToolHandle().Tools = tools
        
        except Exception:
            mlog.error("could not append tools to TauProcessor")
            print traceback.format_exc()
            return False

        self

        TauRecConfigured.WrapTauRecToolExecHandle(self, self.TauProcessorToolHandle())
        
        
        return True    
    

    #helpers
    def TauProcessorToolHandle(self):
        return self._TauProcessorToolHandle

#end
