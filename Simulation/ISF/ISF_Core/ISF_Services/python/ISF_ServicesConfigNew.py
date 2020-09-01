"""ComponentAccumulator service configuration for ISF

Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
"""
from AthenaConfiguration.ComponentAccumulator import ComponentAccumulator
from AthenaConfiguration.ComponentFactory import CompFactory
from ISF_HepMC_Tools.ISF_HepMC_ToolsConfigNew import (
    ParticleFinalStateFilterCfg, ParticlePositionFilterDynamicCfg,
    EtaPhiFilterCfg, GenParticleInteractingFilterCfg,
    KeepLLPDecayChildrenStrategyCfg,
    KeepLLPHadronicInteractionChildrenStrategyCfg,
    TruthStrategyGroupID_MC15Cfg,
    TruthStrategyGroupIDHadInt_MC15Cfg,
    TruthStrategyGroupCaloMuBrem_MC15Cfg,
    TruthStrategyGroupCaloDecay_MC15Cfg,
    LLPTruthStrategyCfg,
    TruthStrategyGroupIDCfg,
    TruthStrategyGroupIDHadIntCfg,
    TruthStrategyGroupCaloMuBremCfg,
    ParticleSimWhiteListCfg,
)
from BarcodeServices.BarcodeServicesConfigNew import BarcodeSvcCfg
from ISF_Geant4CommonTools.ISF_Geant4CommonToolsConfigNew import (
    EntryLayerToolCfg, AFIIEntryLayerToolCfg
)
from ISF_Tools.ISF_ToolsConfigNew import ParticleOrderingToolCfg

#include file to access AtlasDetDescr::AtlasRegion enum
import ROOT,cppyy
cppyy.include("AtlasDetDescr/AtlasRegion.h")


def GenParticleFiltersToolCfg(ConfigFlags):
    result = ComponentAccumulator()
    genParticleFilterList = []
    acc = ParticleFinalStateFilterCfg(ConfigFlags)
    genParticleFilterList += [result.popToolsAndMerge(acc)]
    if "ATLAS" in ConfigFlags.GeoModel.Layout or "atlas" in ConfigFlags.GeoModel.Layout:
        if ConfigFlags.Beam.Type != "cosmics":
            acc = ParticlePositionFilterDynamicCfg(ConfigFlags)
            genParticleFilterList += [result.popToolsAndMerge(acc)]
            if ((ConfigFlags.Sim.CavernBG in (False, "Signal")) and
                (not ConfigFlags.Detector.SimulateCavern)):
                acc = EtaPhiFilterCfg(ConfigFlags)
                genParticleFilterList += [result.popToolsAndMerge(acc)]
    acc = GenParticleInteractingFilterCfg(ConfigFlags)
    genParticleFilterList += [result.popToolsAndMerge(acc)]
    result.setPrivateTools(genParticleFilterList)
    return result


def InputConverterCfg(ConfigFlags, name="ISF_InputConverter", **kwargs):
    result = BarcodeSvcCfg(ConfigFlags)
    kwargs.setdefault("BarcodeSvc", result.getPrimary())
    kwargs.setdefault("UseGeneratedParticleMass", False)
    acc_GenParticleFiltersList = GenParticleFiltersToolCfg(ConfigFlags)
    kwargs.setdefault("GenParticleFilters", result.popToolsAndMerge(acc_GenParticleFiltersList) )
    result.addService(CompFactory.ISF.InputConverter(name, **kwargs))
    return result


def LongLivedInputConverterCfg(ConfigFlags, name="ISF_LongLivedInputConverter", **kwargs):
    result = ComponentAccumulator()
    gpfilt = [
        result.popToolsAndMerge(ParticleSimWhiteListCfg(ConfigFlags)),
        result.popToolsAndMerge(ParticlePositionFilterDynamicCfg(ConfigFlags)),
        result.popToolsAndMerge(EtaPhiFilterCfg(ConfigFlags)),
        result.popToolsAndMerge(GenParticleInteractingFilterCfg(ConfigFlags)),
    ]
    kwargs.setdefault("GenParticleFilters", gpfilt)
    kwargs.setdefault("QuasiStableParticlesIncluded", True)
    return InputConverterCfg(ConfigFlags, name, **kwargs)


def ParticleBrokerSvcNoOrderingCfg(ConfigFlags, name="ISF_ParticleBrokerSvcNoOrdering", **kwargs):
    result = ComponentAccumulator()
    if "EntryLayerTool" not in kwargs:
        tool = result.popToolsAndMerge(EntryLayerToolCfg(ConfigFlags))
        kwargs.setdefault("EntryLayerTool", tool)
        kwargs.setdefault("GeoIDSvc", result.getService("ISF_GeoIDSvc"))
    # assume "GeoIDSvc" has been set alongside "EntryLayerTool"
    kwargs.setdefault("AlwaysUseGeoIDSvc", False)
    kwargs.setdefault("ValidateGeoIDs", ConfigFlags.ISF.ValidationMode)
    kwargs.setdefault("ValidationOutput", ConfigFlags.ISF.ValidationMode)
    kwargs.setdefault("ValidationStreamName", "ParticleBroker")

    baracc = BarcodeSvcCfg(ConfigFlags)
    kwargs.setdefault("BarcodeService", baracc.getPrimary())
    result.merge(baracc)

    result.addService(CompFactory.ISF.ParticleBrokerDynamicOnReadIn(name, **kwargs))
    return result


def ParticleBrokerSvcCfg(ConfigFlags, name="ISF_ParticleBrokerSvc", **kwargs):
    # comment copied from old config
    #kwargs.setdefault("ParticleOrderingTool", "ISF_InToOutSubDetOrderingTool")
    result = ParticleOrderingToolCfg(ConfigFlags)
    kwargs.setdefault("ParticleOrderingTool", result.popPrivateTools())
    result.merge(ParticleBrokerSvcNoOrderingCfg(ConfigFlags, name, **kwargs))
    return result


def AFIIParticleBrokerSvcCfg(ConfigFlags, name="ISF_AFIIParticleBrokerSvc", **kwargs):
    result = AFIIEntryLayerToolCfg(ConfigFlags)
    kwargs.setdefault("EntryLayerTool", result.popPrivateTools())
    kwargs.setdefault("GeoIDSvc", result.getService("ISF_AFIIGeoIDSvc"))
    result.merge(ParticleBrokerSvcCfg(ConfigFlags, name, **kwargs))
    return result


# Generic Truth Service Configurations
def TruthServiceCfg(ConfigFlags, **kwargs):
    """Return the TruthService config flagged by Sim.TruthStrategy"""
    stratmap = {
        "": BlankTruthServiceCfg,
        "MC12": MC12TruthServiceCfg,
        "MC12LLP": MC12LLPTruthServiceCfg,
        "MC12Plus": MC12PlusTruthServiceCfg,
        "MC15": MC15TruthServiceCfg,
        "MC15a": MC15aTruthServiceCfg,
        "MC15aPlus": MC15aPlusTruthServiceCfg,
        "MC15aPlusLLP": MC15aPlusLLPTruthServiceCfg,
        "MC16": MC16TruthServiceCfg,
        "MC16LLP": MC16LLPTruthServiceCfg,
        "MC18": MC18TruthServiceCfg,
        "MC18LLP": MC18LLPTruthServiceCfg,
        "Validation": ValidationTruthServiceCfg,
    }
    xCfg = stratmap[ConfigFlags.Sim.TruthStrategy]
    return xCfg(ConfigFlags, **kwargs)


def GenericTruthServiceCfg(ConfigFlags, name="ISF_TruthService", **kwargs):
    result = BarcodeSvcCfg(ConfigFlags)
    kwargs.setdefault("BarcodeSvc", result.getPrimary())

    kwargs.setdefault("SkipIfNoChildren", True)
    kwargs.setdefault("SkipIfNoParentBarcode", True)
    kwargs.setdefault("ForceEndVtxInRegions", [])

    # Mimics old configuration, which is marked as `#FIXME this should be set in a nicer way."
    long_lived_simulators = ["LongLived", "longLived", "QS"]
    simulator = ConfigFlags.Sim.ISF.Simulator
    is_long_lived_simulation = any(x in simulator for x in long_lived_simulators)
    if is_long_lived_simulation:
        kwargs.setdefault("QuasiStableParticlesIncluded", True)

    svc = CompFactory.ISF.TruthSvc(name, **kwargs)
    result.addService(svc, primary=True)
    return result


def ValidationTruthServiceCfg(ConfigFlags, name="ISF_ValidationTruthService", **kwargs):
    kwargs.setdefault("TruthStrategies", ["ISF_ValidationTruthStrategy"] )
    kwargs.setdefault("IgnoreUndefinedBarcodes", True)
    kwargs.setdefault("PassWholeVertices", True)
    return GenericTruthServiceCfg(ConfigFlags, name, **kwargs)


# MC12 Truth Service Configurations
def MC12BeamPipeTruthStrategies():
    return ["ISF_MCTruthStrategyGroupID"]


def MC12IDTruthStrategies():
    return ["ISF_MCTruthStrategyGroupID", "ISF_MCTruthStrategyGroupIDHadInt"]


def MC12CaloTruthStrategies():
    return ["ISF_MCTruthStrategyGroupCaloMuBrem"]


def MC12MSTruthStrategies():
    return []


def MC12TruthServiceCfg(ConfigFlags, name="ISF_MC12TruthService", **kwargs):
    result = ComponentAccumulator()
    if "TruthStrategies" not in kwargs:
        truthCfgs = [
            TruthStrategyGroupIDCfg,
            TruthStrategyGroupIDHadIntCfg,
            TruthStrategyGroupCaloMuBremCfg,
        ]
        truthStrats = [result.popToolsAndMerge(cfg(ConfigFlags)) for cfg in truthCfgs]
        kwargs.setdefault("TruthStrategies", truthStrats)
    kwargs.setdefault("IgnoreUndefinedBarcodes", False)
    kwargs.setdefault("PassWholeVertices", True)
    truthService = GenericTruthServiceCfg(ConfigFlags, name, **kwargs)
    result.addService(truthService.getPrimary(), primary=True)
    result.merge(truthService)
    return result


def BlankTruthServiceCfg(ConfigFlags, name="ISF_TruthService", **kwargs):
    if ConfigFlags.ISF.ValidationMode:
        return ValidationTruthServiceCfg(ConfigFlags, name, **kwargs)
    else:
        return MC12TruthServiceCfg(ConfigFlags, name, **kwargs)


def MC12LLPTruthServiceCfg(ConfigFlags, name="ISF_MC12TruthLLPService", **kwargs):
    result = ComponentAccumulator()
    truthCfgs = [
        TruthStrategyGroupIDCfg,
        TruthStrategyGroupIDHadIntCfg,
        TruthStrategyGroupCaloMuBremCfg,
        LLPTruthStrategyCfg,
    ]
    truthStrats = [result.popToolsAndMerge(cfg(ConfigFlags)) for cfg in truthCfgs]
    kwargs.setdefault("TruthStrategies", truthStrats)
    result.merge(MC12TruthServiceCfg(ConfigFlags, name, **kwargs))
    return result


def MC12PlusTruthServiceCfg(ConfigFlags, name="ISF_MC12PlusTruthService", **kwargs):
    AtlasRegion = ROOT.AtlasDetDescr
    kwargs.setdefault("ForceEndVtxInRegions", [AtlasRegion.fAtlasID] )
    return MC12TruthServiceCfg(ConfigFlags, name, **kwargs)


# MC15 Truth Service Configurations
def MC15BeamPipeTruthStrategies():
    return ["ISF_MCTruthStrategyGroupID_MC15"]


def MC15IDTruthStrategies():
    return ["ISF_MCTruthStrategyGroupID_MC15", "ISF_MCTruthStrategyGroupIDHadInt_MC15"]


def MC15CaloTruthStrategies():
    return ["ISF_MCTruthStrategyGroupCaloMuBrem", "ISF_MCTruthStrategyGroupCaloMuBrem_MC15"]

def MC15MSTruthStrategies():
    return []


def MC15TruthServiceCfg(ConfigFlags, name="ISF_MC15TruthService", **kwargs):
    result = ComponentAccumulator()
    AtlasRegion = ROOT.AtlasDetDescr

    if "TruthStrategies" not in kwargs:
        truthCfgs = [
            TruthStrategyGroupID_MC15Cfg,
            TruthStrategyGroupIDHadInt_MC15Cfg,
            TruthStrategyGroupCaloMuBrem_MC15Cfg,
            TruthStrategyGroupCaloDecay_MC15Cfg,
        ]
        truthStrats = [result.popToolsAndMerge(cfg(ConfigFlags)) for cfg in truthCfgs]
        kwargs.setdefault("TruthStrategies", truthStrats)

    kwargs.setdefault("IgnoreUndefinedBarcodes", False)
    kwargs.setdefault("PassWholeVertices", False) # new for MC15 - can write out partial vertices.
    kwargs.setdefault("ForceEndVtxInRegions", [AtlasRegion.fAtlasID])
    truthService = GenericTruthServiceCfg(ConfigFlags, name, **kwargs)
    result.addService(truthService.getPrimary(), primary=True)
    result.merge(truthService)
    return result


def MC15aTruthServiceCfg(ConfigFlags, name="ISF_MC15aTruthService", **kwargs):
    kwargs.setdefault("ForceEndVtxInRegions", [])
    return MC15TruthServiceCfg(ConfigFlags, name, **kwargs)


def MC15aPlusTruthServiceCfg(ConfigFlags, name="ISF_MC15aPlusTruthService", **kwargs):
    AtlasRegion = ROOT.AtlasDetDescr

    kwargs.setdefault("ForceEndVtxInRegions", [AtlasRegion.fAtlasID])
    result = MC15TruthServiceCfg(ConfigFlags, name, **kwargs)
    return result


def MC15aPlusLLPTruthServiceCfg(ConfigFlags, name="ISF_MC15aPlusLLPTruthService", **kwargs):
    result = ComponentAccumulator()
    truthCfgs = [
        KeepLLPDecayChildrenStrategyCfg,
        KeepLLPHadronicInteractionChildrenStrategyCfg,
        TruthStrategyGroupID_MC15Cfg,
        TruthStrategyGroupIDHadInt_MC15Cfg,
        TruthStrategyGroupCaloMuBrem_MC15Cfg,
        TruthStrategyGroupCaloDecay_MC15Cfg,
        LLPTruthStrategyCfg,
    ]
    truthStrats = [result.popToolsAndMerge(cfg(ConfigFlags)) for cfg in truthCfgs]
    kwargs.setdefault("TruthStrategies", truthStrats)
    return MC15aPlusTruthServiceCfg(ConfigFlags, name, **kwargs)


# MC16 Truth Service Configurations
def MC16TruthServiceCfg(ConfigFlags, name="ISF_MC16TruthService", **kwargs):
    return MC15aPlusTruthServiceCfg(ConfigFlags, name, **kwargs)


def MC16LLPTruthServiceCfg(ConfigFlags, name="ISF_MC16LLPTruthService", **kwargs):
    return MC15aPlusLLPTruthServiceCfg(ConfigFlags, name, **kwargs)


# MC18 Truth Service Configurations
def MC18TruthServiceCfg(ConfigFlags, name="ISF_MC18TruthService", **kwargs):
    return MC15aPlusTruthServiceCfg(ConfigFlags, name, **kwargs)


def MC18LLPTruthServiceCfg(ConfigFlags, name="ISF_MC18LLPTruthService", **kwargs):
    return MC15aPlusLLPTruthServiceCfg(ConfigFlags, name, **kwargs)
