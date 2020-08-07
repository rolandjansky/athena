from InDetEtaDependentCuts.InDetEtaDependentCutsConf import InDet__InDetEtaDependentCutsSvc
import AthenaCommon.SystemOfUnits as Units

# Selections applied on top of
# https://gitlab.cern.ch/atlas/athena/-/blob/21.9/InnerDetector/InDetExample/InDetRecExample/python/ConfiguredNewTrackingCuts.py#L266

def IDEtaDepCutsSvc_IPXD(name):
    InDetEtaDependentCutsSvc = InDet__InDetEtaDependentCutsSvc( name )
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

    return InDetEtaDependentCutsSvc



def IDEtaDepCutsSvc_InDetVKalVxInJetFinder(name):
    InDetEtaDependentCutsSvc = InDet__InDetEtaDependentCutsSvc( name )
    InDetEtaDependentCutsSvc.etaBins                 = [-1.0, 1.65, 2.0, 9999.]
    InDetEtaDependentCutsSvc.minPT                   = [0.7 * Units.GeV]
    InDetEtaDependentCutsSvc.maxPrimaryImpact        = [5.0 * Units.mm]
    InDetEtaDependentCutsSvc.maxZImpact              = [25.0 * Units.mm]
    InDetEtaDependentCutsSvc.minPixelHits            = [1, 1, 2]
    InDetEtaDependentCutsSvc.minStripHits            = [4, 5, 6]
    InDetEtaDependentCutsSvc.minClusters             = [7]
    InDetEtaDependentCutsSvc.Xi2max                  = [3.0]
    #Unused
    InDetEtaDependentCutsSvc.etaWidthBrem            = [-1]
    InDetEtaDependentCutsSvc.phiWidthBrem            = [-1]
    InDetEtaDependentCutsSvc.maxdImpactSSSSeeds      = [-1]
    InDetEtaDependentCutsSvc.maxDoubleHoles          = [-1]
    InDetEtaDependentCutsSvc.maxHoles                = [-1]
    InDetEtaDependentCutsSvc.maxPixelHoles           = [-1]
    InDetEtaDependentCutsSvc.maxSctHoles             = [-1]
    InDetEtaDependentCutsSvc.maxShared               = [-1]
    InDetEtaDependentCutsSvc.minInnermostPixelHits   = [-1]
    InDetEtaDependentCutsSvc.minPTBrem               = [-1]
    InDetEtaDependentCutsSvc.minSiNotShared          = [-1]
    InDetEtaDependentCutsSvc.maxHolesGapPattern      = [-1]
    InDetEtaDependentCutsSvc.maxHolesPattern         = [-1]
    InDetEtaDependentCutsSvc.nWeightedClustersMin    = [-1]
    InDetEtaDependentCutsSvc.Xi2maxNoAdd             = [-1]

    return InDetEtaDependentCutsSvc
