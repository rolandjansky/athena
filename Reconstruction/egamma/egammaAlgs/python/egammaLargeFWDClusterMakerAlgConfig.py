# Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration

__doc__ = """Configure egammaLargeFWDClusterMaker,
             which chooses cells to store in the AOD"""
__author__ = "Jovan Mitrevski"

from AthenaConfiguration.ComponentAccumulator import ComponentAccumulator
from AthenaConfiguration.ComponentFactory import CompFactory
from egammaTools.egammaLargeFWDClusterMakerConfig import (
    egammaLargeFWDClusterMakerCfg)
from CaloClusterCorrection.CaloSwCorrections import (
    make_CaloSwCorrectionsCfg)


def egammaLargeFWDClusterMakerAlgCfg(
        flags,
        name="egammaLargeFWDClusterMaker",
        **kwargs):

    acc = ComponentAccumulator()

    kwargs.setdefault("SaveUncalibratedSignalState", False)
    kwargs.setdefault("ClustersOutputName",
                      flags.Egamma.Keys.Output.EgammaLargeFWDClusters)

    if "ClusterMakerTools" not in kwargs:
        toolAcc = egammaLargeFWDClusterMakerCfg(flags)
        kwargs["ClusterMakerTools"] = [toolAcc.popPrivateTools()]
        acc.merge(toolAcc)

    if "ClusterCorrectionTools" not in kwargs:
        tools = make_CaloSwCorrectionsCfg(
            flags,
            "FWDele6_6",
            suffix="Nocorr",
            version="none",
            cells_name=flags.Egamma.Keys.Input.CaloCells)
        kwargs["ClusterCorrectionTools"] = acc.popToolsAndMerge(tools)

    acc.addEventAlgo(CompFactory.CaloClusterMaker(name, **kwargs))
    return acc
