# Copyright (C) 2020 CERN for the benefit of the ATLAS collaboration 

from   InDetEtaDependentCuts.InDetEtaDependentCutsConf import InDet__InDetEtaDependentCutsSvc


def ConfiguredEtaDependentCuts(NewTrackingCuts = None):
      
   InDetEtaDependentCutsSvc = InDet__InDetEtaDependentCutsSvc(name                    = "InDetEtaDependentCutsSvc"+NewTrackingCuts.mode(),
                                                              etaBins                 = NewTrackingCuts.etaBins(),
                                                              etaWidthBrem            = NewTrackingCuts.etaWidthBrem(),
                                                              maxdImpactSSSSeeds      = NewTrackingCuts.maxdImpactSSSSeeds(),
                                                              maxDoubleHoles          = NewTrackingCuts.maxDoubleHoles(),
                                                              maxHoles                = NewTrackingCuts.maxHoles(),
                                                              maxPixelHoles           = NewTrackingCuts.maxPixelHoles(),
                                                              maxPrimaryImpact        = NewTrackingCuts.maxPrimaryImpact(),
                                                              maxSctHoles             = NewTrackingCuts.maxSCTHoles(),
                                                              maxShared               = NewTrackingCuts.maxShared(),
                                                              maxZImpact              = NewTrackingCuts.maxZImpact(),
                                                              minClusters             = NewTrackingCuts.minClusters(),
                                                              minPixelHits            = NewTrackingCuts.minPixel(),
                                                              minPT                   = NewTrackingCuts.minPT(),
                                                              minPTBrem               = NewTrackingCuts.minPTBrem(),
                                                              minSiNotShared          = NewTrackingCuts.minSiNotShared(),
                                                              maxHolesGapPattern      = NewTrackingCuts.nHolesGapMax(),
                                                              maxHolesPattern         = NewTrackingCuts.nHolesMax(),
                                                              nWeightedClustersMin    = NewTrackingCuts.nWeightedClustersMin(),
                                                              phiWidthBrem            = NewTrackingCuts.phiWidthBrem(),
                                                              Xi2max                  = NewTrackingCuts.Xi2max(),
                                                              Xi2maxNoAdd             = NewTrackingCuts.Xi2maxNoAdd() )


   return InDetEtaDependentCutsSvc
