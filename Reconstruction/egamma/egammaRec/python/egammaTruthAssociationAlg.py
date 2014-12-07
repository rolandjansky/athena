# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

from RecExConfig.Configured import Configured
from egammaRec import egammaRecConf, egammaKeys
from egammaRec.Factories import ToolFactory, AlgFactory, FcnWrapper
from AthenaCommon.BeamFlags import jobproperties
from egammaRec import egammaRecFlags as egRecFlags
egammaRecFlags = egRecFlags.jobproperties.egammaRecFlags
from egammaTools.InDetTools import egammaExtrapolator
from MCTruthClassifier import MCTruthClassifierConf
import AthenaCommon.CfgMgr as CfgMgr

class egammaTruthAssociationGetter ( Configured ) :

    def configure(self):

        EMClassifierParticleCaloExtensionTool =  ToolFactory (CfgMgr.Trk__ParticleCaloExtensionTool, 
                                                              name="EMClassifierParticleCaloExtensionTool",
                                                              Extrapolator = egammaExtrapolator,
                                                              OutputContainerName="EGClassifierCaloExtension"
                                                              )
        
        EMMCTruthClassifier = ToolFactory( MCTruthClassifierConf.MCTruthClassifier, name = 'EMMCTruthClassifier',
                                           ParticleCaloExtensionTool=EMClassifierParticleCaloExtensionTool)
    

        egammaTruthAssociationAlg = AlgFactory( egammaRecConf.egammaTruthAssociationAlg,
                                                ClusterContainerName = egammaKeys.outputClusterKey(),
                                                ElectronContainerName = egammaKeys.outputElectronKey(),
                                                PhotonContainerName = egammaKeys.outputPhotonKey(),
                                                FwdElectronContainerName = egammaKeys.outputFwdElectronKey(),
                                                TruthParticleContainerName = egammaKeys.truthParticleKey(),
                                                EgammaTruthContainerName = egammaKeys.outputTruthKey(),
                                                MCTruthClassifier = EMMCTruthClassifier,
                                                MatchForwardElectrons = egammaRecFlags.doEgammaForwardSeeded(),
                                                MatchClusters = False
                                                )
        # configure egammaTruthAssociation here:
        try:
            self._egammaTruthAssociationHandle = egammaTruthAssociationAlg()
        except Exception:
            mlog.error("could not get handle to egammaTruthAssociation")
            print traceback.format_exc()
            return False
        return True

    def egammaTruthAssociationHandle(self):
        return self._egammaTruthAssociationHandle

