#
#  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
#

def PixelMonitoringConfig(flags):
    from AthenaConfiguration.ComponentAccumulator import ComponentAccumulator
    acc = ComponentAccumulator()

    # run on RAW only
    if flags.DQ.Environment in ('online', 'tier0', 'tier0Raw'):
        kwargsHitMonTool = { 'doOnline'        : flags.Common.isOnline,      #Histograms for online (athenaPT) running
                             'doModules'       : flags.Common.isOnline, #Turn on/off the sets of 1744 module histograms (for dqmf)
                             'doLumiBlock'     : not flags.Common.isOnline,       #Turn on/off histograms stored for each lumi block
                             'doLowOccupancy'  : False,      #Turn on/off histograms with binning for cosmics/single beam                    
                             'doHighOccupancy' : True,       #Turn on/off histograms with binning for collisions
                             #'doHeavyIonMon'   : InDetFlags.doHeavyIon(),   # Histogram modification for heavy ion monitoring
                             #'RDOName'         : InDetKeys.PixelRDOs()
                             'doHeavyIonMon'   : False,      #Until new config ready
                             'RDOName'         : 'PixelRDOs',#Until new config ready
        }

        kwargsClusMonTool = { 'doOnline'        : flags.Common.isOnline,      #Histograms for online (athenaPT) running
                              'doModules'       : flags.Common.isOnline, #Turn on/off the sets of 1744 module histograms (for dqmf)
                              'doLumiBlock'     : not flags.Common.isOnline,       #Turn on/off histograms stored for each lumi block
                              'doLowOccupancy'  : False,      #Turn on/off histograms with binning for cosmics/single beam
                              'doHighOccupancy' : True,       #Turn on/off histograms with binning for collisions
                              #'doHeavyIonMon'   : InDetFlags.doHeavyIon(),   # Histogram modification for heavy ion monitoring
                              'doHeavyIonMon'   : False,   #Until new config ready
                              #                  'doStatus'        : True,       #Turn on/off histograms with Module Status information
                              #'ClusterName'     : InDetKeys.PixelClusters(),
                              'ClusterName'     : 'PixelClusters', #Until new config ready
                              #'TrackName'       : InDetKeys.Tracks()
                              'TrackName'       : 'Tracks', #Until new config ready
        }

        kwargsErrMonTool = { 'doOnline'        : flags.Common.isOnline,      #Histograms for online (athenaPT) running
                             'doModules'       : flags.Common.isOnline, #Turn on/off the sets of 1744 module histograms (for dqmf)
                             'doLumiBlock'     : not flags.Common.isOnline,       #Turn on/off histograms stored for each lumi block
                             'doLowOccupancy'  : False,      #Turn on/off histograms with binning for cosmics/single beam                    
                             'doHighOccupancy' : True,       #Turn on/off histograms with binning for collisions
                             #'doHeavyIonMon'   : InDetFlags.doHeavyIon()
                             'doHeavyIonMon'   : False, #Until new config is ready
        }
        
        from AthenaMonitoring import AthMonitorCfgHelper
        helper = AthMonitorCfgHelper(flags, "NewPixelMonitoring")

        from PixelMonitoring.PixelMonitoringConf import PixelAthHitMonTool
        from PixelMonitoring.PixelAthHitMonToolCfg import PixelAthHitMonToolCfg        
        pixelAthHitMonAlg = helper.addAlgorithm(PixelAthHitMonTool, 'PixelAthHitMonAlg')
        for k, v in kwargsHitMonTool.items():
            setattr(pixelAthHitMonAlg, k, v)
        PixelAthHitMonToolCfg(helper, pixelAthHitMonAlg, **kwargsHitMonTool)

        from PixelMonitoring.PixelMonitoringConf import PixelAthClusterMonTool
        from PixelMonitoring.PixelAthClusterMonToolCfg import PixelAthClusterMonToolCfg 
        pixelAthClusterMonAlg = helper.addAlgorithm(PixelAthClusterMonTool, 'PixelAthClusterMonAlg')
        for k, v in kwargsClusMonTool.items():
            setattr(pixelAthClusterMonAlg, k, v)
        pixelAthClusterMonAlg.TrackSelectionTool.UseTrkTrackTools = True
        pixelAthClusterMonAlg.TrackSelectionTool.CutLevel         = "TightPrimary"
        pixelAthClusterMonAlg.TrackSelectionTool.maxNPixelHoles   = 1
        pixelAthClusterMonAlg.TrackSelectionTool.maxD0            = 2
        pixelAthClusterMonAlg.TrackSelectionTool.maxZ0            = 150

        # Run 3 configs - stolen from SCT
        # Taken from InnerDetector/InDetDigitization/PixelDigitization/python/PixelDigitizationConfigNew.py
        from PixelConditionsTools.PixelConditionsSummaryConfig import PixelConditionsSummaryCfg
        InDetPixelConditionsSummaryTool = acc.popToolsAndMerge(PixelConditionsSummaryCfg(flags))
        # Taken from Tracking/TrkExtrapolation/TrkExTools/python/AtlasExtrapolatorConfig.py
        # AtlasExtrapolatorConfig can give only private extrapolator. We need public extrapolator.
        from TrkDetDescrSvc.AtlasTrackingGeometrySvcConfig import TrackingGeometrySvcCfg
        trackGeomCfg = TrackingGeometrySvcCfg(flags)
        geom_svc = trackGeomCfg.getPrimary() 
        geom_svc.GeometryBuilder.Compactify = False ######## To avoid crash ########
        acc.merge(trackGeomCfg)
        from MagFieldServices.MagFieldServicesConfig import MagneticFieldSvcCfg
        acc.merge(MagneticFieldSvcCfg(flags))
        from TrkExTools.TrkExToolsConf import Trk__Navigator
        AtlasNavigator = Trk__Navigator(name = 'AtlasNavigator')
        AtlasNavigator.TrackingGeometrySvc = geom_svc
        acc.addPublicTool(AtlasNavigator) 
        # Taken from InnerDetector/InDetExample/InDetRecExample/share/InDetRecLoadTools.py
        from TrkExRungeKuttaPropagator.TrkExRungeKuttaPropagatorConf import Trk__RungeKuttaPropagator as Propagator
        InDetPropagator = Propagator(name = 'InDetPropagator')
        InDetPropagator.AccuracyParameter = 0.0001
        InDetPropagator.MaxStraightLineStep = .004
        acc.addPublicTool(InDetPropagator)
        from TrkExTools.TrkExToolsConf import Trk__MaterialEffectsUpdator
        InDetMaterialUpdator = Trk__MaterialEffectsUpdator(name = "InDetMaterialEffectsUpdator")
        acc.addPublicTool(InDetMaterialUpdator)
        InDetSubPropagators = []
        InDetSubUpdators    = []
        # -------------------- set it depending on the geometry ----------------------------------------------------
        # default for ID is (Rk,Mat)
        InDetSubPropagators += [ InDetPropagator.name() ]
        InDetSubUpdators    += [ InDetMaterialUpdator.name() ]
        # default for Calo is (Rk,MatLandau)
        InDetSubPropagators += [ InDetPropagator.name() ]
        InDetSubUpdators    += [ InDetMaterialUpdator.name() ]
        # default for MS is (STEP,Mat)
        #InDetSubPropagators += [ InDetStepPropagator.name() ]
        InDetSubUpdators    += [ InDetMaterialUpdator.name() ]
        from TrkExTools.TrkExToolsConf import Trk__Extrapolator
        InDetExtrapolator = Trk__Extrapolator(name                    = 'InDetExtrapolator',
                                              Propagators             = [ InDetPropagator ],
                                              MaterialEffectsUpdators = [ InDetMaterialUpdator ],
                                              Navigator               = AtlasNavigator,
                                              SubPropagators          = InDetSubPropagators,
                                              SubMEUpdators           = InDetSubUpdators)
        acc.addPublicTool(InDetExtrapolator)
        from InDetTestPixelLayer.InDetTestPixelLayerConf import InDet__InDetTestPixelLayerTool
        InDetTestPixelLayerTool = InDet__InDetTestPixelLayerTool(name = "InDetTestPixelLayerTool",
                                                                 PixelSummaryTool = InDetPixelConditionsSummaryTool,
                                                                 CheckActiveAreas=True,
                                                                 CheckDeadRegions=True,
                                                                 CheckDisabledFEs=True)
        acc.addPublicTool(InDetTestPixelLayerTool)
        from InDetTrackHoleSearch.InDetTrackHoleSearchConf import InDet__InDetTrackHoleSearchTool
        InDetHoleSearchTool = InDet__InDetTrackHoleSearchTool(name = "InDetHoleSearchTool",
                                                              Extrapolator = InDetExtrapolator,
                                                              PixelSummaryTool = InDetPixelConditionsSummaryTool,
                                                              usePixel      = flags.Detector.GeometryPixel,
                                                              useSCT        = flags.Detector.GeometrySCT,
                                                              CountDeadModulesAfterLastHit = True,
                                                              PixelLayerTool = InDetTestPixelLayerTool)
        acc.addPublicTool(InDetHoleSearchTool)
        from InDetAssociationTools.InDetAssociationToolsConf import InDet__InDetPRD_AssociationToolGangedPixels
        InDetPrdAssociationTool = InDet__InDetPRD_AssociationToolGangedPixels(name                           = "InDetPrdAssociationTool",
                                                                              PixelClusterAmbiguitiesMapName = "PixelClusterAmbiguitiesMap",
                                                                              SetupCorrect                   = True,
                                                                              addTRToutliers                 = True)
        acc.addPublicTool(InDetPrdAssociationTool)
        from InDetTrackSummaryHelperTool.InDetTrackSummaryHelperToolConf import InDet__InDetTrackSummaryHelperTool
        InDetTrackSummaryHelperTool = InDet__InDetTrackSummaryHelperTool(name            = "InDetSummaryHelper",
                                                                         AssoTool        = InDetPrdAssociationTool,
                                                                         PixelToTPIDTool = None,
                                                                         TestBLayerTool  = None,
                                                                         RunningTIDE_Ambi = True,
                                                                         DoSharedHits    = False,
                                                                         HoleSearch      = InDetHoleSearchTool,
                                                                         usePixel        = flags.Detector.GeometryPixel,
                                                                         useSCT          = flags.Detector.GeometrySCT,
                                                                         useTRT          = flags.Detector.GeometryTRT)
        from TrkTrackSummaryTool.TrkTrackSummaryToolConf import Trk__TrackSummaryTool
        InDetTrackSummaryTool = Trk__TrackSummaryTool(name = "InDetTrackSummaryTool",
                                                      InDetSummaryHelperTool = InDetTrackSummaryHelperTool,
                                                      doSharedHits           = False,
                                                      doHolesInDet           = True,
                                                      TRT_ElectronPidTool    = None,
                                                      TRT_ToT_dEdxTool       = None,
                                                      PixelToTPIDTool        = None)
        ############################## WORKAROUND (END) ############################
        pixelAthClusterMonAlg.TrackSelectionTool.TrackSummaryTool = InDetTrackSummaryTool
        pixelAthClusterMonAlg.TrackSelectionTool.Extrapolator     = InDetExtrapolator
        
        
        PixelAthClusterMonToolCfg(helper, pixelAthClusterMonAlg, **kwargsClusMonTool)

        from PixelMonitoring.PixelMonitoringConf import PixelAthErrorMonTool
        from PixelMonitoring.PixelAthErrorMonToolCfg import PixelAthErrorMonToolCfg
        pixelAthMonAlgErrorMonAlg = helper.addAlgorithm(PixelAthErrorMonTool, 'PixelAthErrorMonAlg')
        for k, v in kwargsErrMonTool.items():
            setattr(pixelAthMonAlgErrorMonAlg, k, v)
        PixelAthErrorMonToolCfg(helper, pixelAthMonAlgErrorMonAlg, **kwargsErrMonTool)
        acc.merge(helper.result())

    return acc
    
