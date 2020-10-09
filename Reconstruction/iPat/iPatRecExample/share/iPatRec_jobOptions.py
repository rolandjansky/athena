###############################################################
#
# iPatRec jobOptions fragment
#
#==============================================================

#--------------------------------------------------------------
# Reconstruction event processing:
#--------------------------------------------------------------

# check sufficient info available for iPatRec to find tracks
import MagFieldServices.SetupField
from AthenaCommon.DetFlags import DetFlags
if DetFlags.detdescr.ID_on() and (DetFlags.haveRIO.pixel_on() or DetFlags.haveRIO.SCT_on()):

    # tool configuration (need to know which detectors to process)
    from AthenaCommon.AppMgr import ToolSvc
    from iPatGeometryTools.iPatGeometryToolsConf import DetectorSelection
    ToolSvc += DetectorSelection()
    ToolSvc.DetectorSelection.processPixels = DetFlags.haveRIO.pixel_on()
    ToolSvc.DetectorSelection.processSCT    = DetFlags.haveRIO.SCT_on()
    ToolSvc.DetectorSelection.processTRT    = DetFlags.haveRIO.TRT_on()
    # ToolSvc.DetectorSelection.processTRT    = False

    # InDet ROT creators
    from iPatTrackFollower.iPatTrackFollowerConf import SiClusterProperties
    SiClusterProperties = SiClusterProperties(name = 'SiClusterProperties',
                                              PixelBroadROT_Maker   = BroadPixelClusterOnTrackTool,
                                              PixelPreciseROT_Maker = PixelClusterOnTrackTool,
                                              SCT_BroadROT_Maker    = BroadSCT_ClusterOnTrackTool,
                                              SCT_PreciseROT_Maker  = SCT_ClusterOnTrackTool)
    ToolSvc += SiClusterProperties

    # svc used by fitter to distinguish indet/muon clients
    from TrkDetDescrSvc.TrkDetDescrSvcConf import Trk__TrackingVolumesSvc
    ServiceMgr += Trk__TrackingVolumesSvc()

    # set up the extrapolator and fitter as used by iPatRec
    #from TrkExRungeKuttaIntersector.TrkExRungeKuttaIntersectorConf import \
    #     Trk__IntersectorWrapper as Propagator
    from TrkExRungeKuttaPropagator.TrkExRungeKuttaPropagatorConf import \
         Trk__RungeKuttaPropagator as Propagator
    iPatPropagator = Propagator(name              = 'iPatPropagator',
                                AccuracyParameter = 0.0001)
    ToolSvc +=  iPatPropagator

    from TrkExTools.TrkExToolsConf import Trk__Navigator
    iPatNavigator = Trk__Navigator(name                = 'iPatNavigator',
                                   TrackingGeometrySvc = AtlasTrackingGeometrySvc)
    ToolSvc += iPatNavigator

    from TrkExTools.TrkExToolsConf import Trk__MaterialEffectsUpdator
    iPatMaterialUpdator = Trk__MaterialEffectsUpdator(name = "iPatMaterialEffectsUpdator")
    ToolSvc += iPatMaterialUpdator
    
    from TrkExTools.TrkExToolsConf import Trk__Extrapolator
    iPatExtrapolator= Trk__Extrapolator(name                    = 'iPatExtrapolator',
                                        MaterialEffectsUpdators = [ iPatMaterialUpdator ],
                                        Propagators             = [ iPatPropagator ],
                                        Navigator               = iPatNavigator,
                                        Tolerance               = 0.0011)
    ToolSvc += iPatExtrapolator
    
    from TrkiPatFitter.TrkiPatFitterConf import Trk__MaterialAllocator
    iPatMaterialAllocator = Trk__MaterialAllocator(
        name                      = 'iPatMaterialAllocator',
        #AggregateMaterial         = True,
        Extrapolator              = iPatExtrapolator,
        TrackingGeometrySvc       = ServiceMgr.AtlasTrackingGeometrySvc)
    ToolSvc += iPatMaterialAllocator

    from TrkiPatFitter.TrkiPatFitterConf import Trk__iPatFitter
    iPatRecFitter = Trk__iPatFitter(name              = 'iPatRecFitter',
                                    MaterialAllocator = iPatMaterialAllocator)
    ToolSvc += iPatRecFitter

    from iPatTrackFitter.iPatTrackFitterConf import TrackFitter
    ToolSvc += TrackFitter(name = 'TrackFitter',
                           MaterialAllocator = iPatMaterialAllocator)
    
    # iPatRec algorithm
    from iPatRecAlgs.iPatRecAlgsConf import iPatRec
    iPatRec = iPatRec(TRT_DataSelector = "TRT_DataSelector/TRT_DataSelector")
    ## iPatRec = iPatRec(TRT_DataSelector = "")
    iPatRec.Fitter = iPatRecFitter
    topSequence += iPatRec

    from iPatTrackFollower.iPatTrackFollowerConf import TrackFollower
    ToolSvc += TrackFollower(name = 'TrackFollower',
                             TRT_Recon = "TRT_Recon/TRT_Recon")
    
    # include the RegionSelector service
    if DetFlags.haveRIO.TRT_on():
        from AthenaCommon.AppMgr import ServiceMgr
        # from RegionSelector.RegSelSvcDefault import RegSelSvcDefault
        # iPatRegSelSvc  = RegSelSvcDefault()
        # iPatRegSelSvc.enableTRT = True
        # ServiceMgr += iPatRegSelSvc

    # -------------------------------------------------------------
    # Algorithm Private Options: iPatRec
    # -------------------------------------------------------------
    iPatRec.maxSecondaryImpact  = InDetNewTrackingCuts.maxSecondaryImpact()
    iPatRec.minPt               = InDetNewTrackingCuts.minPT()

    # specify vertex region (for primary track finding)
    # iPatRec.vertex       	= [0.0*mm, 0.0*mm, 0.0*mm]
    # iPatRec.vertexWidth	= [1.0*mm, 1.0*mm, 250.0*mm]

    # -------------------------------------------------------------
    # Heavy Ion
    # -------------------------------------------------------------
    from iPatTrackFinder.iPatTrackFinderConf import TrackFinder
    ToolSvc += TrackFinder(name = 'TrackFinder')
    if InDetFlags.doHeavyIon():
        ToolSvc.TrackFinder.searchForSecondaries  = False
    
    # -------------------------------------------------------------
    # truth association via SiClusterPrepRawDataTruthCollection
    # -------------------------------------------------------------
    if InDetFlags.doTruth():
        # iPatRec TrackTruth
        from iPatRecAlgs.iPatRecAlgsConf import iPatTrackTruthAssociator
        iPatTrackTruthAssociator = iPatTrackTruthAssociator( TracksLocation = "iPatTracks" )
        topSequence += iPatTrackTruthAssociator

        # iPatRec ShortTracks
        # include( "iPatRecAlgs/iPatShortTracks_jobOptions.py" )

        #  -------------------------------------------------------------
        # tabulated statistics at end of job: trk efficiency and #hits
        # --------------------------------------------------------------
        # iPatRec statistics
        from iPatRecAlgs.iPatRecAlgsConf import iPatStatistics
        iPatStatistics = iPatStatistics()
        topSequence += iPatStatistics

        iPatStatistics.haveTruth     = InDetFlags.doTruth()
        iPatStatistics.minPt         = iPatRec.minPt

#==============================================================
# End of iPatRec jobOptions fragment
#
###############################################################
