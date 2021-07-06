# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

__doc__ = """ToolFactory to instantiate egammaLargeClusterMaker
with FWD configuration"""
__author__ = "Jovan Mitrevski"

from egammaTools.egammaToolsFactories import egammaLargeFWDClusterMakerTool
from CaloRec import CaloRecConf
from egammaRec.Factories import AlgFactory, FcnWrapper
from egammaRec import egammaKeys
from CaloClusterCorrection.CaloSwCorrections import make_CaloSwCorrections


def clusMakerTools():
    return [egammaLargeFWDClusterMakerTool()]

egammaLargeFWDClusterMakerAlg = AlgFactory(
    CaloRecConf.CaloClusterMaker,
    name="egammaLargeFWDClusterMaker",
    SaveUncalibratedSignalState=False,
    ClustersOutputName=egammaKeys.EgammaLargeFWDClustersKey(),
    ClusterMakerTools=FcnWrapper(clusMakerTools),
    ClusterCorrectionTools=make_CaloSwCorrections("FWDele6_6",
                                                  suffix="Nocorr",
                                                  version="none",
                                                  corrlist=[],
                                                  cells_name=egammaKeys.caloCellKey()))
