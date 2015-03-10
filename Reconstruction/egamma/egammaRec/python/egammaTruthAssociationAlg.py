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

EMClassifierParticleCaloExtensionTool =  ToolFactory (CfgMgr.Trk__ParticleCaloExtensionTool, 
                                                      name="EMClassifierParticleCaloExtensionTool",
                                                      Extrapolator = egammaExtrapolator,
                                                      OutputContainerName="EGClassifierCaloExtension"
                                                      )

EMMCTruthClassifier = ToolFactory( MCTruthClassifierConf.MCTruthClassifier, name = 'EMMCTruthClassifier',
                                   ParticleCaloExtensionTool=EMClassifierParticleCaloExtensionTool)

def getSimBarcodeOffset():
  "Return the simulation barcode offset for G4 particles from metadata"
  from RecExConfig.InputFilePeeker import inputFileSummary
  try:
    return inputFileSummary['metadata']['/Simulation/Parameters']['SimBarcodeOffset']
  except:
    print 'Could not retrieve TruthStrategy from /Simulation/Parameters, leaving barcode_offset at 200k'
    return int(200e3)

egammaTruthAssociationAlg = AlgFactory( egammaRecConf.egammaTruthAssociationAlg,
                                        ClusterContainerName = egammaKeys.outputClusterKey(),
                                        ElectronContainerName = egammaKeys.outputElectronKey(),
                                        PhotonContainerName = egammaKeys.outputPhotonKey(),
                                        FwdElectronContainerName = egammaKeys.outputFwdElectronKey(),
                                        TruthEventContainerName = egammaKeys.truthEventKey(),
                                        TruthParticleContainerName = egammaKeys.truthParticleKey(),                                                
                                        EgammaTruthContainerName = egammaKeys.outputTruthKey(),
                                        MCTruthClassifier = EMMCTruthClassifier,
                                        MatchForwardElectrons = egammaRecFlags.doEgammaForwardSeeded(),
                                        MatchClusters = False,
                                        SimBarcodeOffset = FcnWrapper( getSimBarcodeOffset )
                                        )


class egammaTruthAssociationGetter ( Configured ) :

    def configure(self):

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

