# Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
from AthenaConfiguration.ComponentFactory import CompFactory
from InDetConfig.InDetRecCaloSeededROISelectionConfig import egammaCaloClusterSelectorCfg, egammaHadCaloClusterSelectorCfg


def ITkCaloClusterROI_SelectorCfg(ConfigFlags, name="ITkCaloClusterROISelector", **kwargs):
    from egammaAlgs.egammaTopoClusterCopierConfig import egammaTopoClusterCopierCfg
    result = egammaTopoClusterCopierCfg(ConfigFlags)
    kwargs.setdefault("InputClusterContainerName",  ConfigFlags.Egamma.Keys.Internal.EgammaTopoClusters)
    kwargs.setdefault("OutputClusterContainerName", "ITkCaloClusterROIs")
    if "CaloClusterROIBuilder" not in kwargs:
        kwargs["CaloClusterROIBuilder"] = CompFactory.InDet.CaloClusterROI_Builder(
            "ITkCaloClusterROIBuilder",
            EMEnergyOnly=True,
        )
    if "egammaCaloClusterSelector" not in kwargs:
        kwargs["egammaCaloClusterSelector"] = result.popToolsAndMerge(egammaCaloClusterSelectorCfg(ConfigFlags))
    result.addEventAlgo(CompFactory.InDet.CaloClusterROI_Selector(name, **kwargs), primary=True)
    return result


def ITkHadCaloClusterROI_SelectorCfg(ConfigFlags, name="InDetHadCaloClusterROISelector", **kwargs):
    from egammaAlgs.egammaTopoClusterCopierConfig import egammaTopoClusterCopierCfg
    result = egammaTopoClusterCopierCfg(ConfigFlags)

    kwargs.setdefault("InputClusterContainerName",  "CaloCalTopoClusters")
    kwargs.setdefault("OutputClusterContainerName", "ITkHadCaloClusterROIs" + "Bjet")
    if "CaloClusterROIBuilder" not in kwargs:
        kwargs["CaloClusterROIBuilder"] = CompFactory.InDet.CaloClusterROI_Builder("ITkCaloClusterROIBuilder")
    if "egammaCaloClusterSelector" not in kwargs:
        kwargs["egammaCaloClusterSelector"] = result.popToolsAndMerge(egammaHadCaloClusterSelectorCfg(ConfigFlags))
    result.addEventAlgo(CompFactory.InDet.CaloClusterROI_Selector(name, **kwargs), primary=True)
    return result
