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
        self._TauProcessorAlgHandle = TauProcessorAlg ( name=self.name+'Alg')
        Configured.__init__(self, ignoreExistingDataObject=ignoreExistingDataObject)


    def WrapTauRecToolExecHandle(self, tool=None ):
        self.TauProcessorAlgHandle().Tools = [tool]
        #self.TauProcessorAlgHandle().name = tool.name+'Alg'
        from AthenaCommon.AlgSequence import AlgSequence
        topSequence = AlgSequence()
        topSequence += self.TauProcessorAlgHandle()

    def WrapTauRecToolExecHandles(self, tools=[]):
        self.TauProcessorAlgHandle().Tools = tools
        #self.TauProcessorAlgHandle().name = name
        from AthenaCommon.AlgSequence import AlgSequence
        topSequence = AlgSequence()
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

