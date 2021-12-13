# Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration

from AthenaConfiguration.ComponentAccumulator import ComponentAccumulator
from AthenaConfiguration.ComponentFactory import CompFactory
import AthenaCommon.SystemOfUnits as Units

def IDEtaDependentCuts_IPXD_SvcCfg(flags, name, **kwargs):
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


def IDEtaDependentCuts_SV1_SvcCfg(flags, name, **kwargs):
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
