"""ComponentAccumulator service configuration for ISF

Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
"""
from AthenaConfiguration.ComponentAccumulator import ComponentAccumulator
from AthenaConfiguration.ComponentFactory import CompFactory
from AthenaConfiguration.Enums import BeamType
from G4AtlasApps.SimEnums import CavernBackground, TruthStrategy
from ISF_HepMC_Tools.ISF_HepMC_ToolsConfigNew import (
    ParticleFinalStateFilterCfg, ParticlePositionFilterDynamicCfg,
    EtaPhiFilterCfg, GenParticleInteractingFilterCfg,
    KeepLLPDecayChildrenStrategyCfg,
    KeepLLPHadronicInteractionChildrenStrategyCfg,
    TruthStrategyGroupID_MC15Cfg,
    TruthStrategyGroupIDHadInt_MC15Cfg,
    #TruthStrategyGroupCaloMuBrem_MC15Cfg,
    TruthStrategyGroupCaloDecay_MC15Cfg,
    LLPTruthStrategyCfg,
    TruthStrategyGroupIDCfg,
    TruthStrategyGroupIDHadIntCfg,
    TruthStrategyGroupCaloMuBremCfg,
    ParticleSimWhiteList_ExtraParticlesCfg,
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
        if ConfigFlags.Beam.Type is not BeamType.Cosmics:
            acc = ParticlePositionFilterDynamicCfg(ConfigFlags)
            genParticleFilterList += [result.popToolsAndMerge(acc)]
            if not (ConfigFlags.Detector.GeometryAFP or ConfigFlags.Detector.GeometryALFA or ConfigFlags.Detector.GeometryFwdRegion) \
                and not ConfigFlags.Detector.GeometryCavern \
                and ConfigFlags.Sim.CavernBackground in [CavernBackground.Off, CavernBackground.Signal]:
                acc = EtaPhiFilterCfg(ConfigFlags)
                genParticleFilterList += [result.popToolsAndMerge(acc)]
    acc = GenParticleInteractingFilterCfg(ConfigFlags)
    genParticleFilterList += [result.popToolsAndMerge(acc)]
    result.setPrivateTools(genParticleFilterList)
    return result


def InputConverterCfg(ConfigFlags, name="ISF_InputConverter", **kwargs):
    result = ComponentAccumulator()
    kwargs.setdefault("BarcodeSvc", result.getPrimaryAndMerge(BarcodeSvcCfg(ConfigFlags)).name)
    kwargs.setdefault("UseGeneratedParticleMass", False)
    if "GenParticleFilters" not in kwargs:
        acc_GenParticleFiltersList = GenParticleFiltersToolCfg(ConfigFlags)
        kwargs.setdefault("GenParticleFilters", result.popToolsAndMerge(acc_GenParticleFiltersList) )
    result.addService(CompFactory.ISF.InputConverter(name, **kwargs), primary = True)
    return result


def LongLivedInputConverterCfg(ConfigFlags, name="ISF_LongLivedInputConverter", **kwargs):
    result = ComponentAccumulator()
    gpfilt = [
        result.popToolsAndMerge(ParticleSimWhiteList_ExtraParticlesCfg(ConfigFlags)),
        result.popToolsAndMerge(ParticlePositionFilterDynamicCfg(ConfigFlags)),
        result.popToolsAndMerge(EtaPhiFilterCfg(ConfigFlags)),
        result.popToolsAndMerge(GenParticleInteractingFilterCfg(ConfigFlags)),
    ]
    kwargs.setdefault("GenParticleFilters", gpfilt)
    kwargs.setdefault("QuasiStableParticlesIncluded", True)
    inptCnv = result.getPrimaryAndMerge(InputConverterCfg(ConfigFlags, name, **kwargs))
    result.addService(inptCnv, primary = True)
    return result


def ParticleBrokerSvcNoOrderingCfg(ConfigFlags, name="ISF_ParticleBrokerSvcNoOrdering", **kwargs):
    result = ComponentAccumulator()
    if "EntryLayerTool" not in kwargs:
        tool = result.popToolsAndMerge(EntryLayerToolCfg(ConfigFlags))
        result.addPublicTool(tool)
        kwargs.setdefault("EntryLayerTool", result.getPublicTool(tool.name))
        kwargs.setdefault("GeoIDSvc", result.getService("ISF_GeoIDSvc").name) # FIXME
    # assume "GeoIDSvc" has been set alongside "EntryLayerTool"
    kwargs.setdefault("AlwaysUseGeoIDSvc", False)
    kwargs.setdefault("ValidateGeoIDs", ConfigFlags.Sim.ISF.ValidationMode)
    kwargs.setdefault("ValidationOutput", ConfigFlags.Sim.ISF.ValidationMode)
    kwargs.setdefault("ValidationStreamName", "ParticleBroker")

    kwargs.setdefault("BarcodeService", result.getPrimaryAndMerge(BarcodeSvcCfg(ConfigFlags)).name)

    result.addService(CompFactory.ISF.ParticleBrokerDynamicOnReadIn(name, **kwargs), primary = True)
    return result


def ParticleBrokerSvcCfg(ConfigFlags, name="ISF_ParticleBrokerSvc", **kwargs):
    # comment copied from old config
    #kwargs.setdefault("ParticleOrderingTool", "ISF_InToOutSubDetOrderingTool")
    result = ComponentAccumulator()
    kwargs.setdefault("ParticleOrderingTool", result.popToolsAndMerge(ParticleOrderingToolCfg(ConfigFlags)))
    pbsvc = result.getPrimaryAndMerge(ParticleBrokerSvcNoOrderingCfg(ConfigFlags, name, **kwargs))
    result.addService(pbsvc, primary = True)
    return result


def AFIIParticleBrokerSvcCfg(ConfigFlags, name="ISF_AFIIParticleBrokerSvc", **kwargs):
    result = ComponentAccumulator()
    tool = result.popToolsAndMerge(AFIIEntryLayerToolCfg(ConfigFlags))
    result.addPublicTool(tool)

    kwargs.setdefault("EntryLayerTool", result.getPublicTool(tool.name))
    kwargs.setdefault("GeoIDSvc", result.getService("ISF_AFIIGeoIDSvc").name) # FIXME
    pbsvc = result.getPrimaryAndMerge(ParticleBrokerSvcCfg(ConfigFlags, name, **kwargs))
    result.addService(pbsvc, primary = True)
    return result


# Generic Truth Service Configurations
def TruthServiceCfg(ConfigFlags, **kwargs):
    """Return the TruthService config flagged by Sim.TruthStrategy"""
    stratmap = {
        TruthStrategy.MC12: MC12TruthServiceCfg,
        TruthStrategy.MC12LLP: MC12LLPTruthServiceCfg,
        TruthStrategy.MC12Plus: MC12PlusTruthServiceCfg,
        TruthStrategy.MC15: MC15TruthServiceCfg,
        TruthStrategy.MC15a: MC15aTruthServiceCfg,
        TruthStrategy.MC15aPlus: MC15aPlusTruthServiceCfg,
        TruthStrategy.MC15aPlusLLP: MC15aPlusLLPTruthServiceCfg,
        TruthStrategy.MC16: MC16TruthServiceCfg,
        TruthStrategy.MC16LLP: MC16LLPTruthServiceCfg,
        TruthStrategy.MC18: MC18TruthServiceCfg,
        TruthStrategy.MC18LLP: MC18LLPTruthServiceCfg,
        # TruthStrategy.PhysicsProcess: PhysicsProcessTruthServiceCfg,
        # TruthStrategy.Global: GlobalTruthServiceCfg,
        TruthStrategy.Validation: ValidationTruthServiceCfg,
        # TruthStrategy.Cosmic: CosmicTruthServiceCfg,
    }
    xCfg = stratmap[ConfigFlags.Sim.TruthStrategy]
    return xCfg(ConfigFlags, **kwargs)


def GenericTruthServiceCfg(ConfigFlags, name="ISF_TruthService", **kwargs):
    result = ComponentAccumulator()
    kwargs.setdefault("BarcodeSvc", result.getPrimaryAndMerge(BarcodeSvcCfg(ConfigFlags)).name)

    kwargs.setdefault("SkipIfNoChildren", True)
    kwargs.setdefault("SkipIfNoParentBarcode", True)
    kwargs.setdefault("ForceEndVtxInRegions", [])

    if ConfigFlags.Sim.ISF.Simulator.isQuasiStable():
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
    truthService = result.getPrimaryAndMerge(GenericTruthServiceCfg(ConfigFlags, name, **kwargs))
    result.addService(truthService, primary=True)
    return result


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
    truthService = result.getPrimaryAndMerge(MC12TruthServiceCfg(ConfigFlags, name, **kwargs))
    result.addService(truthService, primary = True)
    return result


def MC12PlusTruthServiceCfg(ConfigFlags, name="ISF_MC12PlusTruthService", **kwargs):
    AtlasRegion = ROOT.AtlasDetDescr.AtlasRegion
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
    AtlasRegion = ROOT.AtlasDetDescr.AtlasRegion

    if "TruthStrategies" not in kwargs:
        truthCfgs = [
            TruthStrategyGroupID_MC15Cfg,
            TruthStrategyGroupIDHadInt_MC15Cfg,
            TruthStrategyGroupCaloMuBremCfg, # FIXME - should be TruthStrategyGroupCaloMuBrem_MC15Cfg but keeping this for consistency with old style
            TruthStrategyGroupCaloDecay_MC15Cfg ]
        truthStrats = [result.popToolsAndMerge(cfg(ConfigFlags)) for cfg in truthCfgs]
        kwargs.setdefault("TruthStrategies", truthStrats)

    kwargs.setdefault("IgnoreUndefinedBarcodes", False)
    kwargs.setdefault("PassWholeVertices", False) # new for MC15 - can write out partial vertices.
    kwargs.setdefault("ForceEndVtxInRegions", [AtlasRegion.fAtlasID])
    truthService = result.getPrimaryAndMerge(GenericTruthServiceCfg(ConfigFlags, name, **kwargs))
    result.addService(truthService, primary=True)
    return result


def MC15aTruthServiceCfg(ConfigFlags, name="ISF_MC15aTruthService", **kwargs):
    kwargs.setdefault("ForceEndVtxInRegions", [])
    return MC15TruthServiceCfg(ConfigFlags, name, **kwargs)


def MC15aPlusTruthServiceCfg(ConfigFlags, name="ISF_MC15aPlusTruthService", **kwargs):
    AtlasRegion = ROOT.AtlasDetDescr.AtlasRegion

    kwargs.setdefault("ForceEndVtxInRegions", [AtlasRegion.fAtlasID])
    return MC15TruthServiceCfg(ConfigFlags, name, **kwargs)


def MC15aPlusLLPTruthServiceCfg(ConfigFlags, name="ISF_MC15aPlusLLPTruthService", **kwargs):
    result = ComponentAccumulator()
    truthCfgs = [
        KeepLLPDecayChildrenStrategyCfg,
        KeepLLPHadronicInteractionChildrenStrategyCfg,
        TruthStrategyGroupID_MC15Cfg,
        TruthStrategyGroupIDHadInt_MC15Cfg,
        TruthStrategyGroupCaloMuBremCfg, # FIXME - should be TruthStrategyGroupCaloDecay_MC15Cfg but keeping this for consistency with old style
        TruthStrategyGroupCaloDecay_MC15Cfg,
        LLPTruthStrategyCfg,
    ]
    truthStrats = [result.popToolsAndMerge(cfg(ConfigFlags)) for cfg in truthCfgs]
    kwargs.setdefault("TruthStrategies", truthStrats)
    truthService = result.getPrimaryAndMerge(MC15aPlusTruthServiceCfg(ConfigFlags, name, **kwargs))
    result.addService(truthService, primary = True)
    return result


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
