from egammaTools import egammaToolsConf

class TrigEMBremCollectionBuilder(egammaToolsConf.EMBremCollectionBuilder) :
  __slots__ = ()
  def __init__(self, name="TrigEMBremCollectionBuilder", **kw):
    from AthenaCommon.Logging import logging
    mlog = logging.getLogger(name)
    mlog.info("entering")

    super(TrigEMBremCollectionBuilder, self).__init__(name, **kw)
    
    from egammaTrackTools.egammaTrackToolsConf import egammaTrkRefitterTool
    from TrigEgammaRec.TrigEgammaTrackTools import GSFTrigTrackFitter,GSFTrigExtrapolator,egTrigTrkExtrapolator
    GSFTrigRefitterTool = egammaTrkRefitterTool(name = 'GSFTrigRefitterTool',
                                                FitterTool = GSFTrigTrackFitter,
                                                useBeamSpot = False,
                                                ReintegrateOutliers=True)
    from AthenaCommon.AppMgr import ToolSvc
    ToolSvc += GSFTrigRefitterTool
    
    # ----------- load association tool from Inner Detector to handle pixel ganged ambiguities
    #
    from InDetAssociationTools.InDetAssociationToolsConf import InDet__InDetPRD_AssociationToolGangedPixels
    GSFTrigInDetPrdAssociationTool = InDet__InDetPRD_AssociationToolGangedPixels(name = "GSFTrigInDetPrdAssociationTool",
                                                                                 PixelClusterAmbiguitiesMapName = 'PixelClusterAmbiguitiesMap')
    ToolSvc += GSFTrigInDetPrdAssociationTool        
    #
    # ----------- Load SummaryTool
    #
    # Loading Configurable HoleSearchTool
    #
    from InDetTrackHoleSearch.InDetTrackHoleSearchConf import InDet__InDetTrackHoleSearchTool
    from AthenaCommon.DetFlags import DetFlags
    from InDetTrigRecExample.InDetTrigFlags import InDetTrigFlags
    from AthenaCommon.GlobalFlags import globalflags
    
    GSFTrigHoleSearchTool = InDet__InDetTrackHoleSearchTool(name = "GSFTrigHoleSearchTool",
                                                            Extrapolator = egTrigTrkExtrapolator,
                                                            usePixel      = DetFlags.haveRIO.pixel_on(),
                                                            useSCT        = DetFlags.haveRIO.SCT_on(),
                                                            checkBadSCTChip = InDetTrigFlags.checkDeadElementsOnTrack(),
                                                            CountDeadModulesAfterLastHit = True)
    
    from AthenaCommon.AppMgr import ServiceMgr
    if (DetFlags.haveRIO.SCT_on()):
      from SCT_ConditionsServices.SCT_ConditionsServicesConf import SCT_ConditionsSummarySvc
      InDetSCT_ConditionsSummarySvc = SCT_ConditionsSummarySvc(name = "InDetSCT_ConditionsSummarySvc")
      ServiceMgr += InDetSCT_ConditionsSummarySvc
      GSFTrigHoleSearchTool.SctSummarySvc = ServiceMgr.InDetSCT_ConditionsSummarySvc
    else:
      GSFTrigHoleSearchTool.SctSummarySvc = None
      
    ToolSvc += GSFTrigHoleSearchTool
    #
    # Load BLayer tool
    #
    GSFTrigTestBLayerTool = None
    if DetFlags.haveRIO.pixel_on() :
      from InDetTestBLayer.InDetTestBLayerConf import InDet__InDetTestBLayerTool
      from PixelConditionsServices.PixelConditionsServicesConf import PixelConditionsSummarySvc
      ServiceMgr += PixelConditionsSummarySvc()            
      GSFTrigTestBLayerTool = InDet__InDetTestBLayerTool(name            = "GSFTrigTestBLayerTool",
                                                         PixelSummarySvc = ServiceMgr.PixelConditionsSummarySvc,
                                                         Extrapolator    = egTrigTrkExtrapolator)
      ToolSvc += GSFTrigTestBLayerTool

    # Configurable version of TRT_ElectronPidTools
    #
    GSFTrigTRT_ElectronPidTool = None
    if DetFlags.haveRIO.TRT_on() and not InDetTrigFlags.doSLHC() and not InDetTrigFlags.doHighPileup() :
      
        from TRT_ElectronPidTools.TRT_ElectronPidToolsConf import InDet__TRT_LocalOccupancy
        GSFTrigTRT_LocalOccupancy = InDet__TRT_LocalOccupancy(name ="GSFTrig_TRT_LocalOccupancy")
        ToolSvc += GSFTrigTRT_LocalOccupancy
        
        from TRT_ElectronPidTools.TRT_ElectronPidToolsConf import InDet__TRT_ElectronPidToolRun2
        GSFTrigTRT_ElectronPidTool = InDet__TRT_ElectronPidToolRun2(name   = "GSFTrigTRT_ElectronPidTool",
                                                                    TRT_LocalOccupancyTool = GSFTrigTRT_LocalOccupancy,
                                                                    isData = (globalflags.DataSource == 'data') )
        
        ToolSvc += GSFTrigTRT_ElectronPidTool

    #
    # Configurable version of PixelToTPIDTOol
    #
    GSFTrigPixelToTPIDTool = None

    #
    # Configrable version of loading the InDetTrackSummaryHelperTool
    #
    from InDetTrackSummaryHelperTool.InDetTrackSummaryHelperToolConf import InDet__InDetTrackSummaryHelperTool
    GSFTrigTrackSummaryHelperTool = InDet__InDetTrackSummaryHelperTool(name            = "GSFTrigTrackSummaryHelperTool",
                                                                       AssoTool        = GSFTrigInDetPrdAssociationTool,
                                                                       PixelToTPIDTool = GSFTrigPixelToTPIDTool,
                                                                       TestBLayerTool  = GSFTrigTestBLayerTool,
                                                                       DoSharedHits    = False,
                                                                       HoleSearch      = GSFTrigHoleSearchTool,
                                                                       usePixel        = DetFlags.haveRIO.pixel_on(),
                                                                       useSCT          = DetFlags.haveRIO.SCT_on(),
                                                                       useTRT          = DetFlags.haveRIO.TRT_on())
    ToolSvc += GSFTrigTrackSummaryHelperTool
    #
    # Configurable version of TrkTrackSummaryTool: no TRT_PID tool needed here (no shared hits)
    #
    from TrkTrackSummaryTool.TrkTrackSummaryToolConf import Trk__TrackSummaryTool
    GSFTrigInDetTrackSummaryTool = Trk__TrackSummaryTool(name = "GSFTrigInDetTrackSummaryTool",
                                                         InDetSummaryHelperTool = GSFTrigTrackSummaryHelperTool,
                                                         doSharedHits           = False,
                                                         InDetHoleSearchTool    = GSFTrigHoleSearchTool,
                                                         TRT_ElectronPidTool    = GSFTrigTRT_ElectronPidTool,
                                                         PixelToTPIDTool        = GSFTrigPixelToTPIDTool)
    ToolSvc += GSFTrigInDetTrackSummaryTool
    #
    # --- load patricle creator tool
    #
    from TrkParticleCreator.TrkParticleCreatorConf import Trk__TrackParticleCreatorTool
    GSFTrigInDetParticleCreatorTool = Trk__TrackParticleCreatorTool(name                    = "GSFTrigInDetParticleCreatorTool",
                                                                    KeepParameters          = True,
                                                                    Extrapolator            = egTrigTrkExtrapolator,
                                                                    TrackSummaryTool        = GSFTrigInDetTrackSummaryTool,
                                                                    UseTrackSummaryTool     = False,
                                                                    ForceTrackSummaryUpdate = False)
    ToolSvc += GSFTrigInDetParticleCreatorTool
    #
    # --- do track slimming
    #
    from TrkTrackSlimmingTool.TrkTrackSlimmingToolConf import Trk__TrackSlimmingTool as ConfigurableTrackSlimmingTool
    GSFTrigInDetTrkSlimmingTool = ConfigurableTrackSlimmingTool(name  = "GSFTrigInDetTrackSlimmingTool",
                                                                KeepParameters = False,
                                                                KeepOutliers   = True )
    ToolSvc += GSFTrigInDetTrkSlimmingTool


    # do the configuration
    # self.ClusterContainerName="LArClusterEM"
    # self.TrackParticleContainerName=InDetKeys.xAODTrackParticleContainer()
    # self.OutputTrkPartContainerName="GSFTrigTrackParticles"
    # self.OutputTrackContainerName="GSFTrigTracks"
    self.TrackRefitTool= GSFTrigRefitterTool
    self.TrackParticleCreatorTool=GSFTrigInDetParticleCreatorTool
    self.TrackSlimmingTool=GSFTrigInDetTrkSlimmingTool
    self.TrackSummaryTool=GSFTrigInDetTrackSummaryTool

    self.minNoSiHits=4
    self.broadDeltaEta=0.1   # this is multiplied by 2 for the Candidate Match , so +- 0.2 in eta
    self.broadDeltaPhi=0.15   # this is multiplied by 2 for the Candidate Match , so +- 0.3 in phi
    self.narrowDeltaEta=0.05 
    #These have to be relaxed enough for the conversions
    self.narrowDeltaPhi=0.05   
    self.narrowDeltaPhiBrem=0.20 #Dominated by the needs of assymetric conversions
    self.narrowDeltaPhiRescale=0.05  
    self.narrowDeltaPhiRescaleBrem=0.1
