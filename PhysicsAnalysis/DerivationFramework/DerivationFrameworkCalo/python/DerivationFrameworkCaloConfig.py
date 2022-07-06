# Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
#!/usr/bin/env python

from AthenaConfiguration.ComponentAccumulator import ComponentAccumulator
from AthenaConfiguration.ComponentFactory import CompFactory


def MaxCellDecoratorCfg(ConfigFlags,**kwargs):
    acc = ComponentAccumulator()
    kwargs.setdefault("SGKey_electrons", ConfigFlags.Egamma.Keys.Output.Electrons)
    kwargs.setdefault("SGKey_photons", ConfigFlags.Egamma.Keys.Output.Photons)
    acc.setPrivateTools(CompFactory.DerivationFramework.MaxCellDecorator(**kwargs))
    return acc

def GainDecoratorCfg(ConfigFlags,**kwargs):
    acc = ComponentAccumulator()
    kwargs.setdefault("SGKey_electrons", ConfigFlags.Egamma.Keys.Output.Electrons)
    kwargs.setdefault("SGKey_photons", ConfigFlags.Egamma.Keys.Output.Photons)
    kwargs.setdefault("name", "GainDecor")
    acc.setPrivateTools(CompFactory.DerivationFramework.GainDecorator(**kwargs))
    return acc

def CaloFillRectangularClusterCfg(ConfigFlags,**kwargs):
    acc = ComponentAccumulator()
    kwargs.setdefault("cells_name", ConfigFlags.Egamma.Keys.Input.CaloCells)
    kwargs.setdefault("fill_cluster", True)
    acc.setPrivateTools(CompFactory.CaloFillRectangularCluster(**kwargs))
    return acc

def ClusterEnergyPerLayerDecoratorCfg(ConfigFlags,**kwargs):
    acc = ComponentAccumulator()
    kwargs.setdefault("SGKey_electrons", ConfigFlags.Egamma.Keys.Output.Electrons)
    kwargs.setdefault("SGKey_photons", ConfigFlags.Egamma.Keys.Output.Photons)
    kwargs.setdefault("SGKey_caloCells", ConfigFlags.Egamma.Keys.Input.CaloCells)
    kwargs.setdefault("neta", 5)
    kwargs.setdefault("nphi", 5)
    toolArgs = {}
    toolArgs.update({"eta_size": kwargs["neta"]})
    toolArgs.update({"phi_size": kwargs["nphi"]})
    kwargs.setdefault("CaloFillRectangularClusterTool", acc.popToolsAndMerge(CaloFillRectangularClusterCfg(ConfigFlags,**toolArgs) ) )
    acc.setPrivateTools(CompFactory.DerivationFramework.ClusterEnergyPerLayerDecorator(**kwargs))
    return acc

def CaloClusterThinningCfg(ConfigFlags,**kwargs):
    acc = ComponentAccumulator()
    CaloClusterThinning = CompFactory.DerivationFramework.CaloClusterThinning
    acc.addPublicTool(CaloClusterThinning(**kwargs),primary=True)
    return acc
