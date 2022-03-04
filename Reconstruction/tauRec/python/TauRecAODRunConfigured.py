# Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration

################################################################################
##
#@file TauRecAODRunConfigured.py
#
#@brief Auxilary class used by TauRecAODRunner.py
#@brief Main tau algorithms in TauRecBuilder.py are actually tools and not algs
#@brief this class facilitates an easy method of wrapping the TauRecBuilder tools
#@brief with a dummy algorithm, TauProcessorAlg.cxx
#
#@author D. Qichen
#
################################################################################

from RecExConfig.Configured import Configured

################################################################################
## @class TauRecAODRunConfigured
# Make TauRecXYZ public this class
################################################################################
class TauRecAODRunConfigured ( Configured ) :
    """Re-Reconstruct TauJets on AOD level. 
    IMPORTANT: the results may deviate from official Tau reconstruction
    """
    def __init__(self, name = "TauRecAODRunConfigured", msglevel=3, ignoreExistingDataObject=True) :
        self.name = name
        self.msglevel = msglevel
        from tauRec.tauRecConf import TauAODRunnerAlg
        self._TauAODRunnerAlgHandle = TauAODRunnerAlg(  name=self.name+'Alg', 
                                                        OutputLevel = msglevel )
        Configured.__init__(self, ignoreExistingDataObject=ignoreExistingDataObject)

    def WrapTauRecToolExecHandle(self, tools_mod=None, tools_after=None):
        from tauRec.tauRecFlags import tauFlags
        for atool in tools_mod:
            atool.calibFolder = tauFlags.tauRecToolsCVMFSPath()
            atool.inAOD = True
        for atool in tools_after:
            atool.calibFolder = tauFlags.tauRecToolsCVMFSPath()
            atool.inAOD = True
        self.TauAODRunnerAlgHandle().modificationTools = tools_mod
        self.TauAODRunnerAlgHandle().officialTools = tools_after
        from AthenaCommon.AlgSequence import AlgSequence
        topSequence = AlgSequence()
        topSequence += self.TauAODRunnerAlgHandle()

    def TauAODRunnerAlgHandle(self):
        return self._TauAODRunnerAlgHandle

