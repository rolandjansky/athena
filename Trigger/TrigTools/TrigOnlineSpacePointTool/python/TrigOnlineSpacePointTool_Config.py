# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

from TrigOnlineSpacePointTool.TrigOnlineSpacePointToolConf import OnlineSpacePointProviderTool
from TrigOnlineSpacePointTool.TrigOnlineSpacePointToolConf import PixelClusterCacheTool
from TrigOnlineSpacePointTool.TrigOnlineSpacePointToolConf import SCT_ClusterCacheTool

from SiClusterizationTool.SiClusterizationToolConf import InDet__MergedPixelsTool
from SiClusterizationTool.SiClusterizationToolConf import InDet__SCT_ClusteringTool
from SiClusterizationTool.SiClusterizationToolConf import InDet__ClusterMakerTool


class ConfiguredOnlineSpacePointProviderTool(OnlineSpacePointProviderTool) :
    __slots__ = []
    def __init__(self, name = "ConfiguredOnlineSpacePointProviderTool", doBS = True) :
        OnlineSpacePointProviderTool.__init__(self, name)

        self.UseStoreGate = False
        self.DoBS_Conversion = doBS
    
        from AthenaCommon.AppMgr import ToolSvc
        
        from InDetTrigRecExample.InDetTrigConditionsAccess import PixelConditionsSetup
        from InDetTrigRecExample.InDetTrigFlags import InDetTrigFlags
        
        InDetL2TrigClusterMakerTool = InDet__ClusterMakerTool( name = "InDetL2TrigClusterMakerTool",
                                                               UsePixelCalibCondDB = False,
                                                               PixelOfflineCalibSvc =  PixelConditionsSetup.instanceName('PixelOfflineCalibSvc'),
                                                               )

        ToolSvc += InDetL2TrigClusterMakerTool

        InDetL2TrigMergedPixelsTool = InDet__MergedPixelsTool( name = "InDetL2TrigMergedPixelsTool",
                                                               globalPosAlg  = InDetL2TrigClusterMakerTool,
                                                               UseSpecialPixelMap = False,
                                                               PixelConditionsSummarySvc = PixelConditionsSetup.instanceName('PixelConditionsSummarySvc'),
                                                             )

        from InDetTrigRecExample.InDetTrigConditionsAccess import SCT_ConditionsSetup
        InDetL2TrigSCT_ClusteringTool = InDet__SCT_ClusteringTool(name = "InDetL2TrigSCT_ClusteringTool",
                                                                  globalPosAlg  = InDetL2TrigClusterMakerTool,
                                                                  conditionsService = SCT_ConditionsSetup.instanceName("InDetSCT_ConditionsSummarySvc"),
                                                                )

        if InDetTrigFlags.doSCTIntimeHits():
          if InDetTrigFlags.InDet25nsec():
            InDetL2TrigSCT_ClusteringTool.timeBins = "01X"
          else:
            InDetL2TrigSCT_ClusteringTool.timeBins = "X1X"

        ToolSvc += InDetL2TrigMergedPixelsTool
        ToolSvc += InDetL2TrigSCT_ClusteringTool

        ConfiguredPixelClusterCacheTool = PixelClusterCacheTool(PixelClusteringTool=InDetL2TrigMergedPixelsTool)
        ConfiguredSCT_ClusterCacheTool = SCT_ClusterCacheTool(SCT_ClusteringTool = InDetL2TrigSCT_ClusteringTool,
                                                              SCT_ByteStreamErrorsSvc= SCT_ConditionsSetup.instanceName("InDetSCT_ByteStreamErrorsSvc"),
                                                              )

        if not self.DoBS_Conversion :
            ConfiguredPixelClusterCacheTool.RDO_ContainerName = "PixelRDOs"
            ConfiguredSCT_ClusterCacheTool.RDO_ContainerName = "SCT_RDOs"
            ConfiguredPixelClusterCacheTool.DoBS_Conversion = False
            ConfiguredSCT_ClusterCacheTool.DoBS_Conversion = False
        

        ToolSvc += ConfiguredPixelClusterCacheTool
        ToolSvc += ConfiguredSCT_ClusterCacheTool
        
        self.PixelClusterCacheTool = ConfiguredPixelClusterCacheTool
        self.SctClusterCacheTool = ConfiguredSCT_ClusterCacheTool
        
