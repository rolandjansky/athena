# Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
# Configuration of InDetCaloClusterROISelector package
from AthenaConfiguration.ComponentFactory import CompFactory

def CaloClusterROI_SelectorCfg(ConfigFlags, name="CaloClusterROISelector", **kwargs):
    from egammaAlgs.egammaTopoClusterCopierConfig import egammaTopoClusterCopierCfg
    result = egammaTopoClusterCopierCfg(ConfigFlags)

    kwargs.setdefault("InputClusterContainerName",  ConfigFlags.Egamma.Keys.Internal.EgammaTopoClusters)
    kwargs.setdefault("OutputClusterContainerName", "ITkCaloClusterROIs" if ConfigFlags.Detector.GeometryITk else "InDetCaloClusterROIs")
    if "CaloClusterROIBuilder" not in kwargs:
        kwargs["CaloClusterROIBuilder"] = CompFactory.InDet.CaloClusterROI_Builder(
            "CaloClusterROIBuilder",
            EMEnergyOnly=True,
        )
    if "egammaCaloClusterSelector" not in kwargs:
        from egammaCaloTools.egammaCaloToolsConfig import egammaCaloClusterSelectorCfg
        kwargs["egammaCaloClusterSelector"] = result.popToolsAndMerge(egammaCaloClusterSelectorCfg(ConfigFlags))
    result.addEventAlgo(CompFactory.InDet.CaloClusterROI_Selector(name, **kwargs), primary=True)
    return result

def HadCaloClusterROI_SelectorCfg(ConfigFlags, name="HadCaloClusterROISelector", **kwargs):
    from egammaAlgs.egammaTopoClusterCopierConfig import egammaTopoClusterCopierCfg
    result = egammaTopoClusterCopierCfg(ConfigFlags)

    kwargs.setdefault("InputClusterContainerName",  "CaloCalTopoClusters")
    kwargs.setdefault("OutputClusterContainerName", "ITkHadCaloClusterROIsBjet" if ConfigFlags.Detector.GeometryITk else "InDetHadCaloClusterROIsBjet")
    if "CaloClusterROIBuilder" not in kwargs:
        kwargs["CaloClusterROIBuilder"] = CompFactory.InDet.CaloClusterROI_Builder(
            "CaloClusterROIBuilder",
            EMEnergyOnly=False,
        )
    if "egammaCaloClusterSelector" not in kwargs:
        from egammaCaloTools.egammaCaloToolsConfig import egammaHadCaloClusterSelectorCfg
        kwargs["egammaCaloClusterSelector"] = result.popToolsAndMerge(egammaHadCaloClusterSelectorCfg(ConfigFlags))
    result.addEventAlgo(CompFactory.InDet.CaloClusterROI_Selector(name, **kwargs), primary=True)
    return result
