#
#  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
#

from AthenaConfiguration.ComponentAccumulator import ComponentAccumulator
from AthenaConfiguration.ComponentFactory import CompFactory
from AthenaConfiguration.Enums import Format
from InDetRecExample.InDetKeys import InDetKeys


def RungeKuttaPropagatorCfg(flags, name="InDetTrigPatternPropagator"):
  acc = ComponentAccumulator()
  acc.addPublicTool( CompFactory.Trk.RungeKuttaPropagator( name ), primary=True )
  return acc

def SiDetElementsRoadMaker_xkCfg( flags, name="InDetTrigSiDetElementsRoadMaker" ):
  """
  based  on: InnerDetector/InDetExample/InDetTrigRecExample/python/InDetTrigConfigRecLoadTools.py, should be moved elsewhere
  """
  acc = ComponentAccumulator()
  acc.addCondAlgo( CompFactory.InDet.SiDetElementsRoadCondAlg_xk() )
  tool = CompFactory.InDet.SiDetElementsRoadMaker_xk( name,
                                                      PropagatorTool = acc.getPrimaryAndMerge(RungeKuttaPropagatorCfg( flags )),
                                                      usePixel     = flags.Detector.EnablePixel,
                                                      useSCT       = flags.Detector.EnableSCT,
                                                      RoadWidth    = flags.InDet.Tracking.ActivePass.roadWidth,
                                                        )
  acc.addPublicTool( tool, primary=True )
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
                                                      useToT                       = False,
                                                      NnCollectionReadKey          = 'PixelClusterNN',
                                                      NnCollectionWithTrackReadKey = 'PixelClusterNNWithTrack')
  from .InDetTrigCollectionKeys import TrigPixelKeys
  tool = CompFactory.InDet.PixelClusterOnTrackTool( name,
                                                    ErrorStrategy = 2,
                                                    LorentzAngleTool = pixelLATool,
                                                    NnClusterizationFactory = nnTool, 
                                                    SplitClusterAmbiguityMap = TrigPixelKeys.PixelClusterAmbiguitiesMap )
  acc.addPublicTool( tool, primary=True )
  return acc


def SCT_ClusterOnTrackToolCfg( flags ):
  acc = ComponentAccumulator()
  from SiLorentzAngleTool.SCT_LorentzAngleConfig import SCT_LorentzAngleCfg
  sctLATool =  acc.popToolsAndMerge( SCT_LorentzAngleCfg( flags ) )
  tool = CompFactory.InDet.SCT_ClusterOnTrackTool("SCT_ClusterOnTrackTool",
                                                    CorrectionStrategy = 0,  # do correct position bias
                                                    ErrorStrategy      = 2,  # do use phi dependent errors
                                                    LorentzAngleTool   = sctLATool # default name
                                                    )
  acc.addPublicTool ( tool, primary=True )
  return acc

def RIO_OnTrackCreatorCfg( flags, name="InDetTrigRotCreator" ):
  acc = ComponentAccumulator()
  pixelTool = acc.getPrimaryAndMerge( PixelClusterOnTrackCfg( flags ) )
  sctTool = acc.getPrimaryAndMerge( SCT_ClusterOnTrackToolCfg( flags ) )
  tool = CompFactory.Trk.RIO_OnTrackCreator(name,
                                            ToolPixelCluster = pixelTool,
                                            ToolSCT_Cluster  = sctTool,
                                            Mode             = 'indet')
  acc.addPublicTool( tool, primary=True )
  return acc

def KalmanxkUpdatorCfg(flags):
  acc = ComponentAccumulator()
  acc.addPublicTool(CompFactory.Trk.KalmanUpdator_xk( "InDetTrigPatternUpdator" ), primary=True )
  return acc

def SiCombinatorialTrackFinder_xkCfg( flags, name="InDetTrigSiComTrackFinder" ):
  """
  based  on: InnerDetector/InDetExample/InDetTrigRecExample/python/InDetTrigConfigRecLoadTools.py, should be moved elsewhere
  """
  acc = ComponentAccumulator()
  propagatorTool = acc.getPrimaryAndMerge( RungeKuttaPropagatorCfg( flags ) )  
  patternUpdatorTool = acc.getPrimaryAndMerge( KalmanxkUpdatorCfg( flags ) )
  rioOnTrackTool = acc.getPrimaryAndMerge( RIO_OnTrackCreatorCfg( flags ) )

  from PixelConditionsTools.PixelConditionsSummaryConfig import PixelConditionsSummaryCfg
  pixelCondSummaryTool = acc.popToolsAndMerge( PixelConditionsSummaryCfg(flags) )

  from SCT_ConditionsTools.SCT_ConditionsToolsConfig import SCT_ConditionsSummaryToolCfg
  sctCondSummaryTool = acc.popToolsAndMerge( SCT_ConditionsSummaryToolCfg( flags, withFlaggedCondTool=False, withTdaqTool=False ) )


  tool = CompFactory.InDet.SiCombinatorialTrackFinder_xk(name,
                                                         PropagatorTool        = propagatorTool,
                                                         UpdatorTool           = patternUpdatorTool,
                                                         RIOonTrackTool        = rioOnTrackTool,
                                                         usePixel              = flags.Detector.EnablePixel,
                                                         useSCT                = flags.Detector.EnableSCT,
                                                         PixelClusterContainer = 'PixelTrigClusters',
                                                         SCT_ClusterContainer  = 'SCT_TrigClusters',
                                                         PixelSummaryTool      = pixelCondSummaryTool,
                                                         SctSummaryTool        = sctCondSummaryTool
                                                        )
  acc.setPrivateTools( tool )
  return acc

def SiTrackMaker_xkCfg(flags, name="SiTrackMaker_xk"):
  """
  based on: InnerDetector/InDetExample/InDetTrigRecExample/python/InDetTrigConfigRecNewTracking.py , should be moved elsewhere
  """
  acc = ComponentAccumulator()
  roadTool = acc.getPrimaryAndMerge( SiDetElementsRoadMaker_xkCfg( flags ) )
  combTrackFinderTool = acc.popToolsAndMerge( SiCombinatorialTrackFinder_xkCfg( flags ) )

  from IOVDbSvc.IOVDbSvcConfig import addFoldersSplitOnline
  acc.merge(addFoldersSplitOnline( flags, "INDET", '/Indet/Onl/TrkErrorScaling', '/Indet/TrkErrorScaling', className="CondAttrListCollection") )

  acc.addCondAlgo( CompFactory.RIO_OnTrackErrorScalingCondAlg(ErrorScalingType = ["PixelRIO_OnTrackErrorScaling", "SCTRIO_OnTrackErrorScaling", "TRTRIO_OnTrackErrorScaling"],
                                                              OutKeys        = ["ConditionStore+/Indet/TrkErrorScalingPixel", "ConditionStore+/Indet/TrkErrorScalingSCT", "ConditionStore+/Indet/TrkErrorScalingTRT"],
                                                              ReadKey        = "ConditionStore+/Indet/TrkErrorScaling") )


  tool = CompFactory.InDet.SiTrackMaker_xk( name,
                                            RoadTool                 = roadTool,
                                            CombinatorialTrackFinder = combTrackFinderTool,
                                            pTmin                    = flags.InDet.Tracking.ActivePass.minPT,
                                            nClustersMin             = flags.InDet.Tracking.ActivePass.minClusters,
                                            nHolesMax                = flags.InDet.Tracking.ActivePass.nHolesMax,
                                            nHolesGapMax             = flags.InDet.Tracking.ActivePass.nHolesGapMax,
                                            SeedsFilterLevel         = flags.InDet.Tracking.ActivePass.seedFilterLevel,
                                            Xi2max                   = flags.InDet.Tracking.ActivePass.Xi2max,
                                            Xi2maxNoAdd              = flags.InDet.Tracking.ActivePass.Xi2maxNoAdd,
                                            nWeightedClustersMin     = flags.InDet.Tracking.ActivePass.nWeightedClustersMin,
                                            Xi2maxMultiTracks        = flags.InDet.Tracking.ActivePass.Xi2max,
                                            UseAssociationTool       = False )
  acc.addPublicTool( tool, primary=True )
  return acc



def ExtrapolatorCfg(flags):
  from TrkConfig.AtlasExtrapolatorConfig import InDetExtrapolatorCfg
  return InDetExtrapolatorCfg(flags, name="InDetTrigExtrapolator")

def InDetTestPixelLayerToolCfg(flags):
  acc = ComponentAccumulator()
  from PixelConditionsTools.PixelConditionsSummaryConfig import PixelConditionsSummaryCfg

  tool = CompFactory.InDet.InDetTestPixelLayerTool(PixelSummaryTool = acc.popToolsAndMerge( PixelConditionsSummaryCfg(flags) ),
                                                   Extrapolator     = acc.getPrimaryAndMerge(ExtrapolatorCfg( flags)), 
                                                   CheckActiveAreas = True,
                                                   CheckDeadRegions = True)
  acc.setPrivateTools( tool )
  return acc


def InDetHoleSearchToolCfg(flags, name="InDetTrigHoleSearchTool"):
  acc = ComponentAccumulator()

# a possible change in HoleSearchTool impl? - This two tools do not seem to be needed now, leaving them commented out  TODO - decide if can be removed ( also func above creting the config )
#  from SCT_ConditionsTools.SCT_ConditionsToolsConfig import SCT_ConditionsSummaryToolCfg
#  sctCondSummaryTool = acc.popToolsAndMerge( SCT_ConditionsSummaryToolCfg( flags,withFlaggedCondTool=False, withTdaqTool=False ) )

#  acc.merge( InDetTestPixelLayerToolCfg( flags, **kwargs ) )

  extrapolatorTool = acc.popToolsAndMerge( ExtrapolatorCfg( flags ) )
  acc.addPublicTool(extrapolatorTool)

  tool = CompFactory.InDet.InDetTrackHoleSearchTool(name,
                                                    Extrapolator =  extrapolatorTool)
  acc.addPublicTool( tool, primary=True )
  return acc

def InDetPrdAssociationToolGangedPixelsCfg(flags):
  acc = ComponentAccumulator()
  from .InDetTrigCollectionKeys import TrigPixelKeys
  tool = CompFactory.InDet.InDetPRD_AssociationToolGangedPixels(
    name = "InDetTrigPrdAssociationTool",
    PixelClusterAmbiguitiesMapName = TrigPixelKeys.PixelClusterAmbiguitiesMap,
    )
  acc.addPublicTool(tool, primary=True)
  return acc

def TestBlayerToolCfg(flags):
  acc = ComponentAccumulator()
  from PixelConditionsTools.PixelConditionsSummaryConfig import PixelConditionsSummaryCfg
  tool = CompFactory.InDet.InDetTestBLayerTool("InDetTrigTestBLayerTool",
                                                PixelSummaryTool = acc.popToolsAndMerge( PixelConditionsSummaryCfg(flags) ),
                                                Extrapolator    = acc.getPrimaryAndMerge( ExtrapolatorCfg( flags ) ),
                                                CheckActiveAreas= True 
                                                )
  acc.setPrivateTools(tool)

  return acc

def InDetTrackSummaryHelperToolCfg(flags, name="InDetTrigSummaryHelper"):
  """
  based on: InnerDetector/InDetExample/InDetTrigRecExample/python/InDetTrigConfigRecLoadTools.py
  """
  acc = ComponentAccumulator()
  holeSearchTool = acc.getPrimaryAndMerge( InDetHoleSearchToolCfg(flags, name = "InDetTrigHoleSearchTool" ) )
  associationTool = acc.getPrimaryAndMerge( InDetPrdAssociationToolGangedPixelsCfg(flags) )

  from TRT_ConditionsAlgs.TRT_ConditionsAlgsConfig import TRTStrawCondAlgCfg
  acc.merge( TRTStrawCondAlgCfg(flags) )

  from TRT_ConditionsServices.TRT_ConditionsServicesConfig import TRT_StrawStatusSummaryToolCfg
  tool = CompFactory.InDet.InDetTrackSummaryHelperTool(name,
                                                       HoleSearch    = holeSearchTool,
                                                       AssoTool      = associationTool,
                                                       TestBLayerTool = acc.popToolsAndMerge(TestBlayerToolCfg(flags)),
                                                       PixelToTPIDTool= None,
                                                       DoSharedHits  = True,
                                                       TRTStrawSummarySvc = acc.popToolsAndMerge( TRT_StrawStatusSummaryToolCfg(flags) ),
                                                       usePixel      = flags.Detector.EnablePixel,
                                                       useSCT        = flags.Detector.EnableSCT,
                                                       useTRT        = flags.Detector.EnableTRT                                                      
                                                      )

  acc.setPrivateTools( tool )
  return acc

def TrackSummaryToolCfg(flags, name="InDetTrigTrackSummaryTool", summaryHelperTool=None, makePublic=True, useTRT=False):
  acc = ComponentAccumulator()
  if not summaryHelperTool:
    summaryHelperTool = acc.popToolsAndMerge( InDetTrackSummaryHelperToolCfg( flags, "InDetTrigSummaryHelper") )

  tool = CompFactory.Trk.TrackSummaryTool(name = name,
                                          InDetSummaryHelperTool = summaryHelperTool,
                                          doSharedHits           = True,
                                          doHolesInDet           = True,
                                          #this may be temporary #61512 (and used within egamma later)
                                          #TRT_ElectronPidTool    = InDetTrigTRT_ElectronPidTool,
                                          TRT_ElectronPidTool    = None
                                          )
  if makePublic:
    acc.addPublicTool( tool, primary=True )
  else:
    acc.setPrivateTools(tool)
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
  from InDetConfig.InDetGeometryConfig import InDetGeometryCfg
  acc.merge( InDetGeometryCfg( flags ) )

  return acc


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
  from PixelReadoutGeometry.PixelReadoutGeometryConfig import PixelReadoutManagerCfg
  from SiPropertiesTool.PixelSiPropertiesConfig import PixelSiPropertiesCfg
  from SiLorentzAngleTool.PixelLorentzAngleConfig import PixelLorentzAngleCfg

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
  acc.merge(PixelReadoutManagerCfg(flags))

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

  if flags.Input.Format is Format.BS:
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

  from RegionSelector.RegSelToolConfig import regSelTool_SCT_Cfg
  RegSelTool_SCT   = acc.popToolsAndMerge(regSelTool_SCT_Cfg(flags))

  # load the SCTRawDataProvider

  if flags.Input.Format is Format.BS:
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

  if flags.Input.Format is Format.BS:
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
    from .InDetTrigCollectionKeys import TrigTRTKeys
    TRTRawDataProvider=CompFactory.TRTRawDataProvider
    InDetTRTRawDataProvider = TRTRawDataProvider(name         = "InDetTRTRawDataProvider"+ signature,
                                                 RDOKey       = TrigTRTKeys.RDOs,
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
  from .InDetTrigCollectionKeys import TrigPixelKeys
  InDetPixelClusterization = InDet__PixelClusterization(name                     = "InDetPixelClusterization"+ signature,
                                                        clusteringTool           = InDetMergedPixelsTool,
                                                        gangedAmbiguitiesFinder  = InDetPixelGangedAmbiguitiesFinder,
                                                        DataObjectName           = InDetKeys.PixelRDOs(),
                                                        AmbiguitiesMap           = TrigPixelKeys.PixelClusterAmbiguitiesMap,
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

  from SCT_ConditionsTools.SCT_ConditionsToolsConfig import SCT_ConditionsSummaryToolCfg
  InDetSCT_ConditionsSummaryToolWithoutFlagged = acc.popToolsAndMerge(SCT_ConditionsSummaryToolCfg(flags, withFlaggedCondTool = False, withTdaqTool = False))

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
                                                                    ProcessPixels          = flags.Detector.EnablePixel,
                                                                    ProcessSCTs            = flags.Detector.EnableSCT,
                                                                    ProcessOverlaps        = flags.Detector.EnableSCT,
                                                                    SpacePointCacheSCT     = InDetCacheNames.SpacePointCacheSCT,
                                                                    SpacePointCachePix     = InDetCacheNames.SpacePointCachePix,)
  acc.addEventAlgo(InDetSiTrackerSpacePointFinder)

  return acc


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
                                         SeedRadBinWidth          =  flags.InDet.Tracking.ActivePass.SeedRadBinWidth,
                                         TrackInitialD0Max        = 1000. if flags.InDet.Tracking.ActivePass.extension == 'cosmics' else 20.0,
                                         TracksName               = flags.InDet.Tracking.ActivePass.trkTracks_FTF,
                                         TripletDoPSS             = False,
                                         Triplet_D0Max            = flags.InDet.Tracking.ActivePass.Triplet_D0Max,
                                         Triplet_D0_PPS_Max       = flags.InDet.Tracking.ActivePass.Triplet_D0_PPS_Max,
                                         Triplet_MaxBufferLength  = 3,
                                         Triplet_MinPtFrac        = 1,
                                         Triplet_nMaxPhiSlice     = 53,
                                         doCloneRemoval           = flags.InDet.Tracking.ActivePass.doCloneRemoval,
                                         doResMon                 = flags.InDet.Tracking.ActivePass.doResMon,
                                         doSeedRedundancyCheck    = flags.InDet.Tracking.ActivePass.doSeedRedundancyCheck,
                                         pTmin                    = flags.InDet.Tracking.ActivePass.minPT,
                                         useNewLayerNumberScheme  = True,
                                         MinHits                  = 5,
                                         useGPU                   = False,
                                         DoubletDR_Max            = 270)
  acc.addEventAlgo( ftf, primary=True )


  return acc

def TrigTrackToVertexCfg(flags, name = 'TrigTrackToVertexTool', **kwargs ):
    acc = ComponentAccumulator()
    if 'Extrapolator' not in kwargs:
      extrapolator_acc = ExtrapolatorCfg( flags )
      extrapolator = extrapolator_acc.getPrimary()
      acc.merge(extrapolator_acc)
      kwargs.setdefault('Extrapolator', extrapolator) # @TODO or atlas extrapolator ?
    tool = CompFactory.Reco.TrackToVertex( name, **kwargs)
    acc.setPrivateTools(tool)
    return acc

def _trackConverterCfg(flags, signature, inputTracksKey, outputTrackParticleKey):
  acc = ComponentAccumulator()

  summaryTool = acc.getPrimaryAndMerge( TrackSummaryToolCfg(flags, name="InDetTrigFastTrackSummaryTool") )
  track_to_vertex = acc.popToolsAndMerge( TrigTrackToVertexCfg(flags) )
  creatorTool = CompFactory.Trk.TrackParticleCreatorTool( name = "InDetTrigParticleCreatorToolFTF",
                                                          TrackSummaryTool      = summaryTool,
                                                          TrackToVertex         = track_to_vertex,
                                                          KeepParameters        = True,
                                                          ComputeAdditionalInfo = True,
                                                          ExtraSummaryTypes     = ['eProbabilityComb', 'eProbabilityHT', 'TRTTrackOccupancy', 'TRTdEdx', 'TRTdEdxUsedHits'])
  acc.addPublicTool(creatorTool)
  from TrigEDMConfig.TriggerEDMRun3 import recordable
  trackParticleCnv=CompFactory.InDet.TrigTrackingxAODCnvMT(name = "InDetTrigTrackParticleCreatorAlg" + signature,
                                                          TrackName           = inputTracksKey,
                                                          TrackParticlesName  = recordable(outputTrackParticleKey),
                                                          ParticleCreatorTool = creatorTool)
  acc.addEventAlgo(trackParticleCnv, primary=True)

  return acc

def trackFTFConverterCfg(flags, signature):
  return _trackConverterCfg(flags, signature, flags.InDet.Tracking.ActivePass.trkTracks_FTF, flags.InDet.Tracking.ActivePass.tracks_FTF)


def trigInDetFastTrackingCfg( inflags, roisKey="EMRoIs", signatureName='', in_view=True ):
  """ Generates precision fast tracking config, it is a primary config function """

  # redirect InDet.Tracking.ActivePass flags to point to a specific trigger setting

  flags = inflags.cloneAndReplace("InDet.Tracking.ActivePass", "Trigger.InDetTracking."+signatureName)

  #If signature specified add suffix to the name of each algorithms
  signature =  ("_" + signatureName if signatureName else '').lower()

  acc = ComponentAccumulator()
  acc.merge(magFieldCfgCfg(flags))

  # TODO merge it to the component needing it
  acc.merge(beamposCondCfg(flags))


  if in_view:
    verifier = CompFactory.AthViews.ViewDataVerifier( name = 'VDVInDetFTF'+signature,
                                                      DataObjects= [('xAOD::EventInfo', 'StoreGateSvc+EventInfo'),
                                                                    ('InDet::PixelClusterContainerCache', 'PixelTrigClustersCache'),
                                                                    ('PixelRDO_Cache', 'PixRDOCache'),
                                                                    ('InDet::SCT_ClusterContainerCache', 'SCT_ClustersCache'),
                                                                    ('SCT_RDO_Cache', 'SctRDOCache'),
                                                                    ( 'IDCInDetBSErrContainer_Cache' , InDetCacheNames.PixBSErrCacheKey ),
                                                                    ( 'IDCInDetBSErrContainer_Cache' , InDetCacheNames.SCTBSErrCacheKey ),
                                                                    ( 'IDCInDetBSErrContainer_Cache' , InDetCacheNames.SCTFlaggedCondCacheKey ),
                                                                    ('SpacePointCache', 'PixelSpacePointCache'),
                                                                    ('SpacePointCache', 'SctSpacePointCache'),
                                                                    ('xAOD::EventInfo', 'EventInfo'),
                                                                    ('TrigRoiDescriptorCollection', str(roisKey)),
                                                                    ( 'TagInfo' , 'DetectorStore+ProcessingTags' )] )
    if flags.Input.isMC:
        verifier.DataObjects += [( 'PixelRDO_Container' , 'StoreGateSvc+PixelRDOs' ),
                                  ( 'SCT_RDO_Container' , 'StoreGateSvc+SCT_RDOs' ) ]
        from SGComps.SGInputLoaderConfig import SGInputLoaderCfg
        sgil_load = [( 'PixelRDO_Container' , 'StoreGateSvc+PixelRDOs' ),
                     ( 'SCT_RDO_Container' , 'StoreGateSvc+SCT_RDOs' ) ]
        acc.merge(SGInputLoaderCfg(flags, Load=sgil_load))

    acc.addEventAlgo(verifier)
  #Only add raw data decoders if we're running over raw data
  acc.merge(pixelDataPrepCfg(flags, roisKey, signature))
  acc.merge(sctDataPrepCfg(flags, roisKey, signature))
  acc.merge(trtDataPrep(flags, roisKey, signature))

  acc.merge(pixelClusterizationCfg(flags, roisKey, signature))
  acc.merge(sctClusterizationCfg(flags, roisKey, signature))
  acc.merge(spacePointsMakingCfg(flags, signature))
  acc.merge(ftfCfg(flags, roisKey, signature, signatureName))
  acc.merge(trackFTFConverterCfg(flags, signature))
  return acc

############################################################################################################################
# precision tracking
############################################################################################################################
prefix="InDetTrigMT"

def TRTDriftCircleCutCfg(flags):
  from TRT_ConditionsAlgs.TRT_ConditionsAlgsConfig import TRTActiveCondAlgCfg
  acc = TRTActiveCondAlgCfg(flags)
  tool = CompFactory.InDet.InDetTrtDriftCircleCutTool('InDetTrigTRTDriftCircleCut',
          MinOffsetDCs     = 5,
          UseNewParameterization = True,
          UseActiveFractionSvc   = True #DetFlags.haveRIO.TRT_on()  # Use Thomas's new parameterization by default
  )
  acc.addPublicTool(tool, primary=True)
  return acc

def TRTDataProviderCfg(flags):
  acc = ComponentAccumulator()
  rodDecoder = CompFactory.TRT_RodDecoder(f"{prefix}TRTRodDecoder{flags.InDet.Tracking.ActivePass.name}",
                                          LoadCompressTableDB=True)
  acc.addPublicTool( rodDecoder )
  dataProviderTool = CompFactory.TRTRawDataProviderTool(f"{prefix}TRTRawDataProviderTool{flags.InDet.Tracking.ActivePass.name}",
                                                    Decoder=rodDecoder)

  acc.addPublicTool( dataProviderTool )
  from .InDetTrigCollectionKeys import TrigTRTKeys
  from RegionSelector.RegSelToolConfig import regSelTool_TRT_Cfg
  dataProviderAlg = CompFactory.TRTRawDataProvider(f"{prefix}TRTRawDataProvider{flags.InDet.Tracking.ActivePass.name}",
                                                   RDOKey       = TrigTRTKeys.RDOs,
                                                   ProviderTool = dataProviderTool,
                                                   RegSelTool   = acc.popToolsAndMerge( regSelTool_TRT_Cfg(flags)),
                                                   isRoI_Seeded = True,
                                                   RoIs         = flags.InDet.Tracking.ActivePass.roi )
  acc.addEventAlgo(dataProviderAlg)
  return acc

def TRTRIOMakerCfg(flags):
  acc = ComponentAccumulator()
  from .InDetTrigCollectionKeys import TrigTRTKeys
  TRT_RDO_Key = "TRT_RDOs"
  if flags.Input.Format is Format.BS:
        TRT_RDO_Key = TrigTRTKeys.RDOs
  
  from InDetConfig.TRTPreProcessing import TRT_DriftCircleToolCfg # TODO, offline config used here, threfore the names are different
  alg = CompFactory.InDet.TRT_RIO_Maker( f"{prefix}TRTDriftCircleMaker_{flags.InDet.Tracking.ActivePass.name}",
                                          TRTRIOLocation=TrigTRTKeys.DriftCircles, 
                                          TRTRDOLocation = TRT_RDO_Key,
                                          isRoI_Seeded = True,
                                          RoIs = flags.InDet.Tracking.ActivePass.roi,
                                          TRT_DriftCircleTool = acc.getPrimaryAndMerge(TRT_DriftCircleToolCfg(flags, prefix=prefix+"_", name=f"{prefix}_DriftCircleTool")))
  acc.addEventAlgo( alg )
  return acc

def _tracksPostAmbi(flags):
  return flags.InDet.Tracking.ActivePass.trkTracks_IDTrig+"_Amb"

def _ambiScore(flags):
  return f"ScoreMap{flags.InDet.Tracking.ActivePass.input_name}"


def TRTExtensionToolCfg(flags):
  acc = ComponentAccumulator()

  detElementCond = CompFactory.InDet.TRT_DetElementsRoadCondAlg_xk(f"{prefix}TRT_DetElementsRoadCondAlg_xk")
  acc.addCondAlgo(detElementCond)

  patternPropagator = acc.getPrimaryAndMerge( RungeKuttaPropagatorCfg(flags))
  roadMaker = CompFactory.InDet.TRT_DetElementsRoadMaker_xk( name   = f'{prefix}TRTRoadMaker{flags.InDet.Tracking.ActivePass.name}',
                                                             MagneticFieldMode     = 'MapSolenoid',
                                                             PropagatorTool        =  patternPropagator )
  acc.addPublicTool( roadMaker )
  from .InDetTrigCollectionKeys import TrigTRTKeys
  extensionTool = CompFactory.InDet.TRT_TrackExtensionTool_xk ( name = f"{prefix}TrackExtensionTool_{flags.InDet.Tracking.ActivePass.name}",
                                                                TRT_ClustersContainer = TrigTRTKeys.DriftCircles,
                                                                PropagatorTool = patternPropagator,
                                                                UpdatorTool    = acc.getPrimaryAndMerge( KalmanxkUpdatorCfg( flags ) ),
                                                                RoadTool       = roadMaker,
                                                                DriftCircleCutTool = acc.getPrimaryAndMerge(TRTDriftCircleCutCfg(flags)),
                                                                MinNumberDriftCircles = flags.InDet.Tracking.ActivePass.minTRTonTrk,
                                                                ScaleHitUncertainty   = 2.,
                                                                RoadWidth             = 20.,
                                                                UseParameterization   = flags.InDet.Tracking.ActivePass.useParameterizedTRTCuts )

  acc.addPublicTool( extensionTool, primary=True )
  return acc

def TRTExtensionAlgCfg(flags):
  acc = ComponentAccumulator()

  alg = CompFactory.InDet.TRT_TrackExtensionAlg( name = f"{prefix}TrackExtensionAlg_{flags.InDet.Tracking.ActivePass.name}",
                                                 InputTracksLocation    = _tracksPostAmbi(flags),
                                                 TrackExtensionTool     = acc.getPrimaryAndMerge(TRTExtensionToolCfg(flags)),
                                                 ExtendedTracksLocation = 'ExtendedTrackMap' )
  acc.addEventAlgo(alg)                                                    
  return acc


def TRTExtensionProcessorCfg(flags):
  acc = ComponentAccumulator()
  extensionProcessor = CompFactory.InDet.InDetExtensionProcessor (name         = f"{prefix}ExtensionProcessor_{flags.InDet.Tracking.ActivePass.name}",
                                                            TrackName          = _tracksPostAmbi(flags),
                                                            #Cosmics           = InDetFlags.doCosmics(),
                                                            ExtensionMap       = 'ExtendedTrackMap',
                                                            NewTrackName       = flags.InDet.Tracking.ActivePass.trkTracks_IDTrig,
                                                            TrackFitter        = acc.getPrimaryAndMerge(FitterToolCfg(flags)),
                                                            TrackSummaryTool   = acc.getPrimaryAndMerge(TrackSummaryToolCfg(flags)),
                                                            ScoringTool        = acc.getPrimaryAndMerge(ambiguityScoringToolCfg(flags)),
                                                            suppressHoleSearch = False,
                                                            RefitPrds = not (flags.InDet.Tracking.ActivePass.refitROT or flags.InDet.Tracking.ActivePass.trtExtensionType == 'DAF') )
                                                            # Check these option after DAF is implemented
                                                            # tryBremFit         = InDetFlags.doBremRecovery(),
                                                            # caloSeededBrem     = InDetFlags.doCaloSeededBrem(),
                                                            # pTminBrem          = NewTrackingCuts.minPTBrem() ) )

  #TODO trigger uses only one type of extension, optional tools can be removed in future

  acc.addEventAlgo( extensionProcessor )
  return acc


def TRTExtrensionBuilderCfg(flags):
  acc = ComponentAccumulator()
  if flags.Input.Format is Format.BS:
    acc.merge( TRTDataProviderCfg(flags) )
  acc.merge( TRTRIOMakerCfg(flags) )

  acc.merge( TRTExtensionAlgCfg(flags) )
  acc.merge( TRTExtensionProcessorCfg(flags) )
#  'TRTRawDataProvider/InDetTrigMTTRTRawDataProvider_electronLRT', 
#  'InDet::TRT_RIO_Maker/InDetTrigMTTRTDriftCircleMaker_electronLRT', 
#  'InDet::TRT_TrackExtensionAlg/InDetTrigMTTrackExtensionAlg_electronLRT', 
#  'InDet::InDetExtensionProcessor/InDetTrigMTExtensionProcessor_electronLRT', 


  return acc

def InDetPRDtoTrackMapToolGangedPixelsCfg(flags):
  acc = ComponentAccumulator()
  from .InDetTrigCollectionKeys import TrigPixelKeys
  tool =  CompFactory.InDet.InDetPRDtoTrackMapToolGangedPixels( "InDetTrigPRDtoTrackMapToolGangedPixels",
                                                                PixelClusterAmbiguitiesMapName=TrigPixelKeys.PixelClusterAmbiguitiesMap, 
                                                                addTRToutliers = False)
  acc.addPublicTool(tool, primary=True)
  return acc

def TrackSelectionToolCfg(flags):
  acc = ComponentAccumulator()

  #TODO add configurations fro beamgas and cosmic see: trackSelectionTool_getter
  tool = CompFactory.InDet.InDetAmbiTrackSelectionTool('InDetTrigAmbiTrackSelectionTool',
                                       DriftCircleCutTool = None, #acc.getPrimaryAndMerge(TRTDriftCircleCutCfg(flags)),
                                       AssociationTool = acc.getPrimaryAndMerge( InDetPRDtoTrackMapToolGangedPixelsCfg(flags) ),
                                       minHits         = flags.InDet.Tracking.ActivePass.minClusters,
                                       minNotShared    = flags.InDet.Tracking.ActivePass.minSiNotShared,
                                       maxShared       = flags.InDet.Tracking.ActivePass.maxShared,
                                       minTRTHits      = 0,  # used for Si only tracking !!!
                                       Cosmics         = False,  #there is a different instance
                                       UseParameterization = False,
                                       # sharedProbCut   = 0.10,
                                       # doPixelSplitting = InDetTrigFlags.doPixelClusterSplitting()
                                       )
  acc.addPublicTool(tool, primary=True)
   
  return acc

def ambiguityScoringToolCfg(flags):
  acc = ComponentAccumulator()
  from TrkConfig.AtlasExtrapolatorConfig import InDetExtrapolatorCfg #TODO using offline, consider porting
  tool = CompFactory.InDet.InDetAmbiScoringTool(name = f"{prefix}_AmbiguityScoringTool_{flags.InDet.Tracking.ActivePass.name}",
                                                SummaryTool = acc.getPrimaryAndMerge(TrackSummaryToolCfg(flags)),
                                                Extrapolator = acc.getPrimaryAndMerge(InDetExtrapolatorCfg(flags, name="InDetTrigExtrapolator")),
                                                DriftCircleCutTool = acc.getPrimaryAndMerge(TRTDriftCircleCutCfg(flags)),
                                                useAmbigFcn = True,
                                                useTRT_AmbigFcn = False,
                                                maxZImp = flags.InDet.Tracking.ActivePass.maxZImpact,
                                                maxEta = flags.InDet.Tracking.ActivePass.maxEta,
                                                usePixel = flags.InDet.Tracking.ActivePass.usePixel,
                                                useSCT = flags.InDet.Tracking.ActivePass.useSCT,
                                                doEmCaloSeed =  False #TODO understand and set appropriately, however current setting is probably a correct one
  )
  # TODO if brem recovery is needed CaloROIInfoName info should be set
  
  acc.addPublicTool(tool, primary=True)
  return acc

def KalmanUpdatorCfg(flags):
  acc = ComponentAccumulator()
  tool = CompFactory.Trk.KalmanUpdator("InDetTrigUpdator")
  acc.setPrivateTools(tool)
  return acc

def FitterToolCfg(flags):
  acc = ComponentAccumulator()
  from TrkConfig.AtlasExtrapolatorToolsConfig import AtlasNavigatorCfg, AtlasEnergyLossUpdatorCfg
  from TrkConfig.AtlasExtrapolatorConfig import InDetExtrapolatorCfg
  from TrackingGeometryCondAlg.AtlasTrackingGeometryCondAlgConfig import (
      TrackingGeometryCondAlgCfg)
  cond_alg = TrackingGeometryCondAlgCfg(flags)
  geom_cond_key = cond_alg.getPrimary().TrackingGeometryWriteKey
  acc.merge(cond_alg)

  fitter = CompFactory.Trk.GlobalChi2Fitter(name                  = 'InDetTrigTrackFitter',
                                            ExtrapolationTool     = acc.getPrimaryAndMerge(InDetExtrapolatorCfg(flags, name="InDetTrigExtrapolator")),
                                            NavigatorTool         = acc.popToolsAndMerge(AtlasNavigatorCfg(flags, name="InDetTrigNavigator")),
                                            PropagatorTool        = acc.getPrimaryAndMerge( RungeKuttaPropagatorCfg( flags, "InDetTrigRKPropagator" ) ),
                                            RotCreatorTool        = acc.getPrimaryAndMerge(RIO_OnTrackCreatorCfg(flags, "InDetTrigRefitRotCreator")),
                                            BroadRotCreatorTool   = None, #InDetTrigBroadInDetRotCreator, #TODO, we have function to configure it
                                            EnergyLossTool        = acc.popToolsAndMerge(AtlasEnergyLossUpdatorCfg(flags)),
                                            MeasurementUpdateTool = acc.popToolsAndMerge(KalmanUpdatorCfg( flags )),
                                            MaterialUpdateTool    = CompFactory.Trk.MaterialEffectsUpdator(name = "InDetTrigMaterialEffectsUpdator"),
                                            StraightLine          = not flags.BField.solenoidOn,
                                            OutlierCut            = 4,
                                            SignedDriftRadius     = True,
                                            TrackChi2PerNDFCut    = 9,
                                            TRTExtensionCuts      = True,
                                            MaxIterations         = 40,
                                            Acceleration          = True,
                                            Momentum=0.,
                                            TrackingGeometryReadKey=geom_cond_key
                                                 )
  acc.addPublicTool(fitter, primary=True)
  #TODO come back to these settings                                                 
    # if InDetTrigFlags.useBroadClusterErrors():
    #   InDetTrigTrackFitter.RecalibrateSilicon = False

    # if InDetTrigFlags.doRefit():
    #   InDetTrigTrackFitter.BroadRotCreatorTool = None
    #   InDetTrigTrackFitter.RecalibrateSilicon = False
    #   InDetTrigTrackFitter.RecalibrateTRT     = False
    #   InDetTrigTrackFitter.ReintegrateOutliers= False


    # if InDetTrigFlags.doRobustReco():
    #   InDetTrigTrackFitter.OutlierCut         = 10.0
    #   InDetTrigTrackFitter.TrackChi2PerNDFCut = 20
    #   InDetTrigTrackFitter.MaxOutliers        = 99
    #   #only switch off for cosmics InDetTrigTrackFitter.Acceleration       = False
  return acc

def ambiguityProcessorToolCfg(flags):
  import AthenaCommon.SystemOfUnits as Units

  acc = ComponentAccumulator()
  
  tool = CompFactory.Trk.SimpleAmbiguityProcessorTool(name = f"{prefix}_AmbiguityProcessor_{flags.InDet.Tracking.ActivePass.name}",
                                                      SuppressHoleSearch = False, #TODO False if flags.InDet.Tracking.ActivePass.name == 'cosmics' else True,
                                                      #RefitPrds = False, #TODO clarify this setting False if flags.InDet.Tracking.ActivePass.name == 'cosmics' else True,
                                                      tryBremFit = True if flags.InDet.Tracking.ActivePass.name == 'electron' and flags.InDet.Tracking.doBremRecovery else False,
                                                      pTminBrem = 5*Units.GeV,
                                                      MatEffects = 3,
                                                      Fitter = acc.getPrimaryAndMerge(FitterToolCfg(flags)),
                                                      ScoringTool        = acc.getPrimaryAndMerge(ambiguityScoringToolCfg(flags)),
                                                      AssociationTool = acc.getPrimaryAndMerge(InDetPRDtoTrackMapToolGangedPixelsCfg(flags)),
                                                      TrackSummaryTool = acc.popToolsAndMerge(TrackSummaryToolCfg(flags, name="InDetTrigTrackSummaryTool",
                                                                                                                  useTRT=False, makePublic=False)),
                                                      SelectionTool    = acc.getPrimaryAndMerge(TrackSelectionToolCfg(flags)
                                                      )
  )
  acc.setPrivateTools(tool)
  return acc

def ambiguitySolverAlgCfg(flags):
  acc = ComponentAccumulator()

  scoreAlg = CompFactory.Trk.TrkAmbiguityScore(name = f"{prefix}TrkAmbiguityScore_{flags.InDet.Tracking.ActivePass.input_name}",
                                               TrackInput = [ flags.InDet.Tracking.ActivePass.trkTracks_FTF ],
                                               TrackOutput = _ambiScore(flags),
                                               AmbiguityScoreProcessor = None 
  )
  acc.addEventAlgo(scoreAlg)

  processorTool = acc.popToolsAndMerge(ambiguityProcessorToolCfg(flags))
  solverAlg = CompFactory.Trk.TrkAmbiguitySolver(name  = f"{prefix}TrkAmbiguitySolver_{flags.InDet.Tracking.ActivePass.input_name}",
                                                 TrackInput         = _ambiScore(flags),
                                                 TrackOutput        = _tracksPostAmbi(flags),
                                                 AmbiguityProcessor = processorTool
  )
  acc.addEventAlgo(solverAlg)

  return acc

def trackEFIDConverterCfg(flags):
  return _trackConverterCfg(flags, "_Precision"+flags.InDet.Tracking.ActivePass.name, flags.InDet.Tracking.ActivePass.trkTracks_IDTrig, flags.InDet.Tracking.ActivePass.tracks_IDTrig)


def trigInDetPrecisionTrackingCfg( inflags, signatureName, in_view=True ):
  """ Generates precision tracking config, it is a primary config function """

  acc = ComponentAccumulator()
  flags = inflags.cloneAndReplace("InDet.Tracking.ActivePass", "Trigger.InDetTracking."+signatureName)

  from .InDetTrigCollectionKeys import TrigPixelKeys,TrigTRTKeys
  if in_view:
    #TODO share setup with FTF
    TRT_RDO_Key = "TRT_RDOs"
    if flags.Input.Format is Format.BS:
          TRT_RDO_Key = TrigTRTKeys.RDOs
    verifier = CompFactory.AthViews.ViewDataVerifier( name = 'VDVInDetPrecision'+flags.InDet.Tracking.ActivePass.suffix,
                                                      DataObjects= [('xAOD::EventInfo', 'StoreGateSvc+EventInfo'),
                                                                    ('InDet::PixelClusterContainerCache', 'PixelTrigClustersCache'),
                                                                    ('PixelRDO_Cache', 'PixRDOCache'),
                                                                    ('SCT_RDO_Cache', 'SctRDOCache'),
                                                                    ( 'TRT_RDO_Container' , TRT_RDO_Key),
                                                                    ('SpacePointCache', 'PixelSpacePointCache'),
                                                                    ('SpacePointCache', 'SctSpacePointCache'),
                                                                    ('TrigRoiDescriptorCollection', flags.InDet.Tracking.ActivePass.roi),
                                                                    ( 'TagInfo', 'DetectorStore+ProcessingTags' ), 
                                                                    ( 'InDet::PixelGangedClusterAmbiguities' , TrigPixelKeys.PixelClusterAmbiguitiesMap),
                                                                    ( 'TrackCollection', flags.InDet.Tracking.ActivePass.trkTracks_FTF )] )

    if flags.Input.Format is Format.BS:
        verifier.DataObjects += [ ('IDCInDetBSErrContainer' , 'PixelByteStreamErrs'),
                                  ('IDCInDetBSErrContainer_Cache', 'SctBSErrCache'),
                                  ('IDCInDetBSErrContainer_Cache', 'SctFlaggedCondCache'), ]
    acc.addEventAlgo(verifier)

  acc.merge(ambiguitySolverAlgCfg(flags))
  acc.merge(TRTExtrensionBuilderCfg(flags))
  acc.merge(trackEFIDConverterCfg(flags))

#   Members = ['Trk::TrkAmbiguityScore/InDetTrigMTTrkAmbiguityScore_electronLRT', 
#  'Trk::TrkAmbiguitySolver/InDetTrigMTTrkAmbiguitySolver_electronLRT', 
#  'xAODMaker::TrackParticleCnvAlg/InDetTrigMTxAODParticleCreatorAlgelectronLRT_IDTrig']

  return acc

if __name__ == "__main__":
    from AthenaCommon.Configurable import Configurable
    Configurable.configurableRun3Behavior=1

    from AthenaConfiguration.AllConfigFlags import ConfigFlags
    from AthenaConfiguration.TestDefaults import defaultTestFiles
    ComponentAccumulator.debugMode = "trackCA trackPublicTool trackEventAlgo trackCondAlgo trackPrivateTool"
    ConfigFlags.Input.Files = defaultTestFiles.RAW
    ConfigFlags.lock()
    # this configuration is not runable, the test checks if there is no mistake in python scripts above
    # output can be used by experts to check actual configuration (e.g. here we configure to run on RAW and it should be reflected in settings)
    from AthenaConfiguration.MainServicesConfig import MainServicesCfg
    acc = MainServicesCfg( ConfigFlags )
    acc.merge( trigInDetFastTrackingCfg( ConfigFlags, roisKey="ElectronRoIs", signatureName="Electron" ) )

    acc.merge( trigInDetPrecisionTrackingCfg( ConfigFlags, signatureName="Electron" , in_view=True) )


    acc.printConfig(withDetails=True, summariseProps=True)
    acc.store( open("test.pkl", "wb") )
