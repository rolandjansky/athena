# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

from TrigOnlineSpacePointTool.TrigOnlineSpacePointToolConf import OnlineSpacePointProviderTool
from TrigOnlineSpacePointTool.TrigOnlineSpacePointToolConf import PixelClusterCacheTool
from TrigOnlineSpacePointTool.TrigOnlineSpacePointToolConf import SCT_ClusterCacheTool
from TrigOnlineSpacePointTool.TrigOnlineSpacePointToolConf import FastSCT_RodDecoder

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

        # --- SiLorentzAngleTool
        if not hasattr(ToolSvc, "PixelLorentzAngleTool"):
            from SiLorentzAngleSvc.PixelLorentzAngleToolSetup import PixelLorentzAngleToolSetup
            pixelLorentzAngleToolSetup = PixelLorentzAngleToolSetup()

        if not hasattr(ToolSvc, "SCTLorentzAngleTool"):
            from SiLorentzAngleSvc.SCTLorentzAngleToolSetup import SCTLorentzAngleToolSetup
            sctLorentzAngleToolSetup = SCTLorentzAngleToolSetup()
        
        InDetL2TrigClusterMakerTool = InDet__ClusterMakerTool( name = "InDetL2TrigClusterMakerTool",
                                                               UsePixelCalibCondDB = False,
                                                               PixelOfflineCalibSvc =  PixelConditionsSetup.instanceName('PixelOfflineCalibSvc'),
                                                               PixelLorentzAngleTool = ToolSvc.PixelLorentzAngleTool,
                                                               SCTLorentzAngleTool = ToolSvc.SCTLorentzAngleTool
                                                               )

        ToolSvc += InDetL2TrigClusterMakerTool
        from PixelConditionsTools.PixelConditionsToolsConf import PixelConditionsSummaryTool
        InDetL2TrigMergedPixelsTool = InDet__MergedPixelsTool( name = "InDetL2TrigMergedPixelsTool",
                                                               globalPosAlg  = InDetL2TrigClusterMakerTool,
                                                               UseSpecialPixelMap = False,
                                                               PixelConditionsSummaryTool = PixelConditionsSummaryTool(PixelConditionsSetup.instanceName('PixelConditionsSummaryTool'))
                                                             )

        from SCT_ConditionsTools.SCT_ConditionsToolsConf import SCT_ConditionsSummaryTool
        from InDetTrigRecExample.InDetTrigConditionsAccess import SCT_ConditionsSetup
        InDetL2TrigSCT_ClusteringTool = InDet__SCT_ClusteringTool(name = "InDetL2TrigSCT_ClusteringTool",
                                                                  globalPosAlg  = InDetL2TrigClusterMakerTool,
                                                                  conditionsTool = SCT_ConditionsSummaryTool(SCT_ConditionsSetup.instanceName('InDetSCT_ConditionsSummaryTool'))
                                                                )

        if InDetTrigFlags.doSCTIntimeHits():
          if InDetTrigFlags.InDet25nsec():
            InDetL2TrigSCT_ClusteringTool.timeBins = "01X"
          else:
            InDetL2TrigSCT_ClusteringTool.timeBins = "X1X"

        ToolSvc += InDetL2TrigMergedPixelsTool
        ToolSvc += InDetL2TrigSCT_ClusteringTool

        # SiLorentzAngleTool for FastSCT_RodDecoder
        if not hasattr(ToolSvc, "SCTLorentzAngleTool"):
            from SiLorentzAngleSvc.SCTLorentzAngleToolSetup import SCTLorentzAngleToolSetup
            sctLorentzAngleToolSetup = SCTLorentzAngleToolSetup() 

        # FastSCT_RodDecoder
        ToolSvc += FastSCT_RodDecoder(name = "FastSCT_RodDecoder",
                                      LorentzAngleTool=ToolSvc.SCTLorentzAngleTool)

        ConfiguredPixelClusterCacheTool = PixelClusterCacheTool(PixelClusteringTool=InDetL2TrigMergedPixelsTool,
                                                                LorentzAngleTool = ToolSvc.PixelLorentzAngleTool)
        ConfiguredSCT_ClusterCacheTool = SCT_ClusterCacheTool(SCT_ClusteringTool = InDetL2TrigSCT_ClusteringTool,
                                                              FastSCT_RodDecoder = ToolSvc.FastSCT_RodDecoder
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
        
