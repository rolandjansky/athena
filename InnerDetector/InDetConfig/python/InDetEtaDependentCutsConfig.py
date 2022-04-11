# Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
# Configuration of InDetEtaDependentCuts package
from AthenaConfiguration.ComponentAccumulator import ComponentAccumulator
from AthenaConfiguration.ComponentFactory import CompFactory
import AthenaCommon.SystemOfUnits as Units

def ITkEtaDependentCutsSvcCfg(flags, name = 'ITkEtaDependentCutsSvc', **kwargs):
    acc = ComponentAccumulator()

    cuts = flags.ITk.Tracking.ActivePass

    kwargs.setdefault("etaBins",              cuts.etaBins)
    kwargs.setdefault("etaWidthBrem",         cuts.etaWidthBrem)
    kwargs.setdefault("maxdImpactSSSSeeds",   cuts.maxdImpactSSSSeeds)
    kwargs.setdefault("maxDoubleHoles",       cuts.maxDoubleHoles)
    kwargs.setdefault("maxHoles",             cuts.maxHoles)
    kwargs.setdefault("maxPixelHoles",        cuts.maxPixelHoles)
    kwargs.setdefault("maxPrimaryImpact",     cuts.maxPrimaryImpact)
    kwargs.setdefault("maxSctHoles",          cuts.maxSctHoles)
    kwargs.setdefault("maxShared",            cuts.maxShared)
    kwargs.setdefault("maxZImpact",           cuts.maxZImpact)
    kwargs.setdefault("minClusters",          cuts.minClusters)
    kwargs.setdefault("minPixelHits",         cuts.minPixel)
    kwargs.setdefault("minPT",                cuts.minPT)
    kwargs.setdefault("minPTBrem",            cuts.minPTBrem)
    kwargs.setdefault("minSiNotShared",       cuts.minSiNotShared)
    kwargs.setdefault("maxHolesGapPattern",   cuts.nHolesGapMax)
    kwargs.setdefault("maxHolesPattern",      cuts.nHolesMax)
    kwargs.setdefault("nWeightedClustersMin", cuts.nWeightedClustersMin)
    kwargs.setdefault("phiWidthBrem",         cuts.phiWidthBrem)
    kwargs.setdefault("Xi2max",               cuts.Xi2max)
    kwargs.setdefault("Xi2maxNoAdd",          cuts.Xi2maxNoAdd)

    ITkEtaDependentCutsSvc = CompFactory.InDet.InDetEtaDependentCutsSvc( name = name + flags.ITk.Tracking.ActivePass.extension, **kwargs )
    acc.addService(ITkEtaDependentCutsSvc)
    return acc

def IDEtaDependentCuts_IPXD_SvcCfg(flags, name="IDEtaDepCutsSvc_IPXD", **kwargs):
    acc = ComponentAccumulator()

    InDetEtaDependentCutsSvc = CompFactory.InDet.InDetEtaDependentCutsSvc( name, **kwargs )

    InDetEtaDependentCutsSvc.etaBins                 = [-1.0, 9999.]
    InDetEtaDependentCutsSvc.minPT                   = [1.0 * Units.GeV]
    InDetEtaDependentCutsSvc.maxPrimaryImpact        = [1.0 * Units.mm]
    InDetEtaDependentCutsSvc.maxZImpact              = [1.5 * Units.mm]
    InDetEtaDependentCutsSvc.minPixelHits            = [1]
    InDetEtaDependentCutsSvc.minClusters             = [7]
    #Unused
    InDetEtaDependentCutsSvc.etaWidthBrem            = [-1]
    InDetEtaDependentCutsSvc.phiWidthBrem            = [-1]
    InDetEtaDependentCutsSvc.maxdImpactSSSSeeds      = [-1]
    InDetEtaDependentCutsSvc.maxDoubleHoles          = [-1]
    InDetEtaDependentCutsSvc.maxHoles                = [-1]
    InDetEtaDependentCutsSvc.maxSctHoles             = [-1]
    InDetEtaDependentCutsSvc.maxShared               = [-1]
    InDetEtaDependentCutsSvc.minInnermostPixelHits   = [-1]
    InDetEtaDependentCutsSvc.minStripHits            = [-1]
    InDetEtaDependentCutsSvc.minPTBrem               = [-1]
    InDetEtaDependentCutsSvc.minSiNotShared          = [-1]
    InDetEtaDependentCutsSvc.maxHolesGapPattern      = [-1]
    InDetEtaDependentCutsSvc.maxHolesPattern         = [-1]
    InDetEtaDependentCutsSvc.maxPixelHoles           = [-1]
    InDetEtaDependentCutsSvc.nWeightedClustersMin    = [-1]
    InDetEtaDependentCutsSvc.Xi2max                  = [-1]
    InDetEtaDependentCutsSvc.Xi2maxNoAdd             = [-1]

    acc.addService(InDetEtaDependentCutsSvc)
    return acc


def IDEtaDependentCuts_SV1_SvcCfg(flags, name="IDEtaDepCutsSvc_SV1", **kwargs):
    acc = ComponentAccumulator()

    InDetEtaDependentCutsSvc = CompFactory.InDet.InDetEtaDependentCutsSvc( name, **kwargs )

    InDetEtaDependentCutsSvc.etaBins                 = [-1.0, 1.65, 2.0, 9999.]
    InDetEtaDependentCutsSvc.minPT                   = [0.7 * Units.GeV]
    InDetEtaDependentCutsSvc.maxPrimaryImpact        = [5.0 * Units.mm]
    InDetEtaDependentCutsSvc.maxZImpact              = [3.0 * Units.mm]
    InDetEtaDependentCutsSvc.minPixelHits            = [1, 1, 2]
    InDetEtaDependentCutsSvc.minClusters             = [7]
    InDetEtaDependentCutsSvc.maxShared               = [1000]
    InDetEtaDependentCutsSvc.Xi2max                  = [3.0]
    #Unused
    InDetEtaDependentCutsSvc.etaWidthBrem            = [-1]
    InDetEtaDependentCutsSvc.phiWidthBrem            = [-1]
    InDetEtaDependentCutsSvc.maxdImpactSSSSeeds      = [-1]
    InDetEtaDependentCutsSvc.maxDoubleHoles          = [-1]
    InDetEtaDependentCutsSvc.maxHoles                = [-1]
    InDetEtaDependentCutsSvc.maxPixelHoles           = [-1]
    InDetEtaDependentCutsSvc.maxSctHoles             = [-1]
    InDetEtaDependentCutsSvc.minStripHits            = [-1]
    InDetEtaDependentCutsSvc.minInnermostPixelHits   = [-1]
    InDetEtaDependentCutsSvc.minPTBrem               = [-1]
    InDetEtaDependentCutsSvc.minSiNotShared          = [-1]
    InDetEtaDependentCutsSvc.maxHolesGapPattern      = [-1]
    InDetEtaDependentCutsSvc.maxHolesPattern         = [-1]
    InDetEtaDependentCutsSvc.nWeightedClustersMin    = [-1]
    InDetEtaDependentCutsSvc.Xi2maxNoAdd             = [-1]

    acc.addService(InDetEtaDependentCutsSvc)
    return acc
