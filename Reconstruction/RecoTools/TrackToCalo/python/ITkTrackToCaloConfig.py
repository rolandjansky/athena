# Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration

__doc__ = "Tool configuration for the track to calo tools."

#---------------------------------------
from AthenaConfiguration.ComponentAccumulator import ComponentAccumulator
from AthenaConfiguration.ComponentFactory import CompFactory

from TrkConfig.AtlasUpgradeExtrapolatorConfig import AtlasUpgradeExtrapolatorCfg

def ITkParticleCaloExtensionToolCfg(flags, **kwargs):
    acc=ComponentAccumulator()

    if "Extrapolator" not in kwargs:
        extrapAcc = AtlasUpgradeExtrapolatorCfg(flags)
        kwargs["Extrapolator"] = acc.popToolsAndMerge(extrapAcc)

    caloExtensionTool = CompFactory.Trk.ParticleCaloExtensionTool(**kwargs)

    acc.setPrivateTools(caloExtensionTool)
    return acc

def ITkParticleCaloCellAssociationToolCfg(flags, **kwargs):
    acc=ComponentAccumulator()

    if "ParticleCaloExtensionTool" not in kwargs:
        pcExtrapToolAcc = ITkParticleCaloExtensionToolCfg(flags)
        kwargs["ParticleCaloExtensionTool"] = acc.popToolsAndMerge(pcExtrapToolAcc)

    # should this be a more global flag? It depends on whether you are in AOD
    kwargs.setdefault("CaloCellContainer", flags.Egamma.Keys.Input.CaloCells)

    cellAssocTool = CompFactory.Rec.ParticleCaloCellAssociationTool(**kwargs)

    acc.setPrivateTools(cellAssocTool)
    return acc

# Still to do, ParticleCaloClusterAssociationToolCfg and MuonCaloEnergyToolCfg
# They still have public tools

#---------------------------------------

