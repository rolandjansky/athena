# Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration

__doc__ = """
Tool configuration to instantiate
all egammaCaloTools with default configuration.
"""

from AthenaConfiguration.ComponentAccumulator import ComponentAccumulator
from AthenaConfiguration.ComponentFactory import CompFactory


def CaloFillRectangularClusterCfg(flags, **kwargs):
    result = ComponentAccumulator()

    kwargs.setdefault("eta_size", 5)
    kwargs.setdefault("phi_size", 7)
    kwargs.setdefault("cells_name", flags.Egamma.Keys.Input.CaloCells)

    result.setPrivateTools(
        CompFactory.CaloFillRectangularCluster(**kwargs))
    return result


def egammaCaloClusterSelectorCfg(ConfigFlags,
                                 name="caloClusterROISelector",
                                 **kwargs):
    result = ComponentAccumulator()

    if "egammaCheckEnergyDepositTool" not in kwargs:
        kwargs["egammaCheckEnergyDepositTool"] = CompFactory.egammaCheckEnergyDepositTool()
    kwargs.setdefault("EMEtCut", 2250.)
    kwargs.setdefault("EMEtSplittingFraction", 0.7)
    kwargs.setdefault("EMFCut", 0.7)
    kwargs.setdefault("RetaCut", 0.65)
    kwargs.setdefault("HadLeakCut", 0.15)
    result.setPrivateTools(
        CompFactory.egammaCaloClusterSelector(name, **kwargs))
    return result


def egammaHadCaloClusterSelectorCfg(ConfigFlags,
                                    name="caloClusterHadROISelector",
                                    **kwargs):
    result = ComponentAccumulator()
    kwargs.setdefault("egammaCheckEnergyDepositTool", "")
    kwargs.setdefault("ClusterEtCut", 150e3)
    result.setPrivateTools(
        CompFactory.egammaCaloClusterSelector(name, **kwargs))
    return result
