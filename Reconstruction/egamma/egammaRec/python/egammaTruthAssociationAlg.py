# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

__doc__ = "To instantiate egammaTruthAssociationAlg with default configuration"
__author__ = "Bruno Lenzi"

from egammaRec import egammaRecConf, egammaKeys
from egammaRec.Factories import ToolFactory, AlgFactory, FcnWrapper
from egammaRec import egammaRecFlags as egRecFlags
egammaRecFlags = egRecFlags.jobproperties.egammaRecFlags


from egammaTrackTools.egammaTrackToolsFactories import ExtrapolateToCaloTool

from MCTruthClassifier import MCTruthClassifierConf
MCTruthClassifier = ToolFactory( MCTruthClassifierConf.MCTruthClassifier, 
                                 ExtrapolateToCaloTool = ExtrapolateToCaloTool )

################

egammaTruthAssociationAlg = AlgFactory( egammaRecConf.egammaTruthAssociationAlg,
    ElectronContainerName = egammaKeys.outputElectronKey(),
    PhotonContainerName = egammaKeys.outputPhotonKey(),
    FwdElectronContainerName = egammaKeys.outputFwdElectronKey(),
    TruthParticleContainerName = egammaKeys.truthParticleKey(),
    EgammaTruthContainerName = egammaKeys.outputTruthKey(),
    MCTruthClassifier = MCTruthClassifier,
    UseForwardElectrons = egammaRecFlags.doEgammaForwardSeeded()
)

