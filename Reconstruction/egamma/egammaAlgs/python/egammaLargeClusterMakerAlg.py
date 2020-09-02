# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

__doc__ = """ToolFactory to instantiate egammaLargeClusterMaker
with default configuration"""
__author__ = "Jovan Mitrevski"

from egammaTools.egammaToolsFactories import egammaLargeClusterMakerTool
from CaloRec import CaloRecConf
from egammaRec.Factories import AlgFactory, FcnWrapper
from egammaRec import egammaKeys
from CaloClusterCorrection.CaloSwCorrections import make_CaloSwCorrections


def clusMakerTools():
    return [egammaLargeClusterMakerTool()]


egammaLargeClusterMakerAlg = AlgFactory(
    CaloRecConf.CaloClusterMaker,
    name="egammaLargeClusterMaker",
    SaveUncalibratedSignalState=False,
    ClustersOutputName=egammaKeys.EgammaLargeClustersKey(),
    ClusterMakerTools=FcnWrapper(clusMakerTools),
    ClusterCorrectionTools=make_CaloSwCorrections("ele7_11",
                                                  suffix="Nocorr",
                                                  version="none",
                                                  cells_name=egammaKeys.caloCellKey()))
