# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

__doc__ = """ToolFactory to instantiate the
two supercluster builders with default configuration"""
__author__ = "Jovan Mitrevski"

from egammaAlgs import egammaAlgsConf
from egammaRec.Factories import AlgFactory
from egammaRec import egammaKeys

from egammaTools.egammaToolsFactories import EGammaAmbiguityTool

topoEgammaBuilder = AlgFactory(
    egammaAlgsConf.topoEgammaBuilder,
    name='topoEgammaBuilder',
    SuperElectronRecCollectionName=egammaKeys.ElectronSuperRecKey(),
    SuperPhotonRecCollectionName=egammaKeys.PhotonSuperRecKey(),
    ElectronOutputName=egammaKeys.outputElectronKey(),
    PhotonOutputName=egammaKeys.outputPhotonKey(),
    AmbiguityTool=EGammaAmbiguityTool
)
