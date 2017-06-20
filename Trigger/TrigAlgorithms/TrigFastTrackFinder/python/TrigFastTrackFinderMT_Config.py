# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

from TrigFastTrackFinder.TrigFastTrackFinderConf import TrigFastTrackFinderMT
class TrigFastTrackFinderMTBase(TrigFastTrackFinderMT):
    __slots__ = []
    def __init__(self, name="FTF", type= "eGamma"):
        TrigFastTrackFinderMT.__init__(self,name)
        remapped_type = "electron"
        from TrigOnlineSpacePointTool.TrigOnlineSpacePointToolConf import TrigL2LayerNumberTool

        self.retrieveBarCodes = False#Look at truth information for spacepoints from barcodes
        #self.SignalBarCodes = [10001] #single particles
        self.SignalBarCodes = [11 ,12] #z->mumu
        
        self.useNewLayerNumberScheme = True
        
        self.OutputCollectionSuffix = type
        from AthenaCommon.AppMgr import ToolSvc

        numberingTool = TrigL2LayerNumberTool(name = "TrigL2LayerNumberTool_FTF")
        numberingTool.UseNewLayerScheme = self.useNewLayerNumberScheme
        ToolSvc += numberingTool
        self.LayerNumberTool = numberingTool
        
        from InDetTrigRecExample.InDetTrigSliceSettings import InDetTrigSliceSettings
        if type=="FTK" or type=="FTKRefit":
          from TrigFTK_RecExample.TrigFTKLoadTools import theFTK_DataProviderSvc
          self.FTK_DataProviderSvc = theFTK_DataProviderSvc
          self.FTK_Mode=True
        if type=="FTKRefit":    
          self.FTK_Refit=True
        else:
          #Spacepoint conversion
          from TrigOnlineSpacePointTool.TrigOnlineSpacePointToolConf import TrigSpacePointConversionTool
          spTool = TrigSpacePointConversionTool().clone('TrigSpacePointConversionTool_' + remapped_type)
          spTool.DoPhiFiltering = InDetTrigSliceSettings[('doSpPhiFiltering',remapped_type)]
          spTool.UseNewLayerScheme = self.useNewLayerNumberScheme
          spTool.UseBeamTilt = False
          spTool.PixelSP_ContainerName = 'PixelSpacePoints'
          spTool.SCT_SP_ContainerName = 'SCT_SpacePoints'
          spTool.layerNumberTool = numberingTool
          ToolSvc += spTool
          self.SpacePointProviderTool=spTool
          self.MinSPs = 5 #Only process RoI with more than 5 spacepoints 
          
          self.Triplet_MinPtFrac = 1
          self.Triplet_nMaxPhiSlice = 53
          if remapped_type=="cosmics":
            self.Triplet_nMaxPhiSlice = 2 #Divide detector in 2 halves for cosmics
          
          self.Triplet_MaxBufferLength = 3
          self.doSeedRedundancyCheck = InDetTrigSliceSettings[('checkRedundantSeeds',remapped_type)]
          self.Triplet_D0Max        = InDetTrigSliceSettings[('d0SeedMax',remapped_type)]
          self.Triplet_D0_PPS_Max   = InDetTrigSliceSettings[('d0SeedPPSMax',remapped_type)] 
          self.TrackInitialD0Max = 20.
          if remapped_type=='cosmics':
            self.TrackInitialD0Max = 1000.
            self.TrackZ0Max   = 1000.
          self.TripletDoPSS   = False
          self.pTmin = InDetTrigSliceSettings[('pTmin',remapped_type)]
          self.DoubletDR_Max = InDetTrigSliceSettings[('dRdoubletMax',remapped_type)]
          self.SeedRadBinWidth = InDetTrigSliceSettings[('seedRadBinWidth',remapped_type)]
          self.doTrigInDetTrack = InDetTrigSliceSettings[('doTrigInDetTrack',remapped_type)]

          if remapped_type=="cosmics":
            self.Doublet_FilterRZ = False


          ## SCT and Pixel detector elements road builder
          from InDetTrigRecExample.InDetTrigConfigRecLoadTools import InDetTrigSiDetElementsRoadMaker
          InDetTrigSiDetElementsRoadMaker_FTF = InDetTrigSiDetElementsRoadMaker.clone('InDetTrigSiDetElementsRoadMaker_FTF')
          InDetTrigSiDetElementsRoadMaker_FTF.RoadWidth = 10.0
          InDetTrigSiDetElementsRoadMaker_FTF.usePixel = True
          InDetTrigSiDetElementsRoadMaker_FTF.useSCT = True
          if remapped_type=="cosmics":
            from InDetTrigRecExample.InDetTrigConfigRecLoadToolsCosmics import InDetTrigSiDetElementsRoadMakerCosmics
            InDetTrigSiDetElementsRoadMaker_FTF = InDetTrigSiDetElementsRoadMakerCosmics.clone('InDetTrigSiDetElementsRoadMaker_FTF')
          ToolSvc += InDetTrigSiDetElementsRoadMaker_FTF


          from InDetTrigRecExample.InDetTrigConfigRecLoadTools import InDetTrigSiComTrackFinder
          InDetTrigSiComTrackFinder_FTF = InDetTrigSiComTrackFinder.clone("InDetTrigSiComTrackFinder_FTF")
          ToolSvc += InDetTrigSiComTrackFinder_FTF
          print InDetTrigSiComTrackFinder_FTF
        
        
          from InDetTrigRecExample.ConfiguredNewTrackingTrigCuts import EFIDTrackingCuts
          TrackingCuts = EFIDTrackingCuts
          if remapped_type=="cosmics":
            from InDetTrigRecExample.ConfiguredNewTrackingTrigCuts import EFIDTrackingCutsCosmics
            TrackingCuts = EFIDTrackingCutsCosmics

          from InDetRecExample.InDetKeys import InDetKeys
          from TrkExRungeKuttaPropagator.TrkExRungeKuttaPropagatorConf import Trk__RungeKuttaPropagator as Propagator
          InDetPatternPropagator = Propagator(name = 'InDetPatternPropagator')
          ToolSvc += InDetPatternPropagator
          from TrkMeasurementUpdator_xk.TrkMeasurementUpdator_xkConf import Trk__KalmanUpdator_xk
          InDetPatternUpdator = Trk__KalmanUpdator_xk(name = 'InDetPatternUpdator')
          ToolSvc += InDetPatternUpdator

          from SiClusterOnTrackTool.SiClusterOnTrackToolConf import InDet__PixelClusterOnTrackTool
          PixelClusterOnTrackToolDigital = InDet__PixelClusterOnTrackTool("InDetPixelClusterOnTrackToolDigital",
                                                                   DisableDistortions = False,
                                                                   applyNNcorrection = False,
                                                                   NNIBLcorrection = False,
                                                                   SplitClusterAmbiguityMap = InDetKeys.SplitClusterAmbiguityMap(),
                                                                   RunningTIDE_Ambi = False,
                                                                   ErrorStrategy = 2,
                                                                   PositionStrategy = 1 
                                                                   )

          ToolSvc += PixelClusterOnTrackToolDigital
          from SiClusterOnTrackTool.SiClusterOnTrackToolConf import InDet__SCT_ClusterOnTrackTool
          SCT_ClusterOnTrackTool = InDet__SCT_ClusterOnTrackTool ("InDetSCT_ClusterOnTrackTool",
                                                                  #CorrectionStrategy = -1,  # no position correction (test for bug #56477)
                                                                  CorrectionStrategy = 0,  # do correct position bias
                                                                  ErrorStrategy      = 2)  # do use phi dependent errors
          ToolSvc += SCT_ClusterOnTrackTool
          from TrkRIO_OnTrackCreator.TrkRIO_OnTrackCreatorConf import Trk__RIO_OnTrackCreator
          InDetRotCreatorDigital = Trk__RIO_OnTrackCreator(name             = 'InDetRotCreatorDigital',
                                                    ToolPixelCluster = PixelClusterOnTrackToolDigital,
                                                    ToolSCT_Cluster  = SCT_ClusterOnTrackTool,
                                                    Mode             = 'indet')
          ToolSvc += InDetRotCreatorDigital
          from InDetAssociationTools.InDetAssociationToolsConf import InDet__InDetPRD_AssociationToolGangedPixels
          InDetPrdAssociationTool = InDet__InDetPRD_AssociationToolGangedPixels(name                           = "InDetPrdAssociationTool",
                                                                                PixelClusterAmbiguitiesMapName = InDetKeys.GangedPixelMap(),
                                                                                addTRToutliers                 = True)
          ToolSvc += InDetPrdAssociationTool

          from AthenaCommon.DetFlags import DetFlags
          from SiCombinatorialTrackFinderTool_xk.SiCombinatorialTrackFinderTool_xkConf import InDet__SiCombinatorialTrackFinder_xk
          InDetSiComTrackFinder = InDet__SiCombinatorialTrackFinder_xk(name                  = 'InDetSiComTrackFinder',
                                                                       PropagatorTool        = InDetPatternPropagator,
                                                                       UpdatorTool           = InDetPatternUpdator,
                                                                       RIOonTrackTool        = InDetRotCreatorDigital,
                                                                       AssosiationTool       = InDetPrdAssociationTool,
                                                                       usePixel              = True,#DetFlags.haveRIO.pixel_on(),
                                                                       useSCT                = True,#DetFlags.haveRIO.SCT_on(),
                                                                       PixManagerLocation    = InDetKeys.PixelManager(),
                                                                       SCTManagerLocation    = InDetKeys.SCT_Manager(),
                                                                       PixelClusterContainer = InDetKeys.PixelClusters(),
                                                                       SCT_ClusterContainer  = InDetKeys.SCT_Clusters())
          ToolSvc += InDetSiComTrackFinder
          from SiDetElementsRoadTool_xk.SiDetElementsRoadTool_xkConf import InDet__SiDetElementsRoadMaker_xk
          InDetSiDetElementsRoadMaker = InDet__SiDetElementsRoadMaker_xk(name               = 'InDetSiRoadMaker_'+remapped_type,
                                                                         PropagatorTool     = InDetPatternPropagator,
                                                                         usePixel           = True,#DetFlags.haveRIO.pixel_on(),
                                                                         PixManagerLocation = InDetKeys.PixelManager(),
                                                                         useSCT             = True,#DetFlags.haveRIO.SCT_on(), 
                                                                         SCTManagerLocation = InDetKeys.SCT_Manager(),         
                                                                         RoadWidth          = 10.0)
          print InDetSiDetElementsRoadMaker
          ToolSvc += InDetSiDetElementsRoadMaker

          from SiTrackMakerTool_xk.SiTrackMakerTool_xkConf import InDet__SiTrackMaker_xk as SiTrackMaker
          TrackMaker_FTF = SiTrackMaker(name                      = 'InDetTrigSiTrackMaker_FTF_'+remapped_type,
                                           RoadTool                  = InDetSiDetElementsRoadMaker,
                                           CombinatorialTrackFinder  = InDetSiComTrackFinder,
                                           pTmin          = InDetTrigSliceSettings[('pTmin',remapped_type)],
                                           nClustersMin   = TrackingCuts.minClusters(),
                                           nHolesMax      = TrackingCuts.nHolesMax(),
                                           nHolesGapMax   = TrackingCuts.nHolesGapMax(),
                                           SeedsFilterLevel = 0, # Do not use built-in seeds filter
                                           Xi2max         = TrackingCuts.Xi2max(),
                                           Xi2maxNoAdd    = TrackingCuts.Xi2maxNoAdd(),
                                           nWeightedClustersMin= TrackingCuts.nWeightedClustersMin(),
                                           Xi2maxMultiTracks         = TrackingCuts.Xi2max(),
                                           UseAssociationTool       = False)

          from InDetRecExample.InDetJobProperties import InDetFlags
          if not InDetFlags.doCaloSeededBrem():
            TrackMaker_FTF.InputClusterContainerName = ""
            TrackMaker_FTF.InputHadClusterContainerName = ""

          from InDetTrigRecExample.InDetTrigFlags import InDetTrigFlags
          if type=='eGamma' and InDetTrigFlags.doBremRecovery():
            TrackMaker_FTF.useBremModel = True
  
          if remapped_type=="cosmics":
            TrackMaker_FTF.RoadTool.CosmicTrack=True

          ToolSvc += TrackMaker_FTF
          self.initialTrackMaker = TrackMaker_FTF

          from TrigInDetTrackFitter.TrigInDetTrackFitterConf import TrigInDetTrackFitter
          theTrigInDetTrackFitter = TrigInDetTrackFitter()
          #theTrigInDetTrackFitter.correctClusterPos = False #Flag to control whether to correct cluster position
          theTrigInDetTrackFitter.correctClusterPos = True  #temporarily to true to improve err(z0) estimates

            

          from InDetTrigRecExample.InDetTrigConfigRecLoadTools import InDetTrigRotCreator
          theTrigInDetTrackFitter.ROTcreator = InDetTrigRotCreator
          ToolSvc += theTrigInDetTrackFitter
          self.trigInDetTrackFitter = theTrigInDetTrackFitter
          from InDetTrigRecExample.InDetTrigFlags import InDetTrigFlags
          if type=='eGamma' and InDetTrigFlags.doBremRecovery():
            theTrigInDetTrackFitterBrem = TrigInDetTrackFitter(name='theTrigInDetTrackFitterBrem',
                                                               doBremmCorrection = True)
            ToolSvc += theTrigInDetTrackFitterBrem
            self.trigInDetTrackFitter = theTrigInDetTrackFitterBrem

          self.doZFinder = InDetTrigSliceSettings[('doZFinder',remapped_type)]
          if (self.doZFinder):
            from IDScanZFinder.IDScanZFinderConf import TrigZFinder
            theTrigZFinder = TrigZFinder()
            theTrigZFinder.NumberOfPeaks = 3
            theTrigZFinder.LayerNumberTool=numberingTool
            
            theTrigZFinder.FullScanMode = True #TODO: know this from the RoI anyway - should set for every event
            ToolSvc += theTrigZFinder
            self.trigZFinder = theTrigZFinder
            self.doFastZVertexSeeding = True
            self.zVertexResolution = 7.5
          
          from TrigInDetConf.TrigInDetRecCommonTools import InDetTrigFastTrackSummaryTool
          self.TrackSummaryTool = InDetTrigFastTrackSummaryTool

          if remapped_type == "tauCore":
            from TrigInDetConf.TrigInDetRecCommonTools import InDetTrigTrackSummaryToolWithHoleSearch
            self.TrackSummaryTool = InDetTrigTrackSummaryToolWithHoleSearch

          from TrigInDetTrackFitter.TrigInDetTrackFitterConf import TrigL2ResidualCalculator
          resCalc = TrigL2ResidualCalculator(OfflineClusters=False)
          ToolSvc += resCalc
          self.TrigL2ResidualCalculator = resCalc
          self.doCloneRemoval = InDetTrigSliceSettings[('doCloneRemoval',remapped_type)]

class TrigFastTrackFinderMT_eGamma(TrigFastTrackFinderMTBase):
  def __init__(self, name = "TrigFastTrackFinderMT_eGamma"):
    TrigFastTrackFinderMTBase.__init__(self, "TrigFastTrackFinderMT_eGamma","eGamma")
