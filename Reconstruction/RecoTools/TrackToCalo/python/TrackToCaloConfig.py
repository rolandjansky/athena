# Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration

__doc__ = "Tool configuration for the track to calo tools."

#---------------------------------------
from AthenaConfiguration.ComponentAccumulator import ComponentAccumulator
from AthenaConfiguration.ComponentFactory import CompFactory

from TrkConfig.AtlasExtrapolatorConfig import AtlasExtrapolatorCfg
Trk__ParticleCaloExtensionTool, Rec__ParticleCaloCellAssociationTool=CompFactory.getComps("Trk::ParticleCaloExtensionTool","Rec::ParticleCaloCellAssociationTool")

def ParticleCaloExtensionToolCfg(flags, **kwargs):
    acc=ComponentAccumulator()

    if "Extrapolator" not in kwargs:
        extrapAcc = AtlasExtrapolatorCfg(flags)
        kwargs["Extrapolator"] = extrapAcc.popPrivateTools()
        acc.merge(extrapAcc)

    caloExtensionTool = Trk__ParticleCaloExtensionTool(**kwargs)

    acc.setPrivateTools(caloExtensionTool)
    return acc

def ParticleCaloCellAssociationToolCfg(flags, **kwargs):
    acc=ComponentAccumulator()

    if "ParticleCaloExtensionTool" not in kwargs:
        pcExtrapToolAcc = ParticleCaloExtensionToolCfg(flags)
        kwargs["ParticleCaloExtensionTool"] = pcExtrapToolAcc.popPrivateTools()
        acc.merge(pcExtrapToolAcc)

    # should this be a more global flag? It depends on whether you are in AOD
    kwargs.setdefault("CaloCellContainer", flags.Egamma.Keys.Input.CaloCells)

    cellAssocTool = Rec__ParticleCaloCellAssociationTool(**kwargs)

    acc.setPrivateTools(cellAssocTool)
    return acc

# Still to do, ParticleCaloClusterAssociationToolCfg and MuonCaloEnergyToolCfg
# They still have public tools

#---------------------------------------

