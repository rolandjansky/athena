#
#  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
#

'''@file TrackingConfigurationWorkaround.py
@author Peter Onyisi
@date 2019-12-17
@brief Centralize the DQ workarounds for setting up ID tracking tools in new-style config, until this is provided centrally
'''

def TrackSummaryToolWorkaround(flags):
    from AthenaConfiguration.ComponentFactory import CompFactory
    from AthenaConfiguration.ComponentAccumulator import ComponentAccumulator
    result = ComponentAccumulator()
    ############################## WORKAROUND (START) ##########################
    ############################## TO RUN TRACKSUMMARYTOOL #####################

    # Taken from InnerDetector/InDetDigitization/PixelDigitization/python/PixelDigitizationConfigNew.py
    from PixelConditionsAlgorithms.PixelConditionsConfig import PixelConfigCondAlgCfg
    result.merge(PixelConfigCondAlgCfg(flags))

    from PixelConditionsTools.PixelConditionsSummaryConfig import PixelConditionsSummaryCfg
    InDetPixelConditionsSummaryTool = result.popToolsAndMerge(PixelConditionsSummaryCfg(flags))

    # Taken from Tracking/TrkConfig/python/AtlasExtrapolatorConfig.py
    # AtlasExtrapolatorConfig can give only private extrapolator. We need public extrapolator.
    from TrkConfig.AtlasTrackingGeometrySvcConfig import TrackingGeometrySvcCfg
    trackGeomCfg = TrackingGeometrySvcCfg(flags)
    geom_svc = trackGeomCfg.getPrimary() 
    geom_svc.GeometryBuilder.Compactify = False ######## To avoid crash ########
    result.merge(trackGeomCfg)
    from MagFieldServices.MagFieldServicesConfig import MagneticFieldSvcCfg
    result.merge(MagneticFieldSvcCfg(flags))
    AtlasNavigator = CompFactory.Trk.Navigator(name = 'AtlasNavigator')
    AtlasNavigator.TrackingGeometrySvc = geom_svc
    result.addPublicTool(AtlasNavigator) 

    # Taken from InnerDetector/InDetExample/InDetRecExample/share/InDetRecLoadTools.py
    InDetPropagator = CompFactory.Trk.RungeKuttaPropagator(name = 'InDetPropagator')
    InDetPropagator.AccuracyParameter = 0.0001
    InDetPropagator.MaxStraightLineStep = .004
    result.addPublicTool(InDetPropagator)
    InDetMaterialUpdator = CompFactory.Trk.MaterialEffectsUpdator(name = "InDetMaterialEffectsUpdator")
    result.addPublicTool(InDetMaterialUpdator)
    InDetSubPropagators = []
    InDetSubUpdators    = []
    # -------------------- set it depending on the geometry ----------------------------------------------------
    # default for ID is (Rk,Mat)
    InDetSubPropagators += [ InDetPropagator.name ]
    InDetSubUpdators    += [ InDetMaterialUpdator.name ]
    # default for Calo is (Rk,MatLandau)
    InDetSubPropagators += [ InDetPropagator.name ]
    InDetSubUpdators    += [ InDetMaterialUpdator.name ]
    # default for MS is (STEP,Mat)
    #InDetSubPropagators += [ InDetStepPropagator.name ]
    InDetSubUpdators    += [ InDetMaterialUpdator.name ]
    #from TrkExTools.TrkExToolsConf import Trk__Extrapolator
    InDetExtrapolator = CompFactory.Trk.Extrapolator(name                    = 'InDetExtrapolator',
                                          Propagators             = [ InDetPropagator ],
                                          MaterialEffectsUpdators = [ InDetMaterialUpdator ],
                                          Navigator               = AtlasNavigator,
                                          SubPropagators          = InDetSubPropagators,
                                          SubMEUpdators           = InDetSubUpdators)
    result.addPublicTool(InDetExtrapolator)
    InDetTestPixelLayerTool = CompFactory.InDet.InDetTestPixelLayerTool(name = "InDetTestPixelLayerTool",
                                                             PixelSummaryTool = InDetPixelConditionsSummaryTool,
                                                             CheckActiveAreas=True,
                                                             CheckDeadRegions=True,
                                                             CheckDisabledFEs=True)
    result.addPublicTool(InDetTestPixelLayerTool)
    InDetBoundaryCheckTool = CompFactory.InDet.InDetBoundaryCheckTool(
        name="InDetBoundaryCheckTool",
        UsePixel=flags.Detector.GeometryPixel,
        UseSCT=flags.Detector.GeometrySCT,
        PixelLayerTool=InDetTestPixelLayerTool
    )
    result.addPublicTool(InDetBoundaryCheckTool)
    InDetHoleSearchTool = CompFactory.InDet.InDetTrackHoleSearchTool(name = "InDetHoleSearchTool",
                                                          Extrapolator = InDetExtrapolator,
                                                          CountDeadModulesAfterLastHit = True,
                                                          BoundaryCheckTool=InDetBoundaryCheckTool)
    result.addPublicTool(InDetHoleSearchTool)
    InDetPrdAssociationTool = CompFactory.InDet.InDetPRD_AssociationToolGangedPixels(name                           = "InDetPrdAssociationTool_setup",
                                                                          PixelClusterAmbiguitiesMapName = "PixelClusterAmbiguitiesMap",
                                                                          SetupCorrect                   = True,
                                                                          addTRToutliers                 = True)
    result.addPublicTool(InDetPrdAssociationTool)
    InDetTrackSummaryHelperTool = CompFactory.InDet.InDetTrackSummaryHelperTool(name            = "InDetSummaryHelper",
                                                                     AssoTool        = InDetPrdAssociationTool,
                                                                     PixelToTPIDTool = '',
                                                                     TestBLayerTool  = '',
                                                                     RunningTIDE_Ambi = True,
                                                                     DoSharedHits    = False,
                                                                     HoleSearch      = InDetHoleSearchTool,
                                                                     usePixel        = flags.Detector.GeometryPixel,
                                                                     useSCT          = flags.Detector.GeometrySCT,
                                                                     useTRT          = flags.Detector.GeometryTRT)
    InDetTrackSummaryTool = CompFactory.Trk.TrackSummaryTool(name = "InDetTrackSummaryTool",
                                                  InDetSummaryHelperTool = InDetTrackSummaryHelperTool,
                                                  doSharedHits           = False,
                                                  doHolesInDet           = True,
                                                  TRT_ElectronPidTool    = '',
                                                  TRT_ToT_dEdxTool       = '',
                                                  PixelToTPIDTool        = '')
    result.setPrivateTools(InDetTrackSummaryTool)
    ############################## WORKAROUND (END) ############################

    # To run job only with ID
    if hasattr(flags, "Detector") and hasattr(flags.Detector, "GeometryMuon") and hasattr(flags.Detector, "GeometryID"):
        TrkEventCnvSuperTool = CompFactory.Trk.EventCnvSuperTool(name = "EventCnvSuperTool",
                                                                 DoMuons = flags.Detector.GeometryMuon,
                                                                 DoID = flags.Detector.GeometryID)
        result.addPublicTool(TrkEventCnvSuperTool)

    return result
