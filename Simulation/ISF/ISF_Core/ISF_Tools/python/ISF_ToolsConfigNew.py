"""ComponentAccumulator tool configuration for ISF

Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
"""
from AthenaConfiguration.ComponentAccumulator import ComponentAccumulator
from AthenaConfiguration.ComponentFactory import CompFactory
from AthenaCommon.SystemOfUnits import MeV
from BarcodeServices.BarcodeServicesConfigNew import BarcodeSvcCfg


def ParticleHelperCfg(flags, name="ISF_ParticleHelper", **kwargs):
    acc = BarcodeSvcCfg(flags)
    kwargs.setdefault("BarcodeSvc", acc.getPrimary())
    acc.addPublicTool(CompFactory.ISF.ParticleHelper(name, **kwargs))
    return acc


def MemoryMonitorToolCfg(flags, name="ISF_MemoryMonitor", **kwargs):
    acc = ComponentAccumulator()
    acc.setPrivateTools(CompFactory.ISF.MemoryMonitoringTool(name, **kwargs))
    return acc


def EntryLayerFilterCfg(ConfigFlags, **kwargs):
    """Return the MCxEntryLayerFilterCfg config flagged by Sim.TruthStrategy"""
    stratmap = {
        "MC12": MC12EntryLayerFilterCfg,
        "MC12LLP": MC12LLPEntryLayerFilterCfg,
        "MC12Plus": MC12PlusEntryLayerFilterCfg,
        "MC15": MC15EntryLayerFilterCfg,
        "MC15a": MC15aEntryLayerFilterCfg,
        "MC15aPlus": MC15aPlusEntryLayerFilterCfg,
        "MC15aPlusLLP": MC15aPlusLLPEntryLayerFilterCfg,
        "MC16": MC16EntryLayerFilterCfg,
        "MC16LLP": MC16LLPEntryLayerFilterCfg,
        "MC18": MC18EntryLayerFilterCfg,
        "MC18LLP": MC18LLPEntryLayerFilterCfg,
        "Validation": ValidationEntryLayerFilterCfg,
        "Cosmic": CosmicEventFilterToolCfg,
    }
    MCxCfg = stratmap[ConfigFlags.Sim.TruthStrategy]
    return MCxCfg(ConfigFlags, **kwargs)


def MC12EntryLayerFilterCfg(flags, name="ISF_MC12EntryLayerFilter", **kwargs):
    kwargs.setdefault("AllowOnlyDefinedBarcodes", True)
    kwargs.setdefault("AllowOnlyLegacyPrimaries", False)
    kwargs.setdefault("LegacyParticleGenerationIncrement", 1000000)
    kwargs.setdefault("LegacyFirstSecondaryBarcode", 200001)
    return CompFactory.ISF.GenericBarcodeFilter(name, **kwargs)


def MC12LLPEntryLayerFilterCfg(flags, name="ISF_MC12LLPEntryLayerFilter", **kwargs):
    return MC12EntryLayerFilterCfg(flags, name, **kwargs)


def MC12PlusEntryLayerFilterCfg(flags, name="ISF_MC12PlusEntryLayerFilter", **kwargs):
    return MC12EntryLayerFilterCfg(flags, name, **kwargs)


def MC15EntryLayerFilterCfg(flags, name="ISF_MC15EntryLayerFilter", **kwargs):
    kwargs.setdefault("MinEkinCharged", 100.*MeV)
    kwargs.setdefault("MinEkinNeutral", -1.)
    return CompFactory.ISF.EntryLayerFilter(name, **kwargs  )


def MC15aEntryLayerFilterCfg(flags, name="ISF_MC15aEntryLayerFilter", **kwargs):
    return MC15EntryLayerFilterCfg(flags, name, **kwargs)


def MC15aPlusEntryLayerFilterCfg(flags, name="ISF_MC15aPlusEntryLayerFilter", **kwargs):
    return MC15EntryLayerFilterCfg(flags, name, **kwargs)


def MC15aPlusLLPEntryLayerFilterCfg(flags, name="ISF_MC15aPlusLLPEntryLayerFilter", **kwargs):
    return MC15aPlusEntryLayerFilterCfg(flags, name, **kwargs)


def MC16EntryLayerFilterCfg(flags, name="ISF_MC16EntryLayerFilter", **kwargs):
    return MC15aPlusEntryLayerFilterCfg(flags, name, **kwargs)


def MC16LLPEntryLayerFilterCfg(flags, name="ISF_MC16LLPEntryLayerFilter", **kwargs):
    return MC15aPlusLLPEntryLayerFilterCfg(flags, name, **kwargs)


def MC18EntryLayerFilterCfg(flags, name="ISF_MC18EntryLayerFilter", **kwargs):
    return MC15aPlusEntryLayerFilterCfg(flags, name, **kwargs)


def MC18LLPEntryLayerFilterCfg(flags, name="ISF_MC18LLPEntryLayerFilter", **kwargs):
    return MC15aPlusLLPEntryLayerFilterCfg(flags, name, **kwargs)


def ValidationEntryLayerFilterCfg(flags, name="ISF_ValidationEntryLayerFilter", **kwargs):
    return MC12EntryLayerFilterCfg(flags, name, **kwargs)


def CosmicEventFilterToolCfg(flags, name="ISF_CosmicEventFilter", **kwargs):
    volmap = {
        "Muon": ["MuonExitLayer"],
        "Calo": ["MuonEntryLayer"],
        "InnerDetector": ["CaloEntryLayer"],
        "TRT_Barrel": ["TRTBarrelEntryLayer"],
        "TRT_EC": ["TRTECAEntryLayer", "TRTECBEntryLayer"],
        "SCT_Barrel": ["SCTBarrelEntryLayer"],
        "Pixel": ["PixelEntryLayer"],
    }
    volumeNames = []
    for vol in flags.Sim.CosmicFilterVolumeNames:
        volumeNames += volmap[name]

    kwargs.setdefault("UseAndFilter", True)
    kwargs.setdefault("VolumeNames", volumeNames)
    kwargs.setdefault("PDG_ID", flags.Sim.CosmicFilterID)
    kwargs.setdefault("ptMin", flags.Sim.CosmicFilterPTmin)
    kwargs.setdefault("ptMax", flags.Sim.CosmicFilterPTmax)
    acc = ComponentAccumulator()
    acc.setPrivateTools(CompFactory.ISF.CosmicEventFilterTool(name, **kwargs))
    return acc


def StoppedParticleFilterToolCfg(flags, name="ISF_StoppedParticleFilter", **kwargs):
    kwargs.setdefault("VolumeNames", ["StoppingPositions"])
    acc = ComponentAccumulator()
    acc.setPrivateTools(CompFactory.ISF.CosmicEventFilterTool(name, **kwargs))
    return acc


def InToOutSubDetOrderingToolCfg(flags, name="ISF_InToOutSubDetOrderingTool", **kwargs):
    # higher ordered particles will be simulated first
    kwargs.setdefault("OrderID"      , 100000000)
    kwargs.setdefault("OrderBeamPipe", 1000000  )
    kwargs.setdefault("OrderCalo"    , 10000    )
    kwargs.setdefault("OrderMS"      , 100      )
    kwargs.setdefault("OrderCavern"  , 1        )
    acc = ComponentAccumulator()
    acc.setPrivateTools(CompFactory.ISF.GenericParticleOrderingTool(name, **kwargs))
    return acc


def ParticleOrderingToolCfg(flags, name="ISF_ParticleOrderingTool", **kwargs):
    kwargs.setdefault("OrderID"      , 1)
    kwargs.setdefault("OrderBeamPipe", 1)
    kwargs.setdefault("OrderCalo"    , 1)
    kwargs.setdefault("OrderMS"      , 1)
    kwargs.setdefault("OrderCavern"  , 1)
    acc = ComponentAccumulator()
    acc.setPrivateTools(CompFactory.ISF.GenericParticleOrderingTool(name, **kwargs))
    return acc


def EnergyParticleOrderingToolCfg(flags, name="ISF_EnergyParticleOrderingTool", **kwargs):
    acc = ComponentAccumulator()
    acc.setPrivateTools(CompFactory.ISF.EnergyParticleOrderingTool(name, **kwargs))
    return acc


def ParticleKillerToolCfg(flags, name="ISF_ParticleKillerTool", **kwargs):
    acc = ComponentAccumulator()
    acc.setPrivateTools(CompFactory.ISF.ParticleKillerSimTool(name, **kwargs))
    return acc
