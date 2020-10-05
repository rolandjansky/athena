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
from .TauRecConfigured import TauRecConfigured

# global tauRec config keys - to be replaced with tauRecFlags
_outputType = "xAOD::TauJetContainer"
_outputKey = "TauJets"
_outputAuxType = "xAOD::TauJetAuxContainer"
_outputAuxKey = "TauJetsAux."

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
        TauRecConfigured.__init__(self, name, doPi0Clus)


 
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
        if not tauFlags.isStandalone() and not jetFlags.useTracks():
            self.do_TJVA = False  # switch off TJVA
        
        tools = []
        try:
            tools.append(taualgs.getJetSeedBuilder())

            # run vertex finder only in case vertexing is available. This check can also be done in TauAlgorithmsHolder instead doing it here. 
            from InDetRecExample.InDetJobProperties import InDetFlags
            from tauRec.tauRecFlags import jobproperties
            doMVATrackClassification = jobproperties.tauRecFlags.tauRecMVATrackClassification()
            doRNNTrackClassification = jobproperties.tauRecFlags.tauRecRNNTrackClassification()

            if tauFlags.isStandalone() or InDetFlags.doVertexFinding():
                tools.append(taualgs.getTauVertexFinder(doUseTJVA=self.do_TJVA))
            tools.append(taualgs.getTauAxis())
            tools.append(taualgs.getTauTrackFinder(removeDuplicateTracks=(not doMVATrackClassification) ))
            if doMVATrackClassification : tools.append(taualgs.getTauTrackClassifier())
            if not doMVATrackClassification and doRNNTrackClassification:
                tools.append(taualgs.getTauTrackRNNClassifier())
            if jobproperties.Beam.beamType()!="cosmics":
                tools.append(taualgs.getEnergyCalibrationLC(correctEnergy=True, correctAxis=False, postfix='_onlyEnergy'))
            
            tools.append(taualgs.getCellVariables())
            tools.append(taualgs.getElectronVetoVars())
            tools.append(taualgs.getTauShotFinder()) 

            if self.doPi0Clus:
                tools.append(taualgs.getPi0ClusterFinder())
            
            from tauRec.tauRecFlags import tauFlags
            tools+=tauFlags.tauRecToolsDevToolList()
                        
        except Exception:
            mlog.error("could not append tools to TauBuilder")
            traceback.print_exc()
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

#end
