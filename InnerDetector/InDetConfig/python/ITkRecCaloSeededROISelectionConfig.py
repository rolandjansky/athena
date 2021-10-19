# Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
from AthenaConfiguration.ComponentAccumulator import ComponentAccumulator
from AthenaConfiguration.ComponentFactory import CompFactory
from InDetConfig.InDetRecCaloSeededROISelectionConfig import egammaCaloClusterSelectorCfg

def ITkCaloClusterROI_SelectorCfg(ConfigFlags, name="ITkCaloClusterROISelector", **kwargs):
    result = ComponentAccumulator()
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
