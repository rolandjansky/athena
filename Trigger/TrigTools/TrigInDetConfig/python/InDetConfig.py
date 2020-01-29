#
#  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
#

from AthenaConfiguration.ComponentAccumulator import ComponentAccumulator
from AthenaConfiguration.ComponentFactory import CompFactory

class InDetCacheNames(object):
  Pixel_ClusterKey   = "PixelTrigClustersCache"
  SCT_ClusterKey     = "SCT_ClustersCache"
  SpacePointCachePix = "PixelSpacePointCache"
  SpacePointCacheSCT = "SctSpacePointCache"
  SCTRDOCacheKey     = "SctRDOCache"
  PixRDOCacheKey     = "PixRDOCache"

def InDetIDCCacheCreatorCfg():
  #Create IdentifiableCaches
  acc = ComponentAccumulator()
  InDet__CacheCreator=CompFactory.InDet__CacheCreator
  InDetCacheCreatorTrig = InDet__CacheCreator(name = "InDetCacheCreatorTrig",
                                              Pixel_ClusterKey   = InDetCacheNames.Pixel_ClusterKey,
                                              SCT_ClusterKey     = InDetCacheNames.SCT_ClusterKey,
                                              SpacePointCachePix = InDetCacheNames.SpacePointCachePix,
                                              SpacePointCacheSCT = InDetCacheNames.SpacePointCacheSCT,
                                              SCTRDOCacheKey     = InDetCacheNames.SCTRDOCacheKey,
                                              PixRDOCacheKey     = InDetCacheNames.PixRDOCacheKey)

  acc.addEventAlgo( InDetCacheCreatorTrig )
  return acc

#Set up conditions algorithms
def TrigInDetCondConfig( flags ):

  acc = ComponentAccumulator()
  from AtlasGeoModel.InDetGMConfig import InDetGeometryCfg
  acc.merge(InDetGeometryCfg(flags))
  #acc.merge(InDetGMConfig(flags))

  from IOVDbSvc.IOVDbSvcConfig import addFoldersSplitOnline, addFolders
  acc.merge(addFoldersSplitOnline(flags, "INDET","/Indet/Onl/AlignL1/ID","/Indet/AlignL1/ID",className="CondAttrListCollection"))
  acc.merge(addFoldersSplitOnline(flags, "INDET","/Indet/Onl/AlignL2/PIX","/Indet/AlignL2/PIX",className="CondAttrListCollection"))
  acc.merge(addFoldersSplitOnline(flags, "INDET","/Indet/Onl/AlignL2/SCT","/Indet/AlignL2/SCT",className="CondAttrListCollection"))
  acc.merge(addFoldersSplitOnline(flags, "INDET","/Indet/Onl/AlignL3","/Indet/AlignL3",className="AlignableTransformContainer"))
  acc.merge(addFoldersSplitOnline(flags, "INDET","/Indet/Onl/IBLDist","/Indet/IBLDist",className="CondAttrListCollection"))

  SCT_DCSConditionsTool=CompFactory.SCT_DCSConditionsTool
  dcsTool = SCT_DCSConditionsTool(ReadAllDBFolders = True, ReturnHVTemp = True)

  from SCT_ConditionsTools.SCT_SiliconConditionsToolSetup import SCT_SiliconConditionsToolSetup
  sct_SiliconConditionsToolSetup = SCT_SiliconConditionsToolSetup()
  sct_SiliconConditionsToolSetup.setDcsTool(dcsTool)
  sct_SiliconConditionsToolSetup.setToolName("InDetSCT_SiliconConditionsTool")
  sct_SiliconConditionsToolSetup.setup()

  sctSiliconConditionsTool = sct_SiliconConditionsToolSetup.getTool()
  sctSiliconConditionsTool.CheckGeoModel = False
  sctSiliconConditionsTool.ForceUseGeoModel = False

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

  SCT_DCSConditionsTempCondAlg=CompFactory.SCT_DCSConditionsTempCondAlg
  acc.addCondAlgo(SCT_DCSConditionsTempCondAlg( ReadKey = tempFolder ))
  SCT_DCSConditionsStatCondAlg=CompFactory.SCT_DCSConditionsStatCondAlg
  acc.addCondAlgo(SCT_DCSConditionsStatCondAlg(ReturnHVTemp = True,
                                               ReadKeyHV = hvFolder,
                                               ReadKeyState = stateFolder))
  SCT_DCSConditionsHVCondAlg=CompFactory.SCT_DCSConditionsHVCondAlg
  acc.addCondAlgo(SCT_DCSConditionsHVCondAlg(ReadKey = hvFolder))

  SCT_SiliconHVCondAlg=CompFactory.SCT_SiliconHVCondAlg
  acc.addCondAlgo(SCT_SiliconHVCondAlg(UseState = dcsTool.ReadAllDBFolders,
                                       DCSConditionsTool = dcsTool))
  SCT_SiliconTempCondAlg=CompFactory.SCT_SiliconTempCondAlg
  acc.addCondAlgo(SCT_SiliconTempCondAlg(UseState = dcsTool.ReadAllDBFolders, DCSConditionsTool = dcsTool))


  SCTSiLorentzAngleCondAlg=CompFactory.SCTSiLorentzAngleCondAlg
  acc.addCondAlgo(SCTSiLorentzAngleCondAlg(name = "SCTSiLorentzAngleCondAlg",
                                           SiConditionsTool = sctSiliconConditionsTool,
                                           UseMagFieldSvc = True,
                                           UseMagFieldDcs = False))
  SiLorentzAngleTool=CompFactory.SiLorentzAngleTool
  SCTLorentzAngleTool = SiLorentzAngleTool(name = "SCTLorentzAngleTool", DetectorName="SCT", SiLorentzAngleCondData="SCTSiLorentzAngleCondData")
  SCTLorentzAngleTool.UseMagFieldSvc = True #may need also MagFieldSvc instance
  acc.addPublicTool(SCTLorentzAngleTool)


  acc.merge(addFoldersSplitOnline(flags, "INDET", "/Indet/Onl/Beampos", "/Indet/Beampos", className="AthenaAttributeList"))
  acc.merge(addFolders(flags, "/TRT/Onl/ROD/Compress","TRT_ONL", className='CondAttrListCollection'))
  acc.merge(addFoldersSplitOnline(flags, "TRT","/TRT/Onl/Calib/RT","/TRT/Calib/RT",className="TRTCond::RtRelationMultChanContainer"))
  acc.merge(addFoldersSplitOnline(flags, "TRT","/TRT/Onl/Calib/T0","/TRT/Calib/T0",className="TRTCond::StrawT0MultChanContainer"))
  acc.merge(addFoldersSplitOnline (flags, "TRT","/TRT/Onl/Calib/errors","/TRT/Calib/errors",className="TRTCond::RtRelationMultChanContainer"))
  acc.merge(addFoldersSplitOnline(flags, "TRT","/TRT/Onl/Calib/ToTCalib","/TRT/Calib/ToTCalib",className="CondAttrListCollection"))
  acc.merge(addFoldersSplitOnline(flags, "TRT","/TRT/Onl/Calib/HTCalib","/TRT/Calib/HTCalib",className="CondAttrListCollection"))


  ###############
  # Pixel setup #
  ###############
  from PixelConditionsAlgorithms.PixelConditionsConfig import (
      PixelConfigCondAlgCfg, PixelChargeCalibCondAlgCfg, PixelDCSCondHVAlgCfg,
      PixelDCSCondTempAlgCfg, PixelAlignCondAlgCfg, PixelDetectorElementCondAlgCfg,
      PixelHitDiscCnfgAlgCfg, PixelReadoutSpeedAlgCfg, PixelCablingCondAlgCfg,
      PixelDCSCondStateAlgCfg, PixelDCSCondStatusAlgCfg, PixelTDAQCondAlgCfg,
      PixelDistortionAlgCfg, PixelOfflineCalibCondAlgCfg
# NEW FOR RUN3    PixelDeadMapCondAlgCfg
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
  acc.merge(PixelTDAQCondAlgCfg(flags))
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
  acc.merge(TrigInDetCondConfig(flags))

  from InDetRecExample.InDetKeys import InDetKeys

  #Only add raw data decoders if we're running over raw data
  isMC = flags.Input.isMC
  if not isMC:
    #Pixel

    PixelRodDecoder=CompFactory.PixelRodDecoder
    InDetPixelRodDecoder = PixelRodDecoder(name = "InDetPixelRodDecoder"+ signature)
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

    acc.addEventAlgo(InDetSCTRawDataProvider)

    # load the SCTEventFlagWriter
    SCTEventFlagWriter=CompFactory.SCTEventFlagWriter
    InDetSCTEventFlagWriter = SCTEventFlagWriter(name = "InDetSCTEventFlagWriter"+ signature)

    acc.addEventAlgo(InDetSCTEventFlagWriter)


    #TRT
    TRT_CalDbSvc=CompFactory.TRT_CalDbSvc
    InDetTRTCalDbSvc = TRT_CalDbSvc()
    acc.addService(InDetTRTCalDbSvc)

    TRT_StrawStatusSummarySvc=CompFactory.TRT_StrawStatusSummarySvc
    InDetTRTStrawStatusSummarySvc = TRT_StrawStatusSummarySvc(name = "InDetTRTStrawStatusSummarySvc"+ signature)
    acc.addService(InDetTRTStrawStatusSummarySvc)

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
  InDet__ClusterMakerTool=CompFactory.InDet__ClusterMakerTool
  InDetClusterMakerTool = InDet__ClusterMakerTool(name                 = "InDetClusterMakerTool"+ signature)

  acc.addPublicTool(InDetClusterMakerTool)


  InDet__MergedPixelsTool=CompFactory.InDet__MergedPixelsTool
  InDetMergedPixelsTool = InDet__MergedPixelsTool(name                    = "InDetMergedPixelsTool"+ signature,
                                                  globalPosAlg            = InDetClusterMakerTool,
                                                  MinimalSplitSize        = 0,
                                                  MaximalSplitSize        = 49,
                                                  MinimalSplitProbability = 0,
                                                  DoIBLSplitting = True,
                                                  )
  acc.addPublicTool(InDetMergedPixelsTool)

  InDet__PixelGangedAmbiguitiesFinder=CompFactory.InDet__PixelGangedAmbiguitiesFinder
  InDetPixelGangedAmbiguitiesFinder = InDet__PixelGangedAmbiguitiesFinder(name = "InDetPixelGangedAmbiguitiesFinder"+ signature)
  acc.addPublicTool(InDetPixelGangedAmbiguitiesFinder)

  InDet__PixelClusterization=CompFactory.InDet__PixelClusterization
  InDetPixelClusterization = InDet__PixelClusterization(name                    = "InDetPixelClusterization"+ signature,
                                                        clusteringTool          = InDetMergedPixelsTool,
                                                        gangedAmbiguitiesFinder = InDetPixelGangedAmbiguitiesFinder,
                                                        DataObjectName          = InDetKeys.PixelRDOs(),
                                                        AmbiguitiesMap          = 'TrigPixelClusterAmbiguitiesMap',
                                                        ClustersName            = "PixelTrigClusters",)
  InDetPixelClusterization.isRoI_Seeded = True
  InDetPixelClusterization.RoIs = roisKey
  InDetPixelClusterization.ClusterContainerCacheKey = InDetCacheNames.Pixel_ClusterKey

  acc.addEventAlgo(InDetPixelClusterization)

  from SCT_ConditionsTools.SCT_FlaggedConditionToolSetup import SCT_FlaggedConditionToolSetup
  sct_FlaggedConditionToolSetup = SCT_FlaggedConditionToolSetup()
  sct_FlaggedConditionToolSetup.setup()
  InDetSCT_FlaggedConditionTool = sct_FlaggedConditionToolSetup.getTool()

  from SCT_ConditionsTools.SCT_ConditionsSummaryToolSetup import SCT_ConditionsSummaryToolSetup
  sct_ConditionsSummaryToolSetup = SCT_ConditionsSummaryToolSetup()
  sct_ConditionsSummaryToolSetup = SCT_ConditionsSummaryToolSetup("InDetSCT_ConditionsSummaryTool"+ signature)
  sct_ConditionsSummaryToolSetup.setup()
  InDetSCT_ConditionsSummaryTool = sct_ConditionsSummaryToolSetup.getTool()
  condTools = []
  for condToolHandle in InDetSCT_ConditionsSummaryTool.ConditionsTools:
    condTool = condToolHandle.typeAndName
    if condTool not in condTools:
      if condTool != InDetSCT_FlaggedConditionTool.getFullName():
        condTools.append(condTool)
  sct_ConditionsSummaryToolSetupWithoutFlagged = SCT_ConditionsSummaryToolSetup("InDetSCT_ConditionsSummaryToolWithoutFlagged"+ signature)
  sct_ConditionsSummaryToolSetupWithoutFlagged.setup()
  InDetSCT_ConditionsSummaryToolWithoutFlagged = sct_ConditionsSummaryToolSetupWithoutFlagged.getTool()
  InDetSCT_ConditionsSummaryToolWithoutFlagged.ConditionsTools = condTools

  #
  # --- SCT_ClusteringTool (public)
  #
  InDet__SCT_ClusteringTool=CompFactory.InDet__SCT_ClusteringTool
  InDetSCT_ClusteringTool = InDet__SCT_ClusteringTool(name              = "InDetSCT_ClusteringTool"+ signature,
                                                      globalPosAlg      = InDetClusterMakerTool,
                                                      conditionsTool    = InDetSCT_ConditionsSummaryToolWithoutFlagged)
  #
  # --- SCT_Clusterization algorithm
  #

  InDet__SCT_Clusterization=CompFactory.InDet__SCT_Clusterization
  InDetSCT_Clusterization = InDet__SCT_Clusterization(name                    = "InDetSCT_Clusterization"+ signature,
                                                      clusteringTool          = InDetSCT_ClusteringTool,
                                                      # ChannelStatus         = InDetSCT_ChannelStatusAlg,
                                                      DataObjectName          = InDetKeys.SCT_RDOs(),
                                                      ClustersName            = "SCT_TrigClusters",
                                                      conditionsTool          = InDetSCT_ConditionsSummaryToolWithoutFlagged)
  InDetSCT_Clusterization.isRoI_Seeded = True
  InDetSCT_Clusterization.RoIs = roisKey
  InDetSCT_Clusterization.ClusterContainerCacheKey = InDetCacheNames.SCT_ClusterKey

  acc.addEventAlgo(InDetSCT_Clusterization)


  #Space points and FTF

  InDet__SiSpacePointMakerTool=CompFactory.InDet__SiSpacePointMakerTool
  InDetSiSpacePointMakerTool = InDet__SiSpacePointMakerTool(name = "InDetSiSpacePointMakerTool"+ signature)
  acc.addPublicTool(InDetSiSpacePointMakerTool)

  from AthenaCommon.DetFlags import DetFlags
  InDet__SiTrackerSpacePointFinder=CompFactory.InDet__SiTrackerSpacePointFinder
  InDetSiTrackerSpacePointFinder = InDet__SiTrackerSpacePointFinder(name                   = "InDetSiTrackerSpacePointFinder"+ signature,
                                                                    SiSpacePointMakerTool  = InDetSiSpacePointMakerTool,
                                                                    PixelsClustersName     = "PixelTrigClusters",
                                                                    SCT_ClustersName       = "SCT_TrigClusters",
                                                                    SpacePointsPixelName   = "PixelTrigSpacePoints",
                                                                    SpacePointsSCTName     = "SCT_TrigSpacePoints",
                                                                    SpacePointsOverlapName = InDetKeys.OverlapSpacePoints(),
                                                                    ProcessPixels          = DetFlags.haveRIO.pixel_on(),
                                                                    ProcessSCTs            = DetFlags.haveRIO.SCT_on(),
                                                                    ProcessOverlaps        = DetFlags.haveRIO.SCT_on(),
                                                                    SpacePointCacheSCT = InDetCacheNames.SpacePointCacheSCT,
                                                                    SpacePointCachePix = InDetCacheNames.SpacePointCachePix,)

  acc.addEventAlgo(InDetSiTrackerSpacePointFinder)


  #CondSvc=CompFactory.CondSvc
  #acc.addService(CondSvc())


  #from TrigInDetConf.TrigInDetRecCommonTools import InDetTrigFastTrackSummaryTool
  #from TrigInDetConf.TrigInDetPostTools import  InDetTrigParticleCreatorToolFTF

  #InDet__TrigTrackingxAODCnvMT=CompFactory.InDet__TrigTrackingxAODCnvMT
  #theTrackParticleCreatorAlg = InDet__TrigTrackingxAODCnvMT(name = "InDetTrigTrackParticleCreatorAlg",
  #                                                         doIBLresidual = False,
  #                                                         TrackName = "TrigFastTrackFinder_Tracks",
  #                                                         TrackParticlesName = "xAODTracks",
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

    SG__HiveMgrSvc=CompFactory.SG__HiveMgrSvc
    eventDataSvc = SG__HiveMgrSvc("EventDataSvc")
    eventDataSvc.NSlots = nThreads
    acc.addService( eventDataSvc )
    #from AthenaConfiguration.MainServicesConfig import MainServicesThreadedCfg
    #acc.merge( MainServicesThreadedCfg( ConfigFlags ) )
    from L1Decoder.L1DecoderConfig import L1DecoderCfg
    l1DecoderAcc, l1DecoderAlg = L1DecoderCfg( ConfigFlags )
    acc.addEventAlgo(l1DecoderAlg)
    acc.merge(l1DecoderAcc)
    from ByteStreamCnvSvc.ByteStreamConfig import TrigBSReadCfg
    acc.merge(TrigBSReadCfg(ConfigFlags))

    acc.merge( TrigInDetConfig( ConfigFlags ) )
    from RegionSelector.RegSelConfig import regSelCfg
    rsc = regSelCfg( ConfigFlags )
    acc.merge( rsc )

    acc.printConfig()
    acc.store( open("test.pkl", "wb") )
    print('All ok')
