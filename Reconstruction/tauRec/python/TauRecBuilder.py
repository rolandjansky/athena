# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

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
class TauRecCoreBuilder ( Configured ) :
    """Build proper tau candidates and associate tracks, vertex and cells. 
    Calculate properties based on cell informations. 
    Find clusters used for Pi0 identification and eflow variables.
    PhotonConversion will be run here too.
    """
  
    _output     = { _outputType:_outputKey , _outputAuxType:_outputAuxKey }
    
    def __init__(self, name = "TauCoreBuilder",doPi0Clus=False, doTJVA=False, msglevel=3, ignoreExistingDataObject=True):
        self.name = name
        self.doPi0Clus = doPi0Clus
        self.do_TJVA = doTJVA
        self.msglevel = msglevel
        Configured.__init__(self, ignoreExistingDataObject=ignoreExistingDataObject)

 
    def configure(self):
        mlog = logging.getLogger ('TauCoreBuilder.py::configure:')
        mlog.info('entering')
        
        
        from RecExConfig.RecFlags import rec    
        
        # xxx ToDo: still needed?        
        from RecExConfig.ObjKeyStore import objKeyStore
        objKeyStore.addManyTypesStreamESD(self._output)
        objKeyStore.addManyTypesStreamAOD(self._output)              
        
        from AthenaCommon.AlgSequence import AlgSequence
        topSequence = AlgSequence()
        
        import tauRec.TauAlgorithmsHolder as taualgs
        
        ########################################################################
        # TauBuilder
        # create the taus
        try:
            from tauRec.tauRecConf import TauBuilder 
            self._TauBuilderHandle = TauBuilder(
                name = self.name,
                SeedContainer            = _jet_collection,
                TauContainer             = _outputKey,
                TauAuxContainer          = _outputAuxKey,
                MaxEta = 2.5,
                MinPt = 10.*GeV,
                doCreateTauContainers = True)
        except Exception:
            mlog.error("could not get handle to TauBuilder")
            print traceback.format_exc()
            return False


        #switch off TJVA if jet reco don't use tracks.
        from JetRec.JetRecFlags import jetFlags
        if not jetFlags.useTracks():
            self.do_TJVA = False  # switch off TJVA
        
        
        tools = []
        try:
            tools.append(taualgs.getJetSeedBuilder(_jet_collection))

            # run vertex finder only in case vertexing is available. This check can also be done in TauAlgorithmsHolder instead doing it here. 
            from InDetRecExample.InDetJobProperties import InDetFlags
            if InDetFlags.doVertexFinding():
                tools.append(taualgs.getTauVertexFinder(doUseTJVA=self.do_TJVA)) 
            tools.append(taualgs.getTauAxis())
            tools.append(taualgs.getTauTrackFinder())
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
            else:
                #Need to run together, they will select either PID or vertex based on another flag
                tools.append(tauRec.TauConversionAlgorithms.getPhotonConversionTool())
                tools.append(tauRec.TauConversionAlgorithms.getTauConversionFinderTool())
            
            #tools.append(taualgs.getContainerLock())
            self.TauBuilderHandle().Tools = tools
            
        except Exception:
            mlog.error("could not append tools to TauBuilder")
            print traceback.format_exc()
            return False
        
        # run first part of Tau Builder
        topSequence += self.TauBuilderHandle()
        
        return True
        
    # Helpers 
    def TauBuilderHandle(self):
        return self._TauBuilderHandle
    
    def outputKey(self):
         return self._output[self._outputType]
    
    def outputType(self):
         return self._outputType



################################################################################
## @class TauRecPi0EflowProcessor
# Calculate eflow information and run the Pi0 finder algorithms
################################################################################
class TauRecPi0EflowProcessor ( Configured ) :
    """Calculate eflow information and run the Pi0 finder algorithms.
    This needs to be done in a separate step, because first special cluster and cell container have to be build.
    """
       
    def __init__(self, name = "TauProcessorPi0EflowTools",doPi0Clus=False, msglevel=3, ignoreExistingDataObject=True):
        self.name = name
        self.doPi0Clus = doPi0Clus
        self.msglevel = msglevel
        Configured.__init__(self,ignoreExistingDataObject=ignoreExistingDataObject)
    
    def configure(self):
        mlog = logging.getLogger ('TauRecPi0EflowProcessor::configure:')
        mlog.info('entering')
        
        
        from AthenaCommon.AlgSequence import AlgSequence
        topSequence = AlgSequence()
        
        import tauRec.TauAlgorithmsHolder as taualgs
        
        ########################################################################
        # Tau Modifier Algos 
        ########################################################################
        try:
            from tauRec.tauRecConf import TauProcessor
            self._TauProcessorHandle = TauProcessor(
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
            
            self.TauProcessorHandle().Tools = tools
            
        except Exception:
            mlog.error("could not append tools to TauProcessor")
            print traceback.format_exc()
            return False   
        
        topSequence += self.TauProcessorHandle()  
        
        return True    
    
    # Helpers
    def TauProcessorHandle(self):
        return self._TauProcessorHandle


################################################################################
## @class TauRecVariablesProcessor
# Calculate remaining Tau variables and properties
################################################################################
class TauRecVariablesProcessor ( Configured ) :
    """Calculate remaining Tau variables and properties. 
    Use informations available also in AODs, so no cell level is needed.
    """
    
    def __init__(self, name = "TauRecVariablesProcessor", inAODmode=False, doPi0Clus=False, msglevel=3, ignoreExistingDataObject=True):
        self.name = name
        self.doPi0Clus = doPi0Clus
        self.msglevel = msglevel
        self.AODmode = inAODmode 
        Configured.__init__(self, ignoreExistingDataObject=ignoreExistingDataObject)
    
    
    def configure(self):
        mlog = logging.getLogger ('TauRecVariablesProcessor::configure:')
        mlog.info('entering')
        
        
        from AthenaCommon.AlgSequence import AlgSequence
        topSequence = AlgSequence()
        
        import tauRec.TauAlgorithmsHolder as taualgs
        
        ########################################################################
        # Tau Modifier Algos 
        ########################################################################
        try:
            from tauRec.tauRecConf import TauProcessor
            self._TauProcessorHandle = TauProcessor(
                name = self.name,
                TauContainer             = _outputKey,
                TauAuxContainer          = _outputAuxKey,
                runOnAOD                 = self.AODmode)
        
        except Exception:
            mlog.error("could not get handle to TauProcessor")
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
            tools.append(taualgs.getIDPileUpCorrection())
            #
            ## for testing purpose
            #tools.append(taualgs.getTauTestDump())
            #
            ## lock tau containers -> must be the last tau tool!!
            #tools.append(taualgs.getContainerLock())
                        
            self.TauProcessorHandle().Tools = tools
        
        except Exception:
            mlog.error("could not append tools to TauProcessor")
            print traceback.format_exc()
            return False
        
        topSequence += self.TauProcessorHandle()  
        
        return True    
    
    # Helpers
    def TauProcessorHandle(self):
        return self._TauProcessorHandle

#end
