# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

################################################################################
##
#@file TauRecConfigured.py
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
## @class TauRecConfigured
# Make TauRecXYZ public this class
################################################################################
class TauRecConfigured ( Configured ) :
    """Intermediate class which wraps up a ITauToolExecBase
    into a tauRec/TauProcessorAlg algorithm
    """

    def __init__(self, name = "TauRecConfigured", msglevel=3, ignoreExistingDataObject=True) :
        self.name = name
        self.msglevel = msglevel
        from tauRec.tauRecConf import TauProcessorAlg 
        from tauRec.tauRecFlags import tauFlags
        self._TauProcessorAlgHandle = TauProcessorAlg ( name=self.name+'Alg',
                                                        Key_jetInputContainer="AntiKt4LCTopoJets",
                                                        Key_tauOutputContainer="TauJets",
                                                        Key_tauTrackOutputContainer="TauTracks",
                                                        #MaxEta = 2.5,                                                                         
                                                        MaxEta = tauFlags.tauRecSeedMaxEta(),
                                                        MinPt = 10.*GeV,
                                                        doCreateTauContainers = True)

        Configured.__init__(self, ignoreExistingDataObject=ignoreExistingDataObject)


    def WrapTauRecToolExecHandle(self, tool=None ):
        self.TauProcessorAlgHandle().Tools = tool
        #self.TauProcessorAlgHandle().name = tool.name+'Alg'
        from AthenaCommon.AlgSequence import AlgSequence

        topSequence = AlgSequence()

        from SGComps.SGCompsConf import SGInputLoader
        # not needed? There by default now?
        topSequence += SGInputLoader()
        topSequence.SGInputLoader.Load = [ ('xAOD::JetContainer','AntiKt4LCTopoJets'), ('xAOD::VertexContainer', 'PrimaryVertices'),
                                           ('xAOD::TrackParticleContainer','InDetTrackParticles'), ('CaloCellContainer','AllCalo') ]
        topSequence += self.TauProcessorAlgHandle()

    def WrapTauRecToolExecHandles(self, tools=[]):
        self.TauProcessorAlgHandle().Tools = tools
        for tool in tools:
            self.TauProcessorAlghandle().tool.getFullName = tool

        from AthenaCommon.AlgSequence import AlgSequence
        topSequence = AlgSequence()

        from SGComps.SGCompsConf import SGInputLoader
        # not needed? There by default now?
        topSequence += SGInputLoader(OutputLevel=INFO)
        topSequence.SGInputLoader.Load = [ ('xAOD::JetContainer','AntiKt4LCTopoJets'), ('xAOD::VertexContainer', 'PrimaryVertices'),
                                           ('xAOD::TrackParticleContainer','InDetTrackParticles'), ('CaloCellContainer','AllCalo') ]
        topSequence += self.TauProcessorAlgHandle()


    def AddToolsToToolSvc(self, tools=[]):
        from AthenaCommon.AppMgr import ToolSvc
        from tauRec.tauRecFlags import tauFlags
        for tool in tools :
            # if tool.__slots__['calibFolder'].count('TauDiscriminant'):
            #     tool.calibFolder = tauFlags.TauDiscriminantCVMFSPath()
            # else :
            #     tool.calibFolder = tauFlags.tauRecToolsCVMFSPath()
            tool.calibFolder = tauFlags.tauRecToolsCVMFSPath()
            if tool not in ToolSvc : ToolSvc += tool            
            pass

    def TauProcessorAlgHandle(self):
        return self._TauProcessorAlgHandle

