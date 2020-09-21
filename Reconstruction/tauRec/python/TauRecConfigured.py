# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

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
from AthenaCommon.BeamFlags import jobproperties
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
        from CaloRec.CaloRecConf import CaloCellContainerFinalizerTool
        # add calo cell finalizer tool
        TauCellContainerFinalizer = CaloCellContainerFinalizerTool(name='tauRec_tauPi0CellContainerFinalizer')
        from AthenaCommon.AppMgr import ToolSvc

        self._TauProcessorAlgHandle = TauProcessorAlg ( name=self.name+'Alg',
                                                        Key_jetInputContainer=tauFlags.tauRecSeedJetCollection(),
                                                        Key_tauOutputContainer="tmp_TauJets",
                                                        Key_tauTrackOutputContainer="TauTracks",
                                                        Key_tauShotClusOutputContainer="TauShotClusters",
                                                        Key_tauShotPFOOutputContainer="TauShotParticleFlowObjects",
                                                        Key_tauPi0CellOutputContainer="TauCommonPi0Cells",
                                                        MaxEta = tauFlags.tauRecSeedMaxEta(),
                                                        MinPt = tauFlags.tauRecSeedMinPt(),
                                                        CellMakerTool = TauCellContainerFinalizer)

        Configured.__init__(self, ignoreExistingDataObject=ignoreExistingDataObject)


    def WrapTauRecToolExecHandle(self, tool=None ):

        self.TauProcessorAlgHandle().Tools = tool
        from AthenaCommon.AlgSequence import AlgSequence

        topSequence = AlgSequence()

        if jobproperties.Beam.beamType()!="cosmics":
            # Can I move this to different script?
            from AthenaCommon.AppMgr import ToolSvc
            from JetRec.JetRecConf import JetAlgorithm
            jetTrackAlg = JetAlgorithm("JetTrackAlg_forTaus")
            from JetRecTools.JetRecToolsConf import TrackVertexAssociationTool
            TauTVATool = TrackVertexAssociationTool(TrackParticleContainer = "InDetTrackParticles",
                                                    TrackVertexAssociation="JetTrackVtxAssoc_forTaus",
                                                    VertexContainer= "PrimaryVertices",
                                                    MaxTransverseDistance = 2.5 *mm,
                                                    #MaxLongitudinalDistance = 2 *mm,
                                                    MaxZ0SinTheta = 3.0 *mm,
                                                    #OutputLevel=2
                                                    )
            ToolSvc += TauTVATool
            jetTrackAlg.Tools = [ TauTVATool ]
            topSequence += jetTrackAlg

            # import tauRec.TauAlgorithmsHolder as taualgs
            # add tauJVF tool to topSequence
            # taualgs.setupTauJVFTool

        topSequence += self.TauProcessorAlgHandle()

    # No longer used?
    def WrapTauRecToolExecHandles(self, tools=[]):

        self.TauProcessorAlgHandle().Tools = tools
        for tool in tools:
            self.TauProcessorAlghandle().tool.getFullName = tool

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

