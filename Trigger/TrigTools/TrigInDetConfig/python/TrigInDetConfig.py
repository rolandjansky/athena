#
#  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
#

from AthenaConfiguration.ComponentAccumulator import ComponentAccumulator
from AthenaConfiguration.ComponentFactory import CompFactory
from TrigEDMConfig.TriggerEDMRun3 import recordable
from InDetRecExample.InDetKeys import InDetKeys


def RungeKuttaPropagatorCfg(flags, **kwargs):
  acc = ComponentAccumulator()
  name = kwargs.pop("propagatorName", "InDetTrigPatternPropagator")
  acc.addPublicTool( CompFactory.Trk.RungeKuttaPropagator( name ) )
  return acc

def SiDetElementsRoadMaker_xkCfg( flags, **kwargs ):
  """
  based  on: InnerDetector/InDetExample/InDetTrigRecExample/python/InDetTrigConfigRecLoadTools.py, should be moved elsewhere
  """
  acc = ComponentAccumulator()
  name = kwargs.pop( "rodMakerName", "InDetTrigSiDetElementsRoadMaker" )
  acc.merge( RungeKuttaPropagatorCfg( flags, **kwargs ) )
  acc.addCondAlgo( CompFactory.InDet.SiDetElementsRoadCondAlg_xk() )
  tool = CompFactory.InDet.SiDetElementsRoadMaker_xk( name,
                                                      PropagatorTool = acc.getPublicTool( "InDetTrigPatternPropagator" ),
                                                      usePixel     = flags.Detector.RecoPixel,
                                                      useSCT       = flags.Detector.RecoSCT,
                                                      RoadWidth    = flags.InDet.Tracking.roadWidth,
                                                        )
  acc.addPublicTool( tool )
  return acc

def PixelClusterOnTrackCfg( flags, **kwargs ):
  """
  based on: InnerDetector/InDetExample/InDetTrigRecExample/python/InDetTrigConfigRecLoadTools.py
  """
  acc = ComponentAccumulator()
  name =  kwargs.pop("pixelOnTrackName", "InDetTrigPixelClusterOnTrackTool")
  from SiLorentzAngleTool.PixelLorentzAngleConfig import PixelLorentzAngleCfg

  pixelLATool = acc.popToolsAndMerge( PixelLorentzAngleCfg( flags) )

  nnTool = CompFactory.InDet.NnClusterizationFactory( name                         = "TrigNnClusterizationFactory",
                                                      PixelLorentzAngleTool        = pixelLATool,
                                                      useToT                       = flags.InDet.doNNToTCalibration,
                                                      NnCollectionReadKey          = 'PixelClusterNN',
                                                      NnCollectionWithTrackReadKey = 'PixelClusterNNWithTrack')

  tool = CompFactory.InDet.PixelClusterOnTrackTool( name,
                                                    ErrorStrategy = 2,
                                                    LorentzAngleTool = pixelLATool,
                                                    NnClusterizationFactory = nnTool )
  acc.addPublicTool( tool )
  return acc


def SCT_ClusterOnTrackToolCfg( flags, **kwargs ):
  acc = ComponentAccumulator()
  from SiLorentzAngleTool.SCT_LorentzAngleConfig import SCT_LorentzAngleCfg
  sctLATool =  acc.popToolsAndMerge( SCT_LorentzAngleCfg( flags ) )
  tool = CompFactory.InDet.SCT_ClusterOnTrackTool("SCT_ClusterOnTrackTool",
                                                    CorrectionStrategy = 0,  # do correct position bias
                                                    ErrorStrategy      = 2,  # do use phi dependent errors
                                                    LorentzAngleTool   = sctLATool # default name
                                                    )
  acc.addPublicTool ( tool )
  return acc

def RIO_OnTrackCreatorCfg( flags, **kwargs ):
  acc = ComponentAccumulator()
  name =  kwargs.pop("rioOnTrackCreatorName", "InDetTrigRotCreator")
  acc.merge( PixelClusterOnTrackCfg( flags, **kwargs ) )
  acc.merge( SCT_ClusterOnTrackToolCfg( flags, **kwargs ) )
  tool = CompFactory.Trk.RIO_OnTrackCreator(name,
                                            ToolPixelCluster = acc.getPublicTool( "InDetTrigPixelClusterOnTrackTool" ),
                                            ToolSCT_Cluster  = acc.getPublicTool( "SCT_ClusterOnTrackTool" ),
                                            Mode             = 'indet')
  acc.addPublicTool( tool )
  return acc

def SiCombinatorialTrackFinder_xkCfg( flags, **kwargs ):
  """
  based  on: InnerDetector/InDetExample/InDetTrigRecExample/python/InDetTrigConfigRecLoadTools.py, should be moved elsewhere
  """
  acc = ComponentAccumulator()
  name = kwargs.pop( "combinatorialTrackFinderName", "InDetTrigSiComTrackFinder" )
  acc.merge( RungeKuttaPropagatorCfg( flags, **kwargs ) )
  acc.addPublicTool( CompFactory.Trk.KalmanUpdator_xk( "InDetTrigPatternUpdator" ) )
  acc.merge( RIO_OnTrackCreatorCfg( flags, **kwargs ) )

  from PixelConditionsTools.PixelConditionsSummaryConfig import PixelConditionsSummaryCfg
  pixelCondSummaryTool = acc.popToolsAndMerge( PixelConditionsSummaryCfg(flags) )

  from InDetConfig.InDetRecToolConfig import InDetSCT_ConditionsSummaryToolCfg
  sctCondSummaryTool = acc.popToolsAndMerge( InDetSCT_ConditionsSummaryToolCfg( flags, withFlaggedCondTool=False, withTdaqTool=False ) )


  tool = CompFactory.InDet.SiCombinatorialTrackFinder_xk(name,
                                                         PropagatorTool        = acc.getPublicTool( "InDetTrigPatternPropagator" ),
                                                         UpdatorTool           = acc.getPublicTool( "InDetTrigPatternUpdator" ),
                                                         RIOonTrackTool        = acc.getPublicTool( "InDetTrigRotCreator" ),
                                                         usePixel              = flags.Detector.RecoPixel,
                                                         useSCT                = flags.Detector.RecoSCT,
                                                         PixelClusterContainer = 'PixelTrigClusters',
                                                         SCT_ClusterContainer  = 'SCT_TrigClusters',
                                                         PixelSummaryTool      = pixelCondSummaryTool,
                                                         SctSummaryTool        = sctCondSummaryTool
                                                        )
  acc.setPrivateTools( tool )
  return acc

def SiTrackMaker_xkCfg(flags, **kwargs):
  """
  based on: InnerDetector/InDetExample/InDetTrigRecExample/python/InDetTrigConfigRecNewTracking.py , should be moved elsewhere
  """
  name = kwargs.pop("name", "SiTrackMaker_xk")
  acc = ComponentAccumulator()
  acc.merge( SiDetElementsRoadMaker_xkCfg( flags, **kwargs ) )
  combTrackFinderTool = acc.popToolsAndMerge( SiCombinatorialTrackFinder_xkCfg( flags, **kwargs ) )

  from IOVDbSvc.IOVDbSvcConfig import addFoldersSplitOnline
  acc.merge(addFoldersSplitOnline( flags, "INDET", '/Indet/Onl/TrkErrorScaling', '/Indet/TrkErrorScaling', className="CondAttrListCollection") )

  acc.addCondAlgo( CompFactory.RIO_OnTrackErrorScalingCondAlg(ErrorScalingType = ["PixelRIO_OnTrackErrorScaling", "SCTRIO_OnTrackErrorScaling", "TRTRIO_OnTrackErrorScaling"],
                                                              OutKeys        = ["ConditionStore+/Indet/TrkErrorScalingPixel", "ConditionStore+/Indet/TrkErrorScalingSCT", "ConditionStore+/Indet/TrkErrorScalingTRT"],
                                                              ReadKey        = "ConditionStore+/Indet/TrkErrorScaling") )


  tool = CompFactory.InDet.SiTrackMaker_xk( name,
                                            RoadTool                 = acc.getPublicTool( "InDetTrigSiDetElementsRoadMaker" ),
                                            CombinatorialTrackFinder = combTrackFinderTool,
                                            pTmin                    = flags.InDet.Tracking.minPT,
                                            nClustersMin             = flags.InDet.Tracking.minClusters,
                                            nHolesMax                = flags.InDet.Tracking.nHolesMax,
                                            nHolesGapMax             = flags.InDet.Tracking.nHolesGapMax,
                                            SeedsFilterLevel         = flags.InDet.Tracking.seedFilterLevel,
                                            Xi2max                   = flags.InDet.Tracking.Xi2max,
                                            Xi2maxNoAdd              = flags.InDet.Tracking.Xi2maxNoAdd,
                                            nWeightedClustersMin     = flags.InDet.Tracking.nWeightedClustersMin,
                                            Xi2maxMultiTracks        = flags.InDet.Tracking.Xi2max,
                                            UseAssociationTool       = False )
  acc.addPublicTool( tool )
  return acc



def InDetTestPixelLayerToolCfg(flags, **kwargs):
  acc = ComponentAccumulator()
  from PixelConditionsTools.PixelConditionsSummaryConfig import PixelConditionsSummaryCfg
  pixelCondSummaryTool = acc.popToolsAndMerge( PixelConditionsSummaryCfg(flags) )
  from InDetConfig.InDetRecToolConfig import InDetExtrapolatorCfg
  extrapolator_acc = InDetExtrapolatorCfg( flags, name = "InDetTrigExtrapolator" )
  extrapolator = acc.getPrimary()
  acc.merge(extrapolator_acc)

  tool = CompFactory.InDet.InDetTestPixelLayerTool("InDetTrigTestPixelLayerTool",
                                                               PixelSummaryTool = pixelCondSummaryTool,
                                                               Extrapolator     = extrapolator,
                                                               CheckActiveAreas = True,
                                                               CheckDeadRegions = True)
  acc.addPublicTool( tool )
  return acc


def InDetHoleSearchToolCfg(flags, **kwargs):
  acc = ComponentAccumulator()

# a possible change in HoleSearchTool impl? - This two tools do not seem to be needed now, leaving them commented out  TODO - decide if can be removed ( also func above creting the config )
#  from InDetConfig.InDetRecToolConfig import InDetSCT_ConditionsSummaryToolCfg
#  sctCondSummaryTool = acc.popToolsAndMerge( InDetSCT_ConditionsSummaryToolCfg( flags,withFlaggedCondTool=False, withTdaqTool=False ) )

#  acc.merge( InDetTestPixelLayerToolCfg( flags, **kwargs ) )

  from InDetConfig.InDetRecToolConfig import InDetExtrapolatorCfg
  acc.merge( InDetExtrapolatorCfg( flags, name = "InDetTrigExtrapolator" ) )

  name = kwargs.pop("name", "InDetTrigHoleSearchTool")
  tool = CompFactory.InDet.InDetTrackHoleSearchTool(name,
                                                    Extrapolator =  acc.getPublicTool( "InDetTrigExtrapolator" ))
  acc.addPublicTool( tool )
  return acc

def InDetTrackSummaryHelperToolCfg(flags, **kwargs):
  """
  based on: InnerDetector/InDetExample/InDetTrigRecExample/python/InDetTrigConfigRecLoadTools.py
  """
  acc = ComponentAccumulator()
  name = kwargs.pop("name", "InDetSummaryHelperTool")
  acc.merge( InDetHoleSearchToolCfg(flags, name = "InDetTrigHoleSearchTool" ) )

  acc.addPublicTool( CompFactory.InDet.InDetPRD_AssociationToolGangedPixels(name = "InDetTrigPrdAssociationTool",
                                                              PixelClusterAmbiguitiesMapName = "TrigPixelClusterAmbiguitiesMap") )

  from InDetOverlay.TRT_ConditionsConfig import TRTStrawCondAlgCfg,TRT_StrawStatusSummaryToolCfg # this will be moved somewhere else so this import will need adjustment
  acc.merge( TRTStrawCondAlgCfg(flags) )
  trtStrawSummaryTool = acc.popToolsAndMerge( TRT_StrawStatusSummaryToolCfg(flags) )

  tool = CompFactory.InDet.InDetTrackSummaryHelperTool(name,
                                                       HoleSearch    = acc.getPublicTool( "InDetTrigHoleSearchTool" ),
                                                       AssoTool      = acc.getPublicTool( "InDetTrigPrdAssociationTool" ),
                                                       TestBLayerTool = None,
                                                       PixelToTPIDTool= None, #InDetTrigPixelToTPIDTool,
                                                       DoSharedHits  = False,
                                                       TRTStrawSummarySvc = trtStrawSummaryTool,
                                                       usePixel      = flags.Detector.RecoPixel,
                                                       useSCT        = flags.Detector.RecoSCT,
                                                       useTRT        = flags.Detector.RecoTRT,
                                                         )

  acc.addPublicTool( tool )
  return acc

def TrackSummaryToolCfg(flags, **kwargs):
  acc = ComponentAccumulator()
  name = kwargs.pop("name", "InDetTrackSummaryTool")
  summaryHelperTool = kwargs.pop( "summaryHelperTool", None )
  if not summaryHelperTool:
    acc.merge( InDetTrackSummaryHelperToolCfg( flags ) )
    summaryHelperTool = acc.getPublicTool( "InDetSummaryHelperTool" )


  tool = CompFactory.Trk.TrackSummaryTool(name = name,
                                          InDetSummaryHelperTool = summaryHelperTool,
                                          doSharedHits           = False,
                                          doHolesInDet           = True,
                                          #this may be temporary #61512 (and used within egamma later)
                                          #TRT_ElectronPidTool    = InDetTrigTRT_ElectronPidTool,
                                          TRT_ElectronPidTool    = None,
                                          )

  acc.addPublicTool( tool )
  return acc



class InDetCacheNames(object):
  Pixel_ClusterKey   = "PixelTrigClustersCache"
  SCT_ClusterKey     = "SCT_ClustersCache"
  SpacePointCachePix = "PixelSpacePointCache"
  SpacePointCacheSCT = "SctSpacePointCache"
  SCTBSErrCacheKey   = "SctBSErrCache"
  SCTFlaggedCondCacheKey = "SctFlaggedCondCache"
  SCTRDOCacheKey     = "SctRDOCache"
  PixRDOCacheKey     = "PixRDOCache"
  PixBSErrCacheKey   = "PixBSErrCache"

def InDetIDCCacheCreatorCfg():
  #Create IdentifiableCaches
  acc = ComponentAccumulator()
  InDet__CacheCreator=CompFactory.getComp("InDet::CacheCreator")
  InDetCacheCreatorTrig = InDet__CacheCreator(name = "InDetCacheCreatorTrig",
                                              Pixel_ClusterKey   = InDetCacheNames.Pixel_ClusterKey,
                                              SCT_ClusterKey     = InDetCacheNames.SCT_ClusterKey,
                                              SpacePointCachePix = InDetCacheNames.SpacePointCachePix,
                                              SpacePointCacheSCT = InDetCacheNames.SpacePointCacheSCT,
                                              SCTRDOCacheKey     = InDetCacheNames.SCTRDOCacheKey,
                                              SCTBSErrCacheKey   = InDetCacheNames.SCTBSErrCacheKey,
                                              SCTFlaggedCondCacheKey = InDetCacheNames.SCTFlaggedCondCacheKey,
                                              PixRDOCacheKey     = InDetCacheNames.PixRDOCacheKey,
                                              PixBSErrCacheKey   = InDetCacheNames.PixBSErrCacheKey)

  acc.addEventAlgo( InDetCacheCreatorTrig )
  return acc


def geoModelCfg(flags):
  acc = ComponentAccumulator()
  from BeamPipeGeoModel.BeamPipeGMConfig import BeamPipeGeometryCfg
  acc.merge( BeamPipeGeometryCfg( flags ) )
  from AtlasGeoModel.InDetGMConfig import InDetGeometryCfg
  acc.merge( InDetGeometryCfg( flags ) )

  return acc


def sctCondCfg(flags):
  # acc = ComponentAccumulator()
  from SCT_GeoModel.SCT_GeoModelConfig import SCT_GeometryCfg
  return SCT_GeometryCfg(flags)
def pixelCondCfg(flags):
  acc = ComponentAccumulator()
  ###############
  # Pixel setup #
  ###############
  from PixelConditionsAlgorithms.PixelConditionsConfig import (
      PixelConfigCondAlgCfg, PixelChargeCalibCondAlgCfg, PixelDCSCondHVAlgCfg,
      PixelDCSCondTempAlgCfg, PixelAlignCondAlgCfg, PixelDetectorElementCondAlgCfg,
      PixelHitDiscCnfgAlgCfg, PixelReadoutSpeedAlgCfg, PixelCablingCondAlgCfg,
      PixelDCSCondStateAlgCfg, PixelDCSCondStatusAlgCfg,
      PixelDistortionAlgCfg, PixelOfflineCalibCondAlgCfg,
      PixelDeadMapCondAlgCfg
# NEW FOR RUN3    PixelChargeLUTCalibCondAlgCfg/
  )

  from PixelConditionsTools.PixelConditionsSummaryConfig import PixelConditionsSummaryCfg
  from SiPropertiesTool.PixelSiPropertiesConfig import PixelSiPropertiesCfg
  from SiLorentzAngleTool.PixelLorentzAngleConfig import PixelLorentzAngleCfg
  from PixelCabling.PixelCablingConfigNew import PixelCablingSvcCfg

  # module parameters
  acc.merge(PixelConfigCondAlgCfg(flags))
  # charge calibration
  acc.merge(PixelChargeCalibCondAlgCfg(flags))
# NEW FOR RUN3  acc.merge(PixelChargeLUTCalibCondAlgCfg(flags))
  # DCS setup
  acc.merge(PixelDCSCondHVAlgCfg(flags))
  acc.merge(PixelDCSCondTempAlgCfg(flags))
  # alignment setup
  acc.merge(PixelAlignCondAlgCfg(flags, UseDynamicAlignFolders=flags.GeoModel.Align.Dynamic))
  acc.merge(PixelDetectorElementCondAlgCfg(flags))
  # cabling setup
  acc.merge(PixelHitDiscCnfgAlgCfg(flags))
  acc.merge(PixelReadoutSpeedAlgCfg(flags))
  acc.merge(PixelCablingCondAlgCfg(flags))
  # deadmap
  acc.merge(PixelDCSCondStateAlgCfg(flags))
  acc.merge(PixelDCSCondStatusAlgCfg(flags))
  acc.merge(PixelDeadMapCondAlgCfg(flags))
  # offline calibration
  acc.merge(PixelDistortionAlgCfg(flags))
  acc.merge(PixelOfflineCalibCondAlgCfg(flags))


  acc.popToolsAndMerge(PixelConditionsSummaryCfg(flags))
  acc.popToolsAndMerge(PixelSiPropertiesCfg(flags))
  acc.popToolsAndMerge(PixelLorentzAngleCfg(flags))
  acc.merge(PixelCablingSvcCfg(flags))

  return acc

def trtCondCfg(flags):
  acc = ComponentAccumulator()
  from IOVDbSvc.IOVDbSvcConfig import addFoldersSplitOnline, addFolders
  #TODO switch to use config from TRT_ConditionsConfig
  if flags.Common.isOnline:
    acc.merge(addFolders(flags, "/TRT/Onl/ROD/Compress","TRT_ONL", className='CondAttrListCollection'))
  acc.merge(addFoldersSplitOnline(flags, "TRT","/TRT/Onl/Calib/RT","/TRT/Calib/RT",className="TRTCond::RtRelationMultChanContainer"))
  acc.merge(addFoldersSplitOnline(flags, "TRT","/TRT/Onl/Calib/T0","/TRT/Calib/T0",className="TRTCond::StrawT0MultChanContainer"))
  acc.merge(addFoldersSplitOnline (flags, "TRT","/TRT/Onl/Calib/errors","/TRT/Calib/errors",className="TRTCond::RtRelationMultChanContainer"))

  return acc

def beamposCondCfg(flags):
  acc = ComponentAccumulator()
  from IOVDbSvc.IOVDbSvcConfig import addFoldersSplitOnline
  acc.merge(addFoldersSplitOnline(flags, "INDET", "/Indet/Onl/Beampos", "/Indet/Beampos", className="AthenaAttributeList"))
  BeamSpotCondAlg=CompFactory.BeamSpotCondAlg
  acc.addCondAlgo(BeamSpotCondAlg( "BeamSpotCondAlg" ))

  return acc

def magFieldCfgCfg(flags):
  acc = ComponentAccumulator()

  acc.merge(geoModelCfg(flags))
  from MagFieldServices.MagFieldServicesConfig import MagneticFieldSvcCfg
  acc.merge( MagneticFieldSvcCfg(flags) )

  return acc


def pixelDataPrepCfg(flags, roisKey, signature):
  acc = ComponentAccumulator()
  acc.merge(pixelCondCfg(flags))
  from RegionSelector.RegSelToolConfig import regSelTool_Pixel_Cfg
  RegSelTool_Pixel = acc.popToolsAndMerge(regSelTool_Pixel_Cfg(flags))

  PixelRodDecoder=CompFactory.PixelRodDecoder
  InDetPixelRodDecoder = PixelRodDecoder(name = "InDetPixelRodDecoder"+ signature)
  # Disable duplcated pixel check for data15 because duplication mechanism was used.
  if len(flags.Input.ProjectName)>=6 and flags.Input.ProjectName[:6]=="data15":
    InDetPixelRodDecoder.CheckDuplicatedPixel=False
  acc.addPublicTool(InDetPixelRodDecoder)

  PixelRawDataProviderTool=CompFactory.PixelRawDataProviderTool
  InDetPixelRawDataProviderTool = PixelRawDataProviderTool(name    = "InDetPixelRawDataProviderTool"+ signature,
                                                           Decoder = InDetPixelRodDecoder)
  acc.addPublicTool(InDetPixelRawDataProviderTool)

   # load the PixelRawDataProvider
  PixelRawDataProvider=CompFactory.PixelRawDataProvider
  InDetPixelRawDataProvider = PixelRawDataProvider(name         = "InDetPixelRawDataProvider"+ signature,
                                                   RDOKey       = InDetKeys.PixelRDOs(),
                                                   ProviderTool = InDetPixelRawDataProviderTool,
                                                   isRoI_Seeded = True,
                                                   RoIs         = roisKey,
                                                   RDOCacheKey  = InDetCacheNames.PixRDOCacheKey,
                                                   RegSelTool   = RegSelTool_Pixel)

  acc.addEventAlgo(InDetPixelRawDataProvider)

  return acc

def sctDataPrepCfg(flags, roisKey, signature):
  acc = ComponentAccumulator()

  acc.merge(sctCondCfg(flags))

  from RegionSelector.RegSelToolConfig import regSelTool_SCT_Cfg
  RegSelTool_SCT   = acc.popToolsAndMerge(regSelTool_SCT_Cfg(flags))

  # load the SCTRawDataProvider

  from SCT_RawDataByteStreamCnv.SCT_RawDataByteStreamCnvConfig import SCTRawDataProviderCfg
  sctProviderArgs = {}
  sctProviderArgs["RDOKey"] = InDetKeys.SCT_RDOs()
  sctProviderArgs["isRoI_Seeded"] = True
  sctProviderArgs["RoIs"] = roisKey
  sctProviderArgs["RDOCacheKey"] = InDetCacheNames.SCTRDOCacheKey
  sctProviderArgs["RegSelTool"] = RegSelTool_SCT
  acc.merge(SCTRawDataProviderCfg(flags, suffix=signature, **sctProviderArgs))
  # load the SCTEventFlagWriter
  from SCT_RawDataByteStreamCnv.SCT_RawDataByteStreamCnvConfig import SCTEventFlagWriterCfg
  acc.merge(SCTEventFlagWriterCfg(flags, suffix=signature))

  return acc




def trtDataPrep(flags, roisKey, signature):
  acc = ComponentAccumulator()

  acc.merge(trtCondCfg(flags))

  from RegionSelector.RegSelToolConfig import regSelTool_TRT_Cfg
  RegSelTool_TRT = acc.popToolsAndMerge(regSelTool_TRT_Cfg(flags))

  TRT_RodDecoder=CompFactory.TRT_RodDecoder
  InDetTRTRodDecoder = TRT_RodDecoder(name = "InDetTRTRodDecoder")
  if flags.Input.isMC:
    InDetTRTRodDecoder.LoadCompressTableDB = False
    InDetTRTRodDecoder.keyName=""
  acc.addPublicTool(InDetTRTRodDecoder)

  TRTRawDataProviderTool=CompFactory.TRTRawDataProviderTool
  InDetTRTRawDataProviderTool = TRTRawDataProviderTool(name    = "InDetTRTRawDataProviderTool"+ signature,
                                                       Decoder = InDetTRTRodDecoder)
  acc.addPublicTool(InDetTRTRawDataProviderTool)

   # load the TRTRawDataProvider
  TRTRawDataProvider=CompFactory.TRTRawDataProvider
  InDetTRTRawDataProvider = TRTRawDataProvider(name         = "InDetTRTRawDataProvider"+ signature,
                                               RDOKey       = "TRT_RDOs",
                                               ProviderTool = InDetTRTRawDataProviderTool,
                                               RegSelTool   = RegSelTool_TRT,
                                               isRoI_Seeded = True,
                                               RoIs         = roisKey)

  acc.addEventAlgo(InDetTRTRawDataProvider)

  return acc


def pixelClusterizationCfg(flags, roisKey, signature):
  acc = ComponentAccumulator()
  from RegionSelector.RegSelToolConfig import regSelTool_Pixel_Cfg
  RegSelTool_Pixel = acc.popToolsAndMerge(regSelTool_Pixel_Cfg(flags))

  #Pixel clusterisation
  InDet__ClusterMakerTool=CompFactory.InDet.ClusterMakerTool
  InDetClusterMakerTool = InDet__ClusterMakerTool(name = "InDetClusterMakerTool"+ signature)

  acc.addPublicTool(InDetClusterMakerTool)


  InDet__MergedPixelsTool=CompFactory.InDet.MergedPixelsTool
  InDetMergedPixelsTool = InDet__MergedPixelsTool(name = "InDetMergedPixelsTool"+ signature,
                                                  globalPosAlg = InDetClusterMakerTool)
  # Enable duplcated RDO check for data15 because duplication mechanism was used.

  if len(flags.Input.ProjectName)>=6 and flags.Input.ProjectName[:6]=="data15":
    InDetMergedPixelsTool.CheckDuplicatedRDO = True
  acc.addPublicTool(InDetMergedPixelsTool)

  InDet__PixelGangedAmbiguitiesFinder=CompFactory.InDet.PixelGangedAmbiguitiesFinder
  InDetPixelGangedAmbiguitiesFinder = InDet__PixelGangedAmbiguitiesFinder(name = "InDetPixelGangedAmbiguitiesFinder"+ signature)
  acc.addPublicTool(InDetPixelGangedAmbiguitiesFinder)

  InDet__PixelClusterization=CompFactory.InDet.PixelClusterization
  InDetPixelClusterization = InDet__PixelClusterization(name                     = "InDetPixelClusterization"+ signature,
                                                        clusteringTool           = InDetMergedPixelsTool,
                                                        gangedAmbiguitiesFinder  = InDetPixelGangedAmbiguitiesFinder,
                                                        DataObjectName           = InDetKeys.PixelRDOs(),
                                                        AmbiguitiesMap           = 'TrigPixelClusterAmbiguitiesMap',
                                                        ClustersName             = "PixelTrigClusters",
                                                        RegSelTool               = RegSelTool_Pixel,
                                                        isRoI_Seeded             = True,
                                                        RoIs                     = roisKey,
                                                        ClusterContainerCacheKey = InDetCacheNames.Pixel_ClusterKey)

  acc.addEventAlgo(InDetPixelClusterization)

  return acc

def sctClusterizationCfg(flags, roisKey, signature):
  acc = ComponentAccumulator()

  from RegionSelector.RegSelToolConfig import regSelTool_SCT_Cfg
  RegSelTool_SCT   = acc.popToolsAndMerge(regSelTool_SCT_Cfg(flags))

  from InDetConfig.InDetRecToolConfig import InDetSCT_ConditionsSummaryToolCfg
  InDetSCT_ConditionsSummaryToolWithoutFlagged = acc.popToolsAndMerge(InDetSCT_ConditionsSummaryToolCfg(flags, withFlaggedCondTool = False, withTdaqTool = False))

  InDet__ClusterMakerTool=CompFactory.InDet.ClusterMakerTool
  InDetClusterMakerTool = InDet__ClusterMakerTool(name = "InDetClusterMakerTool"+ signature)
  acc.addPublicTool(InDetClusterMakerTool)

  #
  # --- SCT_ClusteringTool (public)
  #
  InDet__SCT_ClusteringTool=CompFactory.InDet.SCT_ClusteringTool
  InDetSCT_ClusteringTool = InDet__SCT_ClusteringTool(name              = "InDetSCT_ClusteringTool"+ signature,
                                                      globalPosAlg      = InDetClusterMakerTool,
                                                      conditionsTool    = InDetSCT_ConditionsSummaryToolWithoutFlagged)
  #
  # --- SCT_Clusterization algorithm
  #

  InDet__SCT_Clusterization=CompFactory.InDet.SCT_Clusterization
  InDetSCT_Clusterization = InDet__SCT_Clusterization(name                     = "InDetSCT_Clusterization"+ signature,
                                                      clusteringTool           = InDetSCT_ClusteringTool,
                                                      # ChannelStatus          = InDetSCT_ChannelStatusAlg,
                                                      DataObjectName           = InDetKeys.SCT_RDOs(),
                                                      ClustersName             = "SCT_TrigClusters",
                                                      conditionsTool           = InDetSCT_ConditionsSummaryToolWithoutFlagged,
                                                      isRoI_Seeded             = True,
                                                      RoIs                     = roisKey,
                                                      ClusterContainerCacheKey = InDetCacheNames.SCT_ClusterKey,
                                                      FlaggedCondCacheKey      = InDetCacheNames.SCTFlaggedCondCacheKey,
                                                      RegSelTool               = RegSelTool_SCT)

  acc.addEventAlgo(InDetSCT_Clusterization)

  return acc

def spacePointsMakingCfg(flags, signature):
  acc = ComponentAccumulator()
  #Space points

  InDet__SiSpacePointMakerTool=CompFactory.InDet.SiSpacePointMakerTool
  InDetSiSpacePointMakerTool = InDet__SiSpacePointMakerTool(name = "InDetSiSpacePointMakerTool"+ signature)
  acc.addPublicTool(InDetSiSpacePointMakerTool)

  acc.addCondAlgo( CompFactory.InDet.SiElementPropertiesTableCondAlg(name = "InDetSiElementPropertiesTableCondAlg") )

  InDet__SiTrackerSpacePointFinder=CompFactory.InDet.SiTrackerSpacePointFinder
  InDetSiTrackerSpacePointFinder = InDet__SiTrackerSpacePointFinder(name                   = "InDetSiTrackerSpacePointFinder_"+ signature,
                                                                    SiSpacePointMakerTool  = InDetSiSpacePointMakerTool,
                                                                    PixelsClustersName     = "PixelTrigClusters",
                                                                    SCT_ClustersName       = "SCT_TrigClusters",
                                                                    SpacePointsPixelName   = "PixelTrigSpacePoints",
                                                                    SpacePointsSCTName     = "SCT_TrigSpacePoints",
                                                                    SpacePointsOverlapName = InDetKeys.OverlapSpacePoints(),
                                                                    ProcessPixels          = flags.Detector.RecoPixel,
                                                                    ProcessSCTs            = flags.Detector.RecoSCT,
                                                                    ProcessOverlaps        = flags.Detector.RecoSCT,
                                                                    SpacePointCacheSCT     = InDetCacheNames.SpacePointCacheSCT,
                                                                    SpacePointCachePix     = InDetCacheNames.SpacePointCachePix,)
  acc.addEventAlgo(InDetSiTrackerSpacePointFinder)

  return acc


def __trackCollName(signatureName):
    return "HLT_IDTrkTrack_"+signatureName+"_FTF"

def ftfCfg(flags, roisKey, signature, signatureName):
  acc = ComponentAccumulator()

  acc.merge( TrackSummaryToolCfg(flags, name="InDetTrigFastTrackSummaryTool") )

  acc.merge( SiTrackMaker_xkCfg( flags, name = "InDetTrigSiTrackMaker_FTF"+signature ) )

  acc.addPublicTool( CompFactory.TrigInDetTrackFitter( "TrigInDetTrackFitter" ) )
  from RegionSelector.RegSelToolConfig import (regSelTool_SCT_Cfg, regSelTool_Pixel_Cfg)

  pixRegSelTool = acc.popToolsAndMerge( regSelTool_Pixel_Cfg( flags) )
  sctRegSelTool = acc.popToolsAndMerge( regSelTool_SCT_Cfg( flags) )

  from TrkConfig.AtlasTrackingGeometrySvcConfig import TrackingGeometrySvcCfg
  acc.merge( TrackingGeometrySvcCfg( flags ) )
  acc.addPublicTool( CompFactory.TrigL2LayerNumberTool( name = "TrigL2LayerNumberTool_FTF",
                                                        UseNewLayerScheme = True) )
  acc.addPublicTool( CompFactory.TrigL2LayerNumberTool( "TrigL2LayerNumberTool_FTF" ) )

  acc.addPublicTool( CompFactory.TrigSpacePointConversionTool( "TrigSpacePointConversionTool" + signature,
                                                                 DoPhiFiltering    = True,
                                                                 UseBeamTilt       = False,
                                                                 UseNewLayerScheme = True,
                                                                 RegSelTool_Pixel  = pixRegSelTool,
                                                                 RegSelTool_SCT    = sctRegSelTool,
                                                                 layerNumberTool   = acc.getPublicTool("TrigL2LayerNumberTool_FTF") ) )

  # TODO remove once offline configured counterparts are available?
  acc.addCondAlgo( CompFactory.InDet.SiDetElementBoundaryLinksCondAlg_xk(name= "InDetSiDetElementBoundaryLinksSCTCondAlg",
                                                                         WriteKey = "SCT_DetElementBoundaryLinks_xk") )

  acc.addCondAlgo( CompFactory.InDet.SiDetElementBoundaryLinksCondAlg_xk(name = "InDetSiDetElementBoundaryLinksPixelCondAlg",
                                                                         ReadKey  = "PixelDetectorElementCollection",
                                                                         WriteKey = "PixelDetElementBoundaryLinks_xk") )

  ftf = CompFactory.TrigFastTrackFinder( name = "TrigFastTrackFinder_" + signature,
                                         LayerNumberTool          = acc.getPublicTool( "TrigL2LayerNumberTool_FTF" ),
                                         SpacePointProviderTool   = acc.getPublicTool( "TrigSpacePointConversionTool" + signature ),
                                         TrackSummaryTool         = acc.getPublicTool( "InDetTrigFastTrackSummaryTool" ),
#                                         TrigL2ResidualCalculator = acc.getPublicTool( "TrigL2ResidualCalculator" ),
                                         initialTrackMaker        = acc.getPublicTool( "InDetTrigSiTrackMaker_FTF" + signature ),
                                         trigInDetTrackFitter     = acc.getPublicTool( "TrigInDetTrackFitter" ),
                                         RoIs                     = roisKey,
                                         trigZFinder              = CompFactory.TrigZFinder(),
                                         doZFinder                = False,
                                         SeedRadBinWidth          =  flags.InDet.Tracking.seedRadBinWidth,
                                         TrackInitialD0Max        = 1000. if flags.InDet.Tracking.extension == 'cosmics' else 20.0,
                                         TracksName               = __trackCollName(signatureName),
                                         TripletDoPSS             = False,
                                         Triplet_D0Max            = flags.InDet.Tracking.d0SeedMax,
                                         Triplet_D0_PPS_Max       = flags.InDet.Tracking.d0SeedPPSMax,
                                         Triplet_MaxBufferLength  = 3,
                                         Triplet_MinPtFrac        = 1,
                                         Triplet_nMaxPhiSlice     = 53,
                                         doCloneRemoval           = flags.InDet.Tracking.doCloneRemoval,
                                         doResMon                 = flags.InDet.Tracking.doResMon,
                                         doSeedRedundancyCheck    = flags.InDet.Tracking.checkRedundantSeeds,
                                         pTmin                    = flags.InDet.Tracking.minPT,
                                         useNewLayerNumberScheme  = True,
                                         MinHits                  = 5,
                                         useGPU                   = False,
                                         DoubletDR_Max            = 270)
  acc.addEventAlgo( ftf )


  return acc

def TrigTrackToVertexCfg(flags, name = 'TrigTrackToVertexTool', **kwargs ):
    acc = ComponentAccumulator()
    if 'Extrapolator' not in kwargs:
      from InDetConfig.InDetRecToolConfig import InDetExtrapolatorCfg
      extrapolator_acc = InDetExtrapolatorCfg( flags, name = "InDetTrigExtrapolator" )
      extrapolator = extrapolator_acc.getPrimary()
      acc.merge(extrapolator_acc)
      kwargs.setdefault('Extrapolator', extrapolator) # @TODO or atlas extrapolator ?
    tool = CompFactory.Reco.TrackToVertex( name, **kwargs)
    acc.setPrivateTools(tool)
    return acc

def trackConverterCfg(flags, signature, signatureName):
  acc = ComponentAccumulator()

  acc.merge( TrackSummaryToolCfg(flags, name="InDetTrigFastTrackSummaryTool") )
  track_to_vertex = acc.popToolsAndMerge( TrigTrackToVertexCfg(flags) )
  creatorTool = CompFactory.Trk.TrackParticleCreatorTool( name = "InDetTrigParticleCreatorToolFTF",
                                                          TrackSummaryTool      = acc.getPublicTool( "InDetTrigFastTrackSummaryTool" ),
                                                          TrackToVertex         = track_to_vertex,
                                                          KeepParameters        = True,
                                                          ComputeAdditionalInfo = True,
                                                          ExtraSummaryTypes     = ['eProbabilityComb', 'eProbabilityHT', 'TRTTrackOccupancy', 'TRTdEdx', 'TRTdEdxUsedHits'])
  acc.addPublicTool(creatorTool)
  trackParticleCnv=CompFactory.InDet.TrigTrackingxAODCnvMT(name = "InDetTrigTrackParticleCreatorAlg" + signature,
                                                          TrackName           = __trackCollName(signatureName),
                                                          TrackParticlesName  = recordable("HLT_IDTrack_"+signatureName+"_FTF"),
                                                          ParticleCreatorTool = creatorTool)

  acc.addEventAlgo(trackParticleCnv)

  return acc

def trigInDetFastTrackingCfg( inflags, roisKey="EMRoIs", signatureName='' ):

  # redirect InDet.Tracking flags to point to a specific trigger setting
  if 'Muon' in signatureName:
    signatureFlags='Muon'
  else:
    signatureFlags = signatureName

  flags = inflags.cloneAndReplace("InDet.Tracking", "Trigger.InDetTracking."+signatureFlags)

  #If signature specified add suffix to the name of each algorithms
  signature =  ("_" + signatureName if signatureName else '').lower()

  acc = ComponentAccumulator()
  acc.merge(magFieldCfgCfg(flags))

  # TODO merge it to the component needing it
  acc.merge(beamposCondCfg(flags))


  verifier = CompFactory.AthViews.ViewDataVerifier( name = 'VDVInDetFTF'+signature,
                                                    DataObjects= [('xAOD::EventInfo', 'StoreGateSvc+EventInfo'),
                                                                  ('InDet::PixelClusterContainerCache', 'PixelTrigClustersCache'),
                                                                  ('PixelRDO_Cache', 'PixRDOCache'),
                                                                  ('InDet::SCT_ClusterContainerCache', 'SCT_ClustersCache'),
                                                                  ('SCT_RDO_Cache', 'SctRDOCache'),
                                                                  ('SpacePointCache', 'PixelSpacePointCache'),
                                                                  ('SpacePointCache', 'SctSpacePointCache'),
                                                                  ('IDCInDetBSErrContainer_Cache', 'SctBSErrCache'),
                                                                  ('IDCInDetBSErrContainer_Cache', 'SctFlaggedCondCache'),
                                                                  ('xAOD::EventInfo', 'EventInfo'),
                                                                  ('TrigRoiDescriptorCollection', roisKey),
                                                                  ( 'TagInfo' , 'DetectorStore+ProcessingTags' )] )

  acc.addEventAlgo(verifier)
  #Only add raw data decoders if we're running over raw data
  acc.merge(pixelDataPrepCfg(flags, roisKey, signature))
  acc.merge(sctDataPrepCfg(flags, roisKey, signature))
  acc.merge(trtDataPrep(flags, roisKey, signature))

  acc.merge(pixelClusterizationCfg(flags, roisKey, signature))
  acc.merge(sctClusterizationCfg(flags, roisKey, signature))
  acc.merge(spacePointsMakingCfg(flags, signature))
  acc.merge(ftfCfg(flags, roisKey, signature, signatureName))
  acc.merge(trackConverterCfg(flags, signature, signatureName))
  return acc

if __name__ == "__main__":
    from AthenaCommon.Configurable import Configurable
    Configurable.configurableRun3Behavior=1

    from AthenaConfiguration.AllConfigFlags import ConfigFlags
    from AthenaConfiguration.TestDefaults import defaultTestFiles

    ConfigFlags.Input.Files = defaultTestFiles.RAW
    ConfigFlags.lock()
    # this configuration is not runable, the test checks if there is no mistake in python scripts above
    # output can be used by experts to check actual configuration (e.g. here we configure to run on RAW and it should be reflected in settings)
    from AthenaConfiguration.MainServicesConfig import MainServicesCfg
    acc = MainServicesCfg( ConfigFlags )
    acc.merge( trigInDetFastTrackingCfg( ConfigFlags, roisKey="ElectronRoIs", signatureName="Electron" ) )

    acc.printConfig(withDetails=True, summariseProps=True)
    acc.store( open("test.pkl", "wb") )
