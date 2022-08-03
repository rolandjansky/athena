#
#  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
#

from AthenaConfiguration.ComponentAccumulator import ComponentAccumulator
from AthenaConfiguration.ComponentFactory import CompFactory
from AthenaConfiguration.Enums import Format
from InDetRecExample.InDetKeys import InDetKeys

def SiTrackMaker_xkCfg(flags, name="SiTrackMaker_xk"):
  """
  based on: InnerDetector/InDetExample/InDetTrigRecExample/python/InDetTrigConfigRecNewTracking.py , should be moved elsewhere
  """
  acc = ComponentAccumulator()

  from InDetConfig.SiDetElementsRoadToolConfig import SiDetElementsRoadMaker_xkCfg
  roadTool = acc.popToolsAndMerge( SiDetElementsRoadMaker_xkCfg( flags, name="InDetTrigSiDetElementsRoadMaker" ) )
  from InDetConfig.SiCombinatorialTrackFinderToolConfig import SiCombinatorialTrackFinder_xk_Trig_Cfg
  combTrackFinderTool = acc.popToolsAndMerge( SiCombinatorialTrackFinder_xk_Trig_Cfg( flags ) )

  from TrkConfig.TrkRIO_OnTrackCreatorConfig import RIO_OnTrackErrorScalingCondAlgCfg
  acc.merge(RIO_OnTrackErrorScalingCondAlgCfg(flags))

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

def InDetIDCCacheCreatorCfg(flags):
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
  if not flags.Detector.GeometryTRT:
    InDetCacheCreatorTrig.disableTRT = True

  acc.addEventAlgo( InDetCacheCreatorTrig )
  return acc


def geoModelCfg(flags):
  acc = ComponentAccumulator()
  from BeamPipeGeoModel.BeamPipeGMConfig import BeamPipeGeometryCfg
  acc.merge( BeamPipeGeometryCfg( flags ) )
  from InDetConfig.InDetGeometryConfig import InDetGeometryCfg
  acc.merge( InDetGeometryCfg( flags ) )

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
  from MagFieldServices.MagFieldServicesConfig import AtlasFieldCacheCondAlgCfg
  acc.merge( AtlasFieldCacheCondAlgCfg(flags) )

  return acc


def pixelDataPrepCfg(flags, roisKey, signature):
  from PixelReadoutGeometry.PixelReadoutGeometryConfig import PixelReadoutManagerCfg
  acc = PixelReadoutManagerCfg(flags)

  from RegionSelector.RegSelToolConfig import regSelTool_Pixel_Cfg
  RegSelTool_Pixel = acc.popToolsAndMerge(regSelTool_Pixel_Cfg(flags))

  from PixelConditionsAlgorithms.PixelConditionsConfig import PixelCablingCondAlgCfg
  acc.merge(PixelCablingCondAlgCfg(flags)) # To produce PixelCablingCondData for PixelRodDecoder + PixelRawDataProvider

  if flags.Input.Format is Format.BS:
    from PixelConditionsAlgorithms.PixelConditionsConfig import PixelHitDiscCnfgAlgCfg
    acc.merge(PixelHitDiscCnfgAlgCfg(flags)) # To produce PixelHitDiscCnfgData for PixelRodDecoder
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

def ftfCfg(flags, roisKey, signature, signatureName):
  acc = ComponentAccumulator()

  from TrkConfig.TrkTrackSummaryToolConfig import InDetTrigTrackSummaryToolCfg
  TrackSummaryTool = acc.popToolsAndMerge( InDetTrigTrackSummaryToolCfg(flags, name="InDetTrigFastTrackSummaryTool") )
  acc.addPublicTool(TrackSummaryTool)

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

  from InDetConfig.SiCombinatorialTrackFinderToolConfig import SiDetElementBoundaryLinksCondAlg_xk_Pixel_Cfg, SiDetElementBoundaryLinksCondAlg_xk_SCT_Cfg
  acc.merge(SiDetElementBoundaryLinksCondAlg_xk_Pixel_Cfg(flags))
  acc.merge(SiDetElementBoundaryLinksCondAlg_xk_SCT_Cfg(flags))

  ftf = CompFactory.TrigFastTrackFinder( name = "TrigFastTrackFinder_" + signature,
                                         LayerNumberTool          = acc.getPublicTool( "TrigL2LayerNumberTool_FTF" ),
                                         SpacePointProviderTool   = acc.getPublicTool( "TrigSpacePointConversionTool" + signature ),
                                         TrackSummaryTool         = TrackSummaryTool,
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

  from TrkConfig.TrkParticleCreatorConfig import InDetTrigParticleCreatorToolFTFCfg
  creatorTool = acc.popToolsAndMerge(InDetTrigParticleCreatorToolFTFCfg(flags))
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

  from InDetConfig.InDetPrepRawDataFormationConfig import TrigPixelClusterizationCfg, TrigSCTClusterizationCfg
  acc.merge(TrigPixelClusterizationCfg(flags, roisKey=roisKey, signature=signature))
  acc.merge(TrigSCTClusterizationCfg(flags, roisKey=roisKey, signature=signature))
  from InDetConfig.SiSpacePointFormationConfig import TrigSiTrackerSpacePointFinderCfg
  acc.merge(TrigSiTrackerSpacePointFinderCfg(flags, name="InDetSiTrackerSpacePointFinder_"+signature))
  acc.merge(ftfCfg(flags, roisKey, signature, signatureName))
  acc.merge(trackFTFConverterCfg(flags, signature))
  return acc

############################################################################################################################
# precision tracking
############################################################################################################################
prefix="InDetTrigMT"

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

def _tracksPostAmbi(flags):
  return flags.InDet.Tracking.ActivePass.trkTracks_IDTrig+"_Amb"


def TRTExtensionToolCfg(flags):
  acc = ComponentAccumulator()

  from TrkConfig.TrkExRungeKuttaPropagatorConfig import RungeKuttaPropagatorCfg
  patternPropagator = acc.popToolsAndMerge(RungeKuttaPropagatorCfg(flags, name="InDetTrigPatternPropagator"))
  acc.addPublicTool(patternPropagator)

  from TrkConfig.TrkMeasurementUpdatorConfig import KalmanUpdator_xkCfg
  updator = acc.popToolsAndMerge(KalmanUpdator_xkCfg(flags, name="InDetTrigPatternUpdator"))
  acc.addPublicTool(updator)

  from InDetConfig.TRT_DetElementsRoadToolConfig import TRT_DetElementsRoadMaker_xkCfg
  roadMaker = acc.popToolsAndMerge(TRT_DetElementsRoadMaker_xkCfg(flags))

  from InDetConfig.InDetTrackSelectorToolConfig import InDetTrigTRTDriftCircleCutToolCfg
  driftCircleCutTool = acc.popToolsAndMerge(InDetTrigTRTDriftCircleCutToolCfg(flags))
  acc.addPublicTool(driftCircleCutTool)

  from .InDetTrigCollectionKeys import TrigTRTKeys
  extensionTool = CompFactory.InDet.TRT_TrackExtensionTool_xk ( name = f"{prefix}TrackExtensionTool_{flags.InDet.Tracking.ActivePass.name}",
                                                                TRT_ClustersContainer = TrigTRTKeys.DriftCircles,
                                                                PropagatorTool = patternPropagator,
                                                                UpdatorTool    = updator,
                                                                RoadTool       = roadMaker,
                                                                DriftCircleCutTool = driftCircleCutTool,
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

  from TrkConfig.TrkTrackSummaryToolConfig import InDetTrigTrackSummaryToolCfg
  from TrkConfig.TrkGlobalChi2FitterConfig import InDetTrigGlobalChi2FitterCfg
  TrackFitter = acc.popToolsAndMerge(InDetTrigGlobalChi2FitterCfg(flags))
  acc.addPublicTool(TrackFitter)

  from InDetConfig.InDetTrackScoringToolsConfig import InDetTrigAmbiScoringToolCfg
  ScoringTool = acc.popToolsAndMerge(InDetTrigAmbiScoringToolCfg(flags))
  acc.addPublicTool(ScoringTool)

  extensionProcessor = CompFactory.InDet.InDetExtensionProcessor (name         = f"{prefix}ExtensionProcessor_{flags.InDet.Tracking.ActivePass.name}",
                                                            TrackName          = _tracksPostAmbi(flags),
                                                            ExtensionMap       = 'ExtendedTrackMap',
                                                            NewTrackName       = flags.InDet.Tracking.ActivePass.trkTracks_IDTrig,
                                                            TrackFitter        = TrackFitter,
                                                            TrackSummaryTool   = acc.popToolsAndMerge(InDetTrigTrackSummaryToolCfg(flags)),
                                                            ScoringTool        = ScoringTool,
                                                            suppressHoleSearch = False,
                                                            RefitPrds = not flags.InDet.Tracking.ActivePass.refitROT )

  #TODO trigger uses only one type of extension, optional tools can be removed in future

  acc.addEventAlgo( extensionProcessor )
  return acc


def TRTExtrensionBuilderCfg(flags):
  acc = ComponentAccumulator()
  if flags.Input.Format is Format.BS:
    acc.merge( TRTDataProviderCfg(flags) )

  from InDetConfig.InDetPrepRawDataFormationConfig import TrigTRTRIOMakerCfg
  acc.merge( TrigTRTRIOMakerCfg(flags) )

  acc.merge( TRTExtensionAlgCfg(flags) )
  acc.merge( TRTExtensionProcessorCfg(flags) )
#  'TRTRawDataProvider/InDetTrigMTTRTRawDataProvider_electronLRT', 
#  'InDet::TRT_RIO_Maker/InDetTrigMTTRTDriftCircleMaker_electronLRT', 
#  'InDet::TRT_TrackExtensionAlg/InDetTrigMTTrackExtensionAlg_electronLRT', 
#  'InDet::InDetExtensionProcessor/InDetTrigMTExtensionProcessor_electronLRT', 


  return acc

def ambiguitySolverAlgCfg(flags):
  acc = ComponentAccumulator()

  from TrkConfig.TrkAmbiguitySolverConfig import TrkAmbiguityScore_Trig_Cfg, TrkAmbiguitySolver_Trig_Cfg
  acc.merge(TrkAmbiguityScore_Trig_Cfg(flags, name = f"{prefix}TrkAmbiguityScore_{flags.InDet.Tracking.ActivePass.input_name}"))
  acc.merge(TrkAmbiguitySolver_Trig_Cfg(flags, name  = f"{prefix}TrkAmbiguitySolver_{flags.InDet.Tracking.ActivePass.input_name}"))

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
