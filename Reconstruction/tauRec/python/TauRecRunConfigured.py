# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

################################################################################
##
#@file TauRecRunConfigured.py
#
#@brief Auxilary class used by TauRec[AOD]Builder.py
#@brief Main tau algorithms in TauRecBuilder.py are actually tools and not algs
#@brief this class facilitates an easy method of wrapping the TauRecBuilder tools
#@brief with a dummy algorithm, TauProcessorAlg.cxx
#
#@author J. Griffiths
#
################################################################################

from RecExConfig.Configured import Configured
from AthenaCommon.SystemOfUnits import *

################################################################################
## @class TauRecRunConfigured
# Make TauRecXYZ public this class
################################################################################
class TauRecRunConfigured ( Configured ) :
    """Intermediate class which wraps up a ITauToolExecBase
    into a tauRec/TauProcessorAlg algorithm
    """

    def __init__(self, name = "TauRecRunConfigured", msglevel=3, ignoreExistingDataObject=True) :
        self.name = name
        self.msglevel = msglevel
        from tauRec.tauRecConf import TauRunnerAlg 
        from tauRec.tauRecFlags import tauFlags
        self._TauRunnerAlgHandle = TauRunnerAlg ( name=self.name+'Alg', 
                                                  Key_tauInputContainer="tmp_TauJets",
                                                  Key_Pi0ClusterInputContainer="TauPi0SubtractedClusters",
                                                  Key_tauOutputContainer="TauJets",
                                                  Key_neutralPFOOutputContainer="TauNeutralParticleFlowObjects",
                                                  Key_pi0ClusterOutputContainer="TauPi0Clusters",
                                                  Key_hadronicPFOOutputContainer="TauHadronicParticleFlowObjects",
                                                  Key_vertexOutputContainer = "TauSecondaryVertices",
                                                  Key_chargedPFOOutputContainer = "TauChargedParticleFlowObjects",
                                                  Key_pi0Container= "finalTauPi0s"
                                                  )
                                                                                                
        Configured.__init__(self, ignoreExistingDataObject=ignoreExistingDataObject)


    def WrapTauRecToolExecHandle(self, tool=None ):
        self.TauRunnerAlgHandle().Tools = tool
        from AthenaCommon.AlgSequence import AlgSequence

        topSequence = AlgSequence()
        
        topSequence += self.TauRunnerAlgHandle()

    def AddToolsToToolSvc(self, tools=[]):
        from AthenaCommon.AppMgr import ToolSvc
        from tauRec.tauRecFlags import tauFlags
        for tool in tools :
            tool.calibFolder = tauFlags.tauRecToolsCVMFSPath()
            if tool not in ToolSvc : ToolSvc += tool

    def TauRunnerAlgHandle(self):
        return self._TauRunnerAlgHandle

