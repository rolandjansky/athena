# Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration

__doc__ = """
         Configure egammaLargeClusterMaker, 
         which chooses cells to store in the AOD"""
__author__ = "Jovan Mitrevski"

from AthenaConfiguration.ComponentAccumulator import ComponentAccumulator
from AthenaConfiguration.ComponentFactory import CompFactory
from egammaTools.egammaLargeClusterMakerConfig import egammaLargeClusterMakerCfg
from CaloClusterCorrection.CaloSwCorrections import make_CaloSwCorrections
CaloClusterMaker = CompFactory.CaloClusterMaker


def egammaLargeClusterMakerAlgCfg(
        flags,
        name="egammaLargeClusterMaker",
        **kwargs):

    acc = ComponentAccumulator

    kwargs.setdefault("SaveUncalibratedSignalState", False)
    kwargs.setdefault("ClustersOutputName",
                      flags.Egamma.Keys.Output.EgammaLargeClusters)

    if "ClusterMakerTools" not in kwargs:
        toolAcc = egammaLargeClusterMakerCfg(flags)
        kwargs["ClusterMakerTools"] = [toolAcc.popPrivateTools()]
        acc.merge(toolAcc)

    kwargs.setdefault("ClusterCorrectionTools",
                      make_CaloSwCorrections("ele7_11",
                                             suffix="Nocorr",
                                             version="none",
                                             cells_name=flags.Egamma.Keys.Input.CaloCells))

    acc.addEventAlgo(CaloClusterMaker(name, **kwargs))
    return acc
