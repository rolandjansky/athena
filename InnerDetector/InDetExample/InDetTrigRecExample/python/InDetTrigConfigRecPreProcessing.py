# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

from __future__ import print_function

# ------------------------------------------------------------
#
# ----------- Data-Preparation stage
#
# ------------------------------------------------------------
#
# ----------- PrepRawData creation from Raw Data Objects
#
# common things

#EF_PixRDOKey="PixelRDOs"
#EF_SCTRDOKey="SCT_RDOs"
#EF_TRTRDOKey="TRT_RDOs"


EF_PixRDOKey="PixelRDOs_EF"
EF_SCTRDOKey="SCT_RDOs_EF"
EF_TRTRDOKey="TRT_RDOs_TRIG"
from TriggerJobOpts.TriggerFlags import TriggerFlags
from AthenaCommon.GlobalFlags import globalflags
if globalflags.InputFormat == 'pool' and not TriggerFlags.doTransientByteStream():
   EF_PixRDOKey="PixelRDOs"



from AthenaCommon.Logging import logging 
log = logging.getLogger("EFID")



# PixelClusterization algorithm
#
from InDetTrigPrepRawDataFormat.InDetTrigPrepRawDataFormatConf import InDet__Pixel_TrgClusterization
class PixelClustering_EF( InDet__Pixel_TrgClusterization ):
   __slots__ = []
   def __init__(self, name="PixelClustering_Electron_EF", type="electron"):
      super( InDet__Pixel_TrgClusterization, self ).__init__( name )
      
      from RegionSelector.RegSelToolConfig import makeRegSelTool_Pixel
      self.RegSelTool = makeRegSelTool_Pixel()
      
      from AthenaCommon.AppMgr import ToolSvc
      # configure tools used

      from PixelRawDataByteStreamCnv.PixelRawDataByteStreamCnvConf import PixelRodDecoder
      InDetTrigPixelRodDecoder = PixelRodDecoder(name = "InDetTrigPixelRodDecoder")
      #InDetTrigPixelRodDecoder.OutputLevel=1
      # Disable duplcated pixel check for data15 because duplication mechanism was used.
      from RecExConfig.RecFlags import rec
      if len(rec.projectName())>=6 and rec.projectName()[:6]=="data15":
         InDetTrigPixelRodDecoder.CheckDuplicatedPixel=False
      ToolSvc += InDetTrigPixelRodDecoder

      from PixelRawDataByteStreamCnv.PixelRawDataByteStreamCnvConf import PixelRawDataProviderTool
      InDetTrigPixelRawDataProviderTool = PixelRawDataProviderTool(name    = "InDetTrigPixelRawDataProviderTool",
                                                                   Decoder = InDetTrigPixelRodDecoder)
      ToolSvc += InDetTrigPixelRawDataProviderTool

      
      from InDetTrigRawDataProvider.InDetTrigRawDataProviderConf import InDet__TrigPixRawDataProvider

      InDetTrigPixRawDataProvider = \
          InDet__TrigPixRawDataProvider(name="TrigPixRawDataProvider_EF",
                                        RawDataProviderTool = InDetTrigPixelRawDataProviderTool,
                                        RDOKey = EF_PixRDOKey)

      from RegionSelector.RegSelToolConfig import makeRegSelTool_Pixel
      InDetTrigPixRawDataProvider.RegSelTool = makeRegSelTool_Pixel()

      ToolSvc += InDetTrigPixRawDataProvider


      # ClusterMakerTool (public), needed by Pixel and SCT Clusterization
      from InDetTrigRecExample.InDetTrigConfigRecLoadTools import InDetTrigClusterMakerTool

      from InDetTrigRecExample.InDetTrigConditionsAccess import PixelConditionsSetup

      # MergedPixelTool (public)
      from SiClusterizationTool.SiClusterizationToolConf import InDet__MergedPixelsTool
      from InDetTrigRecExample.InDetTrigConfigRecLoadTools import InDetTrigPixelConditionsSummaryTool
      InDetTrigMergedPixelsTool = InDet__MergedPixelsTool( name = "InDetTrigMergedPixelsTool",
                                                           globalPosAlg  = InDetTrigClusterMakerTool,
                                                           PixelConditionsSummaryTool = InDetTrigPixelConditionsSummaryTool,
                                                           #UsePixelModuleMap = False  #simpler setup for EFID
                                                           UsePixelModuleMap = True)
      # Enable duplcated RDO check for data15 because duplication mechanism was used.
      from RecExConfig.RecFlags import rec
      if len(rec.projectName())>=6 and rec.projectName()[:6]=="data15":
         InDetTrigMergedPixelsTool.CheckDuplicatedRDO = True

      ToolSvc += InDetTrigMergedPixelsTool

      # PixelGangedAmbiguitiesFinder tool (public)
      from SiClusterizationTool.SiClusterizationToolConf import InDet__PixelGangedAmbiguitiesFinder
      InDetTrigPixelGangedAmbiguitiesFinder = InDet__PixelGangedAmbiguitiesFinder(name = "InDetTrigPixelGangedAmbiguitiesFinder")
      ToolSvc += InDetTrigPixelGangedAmbiguitiesFinder
      
      self.RawDataProvider   = InDetTrigPixRawDataProvider
      self.clusteringTool          = InDetTrigMergedPixelsTool
      self.gangedAmbiguitiesFinder = InDetTrigPixelGangedAmbiguitiesFinder
      self.Pixel_RDOContainerName  = EF_PixRDOKey
      from AthenaCommon.GlobalFlags import globalflags
      self.skipBSDecoding = globalflags.InputFormat == 'pool' and not TriggerFlags.doTransientByteStream()
      
      from InDetTrigRecExample.InDetTrigSliceSettings import InDetTrigSliceSettings
      self.EtaHalfWidth = InDetTrigSliceSettings[('etaHalfWidth',type)]
      self.PhiHalfWidth = InDetTrigSliceSettings[('phiHalfWidth',type)]
      self.doFullScan =   InDetTrigSliceSettings[('doFullScan',type)]

      #monitoring
      from InDetTrigPrepRawDataFormat.InDetTrigPrepRawDataFormatMonitoring import PixelTrigPrepRawDataFormatValidationMonitor
      from InDetTrigPrepRawDataFormat.InDetTrigPrepRawDataFormatMonitoring import PixelTrigPrepRawDataFormatOnlineMonitor
      from TrigTimeMonitor.TrigTimeHistToolConfig import TrigTimeHistToolConfig
      pixtime = TrigTimeHistToolConfig("PixTime")
      pixtime.TimerHistLimits = [0,100]
      self.AthenaMonTools = [PixelTrigPrepRawDataFormatValidationMonitor(type=type),
                             PixelTrigPrepRawDataFormatOnlineMonitor(type=type),
                             pixtime]
   
# SCT Clusterization
#
from InDetTrigPrepRawDataFormat.InDetTrigPrepRawDataFormatConf import InDet__SCT_TrgClusterization
class SCTClustering_EF( InDet__SCT_TrgClusterization ):
   __slots__ = []
   def __init__(self, name= "SCTClustering_Electron_EF", type="electron" ):
      super(InDet__SCT_TrgClusterization, self).__init__(name)

      from RegionSelector.RegSelToolConfig import makeRegSelTool_SCT
      self.RegSelTool = makeRegSelTool_SCT()

      from AthenaCommon.AppMgr import ToolSvc
      from InDetTrigRecExample.InDetTrigFlags import InDetTrigFlags


      from AthenaCommon.AppMgr import ServiceMgr as svcMgr
      from SCT_ConditionsTools.SCT_ConditionsToolsConf import SCT_ByteStreamErrorsTool
      from InDetTrigRecExample.InDetTrigConditionsAccess import SCT_ConditionsSetup
      InDetTrigBSErrorTool = SCT_ByteStreamErrorsTool(name=SCT_ConditionsSetup.instanceName("InDetSCT_ByteStreamErrorsTool"))

      from SCT_RawDataByteStreamCnv.SCT_RawDataByteStreamCnvConf import SCT_RodDecoder
      InDetTrigSCTRodDecoder = SCT_RodDecoder(name = "InDetTrigSCTRodDecoder")
      ToolSvc += InDetTrigSCTRodDecoder
      if (InDetTrigFlags.doPrintConfigurables()):
        print (     InDetTrigSCTRodDecoder)


      from SCT_RawDataByteStreamCnv.SCT_RawDataByteStreamCnvConf import SCTRawDataProviderTool
      from InDetTrigRecExample.InDetTrigConditionsAccess import SCT_ConditionsSetup
      InDetTrigSCTRawDataProviderTool = SCTRawDataProviderTool(name    = "InDetTrigSCTRawDataProviderTool",
                                                               Decoder = InDetTrigSCTRodDecoder)

      ToolSvc += InDetTrigSCTRawDataProviderTool


      from InDetTrigRawDataProvider.InDetTrigRawDataProviderConf import InDet__TrigSCTRawDataProvider
      InDetTrigSCTRawDataProvider = \
          InDet__TrigSCTRawDataProvider(name="TrigSCTRawDataProvider_EF",
                                        RDOKey = EF_SCTRDOKey,
                                        RawDataTool = InDetTrigSCTRawDataProviderTool)

      from RegionSelector.RegSelToolConfig import makeRegSelTool_SCT
      InDetTrigSCTRawDataProvider.RegSelTool = makeRegSelTool_SCT()



      ToolSvc += InDetTrigSCTRawDataProvider

      from InDetTrigRecExample.InDetTrigConfigRecLoadTools import InDetTrigClusterMakerTool

      from SCT_ConditionsTools.SCT_ConditionsToolsConf import SCT_ConditionsSummaryTool
      self.conditionsSummaryTool=SCT_ConditionsSummaryTool(SCT_ConditionsSetup.instanceName('InDetSCT_ConditionsSummaryToolWithoutFlagged'))

      # SCT_ClusteringTool (public)
      from SiClusterizationTool.SiClusterizationToolConf import InDet__SCT_ClusteringTool
      InDetTrigSCT_ClusteringTool = \
          InDet__SCT_ClusteringTool(name          = "InDetTrigSCT_ClusteringTool",
                                    globalPosAlg  = InDetTrigClusterMakerTool,
                                    conditionsTool = self.conditionsSummaryTool
                                    )
      if InDetTrigFlags.doSCTIntimeHits():
         if InDetTrigFlags.InDet25nsec():
            InDetTrigSCT_ClusteringTool.timeBins = "01X"
         else:
            InDetTrigSCT_ClusteringTool.timeBins = "X1X"

      ToolSvc += InDetTrigSCT_ClusteringTool

      self.RawDataProvider   = InDetTrigSCTRawDataProvider
      self.clusteringTool = InDetTrigSCT_ClusteringTool
      self.SCT_RDOContainerName=EF_SCTRDOKey

      from InDetTrigRecExample.InDetTrigSliceSettings import InDetTrigSliceSettings
      self.EtaHalfWidth = InDetTrigSliceSettings[('etaHalfWidth',type)]
      self.PhiHalfWidth = InDetTrigSliceSettings[('phiHalfWidth',type)]
      self.doFullScan =   InDetTrigSliceSettings[('doFullScan',type)]

      if InDetTrigFlags.cutSCTOccupancy():
         self.maxRDOs = 384   #used to be 77 but it was reached quite frequently

      #monitoring
      from InDetTrigPrepRawDataFormat.InDetTrigPrepRawDataFormatMonitoring import SctTrigPrepRawDataFormatValidationMonitor
      from InDetTrigPrepRawDataFormat.InDetTrigPrepRawDataFormatMonitoring import SctTrigPrepRawDataFormatOnlineMonitor
      from TrigTimeMonitor.TrigTimeHistToolConfig import TrigTimeHistToolConfig
      scttime = TrigTimeHistToolConfig("SctTime")
      scttime.TimerHistLimits = [0,100]
      self.AthenaMonTools = [SctTrigPrepRawDataFormatValidationMonitor(type=type),
                             SctTrigPrepRawDataFormatOnlineMonitor(type=type),
                             scttime]

            
# TRT RIO Maker
#
from InDetTrigPrepRawDataFormat.InDetTrigPrepRawDataFormatConf import InDet__TRT_TrgRIO_Maker
class TRTDriftCircleMaker_EF(  InDet__TRT_TrgRIO_Maker ):
   __slots__ = []
   def __init__(self, name= "TRTDriftCircleMaker_Electron_EF", type="electron" ):
      super(InDet__TRT_TrgRIO_Maker, self).__init__(name)

      from RegionSelector.RegSelToolConfig import makeRegSelTool_TRT
      self.RegSelTool = makeRegSelTool_TRT()
      

      from AthenaCommon.AppMgr import ToolSvc

      from InDetTrigRecExample.InDetTrigCommonTools import InDetTrigTRTRodDecoder
      from TRT_RawDataByteStreamCnv.TRT_RawDataByteStreamCnvConf import TRTRawDataProviderTool
      InDetTrigTRTRawDataProviderTool = TRTRawDataProviderTool(name="InDetTrigTRTRawDataProviderTool",
                                                               Decoder=InDetTrigTRTRodDecoder,
                                                               StoreInDetTimeCollections=False)
      ToolSvc += InDetTrigTRTRawDataProviderTool

      from InDetTrigRawDataProvider.InDetTrigRawDataProviderConf import InDet__TrigTRTRawDataProvider


      InDetTrigTRTRawDataProvider = \
          InDet__TrigTRTRawDataProvider(name="TrigTRTRawDataProvider_EF",
                                        RDOKey = EF_TRTRDOKey,
                                        RawDataTool="TRTRawDataProviderTool/InDetTrigTRTRawDataProviderTool")

      from RegionSelector.RegSelToolConfig import makeRegSelTool_TRT
      InDetTrigTRTRawDataProvider.RegSelTool = makeRegSelTool_TRT()

      ToolSvc += InDetTrigTRTRawDataProvider
      
      from InDetTrigRecExample.InDetTrigCommonTools import InDetTrigTRT_DriftFunctionTool, \
           InDetTrigTRT_DriftCircleTool

      self.RawDataProvider   = InDetTrigTRTRawDataProvider
      self.TRT_DriftCircleTool = InDetTrigTRT_DriftCircleTool
      #TRTStrawStatusTool  =

      from InDetTrigRecExample.InDetTrigSliceSettings import InDetTrigSliceSettings
      self.EtaHalfWidth = InDetTrigSliceSettings[('etaHalfWidth',type)]
      self.PhiHalfWidth = InDetTrigSliceSettings[('phiHalfWidth',type)]
      self.doFullScan =   InDetTrigSliceSettings[('doFullScan',type)]
      self.TRT_RDOContainerName = EF_TRTRDOKey

      #monitoring
      from InDetTrigPrepRawDataFormat.InDetTrigPrepRawDataFormatMonitoring import TrtTrigPrepRawDataFormatValidationMonitor
      from InDetTrigPrepRawDataFormat.InDetTrigPrepRawDataFormatMonitoring import TrtTrigPrepRawDataFormatOnlineMonitor
      from TrigTimeMonitor.TrigTimeHistToolConfig import TrigTimeHistToolConfig
      trttime = TrigTimeHistToolConfig("TRTTime")
      trttime.TimerHistLimits = [0,80]
      if self.doFullScan:
         trttime.TimerHistLimits = [0,200]
      self.AthenaMonTools = [TrtTrigPrepRawDataFormatValidationMonitor(type=type),
                             TrtTrigPrepRawDataFormatOnlineMonitor(type=type),
                             trttime]
      
#
# -------- we need to do truth association if requested
from InDetTrigTruthAlgs.InDetTrigTruthAlgsConf import InDet__PRD_TrigMultiTruthMaker
class InDetTrigPRD_MultiTruthMaker_EF( InDet__PRD_TrigMultiTruthMaker ):
   __slots__ = []
   def __init__(self, name = "InDetTrigPRD_MultiTruthMaker_Electron_EF", type="electron"):
      super (InDet__PRD_TrigMultiTruthMaker, self).__init__(name)

      from InDetTrigRecExample.InDetTrigFlags import InDetTrigFlags
      self.doTruthAss = InDetTrigFlags.doTruth()

      #monitoring
      from InDetTrigTruthAlgs.PRD_TrigMultiTruthMonitoring import PRD_TrigMultiTruthValidationMonitor
      from InDetTrigTruthAlgs.PRD_TrigMultiTruthMonitoring import PRD_TrigMultiTruthOnlineMonitor
      from TrigTimeMonitor.TrigTimeHistToolConfig import TrigTimeHistToolConfig
      prdtime = TrigTimeHistToolConfig("PRDTime")
      prdtime.TimerHistLimits = [0,200]
      self.AthenaMonTools = [PRD_TrigMultiTruthValidationMonitor(),
                             prdtime]
      
         
# ----------- form SpacePoints from clusters in SCT and Pixels
#
# SiTrackerSpacePointFinder algorithm
#
from SiTrigSpacePointFormation.SiTrigSpacePointFormationConf import InDet__SiTrigSpacePointFinder
class SiTrigSpacePointFinder_EF( InDet__SiTrigSpacePointFinder ):
   __slots__ = []
   def __init__(self, name = "SiTrigSpacePointFinder_Electron_EF", type="electron", lowPt=False):
      super (InDet__SiTrigSpacePointFinder, self).__init__(name)

      from RegionSelector.RegSelToolConfig import makeRegSelTool_Pixel
      from RegionSelector.RegSelToolConfig import makeRegSelTool_SCT
      self.RegSelTool_Pixel = makeRegSelTool_Pixel()
      self.RegSelTool_SCT   = makeRegSelTool_SCT()

      from AthenaCommon.AppMgr import ToolSvc
      
      from InDetTrigRecExample.InDetTrigConfigRecLoadTools import \
          InDetTrigSiSpacePointMakerTool,SCT_TrigSpacePointTool
      from InDetTrigRecExample.InDetTrigFlags import InDetTrigFlags

      if (InDetTrigFlags.doPrintConfigurables()):
         print (SCT_TrigSpacePointTool )
      ToolSvc +=  SCT_TrigSpacePointTool

      # Condition algorithm for SiTrigSpacePointFinder
      from AthenaCommon.AlgSequence import AthSequencer
      condSeq = AthSequencer("AthCondSeq")
      if not hasattr(condSeq, "InDetSiElementPropertiesTableCondAlg"):
         from SiSpacePointFormation.SiSpacePointFormationConf import InDet__SiElementPropertiesTableCondAlg
         condSeq += InDet__SiElementPropertiesTableCondAlg(name = "InDetSiElementPropertiesTableCondAlg")

      self.SiSpacePointMakerTool = InDetTrigSiSpacePointMakerTool
      self.SCTSpacePointTrigHelperTool = SCT_TrigSpacePointTool
      self.ProcessPixels = True
      self.ProcessSCTs = True
      self.ProcessOverlaps = InDetTrigFlags.doOverlapSP()

      if type=='cosmicsN':
         from InDetTrigRecExample.InDetTrigConfigRecLoadToolsCosmics import \
             InDetTrigSiSpacePointMakerToolCosmics, SCT_TrigSpacePointToolCosmics
         self.SiSpacePointMakerTool = InDetTrigSiSpacePointMakerToolCosmics
         self.SCTSpacePointTrigHelperTool = SCT_TrigSpacePointToolCosmics
         self.SpacePointsSCTName = "SCT_CosmicsTrigSpacePoints"
         self.SpacePointsPixelName = "PixelCosmicsTrigSpacePoints"


      from InDetTrigRecExample.InDetTrigSliceSettings import InDetTrigSliceSettings
      self.EtaHalfWidth = InDetTrigSliceSettings[('etaHalfWidth',type)]
      self.PhiHalfWidth = InDetTrigSliceSettings[('phiHalfWidth',type)]
      self.doFullScan =   InDetTrigSliceSettings[('doFullScan',type)]

      #monitoring
      from SiTrigSpacePointFormation.SiTrigSpacePointFormatMonitoring import SiTrigSpacePointFormatValidationMonitor
      from SiTrigSpacePointFormation.SiTrigSpacePointFormatMonitoring import SiTrigSpacePointFormatOnlineMonitor
      from TrigTimeMonitor.TrigTimeHistToolConfig import TrigTimeHistToolConfig
      sitime = TrigTimeHistToolConfig("SiTime")
      sitime.TimerHistLimits = [0,50]
      self.AthenaMonTools = [SiTrigSpacePointFormatValidationMonitor(type=type),
                             SiTrigSpacePointFormatOnlineMonitor(type=type),
                             sitime]
