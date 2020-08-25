#
#  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
#

from AthenaConfiguration.ComponentAccumulator import ComponentAccumulator
from AthenaConfiguration.ComponentFactory import CompFactory
from AthenaCommon.Constants import DEBUG

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
                                                      usePixel     = flags.Detector.RecoPixel, # DetFlags.haveRIO.pixel_on(),
                                                      useSCT       = flags.Detector.RecoSCT, #DetFlags.haveRIO.SCT_on(),
                                                      RoadWidth    = 10, #InDetTrigCutValues.RoadWidth()
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
  acc.addPublicTool( pixelLATool )

  nnTool = CompFactory.InDet.NnClusterizationFactory( name                         = "TrigNnClusterizationFactory",
                                                      PixelLorentzAngleTool        = pixelLATool,
                                                      useToT                       = flags.InDet.doNNToTCalibration,
                                                      NnCollectionReadKey          = 'PixelClusterNN',
                                                      NnCollectionWithTrackReadKey = 'PixelClusterNNWithTrack')

  tool = CompFactory.InDet.PixelClusterOnTrackTool( name,
                                                    ErrorStrategy = 2,
                                                    LorentzAngleTool = acc.getPublicTool( "PixelLorentzAngleTool" ),
                                                    NnClusterizationFactory = nnTool )
  acc.addPublicTool( tool )
  return acc


def SCT_ClusterOnTrackToolCfg( flags, **kwargs ):
  acc = ComponentAccumulator()
  from SiLorentzAngleTool.SCT_LorentzAngleConfig import SCT_LorentzAngleCfg
  sctLATool =  acc.popToolsAndMerge( SCT_LorentzAngleCfg( flags ) )
  acc.addPublicTool( sctLATool )
  tool = CompFactory.InDet.SCT_ClusterOnTrackTool("SCT_ClusterOnTrackTool",
                                                    CorrectionStrategy = 0,  # do correct position bias
                                                    ErrorStrategy      = 2,  # do use phi dependent errors
                                                    LorentzAngleTool   = acc.getPublicTool( "SCT_LorentzAngleTool" ) # default name
                                                    )
  acc.addPublicTool ( tool )
  return acc

def RIO_OnTrackCreatorCfg( flags, **kwargs ):
  acc = ComponentAccumulator()
  name =  kwargs.pop("rioOnTrackCreatorName", "InDetTrigRotCreator")
  acc.merge( PixelClusterOnTrackCfg( flags, **kwargs ) )
  acc.merge( SCT_ClusterOnTrackToolCfg( flags, **kwargs ) )
  tool = CompFactory.Trk.RIO_OnTrackCreator(name,
                                            ToolPixelCluster = acc.getPublicTool( "InDetTrigPixelClusterOnTrackTool" ), #InDetTrigPixelClusterOnTrackTool,
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
                                                         usePixel              = flags.Detector.RecoPixel, #DetFlags.haveRIO.pixel_on(),
                                                         useSCT                = flags.Detector.RecoSCT, #DetFlags.haveRIO.SCT_on(),
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
  import AthenaCommon.SystemOfUnits as Unit
  name = kwargs.pop("name", "SiTrackMaker_xk")
  acc = ComponentAccumulator()
  acc.merge( SiDetElementsRoadMaker_xkCfg( flags, **kwargs ) )
  combTrackFinderTool = acc.popToolsAndMerge( SiCombinatorialTrackFinder_xkCfg( flags, **kwargs ) )

  from IOVDbSvc.IOVDbSvcConfig import addFoldersSplitOnline, addFolders
  acc.merge(addFoldersSplitOnline( flags, "INDET", '/Indet/Onl/TrkErrorScaling', '/Indet/TrkErrorScaling', className="CondAttrListCollection") )

  acc.addCondAlgo( CompFactory.RIO_OnTrackErrorScalingCondAlg(ErrorScalingType = ["PixelRIO_OnTrackErrorScaling", "SCTRIO_OnTrackErrorScaling", "TRTRIO_OnTrackErrorScaling"],
                                                              OutKeys        = ["ConditionStore+/Indet/TrkErrorScalingPixel", "ConditionStore+/Indet/TrkErrorScalingSCT", "ConditionStore+/Indet/TrkErrorScalingTRT"],
                                                              ReadKey        = "ConditionStore+/Indet/TrkErrorScaling") )


  tool = CompFactory.InDet.SiTrackMaker_xk( name,
                                            RoadTool                 = acc.getPublicTool( "InDetTrigSiDetElementsRoadMaker" ),
                                            CombinatorialTrackFinder = combTrackFinderTool,
                                            pTmin                    = 1000*Unit.MeV, # TODO use flag for these props
                                            nClustersMin             = 7,
                                            nHolesMax                = 3,
                                            nHolesGapMax             = 3,
                                            SeedsFilterLevel         = 0,
                                            Xi2max                   = 15,
                                            Xi2maxNoAdd              = 35,
                                            nWeightedClustersMin     = 6,
                                            #CosmicTrack             = InDetFlags.doCosmics(),
                                            Xi2maxMultiTracks        = 15,
                                            UseAssociationTool       = False )
  acc.addPublicTool( tool )
  return acc


# def (flags, **kwargs):
#   acc = ComponentAccumulator()

#   tool = CompFactory.
#   acc.addPublicTool( tool )
#   return acc


# def (flags, **kwargs):
#   acc = ComponentAccumulator()

#   tool = CompFactory.
#   acc.addPublicTool( tool )
#   return acc


def InDetTestPixelLayerToolCfg(flags, **kwargs):
  acc = ComponentAccumulator()
  from PixelConditionsTools.PixelConditionsSummaryConfig import PixelConditionsSummaryCfg
  pixelCondSummaryTool = acc.popToolsAndMerge( PixelConditionsSummaryCfg(flags) )
  from InDetConfig.InDetRecToolConfig import InDetExtrapolatorCfg
  extrapolator = acc.popToolsAndMerge( InDetExtrapolatorCfg( flags, name = "TrigInDetExtrapolator" ) )

  tool = CompFactory.InDet.InDetTestPixelLayerTool("InDetTrigTestPixelLayerTool",
                                                               PixelSummaryTool = pixelCondSummaryTool,
                                                               Extrapolator     = extrapolator,
                                                               CheckActiveAreas = True,
                                                               CheckDeadRegions = True)
  acc.addPublicTool( tool )
  return acc


def InDetHoleSearchToolCfg(flags, **kwargs):
  acc = ComponentAccumulator()

  from InDetConfig.InDetRecToolConfig import InDetSCT_ConditionsSummaryToolCfg
  sctCondSummaryTool = acc.popToolsAndMerge( InDetSCT_ConditionsSummaryToolCfg( flags,withFlaggedCondTool=False, withTdaqTool=False ) )

  acc.merge( InDetTestPixelLayerToolCfg( flags, **kwargs ) )

  from InDetConfig.InDetRecToolConfig import InDetExtrapolatorCfg
  acc.merge( InDetExtrapolatorCfg( flags, name = "TrigInDetExtrapolator" ) )
  #acc.addPublicTool(extrapolator)

  name = kwargs.pop("name", "InDetTrigHoleSearchTool")
  tool = CompFactory.InDet.InDetTrackHoleSearchTool(name,
                                                    Extrapolator =  acc.getPublicTool( "TrigInDetExtrapolator" ),
                                                      #usePixel      = flags.Detector.RecoPixel, #DetFlags.haveRIO.pixel_on(),
                                                      #useSCT        = flags.Detector.RecoSCT, #DetFlags.haveRIO.SCT_on(),
                                                    #SctSummaryTool = sctCondSummaryTool,
                                                    #PixelLayerTool = acc.getPublicTool( "InDetTrigTestPixelLayerTool" ),
                                                    )
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
                                                       usePixel      = flags.Detector.RecoPixel,  #DetFlags.haveRIO.pixel_on(),
                                                       useSCT        = flags.Detector.RecoSCT,  #DetFlags.haveRIO.SCT_on(),
                                                       useTRT        = True, # flags.Detector.TRTOn,  #DetFlags.haveRIO.TRT_on()
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
                                              PixRDOCacheKey     = InDetCacheNames.PixRDOCacheKey,
                                              PixBSErrCacheKey   = InDetCacheNames.PixBSErrCacheKey)

  acc.addEventAlgo( InDetCacheCreatorTrig )
  return acc




#Set up conditions algorithms
def TrigInDetCondCfg( flags ):

  acc = ComponentAccumulator()
  from BeamPipeGeoModel.BeamPipeGMConfig import BeamPipeGeometryCfg
  acc.merge( BeamPipeGeometryCfg( flags ) )
  from AtlasGeoModel.InDetGMConfig import InDetGeometryCfg
  acc.merge( InDetGeometryCfg( flags ) )
  #acc.merge(InDetGMConfig(flags))



  from IOVDbSvc.IOVDbSvcConfig import addFoldersSplitOnline, addFolders
  acc.merge(addFoldersSplitOnline(flags, "INDET","/Indet/Onl/AlignL1/ID","/Indet/AlignL1/ID",className="CondAttrListCollection"))
  acc.merge(addFoldersSplitOnline(flags, "INDET","/Indet/Onl/AlignL2/PIX","/Indet/AlignL2/PIX",className="CondAttrListCollection"))
  acc.merge(addFoldersSplitOnline(flags, "INDET","/Indet/Onl/AlignL2/SCT","/Indet/AlignL2/SCT",className="CondAttrListCollection"))
  acc.merge(addFoldersSplitOnline(flags, "INDET","/Indet/Onl/AlignL3","/Indet/AlignL3",className="AlignableTransformContainer"))
  acc.merge(addFoldersSplitOnline(flags, "INDET","/Indet/Onl/IBLDist","/Indet/IBLDist",className="CondAttrListCollection"))

  from SCT_ConditionsTools.SCT_DCSConditionsConfig import SCT_DCSConditionsCfg, SCT_DCSConditionsToolCfg
  dcsTool =  acc.popToolsAndMerge( SCT_DCSConditionsCfg( flags, DCSConditionsTool = SCT_DCSConditionsToolCfg( flags, ReadAllDBFolders = True, ReturnHVTemp = True)) )
#  SCT_DCSConditionsTool=CompFactory.SCT_DCSConditionsTool
#  dcsTool = SCT_DCSConditionsTool(ReadAllDBFolders = True, ReturnHVTemp = True)

  from SCT_ConditionsTools.SCT_SiliconConditionsConfig import SCT_SiliconConditionsCfg #, SCT_SiliconConditionsToolCfg
  #sctSiliconConditionsTool= SCT_SiliconConditionsCfg(flags, toolName="InDetSCT_SiliconConditionsTool", dcsTool=dcsTool )
  #sctSiliconConditionsTool = SCT_SiliconConditionsToolCfg(flags)
  acc.merge(SCT_SiliconConditionsCfg(flags, DCSConditionsTool=dcsTool))

  SCT_AlignCondAlg=CompFactory.SCT_AlignCondAlg
  acc.addCondAlgo(SCT_AlignCondAlg(UseDynamicAlignFolders = True))

  SCT_DetectorElementCondAlg=CompFactory.SCT_DetectorElementCondAlg
  acc.addCondAlgo(SCT_DetectorElementCondAlg(name = "SCT_DetectorElementCondAlg"))

  from SCT_Cabling.SCT_CablingConfig import SCT_CablingCondAlgCfg
  acc.merge(SCT_CablingCondAlgCfg(flags))
  SCT_ConfigurationConditionsTool=CompFactory.SCT_ConfigurationConditionsTool
  acc.addPublicTool(SCT_ConfigurationConditionsTool())
  channelFolder = "/SCT/DAQ/Config/Chip"
  moduleFolder = "/SCT/DAQ/Config/Module"
  murFolder = "/SCT/DAQ/Config/MUR"
  SCT_ConfigurationCondAlg=CompFactory.SCT_ConfigurationCondAlg
  acc.addCondAlgo(SCT_ConfigurationCondAlg(ReadKeyChannel = channelFolder,
                                           ReadKeyModule = moduleFolder,
                                           ReadKeyMur = murFolder))
  acc.merge(addFolders(flags, [channelFolder, moduleFolder, murFolder], "SCT", className="CondAttrListVec"))
  # Set up SCTSiLorentzAngleCondAlg
  SCT_ConfigurationConditionsTool=CompFactory.SCT_ConfigurationConditionsTool
  stateFolder = "/SCT/DCS/CHANSTAT"
  hvFolder = "/SCT/DCS/HV"
  tempFolder = "/SCT/DCS/MODTEMP"
  dbInstance = "DCS_OFL"
  acc.merge(addFolders(flags, [stateFolder, hvFolder, tempFolder], dbInstance, className="CondAttrListCollection"))


  # from InDetConfig.InDetRecToolConfig import InDetSCT_ConditionsSummaryToolCfg
  # sctCondSummaryTool = acc.popToolsAndMerge( InDetSCT_ConditionsSummaryToolCfg( flags, withFlaggedCondTool=False, withTdaqTool=False ) )

  # SCT_DCSConditionsTempCondAlg=CompFactory.SCT_DCSConditionsTempCondAlg
  # acc.addCondAlgo(SCT_DCSConditionsTempCondAlg( ReadKey = tempFolder ))

  # SCT_DCSConditionsStatCondAlg=CompFactory.SCT_DCSConditionsStatCondAlg
  # acc.addCondAlgo(SCT_DCSConditionsStatCondAlg(ReturnHVTemp = True,
  #                                             ReadKeyHV = hvFolder,
  #                                             ReadKeyState = stateFolder))
  # SCT_DCSConditionsHVCondAlg=CompFactory.SCT_DCSConditionsHVCondAlg
  # acc.addCondAlgo(SCT_DCSConditionsHVCondAlg(ReadKey = hvFolder))

  # SCT_SiliconHVCondAlg=CompFactory.SCT_SiliconHVCondAlg
  # acc.addCondAlgo(SCT_SiliconHVCondAlg(UseState = dcsTool.ReadAllDBFolders,
  #                                     DCSConditionsTool = dcsTool))
  # SCT_SiliconTempCondAlg=CompFactory.SCT_SiliconTempCondAlg
  # acc.addCondAlgo(SCT_SiliconTempCondAlg(UseState = dcsTool.ReadAllDBFolders, DCSConditionsTool = dcsTool))




  # SCTSiLorentzAngleCondAlg=CompFactory.SCTSiLorentzAngleCondAlg
  # acc.addCondAlgo(SCTSiLorentzAngleCondAlg(name = "SCTSiLorentzAngleCondAlg",
  #                                          SiConditionsTool = sctSiliconConditionsTool,
  #                                          UseMagFieldCache = True,
  #                                          UseMagFieldDcs = False))
  # SiLorentzAngleTool=CompFactory.SiLorentzAngleTool
  # SCTLorentzAngleTool = SiLorentzAngleTool(name = "SCTLorentzAngleTool", DetectorName="SCT", SiLorentzAngleCondData="SCTSiLorentzAngleCondData")
  # SCTLorentzAngleTool.UseMagFieldCache = True
  # acc.addPublicTool(SCTLorentzAngleTool)


  from SiLorentzAngleTool.SCT_LorentzAngleConfig import SCT_LorentzAngleCfg
  SCTLorentzAngleTool =  acc.popToolsAndMerge( SCT_LorentzAngleCfg( flags ) )
  acc.addPublicTool(SCTLorentzAngleTool)

  acc.merge(addFoldersSplitOnline(flags, "INDET", "/Indet/Onl/Beampos", "/Indet/Beampos", className="AthenaAttributeList"))
  acc.merge(addFolders(flags, "/TRT/Onl/ROD/Compress","TRT_ONL", className='CondAttrListCollection'))
  acc.merge(addFoldersSplitOnline(flags, "TRT","/TRT/Onl/Calib/RT","/TRT/Calib/RT",className="TRTCond::RtRelationMultChanContainer"))
  acc.merge(addFoldersSplitOnline(flags, "TRT","/TRT/Onl/Calib/T0","/TRT/Calib/T0",className="TRTCond::StrawT0MultChanContainer"))
  acc.merge(addFoldersSplitOnline (flags, "TRT","/TRT/Onl/Calib/errors","/TRT/Calib/errors",className="TRTCond::RtRelationMultChanContainer"))


  ###############
  # Pixel setup #
  ###############
  from PixelConditionsAlgorithms.PixelConditionsConfig import (
      PixelConfigCondAlgCfg, PixelChargeCalibCondAlgCfg, PixelDCSCondHVAlgCfg,
      PixelDCSCondTempAlgCfg, PixelAlignCondAlgCfg, PixelDetectorElementCondAlgCfg,
      PixelHitDiscCnfgAlgCfg, PixelReadoutSpeedAlgCfg, PixelCablingCondAlgCfg,
      PixelDCSCondStateAlgCfg, PixelDCSCondStatusAlgCfg,
      PixelDistortionAlgCfg, PixelOfflineCalibCondAlgCfg
# NEW FOR RUN3    PixelDeadMapCondAlgCfg, PixelChargeLUTCalibCondAlgCfg/
  )

  from PixelConditionsTools.PixelConditionsSummaryConfig import PixelConditionsSummaryCfg
  from SiPropertiesTool.PixelSiPropertiesConfig import PixelSiPropertiesCfg
  from SiLorentzAngleTool.PixelLorentzAngleConfig import PixelLorentzAngleCfg
  from PixelCabling.PixelCablingConfigNew import PixelCablingSvcCfg

  # module parameters
  acc.merge(PixelConfigCondAlgCfg(flags,
                                  UseCalibConditions=False,
                                  UseDeadmapConditions=True,
                                  UseDCSStateConditions=False,
                                  UseDCSStatusConditions=False,
                                  UseDCSHVConditions=True,
                                  UseDCSTemperatureConditions=True,
                                  UseTDAQConditions=False))

  # charge calibration
  acc.merge(PixelChargeCalibCondAlgCfg(flags))
# NEW FOR RUN3  acc.merge(PixelChargeLUTCalibCondAlgCfg(flags))
  # DCS setup
  acc.merge(PixelDCSCondHVAlgCfg(flags))
  acc.merge(PixelDCSCondTempAlgCfg(flags))
  # alignment setup
  acc.merge(PixelAlignCondAlgCfg(flags, UseDynamicAlignFolders=True))
  acc.merge(PixelDetectorElementCondAlgCfg(flags))
  # cabling setup
  acc.merge(PixelHitDiscCnfgAlgCfg(flags))
  acc.merge(PixelReadoutSpeedAlgCfg(flags))
  acc.merge(PixelCablingCondAlgCfg(flags))
  # deadmap
  acc.merge(PixelDCSCondStateAlgCfg(flags))
  acc.merge(PixelDCSCondStatusAlgCfg(flags))
# NEW FOR RUN3    acc.merge(PixelDeadMapCondAlgCfg(flags))
  # offline calibration
  acc.merge(PixelDistortionAlgCfg(flags))
  acc.merge(PixelOfflineCalibCondAlgCfg(flags))

  acc.popToolsAndMerge(PixelConditionsSummaryCfg(flags))
  acc.popToolsAndMerge(PixelSiPropertiesCfg(flags))
  acc.popToolsAndMerge(PixelLorentzAngleCfg(flags))
  acc.merge(PixelCablingSvcCfg(flags))


  BeamSpotCondAlg=CompFactory.BeamSpotCondAlg
  acc.addCondAlgo(BeamSpotCondAlg( "BeamSpotCondAlg" ))


  from MagFieldServices.MagFieldServicesConfig import MagneticFieldSvcCfg
  mfsc = MagneticFieldSvcCfg(flags)
  acc.merge( mfsc )
  return acc

def TrigInDetConfig( flags, roisKey="EMRoIs", signatureName='' ):
  #If signature specified add suffix to the name of each algorithms
  signature =  "_" + signatureName if signatureName else ''

  acc = ComponentAccumulator()
  acc.merge(TrigInDetCondCfg(flags))

  from InDetRecExample.InDetKeys import InDetKeys

  # Region selector tool for SCT
  from RegionSelector.RegSelToolConfig import regSelTool_SCT_Cfg
  RegSelTool_SCT = acc.popToolsAndMerge(regSelTool_SCT_Cfg(flags))

  verifier = CompFactory.AthViews.ViewDataVerifier( name = 'VDVInDet'+signature,
                                                    DataObjects= [('xAOD::EventInfo', 'StoreGateSvc+EventInfo'),
                                                                  ('InDet::PixelClusterContainerCache', 'PixelTrigClustersCache'),
                                                                  ('PixelRDO_Cache', 'PixRDOCache'),
                                                                  ('InDet::SCT_ClusterContainerCache', 'SCT_ClustersCache'),
                                                                  ('SCT_RDO_Cache', 'SctRDOCache'),
                                                                  ('SpacePointCache', 'PixelSpacePointCache'),
                                                                  ('SpacePointCache', 'SctSpacePointCache'),
                                                                  ('IDCInDetBSErrContainer_Cache', 'SctBSErrCache'),
                                                                  ('TrigRoiDescriptorCollection', 'StoreGateSvc+'+roisKey),
                                                                  ( 'TagInfo' , 'DetectorStore+ProcessingTags' )] )

  acc.addEventAlgo(verifier)

  #Only add raw data decoders if we're running over raw data
  isMC = flags.Input.isMC
  if not isMC:
    #Pixel

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
                                                     ProviderTool = InDetPixelRawDataProviderTool,)

    InDetPixelRawDataProvider.isRoI_Seeded = True
    InDetPixelRawDataProvider.RoIs = roisKey
    InDetPixelRawDataProvider.RDOCacheKey = InDetCacheNames.PixRDOCacheKey
    acc.addEventAlgo(InDetPixelRawDataProvider)




    #SCT
    SCT_RodDecoder=CompFactory.SCT_RodDecoder
    InDetSCTRodDecoder = SCT_RodDecoder(name        = "InDetSCTRodDecoder"+ signature)
    acc.addPublicTool(InDetSCTRodDecoder)

    SCTRawDataProviderTool=CompFactory.SCTRawDataProviderTool
    InDetSCTRawDataProviderTool = SCTRawDataProviderTool(name    = "InDetSCTRawDataProviderTool"+ signature,
                                                         Decoder = InDetSCTRodDecoder)
    acc.addPublicTool(InDetSCTRawDataProviderTool)

    # load the SCTRawDataProvider
    SCTRawDataProvider=CompFactory.SCTRawDataProvider
    InDetSCTRawDataProvider = SCTRawDataProvider(name         = "InDetSCTRawDataProvider"+ signature,
                                                 RDOKey       = InDetKeys.SCT_RDOs(),
                                                 ProviderTool = InDetSCTRawDataProviderTool, )

    InDetSCTRawDataProvider.isRoI_Seeded = True
    InDetSCTRawDataProvider.RoIs = roisKey
    InDetSCTRawDataProvider.RDOCacheKey = InDetCacheNames.SCTRDOCacheKey

    InDetSCTRawDataProvider.RegSelTool = RegSelTool_SCT
    InDetSCTRawDataProvider.OutputLevel=DEBUG
    acc.addEventAlgo(InDetSCTRawDataProvider)

    # load the SCTEventFlagWriter
    SCTEventFlagWriter=CompFactory.SCTEventFlagWriter
    InDetSCTEventFlagWriter = SCTEventFlagWriter(name = "InDetSCTEventFlagWriter"+ signature)

    acc.addEventAlgo(InDetSCTEventFlagWriter)


    #TRT


    TRT_RodDecoder=CompFactory.TRT_RodDecoder
    InDetTRTRodDecoder = TRT_RodDecoder(name = "InDetTRTRodDecoder",
                                        LoadCompressTableDB = True)#(globalflags.DataSource() != 'geant4'))
    acc.addPublicTool(InDetTRTRodDecoder)

    TRTRawDataProviderTool=CompFactory.TRTRawDataProviderTool
    InDetTRTRawDataProviderTool = TRTRawDataProviderTool(name    = "InDetTRTRawDataProviderTool"+ signature,
                                                         Decoder = InDetTRTRodDecoder)
    acc.addPublicTool(InDetTRTRawDataProviderTool)


    # load the TRTRawDataProvider
    TRTRawDataProvider=CompFactory.TRTRawDataProvider
    InDetTRTRawDataProvider = TRTRawDataProvider(name         = "InDetTRTRawDataProvider"+ signature,
                                                 RDOKey       = "TRT_RDOs",
                                                 ProviderTool = InDetTRTRawDataProviderTool)
    InDetTRTRawDataProvider.isRoI_Seeded = True
    InDetTRTRawDataProvider.RoIs = roisKey

    acc.addEventAlgo(InDetTRTRawDataProvider)


  #Pixel clusterisation
  InDet__ClusterMakerTool=CompFactory.InDet.ClusterMakerTool
  InDetClusterMakerTool = InDet__ClusterMakerTool(name                 = "InDetClusterMakerTool"+ signature)

  acc.addPublicTool(InDetClusterMakerTool)


  InDet__MergedPixelsTool=CompFactory.InDet.MergedPixelsTool
  InDetMergedPixelsTool = InDet__MergedPixelsTool(name                    = "InDetMergedPixelsTool"+ signature,
                                                  globalPosAlg            = InDetClusterMakerTool,
                                                  MinimalSplitSize        = 0,
                                                  MaximalSplitSize        = 49,
                                                  MinimalSplitProbability = 0,
                                                  DoIBLSplitting = True,
                                                  )
  # Enable duplicated RDO check for data15 because duplication mechanism was used.
  if len(flags.Input.ProjectName)>=6 and flags.Input.ProjectName[:6]=="data15":
    InDetMergedPixelsTool.CheckDuplicatedRDO = True
  acc.addPublicTool(InDetMergedPixelsTool)

  InDet__PixelGangedAmbiguitiesFinder=CompFactory.InDet.PixelGangedAmbiguitiesFinder
  InDetPixelGangedAmbiguitiesFinder = InDet__PixelGangedAmbiguitiesFinder(name = "InDetPixelGangedAmbiguitiesFinder"+ signature)
  acc.addPublicTool(InDetPixelGangedAmbiguitiesFinder)

  InDet__PixelClusterization=CompFactory.InDet.PixelClusterization
  InDetPixelClusterization = InDet__PixelClusterization(name                    = "InDetPixelClusterization"+ signature,
                                                        clusteringTool          = InDetMergedPixelsTool,
                                                        gangedAmbiguitiesFinder = InDetPixelGangedAmbiguitiesFinder,
                                                        DataObjectName          = InDetKeys.PixelRDOs(),
                                                        AmbiguitiesMap          = 'TrigPixelClusterAmbiguitiesMap',
                                                        ClustersName            = "PixelTrigClusters",)
  InDetPixelClusterization
  InDetPixelClusterization.isRoI_Seeded = True
  InDetPixelClusterization.RoIs = roisKey
  InDetPixelClusterization.ClusterContainerCacheKey = InDetCacheNames.Pixel_ClusterKey
  acc.addEventAlgo(InDetPixelClusterization)

  from InDetConfig.InDetRecToolConfig import InDetSCT_ConditionsSummaryToolCfg
  InDetSCT_ConditionsSummaryToolWithoutFlagged = acc.popToolsAndMerge(InDetSCT_ConditionsSummaryToolCfg(flags,withFlaggedCondTool=False, withTdaqTool=False))


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
  InDetSCT_Clusterization = InDet__SCT_Clusterization(name                    = "InDetSCT_Clusterization"+ signature,
                                                      clusteringTool          = InDetSCT_ClusteringTool,
                                                      # ChannelStatus         = InDetSCT_ChannelStatusAlg,
                                                      DataObjectName          = InDetKeys.SCT_RDOs(),
                                                      ClustersName            = "SCT_TrigClusters",
                                                      conditionsTool          = InDetSCT_ConditionsSummaryToolWithoutFlagged)
  InDetSCT_Clusterization.isRoI_Seeded = True
  InDetSCT_Clusterization.RoIs = roisKey
  InDetSCT_Clusterization.ClusterContainerCacheKey = InDetCacheNames.SCT_ClusterKey

  InDetSCT_Clusterization.RegSelTool = RegSelTool_SCT
  InDetSCT_Clusterization.OutputLevel = DEBUG
  acc.addEventAlgo(InDetSCT_Clusterization)


  #Space points and FTF

  InDet__SiSpacePointMakerTool=CompFactory.InDet.SiSpacePointMakerTool
  InDetSiSpacePointMakerTool = InDet__SiSpacePointMakerTool(name = "InDetSiSpacePointMakerTool"+ signature)
  acc.addPublicTool(InDetSiSpacePointMakerTool)

  acc.addCondAlgo( CompFactory.InDet.SiElementPropertiesTableCondAlg(name = "InDetSiElementPropertiesTableCondAlg") )

  InDet__SiTrackerSpacePointFinder=CompFactory.InDet.SiTrackerSpacePointFinder
  InDetSiTrackerSpacePointFinder = InDet__SiTrackerSpacePointFinder(name                   = "TrigSPFinder"+ signature,
                                                                    SiSpacePointMakerTool  = InDetSiSpacePointMakerTool,
                                                                    PixelsClustersName     = "PixelTrigClusters",
                                                                    SCT_ClustersName       = "SCT_TrigClusters",
                                                                    SpacePointsPixelName   = "PixelTrigSpacePoints",
                                                                    SpacePointsSCTName     = "SCT_TrigSpacePoints",
                                                                    SpacePointsOverlapName = InDetKeys.OverlapSpacePoints(),
                                                                    ProcessPixels          = flags.Detector.RecoPixel,
                                                                    ProcessSCTs            = flags.Detector.RecoSCT,
                                                                    ProcessOverlaps        = flags.Detector.RecoSCT,
                                                                    SpacePointCacheSCT = InDetCacheNames.SpacePointCacheSCT,
                                                                    SpacePointCachePix = InDetCacheNames.SpacePointCachePix,)
  InDetSiTrackerSpacePointFinder.OutputLevel=DEBUG
  acc.addEventAlgo(InDetSiTrackerSpacePointFinder)

  acc.addPublicTool( CompFactory.TrigL2LayerNumberTool( "TrigL2LayerNumberTool_FTF" ) )

  acc.merge( TrackSummaryToolCfg(flags, name="TrigSummaryTool_FTF") )

#  acc.addPublicTool( CompFactory.TrigL2ResidualCalculator( "TrigL2ResidualCalculator" ) )
  acc.merge( SiTrackMaker_xkCfg( flags, name = "TrigTrackMaker_FTF"+signature ) )

  acc.addPublicTool( CompFactory.TrigInDetTrackFitter( "TrigTrackFitter_FTF" ) )
  from RegionSelector.RegSelToolConfig import (regSelTool_SCT_Cfg, regSelTool_Pixel_Cfg)

  pixRegSelTool = acc.popToolsAndMerge( regSelTool_Pixel_Cfg( flags) )
  sctRegSelTool = acc.popToolsAndMerge( regSelTool_SCT_Cfg( flags) )

  from TrkConfig.AtlasTrackingGeometrySvcConfig import TrackingGeometrySvcCfg
  acc.merge( TrackingGeometrySvcCfg( flags ) )
  acc.addPublicTool( CompFactory.TrigL2LayerNumberTool( name = "TrigL2LayerNumberTool_FTF",
                                                        UseNewLayerScheme = True) )

  acc.addPublicTool( CompFactory.TrigSpacePointConversionTool( "TrigSPConversionTool" + signature.lower(), # lowercased to ease comparison to old style conf. TODO, remove
                                                                 DoPhiFiltering = True,
                                                                 UseBeamTilt = False,
                                                                 UseNewLayerScheme = True,
                                                                 RegSel_Pixel = pixRegSelTool,
                                                                 RegSel_SCT = sctRegSelTool,
                                                                 layerNumberTool = acc.getPublicTool("TrigL2LayerNumberTool_FTF") ) )


  # TODO remove once offline configured counterparts are available?
  acc.addCondAlgo( CompFactory.InDet.SiDetElementBoundaryLinksCondAlg_xk(name= "InDetSiDetElementBoundaryLinksSCTCondAlg",
                                                                         WriteKey = "SCT_DetElementBoundaryLinks_xk") )

  acc.addCondAlgo( CompFactory.InDet.SiDetElementBoundaryLinksCondAlg_xk(name = "InDetSiDetElementBoundaryLinksPixelCondAlg",
                                                                         ReadKey  = "PixelDetectorElementCollection",
                                                                         WriteKey = "PixelDetElementBoundaryLinks_xk") )

  ftf = CompFactory.TrigFastTrackFinder( name = "FTF" + signature,
                                         LayerNumberTool          = acc.getPublicTool( "TrigL2LayerNumberTool_FTF" ),
                                         SpacePointProviderTool   = acc.getPublicTool( "TrigSPConversionTool" + signature.lower() ),
                                         TrackSummaryTool         = acc.getPublicTool( "TrigSummaryTool_FTF" ),
#                                         TrigL2ResidualCalculator = acc.getPublicTool( "TrigL2ResidualCalculator" ),
                                         initialTrackMaker        = acc.getPublicTool( "TrigTrackMaker_FTF" + signature ),
                                         trigInDetTrackFitter     = acc.getPublicTool( "TrigTrackFitter_FTF" ),
                                         RoIs = roisKey,
                                         trigZFinder = CompFactory.TrigZFinder(),
                                         doZFinder = False, # this and all below, copied over from comparison with running JOs, TODO find a proper surce of this settings
                                         SeedRadBinWidth =  2,
                                         TrackInitialD0Max = 20.0,
                                         TracksName = "TrigFastTrackFinder_Tracks"+signature,
                                         OutputCollectionSuffix = signature,
                                         TripletDoPSS = False,
                                         Triplet_D0Max = 4.0,
                                         Triplet_D0_PPS_Max = 1.7,
                                         Triplet_MaxBufferLength = 3,
                                         Triplet_MinPtFrac = 1,
                                         Triplet_nMaxPhiSlice = 53,
                                         doCloneRemoval = True,
                                         doResMon = False,
                                         doSeedRedundancyCheck = True,
                                         pTmin = 1000.0,
                                         useNewLayerNumberScheme = True,
                                         MinHits = 5
                                           )
  #ftf.RoIs = roisKey
  ftf.OutputLevel=DEBUG
  acc.addEventAlgo( ftf )

  #CondSvc=CompFactory.CondSvc
  #acc.addService(CondSvc())


  #from TrigInDetConf.TrigInDetRecCommonTools import InDetTrigFastTrackSummaryTool
  #from TrigInDetConf.TrigInDetPostTools import  InDetTrigParticleCreatorToolFTF

  #InDet__TrigTrackingxAODCnvMT=CompFactory.InDet.TrigTrackingxAODCnvMT
  #theTrackParticleCreatorAlg = InDet__TrigTrackingxAODCnvMT(name = "InDetTrigTrackParticleCreatorAlg",
  #                                                         doIBLresidual = False,
  #                                                         TrackName = "TrigFastTrackFinder_Tracks",
  #                                                         TrackParticlesName = "IDTrack",
  #                                                         ParticleCreatorTool = InDetTrigParticleCreatorToolFTF)
  #theTrackParticleCreatorAlg.roiCollectionName = roisKey
  #acc.addEventAlgo(theTrackParticleCreatorAlg)



  return acc

def indetInViewRecoCfg( flags, viewMakerName, signature='' ):
  """ TBD if this function should be defined here or moved to the menu are, for sake of symmetry it is kept here now
  There would certainly be additional algorithms
  """
  from TriggerMenuMT.HLTMenuConfig.Menu.MenuComponents import InViewReco
  reco = InViewReco( viewMakerName )
  algAcc = TrigInDetConfig( flags, roisKey=reco.inputMaker().InViewRoIs, signatureName=signature )

  reco.mergeReco( algAcc )
  return reco


if __name__ == "__main__":
    from AthenaCommon.Configurable import Configurable
    Configurable.configurableRun3Behavior=1

    from AthenaConfiguration.AllConfigFlags import ConfigFlags
    ConfigFlags.Input.Files = ["/cvmfs/atlas-nightlies.cern.ch/repo/data/data-art/TrigP1Test/data17_13TeV.00327265.physics_EnhancedBias.merge.RAW._lb0100._SFO-1._0001.1"]
    ConfigFlags.Trigger.LVL1ConfigFile = "LVL1config_Physics_pp_v7.xml"
    #ConfigFlags.IOVDb.GlobalTag = "CONDBR2-BLKPA-2018-13"
    ConfigFlags.lock()

    acc = ComponentAccumulator()

    nThreads=1

    SG__HiveMgrSvc=CompFactory.SG.HiveMgrSvc
    eventDataSvc = SG__HiveMgrSvc("EventDataSvc")
    eventDataSvc.NSlots = nThreads
    acc.addService( eventDataSvc )
    #from AthenaConfiguration.MainServicesConfig import MainServicesCfg
    #acc.merge( MainServicesCfg( ConfigFlags ) )
    from L1Decoder.L1DecoderConfig import L1DecoderCfg
    l1DecoderAcc = L1DecoderCfg( ConfigFlags )
    acc.merge(l1DecoderAcc)
    from ByteStreamCnvSvc.ByteStreamConfig import ByteStreamReadCfg
    acc.merge(ByteStreamReadCfg(ConfigFlags))

    acc.merge( TrigInDetConfig( ConfigFlags ) )
    from RegionSelector.RegSelConfig import regSelCfg
    rsc = regSelCfg( ConfigFlags )
    acc.merge( rsc )

    acc.printConfig()
    acc.store( open("test.pkl", "wb") )
    print('All ok')
